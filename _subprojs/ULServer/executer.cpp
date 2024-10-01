#include "executer.h"
#include <unordered_map>
#include <stdlib.h>

using namespace alib;

Executer::Executer(Game & g,ng::LogFactory & logger,TCPConn & t){
    this->game = &g;
    this->lg = &logger;
    tcpt = &t;
}

int Executer::RunCommand(int id,dstring head,dstring args,dsvector sep_args,dstring full){
    int ret = 0;
    if(!head.compare(""))return -1;
    if(!head.compare("hello")){
        tcpt->sendStr(id,"hi");
    }else if(!head.compare("P")){
        tcpt->sendStr(id,"P");
    }else{
        tcpt->sendStr(id,full);
    }
    return ret;
}
