#ifndef IDT_CHECKER_H
#define IDT_CHECKER_H

// Check if the IDT is correctly set up
void check_idt_setup(void);

// Check if interrupt handlers are properly registered
void check_interrupt_handlers(void);

// Check PIC configuration
void check_pic_configuration(void);

// Combined diagnostic function
void diagnose_interrupt_system(void);

#endif // IDT_CHECKER_H
