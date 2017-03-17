#include "Timer.h"
#include "inner/inner_IO.h"

enum __timer_reg_offset
{
    GPTM_CTL_R = 0x00C,
    GPTM_CFG_R = 0x000,
    GPTM_TA_M_R = 0x004,
    GPTM_TB_M_R = 0x008,
    GPTM_TA_IL_R = 0x028,
    GPTM_TB_IL_R = 0x02C,
    GPTM_MRIS = 0x01C
};
typedef enum __timer_reg_offset __timer_reg_offset;

void Timer_init(TIMER_MODULE_t, __Timer_Mode mode, __Timer_Count_Dir dir, uint32_t value)
{
    //configure Timer clock register and set the bit alternated to the timer number
    IO_REG(RCGC_BASE, RCGCTIMER_OFFSET)   |= (1<< timer_num);
    //configure GPIO clock register and set the bit alternated to the port number
    IO_REG(RCGC_BASE, RCGCGPIO_OFFSET)   |= (1<< port);
    //configure port control register and set the value of the timer configuration
    IO_REG(__PORTS_ADDR[port], __IO_PCTL)   |= 0x7;
    // clear configuration register
    IO_REG(timer_num, GPTM_CFG_R)   &= 0;

    //check wether the timer is A or B
    //if timer is A
    if (!counter)
    {
        //disable TnEN bit in GPTMCTL register
        IO_REG(timer_num, GPTM_CTL_R)   &= (0 << 0);

        //if timer is periodic mode
        if(mode)
            IO_REG(timer_num, GPTM_TA_M_R)   |= 0x2;


        //if timer is one shot mode
        else
            IO_REG(timer_num, GPTM_TA_M_R)   |= 0x1;

        //set the direction of the counter
        IO_REG(timer_num, GPTM_TA_M_R)   = (dir << 4);
        //set the value of the timer
        IO_REG(timer_num, GPTM_TA_IL_R)   = value;
    }

    //if timer is B
    else
    {
        //disable TnEN bit in GPTMCTL register
        IO_REG(timer_num, GPTM_CTL_R)   &= (0 << 8);

        //if timer is periodic mode
        if(mode)
            IO_REG(timer_num, GPTM_TB_M_R)   |= 0x2;

        //if timer is one shot mode
        else
            IO_REG(timer_num, GPTM_TB_M_R)   |= 0x1;

        //set the direction of the counter
        IO_REG(timer_num, GPTM_TB_M_R)   = (dir << 4);
        //set the value of the timer
        IO_REG(timer_num, GPTM_TB_IL_R)   = value;
    }

}

bool Timer_start(TIMER_MODULE_t )
{

    if (counter)
        //set enable bit in control register to start counting if timer is B
        IO_REG(timer_num, GPTM_CTL_R) |= (1<< 8);

    else
        //set enable bit in control register to start counting if timer is A
        IO_REG(timer_num, GPTM_CTL_R) |= (1<< 0);

    //poll the register to check if timer is timed out
    return Timer_isDone(timer_num, counter, port, pin);
}

bool Timer_isDone(TIMER_MODULE_t)
{
    if (counter)
    {
        while(!(IO_REG(timer_num, GPTM_MRIS) & (1<< 8)));
        return true;
    }
    else
    {
        while(!(IO_REG(timer_num, GPTM_MRIS) & (1<< 0)));
        return true;
    }
}

