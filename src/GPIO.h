#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "IO.h"
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
void GPIO_init(PORT_PIN port_pin, PIN_MODES mode, TaskID id);
void GPIO_write( PORT_PIN port_pin, PIN_STATE state );
PIN_STATE GPIO_read( PORT_PIN port_pin );
void GPIO_deinit( PORT_PIN port_pin, TaskID id );

#endif // GPIO_H_
