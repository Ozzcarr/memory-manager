#include "memory_manager.h"

void *memory;
unsigned char *starts;
unsigned char *ends;
size_t memorySize;

/**
 * @brief Sets a bit in the bit array.
 *
 * @param array The bit array.
 * @param index The index of the bit to set.
 */
void set_bit(unsigned char *array, size_t index) {
    array[index / 8] |= (1 << (index % 8));
}

/**
 * @brief Clears a bit in the bit array.
 *
 * @param array The bit array.
 * @param index The index of the bit to clear.
 */
void clear_bit(unsigned char *array, size_t index) {
    array[index / 8] &= ~(1 << (index % 8));
}

/**
 * @brief Checks if a bit is set in the bit array.
 *
 * @param array The bit array.
 * @param index The index of the bit to check.
 * @return True if the bit is set, false otherwise.
 */
bool is_set(const unsigned char *array, size_t index) {
    return array[index / 8] & (1 << (index % 8));
}

/**
 * @brief Initializes the memory manager with the specified size.
 *
 * @param size The size of the memory pool in bytes.
 */
void mem_init(size_t size) {
    memory = malloc(size);
    size_t bitArraySize = (size + 7) / 8;
    starts = calloc(bitArraySize, 1);
    ends = calloc(bitArraySize, 1);
    memorySize = size;
}

/**
 * @brief Allocates a block of memory with the specified size.
 *
 * @param size The size of the allocated block in bytes.
 * @return A pointer to the start of the allocated memory, or NULL if the
 * allocation fails.
 */
void *mem_alloc(size_t size) {
    if (!memory || size == 0 || size > memorySize) return NULL;

    // Loop to find a place to allocate (first-fit)
    int consecutive_free = 0;
    bool occupied = false;
    for (size_t i = 0; i < memorySize; i++) {
        if (is_set(starts, i)) occupied = true;
        consecutive_free = (!occupied) ? consecutive_free + 1 : 0;
        if (consecutive_free == size) {
            // Set memory as allocated
            set_bit(starts, i - size + 1);
            set_bit(ends, i);
            return memory + i - size + 1;
        }
        if (is_set(ends, i)) occupied = false;
    }

    return NULL;
}

/**
 * @brief Frees the specified block of memory.
 *
 * @param block A pointer to the start of the memory block.
 */
void mem_free(void *block) {
    if (!block) return;

    // Get start index to free
    size_t index = block - memory;
    if (index >= memorySize) return;
    if (!is_set(starts, index)) return;

    // Set memory as no longer occupied
    clear_bit(starts, index);
    while (!is_set(ends, index)) index++;
    clear_bit(ends, index);
}

/**
 * @brief Changes the size of the memory block, possibly moving it.
 *
 * @param block A pointer to the start of the memory block.
 * @param size The new size of the memory block.
 * @return A pointer to the start of the resized memory block, or NULL if the
 * resize fails.
 */
void *mem_resize(void *block, size_t size) {
    if (size == 0) {
        if (block) mem_free(block);
        return NULL;
    }

    if (!block) return mem_alloc(size);

    // Get start index to resize
    size_t startIndex = block - memory;
    if (startIndex >= memorySize) return NULL;
    if (!is_set(starts, startIndex)) return NULL;

    // Get end index
    size_t endIndex = startIndex;
    while (!is_set(ends, endIndex)) endIndex++;
    size_t currentSize = endIndex - startIndex + 1;

    mem_free(block);
    void *newBlock = mem_alloc(size);
    if (!newBlock) {
        set_bit(starts, startIndex);
        set_bit(ends, endIndex);
        return NULL;
    }
    size_t newSize = (size <= currentSize) ? size : currentSize;
    if (newBlock != block) memcpy(newBlock, block, newSize);
    return newBlock;
}

/**
 * @brief Deinitializes the memory manager previously initialized with
 * `mem_init`.
 */
void mem_deinit() {
    free(memory);
    free(starts);
    free(ends);
    memorySize = 0;
}
