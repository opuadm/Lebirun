#include "keyboard.h"
#include "screen.h"
#include "shell.h"  // Add this include

void kmain(unsigned long magic __attribute__((unused)),
           unsigned long addr __attribute__((unused))) {
    // Initialize core systems
    init_screen();
    init_keyboard();

    // Clear screen to ensure visibility
    clear_screen();

    // Initial boot message
    print_string("Lebirun 0.1.0 Beta 2\n");
    print_string("Type \"help\" for the list of currently available commands\n");

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
