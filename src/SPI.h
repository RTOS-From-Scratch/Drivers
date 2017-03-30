#ifndef SPI_H_
#define SPI_H_

#include "IO.h"
#include <stdint.h>
#include "inner/__SPI.h"

//typedef enum SPI_MODULES {
//    SPI_MODULE_0,
//    SPI_MODULE_1_PORT_F = 1,
//    SPI_MODULE_1_PORT_D = 1,
//    SPI_MODULE_2,
//    SPI_MODULE_3,
//} SPI_MODULES;

//#define SPI_MODULE_t    SPI_MODULES module_num, PORT_PIN
//#define SPI0            0, __SPI0_CLK
//#define SPI1_PORTF      1, __SPI1_PORTF_CLK
//#define SPI1_PORTD      1, __SPI1_PORTD_CLK
//#define SPI2            2, __SPI2_CLK
//#define SPI3            3, __SPI3_CLK

//#define __SPI0_CLK            A2
//#define __SPI1_PORTF_CLK      F2
//#define __SPI1_PORTD_CLK      D0
//#define __SPI2_CLK            B4
//#define __SPI3_CLK            D0

// CLK pin | CLK port | module_num
typedef enum SPI_t {
    SPI0        = 0x020000,
    SPI1_PORTF  = 0x020501,
    SPI1_PORTD  = 0x000301,
    SPI2        = 0x040102,
    SPI3        = 0x000303,
} SPI_t;

typedef enum SPI_SPEED {
    SPI_SPEED_FULL      = __SPI_SPEED_FULL_CLK_RATE,
    SPI_SPEED_HALF      = __SPI_SPEED_HALF_CLK_RATE,
    SPI_SPEED_QUARTER   = __SPI_SPEED_QUARTER_CLK_RATE,
    SPI_SPEED_EIGHTH    = __SPI_SPEED_EIGHTH_CLK_RATE
} SPI_SPEED;

typedef enum SPI_MODE_t {
    SPI_MODE_Tx,
    SPI_MODE_Rx,
    SPI_MODE_TxRx
}SPI_MODE_t;

void SPI_initAsMaster( SPI_t spi_module, SPI_SPEED speed, SPI_MODE_t mode );
void SPI_initAsSlave( SPI_t spi_module );
void SPI_write( SPI_t spi_module, byte data );
byte SPI_read( SPI_t spi_module );
void SPI_deinit( SPI_t spi_module );

#endif // SPI_H_
