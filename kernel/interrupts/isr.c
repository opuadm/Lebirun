#include "isr.h"
#include "../screen.h"

// External declarations for IDT functions to avoid circular includes
extern int get_idt_entry_present(uint8_t index);
extern void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

// Create a function that forwards to idt_set_gate for compatibility
// Now defined in the header file for all files to see
void set_idt_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt_set_gate(num, base, selector, flags);
}

// Define our own NULL instead of including stddef.h
#ifndef NULL
#define NULL ((void*)0)
#endif

// Array of interrupt handlers
static isr_handler_t interrupt_handlers[256] = {0};

// Function to install ISRs into the IDT
void isr_install() {
    // Set IDT gates for ISRs 0-31
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    // Remap the PIC
    outb(PIC1_COMMAND, 0x11); // Start initialization sequence
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);    // Master PIC vector offset (start at 32)
    outb(PIC2_DATA, 0x28);    // Slave PIC vector offset (start at 40)
    outb(PIC1_DATA, 0x04);    // Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC2_DATA, 0x02);    // Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC1_DATA, 0x01);    // 8086/88 (MCS-80/85) mode
    outb(PIC2_DATA, 0x01);    // 8086/88 (MCS-80/85) mode
    outb(PIC1_DATA, 0x0);     // Restore saved masks (enable all)
    outb(PIC2_DATA, 0x0);

    // Set IDT gates for IRQs 0-15 (ISR 32-47)
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    print_string("ISRs installed.\n");
}


// Function to check if an interrupt handler is registered
int is_handler_registered(uint8_t interrupt_num) {
    // First check if the IDT entry is present
    if (!get_idt_entry_present(interrupt_num)) {
        return 0;
    }
    
    // Then check if there's a handler in the handlers array
    if (interrupt_handlers[interrupt_num] == NULL) {
        // IDT entry exists but no handler function
        print_string("Warning: IDT entry present but no handler function for INT ");
        char num_str[4] = {0};
        int i = 0;
        int temp = interrupt_num;
        do {
            num_str[i++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);
        
        while (i > 0) {
            char c = num_str[--i];
            print_char(c);
        }
        print_string("\n");
        return 0;
    }
    
    return 1;
}

// Function to register an interrupt handler
void register_interrupt_handler(uint8_t n, isr_handler_t handler) {
    if (handler != NULL) {
        interrupt_handlers[n] = handler;
        
        // Debug verification for division by zero handler
        if (n == 0) {
            if (interrupt_handlers[0] == handler) {
                print_string("Handler correctly stored in array.\n");
            } else {
                print_string("ERROR: Handler not stored correctly in array!\n");
            }
        }
    } else {
        // Print debug info for invalid registrations
        print_string("ERROR: Attempted to register NULL handler.\n");
    }
}

// Common Interrupt Service Routine handler
void isr_handler(registers_t* regs) {
    // If we have a custom handler for this interrupt, call it
    if (interrupt_handlers[regs->int_no] != NULL) {
        isr_handler_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    } else {
        // No handler registered for this interrupt
        print_string("Unhandled interrupt: ");
        
        // Print the interrupt number
        char int_str[4] = {0};
        int i = 0, temp = regs->int_no;
        
        do {
            int_str[i++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);
        
        if (i == 0) { // If int_no was 0
            print_char('0');
        } else {
            while (i > 0) {
                print_char(int_str[--i]);
            }
        }
        
        print_string("\n");
    }
}

// IRQ handler - remaps IRQ to ISR and calls the ISR handler
void irq_handler(registers_t* regs) {
    // Send EOI (End of Interrupt) to the PICs
    if (regs->int_no >= 40) {
        // If this came from the slave PIC, send EOI to it too
        outb(PIC2_COMMAND, PIC_EOI);
    }
    
    // Always send EOI to master PIC
    outb(PIC1_COMMAND, PIC_EOI);
    
    // Handle timer IRQ differently for maximum stability
    if (regs->int_no == 32 && interrupt_handlers[32] != NULL) {
        // Timer IRQ handling, but with safety checks
        isr_handler_t handler = interrupt_handlers[32];
        handler(regs);
    }
    // Handle IRQs other than timer with the normal approach
    else if (interrupt_handlers[regs->int_no] != NULL && regs->int_no != 32) {
        isr_handler_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }
}
