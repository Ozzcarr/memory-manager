#include "linked_list.h"

/**
 * @brief Initializes the linked list.
 *
 * @param head A pointer to the head of the linked list.
 */
void list_init(Node **head) {}

/**
 * @brief Inserts the specified data at the end of the linked list.
 *
 * @param head A pointer to the head of the linked list.
 * @param data The data to insert into the linked list.
 */
void list_insert(Node **head, int data) {}

/**
 * @brief Inserts the specified data after the given node.
 *
 * @param prev_node A pointer to the node to insert data after.
 * @param data The data to insert into the linked list.
 */
void list_insert_after(Node *prev_node, int data) {}

/**
 * @brief Inserts the specified data before the given node.
 *
 * @param prev_node A pointer to the node to insert data before.
 * @param data The data to insert into the linked list.
 */
// A bit trickier than list_insert_after method. You have to consider cases when
// the next_node is the head, you need to find the previous node (note this is a
// single linked list), ...
void list_insert_before(Node **head, Node *next_node, int data) {}

/**
 * @brief Removes a node with the specified data from the linked list.
 *
 * @param head A pointer to the head of the linked list.
 * @param data The data to insert into the linked list.
 */
void list_delete(Node **head, int data) {}

/**
 * @brief Searches for a node with the specified data and returns a pointer to
 * it.
 *
 * @param head A pointer to the head of the linked list.
 * @param data The data to insert into the linked list.
 * @return A pointer to the returned node.
 */
Node *list_search(Node **head, int data) { return NULL; }

/**
 * @brief Prints all elements of the list between two nodes.
 *
 * @param head A pointer to the head of the linked list.
 * @param start_node A pointer to the start node (0 for start of linked list).
 * @param end_node A pointer to the end node (-1 for end of linked list).
 */
void list_display(Node **head, Node *start_node, Node *end_node) {}

/**
 * @brief Counts the number of nodes in the linked list.
 *
 * @param head A pointer to the head of the linked list.
 * @return The number of nodes in the linked list.
 */
int list_count_nodes(Node **head) { return 0; }

/**
 * @brief Frees all the nodes in the linked list.
 *
 * @param head A pointer to the head of the linked list.
 */
void list_cleanup(Node **head) {}
