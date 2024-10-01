#ifndef EXECUTER_H_INCLUDED
#define EXECUTER_H_INCLUDED
#include <string>
#include <vector>
#include "game.h"
#include <alib/alogger.h>
#include "tcpconn.h"

using namespace alib;

using dstring = const std::string &;
template<class T> using dvector = const std::vector<T> &;
using dsvector = const std::vector<std::string> &;

class Executer{
public:
    Executer(Game & g,ng::LogFactory & fac,TCPConn & tcp);
    int RunCommand(int id,dstring head,dstring args,dsvector sep_args,dstring full);


    TCPConn * tcpt;
    Game * game;
    ng::LogFactory * lg;
};

#endif // EXECUTER_H_INCLUDED
