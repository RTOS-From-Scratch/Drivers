#ifndef IO_H_
#define IO_H_

enum PORTS { PORT_A, PORT_B, PORT_C, PORT_D, PORT_E, PORT_F };
typedef enum PORTS PORTS;

enum PINS { PIN_0, PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7 };
typedef enum PINS PINS;

enum PIN_STATE { LOW, HIGH };
typedef enum PIN_STATE PIN_STATE;

enum PIN_MODES { INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN };
typedef enum PIN_MODES PIN_MODES;

enum PIN_PULL_STATE { UP, DOWN };
typedef enum PIN_PULL_STATE PIN_PULL_STATE;

#define PORT_PIN PORTS port, PINS pin
#define A0 PORT_A, PIN_0
#define A1 PORT_A, PIN_1
#define A2 PORT_A, PIN_2
#define A3 PORT_A, PIN_3
#define A4 PORT_A, PIN_4
#define A5 PORT_A, PIN_5
#define A6 PORT_A, PIN_6
#define A7 PORT_A, PIN_7
#define B0 PORT_B, PIN_0
#define B1 PORT_B, PIN_1
#define B2 PORT_B, PIN_2
#define B3 PORT_B, PIN_3
#define B4 PORT_B, PIN_4
#define B5 PORT_B, PIN_5
#define B6 PORT_B, PIN_6
#define B7 PORT_B, PIN_7
#define C0 PORT_C, PIN_0
#define C1 PORT_C, PIN_1
#define C2 PORT_C, PIN_2
#define C3 PORT_C, PIN_3
#define C4 PORT_C, PIN_4
#define C5 PORT_C, PIN_5
#define C6 PORT_C, PIN_6
#define C7 PORT_C, PIN_7
#define D0 PORT_D, PIN_0
#define D1 PORT_D, PIN_1
#define D2 PORT_D, PIN_2
#define D3 PORT_D, PIN_3
#define D4 PORT_D, PIN_4
#define D5 PORT_D, PIN_5
#define D6 PORT_D, PIN_6
#define D7 PORT_D, PIN_7
#define E0 PORT_E, PIN_0
#define E1 PORT_E, PIN_1
#define E2 PORT_E, PIN_2
#define E3 PORT_E, PIN_3
#define E4 PORT_E, PIN_4
#define E5 PORT_E, PIN_5
#define E6 PORT_E, PIN_6
#define E7 PORT_E, PIN_7
#define F0 PORT_F, PIN_0
#define F1 PORT_F, PIN_1
#define F2 PORT_F, PIN_2
#define F3 PORT_F, PIN_3
#define F4 PORT_F, PIN_4
#define F5 PORT_F, PIN_5
#define F6 PORT_F, PIN_6
#define F7 PORT_F, PIN_7

#endif // IO_H_
