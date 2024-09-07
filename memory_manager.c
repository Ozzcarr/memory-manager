#include "memory_manager.h"

void *memory;
bool *occupied;
bool *starts;
size_t mem_size;

/**
 * @brief Initializes the memory manager with the specified size.
 *
 * @param size The size of the memory pool in bytes.
 */
void mem_init(size_t size) {
    memory = malloc(size);
    occupied = calloc(size, sizeof(bool));
    starts = calloc(size, sizeof(bool));
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
        consecutive_free = (occupied[i] == 0) ? consecutive_free + 1 : 0;
        if (consecutive_free == size) {
            // Set memory as allocated
            for (size_t j = 0; j < size; j++) {
                occupied[i - j] = 1;
            }
            starts[i - size + 1] = 1;

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
    if (starts[index] == 0) return;
    starts[index] = 0;

    // Set memory as no longer occupied
    for (size_t i = index; i < mem_size; i++) {
        if (starts[i] == 1 || occupied[i] == 0) break;
        occupied[i] = 0;
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
    if (starts[index] == 1) {
        current_size = 1;
        for (size_t i = index + 1; i < mem_size; i++) {
            if (starts[i] == 1 || occupied[i] == 0) break;
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
            if (starts[i] == 1 || occupied[i] == 1) break;
            available++;
        }

        if (available >= size) {
            // Expand
            for (size_t i = index + current_size; i < index + size; i++) {
                occupied[i] = 1;
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
            occupied[i] = 0;
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
