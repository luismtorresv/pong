#include "timer.h"
#include "raylib.h"

//----------------------------------------------------------------------------------
// Ad hoc implementation of a timer from the wiki
//----------------------------------------------------------------------------------
void
StartTimer(Timer* timer, double lifetime)
{
    timer->startTime = GetTime();
    timer->lifeTime = lifetime;
}

bool
TimerDone(Timer timer)
{
    return GetElapsed(timer) >= timer.lifeTime;
}

double
GetElapsed(Timer timer)
{
    return GetTime() - timer.startTime;
}
