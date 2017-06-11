#ifndef LCD_h
#define LCD_h

#include <inttypes.h>
#include <stddef.h>
#include "Misc/src/definitions.h"
#include "../driver.h"
#include "../GPIO.h"

PUBLIC
//    LCD(uint8_t rs, uint8_t enable,
//                uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
//                uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
//    LCD(uint8_t rs, uint8_t rw, uint8_t enable,
//                uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
//                uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
//    LCD(uint8_t rs, uint8_t rw, uint8_t enable,
//                uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
//    LCD(uint8_t rs, uint8_t enable,
//                uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

    void LCD_init( PORT_PIN rs, PORT_PIN rw, PORT_PIN enable,
                   PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3,
                   PORT_PIN d4, PORT_PIN d5, PORT_PIN d6, PORT_PIN d7 );

    void LCD_initWithoutRW( PORT_PIN rs, PORT_PIN enable,
                            PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3,
                            PORT_PIN d4, PORT_PIN d5, PORT_PIN d6, PORT_PIN d7 );

    void LCD_init4Bits( PORT_PIN rs, PORT_PIN rw, PORT_PIN enable,
                        PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3 );

    void LCD_init4BitsWithoutRW( PORT_PIN rs, PORT_PIN enable,
                                 PORT_PIN d0, PORT_PIN d1, PORT_PIN d2, PORT_PIN d3 );

    void LCD_begin( uint8_t cols, uint8_t rows, uint8_t charsize/* = LCD_5x8DOTS*/ );

    void LCD_clear();
    void LCD_home();

    void LCD_noDisplay();
    void LCD_display();
    void LCD_noBlink();
    void LCD_blink();
    void LCD_noCursor();
    void LCD_cursor();
    void LCD_scrollDisplayLeft();
    void LCD_scrollDisplayRight();
    void LCD_leftToRight();
    void LCD_rightToLeft();
    void LCD_autoscroll();
    void LCD_noAutoscroll();

    void LCD_setRowOffsets( int row1, int row2, int row3, int row4 );
    void LCD_createChar( uint8_t, uint8_t[] );
    void LCD_setCursor( uint8_t, uint8_t );
    size_t LCD_write( uint8_t );
    size_t LCD_writeInt( int num );
    size_t LCD_print( char* data );

    void LCD_command( uint8_t );

#endif
