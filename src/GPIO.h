#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "IO.h"

#define HEX uint32_t

enum GPIO_ISR_EDGE {
    Edge_FALLING,
    Edge_RISING,
    Edge_Both,
};

enum GPIO_ISR_LEVEL {
    LEVEL_HIGH,
    LEVEL_LOW,
};

typedef enum GPIO_ISR_EDGE GPIO_ISR_EDGE;
typedef enum GPIO_ISR_LEVEL GPIO_ISR_LEVEL;

// functions
void GPIO_pinMode( PORT_PIN, PIN_MODES mode );
void GPIO_pinsMode( PORTS port, HEX pins, PIN_MODES mode );
void GPIO_pinWrite( PORT_PIN, PIN_STATE state );
PIN_STATE GPIO_pinRead( PORT_PIN );
void GPIO_pinsWrite( PORTS port, HEX pins, HEX states );
HEX GPIO_pinsRead( PORTS port, HEX pins );

void GPIO_ISR_edges( PORT_PIN, GPIO_ISR_EDGE ISR_edge, uint8_t priority, bool enable, void(*run)() );
void GPIO_ISR_levels( PORT_PIN, GPIO_ISR_LEVEL ISR_level, uint8_t priority, bool enable, void(*run)() );
void GPIO_ISR_enable(PORT_PIN);
void GPIO_ISR_disable(PORT_PIN);
bool GPIO_ISR_getStatus(PORTS port);

#endif // GPIO_H_
