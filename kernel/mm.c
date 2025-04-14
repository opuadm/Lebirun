#include "mm.h"
#include "screen.h"
#include "data/types.h"

/* Simple and reliable memory management system */

/* Constants for memory management */
#define KB(x) ((x) * 1024UL)
#define MB(x) (KB(x) * 1024UL)
#define GB(x) (MB(x) * 1024UL)

/* Boolean type and NULL definition */
typedef enum { FALSE = 0, TRUE = 1 } boolean;
#define NULL ((void*)0)

/* Memory Manager State */
static uint8_t* bitmap = NULL;          // Memory allocation bitmap
static uint32_t total_mem_size = 0;     // Total memory size in bytes
static uint32_t physical_mem_size = 0;  // Actual physical memory size
static uint32_t total_frames = 0;       // Total number of frames
static uint32_t free_frames = 0;        // Number of free frames
static uint32_t reserved_end = 0;       // End of reserved memory region

/* Fixed memory regions - Optimized for lower memory usage */
// Kernel starts at 1MB and extends to reserved_end
#define KERNEL_START    MB(1)
#define BITMAP_START    MB(1) + KB(512) // Bitmap starts at 1.5MB to save space
#define BITMAP_MAX_SIZE KB(256)         // 256KB for bitmap (can track 8GB with 4K pages)

/* Heap Management - Optimized for smaller footprint */
static uint8_t* heap_start = (uint8_t*)HEAP_START;
static uint32_t heap_size = 0;
static uint32_t heap_max = MB(4);       // 4MB maximum heap

// Forward declaration of heap_blocks to fix undeclared identifier error
typedef struct alloc_block {
    uint32_t size;          // Size of this block (including header)
    uint8_t is_free;        // 1 if free, 0 if used
    struct alloc_block* next; // Next block in list
} alloc_block_t;

// Start of heap blocks
static alloc_block_t* heap_blocks = NULL;

// Add this function externally accessible for kernel to use
void int_to_str(uint32_t num, char* str) {
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    int i = 0;
    while (num > 0) {
        str[i++] = '0' + (num % 10);
        num /= 10;
    }
    str[i] = '\0';
    
    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

// Print a decimal number
static void print_dec(uint32_t num) {
    char buf[16];
    int_to_str(num, buf);
    print_string(buf);
}

// Set bit in bitmap
static void bitmap_set(uint32_t frame) {
    uint32_t byte = frame / 8;
    uint8_t bit = frame % 8;
    bitmap[byte] |= (1 << bit);
}

// Clear bit in bitmap
static void bitmap_clear(uint32_t frame) {
    uint32_t byte = frame / 8;
    uint8_t bit = frame % 8;
    bitmap[byte] &= ~(1 << bit);
}

// Test if bit is set
static boolean bitmap_test(uint32_t frame) {
    uint32_t byte = frame / 8;
    uint8_t bit = frame % 8;
    return (bitmap[byte] & (1 << bit)) != 0;
}

// Find first free frame
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

// Initialize memory management
int mm_init(uint32_t mem_size) {
    // Universal memory management - works for all platforms
    
    // Store actual physical memory size reported by bootloader
    physical_mem_size = mem_size;
    
    // Minimal memory requirement (absolute minimum)
    const uint32_t MIN_MEM = MB(2);
    
    // Enforce minimum memory size for stability
    if (mem_size < MIN_MEM) {
        mem_size = MIN_MEM;  // Enforce minimum
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

// Allocate a physical frame
void* alloc_frame() {
    int32_t frame = bitmap_first_free();
    if (frame == -1) {
        return NULL;  // No free frames
    }
    
    bitmap_set(frame);
    free_frames--;
    
    // Convert frame number to physical address
    return (void*)(frame * PAGE_SIZE);
}

// Free a physical frame
void free_frame(void* addr) {
    uint32_t frame = (uint32_t)addr / PAGE_SIZE;
    
    // Prevent freeing reserved memory
    if (frame < reserved_end / PAGE_SIZE) {
        return;
    }
    
    // Make sure frame is in range
    if (frame >= total_frames) {
        return;
    }
    
    // Only clear bit if it's currently set
    if (bitmap_test(frame)) {
        bitmap_clear(frame);
        free_frames++;
    }
}

// Get number of free frames
uint32_t get_free_frames() {
    return free_frames;
}

// Initialize heap
static void init_heap() {
    // If already initialized
    if (heap_blocks != NULL) {
        return;
    }
    
    // Create initial block covering entire heap
    heap_blocks = (alloc_block_t*)heap_start;
    heap_blocks->size = heap_size;
    heap_blocks->is_free = 1;
    heap_blocks->next = NULL;
}

// Extend heap if needed
static boolean extend_heap(uint32_t size) {
    // If new size would exceed maximum
    if (heap_size + size > heap_max) {
        return FALSE;
    }
    
    // Create a new block at the end of the heap
    alloc_block_t* last = heap_blocks;
    while (last->next != NULL) {
        last = last->next;
    }
    
    alloc_block_t* new_block = (alloc_block_t*)(heap_start + heap_size);
    new_block->size = size;
    new_block->is_free = 1;
    new_block->next = NULL;
    
    // Link it to the chain
    last->next = new_block;
    
    // Update heap size
    heap_size += size;
    
    return TRUE;
}

// Split a block if necessary
static void split_block(alloc_block_t* block, uint32_t size) {
    // Only split if there's enough space for a new block + header
    if (block->size > size + sizeof(alloc_block_t) + 4) {
        alloc_block_t* new_block = (alloc_block_t*)((uint8_t*)block + size);
        new_block->size = block->size - size;
        new_block->is_free = 1;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

// Allocate memory
void* kmalloc(size_t size) {
    // Initialize heap if needed
    if (heap_blocks == NULL) {
        init_heap();
    }
    
    // Ensure minimum size and alignment
    if (size < 4) {
        size = 4;
    }
    
    // Align size to 4-byte boundary
    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }
    
    // Add header size
    uint32_t total_size = size + sizeof(alloc_block_t);
    
    // Find a free block of sufficient size
    alloc_block_t* block = heap_blocks;
    
    while (block != NULL) {
        if (block->is_free && block->size >= total_size) {
            // Found a suitable block
            block->is_free = 0;
            
            // Split if needed
            split_block(block, total_size);
            
            // Return pointer to memory after the header
            return (void*)((uint8_t*)block + sizeof(alloc_block_t));
        }
        
        block = block->next;
    }
    
    // No suitable block found, try to extend heap
    if (extend_heap(total_size > KB(4) ? total_size : KB(4))) {
        // Try allocation again
        return kmalloc(size);
    }
    
    // Out of memory
    return NULL;
}

// Merge adjacent free blocks
static void merge_free_blocks() {
    alloc_block_t* block = heap_blocks;
    
    while (block != NULL && block->next != NULL) {
        if (block->is_free && block->next->is_free) {
            // Merge with next block
            block->size += block->next->size;
            block->next = block->next->next;
        } else {
            block = block->next;
        }
    }
}

// Free memory
void kfree(void* ptr) {
    if (ptr == NULL || heap_blocks == NULL) {
        return;
    }
    
    // Convert to block pointer
    alloc_block_t* block = (alloc_block_t*)((uint8_t*)ptr - sizeof(alloc_block_t));
    
    // Validate pointer is within heap
    if ((uint8_t*)block < heap_start || 
        (uint8_t*)block >= heap_start + heap_size) {
        return;
    }
    
    // Mark as free
    block->is_free = 1;
    
    // Merge adjacent free blocks
    merge_free_blocks();
}

// Allocate and clear memory
void* kcalloc(size_t nmemb, size_t size) {
    // Calculate total size, checking for overflow
    size_t total = nmemb * size;
    if (size != 0 && total / size != nmemb) {
        return NULL; // Overflow
    }
    
    // Allocate memory
    void* ptr = kmalloc(total);
    if (ptr != NULL) {
        // Clear memory
        memset(ptr, 0, total);
    }
    
    return ptr;
}

// Reallocate memory block
void* krealloc(void* ptr, size_t size) {
    // Special cases
    if (ptr == NULL) {
        return kmalloc(size);
    }
    
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    // Get original block header
    alloc_block_t* block = (alloc_block_t*)((uint8_t*)ptr - sizeof(alloc_block_t));
    
    // Validate pointer is within heap
    if ((uint8_t*)block < heap_start || 
        (uint8_t*)block >= heap_start + heap_size) {
        return NULL;
    }
    
    // Calculate actual size available in this block (without header)
    uint32_t actual_size = block->size - sizeof(alloc_block_t);
    
    // If requested size fits in current block, just return the same pointer
    if (size <= actual_size) {
        // We could potentially split the block here for efficiency
        return ptr;
    }
    
    // Need to allocate a larger block
    void* new_ptr = kmalloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    // Copy old data to new block
    memcpy(new_ptr, ptr, actual_size);
    
    // Free old block
    kfree(ptr);
    
    return new_ptr;
}

// Memory utility functions

// Set memory to a value
void* memset(void* dest, int value, size_t count) {
    uint8_t* ptr = (uint8_t*)dest;
    uint8_t val = (uint8_t)value;
    
    for (size_t i = 0; i < count; i++) {
        ptr[i] = val;
    }
    
    return dest;
}

// Copy memory
void* memcpy(void* dest, const void* src, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    // Make sure we don't overflow
    if ((uint32_t)dest + count < (uint32_t)dest ||
        (uint32_t)src + count < (uint32_t)src) {
        return dest;
    }
    
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

// Move memory (handles overlapping regions)
void* memmove(void* dest, const void* src, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    // Handle overlap
    if (d < s) {
        // Copy forward
        for (size_t i = 0; i < count; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        // Copy backward
        for (size_t i = count; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
    
    return dest;
}

// Compare memory
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
    
    if (heap_blocks != NULL) {
        alloc_block_t* block = heap_blocks;
        alloc_block_t* prev = NULL;
        uint32_t consecutive_free = 0;
        
        while (block != NULL) {
            if (!block->is_free) {
                used_blocks++;
                // Reset consecutive free counter
                consecutive_free = 0;
            } else {
                // Update consecutive free counter
                consecutive_free += block->size;
                if (consecutive_free > largest_free_block) {
                    largest_free_block = consecutive_free;
                }
            }
            
            // Check for fragmentation - small free blocks surrounded by used blocks
            if (block->is_free && prev && !prev->is_free && 
                block->next && !block->next->is_free && 
                block->size < 1024) { // Small block (< 1KB)
                fragmentation++;
            }
            
            prev = block;
            block = block->next;
        }
    }
    
    info->block_count = used_blocks;
    
    // Enhance with additional info (could be accessed via mm_dump_stats)
    info->largest_free_block = largest_free_block;
    info->fragmentation_count = fragmentation;
}

// Define unit constants for memory display
#define UNIT_KB 0
#define UNIT_MB 1
#define UNIT_GB 2

// Print memory statistics with specified display unit
void mm_dump_stats_with_unit(int unit) {
    print_string("=== Memory Information ===\n");
    
    const char* unit_str;
    uint32_t divisor;
    
    // Determine appropriate unit based on memory size
    if (unit == UNIT_KB) {
        unit_str = "KB";
        divisor = KB(1);
    } else if (physical_mem_size >= GB(1)) {
        unit_str = "GB";
        divisor = GB(1);
    } else {
        unit_str = "MB";
        divisor = MB(1);
    }
    
    // Total memory - use physical memory size
    print_string("Total Memory: ");
    
    if (divisor == GB(1)) {
        // For GB, show with decimal point
        uint32_t whole_part = physical_mem_size / divisor;
        uint32_t decimal_part = ((physical_mem_size % divisor) * 100) / divisor;
        
        print_dec(whole_part);
        print_string(".");
        if (decimal_part < 10) print_char('0');
        print_dec(decimal_part);
    } else {
        print_dec(physical_mem_size / divisor);
    }
    
    print_string(" ");
    print_string(unit_str);
    print_string("\n");
    
    // Free memory
    uint32_t free_memory = free_frames * PAGE_SIZE;
    print_string("Free Memory: ");
    
    if (divisor == GB(1) && free_memory >= GB(1)) {
        uint32_t whole_part = free_memory / divisor;
        uint32_t decimal_part = ((free_memory % divisor) * 100) / divisor;
        
        print_dec(whole_part);
        print_string(".");
        if (decimal_part < 10) print_char('0');
        print_dec(decimal_part);
    } else {
        print_dec(free_memory / divisor);
    }
    
    print_string(" ");
    print_string(unit_str);
    
    // Calculate percentage - avoiding 32-bit overflow
    uint32_t percentage;
    if (physical_mem_size == 0) {
        percentage = 0;
    } else if (physical_mem_size > 0x10000000) { // If > 256MB
        // Scale down to avoid overflow
        percentage = ((free_memory / KB(1)) * 100) / (physical_mem_size / KB(1));
    } else {
        percentage = (free_memory * 100) / physical_mem_size;
    }
    
    print_string(" (");
    print_dec(percentage);
    print_string("%)\n");
    
    // Used memory
    uint32_t used_memory = physical_mem_size - free_memory;
    print_string("Used Memory: ");
    
    if (divisor == GB(1) && used_memory >= GB(1)) {
        uint32_t whole_part = used_memory / divisor;
        uint32_t decimal_part = ((used_memory % divisor) * 100) / divisor;
        
        print_dec(whole_part);
        print_string(".");
        if (decimal_part < 10) print_char('0');
        print_dec(decimal_part);
    } else {
        print_dec(used_memory / divisor);
    }
    
    print_string(" ");
    print_string(unit_str);
    print_string("\n");
    
    // OS memory usage
    uint32_t os_footprint = HEAP_START + heap_size - KERNEL_START;
    print_string("OS Memory Usage: ");
    print_dec(os_footprint / divisor);
    print_string(" ");
    print_string(unit_str);
    print_string("\n");
    
    // Print heap information
    print_string("Heap Size: ");
    print_dec(heap_size / divisor);
    print_string(" ");
    print_string(unit_str);
    print_string("\n");
    
    // Block info
    uint32_t free_blocks = 0;
    uint32_t used_blocks = 0;
    uint32_t largest_free = 0;
    
    if (heap_blocks != NULL) {
        alloc_block_t* block = heap_blocks;
        while (block != NULL) {
            if (block->is_free) {
                free_blocks++;
                if (block->size > largest_free) {
                    largest_free = block->size;
                }
            } else {
                used_blocks++;
            }
            block = block->next;
        }
    }
    
    print_string("Heap blocks: ");
    print_dec(used_blocks);
    print_string(" used, ");
    print_dec(free_blocks);
    print_string(" free\n");
    
    print_string("Largest free block: ");
    print_dec(largest_free / divisor);
    print_string(" ");
    print_string(unit_str);
    print_string("\n");
}

// Original function for backward compatibility
void mm_dump_stats() {
    mm_dump_stats_with_unit(UNIT_MB);
}
