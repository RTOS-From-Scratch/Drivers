#include "GPIO.h"
#include "tm4c123gh6pm.h"
#include "inner/__IO.h"
#include "Misc/src/assert.h"
#include "inner/__ISR_vectortable.h"
#include <stddef.h>
#include "driver.h"
#include "ISR_ctrl.h"

// FIXME: fix this disaster
#define GPIO_PORTA_INT 0
#define GPIO_PORTB_INT 1
#define GPIO_PORTC_INT 2
#define GPIO_PORTD_INT 3
#define GPIO_PORTE_INT 4
#define GPIO_PORTF_INT 30

enum __GPIO_Properties {
    __GPIO_INTERRUPT_SENSE         = 0x404,
    __GPIO_INTERRUPT_BOTH_EDGES    = 0x408,
    __GPIO_INTERRUPT_EVENT         = 0x40C,
    __GPIO_INTERRUPT_MASK          = 0x410,
    __GPIO_RAW_INTERRUPT_STATUS    = 0x414,
    __GPIO_MASKED_INTERRUPT_STATUS = 0x418,
    __GPIO_INTERRUPT_CLEAR         = 0x41C,
};

struct GPIO_Driver {
    uint16_t port_pin;
};

static const GPIO_Driver GPIOs[ __PORTS_NUM * __PINS_PER_PORT ] = {
    { A0 },{ A1 },{ A2 },{ A3 },{ A4 },{ A5 },{ A6 },{ A7 },
    { B0 },{ B1 },{ B2 },{ B3 },{ B4 },{ B5 },{ B6 },{ B7 },
    { C0 },{ C1 },{ C2 },{ C3 },{ C4 },{ C5 },{ C6 },{ C7 },
    { D0 },{ D1 },{ D2 },{ D3 },{ D4 },{ D5 },{ D6 },{ D7 },
    { E0 },{ E1 },{ E2 },{ E3 },{ E4 },{ E5 },{ E6 },{ E7 },
    { F0 },{ F1 },{ F2 },{ F3 },{ F4 },{ F5 },{ F6 },{ F7 },
};

typedef void(*__ISR_handler)();
static __ISR_handler __GPIO_ISR_Handlers[ __PORTS_NUM ];
static void __GPIO_ISR_handler();

void GPIO_init( GPIO_Driver *gpio, PIN_MODE mode , bool autoEnable )
{
    // disable interrupt
    // critical section
//    ISR_disable();

    byte port = __GET_PORT(gpio->port_pin);
    byte pin  = __GET_PIN(gpio->port_pin);
    byte bit_specific = 1 << pin;
    byte bit_specific_complemented = ~bit_specific;

    uint32_t port_addr = __IO_PORTS_ADDR[port];

    // enable clock for this port
    SYSCTL_RCGCGPIO_R |= (1 << port);

    // unlock pins
    // F0, D7, C0, C1, C2, C3
    if( (IO_REG( port_addr, __IO_COMMIT ) & bit_specific) == 0 )
    {
        IO_REG( port_addr, __IO_LOCK ) = 0x4C4F434B;
        IO_REG( port_addr, __IO_COMMIT ) |= bit_specific;
    }

    // DIR
    switch( mode )
    {
        case PIN_MODE_INPUT:
            IO_REG(port_addr, __IO_DIRECTION) &= bit_specific_complemented;
            break;

        case PIN_MODE_OUTPUT:
            IO_REG(port_addr, __IO_DIRECTION) |= bit_specific;
            break;

        case PIN_MODE_INPUT_PULLUP:
            IO_REG(port_addr, __IO_PULL_UP) |= bit_specific;
            IO_REG(port_addr, __IO_DIRECTION) &= bit_specific_complemented;
            break;

        case PIN_MODE_INPUT_PULLDOWN:
            IO_REG(port_addr, __IO_PULL_DOWN) |= bit_specific;
            IO_REG(port_addr, __IO_DIRECTION)  &= bit_specific_complemented;
            break;
    }

    // AFSEL
    IO_REG(port_addr, __IO_ALTERNATIVE_FUNC_SEL) &= bit_specific_complemented;

    // PCTL
    IO_REG(__IO_PORTS_ADDR[port], __IO_PORT_CONTROL) &= ~( 0xF << (pin * 4) );

    // AMSEL
    IO_REG(port_addr, __IO_ANALOG_MODLE_SEL) &= bit_specific_complemented;

    // DEN
    if( autoEnable is true )
        IO_REG(port_addr, __IO_DIGITAL_ENABLE) |= bit_specific;

    // re-enable interrupts
//    ISR_enable();
}

void GPIO_enable( GPIO_Driver *gpio )
{
    byte port = __GET_PORT(gpio->port_pin);
    byte pin = __GET_PIN(gpio->port_pin);

    // DEN
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE) |= 1 << pin;
}

void GPIO_disable( GPIO_Driver *gpio )
{
    byte port = __GET_PORT(gpio->port_pin);
    byte pin = __GET_PIN(gpio->port_pin);

    // DEN
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE) &= ~(1 << pin);
}

void GPIO_ISR_init( GPIO_Driver *gpio,
                    PIN_MODE pin_mode,
                    GPIO_ISR_MODE ISR_mode,
                    ISR_PRIORITY priority,
                    void(*ISR_handler)(),
                    bool autoEnable )
{
    // FIXME: make sure that this GPIO has not been initiated before.
    GPIO_init( gpio, pin_mode, false );

    byte port = __GET_PORT(gpio->port_pin);
    intptr_t port_addr = __IO_PORTS_ADDR[port];
    byte config = 1 << __GET_PIN(gpio->port_pin);
    byte vectorNumber;

    switch( ISR_mode )
    {
        case(GPIO_ISR_MODE_Edge_FALLING):
            IO_REG(port_addr, __GPIO_INTERRUPT_SENSE) &= ~config;
            IO_REG(port_addr, __GPIO_INTERRUPT_EVENT) &= ~config;
            break;

        case(GPIO_ISR_MODE_Edge_RISING):
            IO_REG(port_addr, __GPIO_INTERRUPT_SENSE) &= ~config;
            IO_REG(port_addr, __GPIO_INTERRUPT_EVENT) |= config;
            break;

        case(GPIO_ISR_MODE_Edge_Both):
            IO_REG(port_addr, __GPIO_INTERRUPT_SENSE)      &= ~config;
            IO_REG(port_addr, __GPIO_INTERRUPT_BOTH_EDGES) |= config;
            break;

        case(GPIO_ISR_MODE_LEVEL_HIGH):
            IO_REG(port_addr, __GPIO_INTERRUPT_SENSE) |= config;
            IO_REG(port_addr, __GPIO_INTERRUPT_EVENT) |= config;
            break;

        case(GPIO_ISR_MODE_LEVEL_LOW):
            IO_REG(port_addr, __GPIO_INTERRUPT_SENSE) |= config;
            IO_REG(port_addr, __GPIO_INTERRUPT_EVENT) &= ~config;
            break;

        default:
            return;
    }

    if( port is_not __PORT_F )
        vectorNumber = ISR_GPIO_PORT_A + port;

    else
        vectorNumber = ISR_GPIO_PORT_F;

    // register the handler to the vector table
    __ISR_register( vectorNumber, __GPIO_ISR_handler );
    __ISR_CTRL_setPriority(vectorNumber, priority);

    // save the user handler function to be called later
    __GPIO_ISR_Handlers[__GET_PORT(gpio->port_pin)] = ISR_handler;

    // unmask the pin
    IO_REG( __IO_PORTS_ADDR[port], __GPIO_INTERRUPT_MASK ) |= (1 << __GET_PIN(gpio->port_pin));

    if( autoEnable )
    {
        // enable ISR
        GPIO_ISR_enable(gpio);
    }
}

void GPIO_ISR_changePriroity( GPIO_Driver *gpio, ISR_PRIORITY newPriority )
{
    byte port = __GET_PORT(gpio->port_pin);
    byte vectorNumber;

    if( port is_not __PORT_F )
        vectorNumber = ISR_GPIO_PORT_A + port;

    else
        vectorNumber = ISR_GPIO_PORT_F;

    __ISR_CTRL_setPriority(vectorNumber, newPriority);
}

void GPIO_ISR_enable( GPIO_Driver *gpio )
{
    byte port = __GET_PORT(gpio->port_pin);
    byte pin = __GET_PIN(gpio->port_pin);

    if( port is_not __PORT_F )
        __ISR_CTRL_enable( ISR_GPIO_PORT_A + port );
    else
        __ISR_CTRL_enable( ISR_GPIO_PORT_F );

    // check if GPIO pin is enabled
    if( (IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE) & (1 << pin)) EQUAL 0 )
        GPIO_enable(gpio);
}

void GPIO_ISR_disable( GPIO_Driver *gpio )
{
    byte port = __GET_PORT(gpio->port_pin);

    if( port is_not __PORT_F )
        __ISR_CTRL_disable( ISR_GPIO_PORT_A + port );
    else
        __ISR_CTRL_disable( ISR_GPIO_PORT_F );
}

void GPIO_write( GPIO_Driver *gpio, PIN_STATE state )
{
    byte port = __GET_PORT(gpio->port_pin);
    byte pin = __GET_PIN(gpio->port_pin);
    unsigned long address = __IO_PORTS_ADDR[port];
    int GPIO_PIN_STATE;
    int digital_ON;

    digital_ON = (1 << pin);
    GPIO_PIN_STATE = IO_REG(address, __IO_DIGITAL_ENABLE) & digital_ON;
    // TODO: check if the clock is working on that port
    ASSERT( GPIO_PIN_STATE is digital_ON );

    if( state is HIGH )
        IO_REG( address, __IO_DATA ) |= ( 1 << pin );

    else if ( state is LOW )
        IO_REG( address, __IO_DATA ) &= ~( 1 << pin );
}

PIN_STATE GPIO_read( GPIO_Driver *gpio)
{
    byte port = __GET_PORT(gpio->port_pin);
    byte pin = __GET_PIN(gpio->port_pin);

    // TODO: check if the clock is working on that port
    uint32_t state;

    state = IO_REG( __IO_PORTS_ADDR[port], __IO_DATA ) & (1 << pin);

    return state;
}

const GPIO_Driver* __GPIO_getPin( PORT_PIN port_pin )
{
    byte pin_num = __GET_PIN_NUM(port_pin);
    const GPIO_Driver* driver = &GPIOs[pin_num];

    // call Driver APIs to check pin availability.
    if( __Driver_isPinAvailable(driver->port_pin) is false )
        return NULL;

    // call Driver APIs to mark pin busy.
    __Driver_setPinBusy( driver->port_pin );

    return &GPIOs[__GET_PIN_NUM(port_pin)];
}

bool __GPIO_isAvailable( PORT_PIN port_pin )
{
    const GPIO_Driver* driver = &GPIOs[__GET_PIN_NUM(port_pin)];

    return __Driver_isPinAvailable(driver->port_pin);
}

void GPIO_deinit( GPIO_Driver *gpio )
{
    // disable interrupt
    // critical section
//    ISR_disable();

    byte port = __GET_PORT(gpio->port_pin);
    byte pin = __GET_PIN(gpio->port_pin);
    byte bit_specific_complemented = ~(1 << pin);

/*********************************** GPIO ***********************************/
    GPIO_disable(gpio);

    // remove PULLUP and PULLDOWN
    IO_REG(__IO_PORTS_ADDR[port], __IO_PULL_UP)   &= bit_specific_complemented;
    IO_REG(__IO_PORTS_ADDR[port], __IO_PULL_DOWN) &= bit_specific_complemented;
/****************************************************************************/

/******************************** Interrupt ********************************/
    // disable interrupts if it's enabled
    GPIO_ISR_disable(gpio);
    __GPIO_ISR_Handlers[port] = NULL;
/***************************************************************************/

/******************************** free pins *********************************/
//    __Driver_clearSavedConfig(driver);
/****************************************************************************/

    // re-enable interrupts
//    ISR_enable();
}

void __GPIO_ISR_handler()
{
    // identify the port responsable for this interrupt
    byte vectorNumber = NVIC_INT_CTRL_R & NVIC_INT_CTRL_VEC_ACT_M;
    byte port;

    if( vectorNumber <= ISR_GPIO_PORT_E )
        port = vectorNumber - 16;
    else
        port = __PORT_F;

    // acknowledge corresponding pin
    IO_REG(__IO_PORTS_ADDR[port], __GPIO_INTERRUPT_CLEAR) |=
            IO_REG(__IO_PORTS_ADDR[port], __GPIO_MASKED_INTERRUPT_STATUS);

    // call the user-specific function
    __GPIO_ISR_Handlers[port]();
}
