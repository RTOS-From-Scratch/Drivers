#ifndef INNER_Timer_H_
#define INNER_Timer_H_

#include "../GPIO.h"
#include <stdint.h>

#define __RCGC_BASE           0x400FE000
#define __RCGCTIMER_OFFSET    0x604

enum __timer_reg_offset
{
    __TIMER_PP                      = 0xFC0,
    __TIMER_CTL_R                   = 0x00C,
    __TIMER_CONFIG                  = 0x000,
    __TIMER_TIMER_A_MODE            = 0x004,
    __TIMER_TIMER_B_MODE            = 0x008,
    __TIMER_TIMER_A_INTERVAL_LOAD   = 0x028,
    __TIMER_TIMER_B_INTERVAL_LOAD   = 0x02C,
    __TIMER_RAW_INTERRUPT_STATUS    = 0x01C,
    __TIMER_INTERRUPT_CLEAR         = 0X024,
    __TIMER_INTERRUPT_MASK          = 0x018,
    __TIMER_TIMER_A_VALUE           = 0x050,
    __TIMER_TIMER_A_MATCHR          = 0x030,
    __TIMER_TIMER_B_MATCHR          = 0x034,
    __TIMER_TIMER_A_PRESCALE        = 0x038,
    __TIMER_MASKED_INTERRUPT_STATUS = 0x020
};
typedef enum __timer_reg_offset __timer_reg_offset;

enum __Timer_Counter {
    __Timer_A, __Timer_B
};

typedef enum __Timer_Counter __Timer_Counter;

static const unsigned long __Timer_Addr[] = {
    0x40030000,
    0x40031000,
    0x40032000,
    0x40033000,
    0x40034000,
    0x40035000
};

void __start_critical();
void __end_critical();

#endif //INNER_Timer_H_
