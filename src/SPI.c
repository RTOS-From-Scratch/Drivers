#include "SPI.h"
#include "inner/__IO.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define __SPI_MODULE_NUMBER(spi_module) ((byte)spi_module)
#define __SPI_CLK_PORT(spi_module) ((byte)(spi_module >> BYTE_LENGTH))
#define __SPI_CLK_PIN(spi_module) ((byte)(spi_module >> (BYTE_LENGTH * 2)))
#define __SPI_PORT(spi_module) __SPI_CLK_PORT(spi_module)
#define __SPI_PIN(spi_module) __SPI_CLK_PIN(spi_module)

#define __SPI_MODULES_NUM 5



// TXRX(DIR|BIT_SPEC) <- RX(DIR|BIT_SPEC) <- TX(DIR|BIT_SPEC)
// each 4 bits represent one configuration for a mode
byte __SPI_CONFIG[] = { 0xBB, 0x73, 0xBF };
byte __SPI_PORTF_CONFIG[] = { 0xBB, 0x73, 0xBF };

#define __SPI_CONFIG(mode) ( __SPI_CONFIG[mode] & 0xf )
#define __SPI_DIR_CONFIG(mode) ( (__SPI_CONFIG[mode] & 0xf0) >> 4 )
#define __SPI_PORTF_CONFIG(mode) ( __SPI_CONFIG[mode] & 0xf )
#define __SPI_PORTF_DIR_CONFIG(mode) ( (__SPI_CONFIG[mode] & 0xf0) >> 4 )



#define __PCTL_SPI_MODULE_3 ( GPIO_PCTL_PD0_SSI3CLK | GPIO_PCTL_PD1_SSI3FSS | \
                              GPIO_PCTL_PD2_SSI3RX  | GPIO_PCTL_PD3_SSI3TX )
#define __PCTL_SPI_MODULE_3_CLR ( ~( GPIO_PCTL_PD0_M | GPIO_PCTL_PD1_M | \
                                  GPIO_PCTL_PD2_M | GPIO_PCTL_PD3_M ) )
// all other Modules have the same encode as SSI0
#define __PCTL_SPI_MODULES  ( GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA3_SSI0FSS | \
                              GPIO_PCTL_PA4_SSI0RX  | GPIO_PCTL_PA5_SSI0TX )
#define __PCTL_SPI_MODULES_CLR  ( ~( GPIO_PCTL_PA2_M | GPIO_PCTL_PA3_M | \
                                    GPIO_PCTL_PA4_M | GPIO_PCTL_PA5_M ) )

#define __SPI_CLK_PIN_INDEX 0
#define __SPI_Fss_PIN_INDEX 1
#define __SPI_Rx_PIN_INDEX  2
#define __SPI_Tx_PIN_INDEX  3

#define __SPI_PORTF_CLK_PIN_INDEX 2
#define __SPI_PORTF_Fss_PIN_INDEX 3
#define __SPI_PORTF_Rx_PIN_INDEX  0
#define __SPI_PORTF_Tx_PIN_INDEX  1

#define __SPI_MASTER_IO_DIR (__SPI_CLK_PIN_INDEX | __SPI_Fss_PIN_INDEX)
#define __SPI_MASTER_WITH_WRITE_IO_DIR (__SPI_CLK_PIN_INDEX | __SPI_Fss_PIN_INDEX | __SPI_Tx_PIN_INDEX)
#define __SPI_PORTF_MASTER_IO_DIR (__SPI_PORTF_CLK_PIN_INDEX | __SPI_PORTF_Fss_PIN_INDEX)
#define __SPI_PORTF_MASTER_WITH_WRITE_IO_DIR \
            (__SPI_PORTF_CLK_PIN_INDEX | __SPI_PORTF_Fss_PIN_INDEX | __SPI_PORTF_Tx_PIN_INDEX)

// replace first byte by the second one
// TODO: test this algorithm
#define __SPI_CONVERT_INTO_PORTF_CONFIG(old_config) \
                (((old_config & 0b1100) >> 2) + ((old_config & 0b0011) << 2))

#define SPI_MODULE_3 3

#define __SPI_MODE_WRITE_ONLY   0b1011
#define __SPI_MODE_READ_ONLY    0b0011
#define __SPI_MODE_READ_WRITE   0b1011

enum __SPI_Properties_t
{
    __SPI_CONTROL_0             = 0x0,
    __SPI_CONTROL_1             = 0x4,
    __SPI_DATA                  = 0x8,
    __SPI_STATUS                = 0xC,
    __SPI_CLK_PRESCALE          = 0x10,
    __SPI_INTERRUPT_MASK        = 0x14,
    __SPI_RAW_INTERRUPT_MASK    = 0x18,
    __SPI_MASKED_INTERRUPT_MASK = 0x1C,
    __SPI_INTERRUPT_CLEAR       = 0x20,
    __SPI_DMA_CONTROL           = 0x24,
    __SPI_CLK_CONFIGURATION     = 0xFC8
};

static const unsigned long __SPI_MODULES_ADDR[] = {
    0x40008000,         // SPI MODULE 0
    0x40009000,         // SPI MODULE 1 PORT F, PORT D
    0x4000A000,         // SPI MODULE 2
    0x4000B000          // SPI MODULE 3
};

// TODO: They are only 4!
#define __SPI_MODULES_NUM 5
// save the pins used by the current used SPIs
byte __SPI_pinsUsed[__SPI_MODULES_NUM] = { 0 };

void SPI_initAsMaster(SPI_t spi_module, SPI_SPEED speed , SPI_MODE_t mode)
{
    // FIXME: critical section
    byte port = __SPI_PORT(spi_module);
    byte module_number = __SPI_MODULE_NUMBER(spi_module);
    byte bits_specific;
    byte bits_specific_complemented;
    unsigned long base_address = __SPI_MODULES_ADDR[module_number];
    unsigned long PCTL_config;
    unsigned long DIR_config;

/*************************** Clock configuration ***************************/
    // TODO: check for clk stabilizations
    SYSCTL_RCGCGPIO_R |= 1 << port;
    SYSCTL_RCGCSSI_R  |= 1 << module_number;
/***************************************************************************/

/***************************** IO configuration ****************************/
    if( port is PORT_F )
    {
        bits_specific = __SPI_PORTF_CONFIG(mode) << __SPI_PIN(spi_module);
        DIR_config = __SPI_PORTF_DIR_CONFIG(mode) << __SPI_PIN(spi_module);

        // unlock pins
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_LOCK ) = 0x4C4F434B;
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_COMMIT ) |= bits_specific;
    }

    else
    {
        bits_specific = __SPI_CONFIG(mode) << __SPI_PIN(spi_module);
        DIR_config = __SPI_DIR_CONFIG(mode) << __SPI_PIN(spi_module);
    }

    bits_specific_complemented = ~bits_specific;

    // save pins used by the SPI
    __SPI_pinsUsed[module_number] = bits_specific;

    if( module_number == SPI_MODULE_3 )
    {
        PCTL_config = __PCTL_SPI_MODULE_3;
        IO_REG(__IO_PORTS_ADDR[port], __IO_PORT_CONTROL) &= __PCTL_SPI_MODULE_3_CLR;
    }

    else
    {
        PCTL_config = __PCTL_SPI_MODULES;
        IO_REG(__IO_PORTS_ADDR[port], __IO_PORT_CONTROL) &= __PCTL_SPI_MODULES_CLR;
    }

    // enable alternative functions
    IO_REG(__IO_PORTS_ADDR[port], __IO_PORT_CONTROL) |= PCTL_config;
    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) |= bits_specific;
    // direction
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIRECTION) =
            ( IO_REG(__IO_PORTS_ADDR[port], __IO_DIRECTION) & bits_specific_complemented ) | DIR_config;
    // Digital enable
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE)   |= bits_specific;
    // disable analog
    IO_REG(__IO_PORTS_ADDR[port], __IO_ANALOG_MODLE_SEL) &= bits_specific_complemented;
/***************************************************************************/

/**************************** SPI configuration ****************************/
    // disable SPI for configuration and configure it as Master
    IO_REG(base_address, __SPI_CONTROL_1) &= ~(SSI_CR1_SSE | SSI_CR1_MS);
    // for testing, where Tx send to Rx of the same module
    // IO_REG(base_address, __SPI_CONTROL_1) |= SSI_CR1_LBM;

    // use system clk
    IO_REG(base_address, __SPI_CLK_CONFIGURATION) = 0x0;

    // Prescale
    IO_REG(base_address, __SPI_CLK_PRESCALE) =
            (IO_REG(base_address, __SPI_CLK_PRESCALE) & ~SSI_CPSR_CPSDVSR_M) |
            __SPI_SPEED_CONST_CLK_PRESCALE;

    // config the SPI clk speed
    // CLK polarity = 0 and phase = 0
    // 8 bit data width
    // frame format is FreeScale SPI
    IO_REG(base_address, __SPI_CONTROL_0) =
            (IO_REG(base_address, __SPI_CONTROL_0) & 0x0) |
            ( speed | SSI_CR0_DSS_8 | SSI_CR0_FRF_MOTO );

    // enable SPI
    IO_REG(base_address, __SPI_CONTROL_1)    |= SSI_CR1_SSE;
/***************************************************************************/
}

void SPI_write( SPI_t spi_module, byte data )
{
    unsigned long spi_module_address = __SPI_MODULES_ADDR[__SPI_MODULE_NUMBER(spi_module)];

    // poll until the fifo has a space for new data
    while( (IO_REG(spi_module_address, __SPI_STATUS) & SSI_SR_TNF) == 0 );
    IO_REG(spi_module_address, __SPI_DATA) = data;
}

byte SPI_read( SPI_t spi_module )
{
    unsigned long spi_module_address = __SPI_MODULES_ADDR[__SPI_MODULE_NUMBER(spi_module)];

    while( (IO_REG(spi_module_address, __SPI_STATUS) & SSI_SR_RNE) == 0 );
    // FIXME: what if it's configured to use 16 bits ?
    return IO_REG(spi_module_address, __SPI_DATA) & 0xFF;
}

void SPI_deinit( SPI_t spi_module )
{
/********************************* free pins *********************************/
//    byte pins;

//    if( mode is SPI_MODE_Tx )
//        pins = (__SPI_MODE_WRITE_ONLY << __SPI_PIN(spi_module));
//    else if( mode is SPI_MODE_Rx )
//        pins = (__SPI_MODE_READ_ONLY  << __SPI_PIN(spi_module));
//    // both Rx and Tx
//    else
//        pins = (__SPI_MODE_READ_WRITE << __SPI_PIN(spi_module));

//    if( __SPI_PORT(spi_module) is PORT_F )
//        pins = __SPI_CONVERT_INTO_PORTF_CONFIG(pins);

//    __IO_setPinsFree(__SPI_PORT(spi_module), pins);
/*****************************************************************************/






    // FIXME: critical section
    byte port = __SPI_PORT(spi_module);
    byte module_num = __SPI_MODULE_NUMBER(spi_module);
/*********************************** SPI ***********************************/
    // disable SPI module
    IO_REG(__SPI_MODULES_ADDR[__SPI_MODULE_NUMBER(spi_module)], __SPI_CONTROL_1) &= ~SSI_CR1_SSE;

    // disable SPI CLK
    SYSCTL_RCGCSSI_R &= ~(1 << __SPI_MODULE_NUMBER(spi_module));
/****************************************************************************/

/*********************************** GPIO ***********************************/
    // disable DEN
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE) &= ~(__SPI_pinsUsed[module_num]);
    // disable AFSEL
    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) &= ~(__SPI_pinsUsed[module_num]);
/****************************************************************************/

/********************************* Interrupt *********************************/
    // disable interrupts if it's enabled
    // TODO: Global interrupt
    // IO_REG(__SPI_MODULES_ADDR[__SPI_MODULE_NUMBER(spi_module)], __SPI_INTERRUPT_MASK) = 0;
/*****************************************************************************/

/********************************* free pins *********************************/
//    __IO_setPinsFree(port, __SPI_pinsUsed[module_num]);
    __SPI_pinsUsed[module_num] = 0;
/*****************************************************************************/
}
