#ifndef GAME_TIMER_H
#define GAME_TIMER_H
#include <math.h>

#define MILLISECOND_PER_FRAME (1000.0/60.0)

enum TimerTweenType
{
    TWEEN_LINEAR,
    TWEEN_CUBICBEZIER,
    TWEEN_QUAD_IN,
    TWEEN_QUAD_OUT,
    TWEEN_QUAD_INOUT,
    TWEEN_CUBIC_IN,
    TWEEN_CUBIC_OUT,
    TWEEN_CUBIC_INOUT
};

class GameTimer
{
    public:
        GameTimer();
        GameTimer(uint32 millisecondTime);
        GameTimer(uint32 millisecondTime, real32 startValue, real32 endValue);

        void restart();
        void restart(uint32 millisecondTime);
        void restart(uint32 millisecondTime, real32 startValue, real32 endValue);

        real32 getValue(real32 t);
        bool   hasFinished();

        void   setLoop(bool loop);
        bool   getLoop();

        void   setWave(bool wave);
        bool   getWave();

        void            setTween(TimerTweenType newTween);
        void            setBezierTween(real32 x_1, real32 y_1, real32 x_2, real32 y_2);
        TimerTweenType  getTween();

    private:
        real32 getInterpoleForTween(real32 precentageThrough);

        TimerTweenType  tween       = TWEEN_LINEAR;
        real32          maxTime     = 1;
        real32          currentTime = 0;

        bool            isFinished  = false;
        bool            doesLoop    = false;
        bool            doesWave    = false;
        real32          minValue    = 0;
        real32          maxValue    = 1;

        real32 bezierX_1, bezierY_1, bezierX_2, bezierY_2;

};

#endif /* end of include guard: GAME_TIMER_H */
