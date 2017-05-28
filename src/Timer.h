#ifndef TIMER_H_
#define TIMER_H_

#include "GPIO.h"
#include "inner/__Timer.h"
#include <stdint.h>
#include <stdbool.h>
#include "DataStructures/src/inverted_priority_queue.h"
#include "Misc/src/definitions.h"

void (*Periodic_Task)(void);
// TODO: move to .c
int prevTimerValue, curretTimerValue, nextTimerValue;

//TODO:remove this
#define NVIC_R 0xE000E000
#define PRI4 0x410
#define EN0 0x100

#define __TIMER_MODULE_NUMBER(timer_module) (byte)timer_module
#define __TIMER_COUNTER(timer_module) (byte)(timer_module >> BYTE_LENGTH)
#define __TIMER_PORT(timer_module) (byte)(timer_module >> (BYTE_LENGTH * 2))
#define __TIMER_PIN(timer_module) (byte)(timer_module >> (BYTE_LENGTH * 3))

// timer_num, counter, PORT, PIN
typedef enum TIMER_MODULE_t
{
    TIMER0_PORTB = 0x06010000,
    TIMER0_PORTF = 0x00050000,
    TIMER1_PORTB = 0x04010001,
    TIMER1_PORTF = 0x02050001,
    TIMER2_PORTB = 0x00010002,
    TIMER2_PORTF = 0x04050002,
    TIMER3_PORTB = 0x02010003,
    TIMER4_PORTC = 0x00030004,
    TIMER5_PORTC = 0x02030005,
} TIMER_MODULE_t;

void Timer_init(TIMER_MODULE_t timer_module, __Timer_Mode mode, __Timer_Count_Dir dir,  uint32_t value);
void Timer_start(TIMER_MODULE_t timer_module);
bool Timer_isDone(TIMER_MODULE_t timer_module);
void Timer_handleDelayedStart( int delay );

void RTC_init(TIMER_MODULE_t timer_module,  uint32_t value);
bool RTC_isDone(TIMER_MODULE_t timer_module);

void Timer_SysTimer_init(int delay);
void Timer_sysTimer_start();
bool Timer_sysTimer_isDone();
void Timer_sysTimer_reset(int newDelay);
void Timer_sysTimer_stop();
bool Timer_sysTimer_isWorking();
uint32_t Timer_sysTimer_getCurrentTicks();
uint32_t Timer_sysTimer_getMaxTicks();
uint32_t Timer_sysTimer_getCurrentDelay();

#endif //TIMER_H_
