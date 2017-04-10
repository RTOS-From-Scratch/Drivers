.section .text
.ALIGN 0
.thumb
.syntax unified           @ Force thumb syntax - `https://sourceware.org/binutils/docs/as/ARM_002dInstruction_002dSet.html#ARM_002dInstruction_002dSet`
.global ISR_enable
.global ISR_disable
.global ISR_WaitForInterrupt

ISR_enable:
    CPSIE   I
    BX      LR

ISR_disable:
    CPSID   I
    BX      LR

ISR_WaitForInterrupt:
    WFI
    BX      LR
