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

//typedef enum PIN_PULL_STATE {
//    PULL_UP,
//    PULL_DOWN
//} PIN_PULL_STATE;

//typedef enum GPIO_ISR_EDGE {
//    Edge_FALLING,
//    Edge_RISING,
//    Edge_Both,
//} GPIO_ISR_EDGE;

//typedef enum GPIO_ISR_LEVEL {
//    LEVEL_HIGH,
//    LEVEL_LOW,
//} GPIO_ISR_LEVEL;

// functions
// TODO: functions of clearing GPIO configurations
void GPIO_init(PORT_PIN port_pin, PIN_MODES mode );
//void GPIO_pinsMode( PORTS port, hex_t pins, PIN_MODES mode );
void GPIO_write( PORT_PIN port_pin, PIN_STATE state );
PIN_STATE GPIO_read( PORT_PIN port_pin );
//void GPIO_pinsWrite( PORTS port, hex_t pins, hex_t states );
//hex_t GPIO_pinsRead( PORTS port, hex_t pins );

//void GPIO_ISR_edges( PORT_PIN, GPIO_ISR_EDGE ISR_edge, uint8_t priority, bool enable, void(*run)() );
//void GPIO_ISR_levels( PORT_PIN, GPIO_ISR_LEVEL ISR_level, uint8_t priority, bool enable, void(*run)() );
//void GPIO_ISR_enable(PORT_PIN);
//void GPIO_ISR_disable(PORT_PIN);
//bool GPIO_ISR_getStatus(PORTS port);

#endif // GPIO_H_
