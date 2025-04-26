#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "screen.h" // Include for boolean type

// Initialize keyboard
void init_keyboard(void);

// Get a single character from keyboard input
char read_char(void);

// Read a full line of input
char* read_line(void);

// Check if keyboard has data available
boolean keyboard_data_available(void);

#endif // KEYBOARD_H