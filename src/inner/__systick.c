#include "__systick.h"
#include "tm4c123gh6pm.h"
#include "Misc/src/definitions.h"
#include "__ISR_vectortable.h"
#include <stddef.h>
#include "../PLL.h"

#define ST_MODE_NOT_INITIATED -1

static byte __mode = ST_MODE_NOT_INITIATED;
static int __rounds = -1;
static bool __isStarted = false;
static uint32_t __ticks = 0;

static void(*__run_me)() = NULL;

static void ST_handler();

void __SysTick_init( uint32_t ticks, void(*run)(), ISR_PRIORITY priority )
{
    ticks--;
    __run_me = run;
    __isStarted = true;

    __ticks = ticks;

    // disable before configurations
    NVIC_ST_CTRL_R = 0;

    // enable interrupt, use system clock
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN;

    // clear current ticks
    NVIC_ST_CURRENT_R = 0;

    if( ticks > NVIC_ST_RELOAD_M )
    {
        __rounds = ticks / NVIC_ST_RELOAD_M;
        ticks = ticks % NVIC_ST_RELOAD_M;
    }

    NVIC_PRI3_R = (NVIC_PRI3_R & ~NVIC_PRI3_INT15_M) | (priority << NVIC_PRI3_INT15_S);

    NVIC_ST_RELOAD_R = ticks;

    __ISR_register(ISR_SYSTICK, ST_handler);
}

bool __SysTick_start( ST_MODE mode )
{
    if( mode is ST_MODE_NOT_INITIATED )
        return false;

    __mode = mode;

    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;

    // change the reload value to be used after this round
    if( __rounds NOT_EQUAL -1 )
        NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;

    return true;
}

void __SysTick_stop()
{
    if( !__isStarted )
        return;

    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
    __rounds = -1;
    __mode = ST_MODE_NOT_INITIATED;
    __isStarted = false;
}

static void ST_handler()
{
    // run the user function
    __run_me();

    if( __mode is ST_MODE_ONE_SHOT )
    {
        __SysTick_stop();
        return;
    }

    else if( --__rounds > 0 )
    {
        __SysTick_reset(__ticks);
        __SysTick_start(__mode);
    }

//    else
//        __SysTick_stop();
}

void __SysTick_reset( uint32_t ticks )
{
    __SysTick_stop();

    if( ticks > NVIC_ST_RELOAD_M )
    {
        __rounds = ticks / NVIC_ST_RELOAD_M;
        ticks = ticks % NVIC_ST_RELOAD_M;
    }

    NVIC_ST_RELOAD_R = ticks;
}

bool __SysTick_isInit()
{
    return NVIC_ST_CTRL_R NOT_EQUAL 0 ? true : false;
}

uint32_t __SysTick_getTicks(uint32_t milliSecond)
{
    ClockSpeed sys_clk_speed = PLL_getClockSpeed();
    return (sys_clk_speed * 1000) * milliSecond;
}

uint32_t __SysTick_getMilliSecond(uint32_t ticks)
{
    ClockSpeed sys_clk_speed = PLL_getClockSpeed();
    return ticks / (sys_clk_speed * 1000);
}
