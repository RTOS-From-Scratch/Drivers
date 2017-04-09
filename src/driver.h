#ifndef DEVICE_H_
#define DEVICE_H_

#include "Misc/src/definitions.h"
#include "IO.h"
#include "stdbool.h"

typedef enum DriverName {
    GPIO,
    UART,
    SPI,
    Timer
} DriverName;

// Module here recieves UART, GPIO, SPI, ... modules enums
// example: `F4`, `U0`, `SPI3` from `GPIO`, `UART`, `SPI` enums
typedef int Module;

typedef struct __Driver Driver;

// NOTE: use this function only if you are using only the drivers WITHOUT any kernel
Driver* Driver_construct( DriverName driverName, Module module );

// check if the driver availablity
bool Driver_isAvailable( DriverName driverName , Module module );

// This is the same as calling the deinit function for that driver
void Driver_deinit(Driver* driver);

#endif // DEVICE_H_
