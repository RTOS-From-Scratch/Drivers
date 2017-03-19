#ifndef IO_H_
#define IO_H_

typedef unsigned char byte;

// This enum represents ports and pins numbers
// ex: 0x0301 -> PIN: 03, PORT: 01(PORT B)
typedef enum PORT_PIN {
    A0 = 0x0000,
    A1 = 0x0100,
    A2 = 0x0200,
    A3 = 0x0300,
    A4 = 0x0400,
    A5 = 0x0500,
    A6 = 0x0600,
    A7 = 0x0700,
    B0 = 0x0001,
    B1 = 0x0101,
    B2 = 0x0201,
    B3 = 0x0301,
    B4 = 0x0401,
    B5 = 0x0501,
    B6 = 0x0601,
    B7 = 0x0701,
    C0 = 0x0002,
    C1 = 0x0102,
    C2 = 0x0202,
    C3 = 0x0302,
    C4 = 0x0402,
    C5 = 0x0502,
    C6 = 0x0602,
    C7 = 0x0702,
    D0 = 0x0003,
    D1 = 0x0103,
    D2 = 0x0203,
    D3 = 0x0303,
    D4 = 0x0403,
    D5 = 0x0503,
    D6 = 0x0603,
    D7 = 0x0703,
    E0 = 0x0004,
    E1 = 0x0104,
    E2 = 0x0204,
    E3 = 0x0304,
    E4 = 0x0404,
    E5 = 0x0504,
    E6 = 0x0604,
    E7 = 0x0704,
    F0 = 0x0005,
    F1 = 0x0105,
    F2 = 0x0205,
    F3 = 0x0305,
    F4 = 0x0405,
    F5 = 0x0505,
    F6 = 0x0605,
    F7 = 0x0705,
} PORT_PIN;

#endif // IO_H_
