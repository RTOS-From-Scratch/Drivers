#include "GPIO.h"
#include "tm4c123gh6pm.h"
#include "inner/inner_IO.h"

#define is ==
#define is_not !=
#define GPIO_PORTA_INT 0
#define GPIO_PORTB_INT 1
#define GPIO_PORTC_INT 2
#define GPIO_PORTD_INT 3
#define GPIO_PORTE_INT 4
#define GPIO_PORTF_INT 30

enum GPIO_PROPERTIES
{
    GPIO_DATA  = 0x3FC,
    GPIO_DIR   = 0x400,
    GPIO_AFSEL = 0x420,
    GPIO_PUR   = 0x510,
    GPIO_PDR   = 0x514,
    GPIO_DEN   = 0x51C,
    GPIO_LOCK  = 0x520,
    GPIO_CR    = 0x524,
    GPIO_AMSEL = 0x528,
    GPIO_PCTL  = 0x52C,
};

static void GPIO_PCTL_Configuration( uint32_t PCTL_addr, HEX pins );
static void GPIO_pinModeCore( PORTS port, uint32_t bit_specific, uint32_t bit_specific_complemented, PIN_MODES mode );

void GPIO_pinMode( PORTS port, PINS pin, PIN_MODES mode )
{
	uint32_t bit_specific = (1 << pin);
	GPIO_pinModeCore( port, bit_specific, ~bit_specific, mode );

	// PCTL
	REG_VALUE(PORTS_ADDR[port] + GPIO_PCTL) &= ~( 0xF << (pin * 4) );
}

// TODO: didn't finished yet
void GPIO_pinsMode( PORTS port, HEX pins, PIN_MODES mode )
{
	GPIO_pinModeCore( port, pins, ~pins, mode );

	// PCTL
	GPIO_PCTL_Configuration( PORTS_ADDR[port] + GPIO_PCTL, pins );
}

void GPIO_PCTL_Configuration( uint32_t PCTL_addr, HEX pins )
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
}

void GPIO_pinModeCore( PORTS port, uint32_t bit_specific, uint32_t bit_specific_complemented, PIN_MODES mode )
{
	uint32_t port_addr;

	port_addr = PORTS_ADDR[ port ];

	// enable clock for this port
	SYSCTL_RCGCGPIO_R |= (1 << port);

	// unlock pins
	// TODO: we need to handle this only for
	// F0, D7, C0, C1, C2, C3
	REG_VALUE( port_addr + GPIO_LOCK ) = 0x4C4F434B;
	REG_VALUE( port_addr + GPIO_CR ) |= bit_specific;

	// DEN
	REG_VALUE(port_addr + GPIO_DEN) |= bit_specific;

	// AMSEL
	REG_VALUE(port_addr + GPIO_AMSEL) &= bit_specific_complemented;

	// AFSEL
	REG_VALUE(port_addr + GPIO_AFSEL) &= bit_specific_complemented;

	// DIR
	switch( mode )
	{
		case IN:
			REG_VALUE(port_addr + GPIO_DIR) &= bit_specific_complemented;
			break;

		case OUT:
			REG_VALUE(port_addr + GPIO_DIR) |= bit_specific;
			break;

		case IN_PULLUP:
			REG_VALUE(port_addr + GPIO_PUR) |= bit_specific;
			REG_VALUE(port_addr + GPIO_DIR)  &= bit_specific_complemented;
			break;

		case IN_PULLDOWN:
			REG_VALUE(port_addr + GPIO_PDR) |= bit_specific;
			REG_VALUE(port_addr + GPIO_DIR)  &= bit_specific_complemented;
			break;
	}
}

void GPIO_pinWrite( PORTS port, PINS pin, PIN_STATE state )
{
	// TODO: check if the clock is working on that port
//	assert( (SYSCTL_RCGCGPIO_R & ON) is ON );

	if( state is HIGH )
		REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) |= ( 1 << pin );

	else if ( state is LOW )
		REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) &= ~( 1 << pin );
}

PIN_STATE GPIO_pinRead( PORTS port, PINS pin )
{
	// TODO: check if the clock is working on that port

	uint32_t state;

	state = REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) & (1 << pin);

	return state;
}

void GPIO_pinsWrite( PORTS port, HEX pins, HEX state )
{
	// TODO: check if the clock is working on that port
//	assert( (SYSCTL_RCGCGPIO_R & ON) is ON );

//	if( state is HIGH )
//		REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) |= ( 1 << pin );

//	else if ( state is LOW )
//		REG_VALUE( PORTS_ADDR[ port ] + GPIO_DATA ) &= ~( 1 << pin );
}

HEX GPIO_pinsRead( PORTS port, HEX pins )
{

}

void GPIO_ISR_edges( PORTS port, PINS pin, GPIO_ISR_EDGE ISR_edge, uint8_t priority, bool enable, void(*run)() )
{
	// TODO: check if the clock is working on that port
	// TODO: check if it has level configuration
	// Check if act as GPIO

	// enable ISR trigger as egde
	REG_VALUE( PORTS_ADDR[ port ] + GPIOIS_OFFSET ) &= ~(1 << pin);

	if( ISR_edge is Edge_FALLING )
		REG_VALUE( PORTS_ADDR[ port ] + GPIOIEV_OFFSET ) &= ~(1 << pin);
	else if(ISR_edge is Edge_RISING)
		REG_VALUE( PORTS_ADDR[ port ] + GPIOIEV_OFFSET ) |= (1 << pin);
	else if( ISR_edge is Edge_Both )
		REG_VALUE( PORTS_ADDR[ port ] + GPIOIBE_OFFSET ) |= (1 << pin);

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
	REG_VALUE( PORTS_ADDR[ port ] + GPIOIS_OFFSET ) |= (1 << pin);

	if( ISR_level is LEVEL_LOW )
		REG_VALUE( PORTS_ADDR[ port ] + GPIOIEV_OFFSET ) &= ~(1 << pin);
	else if( ISR_level is LEVEL_HIGH )
		REG_VALUE( PORTS_ADDR[ port ] + GPIOIEV_OFFSET ) |= (1 << pin);

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
	REG_VALUE( PORTS_ADDR[ port ] + GPIOIM_OFFSET ) |= (1 << pin);
}

void GPIO_ISR_disable( PORTS port, PINS pin )
{
	if( port is_not PORT_F )
		NVIC_DIS0_R |= port;
	else if( port is PORT_F )
		NVIC_DIS0_R |= GPIO_PORTF_INT;

	// enable masking
	REG_VALUE( PORTS_ADDR[ port ] + GPIOIM_OFFSET ) &= ~(1 << pin);
}

bool GPIO_ISR_getStatus( PORTS port )
{
	if( port is_not PORT_F )
		return (NVIC_DIS0_R & port) is_not 0 ? false : true;
	else if( port is PORT_F )
		return (NVIC_DIS0_R & GPIO_PORTF_INT) is_not 0 ? false : true;

	else return false;
}
