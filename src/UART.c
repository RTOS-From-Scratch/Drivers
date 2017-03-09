#include "UART.h"
#include "tm4c123gh6pm.h"
#include "inner/PCTL.h"
#include "inner/inner_IO.h"
#include <math.h>

#define UART0_BASE_ADDR     0x4000C000
#define UART_MODULES_OFFSET 0x1000

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
};

typedef enum UART_Properties_t UART_Properties_t;

void UART_init( UART_BAUDRATE_t baudRate, UART_MODE_t mode, UART_MODULE_t )
{
/************************************ CLK ************************************/
    // if module number == 8, this means it is U1_PORTB
    uint8_t new_module_num = (module_num == 8) ? 1 : module_num;
    // enable UART CLK
    SYSCTL_RCGCUART_R |= (1 << new_module_num);
    // poll until UART is available
    while( (SYSCTL_PPUART_R & (1 << new_module_num)) == 0);

    // enable GPIO CLK
    SYSCTL_RCGCGPIO_R |= (1 << port);
    // poll until GPIO is available
    while( (SYSCTL_PPGPIO_R & (1 << port)) == 0);
/*****************************************************************************/

/**************************** GPIO configurations ****************************/
    unsigned long TxRx_port_addr                 = __PORTS_ADDR[port];
    unsigned long TxRx_bit_specific              = (1 << TxPin) | (1 << RxPin);
    unsigned long TxRx_bit_specific_complemented = TxRx_bit_specific;

    // digital enable
    IO_REG(TxRx_port_addr, __IO_DEN)   |= TxRx_bit_specific;

    // disable analog
    IO_REG(TxRx_port_addr, __IO_DEN)   |= TxRx_bit_specific_complemented;

    // enable alternative function
    IO_REG(TxRx_port_addr, __IO_AFSEL) |= TxRx_bit_specific;

    /* PCTL and Directiom configurations
     * Tx is always the next higher bit
     * and therefor each bit has 4 bits PCTL configuration
     * so, Tx = Rx << 4 */
    if( mode == UART_MODE_Tx )
    {
        IO_REG(TxRx_port_addr, __IO_PCTL) |= PCTL_UART_Rx[module_num] << 4;
        IO_REG(TxRx_port_addr, __IO_DIR)  |= (1 << TxPin);
    }

    else if( mode == UART_MODE_Rx )
    {
        IO_REG(TxRx_port_addr, __IO_PCTL) |= PCTL_UART_Rx[module_num];
        IO_REG(TxRx_port_addr, __IO_DIR)  |= ~(1 << RxPin);
    }

    else    // Tx and Rx
    {
        IO_REG(TxRx_port_addr, __IO_PCTL) |= PCTL_UART_Rx[module_num];
        IO_REG(TxRx_port_addr, __IO_PCTL) |= PCTL_UART_Rx[module_num] << 4;
        IO_REG(TxRx_port_addr, __IO_DIR)  |= (1 << TxPin);
        IO_REG(TxRx_port_addr, __IO_DIR)  |= ~(1 << RxPin);
    }
/*****************************************************************************/

/**************************** UART configurations ****************************/
    unsigned long UART_base_addr = UART0_BASE_ADDR + ( UART_MODULES_OFFSET * new_module_num );

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

void UART_write( UART_Tx_t Tx, char _char )
{
    // poll until the fifo has a space for new data
    while( (IO_REG(Tx, __UART_FLAG) & UART_FR_TXFF) != 0 );

    IO_REG(Tx, __UART_DATA) = _char;
}

void UART_writeLine( UART_Tx_t Tx , char* _char )
{
    while(*_char != '\0')
    {
        // poll until the fifo has a space for new data
        while( (IO_REG(Tx, __UART_FLAG) & UART_FR_TXFF) != 0 );
        IO_REG(Tx, __UART_DATA) = *_char++;
    }
}

char UART_read( UART_Tx_t Rx )
{
    // poll until there is new data
    while( (IO_REG(Rx, __UART_FLAG) & UART_FR_RXFE) != 0 );

    // return only 1 byte
    return IO_REG(Rx, __UART_DATA) & 0xFF;
}

char* UART_readLine( UART_Rx_t Rx, char* buffer, size_t len )
{
    size_t counter = 0;

    while(counter < len - 1)
    {
        // poll until there is new data
        while( (IO_REG(Rx, __UART_FLAG) & UART_FR_RXFE) != 0 );

        // get only 1 byte
        buffer[counter] = IO_REG(Rx, __UART_DATA) & 0xFF;

        if(buffer[counter] == '\n')
        {
            buffer[counter + 1] = '\0';
            break;
        }

        counter++;
    }

    return buffer;
}

void UART_disable( UART_MODE_t mode, UART_MODULE_t )
{

}
