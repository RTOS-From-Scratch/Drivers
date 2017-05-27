#ifndef TIMER_H_
#define TIMER_H_

#include "GPIO.h"
#include "inner/__Timer.h"
#include <stdint.h>
#include <stdbool.h>

void (*PeriodicTask)(void);


//TODO:remove this
#define NVIC_R 0xE000E000
#define PRI4 0x410
#define EN0 0x100

// timer_num, counter, PORT, PIN
typedef enum TIMER_MODULE_t
{
    TIMER0_A_PORTB = 0x06010000,
    TIMER0_A_PORTF = 0x00050000,
    TIMER0_B_PORTB = 0x07010100,
    TIMER0_B_PORTF = 0x01050100,
    TIMER1_A_PORTB = 0x04010001,
    TIMER1_A_PORTF = 0x02050001,
    TIMER1_B_PORTB = 0x05010101,
    TIMER1_B_PORTF = 0x03050101,
    TIMER2_A_PORTB = 0x00010002,
    TIMER2_A_PORTF = 0x04050002,
    TIMER2_B_PORTB = 0x01010102,
    TIMER3_A_PORTB = 0x02010003,
    TIMER3_B_PORTB = 0x03010103,
    TIMER4_A_PORTC = 0x00030004,
    TIMER4_B_PORTC = 0x01030104,
    TIMER5_A_PORTC = 0x02030005,
    TIMER5_B_PORTC = 0x03030105
} TIMER_MODULE_t;


void Timer_init(TIMER_MODULE_t timer_module, __Timer_Mode mode, __Timer_Count_Dir dir,  uint32_t value);
bool Timer_start(TIMER_MODULE_t timer_module);
bool Timer_isDone(TIMER_MODULE_t timer_module);
void Timer_handleDelayedStart(int value);

#endif //TIMER_H_
