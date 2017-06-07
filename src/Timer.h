#ifndef TIMER_H_
#define TIMER_H_

#include "GPIO.h"
#include "inner/__Timer.h"
#include <stdint.h>
#include <stdbool.h>
#include "DataStructures/src/inverted_priority_queue.h"
#include "Misc/src/definitions.h"

PUBLIC
    //void (*Periodic_Task)(void);
    // TODO: move to .c
    //int prevTimerValue, curretTimerValue, nextTimerValue;

    typedef enum Timer_t
    {
        TIMER0,
        TIMER1,
        TIMER2,
        TIMER3,
        TIMER4,
        TIMER5
    } Timer_t;

    typedef enum TIMER_DIR
    {
        TIMER_DIR_COUNT_DOWN,
        TIMER_DIR_COUNT_UP
    } TIMER_DIR;

    typedef enum TIMER_MODE
    {
        TIMER_MODE_ONE_SHOT,
        TIMER_MODE_PERIODIC
    } TIMER_MODE;

    typedef struct Timer_Driver Timer_Driver;

    void Timer_init( Timer_Driver* timer,
                     TIMER_MODE mode,
                     TIMER_DIR dir,
                     uint32_t value );
    void Timer_ISR_init(Timer_Driver* timer,
                         uint32_t value, void(*timer_handler)()
                        //TODO: priority,
                        , ISR_PRIORITY priority);
    void Timer_ISR_handler();
    void Timer_setMatcher( uint32_t matchValue );
    void Timer_start( Timer_Driver* timer );
    void Timer_reset(Timer_Driver *timer, int newDelay , bool resetMatcher);
    void Timer_stop( Timer_Driver* timer );
    bool Timer_isDone( Timer_Driver* timer );
    bool Timer_isStarted( Timer_Driver* timer );
    uint32_t Timer_getCurrentTicks( Timer_Driver *timer );
    uint32_t Timer_toTicks( uint32_t millisecond );
    uint32_t Timer_toMillisecond( uint32_t ticks );
    void Timer_deinit( Timer_Driver* timer );

//    void RTC_init(Timer_Driver timer,  uint32_t value);
//    bool RTC_isDone(Timer_Driver timer);

//    uint32_t Timer_getMaxTicks();
//    uint32_t Timer_getCurrentDelay(Timer_Driver *timer);

PRIVATE
    const Timer_Driver* __Timer_getModule(Timer_t timer);
    bool __Timer_isAvailable( Timer_t timer );

#endif //TIMER_H_
