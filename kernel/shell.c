#include "screen.h"
#include "keyboard.h"
#include "data/about.h"
#include "power.h"
#include "mm.h"
#include "interrupts/exceptions.h" 
#include "interrupts/idt_checker.h"
#include "drivers/timer.h"

// Remove the conflicting boolean definition - use the one from timer.h
// typedef enum { FALSE = 0, TRUE = 1 } boolean;

#define MAX_COMMANDS_PER_PAGE 8
#define MAX_ARGS 8
#define MAX_ARG_LENGTH 64

struct HelpPage {
    char commands[MAX_COMMANDS_PER_PAGE][20];
    char descriptions[MAX_COMMANDS_PER_PAGE][50];
};

// Add a static variable to track debug mode
static boolean debug_mode = FALSE;

// Restructured help pages with vertical formatting
// Standard commands only (page 1)
struct HelpPage help_pages[2] = {
    // Page 1 - Standard commands
    {
        // Commands
        {
            "help",
            "clear",
            "echo",
            "about",
            "shutdown",
            "reboot",
            "root",
            "meminfo"
        },
        // Descriptions
        {
            "Show help pages",
            "Clear screen",
            "Print text",
            "Show OS info",
            "Shutdown computer",
            "Restart computer",
            "R00T (Joke Command)",
            "Show memory info [--kb]"
        }
    },
    // Page 2 - Debug commands (only shown in debug mode)
    {
        // Commands
        {
            "debug",
            "timer",
            "irqtest",
            "exception",
            "sysdiag",
            "",
            "",
            ""
        },
        // Descriptions
        {
            "Control debug mode [--on/--off]",
            "Timer control functions",
            "Test IRQ handling (timer sleep)",
            "Trigger a test exception",
            "Run system diagnostics",
            "",
            "",
            ""
        }
    }
};

// Update total help pages
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
    if (page_num < 0 || page_num >= total_help_pages) {
        print_string("Invalid help page number.\n");
        return;
    }
    
    // Skip the debug page if debug mode is disabled and user requested it directly
    if (page_num == 1 && !debug_mode) {
        print_string("Help page 2 is not available.\n");
        return;
    }
    
    print_string("Help Page ");
    print_char(page_num + 1 + '0');
    print_string(":\n");

    // Display commands for the specific page
    for (int i = 0; i < MAX_COMMANDS_PER_PAGE; i++) {
        // Only show debug command in regular help if debug mode is disabled
        if (page_num == 1 && !debug_mode && i > 0) {
            continue;
        }
        
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

// Add these for hex display in the shell
void print_hex(uint32_t value) {
    const char* hex_chars = "0123456789ABCDEF";
    char buffer[9];
    int i;
    
    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    buffer[8] = '\0';
    
    print_string("0x");
    print_string(buffer);
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

// Update trigger_test_exception to be more robust
void trigger_test_exception(void) {
    print_string("Starting controlled exception test...\n");
    
    // Call the controlled exception handler that has the NEM safety check
    int result = trigger_controlled_exception();
    
    if (result) {
        print_string("Exception handling configured successfully.\n");
    } else {
        print_string("Exception handler configuration check failed.\n");
        print_string("This could indicate an issue with the interrupt system.\n");
    }
}

void test_irq_handling(void) {
    print_string("Testing timer IRQ handling...\n");
    
    uint32_t start_ticks = timer_get_ticks();
    print_string("Current tick count: ");
    print_int(start_ticks);
    print_string("\n");
    
    print_string("Enabling timer interrupts temporarily...\n");
    print_string("Simulating sleep for 3 seconds (300 ticks at 100Hz)...\n");
    
    // Try to enable real hardware timer interrupt
    // It will fall back to simulation if interrupts don't work
    timer_sleep(300);
    
    uint32_t end_ticks = timer_get_ticks();
    print_string("Woke up! Current tick count: ");
    print_int(end_ticks);
    print_string("\n");
    
    print_string("Elapsed ticks: ");
    print_int(end_ticks - start_ticks);
    print_string("\n");
    
    if (end_ticks - start_ticks == 300) {
        print_string("Note: This may have been a simulated timer test.\n");
        print_string("Hardware timer interrupts may still be unavailable.\n");
    }
}

void test_timer_control(void) {
    print_string("\n=== TIMER CONTROL AND TESTING ===\n");
    print_string("Current timer status: ");
    print_string(timer_get_status());
    print_string("\nCurrent tick count: ");
    print_int(timer_get_ticks());
    print_string("\n\n");
    
    // Show options menu
    print_string("Timer test options:\n");
    print_string("1. Test hardware timer (force enable)\n");
    print_string("2. Test simulated timer (force simulation)\n");
    print_string("3. Set timer debug level\n");
    print_string("4. Show current timer status\n");
    print_string("5. Run standard sleep test\n");
    print_string("6. Go back\n\n");
    
    print_string("Enter option (1-6): ");
    char* input = read_line();
    int option = str_to_int(input);
    
    switch (option) {
        case 1:
            print_string("\nForcing hardware timer mode...\n");
            timer_set_safe_mode(FALSE);
            if (timer_enable_hardware()) {
                print_string("Hardware timer enabled successfully!\n");
            } else {
                print_string("Failed to enable hardware timer!\n");
            }
            test_irq_handling();
            break;
            
        case 2:
            print_string("\nForcing simulation mode...\n");
            timer_set_safe_mode(TRUE);
            test_irq_handling();
            break;
            
        case 3:
            print_string("\nEnter debug level (0-3): ");
            input = read_line();
            int level = str_to_int(input);
            if (level >= 0 && level <= 3) {
                timer_set_debug_level(level);
                print_string("Debug level set to ");
                print_int(level);
                print_string("\n");
            } else {
                print_string("Invalid debug level! Valid range is 0-3.\n");
            }
            break;
            
        case 4:
            print_string("\nTimer Status: ");
            print_string(timer_get_status());
            print_string("\nTick Count: ");
            print_int(timer_get_ticks());
            print_string("\n");
            break;
            
        case 5:
            test_irq_handling();
            break;
            
        case 6:
            return;
            
        default:
            print_string("\nInvalid option!\n");
            break;
    }
    
    // Recurse to show menu again
    test_timer_control();
}

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
        // Modified help command to only show first page by default
        if (arg_count > 2) {
            print_string("Usage: help [page_number]\n");
        } else if (arg_count == 2) {
            // Convert page number string to int
            int page = str_to_int(args[1]) - 1; // Pages shown as 1-based, stored as 0-based
            
            // Adjust available pages based on debug mode
            int available_pages = debug_mode ? total_help_pages : 1;
            
            if (page >= 0 && page < available_pages) {
                display_help_page(page);
            } else {
                print_string("Invalid page number. Available pages: 1");
                if (debug_mode) {
                    print_string(", 2");
                }
                print_string("\n");
            }
        } else {
            // Show only the first page when no page number provided
            display_help_page(0);
            
            // If debug mode is on, show that there's another page available
            if (debug_mode) {
                print_string("\nType 'help 2' to see debug commands.\n");
            }
        }
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
    // Debug commands - only available when debug mode is enabled
    else if (debug_mode && strcmp(args[0], "exception") == 0) {
        // exception: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: exception (no arguments expected)\n");
        } else {
            print_string("Triggering a test exception (division by zero)...\n");
            trigger_test_exception();
        }
    }
    else if (debug_mode && strcmp(args[0], "sysdiag") == 0) {
        // sysdiag: run system diagnostics
        if (arg_count > 1) {
            print_string("Usage: sysdiag (no arguments expected)\n");
        } else {
            print_string("Running system diagnostics...\n");
            diagnose_interrupt_system();
        }
    }
    else if (debug_mode && strcmp(args[0], "irqtest") == 0) {
        // irqtest: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: irqtest (no arguments expected)\n");
        } else {
            test_irq_handling();
        }
    }
    else if (debug_mode && strcmp(args[0], "timer") == 0) {
        // timer: no arguments expected
        if (arg_count > 1) {
            print_string("Usage: timer (no arguments expected)\n");
        } else {
            test_timer_control();
        }
    }
    // Debug command is always available
    else if (strcmp(args[0], "debug") == 0) {
        if (arg_count < 2) {
            // Show current debug status
            print_string("Debug mode is currently ");
            print_string(debug_mode ? "ON" : "OFF");
            print_string("\n");
            print_string("Usage: debug [--on | --off | -o | -n]\n");
            print_string("  --on, -o    Enable debug mode\n");
            print_string("  --off, -n   Disable debug mode\n");
        } 
        else if (strcmp(args[1], "--on") == 0 || strcmp(args[1], "-o") == 0) {
            debug_mode = TRUE;
            print_string("Debug mode enabled. Additional commands now available.\n");
            print_string("Use 'help 2' to see all available debug commands.\n");
        }
        else if (strcmp(args[1], "--off") == 0 || strcmp(args[1], "-n") == 0) {
            debug_mode = FALSE;
            print_string("Debug mode disabled. Debug commands hidden.\n");
        }
        else if (debug_mode && strcmp(args[1], "--cursor-blink") == 0) {
            debug_cursor_blink();
        }
        else if (debug_mode && strcmp(args[1], "--memory") == 0) {
            mm_dump_stats();
        }
        else if (debug_mode && strcmp(args[1], "--timer") == 0) {
            test_timer_control();
        }
        else {
            print_string("Unknown debug option: ");
            print_string(args[1]);
            print_string("\n");
            print_string("Usage: debug [--on | --off | -o | -n]\n");
            if (debug_mode) {
                print_string("Debug options:\n");
                print_string("  --cursor-blink   Debug cursor blinking functionality\n");
                print_string("  --memory         Display memory information\n");
                print_string("  --timer          Test timer functionality\n");
            }
        }
    }
    else {
        print_string("Unknown command: ");
        print_string(args[0]);
        print_string("\n");
    }
}

// Add this shell_main function if it doesn't exist
void shell_main(void) {
    // Main command loop
    while(1) {
        print_string(">> ");
        
        // Force cursor to blink right before reading input
        screen_timer_tick(); // Force an immediate cursor update
        
        char* cmd = read_line();

        if(cmd[0] == '\0') {
            print_string("\n");
            continue;
        }

        execute_command(cmd);
    }
}