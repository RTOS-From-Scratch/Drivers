#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include <stdint.h>
#include <stdbool.h>
#include "Misc/src/definitions.h"
#include "../ISR_ctrl.h"

typedef enum ST_MODE {
    ST_MODE_ONE_SHOT,
    ST_MODE_CONTINOUS
} ST_MODE;

void __SysTick_init( uint32_t ticks, void(*run)(), ISR_PRIORITY priority );
bool __SysTick_start( ST_MODE mode );
void __SysTick_stop();
void __SysTick_reset( uint32_t ticks );
bool __SysTick_isInit();
uint32_t __SysTick_getTicks( uint32_t milliSecond );
uint32_t __SysTick_getMilliSecond( uint32_t ticks );

#endif // __SYSTICK_H_
