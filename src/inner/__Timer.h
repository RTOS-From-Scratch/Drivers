#ifndef INNER_Timer_H_
#define INNER_Timer_H_

#include "../IO.h"

#define RCGC_BASE           0x400FE000
#define RCGCTIMER_OFFSET    0x604
#define RCGCGPIO_OFFSET     0x608

enum __Timer_Count_Dir
{
    __DOWN, __UP
};
typedef enum __Timer_Count_Dir __Timer_Count_Dir;

enum __Timer_Counter
{
    Timer_A, Timer_B
};
typedef enum __Timer_Counter __Timer_Counter;

enum __Timer_Mode
{
    __one_shot_timer,
    __periodic_timer
};
typedef enum __Timer_Mode __Timer_Mode;

static const unsigned long __Timer_Addr[]=
{
    0x40030000,
    0x40031000,
    0x40032000,
    0x40033000,
    0x40034000,
    0x40035000
};

#endif //INNER_Timer_H_
