#include "time.h"
#include "Timer.h"
#include "driver.h"
#include <stddef.h>
#include <limits.h>
#include "Misc/src/assert.h"
#include "PLL.h"

#define MAX_TIMER_VALUE UINT_MAX
static Timer_Driver *timer = NULL;
static void __delay( uint32_t ticks );

#define MSEC_TO_TICKS(msec) ( (msec * PLL_getClockSpeed() * 1E6) / 1000 )
#define USEC_TO_TICKS(msec) ( MSEC_TO_TICKS(msec) / 1000 )
#define TICKS_TO_MSEC(ticks) ( (ticks * 1000) / (PLL_getClockSpeed() * 1E6) )

void __time_init()
{
    timer = Driver_construct( Timer, TIMER0 );
    Timer_init( timer,
                TIMER_MODE_PERIODIC,
                TIMER_DIR_COUNT_UP,
                MAX_TIMER_VALUE );
    Timer_start(timer);
}

void __delay( uint32_t ticks )
{
    ASSERT( timer is_not NULL );

    uint32_t progress = 0;
    uint32_t old = Timer_getCurrentTicks(timer);
    uint32_t current = 0;
    int64_t reminder =  MAX_TIMER_VALUE - Timer_getCurrentTicks(timer) + ticks;

    if( reminder < 0 )
    {
        while( (current = Timer_getCurrentTicks(timer)) >= progress )
        {
            progress += current - old;
            old = current;
        }

        progress += MAX_TIMER_VALUE - old;
        old = 0;
    }

    while( (current = Timer_getCurrentTicks(timer)) >= progress )
    {
        if( ( progress += current - old ) >= ticks )
            break;

        old = current;
    }
}

void delay_msec( uint16_t msec )
{
    uint32_t ticks = MSEC_TO_TICKS(msec);
    __delay(ticks);
}

void delay_usec( uint16_t usec )
{
    uint32_t ticks = USEC_TO_TICKS(usec);
    __delay(ticks);
}

uint32_t get_current_time()
{
    ASSERT( timer is_not NULL );
    return Timer_getCurrentTicks(timer);
}

uint32_t ticks_to_msec(uint32_t ticks)
{
    return TICKS_TO_MSEC(ticks);
}

uint32_t msec_to_ticks(uint16_t msec)
{
    return MSEC_TO_TICKS(msec);
}
