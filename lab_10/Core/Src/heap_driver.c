#include "heap_driver.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#define HEAP_START_ADDR ((uint8_t*)0x20001000)
#define HEAP_SIZE (4 * 1024)
#define BLOCK_SIZE 16
#define BLOCK_COUNT (HEAP_SIZE / BLOCK_SIZE)

static uint8_t block_map[BLOCK_COUNT];

void heap_init(void) {
    memset(block_map, 0, sizeof(block_map));
}

void* heap_alloc(size_t size) {
    if (size == 0) return NULL;
    size_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    size_t count = 0, start = 0;
    for (size_t i = 0; i < BLOCK_COUNT; i++) {
        if (block_map[i] == 0) {
            if (count == 0) start = i;
            count++;
            if (count == blocks_needed) {
                for (size_t j = start; j < start + blocks_needed; j++)
                    block_map[j] = 1;
                return (void*)(HEAP_START_ADDR + start * BLOCK_SIZE);
            }
        } else {
            count = 0;
        }
    }
    return NULL;
}

void heap_free(void* ptr) {
    if (ptr == NULL) return;
    uint8_t* p = (uint8_t*)ptr;
    if (p < HEAP_START_ADDR || p >= HEAP_START_ADDR + HEAP_SIZE) return;
    size_t offset = p - HEAP_START_ADDR;
    if (offset % BLOCK_SIZE != 0) return;
    size_t start = offset / BLOCK_SIZE;
    for (size_t i = start; i < BLOCK_COUNT; i++) {
        if (block_map[i] == 1) block_map[i] = 0;
        else break;
    }
}