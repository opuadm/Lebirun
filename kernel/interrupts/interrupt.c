#include "interrupt.h"
#include "idt.h"       // Added for idt_init()
#include "../screen.h"
#include "exceptions.h"

// Configure system to be compatible with virtualized environments
#define VIRTUALIZATION_COMPATIBLE_MODE 1

// Initialize the interrupt system
void interrupt_init(void) {
    print_string("Initializing interrupt system...\n");
    
    // Initialize the IDT
    idt_init();
    
    // Initialize exception handlers
    init_exception_handlers();
    
    // CRITICAL: Use a safer PIC mask setup for boot time
    // Mask ALL IRQs initially - this is critical to prevent triple faults
    outb(PIC1_DATA, 0xFF);  // Mask all IRQs on master PIC
    outb(PIC2_DATA, 0xFF);  // Mask all IRQs on slave PIC
    
    // Enable interrupts globally, but with all IRQs masked
    // This allows exception handling without IRQs
    enable_interrupts();
    
    print_string("Interrupt system initialized in safe mode\n");
}

// Enable specific IRQ - virtualization-compatible version
void enable_irq(uint8_t irq_num) {
    if (VIRTUALIZATION_COMPATIBLE_MODE) {
        // In virtualization-compatible mode, only enable non-timer IRQs
        if (irq_num == 0) {
            print_string("Timer IRQ (0) not enabled - using simulated timers\n");
            return;
        }
    }
    
    uint16_t port;
    uint8_t value;
    
    if (irq_num < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_num -= 8;
    }
    
    value = inb(port) & ~(1 << irq_num);
    outb(port, value);
    
    print_string("IRQ ");
    char num_str[4] = {0};
    int i = 0, temp = irq_num;
    
    do {
        num_str[i++] = '0' + (temp % 10);
        temp /= 10;
    } while (temp > 0);
    
    while (i > 0) {
        print_char(num_str[--i]);
    }
    
    print_string(" enabled.\n");
}

// Disable specific IRQ
void disable_irq(uint8_t irq_num) {
    uint16_t port;
    uint8_t value;
    
    if (irq_num < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_num -= 8;
    }
    
    value = inb(port) | (1 << irq_num);
    outb(port, value);
    
    print_string("IRQ ");
    char num_str[4] = {0};
    int i = 0, temp = irq_num;
    
    do {
        num_str[i++] = '0' + (temp % 10);
        temp /= 10;
    } while (temp > 0);
    
    while (i > 0) {
        print_char(num_str[--i]);
    }
    
    print_string(" disabled.\n");
}
