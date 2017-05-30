#ifndef ISR_CTRL_H_
#define ISR_CTRL_H_

#include "Misc/src/definitions.h"

PUBLIC
    typedef enum ISR_PRIORITY {
        ISR_PRIORITY_0,
        ISR_PRIORITY_1,
        ISR_PRIORITY_2,
        ISR_PRIORITY_3,
        ISR_PRIORITY_4,
        ISR_PRIORITY_5,
        ISR_PRIORITY_6,
        ISR_PRIORITY_7,
    } ISR_PRIORITY;

PRIVATE
    // NVIC baseaddress
    static const intptr_t __ISR_CTRL_NVIC_baseAddress = 0xE000E000;
    // EN0
    static const short __ISR_CTRL_enable0_offset = 0x100;
    // DIS0
    static const short __ISR_CTRL_disable0_offset = 0x180;
    // PRI0
    static const short __ISR_CTRL_priority0_offset = 0x400;

    #define __ISR_CTRL_GET_ENABLE_ADDRESS(IRQ) ( (unsigned long)( ((IRQ / 32) * 4) + \
                                                 __ISR_CTRL_enable0_offset + \
                                                 __ISR_CTRL_NVIC_baseAddress ) \
                                                )

    #define __ISR_CTRL_GET_ENABLE_INDEX(IRQ) ( IRQ % 32 )

    #define __ISR_CTRL_GET_DISABLE_ADDRESS(IRQ) ( ((IRQ / 32) * 4) + \
                                                 __ISR_CTRL_disable0_offset + \
                                                 __ISR_CTRL_NVIC_baseAddress )

    #define __ISR_CTRL_GET_DISABLE_INDEX(IRQ) ( IRQ % 32 )

    #define __ISR_CTRL_GET_PRIORITY_ADDRESS(IRQ) ( ((IRQ / 4) * 4) + \
                                                   __ISR_CTRL_priority0_offset + \
                                                   __ISR_CTRL_NVIC_baseAddress )

    #define __ISR_CTRL_GET_PRIORITY_index(IRQ) ( ((IRQ % 4) * 3) + \
                                                 (((IRQ % 4) + 1) * 5) \
                                               )

    // These are declared in `inner/__ISR_ctrl.S`
    void ISR_enable();
    void ISR_disable();
    void ISR_WaitForInterrupt();

#endif // ISR_CTRL_H_
