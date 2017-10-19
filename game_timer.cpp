#ifndef GAME_TIMER_CPP
#define GAME_TIMER_CPP
#include "game_timer.h"

GameTimer::GameTimer()
{
    restart();
}

GameTimer::GameTimer(uint32 millisecondTime)
{
    restart(millisecondTime);
}

GameTimer::GameTimer(uint32 millisecondTime, real32 startValue, real32 endValue)
{
    restart(millisecondTime, startValue, endValue);
}

void GameTimer::restart()
{
    currentTime=0;
    isFinished=false;
}

void GameTimer::restart(uint32 millisecondTime)
{
    restart();
    maxTime = (real32)millisecondTime/MILLISECOND_PER_FRAME;
}

void GameTimer::restart(uint32 millisecondTime, real32 startValue, real32 endValue)
{
    restart(millisecondTime);
    minValue = startValue;
    maxValue = endValue;
}

real32 GameTimer::getValue(real32 t)
{
    if(!isFinished)
    {
        currentTime+=t;
        if(currentTime >= maxTime)
        {
            if(doesLoop)
            {
                currentTime = fmod(currentTime,maxTime);
            }else
            {
                currentTime = maxTime;
                isFinished=true;
            }
        }
    }

    return getInterpoleForTween(currentTime/maxTime);
}

bool GameTimer::hasFinished()
{
    return isFinished;
}

void GameTimer::setLoop(bool loop)
{
    doesLoop = loop;
}

bool GameTimer::getLoop()
{
    return doesLoop;
}

void GameTimer::setWave(bool wave)
{
    doesWave = wave;
}

bool GameTimer::getWave()
{
    return doesWave;
}

void GameTimer::setTween(TimerTweenType newTween)
{
    tween = newTween;
}

void GameTimer::setBezierTween(real32 x_1, real32 y_1, real32 x_2, real32 y_2)
{
    tween     = TWEEN_CUBICBEZIER;
    bezierX_1 = x_1;
    bezierY_1 = y_1;
    bezierX_2 = x_2;
    bezierY_2 = y_2;
}

TimerTweenType GameTimer::getTween()
{
    return tween;
}

real32 GameTimer::getInterpoleForTween(real32 t)
{
    real32 outputValue = 0;

    if(doesWave)
    {
        t*=2;
        if(t > 1)
        {
            t=2.0-t;
        }
    }

    switch(tween)
    {
        case TWEEN_LINEAR:
        {
            outputValue = t;
        }break;
        case TWEEN_QUAD_IN:
        {
            outputValue = t*t;
        }break;
        case TWEEN_QUAD_OUT:
        {
            outputValue = 1-(pow(1-t,2));
        }break;
        case TWEEN_QUAD_INOUT:
        {
            if(t < 0.5)
            {
                outputValue = pow(t*2,2)/2;
            }
            else
            {
                outputValue = 1-pow((1-t)*2,2)/2;
            }
        }
        case TWEEN_CUBIC_IN:
        {
            outputValue = t*t*t;
        }break;
        case TWEEN_CUBIC_OUT:
        {
            outputValue = 1-(pow(1-t,3));
        }break;
        case TWEEN_CUBIC_INOUT:
        {
            if(t < 0.5)
            {
                outputValue = pow(t*2,3)/2;
            }
            else
            {
                outputValue = 1-pow((1-t)*2,3)/2;
            }
        }
    }

    return minValue+outputValue*(maxValue-minValue);
}


#endif /* end of include guard: GAME_TIMER_CPP */
