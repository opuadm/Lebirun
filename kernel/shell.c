#include "screen.h"
#include "keyboard.h"
#include "data/about.h"
#include "power.h"
#include "mm.h"

/* Boolean type for readability */
typedef enum { FALSE = 0, TRUE = 1 } boolean;

#define MAX_COMMANDS_PER_PAGE 8
#define MAX_ARGS 8
#define MAX_ARG_LENGTH 64

struct HelpPage {
    char commands[MAX_COMMANDS_PER_PAGE][20];
    char descriptions[MAX_COMMANDS_PER_PAGE][50];
};

struct HelpPage help_pages[2] = {
    // Page 1
    {
        {"help", "clear", "echo", "about", "shutdown", "reboot", "root", "meminfo"},
        {"Show help pages", "Clear screen", "Print text", "Show OS info", "Shutdown computer", "Restart computer", "R00T (Joke Command)", "Show memory info [--kb]"}
    },
    // Page 2
    {
        {"", "", "", "", "", "", "", ""},
        {"", "", "", "", "", "", "", ""}
    }
};

int total_help_pages = 2;

// String operations
int strcmp(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

int strncmp(const char* str1, const char* str2, unsigned int n) {
    while (n > 0 && *str1 && *str2) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }
        str1++;
        str2++;
        n--;
    }
    
    if (n == 0) return 0;
    return *str1 - *str2;
}

char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while ((*dest++ = *src++));
    return original_dest;
}

int str_to_int(const char* str) {
    int result = 0;
    int sign = 1;

    while (*str == ' ') str++;

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

void display_help_page(int page_num) {
    // Validate page number
    if (page_num < 1 || page_num > total_help_pages) {
        print_string("Invalid page number. Available pages: 1-");
        print_char(total_help_pages + '0');
        print_string("\n");
        return;
    }

    // Adjust page number to zero-based index
    page_num--;

    // Print page header
    print_string("Help Page ");
    print_char(page_num + 1 + '0');
    print_string(":\n");

    // Display commands for the specific page
    for (int i = 0; i < MAX_COMMANDS_PER_PAGE; i++) {
        if (help_pages[page_num].commands[i][0] != '\0') {
            print_string(help_pages[page_num].commands[i]);
            print_string(" - ");
            print_string(help_pages[page_num].descriptions[i]);
            print_string("\n");
        }
    }
}

// Print integer
void print_int(int num) {
    char buffer[12];  // Enough for a 32-bit integer
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        print_char('0');
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        print_char('-');
    }

    while (i > 0) {
        print_char(buffer[--i]);
    }
}

// Split command into arguments
int parse_args(char* cmd, char* args[MAX_ARGS]) {
    int count = 0;
    int in_quotes = 0;
    char* p = cmd;
    
    // Skip leading spaces
    while (*p == ' ') p++;
    
    // If empty command, return 0 args
    if (*p == '\0') return 0;
    
    args[count++] = p; // First argument is the command itself
    
    while (*p && count < MAX_ARGS) {
        if (*p == '"') {
            in_quotes = !in_quotes;
            // Remove the quote by shifting everything left
            char* q = p;
            while (*q) {
                *q = *(q+1);
                q++;
            }
            continue;
        }
        
        if (*p == ' ' && !in_quotes) {
            *p = '\0'; // Terminate the current argument
            p++;
            // Skip any additional spaces
            while (*p == ' ') p++;
            if (*p) {
                args[count++] = p; // Start a new argument
            }
        } else {
            p++;
        }
    }
    
    return count;
}

// Define unit constants (matching those in mm.c)
#define UNIT_KB 0
#define UNIT_MB 1

// Main command handler
void execute_command(char* cmd) {
    char* args[MAX_ARGS];
    int arg_count = parse_args(cmd, args);
    
    // Skip if empty command
    if (arg_count == 0) return;
    
    print_string("\n"); // Start output on a new line
    
    // Command dispatch
    if (strcmp(args[0], "clear") == 0) {
        // clear: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: clear (no arguments expected)\n");
        } else {
            clear_screen();
        }
    }
    else if (strcmp(args[0], "help") == 0) {
        int page = 1;  // Default to first page
        
        if (arg_count > 2) {
            print_string("Usage: help [page_number]\n");
        } else if (arg_count == 2) {
            page = str_to_int(args[1]);
            if (page < 1 || page > total_help_pages) {
                print_string("Invalid page number. Available pages: 1-");
                print_char(total_help_pages + '0');
                print_string("\n");
                page = 1;
            }
        }
        
        display_help_page(page);
    }
    else if (strcmp(args[0], "about") == 0) {
        // about: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: about (no arguments expected)\n");
        } else {
            print_string(OS_NAME " v" OS_VERSION "\n");
            print_string("A simple operating system made with C and Assembly\n");
        }
    }
    else if (strcmp(args[0], "echo") == 0) {
        // echo: accepts multiple arguments
        for (int i = 1; i < arg_count; i++) {
            print_string(args[i]);
            if (i < arg_count - 1) {
                print_char(' ');
            }
        }
        print_string("\n");
    }
    else if (strcmp(args[0], "shutdown") == 0) {
        // shutdown: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: shutdown (no arguments expected)\n");
        } else {
            print_string("Initiating system shutdown...\n");
            shutdown_system();
        }
    }
    else if (strcmp(args[0], "reboot") == 0) {
        // reboot: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: reboot (no arguments expected)\n");
        } else {
            print_string("Initiating system reboot...\n");
            reboot_system();
        }
    }
    else if (strcmp(args[0], "root") == 0) {
        // root: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: root (no arguments expected)\n");
        } else {
            print_string("R00T\n");
        }
    }
    else if (strcmp(args[0], "meminfo") == 0) {
        // Check for unit arguments - only allow KB
        int unit = UNIT_MB; // Default to MB
        boolean valid_args = TRUE;
        
        if (arg_count > 2) {
            print_string("Usage: meminfo [--kb]\n");
            valid_args = FALSE;
        } 
        else if (arg_count == 2) {
            if (strcmp(args[1], "--kb") == 0) {
                unit = UNIT_KB;
            } 
            else {
                print_string("Unknown option: ");
                print_string(args[1]);
                print_string("\nUsage: meminfo [--kb]\n");
                valid_args = FALSE;
            }
        }
        
        if (valid_args) {
            mm_dump_stats_with_unit(unit);
        }
    }
    else {
        print_string("Unknown command: ");
        print_string(args[0]);
        print_string("\n");
    }
}