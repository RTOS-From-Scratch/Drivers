#ifndef UART_H_
#define UART_H_

#include "Misc/src/definitions.h"
#include "IO.h"
#include "inner/__UART.h"
#include <stdint.h>
#include <stdlib.h>

// Tx_pin | Rx_pin | PORT | module_num
typedef enum UART_t {
    U0       = 0x01000000,
    U1_PORTC = 0x05040201,
    U1_PORTB = 0x01000101,
    U2       = 0x07060302,
    U3       = 0x07060303,
    U4       = 0x05040204,
    U5       = 0x05040405,
    U6       = 0x05040306,
    U7       = 0x01000407,
} UART_t;

typedef enum UART_MODE_t {
    UART_MODE_Tx,
    UART_MODE_Rx,
    UART_MODE_TxRx
} UART_MODE_t;

typedef enum UART_BAUDRATE_t {
    BR_110    = 110,
    BR_300    = 300,
    BR_600    = 600,
    BR_1200   = 1200,
    BR_2400   = 2400,
    BR_4800   = 4800,
    BR_9600   = 9600,
    BR_14400  = 14400,
    BR_19200  = 19200,
    BR_28800  = 28800,
    BR_38400  = 38400,
    BR_56000  = 56000,
    BR_57600  = 57600,
    BR_115200 = 115200,
    BR_128000 = 128000,
    BR_153600 = 153600,
    BR_230400 = 230400,
    BR_256000 = 256000,
    BR_460800 = 460800,
    BR_921600 = 921600
} UART_BAUDRATE_t;

// Functions
void UART_init( UART_t uart_module, UART_BAUDRATE_t baudRate, UART_MODE_t mode );
void UART_write( UART_t uart_module , byte data );
void UART_writeLine( UART_t uart_module, byte* data );
byte UART_read( UART_t uart_module );
byte* UART_readLine( UART_t uart_module, byte *buffer, size_t len );
void UART_disable( UART_t uart_module, UART_MODE_t mode );

#endif // UART_H_
