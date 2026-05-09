 PRESERVE8
    THUMB

    AREA |.text|, CODE, READONLY

    ; void delay_cycles(uint32_t cycles)
rom_utility_delay_cycles  PROC
    EXPORT  rom_utility_delay_cycles

    ; cycles /= 4 
    LSRS    R0,R0,#2

    ; if 0 == cycles
    CMP R0,#0
    BEQ RET

LOOP    ;4 cycles
    MOV     R0,R0
    SUBS    R0,#1
    BNE     LOOP

RET
    BX      LR
    ENDP

    END