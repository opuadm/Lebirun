#include "screen.h"
#include "data/font_data.h"
#include "data/types.h"

typedef enum { FALSE = 0, TRUE = 1 } boolean;
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

// System timer variables
static volatile u32 system_ticks = 0;
static volatile u32 ms_counter = 0;

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
    u32     last_cursor_toggle;
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
    .last_cursor_toggle = 0
};

static inline void outb(u16 port, u8 value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline u8 inb(u16 port) {
    u8 value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

// Initialize system timer (PIT)
static void init_system_timer(void) {
    u32 divisor = PIT_FREQUENCY / SYSTEM_TIMER_HZ;
    outb(PIT_MODE, 0x36);        // Mode 3 (square wave)
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

// Get current system time in milliseconds
static u32 get_system_time_ms(void) {
    return (system_ticks * 1000) / SYSTEM_TIMER_HZ;
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

static void draw_cursor(void) {
    u32 cursor_x = screen.cursor_x * (FONT_WIDTH + FONT_SPACING);
    u32 cursor_y = (screen.cursor_y * (u32)FONT_HEIGHT) + ((u32)FONT_HEIGHT - CURSOR_THICKNESS);
    
    for (u32 i = 0; i < CURSOR_THICKNESS; i++) {
        for (u32 j = 0; j < FONT_WIDTH; j++) {
            draw_pixel(cursor_x + j, cursor_y + i, 
                      screen.cursor_visible ? screen.fg_color : screen.bg_color);
        }
    }
}

static void update_cursor(void) {
    u32 current_time = get_system_time_ms();
    
    if (current_time - screen.last_cursor_toggle >= CURSOR_BLINK_MS) {
        screen.cursor_visible = !screen.cursor_visible;
        screen.last_cursor_toggle = current_time;
        draw_cursor();
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
    screen.last_cursor_toggle = get_system_time_ms();
    clear_screen();
    
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

void clear_screen(void) {
    if (!screen.initialized) return;
    
    u32* end = screen.framebuffer + (screen.width * screen.height);
    for (u32* ptr = screen.framebuffer; ptr < end; ptr++) {
        *ptr = screen.bg_color;
    }
    
    screen.cursor_x = 0;
    screen.cursor_y = 0;
    screen.cursor_visible = TRUE;
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

void print_char(char c) {
    if (!screen.initialized) return;
    
    // Clear current cursor
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
    
    if (screen.cursor_x >= MAX_COLS) {
        screen.cursor_x = 0;
        screen.cursor_y++;
    }
    
    if (screen.cursor_y >= MAX_ROWS) {
        scroll_screen();
    }
    
    // Draw cursor at new position
    screen.cursor_visible = TRUE;
    draw_cursor();
}

void print_string(const char* str) {
    if (!screen.initialized || !str) return;
    
    while (*str) {
        print_char(*str++);
        update_cursor();  // Keep cursor updated while printing
    }
}

void set_colors(u32 fg, u32 bg) {
    screen.fg_color = fg;
    screen.bg_color = bg;
}

void set_cursor(u32 x, u32 y) {
    if (!screen.initialized) return;
    
    if (x < MAX_COLS && y < MAX_ROWS) {
        // Clear current cursor
        screen.cursor_visible = FALSE;
        draw_cursor();
        
        screen.cursor_x = x;
        screen.cursor_y = y;
        
        // Draw cursor at new position
        screen.cursor_visible = TRUE;
        draw_cursor();
    }
}

void get_screen_dimensions(u32* width, u32* height) {
    if (width) *width = screen.width;
    if (height) *height = screen.height;
}

// Call this from your interrupt handler
void screen_timer_tick(void) {
    system_ticks++;
    ms_counter += 1000 / SYSTEM_TIMER_HZ;
    update_cursor();
}
