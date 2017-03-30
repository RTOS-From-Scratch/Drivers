#ifndef INNER_IO_H_
#define INNER_IO_H_

#include "Misc/src/definitions.h"

#define REG_VALUE(address) (*((volatile unsigned long *)( address )))
#define IO_REG(port_address, offset) REG_VALUE(port_address + offset)
#define __PORT(PORT_PIN) (unsigned char)(PORT_PIN)
#define __PIN(PORT_PIN) (unsigned char)(PORT_PIN >> BYTE_LENGTH)

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

typedef enum PORT { PORT_A, PORT_B, PORT_C, PORT_D, PORT_E, PORT_F } PORT;

typedef enum PIN {
    PIN_0, PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7 } PIN;

#endif // INNER_IO_H_
