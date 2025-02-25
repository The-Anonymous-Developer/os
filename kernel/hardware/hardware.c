#include <stdint.h>
#include "hardware.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "pic.h"
#include "../kernel.h"

// Define GDT entries and pointer
static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t gp;

// Cross-platform assembly macros
#ifdef _MSC_VER
    #define ASM __asm
#else
    #define ASM __asm__
#endif

void init_hardware(void) {
    // Disable interrupts
    #ifdef _MSC_VER
        ASM cli
    #else
        ASM ("cli");
    #endif
    
    init_gdt();
    init_idt();
    init_pic();
    init_pit();
}

void init_gdt(void) {
    // Setup GDT pointer
    gp.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
    gp.base = (uint64_t)&gdt;

    // Null descriptor
    gdt[0] = (gdt_entry_t){0};

    // Code segment
    gdt[1].limit_low = 0xFFFF;
    gdt[1].base_low = 0;
    gdt[1].base_middle = 0;
    gdt[1].access = 0x9A;
    gdt[1].granularity = 0xCF;
    gdt[1].base_high = 0;

    // Data segment
    gdt[2].limit_low = 0xFFFF;
    gdt[2].base_low = 0;
    gdt[2].base_middle = 0;
    gdt[2].access = 0x92;
    gdt[2].granularity = 0xCF;
    gdt[2].base_high = 0;

    // Load GDT using compiler-specific syntax
    #ifdef _MSC_VER
        ASM {
            lgdt [gp]
            mov ax, 0x08
            mov ds, ax
            mov es, ax
            mov fs, ax
            mov gs, ax
            mov ss, ax
            push 0x08
            lea rax, reload_cs
            push rax
            retf
        reload_cs:
        }
    // #else
    //     ASM volatile(
    //         "lgdt %0\n\t"
    //         "mov $0x08, %%ax\n\t"
    //         "mov %%ax, %%ds\n\t"
    //         "mov %%ax, %%es\n\t"
    //         "mov %%ax, %%fs\n\t"
    //         "mov %%ax, %%gs\n\t"
    //         "mov %%ax, %%ss\n\t"
    //         "pushq $0x08\n\t"         // Code segment
    //         "pushq $1f\n\t"           // Return address
    //         "lretq\n\t"
    //         "1:\n\t"
    //         : : "m"(gp) : "rax", "memory"
    //     );
    #endif
}