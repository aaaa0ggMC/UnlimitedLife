#include <winsock2.h>
#include <iostream>
#include <asio.hpp>
#include <string>
#include <functional>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <format>
#include <chrono>
#include <unordered_map>
#include <alib/alogger.h>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include "timer.h"

using namespace asio::ip;
using namespace std;
using namespace alib;

class BinBuffer {
public:
    BinBuffer() = default;

    void writeInt(int32_t value, bool big_endian = false) {
        if (big_endian) {
            value = __builtin_bswap32(value);  // 转换为大端
        }
        buffer_.insert(buffer_.end(), reinterpret_cast<const char*>(&value), reinterpret_cast<const char*>(&value) + sizeof(value));
    }

    void writeStr(const std::string& str) {
        writeInt(static_cast<int32_t>(str.size()));  // 先写入字符串长度
        buffer_.insert(buffer_.end(), str.begin(), str.end());
    }

    void readInt(int32_t& value, bool big_endian = false) {
        if (read_pos_ + sizeof(value) > buffer_.size()) {
            throw std::runtime_error("Not enough data to read an int");
        }
        std::memcpy(&value, buffer_.data() + read_pos_, sizeof(value));
        read_pos_ += sizeof(value);
        if (big_endian) {
            value = __builtin_bswap32(value);  // 转换为主机字节顺序
        }
    }

    void readStr(std::string& str) {
        int32_t length;
        readInt(length);  // 先读取字符串长度
        if (read_pos_ + length > buffer_.size()) {
            throw std::runtime_error("Not enough data to read a string");
        }
        str.assign(buffer_.data() + read_pos_, length);
        read_pos_ += length;
    }

    const std::vector<char>& data() const {
        return buffer_;
    }

    void reset() {
        read_pos_ = 0;
        buffer_.clear();
    }

private:
    std::vector<char> buffer_;
    std::size_t read_pos_ = 0;
};

class Server {
public:
    Server(unsigned short port, std::size_t thread_pool_size, ng::LogFactory& logger, std::chrono::seconds keepalive_interval)
        : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
          thread_pool_size_(thread_pool_size),
          keepalive_interval_(keepalive_interval),
          logger_(&logger) {
        start_accept();
        if (keepalive_interval.count() > 0) {
            manage_alive();  // 启动管理存活状态的定时器
        }
    }

    void run() {
        std::vector<std::thread> thread_pool;
        for (std::size_t i = 0; i < thread_pool_size_; ++i) {
            thread_pool.emplace_back([this]() {
                io_context_.run();
            });
        }
        for (auto& thread : thread_pool) {
            thread.join();
        }
    }

    void onReceiveData(std::function<void(int, const std::string&)> callback) {
        receive_callback_ = callback;
    }

    void onReceiveBinData(std::function<void(int, const BinBuffer&)> callback) {
        receive_bin_callback_ = callback;
    }

    void sendStr(int client_id, const std::string& message) {
        std::string send_message = 'S' + message;  // 添加类型标识符
        send(client_id, send_message);
    }

    void sendBin(int client_id, const BinBuffer& binBuffer) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(client_id);
        if (it != clients_.end()) {
            std::string send_data = "B";
            send_data.append(binBuffer.data().begin(),binBuffer.data().end());  // 添加类型标识符
            asio::async_write(*it->second, asio::buffer(send_data),
                [this, client_id](std::error_code ec, std::size_t) {
                    if (ec) {
                        logger_->error("Failed to send binary data to client " + std::to_string(client_id) + ": " + ec.message());
                    } else {
                        last_keepalive_[client_id] = std::chrono::steady_clock::now();  // 更新最后的 keepalive 时间
                    }
                });
        }
    }

    void send(int client_id, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(client_id);
        if (it != clients_.end()) {
            //std::cout << "Message:" << message << std::endl;
            asio::async_write(*it->second, asio::buffer(message),
                [this, client_id](std::error_code ec, std::size_t) {
                    if (ec) {
                        logger_->error("Failed to send message to client " + std::to_string(client_id) + ": " + ec.message());
                    } else {
                        last_keepalive_[client_id] = std::chrono::steady_clock::now();  // 更新最后的 keepalive 时间
                    }
                });
        }
    }

    void sendAll(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& client : clients_) {
            send(client.first, message);
        }
    }

    std::shared_ptr<asio::ip::tcp::socket> getClient(int client_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(client_id);
        if (it != clients_.end()) {
            return it->second;
        }
        return nullptr;  // 如果找不到，则返回 nullptr
    }

    template<typename Func>
    void executeForClient(int client_id, Func func) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(client_id);
        if (it != clients_.end()) {
            func(it->second);  // 执行传入的函数
        } else {
            logger_->warn("Client " + std::to_string(client_id) + " not found.");
        }
    }

private:
    void start_accept() {
        auto socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
        acceptor_.async_accept(*socket,
            [this, socket](std::error_code ec) {
                if (!ec) {
                    int client_id = next_client_id_++;
                    clients_[client_id] = socket;
                    logger_->info("Client connected: " + std::to_string(client_id));
                    start_read(socket, client_id);
                } else {
                    logger_->error("Accept error: " + ec.message());
                }
                start_accept();
            });
    }

    void start_read(std::shared_ptr<asio::ip::tcp::socket> socket, int client_id) {
        auto buffer = std::make_shared<std::array<char, 1024>>();
        socket->async_read_some(asio::buffer(*buffer),
            [this, socket, buffer, client_id](std::error_code ec, std::size_t length) {
                if (!ec) {
                    // 检查首字节以确定数据类型
                    if (length > 0) {
                        char type = buffer->data()[0];
                        if (type == 'B') {  // 如果首字节为 'B'，表示二进制数据
                            BinBuffer binBuffer;
                            binBuffer.writeStr(std::string(buffer->data() + 1, length - 1));  // 从第二字节开始解析
                            if (receive_bin_callback_) {
                                receive_bin_callback_(client_id, binBuffer);
                            }
                        } else if (type == 'S') {  // 如果首字节为 'S'，表示字符串数据
                            std::string message(buffer->data() + 1, length - 1);  // 从第二字节开始解析
                            if (receive_callback_) {
                                receive_callback_(client_id, message);
                            }
                        } else {
                            logger_->warn("Unknown data type received from client " + std::to_string(client_id));
                        }
                    }

                    last_keepalive_[client_id] = std::chrono::steady_clock::now();  // 更新最后的接收时间
                    start_read(socket, client_id);
                } else {
                    logger_->info("Client " + std::to_string(client_id) + " disconnected: " + ec.message());
                    std::lock_guard<std::mutex> lock(mutex_);
                    clients_.erase(client_id);
                    last_keepalive_.erase(client_id);
                }
            });
    }

    void manage_alive() {
        timer.StartTimer(keepalive_interval_,
        [this]() mutable {
            send_keepalive();
            check_clients();
            manage_alive();
        });
    }

    void send_keepalive() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& client : clients_) {
            int client_id = client.first;
            std::string keepalive_message = "KEEPALIVE:" + std::to_string(client_id);  // 针对每个客户端发送不同的 keepalive 信息
            send(client_id, keepalive_message);
        }
    }

    void check_clients() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        for (auto it = clients_.begin(); it != clients_.end(); ) {
            int client_id = it->first;
            if (now - last_keepalive_[client_id] >= keepalive_interval_) {
                logger_->warn("Client " + std::to_string(client_id) + " is unresponsive, kicking.");
                it->second->close();  // 主动断开连接
                it = clients_.erase(it);  // 从客户端列表中移除
                last_keepalive_.erase(client_id);  // 移除最后的 keepalive 时间
            } else {
                ++it;
            }
        }
    }

    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::size_t thread_pool_size_;
    std::chrono::seconds keepalive_interval_;
    ng::LogFactory* logger_;
    std::unordered_map<int, std::shared_ptr<asio::ip::tcp::socket>> clients_;
    std::unordered_map<int, std::chrono::steady_clock::time_point> last_keepalive_;
    std::mutex mutex_;
    std::size_t next_client_id_ = 0;
    std::function<void(int, const std::string&)> receive_callback_;
    std::function<void(int, const BinBuffer&)> receive_bin_callback_;
    Timer timer;
};
