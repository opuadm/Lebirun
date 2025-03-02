#include "keyboard.h"
#include "screen.h"
#include "shell.h"

void kmain(unsigned long magic __attribute__((unused)),
           unsigned long addr __attribute__((unused))) {
    // Initialize core systems - we only have text mode now
    if (init_screen() != SCREEN_SUCCESS) {
        // If screen initialization fails, we can't continue
        return;
    }

    // Set white text on black background using VGA colors
    set_colors(VGA_WHITE, VGA_BLACK);
    init_keyboard();

    // Clear screen to ensure visibility
    clear_screen();

    // Initial boot message
    print_string("Lebirun 0.1.0 Beta 2\n");
    print_string("Type \"help\" for the list of currently available commands\n");

    // Main command loop
    while(1) {
        print_string(">> ");  // Note the space after >>
        char* cmd = read_line();

        // If command is empty, just print a new line
        if(cmd[0] == '\0') {
            print_string("\n");
            continue;  // Skip execute_command and go back to prompt
        }

        execute_command(cmd);
    }
}
