.section .data
.align 8
perm_entry_s:
    .half 2
    .dword _entry
    .byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF

mode_entry_s:
    .dword supervisor
    .dword 0
    .half 2
    .long 0xFFFFFFFFFFFFFFFF

register_safe_msm:
    .fill 32, 8, 0

register_safe_s:
    .fill 32, 8, 0

.section .text
.global _entry
_entry:
    ecall #call into control core for setup

.section .handlers
.global cc_handler
cc_handler:
    li t1, 119
    flcsr t0, t1, t2
    li t2, 1
    blt t2, t0, no_init

    la a0, perm_entry_s
    li a1, 2
    plbstore a1, a0
    la a0, mode_entry_s
    mlbstore a1, a0

    li t1, 119
    li t0, 2
    f_scsr t2, t1, t0
    li t1, 82
    la t2, supervisor
    f_scsr t0, t1, t2
    mret

no_init:
    li t1, 82
    flcsr t0, t1, t2
    addi t0, t0, 4
    li t1, 2
    mepcstore t0, t1
    li t1, 2
    mepcload t0, t1
    li t1, 82
    f_scsr t2, t1, t0
    li t1, 119
    li t0, 2
    f_scsr t2, t1, t0

    mret


.section .text
.global supervisor
supervisor:
    ecall
