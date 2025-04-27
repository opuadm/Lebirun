#include "keyboard.h"
#include "screen.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64
#define BUFFER_SIZE 256

static char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static char buffer[BUFFER_SIZE];
static int buf_pos = 0;

static inline unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void init_keyboard(void) {
    // Disable keyboard
    outb(KEYBOARD_COMMAND_PORT, 0xAD);
    
    // Flush output buffer
    while(inb(KEYBOARD_STATUS_PORT) & 1) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    // Enable keyboard
    outb(KEYBOARD_COMMAND_PORT, 0xAE);
}

// Check if keyboard data is available without blocking
boolean keyboard_data_available(void) {
    return (inb(KEYBOARD_STATUS_PORT) & 1);
}

char read_char(void) {
    while(1) {
        if(inb(KEYBOARD_STATUS_PORT) & 1) {
            unsigned char scancode = inb(KEYBOARD_DATA_PORT);
            if(scancode < sizeof(scancode_to_ascii) && scancode_to_ascii[scancode]) {
                return scancode_to_ascii[scancode];
            }
        }
    }
}

char* read_line(void) {
    buf_pos = 0;
    while(1) {
        char c = read_char();
        if(c == '\n') {
            buffer[buf_pos] = '\0';
            return buffer;
        } else if(c == '\b') {
            if(buf_pos > 0) {
                buf_pos--;
                print_char('\b');  // Backspace visual feedback
            }
        } else if(buf_pos < BUFFER_SIZE - 1) {
            buffer[buf_pos++] = c;
            print_char(c);  // Echo input
        }
    }
}