#ifndef UPDATE_H_INCLUDED
#define UPDATE_H_INCLUDED
#include <string>
#include <alib/alogger.h>
#include <alib/alogger.h>
#include "game.h"

#define UL_STATUS_RUNNING 0
#define UL_STATUS_STOPPED 1
#define UL_STATUS_ERRORED 2

using namespace alib;

struct Update{
    int status;
    ng::RPSRestrict rps{20};
    ng::LogFactory *lg;
    Game * game;

    Update(Game & game,ng::LogFactory & logf);
    void Run();
};

#endif // UPDATE_H_INCLUDED
