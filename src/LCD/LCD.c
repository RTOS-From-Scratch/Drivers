#include "LCD.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "../time.h"
#include "Misc/src/definitions.h"
#include "Misc/src/itoa.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//        DL = 1; 8-bit interface data
//        N = 0; 1-line display
//        F = 0; 5x8 dot character font
// 3. Display on/off control: 
//        D = 0; Display off
//        C = 0; Cursor off
//        B = 0; Blinking off
// 4. Entry mode set: 
//        I/D = 1; Increment by 1
//        S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LCD constructor is called).

//LCD_LCD(uint8_t rs, uint8_t rw, uint8_t enable,
//                     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
//                     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
//{
//    init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
//}

//LCD_LCD(uint8_t rs, uint8_t enable,
//                     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
//                     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
//{
//    init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
//}

//LCD_LCD(uint8_t rs, uint8_t rw, uint8_t enable,
//                     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
//{
//    init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
//}

//LCD_LCD(uint8_t rs,    uint8_t enable,
//                     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
//{
//    init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
//}


static void __LCD_send( uint8_t, uint8_t );
static void __LCD_write4bits( uint8_t );
static void __LCD_write8bits( uint8_t );
static void __LCD_pulseEnable();

static GPIO_Driver* __LCD_rs_pin = NULL; // LOW: command.    HIGH: character.
static GPIO_Driver* __LCD_rw_pin = NULL; // LOW: write to LCD.    HIGH: read from LCD.
static GPIO_Driver* __LCD_enable_pin = NULL; // activated by a HIGH pulse.
static GPIO_Driver* __LCD_data_pins[8] = { NULL };

static uint8_t __LCD_displayfunction;
static uint8_t __LCD_displaycontrol;
static uint8_t __LCD_displaymode;

//static uint8_t __LCD_initialized;

static uint8_t __LCD_numlines;
static uint8_t __LCD_row_offsets[4];

static void __LCD_init( uint8_t fourbitmode, PORT_PIN rs, PORT_PIN rw, PORT_PIN enable,
                        PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3,
                        PORT_PIN d4, PORT_PIN d5, PORT_PIN d6, PORT_PIN d7 );

void __LCD_init( uint8_t fourbitmode, PORT_PIN rs, PORT_PIN rw, PORT_PIN enable,
               PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3,
               PORT_PIN d4, PORT_PIN d5, PORT_PIN d6, PORT_PIN d7 )
{
    if( rs NOT_EQUAL -1 )
        __LCD_rs_pin = Driver_construct(GPIO, rs);
    if( rw NOT_EQUAL -1 )
        __LCD_rw_pin = Driver_construct(GPIO, rw);
    if( enable NOT_EQUAL -1 )
        __LCD_enable_pin = Driver_construct(GPIO, enable);

    if( d0 NOT_EQUAL -1 )
        __LCD_data_pins[0] = Driver_construct(GPIO, d0);
    if( d1 NOT_EQUAL -1 )
        __LCD_data_pins[1] = Driver_construct(GPIO, d1);
    if( d2 NOT_EQUAL -1 )
        __LCD_data_pins[2] = Driver_construct(GPIO, d2);
    if( d3 NOT_EQUAL -1 )
        __LCD_data_pins[3] = Driver_construct(GPIO, d3);
    if( d4 NOT_EQUAL -1 )
        __LCD_data_pins[4] = Driver_construct(GPIO, d4);
    if( d5 NOT_EQUAL -1 )
        __LCD_data_pins[5] = Driver_construct(GPIO, d5);
    if( d6 NOT_EQUAL -1 )
        __LCD_data_pins[6] = Driver_construct(GPIO, d6);
    if( d7 NOT_EQUAL -1 )
        __LCD_data_pins[7] = Driver_construct(GPIO, d7);

    if (fourbitmode)
        __LCD_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    else
        __LCD_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    LCD_begin(16, 1, LCD_5x8DOTS);
}

void LCD_init( PORT_PIN rs, PORT_PIN rw, PORT_PIN enable,
               PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3,
               PORT_PIN d4, PORT_PIN d5, PORT_PIN d6, PORT_PIN d7 )
{
    __LCD_init( 1, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
}

void LCD_initWithoutRW( PORT_PIN rs, PORT_PIN enable,
                        PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3,
                        PORT_PIN d4, PORT_PIN d5, PORT_PIN d6, PORT_PIN d7 )
{
    __LCD_init(1, rs, -1, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

void LCD_init4Bits( PORT_PIN rs, PORT_PIN rw, PORT_PIN enable,
                    PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3 )
{
    __LCD_init(1, rs, rw, enable, d0, d1, d2, d3, -1, -1, -1, -1);
}

void LCD_init4BitsWithoutRW( PORT_PIN rs, PORT_PIN enable,
                             PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3 )
{
    __LCD_init(1, rs, -1, enable, d0, d1, d2, d3, -1, -1, -1, -1);
}

void LCD_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    if (lines > 1) {
        __LCD_displayfunction |= LCD_2LINE;
    }
    __LCD_numlines = lines;

    LCD_setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
        __LCD_displayfunction |= LCD_5x10DOTS;
    }

    GPIO_init(__LCD_rs_pin, OUTPUT, true);
    // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
    if (__LCD_rw_pin != NULL) {
        GPIO_init(__LCD_rw_pin, OUTPUT, true);
    }
    GPIO_init(__LCD_enable_pin, OUTPUT, true);

    // Do these once, instead of every time a character is drawn for speed reasons.
    for (int i=0; i<((__LCD_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
    {
        if(__LCD_data_pins[i] NOT_EQUAL NULL)
            GPIO_init(__LCD_data_pins[i], OUTPUT, true);
     }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
    delay_usec(50000);
    // Now we pull both RS and R/W low to begin commands
    GPIO_write(__LCD_rs_pin, LOW);
    GPIO_write(__LCD_enable_pin, LOW);
    if (__LCD_rw_pin != NULL) {
        GPIO_write(__LCD_rw_pin, LOW);
    }

    //put the LCD into 4 bit or 8 bit mode
    if (! (__LCD_displayfunction & LCD_8BITMODE)) {
        // this is according to the hitachi HD44780 datasheet
        // figure 24, pg 46

        // we start in 8bit mode, try to set 4 bit mode
        __LCD_write4bits(0x03);
        delay_usec(4500); // wait min 4.1ms

        // second try
        __LCD_write4bits(0x03);
        delay_usec(4500); // wait min 4.1ms

        // third go!
        __LCD_write4bits(0x03);
        delay_usec(150);

        // finally, set to 4-bit interface
        __LCD_write4bits(0x02);
    } else {
        // this is according to the hitachi HD44780 datasheet
        // page 45 figure 23

        // Send function set command sequence
        LCD_command(LCD_FUNCTIONSET | __LCD_displayfunction);
        delay_usec(4500);    // wait more than 4.1ms

        // second try
        LCD_command(LCD_FUNCTIONSET | __LCD_displayfunction);
        delay_usec(150);

        // third go
        LCD_command(LCD_FUNCTIONSET | __LCD_displayfunction);
    }

    // finally, set # lines, font size, etc.
    LCD_command(LCD_FUNCTIONSET | __LCD_displayfunction);

    // turn the display on with no cursor or blinking default
    __LCD_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LCD_display();

    // clear it off
    LCD_clear();

    // Initialize to default text direction (for romance languages)
    __LCD_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    LCD_command(LCD_ENTRYMODESET | __LCD_displaymode);
}

void LCD_setRowOffsets(int row0, int row1, int row2, int row3)
{
    __LCD_row_offsets[0] = row0;
    __LCD_row_offsets[1] = row1;
    __LCD_row_offsets[2] = row2;
    __LCD_row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LCD_clear()
{
   LCD_command(LCD_CLEARDISPLAY);    // clear display, set cursor position to zero
   delay_usec(2000);    // this command takes a long time!
}

void LCD_home()
{
    LCD_command(LCD_RETURNHOME);    // set cursor position to zero
    delay_usec(2000);    // this command takes a long time!
}

void LCD_setCursor(uint8_t col, uint8_t row)
{
    const size_t max_lines = sizeof(__LCD_row_offsets) / sizeof(*__LCD_row_offsets);
    if ( row >= max_lines ) {
        row = max_lines - 1;        // we count rows starting w/0
    }
    if ( row >= __LCD_numlines ) {
        row = __LCD_numlines - 1;        // we count rows starting w/0
    }

    LCD_command(LCD_SETDDRAMADDR | (col + __LCD_row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCD_noDisplay() {
    __LCD_displaycontrol &= ~LCD_DISPLAYON;
    LCD_command(LCD_DISPLAYCONTROL | __LCD_displaycontrol);
}
void LCD_display() {
    __LCD_displaycontrol |= LCD_DISPLAYON;
    LCD_command(LCD_DISPLAYCONTROL | __LCD_displaycontrol);
}

// Turns the underline cursor on/off
void LCD_noCursor() {
    __LCD_displaycontrol &= ~LCD_CURSORON;
    LCD_command(LCD_DISPLAYCONTROL | __LCD_displaycontrol);
}
void LCD_cursor() {
    __LCD_displaycontrol |= LCD_CURSORON;
    LCD_command(LCD_DISPLAYCONTROL | __LCD_displaycontrol);
}

// Turn on and off the blinking cursor
void LCD_noBlink() {
    __LCD_displaycontrol &= ~LCD_BLINKON;
    LCD_command(LCD_DISPLAYCONTROL | __LCD_displaycontrol);
}
void LCD_blink() {
    __LCD_displaycontrol |= LCD_BLINKON;
    LCD_command(LCD_DISPLAYCONTROL | __LCD_displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCD_scrollDisplayLeft(void) {
    LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD_scrollDisplayRight(void) {
    LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCD_leftToRight(void) {
    __LCD_displaymode |= LCD_ENTRYLEFT;
    LCD_command(LCD_ENTRYMODESET | __LCD_displaymode);
}

// This is for text that flows Right to Left
void LCD__LCD_rightToLeft(void) {
    __LCD_displaymode &= ~LCD_ENTRYLEFT;
    LCD_command(LCD_ENTRYMODESET | __LCD_displaymode);
}

// This will 'right justify' text from the cursor
void LCD_autoscroll(void) {
    __LCD_displaymode |= LCD_ENTRYSHIFTINCREMENT;
    LCD_command(LCD_ENTRYMODESET | __LCD_displaymode);
}

// This will 'left justify' text from the cursor
void LCD_noAutoscroll(void) {
    __LCD_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    LCD_command(LCD_ENTRYMODESET | __LCD_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCD_createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // we only have 8 locations 0-7
    LCD_command(LCD_SETCGRAMADDR | (location << 3));
    for (int i=0; i<8; i++) {
        LCD_write(charmap[i]);
    }
}

/*********** mid level commands, for sending data/cmds */

inline void LCD_command(uint8_t value) {
    __LCD_send(value, LOW);
}

inline size_t LCD_write(uint8_t value) {
    __LCD_send(value, HIGH);
    return 1; // assume sucess
}

size_t LCD_writeInt( int num )
{
    char buffer[15];
    itoa( num, buffer, 10 );

    LCD_print(buffer);

    return 1;
}

size_t LCD_print( char* data )
{
    while(*data NOT_EQUAL '\0' )
    {
        LCD_write(*data++);
    }

    return 1;
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void __LCD_send(uint8_t value, uint8_t mode) {
    GPIO_write(__LCD_rs_pin, mode);

    // if there is a RW pin indicated, set it low to Write
    if (__LCD_rw_pin != NULL) {
        GPIO_write(__LCD_rw_pin, LOW);
    }

    if (__LCD_displayfunction & LCD_8BITMODE) {
        __LCD_write8bits(value);
    } else {
        __LCD_write4bits(value>>4);
        __LCD_write4bits(value);
    }
}

void __LCD_pulseEnable(void) {
    GPIO_write(__LCD_enable_pin, LOW);
    delay_usec(1);
    GPIO_write(__LCD_enable_pin, HIGH);
    delay_usec(1);        // enable pulse must be >450ns
    GPIO_write(__LCD_enable_pin, LOW);
    delay_usec(100);     // commands need > 37us to settle
}

void __LCD_write4bits(uint8_t value) {
    for (int i = 0; i < 4; i++) {
        GPIO_write(__LCD_data_pins[i], (value >> i) & 0x01);
    }

    __LCD_pulseEnable();
}

void __LCD_write8bits(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        GPIO_write(__LCD_data_pins[i], (value >> i) & 0x01);
    }

    __LCD_pulseEnable();
}
