#ifndef INNER_Timer_H_
#define INNER_Timer_H_

#include "../IO.h"
#include <stdint.h>

#define RCGC_BASE           0x400FE000
#define RCGCTIMER_OFFSET    0x604
#define RCGCGPIO_OFFSET     0x608

enum __timer_reg_offset
{
    TIMER_CTL_R = 0x00C,
    TIMER_CFG_R = 0x000,
    TIMER_TA_M_R = 0x004,
    TIMER_TB_M_R = 0x008,
    TIMER_TA_IL_R = 0x028,
    TIMER_TB_IL_R = 0x02C,
    TIMER_MRIS = 0x01C,
    TIMER_IC_R = 0X024,
    TIMER_IM_R = 0x018
};
typedef enum __timer_reg_offset __timer_reg_offset;

enum __Timer_Count_Dir
{
    __DOWN, __UP
};
typedef enum __Timer_Count_Dir __Timer_Count_Dir;

enum __Timer_Mode
{
    __one_shot_timer,
    __periodic_timer
};
typedef enum __Timer_Mode __Timer_Mode;

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
