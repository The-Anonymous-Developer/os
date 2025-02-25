# Compiler and tools
NASM=nasm
LD=lld-link
CC=clang
CXX=clang++

# Directories
BOOTLOADER_DIR=bootloader
KERNEL_DIR=kernel
DRIVERS_DIR=drivers
AI_CORE_DIR=ai-core
UI_DIR=ui
BUILD_DIR=build
BIN_DIR=$(BUILD_DIR)/bin
KERNEL_BUILD_DIR=$(BUILD_DIR)/kernel

# Compiler flags
KERNEL_CFLAGS = -Wall -Wextra -ffreestanding -fno-stack-protector \
    -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
    -mcmodel=kernel -target x86_64-pc-none-elf \
    -fms-extensions -nostdlib -masm=intel \
    -fno-pie -static

# Kernel sources and objects
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/core/*.c) \
                $(wildcard $(KERNEL_DIR)/memory/*.c) \
                $(wildcard $(KERNEL_DIR)/process/*.c) \
                $(wildcard $(KERNEL_DIR)/filesystem/*.c) \
                $(wildcard $(KERNEL_DIR)/hardware/*.c)

KERNEL_OBJECTS = $(KERNEL_SOURCES:$(KERNEL_DIR)/%.c=$(KERNEL_BUILD_DIR)/%.o)

# Add hardware initialization to dependencies
KERNEL_DEPS = hardware.o

# Bootloader settings
BOOTLOADER_SOURCES = $(wildcard $(BOOTLOADER_DIR)/*.asm)
BOOTLOADER_OBJECTS = $(BOOTLOADER_SOURCES:%.asm=%.o)
BOOTLOADER_TARGET = $(BIN_DIR)/bootloader.efi

# AI Core settings
AI_LOCAL=$(wildcard $(AI_CORE_DIR)/local/*.cpp)
AI_CLOUD=$(wildcard $(AI_CORE_DIR)/cloud/*.cpp)
AI_MODELS=$(wildcard $(AI_CORE_DIR)/models/*.cpp)
AI_SOURCES=$(AI_LOCAL) $(AI_CLOUD) $(AI_MODELS)
AI_SOURCES = $(wildcard $(AI_CORE_DIR)/*/*.cpp)
AI_OBJECTS = $(AI_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
AI_TARGET=$(BIN_DIR)/ai-core.bin

# UI settings
UI_SHELL=$(wildcard $(UI_DIR)/shell/*.cpp)
UI_DESKTOP=$(wildcard $(UI_DIR)/desktop/*.cpp)
UI_THEMES=$(wildcard $(UI_DIR)/themes/*.cpp)
UI_SOURCES=$(UI_SHELL) $(UI_DESKTOP) $(UI_THEMES)
UI_OBJECTS=$(UI_SOURCES:.cpp=.o)
UI_TARGET=$(BIN_DIR)/ui.bin

# Flags
NASM_FLAGS=-f win64
LD_FLAGS=/subsystem:efi_application /entry:EfiMain
CC_FLAGS=-Wall -Wextra -O2
CXX_FLAGS=$(CC_FLAGS) -std=c++17

# New Kernel settings
CFLAGS = -Wall -Wextra -ffreestanding -fno-stack-protector \
         -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
         -mcmodel=kernel -target x86_64-pc-none-elf

ifeq ($(CC),clang)
    CFLAGS += -fms-extensions
endif

SRCDIR = kernel/core
BUILDDIR = build
BINDIR = $(BUILDDIR)/bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)

# Linker flags
LDFLAGS = -nostdlib -no-pie -static

# Update linker flags for kernel
KERNEL_LDFLAGS = -nostdlib -static -Wl,-no-pie

.PHONY: all clean bootloader kernel drivers ai-core ui

all: create-dirs bootloader kernel drivers ai-core ui

create-dirs:
	-mkdir $(BUILD_DIR) 2>NUL
	-mkdir $(BIN_DIR) 2>NUL
	-mkdir $(KERNEL_BUILD_DIR) 2>NUL
	-mkdir $(KERNEL_BUILD_DIR)/core 2>NUL
	-mkdir $(KERNEL_BUILD_DIR)/memory 2>NUL
	-mkdir $(KERNEL_BUILD_DIR)/process 2>NUL
	-mkdir $(KERNEL_BUILD_DIR)/filesystem 2>NUL
	-mkdir $(KERNEL_BUILD_DIR)/hardware 2>NUL

# Bootloader rules
bootloader: $(BOOTLOADER_TARGET)

$(BOOTLOADER_TARGET): $(BOOTLOADER_OBJECTS)
	$(LD) /subsystem:efi_application /entry:EfiMain $^ /out:$@

%.o: %.asm
	$(NASM) -f win64 $< -o $@

# Kernel rules
kernel: $(BIN_DIR)/kernel.bin

$(BIN_DIR)/kernel.bin: $(KERNEL_OBJECTS)
	$(CC) $(KERNEL_CFLAGS) $(KERNEL_LDFLAGS) -T $(KERNEL_DIR)/linker.ld $^ -o $@

$(KERNEL_BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	@-mkdir "$(dir $@)" 2>NUL
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

# Add assembly files to kernel objects
KERNEL_ASM_SOURCES = $(wildcard $(KERNEL_DIR)/hardware/*.asm)
KERNEL_ASM_OBJECTS = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SOURCES))

KERNEL_OBJECTS += $(KERNEL_ASM_OBJECTS)

# Add assembly compilation rule
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(NASM) $(NASM_FLAGS) $< -o $@

# AI Core rules
ai-core: $(AI_TARGET)

$(AI_TARGET): $(AI_OBJECTS)
	@mkdir -p "$(dir $@)"
	$(CXX) $(CXX_FLAGS) $^ -o $@ 

$(BUILD_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXX_FLAGS) -c $< -o $@

# UI rules
ui: $(UI_TARGET)

$(UI_TARGET): $(UI_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -o $@

# Cleanup
clean:
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf "$(BUILD_DIR)"; fi
	@if [ -f "*.o" ]; then rm -f *.o; fi
	@if [ -f "*.bin" ]; then rm -f *.bin; fi
	@if [ -f "*.efi" ]; then rm -f *.efi; fi

# Build specific components
build-bootloader: bootloader
build-kernel: kernel
build-ai: ai-core
build-ui: ui

# NASM settings for Windows
NASM = nasm
NASM_FLAGS = -f win64

# Fix assembly paths and objects
KERNEL_ASM_SOURCES = $(wildcard $(KERNEL_DIR)/hardware/*.asm)
KERNEL_ASM_OBJECTS = $(KERNEL_ASM_SOURCES:$(KERNEL_DIR)/%.asm=$(KERNEL_BUILD_DIR)/%.o)

# Clear previous KERNEL_OBJECTS definition
KERNEL_OBJECTS = 

# Add C source files
KERNEL_C_SOURCES = $(wildcard $(KERNEL_DIR)/core/*.c) \
                  $(wildcard $(KERNEL_DIR)/memory/*.c) \
                  $(wildcard $(KERNEL_DIR)/process/*.c) \
                  $(wildcard $(KERNEL_DIR)/filesystem/*.c) \
                  $(wildcard $(KERNEL_DIR)/hardware/*.c)

KERNEL_C_SOURCES += kernel/lib/string.c \
    kernel/memory/memory.c

KERNEL_C_OBJECTS = $(KERNEL_C_SOURCES:$(KERNEL_DIR)/%.c=$(KERNEL_BUILD_DIR)/%.o)

# Add assembly sources
KERNEL_ASM_SOURCES = $(wildcard $(KERNEL_DIR)/hardware/*.asm)
KERNEL_ASM_SOURCES += $(KERNEL_DIR)/process/switch.asm
KERNEL_ASM_SOURCES += kernel/hardware/ports_asm.asm
KERNEL_ASM_SOURCES += kernel/hardware/cpu_asm.asm
KERNEL_ASM_OBJECTS = $(KERNEL_ASM_SOURCES:$(KERNEL_DIR)/%.asm=$(KERNEL_BUILD_DIR)/%.o)

# Combine all objects
KERNEL_OBJECTS = $(KERNEL_C_OBJECTS) $(KERNEL_ASM_OBJECTS)

# Single kernel build rule
$(BIN_DIR)/kernel.bin: $(KERNEL_OBJECTS)
	$(CC) $(KERNEL_CFLAGS) $(KERNEL_LDFLAGS) -T $(KERNEL_DIR)/linker.ld $^ -o $@

# C source compilation rule
$(KERNEL_BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	@if [ ! -d "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

# Assembly compilation rule
$(KERNEL_BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm
	@if [ ! -d "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
	$(NASM) $(NASM_FLAGS) -o $@ $<

%.o: %.asm
	nasm -f elf64 -o $@ $<