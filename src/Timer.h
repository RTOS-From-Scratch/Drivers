#ifndef TIMER_H_
#define TIMER_H_

#include "IO.h"
#include "inner/__Timer.h"
#include <stdint.h>
#include <stdbool.h>

//#define TIMER_MODULE_t      __Timer_Base_Addr timer_num, __Timer_Counter counter, PORT_PIN
//#define TIMER0_A_PORTB      Timer_0, Timer_A, B6
//#define TIMER0_A_PORTF      Timer_0, Timer_A, F0
//#define TIMER0_B_PORTB      Timer_0, Timer_B, B7
//#define TIMER0_B_PORTF      Timer_0, Timer_B, F1
//#define TIMER1_A_PORTB      Timer_1, Timer_A, B4
//#define TIMER1_A_PORTF      Timer_1, Timer_A, F2
//#define TIMER1_B_PORTB      Timer_1, Timer_B, B5
//#define TIMER1_B_PORTF      Timer_1, Timer_B, F3
//#define TIMER2_A_PORTB      Timer_2, Timer_A, B0
//#define TIMER2_A_PORTF      Timer_2, Timer_A, F4
//#define TIMER2_B_PORTB      Timer_2, Timer_B, B1
//#define TIMER3_A_PORTB      Timer_3, Timer_A, B2
//#define TIMER3_B_PORTB      Timer_3, Timer_B, B3
//#define TIMER4_A_PORTC      Timer_4, Timer_A, C0
//#define TIMER4_B_PORTC      Timer_4, Timer_B, C1
//#define TIMER5_A_PORTC      Timer_5, Timer_A, C2
//#define TIMER5_B_PORTC      Timer_5, Timer_B, C3
//0x06010000

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


void Timer_init(TIMER_MODULE_t, __Timer_Mode mode, __Timer_Count_Dir dir, uint32_t value);
bool Timer_start(TIMER_MODULE_t);
bool Timer_isDone(TIMER_MODULE_t);


#endif //TIMER_H_
