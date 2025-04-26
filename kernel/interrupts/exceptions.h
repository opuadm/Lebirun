#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "../data/types.h"

// Initialize exception handlers
void init_exception_handlers(void);

// Kernel panic function
void kernel_panic(const char* message, registers_t* regs);

// Handler functions for common exceptions
void page_fault_handler(registers_t* regs);
void general_protection_fault_handler(registers_t* regs);
void double_fault_handler(registers_t* regs);
void divide_by_zero_handler(registers_t* regs);

// Helper function for controlled exception testing
int trigger_controlled_exception(void);

// Use external declarations for print functions from shell.c
extern void print_int(int value);
extern void print_hex(uint32_t value);

#endif // EXCEPTIONS_H
