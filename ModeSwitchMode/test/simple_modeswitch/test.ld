SECTIONS {
    . = 0x4000000;
    .data : {
        *(.data)
        *(.data.*)
        . = ALIGN(0x1000);
    }

    . = 0x80000000;
    .text : {
        *(.text)
        *(.text.*)
        . = ALIGN(0x1000);
    }

    . = 0x82000000;
    .trap_handler : {
        *(.trap_handler)
        . = ALIGN(0x1000);
    }

    . = 0x85000000;
    .supervisor : {
        *(.supervisor)
        . = ALIGN(0x1000);
    }

    . = 0x86000000;
    .userland : {
        *(.userland)
        . = ALIGN(0x1000);
    }
}