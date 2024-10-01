#include <winsock2.h>
#include "tcpconn.h"
#include "internal_server.h"
#include <thread>

using namespace alib;

void TCPConn::setupConnListener(int port,std::function<void(int,const string& s)> callback){
    (*logger)(LOG_INFO) << "Setting up connection listener..." << ng::endlog;
    std::thread th([&]{
        server = (void*)(new Server(port,4,*logger,std::chrono::seconds(0)));
        Server * pser = (Server*)(server);
        pser->onReceiveData(callback);
        pser->run();
    });
    th.detach();
}

TCPConn::~TCPConn(){
    if(server){
        delete (Server*)(server);
    }
}

TCPConn::TCPConn(ng::LogFactory & logger){
    this->logger = &logger;
    server = NULL;
}

void TCPConn::sendStr(int id,const string& str){
    Server * pser = (Server*)(server);
    pser->sendStr(id,str);
}
