#ifndef ISR_CTRL_H_
#define ISR_CTRL_H_

#include "Misc/src/definitions.h"
#include "inner/__ISR_vectortable.h"

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
    void __ISR_CTRL_enable( VT_ExceptionNumber exceptionNumber );
    void __ISR_CTRL_disable( VT_ExceptionNumber exceptionNumber );
    void __ISR_CTRL_setPriority( VT_ExceptionNumber exceptionNumber, ISR_PRIORITY priority );

    // These are declared in `inner/__ISR_ctrl.S`
    void ISR_enable();
    void ISR_disable();
    void ISR_WaitForInterrupt();

#endif // ISR_CTRL_H_
