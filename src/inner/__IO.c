#include "__IO.h"
#include "tm4c123gh6pm.h"
#include "Misc/src/definitions.h"

static byte __IO_pinsUsed[__PORTS_NUM] = {0};

void __IO_setPinsBusy(PORT port, byte pins)
{
    __IO_pinsUsed[port] |= pins;
}

void __IO_setPinsFree(PORT port, byte pins)
{
    __IO_pinsUsed[port] &= ~pins;
    // disable GPIO CLK
    // free port close its clock
    if( __IO_pinsUsed[port] == 0 )
        SYSCTL_RCGCGPIO_R &= ~(1 << port);
}

bool __IO_isPinsAvailable(PORT port, byte pins)
{
    if( (__IO_pinsUsed[port] & pins) is 0 )
        return true;
    else
        return false;
}
