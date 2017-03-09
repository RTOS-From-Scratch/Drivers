#ifndef UART_H_
#define UART_H_

#include "IO.h"
#include "inner/inner_UART.h"
#include <stdint.h>
#include <stdlib.h>

// UART Modules
#define UART_MODULE_t   uint8_t module_num, PORTS port, PINS RxPin, PINS TxPin
#define U0          0, __U0_PORT, __U0_RX_pin, __U0_TX_pin
#define U1_PORTC    1, __U1_PORTC_PORT, __U1_PORTC_RX_pin, __U1_PORTC_TX_pin
#define U2          2, __U2_PORT, __U2_RX_pin, __U2_TX_pin
#define U3          3, __U3_PORT, __U2_RX_pin, __U3_TX_pin
#define U4          4, __U4_PORT, __U2_RX_pin, __U4_TX_pin
#define U5          5, __U5_PORT, __U2_RX_pin, __U5_TX_pin
#define U6          6, __U6_PORT, __U2_RX_pin, __U6_TX_pin
#define U7          7, __U7_PORT, __U2_RX_pin, __U7_TX_pin
#define U1_PORTB    8, __U1_PORTB_PORT, __U1_PORTB_RX_pin, __U1_PORTB_TX_pin

// UART Tx/Rx port_pin
enum UART_TxRx_t {
        U0_Rx       = __U0_ADDR,
        U0_Tx       = __U0_ADDR,
        U1_PORTC_Rx = __U1_PORTC_ADDR,
        U1_PORTC_Tx = __U1_PORTC_ADDR,
        U2_Rx       = __U2_ADDR,
        U2_Tx       = __U2_ADDR,
        U3_Rx       = __U3_ADDR,
        U3_Tx       = __U3_ADDR,
        U4_Rx       = __U4_ADDR,
        U4_Tx       = __U4_ADDR,
        U5_Rx       = __U5_ADDR,
        U5_Tx       = __U5_ADDR,
        U6_Rx       = __U6_ADDR,
        U6_Tx       = __U6_ADDR,
        U7_Rx       = __U7_ADDR,
        U7_Tx       = __U7_ADDR,
        U1_PORTB_Rx = __U1_PORTB_ADDR,
        U1_PORTB_Tx = __U1_PORTB_ADDR,
};

enum UART_MODE_t {
    UART_MODE_Tx,
    UART_MODE_Rx,
    UART_MODE_TxRx
};

enum UART_BAUDRATE_t {
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
};

typedef enum UART_BAUDRATE_t UART_BAUDRATE_t;
typedef enum UART_MODE_t UART_MODE_t;
typedef enum UART_TxRx_t UART_Tx_t;
typedef enum UART_TxRx_t UART_Rx_t;

// Functions
void UART_init( UART_BAUDRATE_t baudRate, UART_MODE_t mode, UART_MODULE_t );
void UART_write( UART_Tx_t Tx , char _char );
void UART_writeLine( UART_Tx_t Tx , char* _char);
char UART_read( UART_Rx_t Rx );
char* UART_readLine(UART_Rx_t Rx , char *buffer, size_t len);
void UART_disable( UART_MODE_t mode, UART_MODULE_t );

#endif // UART_H_
