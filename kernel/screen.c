#include "screen.h"
#include "data/font_data.h"
#include "data/types.h"
#include "drivers/timer.h"  // Provides io_wait and other IO functions

#define NULL ((void*)0)

// PIT (Programmable Interval Timer) configuration
#define PIT_CHANNEL0    0x40
#define PIT_MODE        0x43
#define PIT_FREQUENCY   1193182
#define SYSTEM_TIMER_HZ 100

// Screen configuration
#define PREFERRED_WIDTH  1024
#define PREFERRED_HEIGHT 768
#define PREFERRED_BPP    32

// Font dimensions with precise height
#define FONT_BASE_WIDTH  8
#define FONT_BASE_HEIGHT 16
#define FONT_SCALE      1.075
#define FONT_WIDTH      9
#define FONT_HEIGHT     19.25
#define FONT_SPACING    1

// Screen dimensions in characters
#define MAX_COLS ((PREFERRED_WIDTH) / (FONT_WIDTH + FONT_SPACING))
#define MAX_ROWS ((PREFERRED_HEIGHT) / ((u32)FONT_HEIGHT))

// Cursor configuration
#define CURSOR_BLINK_MS 500    // Cursor blink interval in milliseconds
#define CURSOR_THICKNESS 2
#define CURSOR_ALWAYS_BLINK TRUE  // Force cursor to always blink

// Hardware ports
#define VGA_DAC_WRITE_INDEX  0x3C8
#define VGA_DAC_DATA        0x3C9
#define VGA_MISC_OUTPUT     0x3C2
#define SVGA_INDEX_PORT     0x3D4
#define SVGA_DATA_PORT      0x3D5

// Framebuffer addresses
#define VGA_FB_BASE     0xA0000
#define SVGA_FB_BASE    0xE0000000
#define HIGH_FB_BASE    0xFD000000

// Update system_timer variables to include a forced blink mechanism
static volatile u32 system_ticks = 0;
static volatile u32 ms_counter = 0;
static volatile boolean force_cursor_active = TRUE;

// Cursor blinking variables
static u32 last_blink_tick = 0;
static u32 blink_count = 0;

typedef struct {
    u32*    framebuffer;
    u32     width;
    u32     height;
    u32     pitch;
    u32     bpp;
    u32     fg_color;
    u32     bg_color;
    u32     cursor_x;
    u32     cursor_y;
    boolean initialized;
    boolean cursor_visible;
} screen_t;

static screen_t screen = {
    .framebuffer = (u32*)HIGH_FB_BASE,
    .width = PREFERRED_WIDTH,
    .height = PREFERRED_HEIGHT,
    .pitch = PREFERRED_WIDTH * 4,
    .bpp = PREFERRED_BPP,
    .fg_color = 0xFFFFFFFF,
    .bg_color = 0x00000000,
    .cursor_x = 0,
    .cursor_y = 0,
    .initialized = FALSE,
    .cursor_visible = TRUE,
};

static void init_system_timer(void) {
    timer_init();
    // Try hardware timer first
    timer_set_safe_mode(FALSE);
    if (!timer_enable_hardware()) {
        // Fall back to software timer if hardware fails
        timer_set_safe_mode(TRUE);
        timer_enable();
    }
}

// Use this function somewhere or mark it for potential future use
static u32 get_system_time_ms(void) __attribute__((unused));
static u32 get_system_time_ms(void) {
    // Each timer tick is exactly 10ms at 100Hz
    return timer_get_ticks() * 10;
}

static void draw_pixel(u32 x, u32 y, u32 color) {
    if (!screen.initialized || x >= screen.width || y >= screen.height) {
        return;
    }
    
    u32 offset = y * screen.width + x;
    if (offset < (screen.width * screen.height)) {
        screen.framebuffer[offset] = color;
    }
}

// Function prototypes
static void draw_cursor(void);
static void draw_char_at(char c, u32 x, u32 y);
static void scroll_screen(void);
static boolean try_framebuffer_address(u32* addr);

static void draw_cursor(void) {
    if (!screen.initialized) return;

    u32 cursor_x = screen.cursor_x * (FONT_WIDTH + FONT_SPACING);
    u32 cursor_y = (screen.cursor_y * (u32)FONT_HEIGHT) + ((u32)FONT_HEIGHT - CURSOR_THICKNESS);

    for (u32 i = 0; i < CURSOR_THICKNESS; i++) {
        for (u32 j = 0; j < FONT_WIDTH; j++) {
            // Draw cursor with current visibility state
            draw_pixel(cursor_x + j, cursor_y + i,
                      screen.cursor_visible ? screen.fg_color : screen.bg_color);
        }
    }
}

static boolean try_framebuffer_address(u32* addr) {
    if (!addr) return FALSE;
    
    volatile u32* fb = addr;
    volatile u32 test_value = 0xAA55AA55;
    
    for (int i = 0; i < 16; i++) {
        fb[i] = test_value;
        io_wait();
        if (fb[i] != test_value) {
            return FALSE;
        }
    }
    
    return TRUE;
}

i32 init_screen(void) {
    init_system_timer();
    
    if (try_framebuffer_address((u32*)HIGH_FB_BASE)) {
        screen.framebuffer = (u32*)HIGH_FB_BASE;
    } else if (try_framebuffer_address((u32*)SVGA_FB_BASE)) {
        screen.framebuffer = (u32*)SVGA_FB_BASE;
    } else {
        return SCREEN_ERROR;
    }
    
    screen.initialized = TRUE;
    clear_screen();
    
    // Initialize blinking
    force_cursor_active = TRUE;
    last_blink_tick = timer_get_ticks();
    
    return SCREEN_SUCCESS;
}

static void draw_char_at(char c, u32 x, u32 y) {
    if (!screen.initialized) return;
    
    u8 uc = (u8)c;
    const u8* glyph = font_8x16[uc];
    
    u32 base_x = x * (FONT_WIDTH + FONT_SPACING);
    u32 base_y = y * (u32)FONT_HEIGHT;
    
    // Direct pixel rendering with precise height scaling
    for (u32 dy = 0; dy < FONT_BASE_HEIGHT; dy++) {
        u8 row = glyph[dy];
        float y_stretch = FONT_HEIGHT / FONT_BASE_HEIGHT;
        u32 py_start = base_y + (u32)(dy * y_stretch);
        u32 py_end = base_y + (u32)((dy + 1) * y_stretch);
        
        for (u32 dx = 0; dx < FONT_BASE_WIDTH; dx++) {
            if (row & (0x80 >> dx)) {
                u32 px = base_x + (u32)(dx * FONT_SCALE);
                
                // Draw vertically stretched pixel
                for (u32 py = py_start; py < py_end; py++) {
                    draw_pixel(px, py, screen.fg_color);
                    if (FONT_SCALE > 1.0) {
                        draw_pixel(px + 1, py, screen.fg_color);
                    }
                }
            }
        }
    }
}

void print_char(char c) {
    if (!screen.initialized) return;

    // Store cursor visibility temporarily but don't reset blinking
    boolean was_visible = screen.cursor_visible;
    screen.cursor_visible = FALSE;
    draw_cursor();

    switch (c) {
        case '\n':
            screen.cursor_x = 0;
            screen.cursor_y++;
            break;
        case '\r':
            screen.cursor_x = 0;
            break;
        case '\t':
            screen.cursor_x = (screen.cursor_x + 8) & ~7;
            break;
        case '\b':
            if (screen.cursor_x > 0) {
                screen.cursor_x--;
                u32 base_x = screen.cursor_x * (FONT_WIDTH + FONT_SPACING);
                u32 base_y = screen.cursor_y * (u32)FONT_HEIGHT;
                for (u32 y = 0; y < (u32)FONT_HEIGHT; y++) {
                    for (u32 x = 0; x < FONT_WIDTH + FONT_SPACING; x++) {
                        draw_pixel(base_x + x, base_y + y, screen.bg_color);
                    }
                }
            }
            break;
        default:
            draw_char_at(c, screen.cursor_x, screen.cursor_y);
            screen.cursor_x++;
            break;
    }

    // Update cursor position
    if (screen.cursor_x >= MAX_COLS) {
        screen.cursor_x = 0;
        screen.cursor_y++;
    }
    if (screen.cursor_y >= MAX_ROWS) {
        scroll_screen();
    }

    // Restore cursor visibility without disrupting blink cycle
    screen.cursor_visible = was_visible;
    draw_cursor();
}

void clear_screen(void) {
    if (!screen.initialized) return;

    u32* end = screen.framebuffer + (screen.width * screen.height);
    for (u32* ptr = screen.framebuffer; ptr < end; ptr++) {
        *ptr = screen.bg_color;
    }

    screen.cursor_x = 0;
    screen.cursor_y = 0;
    screen.cursor_visible = TRUE;
    last_blink_tick = timer_get_ticks();  // Start the blink timer
    draw_cursor();
}

static void scroll_screen(void) {
    if (!screen.initialized) return;
    
    u32* src = screen.framebuffer + (screen.width * (u32)FONT_HEIGHT);
    u32* dest = screen.framebuffer;
    u32 size = (screen.height - (u32)FONT_HEIGHT) * screen.width;
    
    for (u32 i = 0; i < size; i++) {
        dest[i] = src[i];
    }
    
    dest = screen.framebuffer + ((screen.height - (u32)FONT_HEIGHT) * screen.width);
    for (u32 i = 0; i < screen.width * (u32)FONT_HEIGHT; i++) {
        dest[i] = screen.bg_color;
    }
    
    screen.cursor_y--;
}

void print_string(const char* str) {
    if (!screen.initialized || !str) return;
    
    while (*str) {
        print_char(*str++);
    }
}

void set_colors(u32 fg, u32 bg) {
    screen.fg_color = fg;
    screen.bg_color = bg;
}

void set_cursor(u32 x, u32 y) {
    if (!screen.initialized) return;

    if (x < MAX_COLS && y < MAX_ROWS) {
        // Hide cursor at old position
        boolean was_visible = screen.cursor_visible;
        screen.cursor_visible = FALSE;
        draw_cursor();

        // Update position
        screen.cursor_x = x;
        screen.cursor_y = y;

        // Restore visibility at new position without disrupting blink
        screen.cursor_visible = was_visible;
        draw_cursor();
    }
}

void get_screen_dimensions(u32* width, u32* height) {
    if (width) *width = screen.width;
    if (height) *height = screen.height;
}

// Force blink immediately - this is called directly by timer interrupt
void screen_timer_tick(void) {
    if (!screen.initialized) return;
    
    // Use the known working approach from debug_cursor_blink
    u32 current_tick = timer_get_ticks();
    
    // Force cursor blink every 50 ticks (500ms at 100Hz)
    if (current_tick - last_blink_tick >= 50) {
        // Toggle cursor visibility
        screen.cursor_visible = !screen.cursor_visible;
        
        // Update counter
        last_blink_tick = current_tick;
        blink_count++;
        
        // Draw cursor with new visibility
        draw_cursor();
    }
}

// Alternative approach in case screen_timer_tick isn't being called properly
void update_cursor_state(void) {
    if (!screen.initialized || !force_cursor_active) return;

    // Check if enough time has elapsed since last blink
    u32 current_tick = timer_get_ticks();
    
    // We know the debug works with this approach (500ms intervals)
    if (current_tick - last_blink_tick >= 50) {
        // Toggle cursor visibility
        screen.cursor_visible = !screen.cursor_visible;
        
        // Record time of this blink
        last_blink_tick = current_tick;
        blink_count++;
        
        // Force redraw of cursor with new visibility
        draw_cursor();
    }
}

// This function must be called periodically from the main code
// Add it to kernel.c main loop if cursor still doesn't blink
void force_cursor_update(void) {
    update_cursor_state();
}

// Improve debug function to show more accurate timing information
void debug_cursor_blink(void) {
    print_string("\n=== CURSOR BLINK DEBUG INFO ===\n");
    
    // Show system timer configuration
    print_string("Timer frequency: ");
    print_int(TIMER_HZ);
    print_string(" Hz\n");
    
    print_string("MS per tick: ");
    print_int(1000 / TIMER_HZ);
    print_string(" ms\n");
    
    print_string("Current timer ticks: ");
    print_int(timer_get_ticks());
    print_string("\n");
    
    print_string("Cursor visible: ");
    print_char(screen.cursor_visible ? '1' : '0');
    print_string("\n");
    
    print_string("Blink interval: ");
    print_int(CURSOR_BLINK_MS);
    print_string(" ms (");
    print_int(CURSOR_BLINK_MS / (1000 / TIMER_HZ));
    print_string(" ticks)\n");
    
    // Timer status check
    print_string("Timer status: ");
    print_string(timer_get_status());
    print_string("\n\n");
    
    print_string("Testing cursor blink timing...\n");
    
    // Display start time
    u32 start_tick = timer_get_ticks();
    print_string("Start tick: ");
    print_int(start_tick);
    print_string("\n");
    
    // Force exactly 3 complete blink cycles (6 toggles)
    u32 expected_duration_ms = CURSOR_BLINK_MS * 6;
    print_string("Expected test duration: ");
    print_int(expected_duration_ms);
    print_string(" ms\n");
    
    // Do 6 toggling (3 complete blink cycles)
    for (int blink = 0; blink < 6; blink++) {
        // Toggle cursor visibility
        screen.cursor_visible = !screen.cursor_visible;
        draw_cursor();
        
        // Sleep for exactly one blink interval
        timer_sleep(CURSOR_BLINK_MS / (1000 / TIMER_HZ));
    }
    
    // Record end time
    u32 end_tick = timer_get_ticks();
    u32 elapsed_ticks = end_tick - start_tick;
    u32 elapsed_ms = elapsed_ticks * (1000 / TIMER_HZ);
    
    print_string("End tick: ");
    print_int(end_tick);
    print_string("\n");
    
    print_string("Elapsed: ");
    print_int(elapsed_ms);
    print_string(" ms (");
    print_int(elapsed_ticks);
    print_string(" ticks)\n");
    
    print_string("Blink test complete.\n");
}

// Add this new function to directly control cursor visibility
void set_cursor_visibility(boolean visible) {
    if (!screen.initialized) return;
    
    // Only redraw if visibility is changing
    if (visible != screen.cursor_visible) {
        screen.cursor_visible = visible;
        draw_cursor();
    }
}