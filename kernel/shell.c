#include "screen.h"
#include "keyboard.h"
#include "about.h"

#define MAX_COMMANDS_PER_PAGE 6

struct HelpPage {
    char commands[MAX_COMMANDS_PER_PAGE][20];
    char descriptions[MAX_COMMANDS_PER_PAGE][50];
};

struct HelpPage help_pages[2] = {
    // Page 1
    {
        {"help", "clear", "echo", "about", "", ""},
        {"Show help pages", "Clear screen", "Print text", "Show info about this OS", "", ""}
    },
    // Page 2
    {
        {"", "", "", "", "", ""},  // Add actual commands for page 2 if needed
        {"", "", "", "", "", ""}
    }
};

int total_help_pages = 2;

void display_help_page(int page_num) {
    // Validate page number
    if (page_num < 1 || page_num > total_help_pages) {
        print_string("Invalid page number. Available pages are: 1 and 2\n");
        return;
    }

    // Adjust page number to zero-based index
    page_num--;

    // Print page header
    print_string("Help Page ");
    char page_str[2];
    page_str[0] = page_num + 1 + '0';  // Convert to character
    page_str[1] = '\0';
    print_string(page_str);
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

// String comparison function
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

// Partial string comparison function
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

// String to integer conversion
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

// Integer printing function
int print_int(int num) {
    char buffer[12];  // Enough for a 32-bit integer
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        print_char('0');
        return 0;
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

    return 0;
}

void execute_command(char* cmd) {
    // Trim leading whitespace
    while(*cmd == ' ') cmd++;

    // If empty command, just return
    if(cmd[0] == '\0') {
        return;
    }

    // Ensure output starts on a new line
    print_string("\n");

    if(strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    else if(strncmp(cmd, "help", 4) == 0) {
        int page = 1;  // Default to first page

        // Check if a page number is specified
        if(cmd[4] == ' ') {
            page = str_to_int(cmd + 5);  // Directly use the input page number

            // Validate page number
            if(page < 1 || page > total_help_pages) {
                page = 1;  // Falls back to first page if invalid
            }
        }

        display_help_page(page);  // Passes the page number
    }
    else if(strcmp(cmd, "about") == 0) {
        print_string(OS_NAME " v" OS_VERSION "\n");
        print_string("A simple operating system made with C and Assembly\n");
    }
    else if(strncmp(cmd, "echo", 4) == 0) {
        // Check if there's text after "echo"
        if(cmd[4] == ' ') {
            print_string(cmd + 5);  // Print everything after "echo "
            print_string("\n");
        } else {
            print_string("Usage: echo <text>\n");
        }
    }
    else {
        print_string("Unknown command: ");
        print_string(cmd);
        print_string("\n");
    }
}
