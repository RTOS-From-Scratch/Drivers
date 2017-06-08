#include "Timer.h"
#include "inner/__ISR_vectortable.h"
#include "inner/__IO.h"
#include "tm4c123gh6pm.h"
#include "ISR_ctrl.h"

#define SYSTIMER_MAX_TICKS 0xFFFFFFFF
#define TIMER_MODULE_NUM 6

struct Timer_Driver {
    byte module_num;
    byte ISR_vector_num;
};

typedef void(*__ISR_handler)();
static __ISR_handler Timer_ISR_Handlers[ TIMER_MODULE_NUM ];

static const Timer_Driver Timers[TIMER_MODULE_NUM] =
{
    { TIMER0, ISR_TIMER_0_SUBTIMER_A },       // Timer0
    { TIMER1, ISR_TIMER_1_SUBTIMER_A },       // Timer1
    { TIMER2, ISR_TIMER_2_SUBTIMER_A },       // Timer2
    { TIMER3, ISR_TIMER_3_SUBTIMER_A },       // Timer3
    { TIMER4, ISR_TIMER_4_SUBTIMER_A },       // Timer4
    { TIMER5, ISR_TIMER_5_SUBTIMER_A },       // Timer5
};

int availbeTimers[] = {0, 0, 0, 0, 0, 0};

void Timer_init(Timer_Driver* timer, TIMER_MODE mode, TIMER_DIR dir, uint32_t value)
{
    byte timer_module = timer->module_num;

    // enable timer clk
    IO_REG(__RCGC_BASE, __RCGCTIMER_OFFSET) |= ( 1 << timer_module );
    while((SYSCTL_PPTIMER_R & (1 << timer_module)) == 0);

    //disable TnEN bit in TIMERCTL register
    IO_REG(__Timer_Addr[timer_module], __TIMER_CTL_R) &= ~TIMER_CTL_TAEN;

    // clear configuration register
    // configure for 32-bit timer
    IO_REG(__Timer_Addr[timer_module], __TIMER_CFG_R) = 0x0;

    //if timer is periodic mode
    if(mode)
        IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R) |= TIMER_TAMR_TAMR_PERIOD;
    //if timer is one shot mode
    else
        IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R) = TIMER_TAMR_TAMR_1_SHOT;

    //set the direction of the counting
    IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R)   |= (dir << 4);

    //set the value of the timer
    IO_REG(__Timer_Addr[timer_module], __TIMER_TA_IL_R)  = value;

}

void Timer_ISR_init(Timer_Driver* timer, uint32_t value, void (*timer_handler)(), ISR_PRIORITY priority)
{
    byte timer_module = timer->module_num;
    IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R) |= 0x20;
    IO_REG(__Timer_Addr[timer_module], __TIMER_TA_MATCHR)   =  value;
    IO_REG(__Timer_Addr[timer_module], __TIMER_IM_R)   |= 0x10;

    NVIC_EN0_R |= 1 << 19;

    __ISR_register( timer->ISR_vector_num, Timer_ISR_handler );

    // set priority
    __ISR_CTRL_setPriority(timer->ISR_vector_num, priority);

    Timer_ISR_Handlers[timer->module_num] = timer_handler;

}

void Timer_ISR_handler()
{
    byte vectorNumber = NVIC_INT_CTRL_R & NVIC_INT_CTRL_VEC_ACT_M;
    const Timer_Driver* timer = NULL;
    intptr_t address;

    // find which module responsable for the interruption
    for( byte iii = 0; iii < TIMER_MODULE_NUM; ++iii )
        if( vectorNumber EQUAL Timers[iii].ISR_vector_num )
            timer = &Timers[iii];

    address = __Timer_Addr[timer->module_num];

    IO_REG(address, __TIMER_IC_R) |=
            IO_REG(address, __TIMER_MIS);

    Timer_ISR_Handlers[timer->module_num]();
}

void Timer_deinit(Timer_Driver* timer)
{
    byte timer_module = timer->module_num;
    availbeTimers[timer_module] = 0;

    IO_REG(__Timer_Addr[timer_module], __TIMER_CTL_R) &= ~TIMER_CTL_TAEN;
    IO_REG(__RCGC_BASE, __RCGCTIMER_OFFSET) &= ( 0 << timer_module );
    IO_REG(__Timer_Addr[timer_module], __TIMER_CFG_R) = 0x0;
    IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R) = 0;
}

bool __Timer_isAvailable(Timer_t timer)
{
    if (availbeTimers[timer])
        return false;
    return true;
}

void Timer_start(Timer_Driver* timer)
{
    IO_REG(__Timer_Addr[timer->module_num], __TIMER_CTL_R) |= (1 << 0);
}

bool Timer_isDone(Timer_Driver* timer)
{
    while(!(IO_REG(__Timer_Addr[timer->module_num], __TIMER_MRIS) & (1<< 0)));

    return true;
}

void Timer_reset(Timer_Driver* timer, int newValue, bool resetMatcher)
{
    // TODO: check if the timer initiated
//    curretTimerValue = IO_REG(__Timer_Addr[TIMER0], TIMER_TA_V);
//    nextTimerValue = (curretTimerValue + newValue) % SYSTIMER_MAX_TICKS;
    if (resetMatcher)
    {
        byte timer_module = timer->module_num;
        IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R)   |= TIMER_TAMR_TAMIE;
        IO_REG(__Timer_Addr[timer_module], __TIMER_TA_M_R) &= ~(TIMER_TAMR_TAMRSU);
        IO_REG(__Timer_Addr[timer_module], __TIMER_TA_MATCHR)   = newValue;
    }

}


uint32_t Timer_getCurrentTicks(Timer_Driver* timer)
{
    return IO_REG(__Timer_Addr[timer->module_num], __TIMER_TA_V);
}

uint32_t Timer_getMaxTicks()
{
    return SYSTIMER_MAX_TICKS;
}

uint32_t Timer_getCurrentDelay(Timer_Driver* timer)
{
    return IO_REG(__Timer_Addr[timer->module_num], __TIMER_TA_MATCHR);
}


const Timer_Driver* __Timer_getModule(Timer_t timer)
{
//    const Timer_Driver* driver = &Timers[timer];

    if( __Timer_isAvailable(timer) is false )
        return NULL;

    availbeTimers[timer] = 1;

    return &Timers[timer];
}
