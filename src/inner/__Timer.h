#ifndef INNER_Timer_H_
#define INNER_Timer_H_

#include "../GPIO.h"
#include <stdint.h>

#define __RCGC_BASE           0x400FE000
#define __RCGCTIMER_OFFSET    0x604

enum __timer_reg_offset
{
    __TIMER_PP        = 0xFC0,
    __TIMER_CTL_R     = 0x00C,
    __TIMER_CFG_R     = 0x000,
    __TIMER_TA_M_R    = 0x004,
    __TIMER_TB_M_R    = 0x008,
    __TIMER_TA_IL_R   = 0x028,
    __TIMER_TB_IL_R   = 0x02C,
    __TIMER_MRIS      = 0x01C,
    __TIMER_IC_R      = 0X024,
    __TIMER_IM_R      = 0x018,
    __TIMER_TA_V      = 0x050,
    __TIMER_TA_MATCHR = 0x030,
    __TIMER_TB_MATCHR = 0x034,
    __TIMER_TA_PR_R   = 0x038,
    __TIMER_MIS       = 0x020
};
typedef enum __timer_reg_offset __timer_reg_offset;

enum __Timer_Counter
{
    __Timer_A, __Timer_B
};
typedef enum __Timer_Counter __Timer_Counter;

static const unsigned long __Timer_Addr[]=
{
    0x40030000,
    0x40031000,
    0x40032000,
    0x40033000,
    0x40034000,
    0x40035000
};


void __start_critical ();
void __end_critical ();

#endif //INNER_Timer_H_
