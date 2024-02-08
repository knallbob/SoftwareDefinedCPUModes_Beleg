CC = /home/knallbob/Desktop/grosser_Beleg/compiler/compiler_ex/bin/riscv64-unknown-elf-gcc
AS = /home/knallbob/Desktop/grosser_Beleg/compiler/compiler_ex/bin/riscv64-unknown-elf-as
LD = /home/knallbob/Desktop/grosser_Beleg/compiler/compiler_ex/bin/riscv64-unknown-elf-ld

SRCS = entry.S
OBJS = $(SRCS:.S=.o)
TARGET = reg24_test
TARGET_DIR = ../../../gem5

LDFLAGS = -T test.ld

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	cp $(TARGET) $(TARGET_DIR)/$(TARGET)

%.o: %.S
	$(AS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f $(TARGET_DIR)/$(TARGET)

LOOP_COUNT ?= 5

override_entry:
	sed -i 's/li a0, [0-9][0-9]*\b/li a0, $(LOOP_COUNT)/g' entry.S

$(OBJS): override_entry

build: override_entry all

.PHONY: build