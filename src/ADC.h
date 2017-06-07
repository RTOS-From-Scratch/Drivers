#ifndef ADC_H_
#define ADC_H_

#include "Misc/src/definitions.h"
#include <stdbool.h>
#include "GPIO.h"

PUBLIC
    typedef enum ADC_t {
        ADC0,        // E3
        ADC1,        // E2
        ADC2,        // E1
        ADC3,        // E0
        ADC4,        // D3
        ADC5,        // D2
        ADC6,        // D1
        ADC7,        // D0
        ADC8,        // E5
        ADC9,        // E4
        ADC10,       // B4
        ADC11,       // B5
    } ADC_t;

    typedef enum ADC_Sample_Rate {
        ADC_Sample_Rate_125KsPs = 0x1,
        ADC_Sample_Rate_250KsPs = 0x3,
        ADC_Sample_Rate_500KsPs = 0x5,
        ADC_Sample_Rate_1MsPs   = 0x7,
    } ADC_Sample_Rate;

    typedef struct ADC_Driver ADC_Driver;

    void ADC_init( ADC_Driver *adc,
                   ADC_Sample_Rate sampleRate,
                   bool autoEnable );
    void ADC_enable( ADC_Driver *adc );
    void ADC_disable( ADC_Driver *adc );
    int32_t ADC_read( ADC_Driver *adc );
    void ADC_deinit( ADC_Driver *adc );

PRIVATE
    // WARNING: These functions shouldn't be called or used
    const ADC_Driver* __ADC_getModule( ADC_t adc );
    bool __ADC_isAvailable( ADC_t adc );

#endif // ADC_H_
