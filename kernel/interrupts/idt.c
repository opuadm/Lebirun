#include "idt.h"
#include "isr.h"
#include "../screen.h"

// IDT entries array and priority table
static idt_entry_t idt_entries[256];
static idtr_t idt_ptr;
static uint8_t interrupt_priorities[256];

void idt_init(void) {
    // Initialize IDT pointer
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // Initialize all entries and priorities
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
        interrupt_priorities[i] = IDT_PRIORITY_DEFAULT;
    }

    // Set up ISRs
    isr_install();

    // Configure timer IRQ with highest priority
    idt_set_interrupt_priority(32, IDT_PRIORITY_TIMER);

    // Load IDT
    idt_load(&idt_ptr);
    
    print_string("IDT initialized with priority configuration.\n");
    verify_idt_setup();
}

// Set an IDT gate
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].offset_low = base & 0xFFFF;
    idt_entries[num].offset_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = sel;
    idt_entries[num].zero = 0;
    idt_entries[num].type_attr = flags; // Flags should be 0x8E for interrupt gates
}

// Set interrupt priority - fix for comparison warning
void idt_set_interrupt_priority(uint8_t interrupt_num, uint8_t priority) {
    // uint8_t is already limited to 0-255, no need for range check
    interrupt_priorities[interrupt_num] = priority;
}

// Get interrupt priority - fix for comparison warning
uint8_t idt_get_interrupt_priority(uint8_t interrupt_num) {
    // uint8_t is already limited to 0-255, no need for range check
    return interrupt_priorities[interrupt_num];
}

// Get IDT info
void get_idt_info(uint32_t* base, uint16_t* limit) {
    if (base) *base = idt_ptr.base;
    if (limit) *limit = idt_ptr.limit;
}

// Get address of a specific IDT entry
uint32_t get_idt_entry_address(int index) {
    if (index < 0 || index >= 256) {
        return 0;
    }
    uint32_t offset_low = idt_entries[index].offset_low;
    uint32_t offset_high = idt_entries[index].offset_high;
    return (offset_high << 16) | offset_low;
}

// Check if an IDT entry is present/valid
int get_idt_entry_present(int index) {
    if (index < 0 || index >= 256) {
        return 0;
    }
    
    // Check if the entry has a valid handler and proper flags
    return (((idt_entries[index].type_attr & 0x80) != 0) &&
           ((idt_entries[index].offset_low != 0) || (idt_entries[index].offset_high != 0)));
}

// Verify the IDT setup
void verify_idt_setup(void) {
    // Verify critical handlers
    if (!get_idt_entry_present(0) || !get_idt_entry_present(8) || 
        !get_idt_entry_present(13) || !get_idt_entry_present(14)) {
        print_string("WARNING: Critical exception handlers not registered\n");
    }
    
    // Verify timer configuration
    if (!get_idt_entry_present(32)) {
        print_string("WARNING: Timer interrupt not registered\n");
    } else {
        uint8_t timer_priority = idt_get_interrupt_priority(32);
        if (timer_priority != IDT_PRIORITY_TIMER) {
            print_string("WARNING: Timer does not have correct priority\n");
        } else {
            print_string("Timer interrupt properly configured\n");
        }
    }
    
    // Verify IDTR loaded correctly
    idtr_t current_idtr;
    __asm__ volatile("sidt %0" : "=m"(current_idtr));
    
    if (current_idtr.base != idt_ptr.base || current_idtr.limit != idt_ptr.limit) {
        print_string("WARNING: IDT has not been properly loaded into CPU\n");
    } else {
        print_string("IDT verification passed\n");
    }
}
