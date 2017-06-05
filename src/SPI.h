#ifndef SPI_H_
#define SPI_H_

#include "GPIO.h"
#include <stdint.h>
#include "inner/__SPI.h"
#include "Misc/src/definitions.h"
#include "driver.h"
#include "GPIO.h"

PUBLIC
    typedef enum SPI_t {
        SPI0,
        SPI1_PORTF,
        SPI2,
        SPI3,
        SPI1_PORTD,
    } SPI_t;

    // full speed -> is half the system clk speed
    typedef enum SPI_SPEED {
        SPI_SPEED_NOCONFIG  = __SPI_SPEED_NOCONFIG,
        SPI_SPEED_FULL      = __SPI_SPEED_FULL_CLK_RATE,
        SPI_SPEED_HALF      = __SPI_SPEED_HALF_CLK_RATE,
        SPI_SPEED_QUARTER   = __SPI_SPEED_QUARTER_CLK_RATE,
        SPI_SPEED_EIGHTH    = __SPI_SPEED_EIGHTH_CLK_RATE
    } SPI_SPEED;

    typedef enum SPI_ISR_MODE {
        SPI_ISR_MODE_Rx,
        SPI_ISR_MODE_Tx,
    } SPI_ISR_MODE;

    typedef struct SPI_Driver SPI_Driver;

    void SPI_initAsMaster( SPI_Driver *spi, SPI_SPEED speed, bool autoEnable );
    void SPI_enable( SPI_Driver *spi );
    void SPI_disable( SPI_Driver *spi );
    void SPI_ISR_initAsMaster( SPI_Driver *spi,
                               SPI_SPEED speed,
                               SPI_ISR_MODE ISR_mode,
                               ISR_PRIORITY priority,
                               void(*ISR_handler)(),
                               bool autoEnable );
    void SPI_ISR_enable( SPI_Driver *spi );
    void SPI_ISR_disable( SPI_Driver *spi );
    void SPI_ISR_changePriroity( SPI_Driver *spi, ISR_PRIORITY newPriority );
    // FIXME: not working
    void SPI_initAsSlave( SPI_Driver *spis );
    // speed must not be more than 1/2 of the system clk speed
    bool SPI_setClkSpeedManually( SPI_Driver *spi, uint32_t speed );
    void SPI_setClkSpeed( SPI_Driver *spi, SPI_SPEED speed );
    void SPI_setChipSelectState( SPI_Driver *spi, PIN_STATE state );
    byte SPI_transfer( SPI_Driver *spi, byte data );
    void SPI_deinit( SPI_Driver* spi );

/******************************************************************************/

PRIVATE
    // WARNING: These functions shouldn't be called or used
    const SPI_Driver* __SPI_getModule( SPI_t spi );
    bool __SPI_isAvailable( SPI_t spi );

#endif // SPI_H_
