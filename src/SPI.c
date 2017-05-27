#include "SPI.h"
#include "inner/__IO.h"
#include "tm4c123gh6pm.h"
#include "Misc/src/definitions.h"
#include <stdint.h>
#include <stddef.h>
#include "PLL.h"

#define __SPI_MODULE_NUMBER(spi_module) ((byte)spi_module)
#define __SPI_CLK_PORT(spi_module) ((byte)(spi_module >> BYTE_LENGTH))
#define __SPI_CLK_PIN(spi_module) ((byte)(spi_module >> (BYTE_LENGTH * 2)))
#define __SPI_PORT(spi_module) __SPI_CLK_PORT(spi_module)
#define __SPI_PIN(spi_module) __SPI_CLK_PIN(spi_module)

// TX | RX | SS | CLK
#define __SPI_CONFIG            0b1111
#define __SPI_DIR               0b1011
#define __SPI_MISO_PIN          0b0100
// SS | CLK | TX | RX
#define __SPI_PORTF_CONFIG      0b1111
#define __SPI_PORTF_DIR         0b1110
#define __SPI_PORTF_MISO_PIN    0b0001

#define __PCTL_SPI_MODULE_3 ( GPIO_PCTL_PD0_SSI3CLK | GPIO_PCTL_PD1_SSI3FSS | \
                              GPIO_PCTL_PD2_SSI3RX  | GPIO_PCTL_PD3_SSI3TX )
#define __PCTL_SPI_MODULE_3_CLR ( ~( GPIO_PCTL_PD0_M | GPIO_PCTL_PD1_M | \
                                  GPIO_PCTL_PD2_M | GPIO_PCTL_PD3_M ) )
// all other Modules have the same encode as SSI0
#define __PCTL_SPI_MODULES  ( GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA3_SSI0FSS | \
                              GPIO_PCTL_PA4_SSI0RX  | GPIO_PCTL_PA5_SSI0TX )
#define __PCTL_SPI_MODULES_CLR  ( ~( GPIO_PCTL_PA2_M | GPIO_PCTL_PA3_M | \
                                    GPIO_PCTL_PA4_M | GPIO_PCTL_PA5_M ) )

#define SPI_MODULE_3 3
#define __SPI_MODULES_NUM 5

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

struct SPI_Driver {
    byte module_num;
    uint16_t CLK;
    uint16_t CS;
    uint16_t Rx;
    uint16_t Tx;
    byte PCTL_encoding;
};

#define PCTL_ENCODING      0b10
#define PCTL_ENCODING_SPI3 0b01

static const SPI_Driver SPIs[__SPI_MODULES_NUM] = {
    { 0, A2, A3, A4, A5 },          // SPI0
    { 1, F2, F3, F0, F1 },          // SPI1_PORTF
    { 2, B4, B5, B6, B7 },          // SPI2
    { 3, D0, D1, D2, D3 },          // SPI3
    { 1, D0, D1, D2, D3 },          // SPI1_PORTD
};

void SPI_initAsMaster( SPI_Driver *spi, SPI_SPEED speed, bool autoEnable )
{
    // FIXME: critical section
//    Module spi_module = driver->module;
    byte port = __GET_PORT(spi->CLK);
    byte bits_specific;
    byte bits_specific_complemented;
    unsigned long base_address = __SPI_MODULES_ADDR[spi->module_num];
    unsigned long DIR_config;

/*************************** Clock configuration ***************************/
    // TODO: check for clk stabilizations
    SYSCTL_RCGCGPIO_R |= 1 << port;
    while((SYSCTL_PRGPIO_R & (1 << port)) == 0){};
    SYSCTL_RCGCSSI_R  |= 1 << spi->module_num;
    while((SYSCTL_PRSSI_R & (1 << spi->module_num)) == 0){};
/***************************************************************************/

/***************************** IO configuration ****************************/
    bits_specific = (1 << __GET_PIN(spi->CLK)) |
                    (1 << __GET_PIN(spi->CS))  |
                    (1 << __GET_PIN(spi->Rx))  |
                    (1 << __GET_PIN(spi->Tx));
    bits_specific_complemented = ~bits_specific;

    DIR_config = (1 << __GET_PIN(spi->CLK)) |
                 (1 << __GET_PIN(spi->CS))  |
                 (1 << __GET_PIN(spi->Tx));

    if( port is __PORT_F )
    {
        // unlock pins
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_LOCK ) = 0x4C4F434B;
        REG_VALUE( __IO_PORTS_ADDR[port] + __IO_COMMIT ) |= bits_specific;
    }

    byte PCTL_encoding = (spi->module_num is SPI3) ?
                         PCTL_ENCODING_SPI3 :
                         PCTL_ENCODING;

    // clear PCTL
    IO_REG(__IO_PORTS_ADDR[port], __IO_PORT_CONTROL) &= ~( (0xF << (__GET_PIN(spi->CLK) * 4)) |
                                                           (0xF << (__GET_PIN(spi->CS) * 4))  |
                                                           (0xF << (__GET_PIN(spi->Rx) * 4))  |
                                                           (0xF << (__GET_PIN(spi->Tx) * 4))  );
    // set PCTL
    IO_REG(__IO_PORTS_ADDR[port], __IO_PORT_CONTROL) += ( (PCTL_encoding << (__GET_PIN(spi->CLK) * 4)) |
                                                          (PCTL_encoding << (__GET_PIN(spi->CS) * 4))  |
                                                          (PCTL_encoding << (__GET_PIN(spi->Rx) * 4))  |
                                                          (PCTL_encoding << (__GET_PIN(spi->Tx) * 4))  );
    // enable alternative functions
    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) |= bits_specific;
    // direction
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIRECTION) =
            ( IO_REG(__IO_PORTS_ADDR[port], __IO_DIRECTION) & bits_specific_complemented ) + DIR_config;
    // disable analog
    IO_REG(__IO_PORTS_ADDR[port], __IO_ANALOG_MODLE_SEL) &= bits_specific_complemented;

    if( autoEnable is true )
        // Digital enable
        IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE)   |= bits_specific;
/***************************************************************************/

/**************************** SPI configuration ****************************/
    // disable SPI for configuration and configure it as Master
    IO_REG(base_address, __SPI_CONTROL_1) &= ~(SSI_CR1_SSE | SSI_CR1_MS);
    // for testing, where Tx send to Rx of the same module
//     IO_REG(base_address, __SPI_CONTROL_1) |= SSI_CR1_LBM;

    // use system clk
    IO_REG(base_address, __SPI_CLK_CONFIGURATION) =
            ( IO_REG(base_address, __SPI_CLK_CONFIGURATION) & ~SSI_CC_CS_M) + SSI_CC_CS_SYSPLL;

    // CLK polarity = 0 and phase = 0
    // 8 bit data width
    // frame format is FreeScale SPI
    IO_REG(base_address, __SPI_CONTROL_0) =( SSI_CR0_DSS_8 | SSI_CR0_FRF_MOTO );

    if( speed is_not SPI_SPEED_NOCONFIG )
    {
        // Prescale
        IO_REG(base_address, __SPI_CLK_PRESCALE) =
                (IO_REG(base_address, __SPI_CLK_PRESCALE) & ~SSI_CPSR_CPSDVSR_M) +
                __SPI_SPEED_CONST_CLK_PRESCALE;

        // config the SPI clk speed
        IO_REG(base_address, __SPI_CONTROL_0) |= speed;
    }

    if( autoEnable is true )
        // enable SPI
        IO_REG(base_address, __SPI_CONTROL_1)    |= SSI_CR1_SSE;
/***************************************************************************/
}

void SPI_enable( SPI_Driver *spi )
{
    unsigned long base_address = __SPI_MODULES_ADDR[spi->module_num];
    unsigned long bits_specific = (1 << __GET_PIN(spi->CLK)) |
                                  (1 << __GET_PIN(spi->CS))  |
                                  (1 << __GET_PIN(spi->Rx))  |
                                  (1 << __GET_PIN(spi->Tx));
    __PORT port = __GET_PORT(spi->CLK);

    // Digital enable
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE)   |= bits_specific;
    // enable SPI
    IO_REG(base_address, __SPI_CONTROL_1)    |= SSI_CR1_SSE;
}

void SPI_disable( SPI_Driver *spi )
{
    unsigned long base_address = __SPI_MODULES_ADDR[spi->module_num];
    unsigned long bits_specific = (1 << __GET_PIN(spi->CLK)) |
                                  (1 << __GET_PIN(spi->CS))  |
                                  (1 << __GET_PIN(spi->Rx))  |
                                  (1 << __GET_PIN(spi->Tx));
    __PORT port = __SPI_PORT(spi->module_num);

    // Digital disable
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE)   &= ~bits_specific;
    // disable SPI
    IO_REG(base_address, __SPI_CONTROL_1)    &= ~SSI_CR1_SSE;
}

void SPI_setClkSpeedManually( SPI_Driver *spi, uint32_t speed )
{
    ClockSpeed sys_clk_speed = PLL_getClockSpeed();
    // calculate the new SCR (SSI Serial Clock Rate)
    byte SCR = ( sys_clk_speed / (speed * __SPI_SPEED_CONST_CLK_PRESCALE) ) - 1;

    // SPI_SPEED is just a shifted SCR to its bit index (SSI_CR0_SCR_S) in SSI_CR0
    SPI_setClkSpeed( spi, SCR << SSI_CR0_SCR_S );
}

void SPI_setClkSpeed( SPI_Driver *spi, SPI_SPEED speed )
{
    uint32_t base_address = __SPI_MODULES_ADDR[spi->module_num];

    // disable for configurations
    IO_REG(base_address, __SPI_CONTROL_1) &= ~(SSI_CR1_SSE);

    // Prescale
    IO_REG(base_address, __SPI_CLK_PRESCALE) =
            (IO_REG(base_address, __SPI_CLK_PRESCALE) & ~SSI_CPSR_CPSDVSR_M) +
            __SPI_SPEED_CONST_CLK_PRESCALE;

    // config the SPI clk speed
    IO_REG(base_address, __SPI_CONTROL_0) =
            (IO_REG(base_address, __SPI_CONTROL_0) & ~SSI_CR0_SCR_M) + speed;

    // enable
    IO_REG(base_address, __SPI_CONTROL_1) |= SSI_CR1_SSE;
}

void SPI_setChipSelectState( SPI_Driver *spi, PIN_STATE state )
{
    // FIXME: check if initiated
    byte port= __GET_PORT(spi->CLK);
    byte pin = __GET_PIN(spi->CS);
    unsigned long address = __IO_PORTS_ADDR[port];

    // check if Chip select pin is digital enabled
    if( (IO_REG(address, __IO_DIGITAL_ENABLE) & pin) EQUAL 0 )
        // enable digital for CS
        IO_REG(address, __IO_DIGITAL_ENABLE) |= pin;

    if( state is HIGH )
        IO_REG(address, __IO_DATA) |= pin;
    else
        IO_REG(address, __IO_DATA) &= ~pin;

}

byte SPI_transfer( SPI_Driver *spi, byte data )
{
    unsigned long base_address = __SPI_MODULES_ADDR[spi->module_num];

    IO_REG(base_address, __SPI_DATA) = data;

    while( (IO_REG(base_address, __SPI_STATUS) & SSI_SR_BSY) is SSI_SR_BSY );

    data = IO_REG(base_address, __SPI_DATA);

    return data;
}

const SPI_Driver* __SPI_getModule( SPI_t spi )
{
    const SPI_Driver* driver = &SPIs[spi];

    // call Driver APIs to check pin availability.
    if( __Driver_isPinAvailable(driver->CLK) is false )
        return NULL;

    // call Driver APIs to mark pins busy.
    __Driver_setPinBusy( driver->CLK );
    __Driver_setPinBusy( driver->CS );
    __Driver_setPinBusy( driver->Rx );
    __Driver_setPinBusy( driver->Tx );

    return &SPIs[spi];
}

bool __SPI_isAvailable( SPI_t spi )
{
    const SPI_Driver* driver = &SPIs[spi];

    // call Driver APIs to check pin availability.
    // check the clock of the driver
    // check CLK CS Rx Tx availability
    if( ( SYSCTL_RCGCSSI_R & (1 << driver->module_num) ) NOT_EQUAL 0 )
        if( ! __Driver_isPinAvailable(driver->CLK) )
            if( ! __Driver_isPinAvailable(driver->CS) )
                if( ! __Driver_isPinAvailable(driver->Rx) )
                    if( ! __Driver_isPinAvailable(driver->Tx) )
                        return false;

    return true;
}

void SPI_deinit( SPI_Driver* spi )
{
    byte port = __GET_PORT(spi->CLK);
    unsigned long config = (1 << __GET_PIN(spi->CLK)) |
                           (1 << __GET_PIN(spi->CS))  |
                           (1 << __GET_PIN(spi->Rx))  |
                           (1 << __GET_PIN(spi->Tx));

/************************************ SPI ************************************/
    // disable SPI module
    IO_REG(__SPI_MODULES_ADDR[spi->module_num], __SPI_CONTROL_1) &= ~(SSI_CR1_SSE);
    // disable SPI clk
    SYSCTL_RCGCSSI_R  &= ~(1 << spi->module_num);
/*****************************************************************************/

/*********************************** GPIO ***********************************/
    // disable DEN
    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE)   &= ~config;
    // disable AFSEL
    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) &= ~config;
/****************************************************************************/

/********************************* free pins *********************************/
//    __Driver_clearSavedConfig(driver);
    __Driver_setPinFree(spi->CLK);
    __Driver_setPinFree(spi->CS);
    __Driver_setPinFree(spi->Rx);
    __Driver_setPinFree(spi->Tx);
/*****************************************************************************/
}
