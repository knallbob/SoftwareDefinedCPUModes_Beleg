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
    sw ra, 0(a0)
    sw sp, 8(a0)
    sw gp, 16(a0)
    sw tp, 24(a0)
    sw t0, 32(a0)
    sw t1, 40(a0)
    sw t2, 48(a0)
    sw s0, 56(a0)
    sw s1, 64(a0)
    sw a1, 80(a0)
    sw a2, 88(a0)
    sw a3, 96(a0)
    sw a4, 104(a0)
    sw a5, 112(a0)
    sw a6, 120(a0)
    sw a7, 128(a0)
    sw s2, 136(a0)
    sw s3, 144(a0)
    sw s4, 152(a0)
    sw s5, 160(a0)
    sw s6, 168(a0)
    sw s7, 176(a0)
    sw s8, 184(a0)
    sw s9, 192(a0)
    sw s10, 200(a0)
    sw s11, 208(a0)
    sw t3, 216(a0)
    sw t4, 224(a0)
    sw t5, 232(a0)
    sw t6, 240(a0)

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
    
    lw ra, 0(a0)
    lw sp, 8(a0)
    lw gp, 16(a0)
    lw tp, 24(a0)
    lw t0, 32(a0)
    lw t1, 40(a0)
    lw t2, 48(a0)
    lw s0, 56(a0)
    lw s1, 64(a0)
    lw a1, 80(a0)
    lw a2, 88(a0)
    lw a3, 96(a0)
    lw a4, 104(a0)
    lw a5, 112(a0)
    lw a6, 120(a0)
    lw a7, 128(a0)
    lw s2, 136(a0)
    lw s3, 144(a0)
    lw s4, 152(a0)
    lw s5, 160(a0)
    lw s6, 168(a0)
    lw s7, 176(a0)
    lw s8, 184(a0)
    lw s9, 192(a0)
    lw s10, 200(a0)
    lw s11, 208(a0)
    lw t3, 216(a0)
    lw t4, 224(a0)
    lw t5, 232(a0)
    lw t6, 240(a0)
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
    
    lw ra, 0(a0)
    lw sp, 8(a0)
    lw gp, 16(a0)
    lw tp, 24(a0)
    lw t0, 32(a0)
    lw t1, 40(a0)
    lw t2, 48(a0)
    lw s0, 56(a0)
    lw s1, 64(a0)
    lw a1, 80(a0)
    lw a2, 88(a0)
    lw a3, 96(a0)
    lw a4, 104(a0)
    lw a5, 112(a0)
    lw a6, 120(a0)
    lw a7, 128(a0)
    lw s2, 136(a0)
    lw s3, 144(a0)
    lw s4, 152(a0)
    lw s5, 160(a0)
    lw s6, 168(a0)
    lw s7, 176(a0)
    lw s8, 184(a0)
    lw s9, 192(a0)
    lw s10, 200(a0)
    lw s11, 208(a0)
    lw t3, 216(a0)
    lw t4, 224(a0)
    lw t5, 232(a0)
    lw t6, 240(a0)
    csrrw a0, sscratch, a0
    mret

.global supervisor
supervisor:
    ecall

.global userland
userland:
    ecall
