
PROJECT_NAME = swo

INCLUDES = -I include

SRC_FILES = src/$(PROJECT_NAME).c \
			src/rcc.c \
			src/gpio.c \
			src/startup.c \

LINKER_SCRIPT = stm32f103c8t6.ld

#########################
## configure toolchain ##
#########################

CROSS_COMPILE  = arm-none-eabi
CC            := $(CROSS_COMPILE)-gcc

CFLAGS += $(INCLUDES)
CFLAGS += -std=gnu23
CFLAGS += -mcpu=cortex-m3 -ffreestanding -nostdlib
CFLAGS += -Wall -Wextra

OFLAGS ?= -Og -g3 -flto
CFLAGS += $(OFLAGS)

LDFLAGS += -T $(LINKER_SCRIPT)

###########
## build ##
###########

BUILDDIR = build
ELF  := $(BUILDDIR)/$(PROJECT_NAME).elf
BIN  := $(BUILDDIR)/$(PROJECT_NAME).bin
MAP  := $(BUILDDIR)/$(PROJECT_NAME).map
LIST := $(BUILDDIR)/$(PROJECT_NAME).list

all: $(ELF)

dbg: OFLAGS = -O0 -g3 -flto
dbg: clean $(ELF)

rel: OFLAGS = -Os -flto
rel: clean $(ELF)

$(ELF): $(SRC_FILES)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map=$(MAP) -Wl,--print-memory-usage $^ -o $@
	$(CROSS_COMPILE)-objcopy -O binary $(ELF) $(BIN)
	$(CROSS_COMPILE)-objdump -h -d $(ELF) > $(LIST)
	$(CROSS_COMPILE)-size $(ELF)

clean:
	rm -rf $(BUILDDIR)
