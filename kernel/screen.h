// =============================================================================
// Screen Driver Interface
// Date: 2025-03-01 19:32:49 UTC
// Purpose: VBE display driver interface for 1024x768 resolution
// =============================================================================

#ifndef SCREEN_H
#define SCREEN_H

// Basic type definitions
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed int     i32;

// Screen dimensions
#define VBE_WIDTH  1024
#define VBE_HEIGHT 768
#define VBE_BPP    32

// Return codes
#define SCREEN_SUCCESS 0
#define SCREEN_ERROR  -1

// VGA colors (in RGB format)
#define VGA_BLACK     0x000000
#define VGA_BLUE      0x0000FF
#define VGA_GREEN     0x00FF00
#define VGA_CYAN      0x00FFFF
#define VGA_RED       0xFF0000
#define VGA_MAGENTA   0xFF00FF
#define VGA_BROWN     0x964B00
#define VGA_LGRAY     0x808080
#define VGA_DGRAY     0x404040
#define VGA_LBLUE     0x0000AA
#define VGA_LGREEN    0x00FF00
#define VGA_LCYAN     0x00FFFF
#define VGA_LRED      0xFF0000
#define VGA_LMAGENTA  0xFF00FF
#define VGA_YELLOW    0xFFFF00
#define VGA_WHITE     0xFFFFFF

// Function declarations
i32  init_screen(void);           // Initialize the screen
void clear_screen(void);          // Clear the entire screen
void print_char(char c);          // Print a single character
void print_string(const char* s); // Print a null-terminated string
void set_colors(u32 fg, u32 bg);  // Set foreground and background colors
void set_cursor(u32 x, u32 y);    // Set cursor position

#endif // SCREEN_H