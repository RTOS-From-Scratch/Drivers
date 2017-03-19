#include "SPI.h"
#include "inner/inner_IO.h"
#include "tm4c123gh6pm.h"

#define __SPI_MODULE_NUMBER(spi_module) ((byte)spi_module)
#define __SPI_CLK_PORT(spi_module) ((byte)(spi_module >> BYTE_LENGTH))
#define __SPI_CLK_PIN(spi_module) ((byte)(spi_module >> (BYTE_LENGTH * 2)))
#define __SPI_PORT(spi_module) __SPI_CLK_PORT(spi_module)
#define __SPI_PIN(spi_module) __SPI_CLK_PIN(spi_module)

#define __PCTL_SPI_MODULE_3 ( GPIO_PCTL_PD0_SSI3CLK | GPIO_PCTL_PD1_SSI3FSS | \
                              GPIO_PCTL_PD2_SSI3RX | GPIO_PCTL_PD3_SSI3TX )
// all other Modules have the same encode as SSI0
#define __PCTL_SPI_MODULES  ( GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA3_SSI0FSS | \
                              GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA5_SSI0TX )

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

#define __SPI_CONVERT_INTO_PORTF_CONFIG(old_config) \
            (((old_config & 0b1100) >> 2) + ((old_config & 0b0011) << 2))

#define PORT_F 5
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

void SPI_initAsMaster(SPI_t spi_module, SPI_SPEED speed , SPI_MODE_t mode)
{
    byte port = __SPI_PORT(spi_module);
    byte module_num = __SPI_MODULE_NUMBER(spi_module);
    unsigned long bit_specific;
    unsigned long bit_specific_complemented;
    unsigned long PCTL_config;
    unsigned long DIR_config;

/*************************** Clock configuration ***************************/
    // check for clk stabilizations
    SYSCTL_RCGCGPIO_R |= 1 << port;
    SYSCTL_RCGCSSI_R  |= 1 << module_num;
/***************************************************************************/

/***************************** IO configuration ****************************/
    if( mode is SPI_MODE_Tx )
    {
        bit_specific = 0b1011 << __SPI_PIN(spi_module);
        DIR_config   = __SPI_MODE_WRITE_ONLY << __SPI_PIN(spi_module);
    }

    else if( mode is SPI_MODE_Rx )
    {
        bit_specific = 0b0111 << __SPI_PIN(spi_module);
        DIR_config   = __SPI_MODE_READ_ONLY << __SPI_PIN(spi_module);
    }

    // Tx/Rx
    else
    {
        bit_specific = 0b1111 << __SPI_PIN(spi_module);
        DIR_config   = __SPI_MODE_READ_WRITE << __SPI_PIN(spi_module);
    }

    if( port is PORT_F )
    {
        bit_specific = __SPI_CONVERT_INTO_PORTF_CONFIG(bit_specific);
        DIR_config   = __SPI_CONVERT_INTO_PORTF_CONFIG(DIR_config);

        // unlock pins
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_LOCK ) = 0x4C4F434B;
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_CR ) |= bit_specific;
    }

    bit_specific_complemented = ~bit_specific;

    if( module_num == SPI_MODULE_3 )
        PCTL_config = __PCTL_SPI_MODULE_3;
    else
        PCTL_config = __PCTL_SPI_MODULES;

    // Digital enable
    IO_REG(__IO_PORTS_ADDR[port], __IO_DEN)   |= bit_specific;
    // disable analog
    IO_REG(__IO_PORTS_ADDR[port], __IO_AMSEL) &= bit_specific_complemented;
    // enable alternative functions
    IO_REG(__IO_PORTS_ADDR[port], __IO_AFSEL) |= bit_specific;
    IO_REG(__IO_PORTS_ADDR[port], __IO_PCTL)  |= PCTL_config;
    // direction
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIR) =
            ( IO_REG(__IO_PORTS_ADDR[port], __IO_DIR) & bit_specific_complemented ) | DIR_config;
/***************************************************************************/

/**************************** SPI configuration ****************************/
    // disable SPI for configuration and configure it as Master
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_1) &= ~(SSI_CR1_SSE | SSI_CR1_MS);
    // for testing, where Tx send to Rx of the same module
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_1) |= SSI_CR1_LBM;

    // use system clk
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CLK_CONFIGURATION) = 0x0;

    // config the SPI clk speed
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CLK_PRESCALE) =
            (IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CLK_PRESCALE) & ~SSI_CPSR_CPSDVSR_M) |
            __SPI_SPEED_CONST_CLK_PRESCALE;
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_0)    = speed;

    // TODO: we need a better idea
    // CLK polarity = 0 and phase = 0
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_0)    &= ~(SSI_CR0_SPO | SSI_CR0_SPH);

    // TODO: we need a better idea
    // 8 bit data width
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_0)    =
            (IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_0) & ~SSI_CR0_DSS_M) | SSI_CR0_DSS_8;

    // TODO: we need a better idea
    // frame format is FreeScale SPI
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_0)    =
            ( IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_0) & ~SSI_CR0_FRF_M) | SSI_CR0_FRF_MOTO;

    // enable SPI
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_1)    |= SSI_CR1_SSE;
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
