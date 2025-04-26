#include "timer.h"
#include "../interrupts/isr.h"
#include "../screen.h"
#include "../shell.h"  // For print_int

// Make timer_ticks globally visible
volatile uint32_t timer_ticks = 0;

// Other static variables
static volatile boolean timer_active = FALSE;
static volatile uint32_t timer_stack_guard = 0xDEADBEEF;
static boolean hw_timer_available = FALSE;

static int debug_level = TIMER_DEBUG_NONE;
static boolean safe_mode = TRUE;

void timer_set_debug_level(int level) {
    if (level >= TIMER_DEBUG_NONE && level <= TIMER_DEBUG_ALL) {
        debug_level = level;
        if (debug_level >= TIMER_DEBUG_BASIC) {
            print_string("Timer debug level set to ");
            print_int(level);
            print_string("\n");
        }
    }
}

void timer_set_safe_mode(boolean safe) {
    safe_mode = safe;
    if (debug_level >= TIMER_DEBUG_BASIC) {
        print_string("Timer safe mode ");
        print_string(safe ? "enabled" : "disabled");
        print_string("\n");
    }
}

int timer_enable_hardware(void) {
    if (safe_mode) {
        return 0;
    }
    
    // Attempt hardware timer initialization
    timer_hw_init();  // Just call - it's a void function
    if (hw_timer_available) {
        // Enable timer IRQ
        outb(PIC1_DATA, inb(PIC1_DATA) & ~0x01);
        io_wait();
        return 1;
    }
    
    return 0;
}

static void timer_handler(registers_t* regs __attribute__((unused))) {
    if (timer_stack_guard != 0xDEADBEEF) {
        return;
    }
    
    timer_ticks++;
    
    // Always call screen_timer_tick regardless of other conditions
    // This ensures the cursor blinks consistently
    screen_timer_tick();
    
    // Force a cursor update with each timer tick to ensure consistency
    force_cursor_update();
    
    outb(PIC1_COMMAND, PIC_EOI);
}

void timer_init(void) {
    if (timer_active) return;

    // Try hardware initialization first
    timer_hw_init();
    
    // Set up interrupt handler
    register_interrupt_handler(32, timer_handler);
    
    // Configure PIT in safe mode (Mode 2 - Rate Generator)
    uint32_t divisor = PIT_FREQUENCY / TIMER_HZ;
    if (divisor > 0xFFFF) divisor = 0xFFFF;
    
    outb(PIT_COMMAND, PIT_MODE_RATE);
    io_wait();
    outb(PIT_CHANNEL0, divisor & 0xFF);
    io_wait();
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    
    // Test if timer is working
    uint32_t initial_ticks = timer_ticks;
    io_wait();
    io_wait();
    
    if (timer_ticks != initial_ticks) {
        hw_timer_available = TRUE;
    }
    
    timer_active = TRUE;
}

// Sleep implementation using selected timer mode
void timer_sleep(uint32_t ticks) {
    if (!timer_active) {
        timer_init();
    }
    
    if (ticks < MIN_SLEEP_TICKS) {
        ticks = MIN_SLEEP_TICKS;
    }

    uint32_t start = timer_ticks;
    uint32_t target = start + ticks;
    uint32_t current_tick = start;
    uint32_t iterations_per_tick = 10000; // Calibrated for ~10ms per tick at 100Hz

    while (current_tick < target) {
        // Calibrated delay loop
        for (uint32_t i = 0; i < iterations_per_tick; i++) {
            io_wait();
        }
        
        // Increment counter to match expected tick rate
        if (!hw_timer_available) {
            timer_ticks++;
        }
        current_tick = timer_ticks;
        
        if (current_tick - start > ticks * 2) {
            break;
        }
    }
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

const char* timer_get_status(void) {
    if (!timer_active) {
        return "Timer Not Initialized";
    }
    return hw_timer_available ? "Hardware Timer" : "Software Timer";
}

int timer_enable(void) {
    if (!timer_active) {
        timer_init();
    }
    return hw_timer_available;
}

void timer_disable(void) {
    if (!timer_active) return;
    
    // Mask timer interrupt
    outb(PIC1_DATA, inb(PIC1_DATA) | 0x01);
    io_wait();
    
    timer_active = FALSE;
    hw_timer_available = FALSE;
}
