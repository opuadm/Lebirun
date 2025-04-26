#ifndef ISR_H
#define ISR_H

#include "../data/types.h"
#include "../drivers/timer.h" // For IO functions

// Function to register an interrupt handler
void register_interrupt_handler(uint8_t n, isr_handler_t handler);

// Function to check if a handler is registered
int is_handler_registered(uint8_t interrupt_num);

// Install ISRs into the IDT
void isr_install(void); // Declaration already exists

// ISR handlers
void isr_handler(registers_t* regs);
void irq_handler(registers_t* regs);

// External declarations for all assembly ISR stubs (0-31)
extern void isr0(void); extern void isr1(void); extern void isr2(void); extern void isr3(void);
extern void isr4(void); extern void isr5(void); extern void isr6(void); extern void isr7(void);
extern void isr8(void); extern void isr9(void); extern void isr10(void); extern void isr11(void);
extern void isr12(void); extern void isr13(void); extern void isr14(void); extern void isr15(void);
extern void isr16(void); extern void isr17(void); extern void isr18(void); extern void isr19(void);
extern void isr20(void); extern void isr21(void); extern void isr22(void); extern void isr23(void);
extern void isr24(void); extern void isr25(void); extern void isr26(void); extern void isr27(void);
extern void isr28(void); extern void isr29(void); extern void isr30(void); extern void isr31(void);

// External declarations for all assembly IRQ stubs (0-15 mapped to ISR 32-47)
extern void irq0(void); extern void irq1(void); extern void irq2(void); extern void irq3(void);
extern void irq4(void); extern void irq5(void); extern void irq6(void); extern void irq7(void);
extern void irq8(void); extern void irq9(void); extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void); extern void irq14(void); extern void irq15(void);


// Declare compatibility names for the stubs needed by exceptions.c
#define divide_by_zero_stub ((void*)isr0)
#define double_fault_stub ((void*)isr8)
#define general_protection_fault_stub ((void*)isr13)
#define page_fault_stub ((void*)isr14)

// Compatibility function to set IDT gates
void set_idt_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

#endif // ISR_H
