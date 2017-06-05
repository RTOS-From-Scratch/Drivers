#ifndef UART_H_
#define UART_H_

#include "Misc/src/definitions.h"
#include "driver.h"
#include <stdint.h>
#include <stdlib.h>

PUBLIC
    typedef enum UART_t {
        U1_PORTC = 1,
        U2,
        U3,
        U4,
        U5,
        U6,
        U7,
        U1_PORTB,
    } UART_t;

    typedef enum UART_BAUDRATE {
        UART_BAUDRATE_110    = 110,
        UART_BAUDRATE_300    = 300,
        UART_BAUDRATE_600    = 600,
        UART_BAUDRATE_1200   = 1200,
        UART_BAUDRATE_2400   = 2400,
        UART_BAUDRATE_4800   = 4800,
        UART_BAUDRATE_9600   = 9600,
        UART_BAUDRATE_14400  = 14400,
        UART_BAUDRATE_19200  = 19200,
        UART_BAUDRATE_28800  = 28800,
        UART_BAUDRATE_38400  = 38400,
        UART_BAUDRATE_56000  = 56000,
        UART_BAUDRATE_57600  = 57600,
        UART_BAUDRATE_115200 = 115200,
        UART_BAUDRATE_128000 = 128000,
        UART_BAUDRATE_153600 = 153600,
        UART_BAUDRATE_230400 = 230400,
        UART_BAUDRATE_256000 = 256000,
        UART_BAUDRATE_460800 = 460800,
        UART_BAUDRATE_921600 = 921600
    } UART_BAUDRATE;

    typedef enum UART_ISR_MODE {
        UART_ISR_MODE_Tx,
        UART_ISR_MODE_Rx,
    } UART_ISR_MODE;

    typedef struct UART_Driver UART_Driver;

    // Functions
    void UART_init( UART_Driver *uart, UART_BAUDRATE baudRate, bool autoEnable );
    void UART_enable( UART_Driver* uart );
    void UART_disable( UART_Driver* uart );
    void UART_ISR_init( UART_Driver *uart,
                        UART_BAUDRATE baudRate,
                        UART_ISR_MODE ISR_mode,
                        ISR_PRIORITY priority,
                        void(*ISR_handler)(),
                        bool autoEnable );
    void UART_ISR_enable( UART_Driver* uart );
    void UART_ISR_disable( UART_Driver* uart );
    void UART_ISR_changePriroity( UART_Driver *uart, ISR_PRIORITY newPriority );
    void UART_writeInt( UART_Driver *uart , int data );
    void UART_write( UART_Driver *uart, byte* data, size_t data_len );
    void UART_print( UART_Driver *uart, char* data );
    void UART_println( UART_Driver *uart, char* data );
    byte UART_read( UART_Driver *uart );
    int UART_readInt( UART_Driver *uart );
    void UART_readAll( UART_Driver *uart, byte *buffer, size_t len );
    void UART_deinit( UART_Driver *uart );

    // this part is using for communication with PC
    #ifdef PC_COMMUNICATION
        void SYS_UART_init();
        void SYS_UART_ISR_init( UART_ISR_MODE ISR_mode, void(*ISR_handler)() );
        void SYS_UART_writeInt( int data );
        void SYS_UART_write( byte* data, size_t data_len );
        void SYS_UART_print( char* data );
        void SYS_UART_println( char* data );
        byte SYS_UART_read();
        int SYS_UART_readInt();
        void SYS_UART_readAll( byte *buffer, size_t len );
    #endif // PC_COMMUNICATION

/******************************************************************************/

PRIVATE
    // WARNING: These functions shouldn't be called or used
    const UART_Driver* __UART_getModule( UART_t uart );
    bool __UART_isAvailable( UART_t uart );

#endif // UART_H_
