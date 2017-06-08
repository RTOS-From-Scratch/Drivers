#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>
#include "Misc/src/definitions.h"

PUBLIC
    void delay_msec( uint16_t msec );
    void delay_usec( uint16_t usec );
    uint32_t get_current_ticks();
    uint32_t ticks_to_msec(uint32_t ticks);
    uint32_t msec_to_ticks(uint16_t msec);

PRIVATE
    void __time_init();

#endif // TIME_H_
