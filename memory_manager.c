#include "memory_manager.h"

void *memory;
unsigned char *starts;
unsigned char *ends;
size_t mem_size;

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
    size_t bit_array_size = (size + 7) / 8;
    starts = calloc(bit_array_size, 1);
    ends = calloc(bit_array_size, 1);
    mem_size = size;
}

/**
 * @brief Allocates a block of memory with the specified size.
 *
 * @param size The size of the allocated block in bytes.
 * @return A pointer to the start of the allocated memory, or NULL if the
 * allocation fails.
 */
void *mem_alloc(size_t size) {
    if (size == 0 || size > mem_size) return NULL;

    // Loop to find a place to allocate (first-fit)
    int consecutive_free = 0;
    bool occupied = false;
    for (size_t i = 0; i < mem_size; i++) {
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
    if (block == NULL) return;

    // Get start index to free
    size_t index = block - memory;
    if (index >= mem_size) return;
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
    if (startIndex >= mem_size) return NULL;
    if (!is_set(starts, startIndex)) return NULL;

    // Get end index
    size_t endIndex = startIndex;
    while (!is_set(ends, endIndex)) endIndex++;

    size_t current_size = endIndex - startIndex + 1;
    int change = size - current_size;

    // Check if there is space to expand
    if (change > 0) {
        size_t stopIndex = endIndex;
        while (stopIndex < mem_size && !is_set(starts, stopIndex)) stopIndex++;

        // There is no space to expand, move memory block
        if (stopIndex <= endIndex + change) {
            void *newBlock = mem_alloc(size);
            if (!newBlock) return NULL;
            memcpy(newBlock, block, current_size);
            mem_free(block);
            return newBlock;
        }
    }

    // Resize (expand or shrink)
    clear_bit(ends, endIndex);
    set_bit(ends, endIndex + change);
    return block;
}

/**
 * @brief Deinitializes the memory manager previously initialized with
 * `mem_init`.
 */
void mem_deinit() {
    free(memory);
    free(starts);
    free(ends);
    mem_size = 0;
}
