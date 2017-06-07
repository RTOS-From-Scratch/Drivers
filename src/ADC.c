#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "inner/__IO.h"
#include <stdint.h>
#include "inner/__ISR_vectortable.h"
#include "driver.h"
#include <stddef.h>
#include "Misc/src/assert.h"

#define __ADC_MODULES_NUM 8

#define ADC0 0
#define ADC1 1

#define SEQ0 0
#define SEQ1 1
#define SEQ2 2
#define SEQ3 3

#define NOT_DEFINED_YET 0

struct ADC_Driver {
    byte module_num;
    byte sequencer_num;
    byte ADC_input_num;
    byte ISR_vectorNum;
};

static struct ADC_Driver ADCs[__ADC_MODULES_NUM] = {
    { ADC0, SEQ0, NOT_DEFINED_YET, ISR_ADC_0_SEQUENCE_0 },
    { ADC0, SEQ1, NOT_DEFINED_YET, ISR_ADC_0_SEQUENCE_1 },
    { ADC0, SEQ2, NOT_DEFINED_YET, ISR_ADC_0_SEQUENCE_2 },
    { ADC0, SEQ3, NOT_DEFINED_YET, ISR_ADC_0_SEQUENCE_3 },
    { ADC1, SEQ0, NOT_DEFINED_YET, ISR_ADC_1_SEQUENCE_0 },
    { ADC1, SEQ1, NOT_DEFINED_YET, ISR_ADC_1_SEQUENCE_1 },
    { ADC1, SEQ2, NOT_DEFINED_YET, ISR_ADC_1_SEQUENCE_2 },
    { ADC1, SEQ3, NOT_DEFINED_YET, ISR_ADC_1_SEQUENCE_3 },
};

const static uint16_t __ADC_inputs[] = {
    E3,             // ADC0
    E2,             // ADC1
    E1,             // ADC2
    E0,             // ADC3
    D3,             // ADC4
    D2,             // ADC5
    D1,             // ADC6
    D0,             // ADC7
    E5,             // ADC8
    E4,             // ADC9
    B4,             // ADC10
    B5,             // ADC11
};

typedef enum __ADC_Properties_t
{
    __ADC_ACTIVE_SAMPLE_SEQ                             = 0x000,
    __ADC_RAW_INTERRUPT_STATUS                          = 0x004,
    __ADC_INTERRUPT_MASK                                = 0x008,
    __ADC_INTERRUPT_STATUS_AND_CLEAR                    = 0x00C,
    __ADC_OVERFLOW_STATUS                               = 0x010,
    __ADC_EVENT_MULTIPLEXER_EVENT                       = 0x014,
    __ADC_UNDERFLOW_STATUS                              = 0x018,
    __ADC_TRIGGER_SOURCE_SEL                            = 0x01C,
    __ADC_SAMPLE_SEQ_PRI                                = 0x020,
    __ADC_SAMPLE_PHASE_CTRL                             = 0x024,
    __ADC_PROCESSOR_SAMPLE_SEQ_INIT                     = 0x028,
    __ADC_SAMPLE_AVERAGE_CTRL                           = 0x030,
    __ADC_DIGITAL_COMPARATOR_INTERRUPT_STATUS_AND_CLEAR = 0x034,
    __ADC_CTRL                                          = 0x038,
    __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL_0            = 0x040,
    __ADC_SAMPLE_SEQ_CTRL_0                             = 0x044,
    __ADC_SAMPLE_SEQ_RESULT_FIFO_0                      = 0x048,
    __ADC_SAMPLE_SEQ_FIFO_STATUS_0                      = 0x04C,
    __ADC_SAMPLE_SEQ_OPERATION_0                        = 0x050,
    __ADC_SAMPLE_SEQ_DIGITAL_COMPARATOR_SEL_0           = 0x054,
    __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL_1            = 0x060,
    __ADC_SAMPLE_SEQ_CTRL_1                             = 0x064,
    __ADC_SAMPLE_SEQ_RESULT_FIFO_1                      = 0x068,
    __ADC_SAMPLE_SEQ_FIFO_STATUS_1                      = 0x06C,
    __ADC_SAMPLE_SEQ_OPERATION_1                        = 0x070,
    __ADC_SAMPLE_SEQ_DIGITAL_COMPARATOR_SEL_1           = 0x074,
    __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL_2            = 0x080,
    __ADC_SAMPLE_SEQ_CTRL_2                             = 0x084,
    __ADC_SAMPLE_SEQ_RESULT_FIFO_2                      = 0x088,
    __ADC_SAMPLE_SEQ_FIFO_STATUS_2                      = 0x08C,
    __ADC_SAMPLE_SEQ_OPERATION_2                        = 0x090,
    __ADC_SAMPLE_SEQ_DIGITAL_COMPARATOR_SEL_2           = 0x094,
    __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL_3            = 0x0A0,
    __ADC_SAMPLE_SEQ_CTRL_3                             = 0x0A4,
    __ADC_SAMPLE_SEQ_RESULT_FIFO_3                      = 0x0A8,
    __ADC_SAMPLE_SEQ_FIFO_STATUS_3                      = 0x0AC,
    __ADC_SAMPLE_SEQ_OPERATION_3                        = 0x0B0,
    __ADC_SAMPLE_SEQ_DIGITAL_COMPARATOR_SEL_3           = 0x0B4,
    __ADC_DIGITAL_COMPARATOR_RESET_INITIAL_CONDITIONS   = 0xD00,
    __ADC_DIGITAL_COMPARATOR_CTRL_0                     = 0xE00,
    __ADC_DIGITAL_COMPARATOR_CTRL_1                     = 0xE04,
    __ADC_DIGITAL_COMPARATOR_CTRL_2                     = 0xE08,
    __ADC_DIGITAL_COMPARATOR_CTRL_3                     = 0xE0C,
    __ADC_DIGITAL_COMPARATOR_CTRL_4                     = 0xE10,
    __ADC_DIGITAL_COMPARATOR_CTRL_5                     = 0xE14,
    __ADC_DIGITAL_COMPARATOR_CTRL_6                     = 0xE18,
    __ADC_DIGITAL_COMPARATOR_CTRL_7                     = 0xE1C,
    __ADC_DIGITAL_COMPARATOR_RANGE_0                    = 0xE40,
    __ADC_DIGITAL_COMPARATOR_RANGE_1                    = 0xE44,
    __ADC_DIGITAL_COMPARATOR_RANGE_2                    = 0xE48,
    __ADC_DIGITAL_COMPARATOR_RANGE_3                    = 0xE4C,
    __ADC_DIGITAL_COMPARATOR_RANGE_4                    = 0xE50,
    __ADC_DIGITAL_COMPARATOR_RANGE_5                    = 0xE54,
    __ADC_DIGITAL_COMPARATOR_RANGE_6                    = 0xE58,
    __ADC_DIGITAL_COMPARATOR_RANGE_7                    = 0xE5C,
    __ADC_PERIPHERAL_PROPERTIES                         = 0xFC0,
    __ADC_PERIPHERAL_CONFIG                             = 0xFC4,
    __ADC_CLK_CONFIG                                    = 0xFC8,
} __ADC_Properties_t;

unsigned long __ADC_MODULES_ADDR[__ADC_MODULES_NUM] = {
    0x40038000,             // ADC0
    0x40039000              // ADC1
};

#define ADC_SSPRI_SS0_S 0
#define ADC_SSPRI_SS1_S 4
#define ADC_SSPRI_SS2_S 8
#define ADC_SSPRI_SS3_S 12

// calculate ADC Sample sequencers options offsets
#define __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL(sequencer_num) \
    ( __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL_0 + (0x20 * sequencer_num) )
#define __ADC_SAMPLE_SEQ_CTRL(sequencer_num) \
    ( __ADC_SAMPLE_SEQ_CTRL_0 + (0x20 * sequencer_num) )
#define __ADC_SAMPLE_SEQ_RESULT_FIFO(sequencer_num) \
    ( __ADC_SAMPLE_SEQ_RESULT_FIFO_0 + (0x20 * sequencer_num) )
#define __ADC_SAMPLE_SEQ_FIFO_STATUS(sequencer_num) \
    ( __ADC_SAMPLE_SEQ_FIFO_STATUS_0 + (0x20 * sequencer_num) )
#define __ADC_SAMPLE_SEQ_OPERATION(sequencer_num) \
    ( __ADC_SAMPLE_SEQ_OPERATION_0 + (0x20 * sequencer_num) )
#define __ADC_SAMPLE_SEQ_DIGITAL_COMPARATOR_SEL(sequencer_num) \
    ( __ADC_SAMPLE_SEQ_DIGITAL_COMPARATOR_SEL_0 + (0x20 * sequencer_num) )

void ADC_init( ADC_Driver* adc,
               ADC_Sample_Rate sampleRate,
               bool autoEnable )
{
    ASSERT(adc is_not NULL);

    byte port = __GET_PORT(__ADC_inputs[adc->ADC_input_num]);
    byte pin  = __GET_PIN(__ADC_inputs[adc->ADC_input_num]);
    uintptr_t ioAddress = __IO_PORTS_ADDR[port];
    uint32_t bit_specific = 1 << pin;
    uint32_t bit_specific_complemented = ~bit_specific;
    byte module_num = adc->module_num;
    byte sequencer_num = adc->sequencer_num;

    uintptr_t moduleAddress = __ADC_MODULES_ADDR[module_num];

/**************************** CLK configuration *****************************/
    // enable GPIO clock and wait to stablize
    SYSCTL_RCGCGPIO_R |= 1 << port;
    while( (SYSCTL_PPGPIO_R & (1 << port)) == 0);

    if( ( SYSCTL_RCGCADC_R & (1 << module_num) ) is 0 )
    {
        // enable ADC0 clock and wait to stablize
        SYSCTL_RCGCADC_R |= 1 << module_num;
        while( (SYSCTL_PPADC_R & (1 << module_num)) == 0);
    }
/****************************************************************************/

/********************************** GPIO ***********************************/
    // direction: input
    IO_REG(ioAddress, __IO_DIRECTION) &= bit_specific_complemented;

    // disable digitial
    IO_REG(ioAddress, __IO_DIGITAL_ENABLE) &= bit_specific_complemented;

    if(autoEnable)
    {
        // enable alternative funtionality
        IO_REG(ioAddress, __IO_ALTERNATIVE_FUNC_SEL) |= bit_specific;
        // enable analog
        IO_REG(ioAddress, __IO_ANALOG_MODE_SEL) |= bit_specific;
    }
/***************************************************************************/

/*********************************** ADC ************************************/
    // disable sequencer
    // TODO: need flexability in using other sequencers
    IO_REG(moduleAddress, __ADC_ACTIVE_SAMPLE_SEQ) &= ~(1 << sequencer_num);

    // set sample rate
    IO_REG(moduleAddress, __ADC_PERIPHERAL_CONFIG) =
            ( IO_REG(moduleAddress, __ADC_PERIPHERAL_CONFIG) & ~0xF ) + sampleRate;

    // The triggering mode
    // clear trigger mode first
    IO_REG(moduleAddress, __ADC_EVENT_MULTIPLEXER_EVENT) &= ~(0xF << (sequencer_num * 4));
//    if( trigger_mode is ADC_TRIGGER_MODE_ON_PURPOSE )
    IO_REG(moduleAddress, __ADC_EVENT_MULTIPLEXER_EVENT) |=
            (ADC_EMUX_EM0_PROCESSOR << (sequencer_num * 4));
//    else
//        IO_REG(moduleAddress, __ADC_EVENT_MULTIPLEXER_EVENT) |= ADC_EMUX_EM0_ALWAYS;

    // configure the corresponding ADC input number (Ain2 -> 0x2)
    // this register specifiy which pin will the sequencer gets its data
    // clear it first
    IO_REG(moduleAddress, __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL(sequencer_num)) &= ~0xF;
    IO_REG(moduleAddress, __ADC_SAMPLE_SEQ_INPUT_MULTIPLEXER_SEL(sequencer_num)) |= adc->ADC_input_num;

    // enable End bit - must be set
    // all the sequencer End bit has the same index
    IO_REG(moduleAddress, __ADC_SAMPLE_SEQ_CTRL(sequencer_num)) |= ADC_SSCTL0_END0;

    if(autoEnable)
        IO_REG(moduleAddress, __ADC_ACTIVE_SAMPLE_SEQ) |= (1 << sequencer_num);
/****************************************************************************/
}

int32_t ADC_read( ADC_Driver *adc )
{
    byte port = __GET_PORT(__ADC_inputs[adc->ADC_input_num]);
    byte pin = __GET_PIN(__ADC_inputs[adc->ADC_input_num]);
    byte sequencer_num = adc->sequencer_num;
    uint32_t module_address = __ADC_MODULES_ADDR[adc->module_num];

    // check if the driver has been initiated
    if( ( IO_REG(__IO_PORTS_ADDR[port], __IO_ANALOG_MODE_SEL) & (1 << pin) ) is 0 )
        return -1;

    // check if the driver is not enabled
    if( ( IO_REG(module_address, __ADC_ACTIVE_SAMPLE_SEQ) & (1 << sequencer_num) ) is 0 )
        return -1;

    // initiate the sequencer
    IO_REG(module_address, __ADC_PROCESSOR_SAMPLE_SEQ_INIT) |= ( 1 << sequencer_num );

    // wait for data availability
    while( (IO_REG(module_address, __ADC_SAMPLE_SEQ_FIFO_STATUS(sequencer_num)) & ADC_SSFSTAT0_EMPTY) is_not 0 );

    // read the data
    return IO_REG(module_address, __ADC_SAMPLE_SEQ_RESULT_FIFO(sequencer_num)) & 0x00000FFF;
}

void ADC_enable( ADC_Driver *adc )
{
    byte port = __GET_PORT(__ADC_inputs[adc->ADC_input_num]);
    byte pin  = __GET_PIN(__ADC_inputs[adc->ADC_input_num]);

    // enable analog
    IO_REG(__IO_PORTS_ADDR[port], __IO_ANALOG_MODE_SEL) |= 1 << pin;
    // enable alternative fucntion
    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) |= 1 << pin;

    // enable sequencer
    IO_REG(__ADC_MODULES_ADDR[adc->module_num], __ADC_ACTIVE_SAMPLE_SEQ) |= (1 << adc->sequencer_num);
}

void ADC_disable( ADC_Driver *adc )
{
    byte port = __GET_PORT(__ADC_inputs[adc->ADC_input_num]);
    byte pin  = __GET_PIN(__ADC_inputs[adc->ADC_input_num]);

    // disable analog
    IO_REG(__IO_PORTS_ADDR[port], __IO_ANALOG_MODE_SEL) &= ~(1 << pin);
    // disable alternative fucntion
    IO_REG(__IO_PORTS_ADDR[port], __IO_ALTERNATIVE_FUNC_SEL) &= ~(1 << pin);

    // disable sequencer
    IO_REG(__ADC_MODULES_ADDR[adc->module_num], __ADC_ACTIVE_SAMPLE_SEQ) &= ~(1 << adc->sequencer_num);
}

const ADC_Driver* __ADC_getModule( ADC_t adc )
{
    ADC_Driver* driver = NULL;

    if( __ADC_isAvailable(adc) is false )
        return NULL;

    // search for the next available driver
    for(byte iii = 0; iii < __ADC_MODULES_NUM; iii++)
    {
        if( ADCs[iii].ADC_input_num is NOT_DEFINED_YET )
        {
            driver =  &ADCs[iii];
            break;
        }
    }

    if( driver is NULL )
        return NULL;

    driver->ADC_input_num = adc;

    // call Driver APIs to mark pins busy.
    __Driver_setPinBusy( driver->ADC_input_num );

    return driver;
}

bool __ADC_isAvailable( ADC_t adc )
{
    // call Driver APIs to check pin availability.
    if( ! __Driver_isPinAvailable(__ADC_inputs[adc]) )
        return false;

    return true;
}

void ADC_deinit( ADC_Driver *adc )
{
    // disable interrupt
    // critical section
    // ISR_disable();

    byte driver_index = adc->module_num is ADC0 ? ADC0 : ADC0 + 4;
    byte sequencers_num = 0;

/*********************************** ADC ***********************************/
    for(byte iii = driver_index; iii < driver_index + 4; ++iii)
    {
        if( ADCs[iii].ADC_input_num is_not NOT_DEFINED_YET )
            sequencers_num++;
    }

    if( sequencers_num EQUAL 1 )
        SYSCTL_RCGCADC_R &= ~(1 << adc->module_num);

//    IO_REG(__ADC_MODULES_ADDR[adc->module_num], __ADC_ACTIVE_SAMPLE_SEQ) |=
//            (1 << adc->sequencer_num);
/***************************************************************************/

/*********************************** GPIO ***********************************/
    ADC_disable(adc);
/****************************************************************************/

/********************************* Interrupt *********************************/
    // disable interrupts if it's enabled
    // TODO: Global interrupt
//    ADC_ISR_disable(adc);
/*****************************************************************************/

/******************************** free pins *********************************/
    __Driver_setPinFree(__ADC_inputs[adc->ADC_input_num]);
/****************************************************************************/

    // clear adc input from the module
    adc->ADC_input_num = NOT_DEFINED_YET;

    // re-enable interrupts
//    ISR_enable();
}
