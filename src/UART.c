#include "UART.h"
#include "tm4c123gh6pm.h"
#include "inner/__IO.h"
#include "ISR_ctrl.h"
#include "Misc/src/itoa.h"
#include "PLL.h"
#include <string.h>
#include "inner/__ISR_vectortable.h"
#include "Misc/src/assert.h"

#define U1_PORTB_PCTL_ENCODE_INDEX 8
#define __UART_MODULES_NUM 9

typedef enum UART_Properties_t {
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

#define NOT_USED 0
#define PCTL_ENCODING          0b01
#define PCTL_ENCODING_U1_PORTC 0b10

struct UART_Driver {
    byte module_num;
    uint16_t Rx;
    uint16_t Tx;
    byte ISR_vectorNum;
};

static const UART_Driver UARTs[__UART_MODULES_NUM] = {
    { 0, A0, A1, ISR_UART_0 },              // U0
    { 1, C4, C5, ISR_UART_1 },              // U1_PORTC
    { 2, D6, D7, ISR_UART_2 },              // U2
    { 3, C6, C7, ISR_UART_3 },              // U3
    { 4, C4, C5, ISR_UART_4 },              // U4
    { 5, E4, E5, ISR_UART_5 },              // U5
    { 6, D4, D5, ISR_UART_6 },              // U6
    { 7, E0, E1, ISR_UART_7 },              // U7
    { 1, B0, B1, ISR_UART_1 },              // U1_PORTB
};

// UART address
static const unsigned long __UART_MODULES_ADDR[] = {
    0x4000C000,         // U0
    0x4000D000,         // U1
    0x4000E000,         // U2
    0x4000F000,         // U3
    0x40010000,         // U4
    0x40011000,         // U5
    0x40012000,         // U6
    0x40013000,         // U7
};

static void __UART_ISR_handler();

typedef void(*__ISR_handler)();
static __ISR_handler __UART_ISR_Handlers[ __UART_MODULES_NUM ];

void UART_init(UART_Driver *uart, UART_BAUDRATE baudRate , bool autoEnable )
{
    // disable interrupt
    // critical section
//    ISR_disable();

    ASSERT(uart is_not NULL);

    byte port          = __GET_PORT(uart->Rx);
    byte RxPin         = __GET_PIN(uart->Rx);
    byte TxPin         = __GET_PIN(uart->Tx);

    unsigned long UART_base_addr = __UART_MODULES_ADDR[uart->module_num];
    ClockSpeed sys_clk_speed = PLL_getClockSpeed();

    byte bits_specific              = (1 << TxPin) | (1 << RxPin);
    byte bits_specific_complemented = ~bits_specific;

/************************************ CLK ************************************/
    // enable UART CLK
    SYSCTL_RCGCUART_R |= (1 << uart->module_num);
    // poll until UART is available
    while( (SYSCTL_PPUART_R & (1 << uart->module_num)) == 0);

    // enable GPIO CLK
    SYSCTL_RCGCGPIO_R |= (1 << port);
    // poll until GPIO is available
    while( (SYSCTL_PPGPIO_R & (1 << port)) == 0);
/*****************************************************************************/

/**************************** GPIO configurations ****************************/
    unsigned long uart_port_addr             = __IO_PORTS_ADDR[port];

    // PCTL
    byte PCTL_encoding = (uart->module_num is U1_PORTC) and (port is __PORT_C) ?
                         PCTL_ENCODING_U1_PORTC :
                         PCTL_ENCODING;
    IO_REG(uart_port_addr, __IO_PORT_CONTROL) &= ~(0xF << (RxPin * 4) |
                                                   0xF << (TxPin * 4) );
    IO_REG(uart_port_addr, __IO_PORT_CONTROL) += (PCTL_encoding << (RxPin * 4) |
                                                  PCTL_encoding << (TxPin * 4) );

    // Direction
    IO_REG(uart_port_addr, __IO_DIRECTION)  &= ~(1 << RxPin);
    IO_REG(uart_port_addr, __IO_DIRECTION)  |= (1 << TxPin);

    if( autoEnable )
        // enable alternative function
        IO_REG(uart_port_addr, __IO_ALTERNATIVE_FUNC_SEL) |= bits_specific;

    // disable analog
    IO_REG(uart_port_addr, __IO_ANALOG_MODE_SEL) &= bits_specific_complemented;

    if( autoEnable )
        // digital enable
        IO_REG(uart_port_addr, __IO_DIGITAL_ENABLE)   |= bits_specific;
/*****************************************************************************/

/**************************** UART configurations ****************************/

    // disable while configure the module
    IO_REG(UART_base_addr, __UART_CONTROL) &= ~UART_CTL_UARTEN;

    // baud rate
    // BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
    // UARTFBRD[DIVFRAC] = integer(BRDF * 64 + 0.5)
    // ClkDiv (by default) = 16
    // TODO: clk need to be global configurational
    float BRDI_float = (float)(sys_clk_speed * 1E6) / ( (float)16.0f * baudRate );
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
     * disable FIFOs,
     * 1 stop bit,
     * no break send(normal use)
     */
    IO_REG(UART_base_addr, __UART_LINE_CONTROL) = 0x0;
    IO_REG(UART_base_addr, __UART_LINE_CONTROL) = UART_LCRH_WLEN_8/* | UART_LCRH_FEN*/;

    // use system clk
    IO_REG(UART_base_addr, __UART_CLK_CONFIG) = UART_CC_CS_SYSCLK;

    if( autoEnable )
        // enable the module
        IO_REG(UART_base_addr, __UART_CONTROL) |= UART_CTL_UARTEN;

/*****************************************************************************/

    // re-enable interrupts
//    ISR_enable();
}

void UART_ISR_init( UART_Driver *uart,
                    UART_BAUDRATE baudRate,
                    UART_ISR_MODE ISR_mode,
                    ISR_PRIORITY priority,
                    void(*ISR_handler)(),
                    bool autoEnable )
{
    intptr_t address = __UART_MODULES_ADDR[uart->module_num];

    // check if it's already initiated
    // if so, disable it for configurations
    if( (SYSCTL_RCGCUART_R & (1 << uart->module_num)) EQUAL 0 )
        UART_init(uart, baudRate, false);
    else if( (IO_REG(address, __UART_CONTROL) & UART_CTL_UARTEN) NOT_EQUAL 0 )
            UART_disable(uart);

    switch( ISR_mode )
    {
        case( UART_ISR_MODE_Rx ):
            IO_REG(address, __UART_INTERRUPT_MASK) |= UART_IM_RXIM;
            break;

        case( UART_ISR_MODE_Tx ):
            IO_REG(address, __UART_INTERRUPT_MASK) |= UART_IM_TXIM;
            break;

        default:
            return;
    }

    __ISR_register( uart->ISR_vectorNum, __UART_ISR_handler );

    // set priority
    __ISR_CTRL_setPriority(uart->ISR_vectorNum, priority);

    __UART_ISR_Handlers[uart->module_num] = ISR_handler;

    if( autoEnable )
        UART_ISR_enable(uart);
}

void UART_ISR_changePriroity( UART_Driver *uart, ISR_PRIORITY newPriority )
{
    __ISR_CTRL_setPriority(uart->ISR_vectorNum, newPriority);
}

void UART_enable( UART_Driver* uart )
{
    unsigned long config = (1 << __GET_PIN(uart->Rx)) | (1 << __GET_PIN(uart->Tx));

    intptr_t address = __IO_PORTS_ADDR[__GET_PORT(uart->Rx)];
    intptr_t uart_address = __UART_MODULES_ADDR[uart->module_num];

    // enable digital
    IO_REG(address, __IO_DIGITAL_ENABLE) |= config;
    // enable alternative funtion
    IO_REG(address, __IO_ALTERNATIVE_FUNC_SEL) |= config;
    // enable UART
    IO_REG(uart_address, __UART_CONTROL) |= UART_CTL_UARTEN;
}

void UART_disable( UART_Driver* uart )
{
    unsigned long config = ( (1 << __GET_PIN(uart->Rx)) | (1 << __GET_PIN(uart->Tx)) );

    intptr_t address = __IO_PORTS_ADDR[__GET_PORT(uart->Rx)];
    byte module_num = uart->module_num % (__UART_MODULES_NUM - 1);
    intptr_t uart_address = __UART_MODULES_ADDR[module_num];

    // disable digital
    IO_REG(address, __IO_DIGITAL_ENABLE) &= ~config;
    // disable alternative function
    IO_REG(address, __IO_ALTERNATIVE_FUNC_SEL) &= ~config;
    // disable UART
    IO_REG(uart_address, __UART_CONTROL) &= ~( UART_CTL_UARTEN );
}

void UART_ISR_enable( UART_Driver *uart )
{
    __ISR_CTRL_enable(uart->ISR_vectorNum);

    if( (IO_REG(__UART_MODULES_ADDR[uart->module_num], __UART_CONTROL) & UART_CTL_UARTEN) EQUAL 0 )
        UART_enable(uart);
}

void UART_ISR_disable( UART_Driver *uart )
{
    __ISR_CTRL_disable(uart->ISR_vectorNum);
}

void UART_print(UART_Driver *uart, char *data )
{
    ASSERT(uart is_not NULL);
    unsigned long address = __UART_MODULES_ADDR[uart->module_num];

    while(*data != '\0')
    {
        // poll until the fifo has a space for new data
        while( (IO_REG(address, __UART_FLAG) & UART_FR_TXFF) != 0 );
//        while( (IO_REG(address, __UART_FLAG) & UART_FR_BUSY) != 0 );
        IO_REG(address, __UART_DATA) = *data++;
    }
}

void UART_println( UART_Driver *uart, char* data )
{
    UART_print(uart, data);

    // write newline
    UART_print(uart, "\n");
}

void UART_writeInt( UART_Driver *uart, int data )
{
    char buffer[11];
    // int value, char* result, int base
    itoa( data, buffer, 10 );
    UART_write( uart, (byte *)buffer, strlen(buffer) );
}

void UART_write( UART_Driver *uart, byte *data, size_t data_len )
{
    unsigned long address = __UART_MODULES_ADDR[uart->module_num];

    do {
        // poll until the fifo has a space for new data
        while( (IO_REG(address, __UART_FLAG) & UART_FR_TXFF) != 0 );
//        while( (IO_REG(address, __UART_FLAG) & UART_FR_BUSY) != 0 );
        IO_REG(address, __UART_DATA) = *data++;
    } while(--data_len);
}

byte UART_read( UART_Driver *uart )
{
    unsigned long address = __UART_MODULES_ADDR[uart->module_num];

    // poll until there is new data
    while( (IO_REG(address, __UART_FLAG) & UART_FR_RXFE) != 0 );
//    while( (IO_REG(address, __UART_FLAG) & UART_FR_BUSY) != 0 );

    // return only 1 byte
    return IO_REG(address, __UART_DATA) & 0xFF;
}

int UART_readInt( UART_Driver *uart )
{
    byte buffer[11];
    UART_readAll( uart, buffer, 11 );

    return atoi((char *)buffer);
}

void UART_readAll( UART_Driver *uart, byte *buffer, size_t len )
{
    size_t counter = 0;

    unsigned long address = __UART_MODULES_ADDR[uart->module_num];

    while(counter < len - 1)
    {
        // poll until there is new data
        while( (IO_REG(address, __UART_FLAG) & UART_FR_RXFE) != 0 );
//        while( (IO_REG(address, __UART_FLAG) & UART_FR_BUSY) != 0 );

        // get only 1 byte
        buffer[counter] = IO_REG(address, __UART_DATA) & 0xFF;

        if(buffer[counter] is '\n')
        {
            buffer[counter + 1] = '\0';
            break;
        }

        counter++;
    }
}

const UART_Driver* __UART_getModule( UART_t uart )
{
    const UART_Driver* driver = &UARTs[uart];

    if( __UART_isAvailable(uart) is false )
        return NULL;

    // call Driver APIs to mark pins busy.
    __Driver_setPinBusy( driver->Rx );
    __Driver_setPinBusy( driver->Tx );

    return &UARTs[uart];
}

bool __UART_isAvailable( UART_t uart )
{
    const UART_Driver* driver = &UARTs[uart];

    // call Driver APIs to check pin availability.
    // check the clock of the driver
    // check Rx Tx availability
    if( (SYSCTL_RCGCUART_R & (1 << driver->module_num)) NOT_EQUAL 0 )
        if( ! __Driver_isPinAvailable(driver->Rx) )
            if( ! __Driver_isPinAvailable(driver->Tx) )
                return false;

    return true;
}

void UART_deinit( UART_Driver *uart )
{
    // disable interrupt
    // critical section
//    ISR_disable();

//    unsigned long config = (1 << uart->Rx) | (1 << uart->Tx);
//    byte port         = __GET_PORT(uart->Rx);

/*********************************** UART ***********************************/
    // disable UART module
//    IO_REG(__UART_MODULES_ADDR[uart->module_num], __UART_CONTROL) &= ~UART_CTL_UARTEN;
    // disable UART CLK
    SYSCTL_RCGCUART_R &= ~(1 << uart->module_num);
/****************************************************************************/

/*********************************** GPIO ***********************************/
//    // disable DEN
//    IO_REG(__IO_PORTS_ADDR[port], __IO_DIGITAL_ENABLE) &= ~config;
//    // disable AFSEL
//    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) &= ~config;
    UART_disable(uart);
/****************************************************************************/

/********************************* Interrupt *********************************/
    // disable interrupts if it's enabled
    // TODO: Global interrupt
    // IO_REG(__UART_MODULES_ADDR[__UART_MODULE_NUMBER(uart_module)], __UART_INTERRUPT_MASK) = 0;
    UART_ISR_disable(uart);
/*****************************************************************************/

/******************************** free pins *********************************/
    __Driver_setPinFree(uart->Rx);
    __Driver_setPinFree(uart->Tx);
/****************************************************************************/

    // re-enable interrupts
//    ISR_enable();
}

void __UART_ISR_handler()
{
    byte vectorNumber = NVIC_INT_CTRL_R & NVIC_INT_CTRL_VEC_ACT_M;
    const UART_Driver* uart = NULL;
    intptr_t address;

    // find with module responsable for the interruption
    for( byte iii = 0; iii < __UART_MODULES_NUM; ++iii )
        if( vectorNumber EQUAL UARTs[iii].ISR_vectorNum )
            uart = &UARTs[iii];

    address = __UART_MODULES_ADDR[uart->module_num];

    IO_REG(address, __UART_INTERRUPT_CLEAR) |=
            IO_REG(address, __UART_MASKED_INTERRUPT_STATUS);

    __UART_ISR_Handlers[uart->module_num]();
}

/**************** This part is using for communication with PC ****************/
#ifdef PC_COMMUNICATION
    #define U0 0
    // TODO: this part should be in cmake file as an option
    #define PC_COMMUNICATION_BAUDRATE UART_BAUDRATE_115200
    static Driver *__UART0 = NULL;

    void SYS_UART_init()
    {
        __UART0 = Driver_construct(UART, U0);
        UART_init( __UART0, PC_COMMUNICATION_BAUDRATE, true );
    }

    void SYS_UART_ISR_init( UART_ISR_MODE ISR_mode, void(*ISR_handler)() )
    {
        if(__UART0 is NULL)
            __UART0 = Driver_construct(UART, U0);
        UART_ISR_init( __UART0,
                       PC_COMMUNICATION_BAUDRATE,
                       ISR_mode,
                       ISR_PRIORITY_0,
                       ISR_handler,
                       true );
    }

    void SYS_UART_writeInt( int data )
    {
        UART_writeInt(__UART0, data);
    }

    void SYS_UART_write( byte* data, size_t data_len )
    {
        UART_write( __UART0, data, data_len );
    }

    void SYS_UART_print( char* data )
    {
        UART_print(__UART0, data);
    }

    void SYS_UART_println( char* data )
    {
        UART_println(__UART0, data);
    }

    byte SYS_UART_read()
    {
        return UART_read( __UART0 );
    }

    int SYS_UART_readInt()
    {
        return UART_readInt(__UART0);
    }

    void SYS_UART_readAll( byte *buffer, size_t len )
    {
        return UART_readAll( __UART0, buffer, len );
    }
#endif // PC_COMMUNICATION

/******************************************************************************/
