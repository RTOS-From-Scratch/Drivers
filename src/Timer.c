#include "Timer.h"
#include "inner/__IO.h"
#include "tm4c123gh6pm.h"

#define __TIMER_MODULE_NUMBER(timer_module) (byte)timer_module
#define TIMER0 0
#define SYSTIMER_MAX_TICKS 0xFFFFFFFF

void Timer_init(TIMER_MODULE_t timer_module, __Timer_Mode mode, __Timer_Count_Dir dir, uint32_t value)
{
    byte timer_num = __TIMER_MODULE_NUMBER(timer_module);

    //configure Timer clock register and set the bit alternated to the timer number
    // enable timer clk
    uint32_t delay;
    IO_REG(RCGC_BASE, RCGCTIMER_OFFSET) |= ( 1 << timer_num );
    delay = IO_REG(RCGC_BASE, RCGCTIMER_OFFSET);

    //disable TnEN bit in TIMERCTL register
    IO_REG(__Timer_Addr[timer_num], TIMER_CTL_R) = 0x0;

    // clear configuration register
    // configure for 32-bit timer
    IO_REG(__Timer_Addr[timer_num], TIMER_CFG_R) = 0x0;

    //if timer is periodic mode
    if(mode)
        IO_REG(__Timer_Addr[timer_num], TIMER_TA_M_R) |= 0x2;

    //if timer is one shot mode
    else
        IO_REG(__Timer_Addr[timer_num], TIMER_TA_M_R) = 0x1;

    //set the direction of the counting
    IO_REG(__Timer_Addr[timer_num], TIMER_TA_M_R)   |= (dir << 4);

    //set the value of the timer
    IO_REG(__Timer_Addr[timer_num], TIMER_TA_IL_R)  = value;
}

void Timer_start(TIMER_MODULE_t timer_module)
{
    byte timer_num = __TIMER_MODULE_NUMBER(timer_module);

    IO_REG(__Timer_Addr[timer_num], TIMER_CTL_R) |= (1 << 0);
}

bool Timer_isDone(TIMER_MODULE_t timer_module)
{
    byte timer_num = __TIMER_MODULE_NUMBER(timer_module);

    while(!(IO_REG(__Timer_Addr[timer_num], TIMER_MRIS) & (1<< 0)));

    return true;
}

void Timer_SysTimer_init(int value)
{
    Timer_init(TIMER0_PORTB, __periodic_timer, __UP, 0xffffffff);

    IO_REG(__Timer_Addr[TIMER0], TIMER_TA_M_R) |= 0x20;
    IO_REG(__Timer_Addr[TIMER0], TIMER_TA_MATCHR)   =  value;
    IO_REG(__Timer_Addr[TIMER0], TIMER_IM_R)   |= 0x10;

    NVIC_EN0_R |= 1 << 19;
}

void Timer_sysTimer_start()
{
    // TODO: check if the timer initiated
    Timer_start(TIMER0_PORTB);
}

void Timer_sysTimer_stop()
{
    // TODO: check if the timer initiated
}

void Timer_sysTimer_reset(int newValue)
{
    // TODO: check if the timer initiated
//    curretTimerValue = IO_REG(__Timer_Addr[TIMER0], TIMER_TA_V);
//    nextTimerValue = (curretTimerValue + newValue) % SYSTIMER_MAX_TICKS;
    IO_REG(__Timer_Addr[TIMER0], TIMER_TA_M_R)   |= TIMER_TAMR_TAMIE;
    IO_REG(__Timer_Addr[TIMER0], TIMER_TA_M_R) &= ~(TIMER_TAMR_TAMRSU);
    IO_REG(__Timer_Addr[TIMER0], TIMER_TA_MATCHR)   = newValue;

}

bool Timer_sysTimer_isWorking()
{
    if ((IO_REG(RCGC_BASE, RCGCTIMER_OFFSET) & (1 << 0)) == 0)
        return false;
    else
        return true;
}

bool Timer_sysTimer_isDone()
{
    while(!(IO_REG(__Timer_Addr[TIMER0], TIMER_MIS) & (1<< 4)));
    return true;
}

uint32_t Timer_sysTimer_getCurrentTicks()
{
    return IO_REG(__Timer_Addr[TIMER0], TIMER_TA_V);
}

uint32_t Timer_sysTimer_getMaxTicks()
{
    return SYSTIMER_MAX_TICKS;
}

uint32_t Timer_sysTimer_getCurrentDelay()
{
    return IO_REG(__Timer_Addr[TIMER0], TIMER_TA_MATCHR);
}


