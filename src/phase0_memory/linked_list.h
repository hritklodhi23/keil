/**
 * @file linked_list.h
 * @brief Singly-linked list implementation — Phase 0 learning project
 * 
 * Demonstrates:
 * - Pointer-to-pointer for list manipulation
 * - Dynamic memory management (malloc/free)
 * - Common list patterns (traverse, insert, delete)
 * - Memory safety (null checks, dangling pointers)
 * 
 * Learning goal: Deep understanding of pointers before hardware
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Node in linked list
 * - data: generic pointer (user provides their data structure)
 * - next: pointer to next node or NULL
 * - data_size: size of data (for debugging/validation)
 */
typedef struct linked_list_node {
    void *data;
    struct linked_list_node *next;
    uint16_t data_size;
} linked_list_node_t;

/**
 * Linked list head structure
 * - head: pointer to first node
 * - count: number of nodes
 */
typedef struct {
    linked_list_node_t *head;
    uint32_t count;
} linked_list_t;

/**
 * Initialize empty list
 * @param list pointer to linked_list_t structure
 */
void ll_init(linked_list_t *list);

/**
 * Insert data at head of list (O(1))
 * @param list pointer to linked_list_t structure
 * @param data pointer to data (will be stored, not copied)
 * @param data_size size of data in bytes
 * @return true if successful, false if allocation failed
 */
bool ll_insert_head(linked_list_t *list, void *data, uint16_t data_size);

/**
 * Append data at tail of list (O(n))
 * @param list pointer to linked_list_t structure
 * @param data pointer to data
 * @param data_size size of data in bytes
 * @return true if successful, false if allocation failed
 */
bool ll_append_tail(linked_list_t *list, void *data, uint16_t data_size);

/**
 * Insert data at specific index (O(n))
 * @param list pointer to linked_list_t structure
 * @param index position to insert (0 = head)
 * @param data pointer to data
 * @param data_size size of data in bytes
 * @return true if successful, false if index invalid or allocation failed
 */
bool ll_insert_at(linked_list_t *list, uint32_t index, void *data, uint16_t data_size);

/**
 * Remove node at head and return its data
 * @param list pointer to linked_list_t structure
 * @param data output buffer to store removed data
 * @param data_size maximum size to copy
 * @return true if successful, false if list empty
 */
bool ll_remove_head(linked_list_t *list, void *data, uint16_t data_size);

/**
 * Remove node at specific index
 * @param list pointer to linked_list_t structure
 * @param index position to remove (0 = head)
 * @param data output buffer to store removed data
 * @param data_size maximum size to copy
 * @return true if successful, false if index invalid or list empty
 */
bool ll_remove_at(linked_list_t *list, uint32_t index, void *data, uint16_t data_size);

/**
 * Get data at specific index without removing
 * @param list pointer to linked_list_t structure
 * @param index position to access (0 = head)
 * @param data output buffer to store data
 * @param data_size maximum size to copy
 * @return true if successful, false if index invalid or data is NULL
 */
bool ll_get_at(const linked_list_t *list, uint32_t index, void *data, uint16_t data_size);

/**
 * Get raw node pointer at index (for advanced operations)
 * @param list pointer to linked_list_t structure
 * @param index position to access
 * @return pointer to node or NULL if index invalid
 */
linked_list_node_t *ll_get_node_at(const linked_list_t *list, uint32_t index);

/**
 * Find first node where predicate returns true
 * @param list pointer to linked_list_t structure
 * @param predicate function that returns true if data matches
 * @return pointer to node or NULL if not found
 */
linked_list_node_t *ll_find(const linked_list_t *list, 
                             bool (*predicate)(const void *data));

/**
 * Get number of nodes in list
 */
static inline uint32_t ll_count(const linked_list_t *list) {
    return list ? list->count : 0;
}

/**
 * Check if list is empty
 */
static inline bool ll_is_empty(const linked_list_t *list) {
    return list && (list->count == 0);
}

/**
 * Delete all nodes and clear list
 * @param list pointer to linked_list_t structure
 */
void ll_clear(linked_list_t *list);

/**
 * Print list contents for debugging (uses printf)
 * @param list pointer to linked_list_t structure
 * @param print_fn function that prints a single node's data
 */
void ll_print(const linked_list_t *list, void (*print_fn)(const void *data));

#endif /* LINKED_LIST_H */
