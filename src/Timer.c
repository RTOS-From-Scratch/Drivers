#include "Timer.h"
#include "inner/__ISR_vectortable.h"
#include "inner/__IO.h"
#include "tm4c123gh6pm.h"
#include "ISR_ctrl.h"
#include "PLL.h"

#define TIMER_MODULE_NUM 6
#define TIMER_MAX_TICKS 0xFFFFFFFF

struct Timer_Driver {
    byte module_num;
    byte ISR_vector_num;
};

typedef void(*__ISR_handler)();
static __ISR_handler __Timer_ISR_Handlers[ TIMER_MODULE_NUM ];

static const Timer_Driver Timers[TIMER_MODULE_NUM] = {
    { TIMER0, ISR_TIMER_0_SUBTIMER_A },       // Timer0
    { TIMER1, ISR_TIMER_1_SUBTIMER_A },       // Timer1
    { TIMER2, ISR_TIMER_2_SUBTIMER_A },       // Timer2
    { TIMER3, ISR_TIMER_3_SUBTIMER_A },       // Timer3
    { TIMER4, ISR_TIMER_4_SUBTIMER_A },       // Timer4
    { TIMER5, ISR_TIMER_5_SUBTIMER_A },       // Timer5
};

uint8_t __availableTimers[TIMER_MODULE_NUM] = { 0 };

static void Timer_ISR_handler();

void Timer_init( Timer_Driver* timer, TIMER_MODE mode, TIMER_DIR dir, uint32_t value )
{
    byte timer_module = timer->module_num;

    // enable timer clk
    IO_REG(__RCGC_BASE, __RCGCTIMER_OFFSET) |= ( 1 << timer_module );
    while((SYSCTL_PPTIMER_R & (1 << timer_module)) == 0);

    // disable the timer for configurations
    // disable TnEN bit in TIMERCTL register
    IO_REG(__Timer_Addr[timer_module], __TIMER_CTL_R) &= ~TIMER_CTL_TAEN;

    // clear configuration register
    // configure for 32-bit timer
    IO_REG(__Timer_Addr[timer_module], __TIMER_CONFIG) = 0x0;

    // if timer is periodic mode
    if( mode is TIMER_MODE_PERIODIC )
        IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE) =
                ( IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE) & ~TIMER_TAMR_TAMR_M )
                + TIMER_TAMR_TAMR_PERIOD;

    // if timer is one shot mode
    else if( mode is TIMER_MODE_ONE_SHOT )
        IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE) =
                ( IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE) & ~TIMER_TAMR_TAMR_M )
                + TIMER_TAMR_TAMR_1_SHOT;

    else if( mode is TIMER_MODE_PWM )
    {
        // TODO:
    }

    // set the direction of the counting
    IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE)   |= (dir << 4);

    // set the value of the timer
    IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_INTERVAL_LOAD)  = value;
}

void Timer_ISRConfig( Timer_Driver* timer,
                      void (*timer_handler)(),
                      ISR_PRIORITY priority )
{
    // unmask timeout interrupt
    IO_REG(__Timer_Addr[timer->module_num], __TIMER_INTERRUPT_MASK) |= TIMER_IMR_TATOIM;

    __ISR_register( timer->ISR_vector_num, Timer_ISR_handler );

    // set priority
    __ISR_CTRL_setPriority(timer->ISR_vector_num, priority);

    __Timer_ISR_Handlers[timer->module_num] = timer_handler;

    // enable global interrupt for this timer
    __ISR_CTRL_enable(timer->ISR_vector_num);
}

void Timer_matcherConfig( Timer_Driver* timer,
                          uint32_t matchValue,
                          void(*matcher_handler)(),
                          ISR_PRIORITY priority )
{
    const unsigned long timer_address = __Timer_Addr[timer->module_num];

    IO_REG(timer_address, __TIMER_TIMER_A_MATCHR) = matchValue;

    if( matcher_handler )
    {
        Timer_ISRConfig( timer, matcher_handler, priority );
        // enable matcher interrupt
        IO_REG(timer_address, __TIMER_TIMER_A_MODE)   |= TIMER_TAMR_TAMIE;
        // unmask matcher interrupt
        IO_REG(timer_address, __TIMER_INTERRUPT_MASK) |= TIMER_IMR_TAMIM;
    }
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

    IO_REG(address, __TIMER_INTERRUPT_CLEAR) |=
            IO_REG(address, __TIMER_MASKED_INTERRUPT_STATUS);

    __Timer_ISR_Handlers[timer->module_num]();
}

void Timer_ISR_changePriroity( Timer_Driver *timer, ISR_PRIORITY newPriority )
{
    __ISR_CTRL_setPriority( timer->ISR_vector_num, newPriority );
}

void Timer_start(Timer_Driver* timer)
{
    IO_REG(__Timer_Addr[timer->module_num], __TIMER_CTL_R) |= TIMER_CTL_TAEN;
}

void Timer_stop( Timer_Driver* timer )
{
//    const unsigned long timer_address = __Timer_Addr[timer->module_num];
//    // if ISR mode is enabled
//    if( __Timer_ISR_Handlers[timer->module_num] is_not NULL )
//    {
//        // disable matcher interrupt
//        IO_REG(timer_address, __TIMER_TIMER_A_MODE)   &= ~TIMER_TAMR_TAMIE;
//        // mask matcher interrupt
//        IO_REG(timer_address, __TIMER_INTERRUPT_MASK) &= ~TIMER_IMR_TAMIM;
//        // mask timeout interrupt
//        IO_REG(timer_address, __TIMER_INTERRUPT_MASK) &= ~TIMER_IMR_TATOIM;
//    }

    // disable timer
    IO_REG(__Timer_Addr[timer->module_num], __TIMER_CTL_R) &= ~TIMER_CTL_TAEN;
}

bool Timer_reset( Timer_Driver* timer, uint32_t newValue )
{
    // TODO: redesign this func
    // TODO: check if the timer initiated
//    curretTimerValue = IO_REG(__Timer_Addr[TIMER0], TIMER_TA_V);
//    nextTimerValue = (curretTimerValue + newValue) % SYSTIMER_MAX_TICKS;
//    if(resetMatcher)
//    {
//        byte timer_module = timer->module_num;
//        IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE)   |= TIMER_TAMR_TAMIE;
//        IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MODE)   &= ~(TIMER_TAMR_TAMRSU);
//        IO_REG(__Timer_Addr[timer_module], __TIMER_TIMER_A_MATCHR) = newValue;
//    }

    if( Timer_isTicking(timer) )
        return false;

    else
    {
        IO_REG(__Timer_Addr[timer->module_num], __TIMER_TIMER_A_INTERVAL_LOAD) =
                newValue;
        return true;
    }
}

bool Timer_resetMatcher( Timer_Driver *timer, uint32_t newValue )
{
    if( Timer_isTicking(timer) )
        return false;

    else
    {
        IO_REG(__Timer_Addr[timer->module_num], __TIMER_TIMER_A_MATCHR) = newValue;
        return true;
    }
}

bool Timer_isDone(Timer_Driver* timer)
{
    if( IO_REG(__Timer_Addr[timer->module_num], __TIMER_RAW_INTERRUPT_STATUS)
            & TIMER_RIS_TATORIS )
        return false;
    else
        return true;
}

bool Timer_isTicking( Timer_Driver* timer )
{
    if( IO_REG(__Timer_Addr[timer->module_num], __TIMER_CTL_R) & TIMER_CTL_TAEN )
        return true;

    else
        return false;
}

uint32_t Timer_getCurrentTicks(Timer_Driver* timer)
{
    return IO_REG(__Timer_Addr[timer->module_num], __TIMER_TIMER_A_VALUE);
}

uint32_t Timer_getMaxTicks()
{
    return TIMER_MAX_TICKS;
}

uint32_t Timer_getCurrentDelay(Timer_Driver* timer)
{
    return IO_REG(__Timer_Addr[timer->module_num], __TIMER_TIMER_A_MATCHR);
}

void Timer_deinit(Timer_Driver* timer)
{
    const unsigned long timer_address = __Timer_Addr[timer->module_num];
    __availableTimers[timer->module_num] = 0;

    // disable the Timer
    IO_REG(timer_address, __TIMER_CTL_R) &= ~TIMER_CTL_TAEN;
    // clear its mode
    IO_REG(timer_address, __TIMER_TIMER_A_MODE) = 0;
    // disable the clock of this timer module
    IO_REG(__RCGC_BASE, __RCGCTIMER_OFFSET) &= ( 0 << timer->module_num );

    if( __Timer_ISR_Handlers[timer->module_num] is_not NULL )
    {
        // disable matcher interrupt
        IO_REG(timer_address, __TIMER_TIMER_A_MODE)   &= ~TIMER_TAMR_TAMIE;
        // mask matcher interrupt
        IO_REG(timer_address, __TIMER_INTERRUPT_MASK) &= ~TIMER_IMR_TAMIM;
        // mask timeout interrupt
        IO_REG(timer_address, __TIMER_INTERRUPT_MASK) &= ~TIMER_IMR_TATOIM;
    }
}

uint32_t Timer_toTicks( uint32_t millisecond )
{
    return millisecond * PLL_getClockSpeed() * 1000;
}

uint32_t Timer_toMillisecond( uint32_t ticks )
{
    return ticks / (PLL_getClockSpeed() * 1000);
}

/****************************** PRIVATE part ******************************/
bool __Timer_isAvailable(Timer_t timer)
{
    return __availableTimers[timer] ? false : true;
}

const Timer_Driver* __Timer_getModule(Timer_t timer)
{
//    const Timer_Driver* driver = &Timers[timer];

    if( __Timer_isAvailable(timer) is false )
        return NULL;

    __availableTimers[timer] = 1;

    return &Timers[timer];
}
