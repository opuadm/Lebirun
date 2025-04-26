#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "../data/types.h"
#include "../drivers/timer.h"  // For IO functions

// Initialize the interrupt system (IDT, ISRs, IRQs)
void interrupt_init(void);

// Enable interrupts
static inline void enable_interrupts(void) {
    __asm__ volatile("sti");
}

// Disable interrupts
static inline void disable_interrupts(void) {
    __asm__ volatile("cli");
}

// Are interrupts enabled?
static inline int are_interrupts_enabled(void) {
    unsigned long flags;
    __asm__ volatile("pushf\n\t"
                     "pop %0"
                     : "=g"(flags));
    return flags & (1 << 9);
}

// Enable specific IRQ
void enable_irq(uint8_t irq_num);

// Disable specific IRQ
void disable_irq(uint8_t irq_num);

#endif // INTERRUPT_H
