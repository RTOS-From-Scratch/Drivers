#include "__driver.h"
#include "../GPIO.h"
#include "../UART.h"
#include "../SPI.h"
#include "../Timer.h"
#include "Misc/src/definitions.h"
#include "__IO.h"

#define MODULE_NUM(module) ((byte)module)
#define PORTLESS -1
#define EMPTY 0

// pointer of function represent the deinitation function of the drivers
typedef void (*__Driver_deinit_func)(struct __Driver *driver);

// TODO: we need `__IO_config` array to be of different type
static __Driver __IO_config[__GET_PORTS_NUM] = { 0 };
static __Driver __UART_config[__UART_MODULES_NUM] = { 0 };
static __Driver __SPI_config[__SPI_MODULES_NUM] = { 0 };

typedef struct __DriverData {
    __Driver_deinit_func deinit_func;
    __Driver* configurations_base_arr;
}__DriverData;

// FIXME:  Timer
static const __DriverData __driversData[] = {
    { GPIO_deinit, __IO_config },         // GPIO
    { UART_deinit, __UART_config },       // UART
    { SPI_deinit, __SPI_config },         // SPI
    { NULL, NULL },                       // Timer
};

void __Driver_saveConfig( Driver *driver, PORT port, byte config )
{
    driver->port = port;

    // save configuration of the driver
    driver->config = config;
    // update `IO` availability
    ( __driversData[GPIO].configurations_base_arr + port )->config |= config;
}

void __Driver_clearSavedConfig( Driver *driver )
{
    // clear IO config
    ( __driversData[GPIO].configurations_base_arr + driver->port )->config &= ~driver->config;
    // clear driver config
    driver->config = 0;
}

bool Driver_isAvailable( DriverName driverName, Module module )
{
    byte * config = &(__driversData[driverName].configurations_base_arr + MODULE_NUM(module))->config;
    bool isAvailable = true;

    // check if the required
    if( driverName is_not GPIO )
        isAvailable = (*config is EMPTY);

    else
        isAvailable = ( (*config & __PINS_TO_BITS(__GET_PIN(module)) ) is EMPTY);

    return isAvailable;
}

Driver* Driver_construct( DriverName driverName, Module module )
{
    __Driver *driver;

    if( driverName is_not GPIO )
    {
        driver = __driversData[driverName].configurations_base_arr +
                           MODULE_NUM(module);
    }

    // TODO: memory management instead of that
    else
        driver = malloc(sizeof(__Driver));

    driver->driverName = driverName;
    driver->module = module;
    driver->config = 0;
    driver->port = PORTLESS;

    return driver;
}

void Driver_deinit(Driver* driver)
{
    // call the deinit function
    __driversData[driver->driverName].deinit_func(driver);
}
