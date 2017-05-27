#ifndef DEVICE_H_
#define DEVICE_H_

#include "Misc/src/definitions.h"
#include "stdbool.h"
#include "GPIO.h"

PRIVATE
    typedef void Driver;

PUBLIC
    typedef enum DriverName {
        GPIO,
        UART,
        SPI,
    } DriverName;

    // Module here recieves UART, GPIO, SPI, ... modules enums
    // example: `F4`, `U0`, `SPI3` from `GPIO`, `UART`, `SPI` enums
    typedef int Module;

    // NOTE: use this function only if you are using only the drivers WITHOUT any kernel
    Driver* Driver_construct( DriverName driverName, Module module );

    // check if the driver availablity
    bool Driver_isAvailable( DriverName driverName , Module module );

    // This is the same as calling the deinit function for that driver
    void Driver_deinit( DriverName driverName, Driver* driver );

/******************************************************************************/

PRIVATE
    bool __Driver_isPinAvailable( PORT_PIN port_pin );
    void __Driver_setPinFree( PORT_PIN port_pin );
    void __Driver_setPinBusy( PORT_PIN port_pin );

#endif // DEVICE_H_
