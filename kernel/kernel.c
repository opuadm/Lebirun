#include "keyboard.h"
#include "screen.h"
#include "shell.h"
#include "mm.h"

// Define memory size constants (matching definitions in mm.c)
#define KB(x) ((x) * 1024UL)
#define MB(x) (KB(x) * 1024UL)
#define GB(x) (MB(x) * 1024UL)

void kmain(unsigned long mem_size, unsigned long mboot_info_addr __attribute__((unused))) {
    // Memory size validation - set sane limits but don't artificially cap
    if (mem_size < 4 * 1024 * 1024) {
        mem_size = 4 * 1024 * 1024;  // Minimum 4MB
    } 
    else if (mem_size > 0xF0000000) {
        mem_size = 0xF0000000;  // Cap at ~4GB for 32-bit addressing
    }

    if (init_screen() != SCREEN_SUCCESS) {
        return;
    }

    set_colors(VGA_WHITE, VGA_BLACK);
    clear_screen();
    
    print_string("Lebirun 0.1.0 Beta 2 booting...\n");
    print_string("Detected memory: ");
    
    // Print memory size in MB
    uint32_t mem_mb_raw = mem_size / (1024 * 1024);
    char raw_buf[16];
    int raw_pos = 0;
    
    do {
        raw_buf[raw_pos++] = '0' + (mem_mb_raw % 10);
        mem_mb_raw /= 10;
        if (raw_pos >= 10) break;
    } while(mem_mb_raw > 0);
    
    for(int i = raw_pos-1; i >= 0; i--) {
        print_char(raw_buf[i]);
    }
    print_string(" MB\n");
    
    print_string("Initializing memory management...\n");
    int mm_result = mm_init(mem_size); // Pass the actual memory size
    
    if (mm_result != MM_SUCCESS) {
        print_string("ERROR: Failed to initialize memory management (code ");
        char err_code[8];
        int err_pos = 0;
        
        do {
            err_code[err_pos++] = '0' + (mm_result % 10);
            mm_result /= 10;
        } while(mm_result > 0 && err_pos < 7);
        
        for(int i = err_pos-1; i >= 0; i--) {
            print_char(err_code[i]);
        }
        print_string(")\n");
        
        print_string("Retrying with conservative memory settings...\n");
        if (mm_init(4 * 1024 * 1024) != MM_SUCCESS) {
            print_string("FATAL: Memory initialization failed. System halted.\n");
            return;
        }
    }

    print_string("Initializing keyboard...\n");
    init_keyboard();
    
    clear_screen();

    print_string("Lebirun 0.1.0 Beta 2\n");
    
    // Get memory info after initialization to show actual available memory
    mem_info_t info;
    get_memory_info(&info);
    
    // Choose appropriate unit based on size
    if (info.total_memory >= GB(1)) {
        // Display as GB with decimal place for larger memory sizes
        uint32_t gb_whole = info.total_memory / GB(1);
        uint32_t gb_fraction = ((info.total_memory % GB(1)) * 100) / GB(1);
        
        print_string("Memory: ");
        // Removing the unused mem_buf variable
        
        // Convert numbers to strings - using our own conversion functions
        // to avoid dependency on sprintf
        char gb_whole_str[16];
        int pos = 0;
        if (gb_whole == 0) {
            gb_whole_str[pos++] = '0';
        } else {
            uint32_t temp = gb_whole;
            while (temp > 0) {
                gb_whole_str[pos++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        gb_whole_str[pos] = '\0';
        
        // Reverse the string
        for (int i = 0; i < pos/2; i++) {
            char temp = gb_whole_str[i];
            gb_whole_str[i] = gb_whole_str[pos-1-i];
            gb_whole_str[pos-1-i] = temp;
        }
        
        print_string(gb_whole_str);
        print_string(".");
        
        // Ensure we show leading zero for fractions less than 0.10
        if (gb_fraction < 10) print_char('0');
        
        // Convert fraction to string
        char gb_fraction_str[16];
        pos = 0;
        if (gb_fraction == 0) {
            gb_fraction_str[pos++] = '0';
        } else {
            uint32_t temp = gb_fraction;
            while (temp > 0) {
                gb_fraction_str[pos++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        gb_fraction_str[pos] = '\0';
        
        // Reverse the string
        for (int i = 0; i < pos/2; i++) {
            char temp = gb_fraction_str[i];
            gb_fraction_str[i] = gb_fraction_str[pos-1-i];
            gb_fraction_str[pos-1-i] = temp;
        }
        
        print_string(gb_fraction_str);
        print_string(" GB (");
    } else {
        // Display as MB for smaller memory
        uint32_t mem_mb = info.total_memory / MB(1);
        
        // Convert to string
        char mem_mb_str[16];
        int pos = 0;
        if (mem_mb == 0) {
            mem_mb_str[pos++] = '0';
        } else {
            uint32_t temp = mem_mb;
            while (temp > 0) {
                mem_mb_str[pos++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        mem_mb_str[pos] = '\0';
        
        // Reverse the string
        for (int i = 0; i < pos/2; i++) {
            char temp = mem_mb_str[i];
            mem_mb_str[i] = mem_mb_str[pos-1-i];
            mem_mb_str[pos-1-i] = temp;
        }
        
        print_string("Memory: ");
        print_string(mem_mb_str);
        print_string(" MB (");
    }
    
    // Calculate free memory percentage using 32-bit math
    uint32_t free_percent;
    if (info.total_memory > 0) {
        // Avoid overflow by scaling values if needed
        if (info.free_memory <= 429496729 && info.total_memory <= 429496729) { 
            free_percent = (info.free_memory * 100) / info.total_memory;
        } else {
            uint32_t scaled_free = info.free_memory / KB(1);
            uint32_t scaled_total = info.total_memory / KB(1);
            free_percent = (scaled_free * 100) / scaled_total;
        }
    } else {
        free_percent = 0;
    }
    
    // Convert percentage to string
    char percent_str[16];
    int pos = 0;
    if (free_percent == 0) {
        percent_str[pos++] = '0';
    } else {
        uint32_t temp = free_percent;
        while (temp > 0) {
            percent_str[pos++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    percent_str[pos] = '\0';
    
    // Reverse the string
    for (int i = 0; i < pos/2; i++) {
        char temp = percent_str[i];
        percent_str[i] = percent_str[pos-1-i];
        percent_str[pos-1-i] = temp;
    }
    
    print_string(percent_str);
    print_string("% free)\n");
    print_string("Type \"help\" for the list of currently available commands\n");

    // Main command loop
    while(1) {
        print_string(">> ");
        char* cmd = read_line();

        if(cmd[0] == '\0') {
            print_string("\n");
            continue;
        }

        execute_command(cmd);
    }
}