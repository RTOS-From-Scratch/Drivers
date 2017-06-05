#ifndef __ISR_VECTORTABLE_H_
#define __ISR_VECTORTABLE_H_

enum VT_ExceptionNumber {
    ISR_RESET                   = 1,
    ISR_NMI,
    ISR_FAULT,
    ISR_MPU_FAULT,
    ISR_BUS_FAULT,
    ISR_USAGE_FAULT,
    ISR_SV_CALL                 = 11,
    ISR_DEBUG_MONITOR,
    ISR_PEND_SV                 = 14,
    ISR_SYSTICK,
    ISR_GPIO_PORT_A,
    ISR_GPIO_PORT_B,
    ISR_GPIO_PORT_C,
    ISR_GPIO_PORT_D,
    ISR_GPIO_PORT_E,
    ISR_UART_0,
    ISR_UART_1,
    ISR_SSI_0,
    ISR_I2C_0,
    ISR_PWM_0_FAULT,
    ISR_PWM_0_GENERATOR_0,
    ISR_PWM_0_GENERATOR_1,
    ISR_PWM_0_GENERATOR_2,
    ISR_QUADRATURE_ENCODER_0,
    ISR_ADC_0_SEQUENCE_0,
    ISR_ADC_0_SEQUENCE_1,
    ISR_ADC_0_SEQUENCE_2,
    ISR_ADC_0_SEQUENCE_3,
    ISR_WATCHDOG_TIMER,
    ISR_TIMER_0_SUBTIMER_A,
    ISR_TIMER_0_SUBTIMER_B,
    ISR_TIMER_1_SUBTIMER_A,
    ISR_TIMER_1_SUBTIMER_B,
    ISR_TIMER_2_SUBTIMER_A,
    ISR_TIMER_2_SUBTIMER_B,
    ISR_ANALOG_COMPERATOR_0,
    ISR_ANALOG_COMPERATOR_1,
    ISR_ANALOG_COMPERATOR_2,
    ISR_SYSTEM_CONTROL,
    ISR_FLASH_CONTROL,
    ISR_GPIO_PORT_F,
    ISR_UART_2                  = 49,
    ISR_SSI_1,
    ISR_TIMER_3_SUBTIMER_A,
    ISR_TIMER_3_SUBTIMER_B,
    ISR_I2C_1,
    ISR_QUADRATURE_ENCODER_1,
    ISR_CAN_0,
    ISR_CAN_1,
    ISR_HIBERNATE               = 59,
    ISR_USB_0,
    ISR_PWM_0_GENERATOR_3,
    ISR_UDMA_SOFTWARE_TRANSFER,
    ISR_UDMA_ERROR,
    ISR_ADC_1_SEQUENCE_0,
    ISR_ADC_1_SEQUENCE_1,
    ISR_ADC_1_SEQUENCE_2,
    ISR_ADC_1_SEQUENCE_3,
    ISR_SSI_2                   = 73,
    ISR_SSI_3,
    ISR_UART_3,
    ISR_UART_4,
    ISR_UART_5,
    ISR_UART_6,
    ISR_UART_7,
    ISR_I2C_2                   = 84,
    ISR_I2C_3,
    ISR_TIMER_4_SUBTIMER_A,
    ISR_TIMER_4_SUBTIMER_B,
    ISR_TIMER_5_SUBTIMER_A      = 108,
    ISR_TIMER_5_SUBTIMER_B,
    ISR_WIDE_TIMER_0_SUBTIMER_A,
    ISR_WIDE_TIMER_0_SUBTIMER_B,
    ISR_WIDE_TIMER_1_SUBTIMER_A,
    ISR_WIDE_TIMER_1_SUBTIMER_B,
    ISR_WIDE_TIMER_2_SUBTIMER_A,
    ISR_WIDE_TIMER_2_SUBTIMER_B,
    ISR_WIDE_TIMER_3_SUBTIMER_A,
    ISR_WIDE_TIMER_3_SUBTIMER_B,
    ISR_WIDE_TIMER_4_SUBTIMER_A,
    ISR_WIDE_TIMER_4_SUBTIMER_B,
    ISR_WIDE_TIMER_5_SUBTIMER_A,
    ISR_WIDE_TIMER_5_SUBTIMER_B,
    ISR_FPU,
    ISR_PWM_1_GENERATOR_0       = 150,
    ISR_PWM_1_GENERATOR_1,
    ISR_PWM_1_GENERATOR_2,
    ISR_PWM_1_FAULT,
};

typedef enum VT_ExceptionNumber VT_ExceptionNumber;

void __ISR_vectorTable_init();
void __ISR_register(VT_ExceptionNumber exception_num, void(*handler)(void));

#endif // NANOKERNEL_ISR_VECTORTABLE_H_
