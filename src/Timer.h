#ifndef TIMER_H_
#define TIMER_H_

#include "IO.h"
#include "inner/inner_Timer.h"
#include <stdint.h>
#include <stdbool.h>

#define TIMER_MODULE_t      __Timer_Base_Addr timer_num, __Timer_Counter counter, PORT_PIN
#define TIMER0_A_PORTB      Timer_0, Timer_A, B6
#define TIMER0_A_PORTF      Timer_0, Timer_A, F0
#define TIMER0_B_PORTB      Timer_0, Timer_B, B7
#define TIMER0_B_PORTF      Timer_0, Timer_B, F1
#define TIMER1_A_PORTB      Timer_1, Timer_A, B4
#define TIMER1_A_PORTF      Timer_1, Timer_A, F2
#define TIMER1_B_PORTB      Timer_1, Timer_B, B5
#define TIMER1_B_PORTF      Timer_1, Timer_B, F3
#define TIMER2_A_PORTB      Timer_2, Timer_A, B0
#define TIMER2_A_PORTF      Timer_2, Timer_A, F4
#define TIMER2_B_PORTB      Timer_2, Timer_B, B1
#define TIMER3_A_PORTB      Timer_3, Timer_A, B2
#define TIMER3_B_PORTB      Timer_3, Timer_B, B3
#define TIMER4_A_PORTC      Timer_4, Timer_A, C0
#define TIMER4_B_PORTC      Timer_4, Timer_B, C1
#define TIMER5_A_PORTC      Timer_5, Timer_A, C2
#define TIMER5_B_PORTC      Timer_5, Timer_B, C3


void Timer_init(TIMER_MODULE_t, __Timer_Mode mode, __Timer_Count_Dir dir, uint32_t value);
bool Timer_start(TIMER_MODULE_t);
bool Timer_isDone(TIMER_MODULE_t);


#endif //TIMER_H_
