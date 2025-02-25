#include "screen.h"

#define VIDEO_MEMORY 0xB8000
#define COLS 80
#define ROWS 25
#define ATTR 0x0F  // White on black

static int cursor_x = 0;
static int cursor_y = 0;
static unsigned short* video_memory = (unsigned short*)VIDEO_MEMORY;

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void update_cursor(void) {
    unsigned short pos = cursor_y * COLS + cursor_x;

    // Set cursor location
    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0xFF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);

    // Enable cursor with slow blink
    outb(0x3D4, 0x0A);
    unsigned char cursor_start = inb(0x3D5) & 0xC0;  // Preserve existing bits
    outb(0x3D5, cursor_start | 0x0E);  // Cursor start line

    outb(0x3D4, 0x0B);
    unsigned char cursor_end = inb(0x3D5) & 0xE0;  // Preserve existing bits
    outb(0x3D5, cursor_end | 0x0F);  // Cursor end line
}

void init_screen(void) {
    // Configure cursor for slow blink
    outb(0x3D4, 0x0A);
    unsigned char cursor_start = inb(0x3D5) & 0xC0;
    outb(0x3D5, cursor_start | 0x20);  // Bit 5 enables slow blink

    clear_screen();
}

void clear_screen(void) {
    for(int i = 0; i < ROWS * COLS; i++) {
        video_memory[i] = (ATTR << 8) | ' ';
    }
    cursor_x = cursor_y = 0;
    update_cursor();
}

void scroll(void) {
    if(cursor_y >= ROWS) {
        for(int i = 0; i < (ROWS-1) * COLS; i++) {
            video_memory[i] = video_memory[i + COLS];
        }
        for(int i = (ROWS-1) * COLS; i < ROWS * COLS; i++) {
            video_memory[i] = (ATTR << 8) | ' ';
        }
        cursor_y = ROWS - 1;
    }
}

void print_char(char c) {
    if(c == '\n') {
        cursor_x = 0;
        cursor_y++;
        scroll();
    } else if(c == '\b') {
        if(cursor_x > 0) {
            cursor_x--;
            video_memory[cursor_y * COLS + cursor_x] = (ATTR << 8) | ' ';
        }
    } else {
        video_memory[cursor_y * COLS + cursor_x] = (ATTR << 8) | c;
        cursor_x++;
        if(cursor_x >= COLS) {
            cursor_x = 0;
            cursor_y++;
            scroll();
        }
    }
    update_cursor();
}

void print_string(const char* str) {
    while(*str) {
        print_char(*str++);
    }
}
