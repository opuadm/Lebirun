#ifndef SCREEN_H
#define SCREEN_H

void init_screen(void);
void clear_screen(void);
void print_char(char c);
void print_string(const char* str);
void scroll(void);  // Add this function declaration

#endif // SCREEN_H
