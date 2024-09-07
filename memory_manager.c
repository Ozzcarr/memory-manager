#include "memory_manager.h"

void *memory;
unsigned char *occupied;
unsigned char *starts;
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
bool is_bit_set(const unsigned char *array, size_t index) {
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
    occupied = calloc(bit_array_size, 1);
    starts = calloc(bit_array_size, 1);
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
    if (size == 0 || size > mem_size) {
        return NULL;
    }

    // Loop to find a place to allocate (first-fit)
    int consecutive_free = 0;
    for (size_t i = 0; i < mem_size; i++) {
        consecutive_free =
            (!is_bit_set(occupied, i)) ? consecutive_free + 1 : 0;
        if (consecutive_free == size) {
            // Set memory as allocated
            for (size_t j = 0; j < size; j++) {
                set_bit(occupied, i - j);
            }
            set_bit(starts, i - size + 1);

            return memory + i - size + 1;
        }
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
    if (!is_bit_set(starts, index)) return;
    clear_bit(starts, index);

    // Set memory as no longer occupied
    for (size_t i = index; i < mem_size; i++) {
        if (is_bit_set(starts, i) || !is_bit_set(occupied, i)) break;
        clear_bit(occupied, i);
    }
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
    if (size <= 0) {
        if (block) mem_free(block);
        return NULL;
    };
    if (!block) return mem_alloc(size);

    // Get start index to resize
    size_t index = block - memory;
    if (index >= mem_size) return NULL;

    // Get current size
    size_t current_size;
    if (is_bit_set(starts, index)) {
        current_size = 1;
        for (size_t i = index + 1; i < mem_size; i++) {
            if (is_bit_set(starts, i) || !is_bit_set(occupied, i)) break;
            current_size++;
        }
    } else {
        return NULL;
    }

    int change = size - current_size;

    if (change > 0) {
        // Check if there is space to expand
        size_t available = current_size;
        for (size_t i = index + current_size; i < mem_size; i++) {
            if (is_bit_set(starts, i) || is_bit_set(occupied, i)) break;
            available++;
        }

        if (available >= size) {
            // Expand
            for (size_t i = index + current_size; i < index + size; i++) {
                set_bit(occupied, i);
            }

            return block;
        } else {
            // Move memory block
            void *newBlock = mem_alloc(size);
            if (!newBlock) return NULL;
            memcpy(newBlock, block, current_size);
            mem_free(block);
            return newBlock;
        }
    } else if (change < 0) {
        // Shrink
        for (size_t i = index + size; i < index + current_size; i++) {
            clear_bit(occupied, i);
        }
        return block;
    }

    return block;
}

/**
 * @brief Deinitializes the memory manager previously initialized with
 * `mem_init`.
 */
void mem_deinit() {
    free(memory);
    free(occupied);
    free(starts);
    mem_size = 0;
}
