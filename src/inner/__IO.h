#ifndef INNER_IO_H_
#define INNER_IO_H_

#include <stdbool.h>
#include "Misc/src/definitions.h"

#define REG_VALUE(address) (*((volatile unsigned long *)( address )))
#define IO_REG(port_address, offset) REG_VALUE(port_address + offset)
#define __GET_PORT(PORT_PIN) (unsigned char)(PORT_PIN)
#define __GET_PIN(PORT_PIN) (unsigned char)(PORT_PIN >> BYTE_LENGTH)
#define __GET_PIN_NUM(port_pin) ( __GET_PORT(port_pin) * __PINS_PER_PORT + __GET_PIN(port_pin) )
#define __PINS_TO_BITS(pins) (1 << pins)
#define BUSY false
#define FREE true
#define __PORTS_NUM     6
#define __PINS_PER_PORT 8

enum __IO_PROPERTIES
{
    __IO_DATA  = 0x3FC,
    __IO_DIRECTION   = 0x400,
    __IO_ALTERNATIVE_FUNC_SEL = 0x420,
    __IO_PULL_UP   = 0x510,
    __IO_PULL_DOWN   = 0x514,
    __IO_DIGITAL_ENABLE   = 0x51C,
    __IO_LOCK  = 0x520,
    __IO_COMMIT    = 0x524,
    __IO_ANALOG_MODLE_SEL = 0x528,
    __IO_PORT_CONTROL  = 0x52C,
};

static const unsigned long __IO_PORTS_ADDR [] = {
    0x40004000, // PORTA
    0x40005000, // PORTB
    0x40006000, // PORTC
    0x40007000, // PORTD
    0x40024000, // PORTE
    0x40025000, // PORTF
};

typedef enum __PORT { __PORT_A, __PORT_B, __PORT_C, __PORT_D, __PORT_E, __PORT_F } __PORT;

typedef enum __PIN {
    __PIN_0, __PIN_1, __PIN_2, __PIN_3, __PIN_4, __PIN_5, __PIN_6, __PIN_7 } __PIN;

#endif // INNER_IO_H_
