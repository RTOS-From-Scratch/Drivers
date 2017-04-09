#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "Misc/src/definitions.h"

typedef enum PIN_STATE {
    LOW,
    HIGH
} PIN_STATE;

typedef enum PIN_MODES {
    INPUT,
    OUTPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN
} PIN_MODES;

// functions
void GPIO_init( Driver *driver, PIN_MODES mode );
void GPIO_write( Driver *driver, PIN_STATE state );
PIN_STATE GPIO_read( Driver *driver );
void GPIO_deinit( Driver *driver );

#endif // GPIO_H_
