#ifndef MM_H
#define MM_H

#include "data/types.h"

// Memory management return codes
#define MM_SUCCESS      0
#define MM_ERROR        1
#define MM_OUT_OF_MEM   2

// Memory constants - optimized for smaller memory footprint
#define PAGE_SIZE       4096UL   // 4KB pages
#define HEAP_START      0x200000UL // Start heap at 2MB (reduced from 3MB)
#define HEAP_INITIAL    0x8000UL   // 32KB initial heap size (reduced from 1MB)
#define HEAP_MAX        0x400000UL // 4MB maximum heap size
#define BLOCKS_PER_BYTE 8        // 8 blocks per byte (1 bit per block)
#define BLOCK_SIZE      16       // 16 bytes per allocation block

// Memory info structure - enhanced with additional fields
typedef struct {
    uint32_t total_memory;      // Total physical memory in bytes
    uint32_t free_memory;       // Free physical memory in bytes
    uint32_t used_memory;       // Used physical memory in bytes
    uint32_t reserved_memory;   // Reserved (unavailable) memory in bytes
    uint32_t block_count;       // Number of allocated blocks
    uint32_t largest_free_block; // Size of largest contiguous free block
    uint32_t fragmentation_count; // Count of small isolated free blocks
} mem_info_t;

// Initialization 
int mm_init(uint32_t mem_size);

// Physical memory management
void* alloc_frame();
void free_frame(void* frame);
uint32_t get_free_frames();

// Heap memory management
void* kmalloc(size_t size);
void* kcalloc(size_t nmemb, size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

// Memory operations
void* memset(void* dest, int value, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
void* memmove(void* dest, const void* src, size_t count);
int memcmp(const void* s1, const void* s2, size_t count);

// Memory information
void get_memory_info(mem_info_t* info);
void mm_dump_stats();
void mm_dump_stats_with_unit(int unit);

// Helper functions
void int_to_str(uint32_t num, char* str);

#endif // MM_H
