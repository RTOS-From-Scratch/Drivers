#include "SPI.h"
#include "inner/inner_IO.h"
#include "tm4c123gh6pm.h"

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

#define __SPI_CLK_PIN 0
#define __SPI_Fss_PIN 1
#define __SPI_Rx_PIN  2
#define __SPI_Tx_PIN  3

#define __SPI_PORTF_CLK_PIN 2
#define __SPI_PORTF_Fss_PIN 3
#define __SPI_PORTF_Rx_PIN  0
#define __SPI_PORTF_Tx_PIN  1

#define __PCTL_SPI_MODULE_3 GPIO_PCTL_PD0_SSI3CLK | GPIO_PCTL_PD1_SSI3FSS | \
                            GPIO_PCTL_PD2_SSI3RX | GPIO_PCTL_PD3_SSI3TX
// all other Modules have the same encode as SSI0
#define __PCTL_SPI_MODULES  GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA3_SSI0FSS | \
                            GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA5_SSI0TX

#define __SPI_MASTER_IO_DIR (__SPI_CLK_PIN | __SPI_Fss_PIN)
#define __SPI_MASTER_WITH_WRITE_IO_DIR (__SPI_CLK_PIN | __SPI_Fss_PIN | __SPI_Tx_PIN)

/*#define BIT_FILED_SET(data, dest, bit, range) __volatile__ __asm__( \
 *                                                       "mov R0, data\n\t" \
 *                                                       "mov R1, dest\n\t" \
 *                                                       "BFI R1, R0, bit, range\n\t" \
 *                                                       ::);
 */

void SPI_initAsMaster( SPI_MODULE_t, SPI_SPEED speed )
{
    unsigned long bit_specific;
    unsigned long bit_specific_complemented;
    unsigned long PCTL_config;

/*************************** Clock configuration ***************************/
    SYSCTL_RCGCGPIO_R |= 1 << port;
    SYSCTL_RCGCSSI_R  |= 1 << module_num;
    while((SYSCTL_PRSSI_R&SYSCTL_PRSSI_R2) == 0){};
/***************************************************************************/

/***************************** IO configuration ****************************/
    if( port == PORT_F )
    {
        bit_specific = 0b1111 << (pin >> 2);
        bit_specific_complemented = ~bit_specific;
        PCTL_config = __PCTL_SPI_MODULES;
        // unlock pins
        // TODO: we need to handle this only for
        // F0, D7, C0, C1, C2, C3
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_LOCK ) = 0x4C4F434B;
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_CR ) |= bit_specific;
    }

    else
    {
        bit_specific = 0b1111 << pin;
        bit_specific_complemented = ~bit_specific;

        if( module_num == SPI_MODULE_3 )
            PCTL_config = __PCTL_SPI_MODULE_3;
        else
            PCTL_config = __PCTL_SPI_MODULES;
    }

    IO_REG(__IO_PORTS_ADDR[port], __IO_DEN)   |= bit_specific;
    IO_REG(__IO_PORTS_ADDR[port], __IO_AMSEL) &= bit_specific_complemented;
    IO_REG(__IO_PORTS_ADDR[port], __IO_AFSEL) |= bit_specific;
    IO_REG(__IO_PORTS_ADDR[port], __IO_PCTL)  |= PCTL_config;
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIR) =
            ( IO_REG(__IO_PORTS_ADDR[port], __IO_DIR) & bit_specific_complemented ) |
            __SPI_MASTER_WITH_WRITE_IO_DIR;
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
    // CLK polarity and phase
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
    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_CONTROL_1) |= SSI_CR1_SSE;
/***************************************************************************/
}

void SPI_write(SPI_MODULE_t, byte data)
{
    // poll until the fifo has a space for new data
    while( (IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_STATUS) & SSI_SR_TNF) == 0 );

    IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_DATA) = data;
}

byte SPI_read(SPI_MODULE_t)
{
    while( (IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_STATUS) & SSI_SR_RNE) == 0 );

    // FIXME: what if it's configured to use 16 bits ?
    return IO_REG(__SPI_MODULES_ADDR[module_num], __SPI_DATA) & 0xFF;
}
