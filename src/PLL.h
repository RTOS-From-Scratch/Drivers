#ifndef PLL_H_
#define PLL_H_

#include <Misc/src/definitions.h>
#include <stdint.h>

/* Configure Phase Lock Loop
 * configure the use of the Main oscillator
 */

PUBLIC
    typedef enum ClockSpeed {
        ClockSpeed_MHZ_80          = 80,
        ClockSpeed_MHZ_50          = 50,
        ClockSpeed_MHZ_40          = 40,
        ClockSpeed_MHZ_25          = 25,
        ClockSpeed_MHZ_20          = 20,
        ClockSpeed_MHZ_16          = 16,
        ClockSpeed_MHZ_10          = 10,
        ClockSpeed_MHZ_8           = 8,
        ClockSpeed_MHZ_5           = 5,
        ClockSpeed_MHZ_4           = 4,
        // TODO: this doesn't work yet
        ClockSpeed_MHZ_internal    = ClockSpeed_MHZ_16,
    } ClockSpeed;

    void PLL_setClockSpeed( ClockSpeed clk );
    ClockSpeed PLL_getClockSpeed();
    //void PLL_setClockSpeedManual( byte XTAL, byte SYSDIV2 );

#endif // PLL_H_
