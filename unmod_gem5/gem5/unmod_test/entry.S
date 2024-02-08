.section .text
.global _entry
_entry:
    la t1, trap_handler
    csrw mtvec, t1
    li t0, 1 << 8
    csrrs t1, mstatus, t0
    
    li t1, 2
    stat t1, t1, t1
    ecall


.global trap_handler
trap_handler:
    li t1, 1
    stat t1, t1, t1

