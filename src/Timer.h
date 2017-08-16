#ifndef TIMER_H_
#define TIMER_H_

#include "GPIO.h"
#include "inner/__Timer.h"
#include <stdint.h>
#include <stdbool.h>
#include "DataStructures/src/inverted_priority_queue.h"
#include "Misc/src/definitions.h"

PUBLIC
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
        TIMER_MODE_PERIODIC,
        TIMER_MODE_PWM                  // Un-finished yet
    } TIMER_MODE;

    typedef struct Timer_Driver Timer_Driver;

    void Timer_init( Timer_Driver* timer,
                     TIMER_MODE mode,
                     TIMER_DIR dir,
                     uint32_t value );
    void Timer_ISRConfig( Timer_Driver* timer,
                          void(*timer_handler)(),
                          ISR_PRIORITY priority );
    void Timer_matcherConfig( Timer_Driver* timer,
                              uint32_t matchValue,
                              void(*matcher_handler)(),
                              ISR_PRIORITY priority );
    void Timer_ISR_changePriroity( Timer_Driver *timer, ISR_PRIORITY newPriority );
    void Timer_start( Timer_Driver* timer );
    void Timer_stop( Timer_Driver* timer );
    bool Timer_reset( Timer_Driver *timer, uint32_t newValue );
    bool Timer_resetMatcher( Timer_Driver *timer, uint32_t newValue );
    bool Timer_isDone( Timer_Driver* timer );
    bool Timer_isTicking( Timer_Driver* timer );
    uint32_t Timer_getCurrentTicks( Timer_Driver *timer );
    uint32_t Timer_toTicks( uint32_t millisecond );
    uint32_t Timer_toMillisecond( uint32_t ticks );
    uint32_t Timer_getMaxTicks();
    void Timer_deinit( Timer_Driver* timer );

//    void RTC_init(Timer_Driver timer,  uint32_t value);
//    bool RTC_isDone(Timer_Driver timer);

//    uint32_t Timer_getMaxTicks();
//    uint32_t Timer_getCurrentDelay(Timer_Driver *timer);

PRIVATE
    const Timer_Driver* __Timer_getModule(Timer_t timer);
    bool __Timer_isAvailable( Timer_t timer );

#endif //TIMER_H_
