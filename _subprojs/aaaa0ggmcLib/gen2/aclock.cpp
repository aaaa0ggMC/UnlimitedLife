#include <alib/aclock.h>

using namespace alib::ng;

static inline double timeGetTimeEx(LARGE_INTEGER & feq){
    LARGE_INTEGER lg;
    QueryPerformanceCounter(&lg);
    return ((double)lg.QuadPart/(double)feq.QuadPart * 1000);
}

#define _d(x) ((double)(x))
#define AssertSt _d(m_useHTimer?timeGetTimeEx(m_cpuFeq):timeGetTime())

bool Clock::static_lock = false;
LARGE_INTEGER Clock::m_cpuFeq = {0};
bool Clock::HTimerSup = false;

Clock::Clock(bool start,bool useHTimerIfSup){
    this->m_StartTime = this->m_PreTime = 0;
    this->m_pauseGained = 0;
    this->m_start = false;
    this->m_paused = false;
    ///initialize timer
    if(!static_lock){
        static_lock = true;
        HTimerSup = QueryPerformanceFrequency(&m_cpuFeq);
    }
    toggleTimer(useHTimerIfSup);
    if(start){
        this->start();
    }
}

bool Clock::isStop(){return !m_start;}

void Clock::start(){
    if(m_start)return;
    this->m_start = true;
    this->m_StartTime = AssertSt;
    clearOffset();
}

TMST0 Clock::pause(){
    if(m_paused)return {0,0};
    m_paused = true;
    TMST0 ret = now();
    m_pauseGained = ret.all;
    stop();
    return ret;
}

void Clock::resume(){
    if(!m_paused)return;
    start();
    clearOffset();
    m_paused = false;
}

TMST0 Clock::now(){
    if(!m_start)return {0,0};
    TMST0 t;
    t.all = AssertSt - this->m_StartTime + m_pauseGained;
    t.offset = AssertSt - this->m_PreTime;
    return t;
}

double Clock::getAllTime(){
    if(!m_start)return 0;
    return now().all;
}

//现在getoffset不会清零！
double Clock::getOffset(){
    if(!m_start)return 0;
    double off = now().offset;
    this->m_PreTime = AssertSt;
    return off;
}

void Clock::clearOffset(){
    this->m_PreTime = AssertSt;
}

TMST0 Clock::stop(){
    if(!m_start)return {0,0};
    TMST0 rt = now();
    this->m_StartTime = 0;
    this->m_start = false;
    if(!m_paused){
        m_pauseGained = 0;
    }
    return rt;
}

void Clock::toggleTimer(bool h){
    if(h && HTimerSup)m_useHTimer = true;
    else m_useHTimer = false;
}

Trigger::Trigger(Clock & clk,double d){
    m_clock = &clk;
    duration = d;
    rec = clk.getAllTime();
}

bool Trigger::test(){
    return (m_clock->getAllTime() - rec) >= duration;
}

void Trigger::setClock(Clock & c){m_clock = &c;}

void Trigger::reset(){
    rec = m_clock->getAllTime();
}

void Trigger::setDuration(double duration){
    this->duration = duration;
}
