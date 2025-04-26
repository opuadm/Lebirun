#ifndef TIMER_H 
#define TIMER_H

#include "../data/types.h"
#include "../screen.h" // Include for the boolean type

// Remove boolean definition - use the one from screen.h
// typedef enum { FALSE = 0, TRUE = 1 } boolean;

// PIT configuration
#define PIT_FREQUENCY    1193182  // Standard PIT frequency
#define TIMER_HZ        100      // Target frequency
#define MIN_SLEEP_TICKS 1        // Minimum sleep duration

// Hardware ports
#define PIT_COMMAND     0x43
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42

// Operating modes
#define PIT_MODE_RATE   0x34    // Rate generator mode
#define PIT_MODE_SQUARE 0x36    // Square wave mode
#define PIT_MODE_SW     0x30    // Software triggered mode

// Magic numbers for stack protection
#define STACK_GUARD_VALUE 0xDEADBEEF

// Debug levels
#define TIMER_DEBUG_NONE    0
#define TIMER_DEBUG_BASIC   1
#define TIMER_DEBUG_VERBOSE 2
#define TIMER_DEBUG_ALL     3

// Safe mode control
void timer_set_safe_mode(boolean safe);
int timer_enable_hardware(void);

// Expose timer_ticks for assembly
extern volatile uint32_t timer_ticks;

// Function declarations
void timer_init(void);
void timer_sleep(uint32_t ticks);
uint32_t timer_get_ticks(void);
const char* timer_get_status(void);
void timer_set_debug_level(int level);
int timer_enable(void);
void timer_disable(void);
void print_int(int value);

// Assembly helpers
extern void timer_hw_init(void);
extern uint64_t read_tsc(void);
extern void timer_wait_next_tick(void);

// IO functions - now centralized here for all modules to use
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_wait(void) {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

#endif