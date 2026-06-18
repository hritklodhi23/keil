/**
 * @file linked_list.c
 * @brief Singly-linked list implementation
 */

#include "linked_list.h"
#include <stdlib.h>
#include <string.h>

void ll_init(linked_list_t *list) {
    if (list) {
        list->head = NULL;
        list->count = 0;
    }
}

bool ll_insert_head(linked_list_t *list, void *data, uint16_t data_size) {
    if (!list || !data) {
        return false;
    }
    
    linked_list_node_t *new_node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));
    if (!new_node) {
        return false;
    }
    
    new_node->data = data;
    new_node->data_size = data_size;
    new_node->next = list->head;
    
    list->head = new_node;
    list->count++;
    
    return true;
}

bool ll_append_tail(linked_list_t *list, void *data, uint16_t data_size) {
    if (!list || !data) {
        return false;
    }
    
    linked_list_node_t *new_node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));
    if (!new_node) {
        return false;
    }
    
    new_node->data = data;
    new_node->data_size = data_size;
    new_node->next = NULL;
    
    if (list->head == NULL) {
        list->head = new_node;
    } else {
        linked_list_node_t *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    
    list->count++;
    
    return true;
}

bool ll_insert_at(linked_list_t *list, uint32_t index, void *data, uint16_t data_size) {
    if (!list || !data || index > list->count) {
        return false;
    }
    
    if (index == 0) {
        return ll_insert_head(list, data, data_size);
    }
    
    linked_list_node_t *current = list->head;
    for (uint32_t i = 0; i < index - 1; i++) {
        if (current == NULL) {
            return false;
        }
        current = current->next;
    }
    
    linked_list_node_t *new_node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));
    if (!new_node) {
        return false;
    }
    
    new_node->data = data;
    new_node->data_size = data_size;
    new_node->next = current->next;
    current->next = new_node;
    
    list->count++;
    
    return true;
}

bool ll_remove_head(linked_list_t *list, void *data, uint16_t data_size) {
    if (!list || list->head == NULL) {
        return false;
    }
    
    linked_list_node_t *node = list->head;
    
    if (data && data_size > 0) {
        memcpy(data, node->data, (data_size < node->data_size) ? data_size : node->data_size);
    }
    
    list->head = node->next;
    free(node);
    list->count--;
    
    return true;
}

bool ll_remove_at(linked_list_t *list, uint32_t index, void *data, uint16_t data_size) {
    if (!list || index >= list->count) {
        return false;
    }
    
    if (index == 0) {
        return ll_remove_head(list, data, data_size);
    }
    
    linked_list_node_t *current = list->head;
    for (uint32_t i = 0; i < index - 1; i++) {
        if (current == NULL || current->next == NULL) {
            return false;
        }
        current = current->next;
    }
    
    linked_list_node_t *node = current->next;
    if (!node) {
        return false;
    }
    
    if (data && data_size > 0) {
        memcpy(data, node->data, (data_size < node->data_size) ? data_size : node->data_size);
    }
    
    current->next = node->next;
    free(node);
    list->count--;
    
    return true;
}

bool ll_get_at(const linked_list_t *list, uint32_t index, void *data, uint16_t data_size) {
    if (!list || !data || index >= list->count) {
        return false;
    }
    
    linked_list_node_t *current = list->head;
    for (uint32_t i = 0; i < index; i++) {
        if (current == NULL) {
            return false;
        }
        current = current->next;
    }
    
    if (!current || !current->data) {
        return false;
    }
    
    memcpy(data, current->data, (data_size < current->data_size) ? data_size : current->data_size);
    
    return true;
}

linked_list_node_t *ll_get_node_at(const linked_list_t *list, uint32_t index) {
    if (!list || index >= list->count) {
        return NULL;
    }
    
    linked_list_node_t *current = list->head;
    for (uint32_t i = 0; i < index; i++) {
        if (current == NULL) {
            return NULL;
        }
        current = current->next;
    }
    
    return current;
}

linked_list_node_t *ll_find(const linked_list_t *list, 
                             bool (*predicate)(const void *data)) {
    if (!list || !predicate) {
        return NULL;
    }
    
    linked_list_node_t *current = list->head;
    while (current != NULL) {
        if (current->data && predicate(current->data)) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

void ll_clear(linked_list_t *list) {
    if (!list) {
        return;
    }
    
    linked_list_node_t *current = list->head;
    while (current != NULL) {
        linked_list_node_t *next = current->next;
        free(current);
        current = next;
    }
    
    list->head = NULL;
    list->count = 0;
}

void ll_print(const linked_list_t *list, void (*print_fn)(const void *data)) {
    if (!list || !print_fn) {
        return;
    }
    
    linked_list_node_t *current = list->head;
    uint32_t index = 0;
    
    while (current != NULL) {
        printf("[%ld] -> ", index);
        if (current->data) {
            print_fn(current->data);
        }
        current = current->next;
        index++;
    }
    printf("NULL\n");
}
