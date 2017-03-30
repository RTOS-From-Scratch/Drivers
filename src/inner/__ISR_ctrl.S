.section .text
.ALIGN 0
.thumb
.syntax unified                            @ Force thumb syntax - `https://sourceware.org/binutils/docs/as/ARM_002dInstruction_002dSet.html#ARM_002dInstruction_002dSet`
.global __ISR_enable
.global __ISR_disable

__ISR_enable:
    CPSIE   I
    BX      LR

__ISR_disable:
    CPSID   I
    BX      LR
