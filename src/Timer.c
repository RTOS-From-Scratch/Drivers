#include "Timer.h"
#include "inner/inner_IO.h"

#define __TIMER_MODULE_NUMBER(timer_module) (byte)timer_module
#define __TIMER_COUNTER(timer_module) (byte)(timer_module >> BYTE_LENGTH)
#define __TIMER_PORT(timer_module) (byte)(timer_module >> (BYTE_LENGTH * 2))
#define __TIMER_PIN(timer_module) (byte)(timer_module >> (BYTE_LENGTH * 3))

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

void Timer_init(TIMER_MODULE_t timer_module, __Timer_Mode mode, __Timer_Count_Dir dir, uint32_t value)
{
    byte timer_num = __TIMER_MODULE_NUMBER(timer_module);
    byte counter = __TIMER_COUNTER(timer_module);
    byte port = __TIMER_PORT(timer_module);
    byte pin = __TIMER_PIN(timer_module);

    //configure Timer clock register and set the bit alternated to the timer number
    IO_REG(RCGC_BASE, RCGCTIMER_OFFSET)   |= (1<< timer_num);
    //configure GPIO clock register and set the bit alternated to the port number
    IO_REG(RCGC_BASE, RCGCGPIO_OFFSET)   |= (1<< port);
    //configure port control register and set the value of the timer configuration
    IO_REG(__PORTS_ADDR[port], __IO_PCTL)   |= (7 << pin);
    // clear configuration register
    IO_REG(__Timer_Addr[timer_num], GPTM_CFG_R)   &= 0;

    //check wether the timer is A or B
    //if timer is A
    if (!counter)
    {
        //disable TnEN bit in GPTMCTL register
        IO_REG(__Timer_Addr[timer_num], GPTM_CTL_R)   &= (0 << 0);

        //if timer is periodic mode
        if(mode)
            IO_REG(__Timer_Addr[timer_num], GPTM_TA_M_R)   |= 0x2;


        //if timer is one shot mode
        else
            IO_REG(__Timer_Addr[timer_num], GPTM_TA_M_R)   |= 0x1;

        //set the direction of the counter
        IO_REG(__Timer_Addr[timer_num], GPTM_TA_M_R)   = (dir << 4);
        //set the value of the timer
        IO_REG(__Timer_Addr[timer_num], GPTM_TA_IL_R)   = value;
    }

    //if timer is B
    else
    {
        //disable TnEN bit in GPTMCTL register
        IO_REG(__Timer_Addr[timer_num], GPTM_CTL_R)   &= (0 << 8);

        //if timer is periodic mode
        if(mode)
            IO_REG(__Timer_Addr[timer_num], GPTM_TB_M_R)   |= 0x2;

        //if timer is one shot mode
        else
            IO_REG(__Timer_Addr[timer_num], GPTM_TB_M_R)   |= 0x1;

        //set the direction of the counter
        IO_REG(__Timer_Addr[timer_num], GPTM_TB_M_R)   = (dir << 4);
        //set the value of the timer
        IO_REG(__Timer_Addr[timer_num], GPTM_TB_IL_R)   = value;
    }

}

bool Timer_start(TIMER_MODULE_t timer_module)
{
    byte timer_num = __TIMER_MODULE_NUMBER(timer_module);
    byte counter = __TIMER_COUNTER(timer_module);
    byte port = __TIMER_PORT(timer_module);
    byte pin = __TIMER_PIN(timer_module);

    if (counter)
        //set enable bit in control register to start counting if timer is B
        IO_REG(__Timer_Addr[timer_num], GPTM_CTL_R) |= (1<< 8);

    else
        //set enable bit in control register to start counting if timer is A
        IO_REG(__Timer_Addr[timer_num], GPTM_CTL_R) |= (1<< 0);

    //poll the register to check if timer is timed out
    return Timer_isDone(timer_module);
}

bool Timer_isDone(TIMER_MODULE_t timer_module)
{
    byte timer_num = __TIMER_MODULE_NUMBER(timer_module);
    byte counter = __TIMER_COUNTER(timer_module);
    byte port = __TIMER_PORT(timer_module);
    byte pin = __TIMER_PIN(timer_module);

    if (counter)
    {
        while(!(IO_REG(__Timer_Addr[timer_num], GPTM_MRIS) & (1<< 8)));
        return true;
    }
    else
    {
        while(!(IO_REG(__Timer_Addr[timer_num], GPTM_MRIS) & (1<< 0)));
        return true;
    }
}
