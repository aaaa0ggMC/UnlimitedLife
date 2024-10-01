#include "update.h"

using namespace alib;

void Update::Run(){
    //(*lg)(LOG_INFO) << "Hi TPS" << ng::endlog;
    rps.wait();
}

Update::Update(Game & gm,ng::LogFactory & lgf){
    status = UL_STATUS_RUNNING;
    lg = &lgf;
    this->game = &gm;
}
