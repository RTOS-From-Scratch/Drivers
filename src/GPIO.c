#include "GPIO.h"
#include "tm4c123gh6pm.h"
#include "inner/inner_IO.h"

//#define GPIO_PORTA_INT 0
//#define GPIO_PORTB_INT 1
//#define GPIO_PORTC_INT 2
//#define GPIO_PORTD_INT 3
//#define GPIO_PORTE_INT 4
//#define GPIO_PORTF_INT 30

enum __GPIO_Properties {
    __GPIO_INTERRUPT_SENSE         = 0x404,
    __GPIO_INTERRUPT_BOTH_EDGES    = 0x408,
    __GPIO_INTERRUPT_EVENT         = 0x40C,
    __GPIO_INTERRUPT_MASK          = 0x410,
    __GPIO_RAW_INTERRUPT_STATUS    = 0x414,
    __GPIO_MASKED_INTERRUPT_STATUS = 0x418,
    __GPIO_INTERRUPT_CLEAR         = 0x41C,
};

//static void GPIO_PCTL_Configuration( uint32_t PCTL_addr, hex_t pins );
//static void GPIO_pinModeCore( PORTS port, long bit_specific, long bit_specific_complemented, PIN_MODES mode );

void GPIO_init( PORT_PIN port_pin, PIN_MODES mode )
{
    byte port = __PORT(port_pin);
    byte pin  = __PIN(port_pin);
    uint32_t bit_specific = 1 << pin;
    uint32_t bit_specific_complemented = ~bit_specific;

    uint32_t port_addr = __PORTS_ADDR[port];

    // enable clock for this port
    SYSCTL_RCGCGPIO_R |= (1 << port);

    // unlock pins
    // F0, D7, C0, C1, C2, C3
    if( (REG_VALUE( port_addr + __IO_CR ) & bit_specific) == 0 )
    {
        REG_VALUE( port_addr + __IO_LOCK ) = 0x4C4F434B;
        REG_VALUE( port_addr + __IO_CR ) |= bit_specific;
    }

    // DEN
    REG_VALUE(port_addr + __IO_DEN) |= bit_specific;

    // AMSEL
    REG_VALUE(port_addr + __IO_AMSEL) &= bit_specific_complemented;

    // AFSEL
    REG_VALUE(port_addr + __IO_AFSEL) &= bit_specific_complemented;

    // DIR
    switch( mode )
    {
        case INPUT:
            REG_VALUE(port_addr + __IO_DIR) &= bit_specific_complemented;
            break;

        case OUTPUT:
            REG_VALUE(port_addr + __IO_DIR) |= bit_specific;
            break;

        case INPUT_PULLUP:
            REG_VALUE(port_addr + __IO_PUR) |= bit_specific;
            REG_VALUE(port_addr + __IO_DIR) &= bit_specific_complemented;
            break;

        case INPUT_PULLDOWN:
            REG_VALUE(port_addr + __IO_PDR) |= bit_specific;
            REG_VALUE(port_addr + __IO_DIR)  &= bit_specific_complemented;
            break;
    }

    // PCTL
    REG_VALUE(__PORTS_ADDR[port] + __IO_PCTL) &= ~( 0xF << (pin * 4) );
}

// TODO: didn't finished yet
/*void GPIO_pinsMode( PORTS port, hex_t pins, PIN_MODES mode )
{
    GPIO_pinModeCore( port, pins, ~pins, mode );

    // PCTL
    GPIO_PCTL_Configuration( __PORTS_ADDR[port] + __IO_PCTL, pins );
}*/

/*void GPIO_PCTL_Configuration( uint32_t PCTL_addr, hex_t pins )
{
    uint8_t pin  = 0x0;

    for( uint8_t bit = 0x1 ; bit <= 0x80 && bit != 0x0; bit = bit << 1 )
    {
        if( (pins & bit) != 0x0 )
        {
            REG_VALUE(PCTL_addr) &= ~( 0xF << (pin * 4) );
        }

        pin++;
    }
}*/

/*void GPIO_pinModeCore( PORTS port, long bit_specific, long bit_specific_complemented, PIN_MODES mode )
{
    uint32_t port_addr;

    port_addr = __PORTS_ADDR[ port ];

    // enable clock for this port
    SYSCTL_RCGCGPIO_R |= (1 << port);

    // unlock pins
    // TODO: we need to handle this only for
    // F0, D7, C0, C1, C2, C3
    REG_VALUE( port_addr + __IO_LOCK ) = 0x4C4F434B;
    REG_VALUE( port_addr + __IO_CR ) |= bit_specific;

    // DEN
    REG_VALUE(port_addr + __IO_DEN) |= bit_specific;

    // AMSEL
    REG_VALUE(port_addr + __IO_AMSEL) &= bit_specific_complemented;

    // AFSEL
    REG_VALUE(port_addr + __IO_AFSEL) &= bit_specific_complemented;

    // DIR
    switch( mode )
    {
        case INPUT:
            REG_VALUE(port_addr + __IO_DIR) &= bit_specific_complemented;
            break;

        case OUTPUT:
            REG_VALUE(port_addr + __IO_DIR) |= bit_specific;
            break;

        case INPUT_PULLUP:
            REG_VALUE(port_addr + __IO_PUR) |= bit_specific;
            REG_VALUE(port_addr + __IO_DIR)  &= bit_specific_complemented;
            break;

        case INPUT_PULLDOWN:
            REG_VALUE(port_addr + __IO_PDR) |= bit_specific;
            REG_VALUE(port_addr + __IO_DIR)  &= bit_specific_complemented;
            break;
    }
}*/

void GPIO_write(PORT_PIN port_pin, PIN_STATE state )
{
    // TODO: check if the clock is working on that port
//    assert( (SYSCTL_RCGCGPIO_R & ON) is ON );

    if( state is HIGH )
        REG_VALUE( __PORTS_ADDR[__PORT(port_pin)] + __IO_DATA ) |= ( 1 << __PIN(port_pin) );

    else if ( state is LOW )
        REG_VALUE( __PORTS_ADDR[__PORT(port_pin)] + __IO_DATA ) &= ~( 1 << __PIN(port_pin) );
}

PIN_STATE GPIO_read(PORT_PIN port_pin)
{
    // TODO: check if the clock is working on that port

    uint32_t state;

    state = REG_VALUE( __PORTS_ADDR[__PORT(port_pin)] + __IO_DATA ) & (1 << __PIN(port_pin));

    return state;
}

/*void GPIO_pinsWrite( PORTS port, hex_t pins, hex_t state )
{
    // TODO: check if the clock is working on that port
//    assert( (SYSCTL_RCGCGPIO_R & ON) is ON );

//    if( state is HIGH )
//        REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) |= ( 1 << pin );

//    else if ( state is LOW )
//        REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) &= ~( 1 << pin );
}*/

/*hex_t GPIO_pinsRead( PORTS port, hex_t pins )
{

}*/

/*void GPIO_ISR_edges( PORTS port, PINS pin, GPIO_ISR_EDGE ISR_edge, uint8_t priority, bool enable, void(*run)() )
{
    // TODO: check if the clock is working on that port
    // TODO: check if it has level configuration
    // Check if act as GPIO

    // enable ISR trigger as egde
    REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_SENSE ) &= ~(1 << pin);

    if( ISR_edge is Edge_FALLING )
        REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_EVENT ) &= ~(1 << pin);
    else if(ISR_edge is Edge_RISING)
        REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_EVENT ) |= (1 << pin);
    else if( ISR_edge is Edge_Both )
        REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_BOTH_EDGES ) |= (1 << pin);

    // Enable interrupt globally

    if( enable is true )
        GPIO_ISR_enable(port, pin);
    else
        GPIO_ISR_disable(port, pin);
}

void GPIO_ISR_levels( PORTS port, PINS pin, GPIO_ISR_LEVEL ISR_level, uint8_t priority, bool enable, void(*run)() )
{
    // TODO: check if the clock is working on that port
    // TODO: check if it has edge configuration
    // Check if act as GPIO

    // enable ISR trigger as level
    REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_SENSE ) |= (1 << pin);

    if( ISR_level is LEVEL_LOW )
        REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_EVENT ) &= ~(1 << pin);
    else if( ISR_level is LEVEL_HIGH )
        REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_EVENT ) |= (1 << pin);

    if( enable is true )
        GPIO_ISR_enable(port, pin);
    else
        GPIO_ISR_disable(port, pin);
}

void GPIO_ISR_enable( PORTS port, PINS pin )
{
    if( port is_not PORT_F )
        NVIC_EN0_R |= port;
    else if( port is PORT_F )
        NVIC_EN0_R |= GPIO_PORTF_INT;

    // disable masking
    REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_MASK ) |= (1 << pin);
}

void GPIO_ISR_disable( PORTS port, PINS pin )
{
    if( port is_not PORT_F )
        NVIC_DIS0_R |= port;
    else if( port is PORT_F )
        NVIC_DIS0_R |= GPIO_PORTF_INT;

    // enable masking
    REG_VALUE( __PORTS_ADDR[ port ] + __GPIO_INTERRUPT_MASK ) &= ~(1 << pin);
}

bool GPIO_ISR_getStatus( PORTS port )
{
    if( port is_not PORT_F )
        return (NVIC_DIS0_R & port) is_not 0 ? false : true;
    else if( port is PORT_F )
        return (NVIC_DIS0_R & GPIO_PORTF_INT) is_not 0 ? false : true;

    else return false;
}*/
