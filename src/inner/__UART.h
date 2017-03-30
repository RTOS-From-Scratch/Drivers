#ifndef INNER_UART_H_
#define INNER_UART_H_

// UART Rx/Tx pins
//#define __U0_PORT         PORT_A
//#define __U0_RX_pin       0
//#define __U0_TX_pin       1
//#define __U1_PORTC_PORT   PORT_C
//#define __U1_PORTC_RX_pin 4
//#define __U1_PORTC_TX_pin 5
//#define __U2_PORT         PORT_D
//#define __U2_RX_pin       6
//#define __U2_TX_pin       7
//#define __U3_PORT         PORT_C
//#define __U3_RX_pin       6
//#define __U3_TX_pin       7
//#define __U4_PORT         PORT_C
//#define __U4_RX_pin       4
//#define __U4_TX_pin       5
//#define __U5_PORT         PORT_E
//#define __U5_RX_pin       4
//#define __U5_TX_pin       5
//#define __U6_PORT         PORT_D
//#define __U6_RX_pin       4
//#define __U6_TX_pin       5
//#define __U7_PORT         PORT_E
//#define __U7_RX_pin       0
//#define __U7_TX_pin       1
//#define __U1_PORTB_PORT   PORT_B
//#define __U1_PORTB_RX_pin 0
//#define __U1_PORTB_TX_pin 1

// UART address
static const unsigned long __UART_MODULES_ADDR[] = {
    0x4000C000,
    0x4000D000,
    0x4000E000,
    0x4000F000,
    0x40010000,
    0x40011000,
    0x40012000,
    0x40013000
};

/*// UART Tx/Rx port_pin
#define UART_Rx_Tx_t    unsigned long addr, PORT_PIN
#define U0_Rx           __U0_ADDR,       A0
#define U0_Tx           __U0_ADDR,       A1
#define U1_PORTC_Rx     __U1_PORTC_ADDR, C4
#define U1_PORTC_Tx     __U1_PORTC_ADDR, C5
#define U2_Rx           __U2_ADDR,       D6
#define U2_Tx           __U2_ADDR,       D7
#define U3_Rx           __U3_ADDR,       C6
#define U3_Tx           __U3_ADDR,       C7
#define U4_Rx           __U4_ADDR,       C4
#define U4_Tx           __U4_ADDR,       C5
#define U5_Rx           __U5_ADDR,       E4
#define U5_Tx           __U5_ADDR,       E5
#define U6_Rx           __U6_ADDR,       D4
#define U6_Tx           __U6_ADDR,       D5
#define U7_Rx           __U7_ADDR,       E0
#define U7_Tx           __U7_ADDR,       E1
#define U1_PORTB_Rx     __U1_PORTB_ADDR, B0
#define U1_PORTB_Tx     __U1_PORTB_ADDR, B1*/

// UART Tx/Rx port_pin
//#define UART_TxRx_t     unsigned long addr
//#define U0_Rx           __U0_ADDR
//#define U0_Tx           __U0_ADDR
//#define U1_PORTC_Rx     __U1_PORTC_ADDR
//#define U1_PORTC_Tx     __U1_PORTC_ADDR
//#define U2_Rx           __U2_ADDR
//#define U2_Tx           __U2_ADDR
//#define U3_Rx           __U3_ADDR
//#define U3_Tx           __U3_ADDR
//#define U4_Rx           __U4_ADDR
//#define U4_Tx           __U4_ADDR
//#define U5_Rx           __U5_ADDR
//#define U5_Tx           __U5_ADDR
//#define U6_Rx           __U6_ADDR
//#define U6_Tx           __U6_ADDR
//#define U7_Rx           __U7_ADDR
//#define U7_Tx           __U7_ADDR
//#define U1_PORTB_Rx     __U1_PORTB_ADDR
//#define U1_PORTB_Tx     __U1_PORTB_ADDR

#endif // INNER_UART_H_
