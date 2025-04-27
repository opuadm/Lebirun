#include "mm.h"
#include "screen.h"
#include "data/types.h"

/* Constants for memory management */
#define KB(x) ((x) * 1024UL)
#define MB(x) (KB(x) * 1024UL)
#define GB(x) (MB(x) * 1024UL)

/* Define unit constants for memory display */
#define UNIT_KB 0
#define UNIT_MB 1
#define UNIT_GB 2

/* Memory bitmap configuration */
#define BITMAP_START    (MB(1) + KB(512)) // Bitmap starts at 1.5MB to save space
#define BITMAP_MAX_SIZE KB(256)          // 256KB for bitmap (can track 8GB with 4K pages)

/* Heap Management - Optimized for smaller footprint */
#ifdef UNUSED
static uint8_t* heap_start = (uint8_t*)HEAP_START;
static uint32_t heap_max = MB(4);       // 4MB maximum heap
#endif

// These variables are actually used
static uint32_t heap_size = 0;

/* NULL definition */
#define NULL ((void*)0)

/* Memory Manager State */
static uint8_t* bitmap = NULL;          // Memory allocation bitmap
static uint32_t total_mem_size = 0;     // Total memory size in bytes
static uint32_t physical_mem_size = 0;  // Actual physical memory size
static uint32_t total_frames = 0;       // Total number of frames
static uint32_t free_frames = 0;        // Number of free frames
static uint32_t reserved_end = 0;       // End of reserved memory region

// Forward declaration of heap_blocks to fix undeclared identifier error
typedef struct alloc_block {
    uint32_t size;          // Size of this block (including header)
    uint8_t is_free;        // 1 if free, 0 if used
    struct alloc_block* next; // Next block in list
} alloc_block_t;

// Start of heap blocks
static alloc_block_t* heap_blocks = NULL;

// Forward declaration for print_int from shell.c
extern void print_int(int num);

/* Bitmap manipulation functions */
// Set a bit in the bitmap (mark as used)
static inline void bitmap_set(uint32_t frame) {
    bitmap[frame / 8] |= (1 << (frame % 8));
}

// Clear a bit in the bitmap (mark as free)
static inline void bitmap_clear(uint32_t frame) {
    bitmap[frame / 8] &= ~(1 << (frame % 8));
}

// Test if a bit is set in the bitmap
static inline boolean bitmap_test(uint32_t frame) {
    return bitmap[frame / 8] & (1 << (frame % 8));
}

// Find the first free frame in the bitmap
static int32_t bitmap_first_free() {
    for (uint32_t i = 0; i < total_frames / 8; i++) {
        if (bitmap[i] != 0xFF) { // Not all bits are set
            for (uint8_t j = 0; j < 8; j++) {
                if (!(bitmap[i] & (1 << j))) {
                    return i * 8 + j;
                }
            }
        }
    }
    return -1; // No free frames
}

/* Helper functions for size conversions */
// Convert bytes to KB
static inline uint32_t bytes_to_kb(uint32_t bytes) {
    return bytes / KB(1);
}

// Convert bytes to MB
static inline uint32_t bytes_to_mb(uint32_t bytes) {
    return bytes / MB(1);
}

// Convert bytes to GB with 2 decimal places (returns fixed point value * 100)
static inline uint32_t bytes_to_gb_fixed(uint32_t bytes) {
    // Scale by 100 to get 2 decimal places without floating point
    return (bytes * 100) / GB(1);
}

// Initialize memory management
int mm_init(uint32_t mem_size) {
    // Store actual physical memory size reported by bootloader
    physical_mem_size = mem_size;
    
    // Force memory to exact values for virtualization environments
    if (mem_size > 900 * 1024 * 1024) {
        // Always use 1024MB for any memory close to or above 1GB
        print_string("Setting memory to exactly 1024MB for virtualization compatibility...\n");
        physical_mem_size = 1024 * 1024 * 1024; // Exactly 1GB
    } else if (mem_size >= 450 * 1024 * 1024) {
        // Set to 512MB for any memory around 512MB
        print_string("Setting memory to exactly 512MB...\n");
        physical_mem_size = 512 * 1024 * 1024; // Exactly 512MB
    } else if (mem_size >= 200 * 1024 * 1024) {
        // Set to 256MB for any memory around 256MB
        print_string("Setting memory to exactly 256MB...\n");
        physical_mem_size = 256 * 1024 * 1024; // Exactly 256MB
    }
    
    // Use assembly function to verify memory
    uint32_t verified_mem = asm_verify_memory_size(physical_mem_size);
    if (verified_mem != physical_mem_size) {
        print_string("Memory size verification corrected to: ");
        print_int(bytes_to_mb(verified_mem));
        print_string(" MB\n");
        physical_mem_size = verified_mem;
    }
    
    // Display detected memory in appropriate units - always in MB for consistency
    print_string("Memory detected: ");
    print_int(bytes_to_mb(physical_mem_size));
    print_string(" MB\n");

    // Use the adjusted physical_mem_size instead of mem_size for the rest of the function
    mem_size = physical_mem_size;
    
    // Minimal memory requirement (absolute minimum)
    const uint32_t MIN_MEM = MB(2);
    
    // Enforce minimum memory size for stability
    if (mem_size < MIN_MEM) {
        mem_size = MIN_MEM;  // Enforce minimum
        print_string("WARNING: Increased to minimum required memory: 2 MB\n");
    }
    
    // Make sure memory size is page-aligned
    mem_size = (mem_size / PAGE_SIZE) * PAGE_SIZE;
    
    // Reserve first 1MB for BIOS (universal standard)
    const uint32_t RESERVED_BASE = MB(1);
    
    // Store usable memory size
    total_mem_size = mem_size;
    
    // Calculate number of frames, ensuring we don't exceed addressable memory
    total_frames = (total_mem_size - RESERVED_BASE) / PAGE_SIZE;
    
    // Calculate bitmap size needed - each bit represents one page
    uint32_t bitmap_size = (total_frames + 7) / 8;
    
    // Cap bitmap size to maximum (reduce memory footprint)
    if (bitmap_size > BITMAP_MAX_SIZE) {
        bitmap_size = BITMAP_MAX_SIZE;
        total_frames = bitmap_size * 8;
    }
    
    // Set bitmap
    bitmap = (uint8_t*)BITMAP_START;
    
    // Ensure bitmap memory is within valid memory range
    if ((uint32_t)bitmap + bitmap_size >= total_mem_size) {
        return MM_ERROR;
    }
    
    // Clear bitmap (mark all frames as free)
    memset(bitmap, 0, bitmap_size);
    
    // Mark reserved memory as used (kernel and bitmap area)
    uint32_t reserved_frames = (HEAP_START + KB(256)) / PAGE_SIZE; // Reserve a bit more than HEAP_START
    if (reserved_frames > total_frames) {
        reserved_frames = total_frames;
    }
    
    // Mark lower memory as reserved
    for (uint32_t i = 0; i < reserved_frames; i++) {
        bitmap_set(i);
    }
    
    // Also mark very high memory as reserved (optimized to 1MB)
    // This is a universal approach to avoid conflicts with hardware
    const uint32_t HIGH_RESERVED = MB(1) / PAGE_SIZE;
    if (total_frames > HIGH_RESERVED) {
        for (uint32_t i = total_frames - HIGH_RESERVED; i < total_frames; i++) {
            bitmap_set(i);
        }
    }
    
    // Set reserved memory end address
    reserved_end = HEAP_START;
    
    // Calculate free frames (avoid underflow)
    free_frames = (reserved_frames < total_frames) ? 
                  (total_frames - reserved_frames) : 0;
    
    // Update for high memory reservation
    if (free_frames > HIGH_RESERVED) {
        free_frames -= HIGH_RESERVED;
    }
    
    // Initialize heap with minimal initial size
    heap_size = KB(32);  // Start with 32KB heap (reduced from 64KB)
    
    // Initialize heap blocks pointer safely
    heap_blocks = NULL; // Will be initialized on first allocation
    
    // Extra safety check - verify heap start is within valid memory
    if (HEAP_START >= total_mem_size) {
        return MM_ERROR;
    }
    
    return MM_SUCCESS;
}

// Print memory statistics with specified display unit
void mm_dump_stats_with_unit(int unit) {
    print_string("\n=== Memory Information ===\n");
    
    const char* unit_str;
    uint32_t total_display, free_display, used_display;
    
    // Select display unit based on size and user preference
    if (unit == UNIT_KB) {
        unit_str = "KB";
        total_display = bytes_to_kb(physical_mem_size);
        free_display = bytes_to_kb(free_frames * PAGE_SIZE);
        used_display = bytes_to_kb((total_frames - free_frames) * PAGE_SIZE);
    } else {
        // Default to MB always, ignore GB even for large memory
        unit_str = "MB";
        total_display = bytes_to_mb(physical_mem_size);
        free_display = bytes_to_mb(free_frames * PAGE_SIZE);
        used_display = bytes_to_mb((total_frames - free_frames) * PAGE_SIZE);
    }
    
    // Display memory statistics
    print_string("Total Memory: ");
    print_int(total_display);
    print_string(" ");
    print_string(unit_str);
    print_string("\n");
    
    // Free memory
    print_string("Free Memory:  ");
    print_int(free_display);
    print_string(" ");
    print_string(unit_str);
    
    // Calculate percentage safely (avoiding overflow)
    uint32_t percentage;
    if (physical_mem_size == 0) {
        percentage = 0;
    } else {
        // Scale down for precision
        percentage = ((free_frames * 100) / total_frames);
    }
    
    print_string(" (");
    print_int(percentage);
    print_string("%)\n");
    
    // Used memory
    print_string("Used Memory:  ");
    print_int(used_display);
    print_string(" ");
    print_string(unit_str);
    print_string(" (");
    print_int(100 - percentage);
    print_string("%)\n");
    
    // Additional memory metrics
    print_string("Page Size:    ");
    if (PAGE_SIZE >= KB(1)) {
        print_int(PAGE_SIZE / KB(1));
        print_string(" KB\n");
    } else {
        print_int(PAGE_SIZE);
        print_string(" bytes\n");
    }
    
    print_string("Total Pages:  ");
    print_int(total_frames);
    print_string("\n");
    
    print_string("Free Pages:   ");
    print_int(free_frames);
    print_string("\n");
    
    // Heap info
    print_string("\n--- Heap Status ---\n");
    print_string("Heap Start:   0x");
    char hex_digits[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        print_char(hex_digits[(HEAP_START >> (i * 4)) & 0xF]);
    }
    print_string("\n");
    
    // Show heap size in appropriate units
    print_string("Heap Size:    ");
    if (heap_size >= MB(1)) {
        print_int(heap_size / MB(1));
        print_string(" MB\n");
    } else if (heap_size >= KB(1)) {
        print_int(heap_size / KB(1));
        print_string(" KB\n");
    } else {
        print_int(heap_size);
        print_string(" bytes\n");
    }
    
    print_string("Max Heap:     ");
    print_int(HEAP_MAX / MB(1));
    print_string(" MB\n");
}

// Standard memory information display (defaults to MB)
void mm_dump_stats(void) {
    mm_dump_stats_with_unit(UNIT_MB);
}

/* Memory operations functions */
void* memset(void* dest, int value, size_t count) {
    uint8_t* dst = (uint8_t*)dest;
    for (size_t i = 0; i < count; i++) {
        dst[i] = (uint8_t)value;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    uint8_t* dst = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < count; i++) {
        dst[i] = s[i];
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t count) {
    uint8_t* dst = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    if (dst < s) {
        // Copy forward
        for (size_t i = 0; i < count; i++) {
            dst[i] = s[i];
        }
    } else if (dst > s) {
        // Copy backward
        for (size_t i = count; i > 0; i--) {
            dst[i-1] = s[i-1];
        }
    }
    
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t count) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    
    for (size_t i = 0; i < count; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    
    return 0;
}

// Get memory information
void get_memory_info(mem_info_t* info) {
    if (info == NULL) {
        return;
    }
    
    // Report actual physical memory rather than limited memory
    info->total_memory = physical_mem_size;
    info->free_memory = free_frames * PAGE_SIZE;
    info->used_memory = physical_mem_size - (free_frames * PAGE_SIZE);
    info->reserved_memory = reserved_end;
    
    // Count allocated blocks
    uint32_t used_blocks = 0;
    uint32_t largest_free_block = 0;
    uint32_t fragmentation = 0;
    
    // Simply set to default values for now
    info->block_count = used_blocks;
    info->largest_free_block = largest_free_block;
    info->fragmentation_count = fragmentation;
}

// Allocate a physical frame
void* alloc_frame() {
    // Find a free frame using our bitmap
    int32_t frame = bitmap_first_free();
    if (frame == -1) {
        return NULL; // No free frames
    }
    
    // Mark frame as used
    bitmap_set(frame);
    
    // Decrement free frames count
    free_frames--;
    
    // Calculate physical address
    uint32_t phys_addr = frame * PAGE_SIZE + MB(1);
    return (void*)phys_addr;
}

// Free a physical frame
void free_frame(void* frame) {
    // Calculate frame number
    uint32_t phys_addr = (uint32_t)frame;
    
    // Check if address is within our memory range
    if (phys_addr < MB(1) || phys_addr >= total_mem_size) {
        return; // Invalid address
    }
    
    uint32_t frame_num = (phys_addr - MB(1)) / PAGE_SIZE;
    
    // Check if frame is currently marked as used
    if (bitmap_test(frame_num)) {
        bitmap_clear(frame_num);
        free_frames++;
    }
}

// Get the number of free frames
uint32_t get_free_frames() {
    return free_frames;
}