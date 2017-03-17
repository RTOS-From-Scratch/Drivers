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

enum __Timer_Base_Addr
{
    Timer_0 = 0x40030000,
    Timer_1 = 0x40031000,
    Timer_2 = 0x40032000,
    Timer_3 = 0x40033000,
    Timer_4 = 0x40034000,
    Timer_5 = 0x40035000
};
typedef enum __Timer_Base_Addr __Timer_Base_Addr;

#endif //INNER_Timer_H_
