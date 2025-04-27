#include "idt.h"
#include "isr.h"
#include "../drivers/screen.h"
#include "exceptions.h"

// These structs should be in idt.c, we need to access them differently
extern void get_idt_info(uint32_t* base, uint16_t* limit);
extern uint32_t get_idt_entry_address(int index);
extern int get_idt_entry_present(int index);

// Local implementations with different names to avoid conflicts
static void idt_checker_print_hex(uint32_t value) {
    const char* hex_chars = "0123456789ABCDEF";
    char buffer[9];
    int i;
    
    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    buffer[8] = '\0';
    
    print_string("0x");
    print_string(buffer);
}

static void idt_checker_print_int(int value) {
    char buffer[12];  // Enough for a 32-bit integer
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        print_string("0");
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (is_negative) {
        print_string("-");
    }

    while (i > 0) {
        char c[2] = {buffer[--i], '\0'};
        print_string(c);
    }
}

// Function to verify IDT setup - good for debugging
void check_idt_setup(void) {
    uint32_t idt_base;
    uint16_t idt_limit;
    
    // Get the IDT info from the proper functions
    get_idt_info(&idt_base, &idt_limit);
    
    print_string("\n=== IDT VALIDATION ===\n");
    
    // Check IDTR
    print_string("IDTR Base: ");
    idt_checker_print_hex(idt_base);
    print_string("\nIDTR Limit: ");
    idt_checker_print_hex(idt_limit);
    
    // Expected limit for 256 entries
    if (idt_limit == (sizeof(idt_entry_t) * 256) - 1) {
        print_string(" (Correct)\n");
    } else {
        print_string(" (INCORRECT - Should be 0x7FF)\n");
    }
    
    // Check a few key IDT entries
    print_string("\nChecking critical interrupt vectors:\n");
    
    // Division by zero (INT 0)
    print_string("INT 0 (Div by Zero): ");
    if (get_idt_entry_present(0)) {
        print_string("Handler present at ");
        idt_checker_print_hex(get_idt_entry_address(0));
        print_string("\n");
    } else {
        print_string("MISSING!\n");
    }
    
    // Double fault (INT 8)
    print_string("INT 8 (Double Fault): ");
    if (get_idt_entry_present(8)) {
        print_string("Handler present at ");
        idt_checker_print_hex(get_idt_entry_address(8));
        print_string("\n");
    } else {
        print_string("MISSING!\n");
    }
    
    // General protection fault (INT 13)
    print_string("INT 13 (GP Fault): ");
    if (get_idt_entry_present(13)) {
        print_string("Handler present at ");
        idt_checker_print_hex(get_idt_entry_address(13));
        print_string("\n");
    } else {
        print_string("MISSING!\n");
    }
    
    // Page fault (INT 14)
    print_string("INT 14 (Page Fault): ");
    if (get_idt_entry_present(14)) {
        print_string("Handler present at ");
        idt_checker_print_hex(get_idt_entry_address(14));
        print_string("\n");
    } else {
        print_string("MISSING!\n");
    }
    
    // Timer IRQ (INT 32)
    print_string("INT 32 (Timer): ");
    if (get_idt_entry_present(32)) {
        print_string("Handler present at ");
        idt_checker_print_hex(get_idt_entry_address(32));
        print_string("\n");
    } else {
        print_string("MISSING!\n");
    }
    
    // Keyboard IRQ (INT 33)
    print_string("INT 33 (Keyboard): ");
    if (get_idt_entry_present(33)) {
        print_string("Handler present at ");
        idt_checker_print_hex(get_idt_entry_address(33));
        print_string("\n");
    } else {
        print_string("MISSING!\n");
    }
    
    print_string("\nIDT validation complete.\n");
}

// Check for properly registered interrupt handlers
void check_interrupt_handlers(void) {
    print_string("\n=== HANDLER REGISTRATION CHECK ===\n");
    
    // Use a function from isr.c to check if handlers are registered
    // We'll add these functions to isr.h and implement them in isr.c
    
    // Check critical exception handlers
    print_string("Div by Zero (INT 0): ");
    if (is_handler_registered(0)) {
        print_string("Handler registered\n");
    } else {
        print_string("NOT REGISTERED!\n");
    }
    
    print_string("Double Fault (INT 8): ");
    if (is_handler_registered(8)) {
        print_string("Handler registered\n");
    } else {
        print_string("NOT REGISTERED!\n");
    }
    
    print_string("GP Fault (INT 13): ");
    if (is_handler_registered(13)) {
        print_string("Handler registered\n");
    } else {
        print_string("NOT REGISTERED!\n");
    }
    
    print_string("Page Fault (INT 14): ");
    if (is_handler_registered(14)) {
        print_string("Handler registered\n");
    } else {
        print_string("NOT REGISTERED!\n");
    }
    
    print_string("\nInterrupt handler check complete.\n");
}

// Check PIC configuration
void check_pic_configuration(void) {
    // Read Interrupt Mask Registers (IMR)
    uint8_t master_mask = inb(PIC1_DATA); // Now uses the defined inb
    uint8_t slave_mask = inb(PIC2_DATA);  // Now uses the defined inb

    print_string("\n=== PIC CONFIGURATION CHECK ===\n");
    print_string("Master PIC Mask (IMR): ");
    idt_checker_print_hex(master_mask);
    print_string("\nSlave PIC Mask (IMR): ");
    idt_checker_print_hex(slave_mask);
    
    print_string("\n\nEnabled IRQs on master PIC: ");
    for (int i = 0; i < 8; i++) {
        if (!(master_mask & (1 << i))) {
            idt_checker_print_int(i);
            print_string(" ");
        }
    }
    
    print_string("\nEnabled IRQs on slave PIC: ");
    for (int i = 0; i < 8; i++) {
        if (!(slave_mask & (1 << i))) {
            idt_checker_print_int(i + 8);
            print_string(" ");
        }
    }
    
    print_string("\n\nPIC configuration check complete.\n");
}

// Combined diagnostic function
void diagnose_interrupt_system(void) {
    print_string("\n==================================================\n");
    print_string("       INTERRUPT SYSTEM DIAGNOSTIC REPORT\n");
    print_string("==================================================\n");
    
    check_idt_setup();
    check_interrupt_handlers();
    check_pic_configuration();
    
    print_string("\n==================================================\n");
    print_string("             DIAGNOSTIC COMPLETE\n");
    print_string("==================================================\n");
}
