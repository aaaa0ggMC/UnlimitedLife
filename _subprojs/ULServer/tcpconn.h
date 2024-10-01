#ifndef TCPCONN_H_INCLUDED
#define TCPCONN_H_INCLUDED
#include "game.h"
#include <alib/alogger.h>
#include <functional>

using namespace alib;

struct TCPConn{
    ng::LogFactory *logger;
    void * server;

    TCPConn(ng::LogFactory & logger);
    ~TCPConn();
    void setupConnListener(int port,std::function<void(int,const std::string& s)> callback);
    void sendStr(int id,const std::string& str);
};

struct ConnMaintainer{

};

struct DataTransferer{

};

#endif // TCPCONN_H_INCLUDED
