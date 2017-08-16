#include "GPIO.h"
#include "UART.h"
#include "SPI.h"
#include "Timer.h"
#include "ADC.h"
#include "Misc/src/definitions.h"
#include <stdbool.h>
#include "inner/__IO.h"

#define PORTLESS -1
#define EMPTY 0

// pointer of function represent the deinitation function of the drivers
typedef void (*__Driver_deinit_func)(void *driver);
typedef Driver* (*__Driver_getModule_func)( Module module );
typedef bool (*__Driver_isAvailable_func)( Module module );

typedef struct __Driver_func {
    __Driver_deinit_func deinit_func;
    __Driver_getModule_func getModule_func;
    __Driver_isAvailable_func isAvailable_func;

} __Driver_func;

// TODO: we need `__IO_config` array to be of different type
//static __Driver __IO_config[__PORTS_NUM] = { 0 };
//static __Driver __UART_config[__UART_MODULES_NUM] = { 0 };
//static __Driver __SPI_config[__SPI_MODULES_NUM] = { 0 };

__Driver_func __Drivers_func[] = {
    // GPIO
    { (__Driver_deinit_func)GPIO_deinit,
      (__Driver_getModule_func)__GPIO_getPin,
      (__Driver_isAvailable_func)__GPIO_isAvailable,
    },

    // UART
    { (__Driver_deinit_func)UART_deinit,
      (__Driver_getModule_func)__UART_getModule,
      (__Driver_isAvailable_func)__UART_isAvailable
    },

    // SPI
    { (__Driver_deinit_func)SPI_deinit,
      (__Driver_getModule_func)__SPI_getModule,
      (__Driver_isAvailable_func)__SPI_isAvailable
    },

    // ADC
    { (__Driver_deinit_func)ADC_deinit,
      (__Driver_getModule_func)__ADC_getModule,
      (__Driver_isAvailable_func)__ADC_isAvailable
    },

    //Timer
    { (__Driver_deinit_func)Timer_deinit,
      (__Driver_getModule_func)__Timer_getModule,
      (__Driver_isAvailable_func)__Timer_isAvailable}
};

static bool __IO_pinsAvailability[__PORTS_NUM * __PINS_PER_PORT] = { 0 };

//typedef struct __DriverData {
//    __Driver_deinit_func deinit_func;
//    __Driver* configurations_base_arr;
//}__DriverData;

// FIXME:  Timer
//static const __DriverData __driversData[] = {
//    { GPIO_deinit, __IO_config },         // GPIO
//    { UART_deinit, __UART_config },       // UART
//    { SPI_deinit, __SPI_config },         // SPI
//    { NULL, NULL },                       // Timer
//};

bool __Driver_isPinAvailable( PORT_PIN port_pin )
{
    byte pin_index = __GET_PIN_NUM(port_pin);

    return __IO_pinsAvailability[pin_index] is false ? true : false;
}

void __Driver_setPinFree( PORT_PIN port_pin )
{
    byte pin_index = __GET_PIN_NUM(port_pin);
    __IO_pinsAvailability[pin_index] = false;
}

void __Driver_setPinBusy( PORT_PIN port_pin )
{
    byte pin_index = __GET_PIN_NUM(port_pin);
    __IO_pinsAvailability[pin_index] = true;
}

//void __Driver_clearSavedConfig( Driver *driver )
//{
//    // clear IO config
//    ( __driversData[GPIO].configurations_base_arr + driver->port )->config &= ~driver->config;
//    // clear driver config
//    driver->config = 0;
//}

//bool Driver_isAvailable( DriverName driverName, Module module )
//{
//    byte * config = &(__driversData[driverName].configurations_base_arr + MODULE_NUM(module))->config;
//    bool isAvailable = true;

//    // check if the required
//    if( driverName is_not GPIO )
//        isAvailable = (*config is EMPTY);

//    else
//        isAvailable = ( (*config & __PINS_TO_BITS(__GET_PIN(module)) ) is EMPTY);

//    return isAvailable;
//}

bool Driver_isAvailable( DriverName driverName , Module module )
{
    return __Drivers_func[driverName].isAvailable_func(module);
}

Driver* Driver_construct( DriverName driverName, Module module )
{
    return __Drivers_func[driverName].getModule_func(module);
}

void Driver_deinit( DriverName driverName, Driver* driver )
{
    // call the deinit function
    __Drivers_func[driverName].deinit_func(driver);
}
