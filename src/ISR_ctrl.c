#include "ISR_ctrl.h"
#include "inner/__IO.h"
#include "tm4c123gh6pm.h"

// NVIC baseaddress
#define __ISR_CTRL_NVIC_baseAddress 0xE000E000
// EN0
#define __ISR_CTRL_enable0_offset 0x100
// DIS0
#define __ISR_CTRL_disable0_offset 0x180
// PRI0
#define __ISR_CTRL_priority0_offset 0x400

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


void __ISR_CTRL_enable(VT_ExceptionNumber exceptionNumber)
{
    byte IRQ = exceptionNumber - 16;
    REG_VALUE(__ISR_CTRL_GET_ENABLE_ADDRESS(IRQ)) |= ( 1 << __ISR_CTRL_GET_ENABLE_INDEX(IRQ) );
}

void __ISR_CTRL_disable(VT_ExceptionNumber exceptionNumber)
{
    byte IRQ = exceptionNumber - 16;
    REG_VALUE(__ISR_CTRL_GET_ENABLE_ADDRESS(IRQ)) &= ~( 1 << __ISR_CTRL_GET_ENABLE_INDEX(IRQ) );
}

void __ISR_CTRL_setPriority( VT_ExceptionNumber exceptionNumber, ISR_PRIORITY priority )
{
    byte IRQ = exceptionNumber - 16;

    // clear priority first
    REG_VALUE(__ISR_CTRL_GET_PRIORITY_ADDRESS( IRQ )) &= ~
            ( 0x7 << __ISR_CTRL_GET_PRIORITY_index( IRQ ) );

    // set new priority
    REG_VALUE(__ISR_CTRL_GET_PRIORITY_ADDRESS( IRQ )) |=
            ( priority << __ISR_CTRL_GET_PRIORITY_index( IRQ ) );

    REG_VALUE(__ISR_CTRL_GET_ENABLE_ADDRESS(IRQ)) |= ( 1 << __ISR_CTRL_GET_ENABLE_INDEX(IRQ) );
}
