#include "UART.h"
#include "tm4c123gh6pm.h"
#include "inner/__PCTL.h"
#include "inner/__IO.h"
#include <math.h>

#define __UART0_BASE_ADDR     0x4000C000
#define __UART_MODULES_OFFSET 0x1000
#define PORT_B 1
#define U1_PORTB_PCTL_ENCODE_INDEX 8
#define __UART_MODULER_NUMBER(uart_module) (byte)uart_module
#define __UART_PORT(uart_module) (byte)(uart_module >> BYTE_LENGTH)
#define __UART_RxPIN(uart_module) (byte)(uart_module >> (BYTE_LENGTH * 2))
#define __UART_TxPIN(uart_module) (byte)(uart_module >> (BYTE_LENGTH * 3))

enum UART_Properties_t {
    __UART_DATA                     = 0x000,
    __UART_RECIEVE_STATUS           = 0x014,
    __UART_FLAG                     = 0x018,
    __UART_INTEGER_BAUDRATE_DIV     = 0x024,
    __UART_FRACTION_BAUDRATE_DIV    = 0x028,
    __UART_LINE_CONTROL             = 0x02C,
    __UART_CONTROL                  = 0x030,
    __UART_INTERRUPT_FIFO_LEVEL_SEL = 0x034,
    __UART_INTERRUPT_MASK           = 0x038,
    __UART_RAW_INTERRUPT_MASK       = 0x03C,
    __UART_MASKED_INTERRUPT_STATUS  = 0x040,
    __UART_INTERRUPT_CLEAR          = 0x044,
    __UART_CLK_CONFIG               = 0xFC8
} UART_Properties_t;

void UART_init( UART_t uart_module, UART_BAUDRATE_t baudRate, UART_MODE_t mode )
{
    byte module_number = __UART_MODULER_NUMBER(uart_module);
    byte port          = __UART_PORT(uart_module);
    byte RxPin         = __UART_RxPIN(uart_module);
    byte TxPin         = __UART_TxPIN(uart_module);

/************************************ CLK ************************************/
    // enable UART CLK
    SYSCTL_RCGCUART_R |= (1 << module_number);
    // poll until UART is available
    while( (SYSCTL_PPUART_R & (1 << module_number)) == 0);

    // enable GPIO CLK
    SYSCTL_RCGCGPIO_R |= (1 << port);
    // poll until GPIO is available
    while( (SYSCTL_PPGPIO_R & (1 << port)) == 0);
/*****************************************************************************/

/**************************** GPIO configurations ****************************/
    unsigned long uart_port_addr             = __IO_PORTS_ADDR[port];

    /* PCTL and Directiom configurations
     * Tx is always the next higher bit
     * and therefor each bit has 4 bits PCTL configuration
     * so, Tx = Rx << 4 */

    // FIXME: same if checks, another solution ?
    if( mode == UART_MODE_Tx )
    {
        IO_REG(uart_port_addr, __IO_PORT_CONTROL) =
                ( IO_REG(uart_port_addr, __IO_PORT_CONTROL) & ~(0xF << (TxPin * 4)) ) |
                PCTL_UART_Rx[module_number] << 4;
        IO_REG(uart_port_addr, __IO_DIRECTION)  |= (1 << TxPin);
    }

    else if( mode == UART_MODE_Rx )
    {
        IO_REG(uart_port_addr, __IO_PORT_CONTROL) =
                ( IO_REG(uart_port_addr, __IO_PORT_CONTROL) & ~(0xF << (RxPin * 4)) ) |
                PCTL_UART_Rx[module_number];
        IO_REG(uart_port_addr, __IO_DIRECTION)  &= ~(1 << RxPin);
    }

    else    // Tx and Rx
    {
        // Rx
        IO_REG(uart_port_addr, __IO_PORT_CONTROL) =
                ( IO_REG(uart_port_addr, __IO_PORT_CONTROL) & ~(0xF << (RxPin * 4)) ) |
                PCTL_UART_Rx[module_number];
        IO_REG(uart_port_addr, __IO_DIRECTION)  &= ~(1 << RxPin);
        // Tx
        IO_REG(uart_port_addr, __IO_PORT_CONTROL) =
                ( IO_REG(uart_port_addr, __IO_PORT_CONTROL) & ~(0xF << (TxPin * 4)) ) |
                PCTL_UART_Rx[module_number] << 4;
        IO_REG(uart_port_addr, __IO_DIRECTION)  |= (1 << TxPin);
    }

    // enable alternative function
    IO_REG(uart_port_addr, __IO_ALTERNATIVE_FUNC_SEL) |= bits_specific;

    // disable analog
    IO_REG(uart_port_addr, __IO_ANALOG_MODLE_SEL) &= bits_specific_complemented;

    // digital enable
    IO_REG(uart_port_addr, __IO_DIGITAL_ENABLE)   |= bits_specific;
/*****************************************************************************/

/**************************** UART configurations ****************************/
    unsigned long UART_base_addr = __UART_MODULES_ADDR[__UART_PORT(uart_module)];

    // disable while configure the module
    IO_REG(UART_base_addr, __UART_CONTROL) &= ~UART_CTL_UARTEN;

    // baud rate
    // BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
    // UARTFBRD[DIVFRAC] = integer(BRDF * 64 + 0.5)
    // ClkDiv (by default) = 16
    // TODO: clk need to be global configurational
    float BRDI_float = 16000000.0f / ( 16.0f * baudRate );
    uint16_t BRDI     = (uint16_t)BRDI_float;
    BRDI_float = BRDI_float - (long)BRDI_float;
    uint8_t BRDF      = (uint8_t)(BRDI_float * 64 + 0.5);
    // integer part
    IO_REG(UART_base_addr, __UART_INTEGER_BAUDRATE_DIV)  = BRDI;
    // fractional part
    IO_REG(UART_base_addr, __UART_FRACTION_BAUDRATE_DIV) = BRDF;

    // TODO: this need to be redesigned
    /* No parity,
     * word length = 8bits,
     * enable FIFOs,
     * 1 stop bit,
     * no break send(normal use)
     */
    // IO_REG(UART_base_addr, __UART_LINE_CONTROL) = 0b01110000;
    IO_REG(UART_base_addr, __UART_LINE_CONTROL) = 0x0;
    IO_REG(UART_base_addr, __UART_LINE_CONTROL) = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    // use system clk
    IO_REG(UART_base_addr, __UART_CLK_CONFIG) = 0x0;

    // enable the module
    IO_REG(UART_base_addr, __UART_CONTROL) |= UART_CTL_UARTEN;

/*****************************************************************************/
}

void UART_write(UART_t uart_module, byte data )
{
    // poll until the fifo has a space for new data
    while( (IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_FLAG) & UART_FR_TXFF) != 0 );

    IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_DATA) = data;
}

void UART_writeLine(UART_t uart_module, byte *data )
{
    while(*data != '\0')
    {
        // poll until the fifo has a space for new data
        while( (IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_FLAG) & UART_FR_TXFF) != 0 );
        IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_DATA) = *data++;
    }
}

byte UART_read( UART_t uart_module )
{
    // poll until there is new data
    while( (IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_FLAG) & UART_FR_RXFE) != 0 );

    // return only 1 byte
    return IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_DATA) & 0xFF;
}

byte *UART_readLine(UART_t uart_module, byte *buffer, size_t len )
{
    size_t counter = 0;

    while(counter < len - 1)
    {
        // poll until there is new data
        while( (IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_FLAG) & UART_FR_RXFE) != 0 );

        // get only 1 byte
        buffer[counter] = IO_REG(__UART_MODULES_ADDR[__UART_PORT(uart_module)], __UART_DATA) & 0xFF;

        if(buffer[counter] == '\n')
        {
            buffer[counter + 1] = '\0';
            break;
        }

        counter++;
    }

    return buffer;
}

void UART_disable( UART_t uart_module, UART_MODE_t mode )
{

}
