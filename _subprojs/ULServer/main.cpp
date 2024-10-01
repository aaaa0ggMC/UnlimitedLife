#include "tcpconn.h"
#include "update.h"
#include "game.h"
#include "parser.h"
#include "executer.h"

using namespace alib;

#define $port 2204

int main(){
    ng::Logger logger;
    ng::LogFactory lg("ULServer",logger);

    Game game;
    Update update(game,lg);
    TCPConn tcpConn(lg);
    Parser parser;
    Executer executer(game,lg,tcpConn);

    logger.setOutputFile("log.txt");

    ///setting up server
    tcpConn.setupConnListener($port,[&](int clientId,const std::string & str){
        std::string head = "",arg = "";
        std::vector<std::string> args;
        parser.ParseCommand(str,head,arg,args);

        executer.RunCommand(clientId,head,arg,args,str);
    });

    while(update.status == UL_STATUS_RUNNING){
        update.Run();
    }
    return 0;
}
