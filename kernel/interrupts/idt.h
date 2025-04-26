#ifndef IDT_H
#define IDT_H

#include "../data/types.h"

// Initialize the Interrupt Descriptor Table
void idt_init(void);

// Set an IDT gate (an entry in the IDT)
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

// Load the IDT register with our IDT pointer
void idt_load(idtr_t* idtr);

// Verify the IDT setup is correct
void verify_idt_setup(void);

// Priority settings for interrupts (in order of highest to lowest)
#define IDT_PRIORITY_TIMER      0  // 0 = Highest priority
#define IDT_PRIORITY_KEYBOARD   1
#define IDT_PRIORITY_DISK       2
#define IDT_PRIORITY_NETWORK    3
#define IDT_PRIORITY_DEFAULT    7  // 7 = Lowest priority
#define IDT_NUM_PRIORITIES      8  // Total number of priority levels

// Interrupt priority functions
void idt_set_interrupt_priority(uint8_t interrupt_num, uint8_t priority);
uint8_t idt_get_interrupt_priority(uint8_t interrupt_num);

// Functions to safely access IDT information
void get_idt_info(uint32_t* base, uint16_t* limit);
uint32_t get_idt_entry_address(int index);
int get_idt_entry_present(int index);

#endif // IDT_H
