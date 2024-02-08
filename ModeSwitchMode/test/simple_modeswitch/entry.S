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

perm_entry_u:
    .half 3
    .dword _entry
    .byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF

mode_entry_u:
    .dword userland
    .dword 0
    .half 3
    .long 0xFFFFFFFFFFFFFFFF

register_safe_msm:
    .fill 32, 8, 0

register_safe_s:
    .fill 32, 8, 0

.section .text
.global _entry
_entry:
    li t0, 1
	la t1, trap_handler
	slli t0, t0, 2
	or t0, t0, t1
	csrw mtvec, t0     

    la a0, perm_entry_s
    li a1, 2
    plbstore a1, a0
    la a0, mode_entry_s
    mlbstore a1, a0

    la a0, perm_entry_u
    li a1, 3
    plbstore a1, a0
    la a0, mode_entry_u
    mlbstore a1, a0


    li t1, 119
    li t0, 2
    f_scsr t2, t1, t0
    la t0, supervisor
    csrw mepc, t0
    mret

.global trap_handler
trap_handler:
    csrrw a0, sscratch, a0
    la a0, register_safe_s
    sw t0, 32(a0)
    sw t1, 40(a0)
    sw t2, 48(a0)
    csrrw a0, sscratch, a0

    li t1, 119
    flcsr t0, t1, t2
    li t2, 3
    beq t2, t0, enter_supervisor
    li t2, 2
    beq t2, t0, enter_userland

enter_supervisor:
    li t1, 82
    flcsr t0, t1, t2
    addi t0, t0, 4
    li t1, 3
    mepcstore t0, t1
    li t1, 119
    li t0, 2
    f_scsr t2, t1, t0

    li t1, 2
    mepcload t0, t1
    li t1, 82
    f_scsr t2, t1, t0
    
    csrrw a0, sscratch, a0
    lw t0, 32(a0)
    lw t1, 40(a0)
    lw t2, 48(a0)
    csrrw a0, sscratch, a0
    
    mret

enter_userland:
    li t1, 82
    flcsr t0, t1, t2
    addi t0, t0, 4
    li t1, 2
    mepcstore t0, t1
    li t1, 119
    li t0, 3
    f_scsr t2, t1, t0

    la t0, userland
    csrw mepc, t0
    
    csrrw a0, sscratch, a0
    lw t0, 32(a0)
    lw t1, 40(a0)
    lw t2, 48(a0)
    csrrw a0, sscratch, a0
    
    mret

.global supervisor
supervisor:
    ecall

.global userland
userland:
    ecall
