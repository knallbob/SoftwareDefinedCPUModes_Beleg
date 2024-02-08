SECTIONS {

    . = 0x80000000;
    .text : {
        *(.text)
        *(.text.*)
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

        . = 0x9FFFFFF0;
    .handlers : {
        *(.handlers)
        . = ALIGN(0x1000);
    }

    . = 0x9FFF0000;
    .data : {
        *(.data)
        *(.data.*)
        . = ALIGN(0x1000);
    }
}