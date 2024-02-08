CC = /home/knallbob/Desktop/grosser_Beleg/compiler/compiler_ex/bin/riscv64-unknown-elf-gcc
AS = /home/knallbob/Desktop/grosser_Beleg/compiler/compiler_ex/bin/riscv64-unknown-elf-as
LD = /home/knallbob/Desktop/grosser_Beleg/compiler/compiler_ex/bin/riscv64-unknown-elf-ld

SRCS = entry.S
OBJS = $(SRCS:.S=.o)
TARGET = syscall_test

LDFLAGS = -T test.ld

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	cp $(TARGET) ../

%.o: %.S
	$(AS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)