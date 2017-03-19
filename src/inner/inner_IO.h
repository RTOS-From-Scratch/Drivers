#ifndef INNER_IO_H_
#define INNER_IO_H_

#define REG_VALUE(address) (*((volatile unsigned long *)( address )))
#define IO_REG(port_address, offset) REG_VALUE(port_address + offset)
#define BYTE_LENGTH 8
#define __PORT(PORT_PIN) (unsigned char)(PORT_PIN)
#define __PIN(PORT_PIN) (unsigned char)(PORT_PIN >> BYTE_LENGTH)
#define is ==
#define is_not !=

enum __IO_PROPERTIES
{
    __IO_DATA  = 0x3FC,
    __IO_DIR   = 0x400,
    __IO_AFSEL = 0x420,
    __IO_PUR   = 0x510,
    __IO_PDR   = 0x514,
    __IO_DEN   = 0x51C,
    __IO_LOCK  = 0x520,
    __IO_CR    = 0x524,
    __IO_AMSEL = 0x528,
    __IO_PCTL  = 0x52C,
};

static const unsigned long __IO_PORTS_ADDR [] = {
    0x40004000, // PORTA
    0x40005000, // PORTB
    0x40006000, // PORTC
    0x40007000, // PORTD
    0x40024000, // PORTE
    0x40025000, // PORTF
};

#endif // INNER_IO_H_
