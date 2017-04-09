#ifndef __DRIVER_H_
#define __DRIVER_H_

#include "../driver.h"
#include "__IO.h"
#include <stdbool.h>

//typedef struct __Driver_data {
//    DriverName driverName;
//    Module module;
//    PORT port;
//} __Driver_data;

typedef struct __Driver {
    DriverName driverName;
    Module module;
    PORT port;
    byte config;
} __Driver;

void __Driver_saveConfig( Driver *driver, PORT port, byte config );
void __Driver_clearSavedConfig( Driver *driver );

#endif // __DRIVER_H_
