#include "GPIO.h"
#include "tm4c123gh6pm.h"
#include "inner/__IO.h"
#include "inner/__driver.h"
#include "ISR_ctrl.h"

enum __GPIO_Properties {
    __GPIO_INTERRUPT_SENSE         = 0x404,
    __GPIO_INTERRUPT_BOTH_EDGES    = 0x408,
    __GPIO_INTERRUPT_EVENT         = 0x40C,
    __GPIO_INTERRUPT_MASK          = 0x410,
    __GPIO_RAW_INTERRUPT_STATUS    = 0x414,
    __GPIO_MASKED_INTERRUPT_STATUS = 0x418,
    __GPIO_INTERRUPT_CLEAR         = 0x41C,
};

void GPIO_init( Driver *driver, PIN_MODES mode )
{
    // disable interrupt
    // critical section
//    ISR_disable();

    Module module = driver->module;
    byte port = __GET_PORT(module);
    byte pin  = __GET_PIN(module);
    byte bit_specific = 1 << pin;
    byte bit_specific_complemented = ~bit_specific;

    uint32_t port_addr = __IO_PORTS_ADDR[port];

    __Driver_saveConfig( driver, port, bit_specific );

    // enable clock for this port
    SYSCTL_RCGCGPIO_R |= (1 << port);

    // unlock pins
    // F0, D7, C0, C1, C2, C3
    if( (REG_VALUE( port_addr + __IO_COMMIT ) & bit_specific) == 0 )
    {
        REG_VALUE( port_addr + __IO_LOCK ) = 0x4C4F434B;
        REG_VALUE( port_addr + __IO_COMMIT ) |= bit_specific;
    }

    // DIR
    switch( mode )
    {
        case INPUT:
            REG_VALUE(port_addr + __IO_DIRECTION) &= bit_specific_complemented;
            break;

        case OUTPUT:
            REG_VALUE(port_addr + __IO_DIRECTION) |= bit_specific;
            break;

        case INPUT_PULLUP:
            REG_VALUE(port_addr + __IO_PULL_UP) |= bit_specific;
            REG_VALUE(port_addr + __IO_DIRECTION) &= bit_specific_complemented;
            break;

        case INPUT_PULLDOWN:
            REG_VALUE(port_addr + __IO_PULL_DOWN) |= bit_specific;
            REG_VALUE(port_addr + __IO_DIRECTION)  &= bit_specific_complemented;
            break;
    }

    // AFSEL
    REG_VALUE(port_addr + __IO_ALTERNATIVE_FUNC_SEL) &= bit_specific_complemented;

    // PCTL
    REG_VALUE(__IO_PORTS_ADDR[port] + __IO_PORT_CONTROL) &= ~( 0xF << (pin * 4) );

    // DEN
    REG_VALUE(port_addr + __IO_DIGITAL_ENABLE) |= bit_specific;

    // AMSEL
    REG_VALUE(port_addr + __IO_ANALOG_MODLE_SEL) &= bit_specific_complemented;

    // re-enable interrupts
//    ISR_enable();
}

void GPIO_write( Driver *driver, PIN_STATE state )
{
    // TODO: check if the clock is working on that port
//    assert( (SYSCTL_RCGCGPIO_R & ON) is ON );

//    Module module = driver->data.module;
    Module module = driver->module;

    if( state is HIGH )
        REG_VALUE( __IO_PORTS_ADDR[__GET_PORT(module)] + __IO_DATA ) |= ( 1 << __GET_PIN(module) );

    else if ( state is LOW )
        REG_VALUE( __IO_PORTS_ADDR[__GET_PORT(module)] + __IO_DATA ) &= ~( 1 << __GET_PIN(module) );
}

PIN_STATE GPIO_read( Driver *driver)
{
    // TODO: check if the clock is working on that port

    uint32_t state;
    Module module = driver->module;

    state = REG_VALUE( __IO_PORTS_ADDR[__GET_PORT(module)] + __IO_DATA ) & (1 << __GET_PIN(module));

    return state;
}

void GPIO_deinit( Driver *driver )
{
    // disable interrupt
    // critical section
//    ISR_disable();

    Module module = driver->module;
    byte port = __GET_PORT(module);
    byte pin  = __GET_PIN(module);
    byte bit_specific_complemented = ~(1 << pin);

/*********************************** GPIO ***********************************/
    // disable DEN
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE) &= bit_specific_complemented;
    // remove PULLUP and PULLDOWN
    IO_REG(__IO_PORTS_ADDR[port], __IO_PULL_UP)   &= bit_specific_complemented;
    IO_REG(__IO_PORTS_ADDR[port], __IO_PULL_DOWN) &= bit_specific_complemented;
/****************************************************************************/

/******************************** Interrupt ********************************/
    // disable interrupts if it's enabled
    // TODO: Global interrupt
/***************************************************************************/

/******************************** free pins *********************************/
    __Driver_clearSavedConfig(driver);
/****************************************************************************/

    // re-enable interrupts
//    ISR_enable();
}
