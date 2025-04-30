#include "solver.h"

/**
 * @brief Allocates an empty DLL
 * 
 * @return A pointer to the allocated DLL
 */
DLL* dll_create() 
{
    DLL* list = (DLL*)safe_malloc(sizeof(DLL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

/**
 * @brief De-allocates dll and all the states that it contains
 * 
 * @param dll A pointer to the DLL to free
 */
void dll_free(DLL* dll) 
{
    Node* current = dll->head;
    while (current) {
        Node* temp = current;
        current = current->next;
        state_destroy(temp->s);
        free(temp);
    }
    free(dll);
}

/**
 * @brief allocates a Node containing s, inserts it at the end of dll and returns it
 * 
 * @param dll 
 * @param s 
 * @return Node* 
 */
Node* dll_insert(DLL* dll, State* s)
{
    Node* new_node = (Node*)safe_malloc(sizeof(Node));
    new_node->s = s;
    new_node->prev = dll->tail;
    new_node->next = NULL;
    
    if (dll->tail) {
        dll->tail->next = new_node;
    } else {
        dll->head = new_node;
    }
    dll->tail = new_node;
    dll->size++;

    return new_node;
}

/**
 * @brief deletes node from dll (given a pointer to it) and returns the state contained in node
 * 
 * @param dll 
 * @param node 
 * @return State* 
 */
State* dll_delete(DLL* dll, Node* node) 
{   
    if (node == NULL) {
        printf("dll_delete : node == NULL\n");
        exit(EXIT_FAILURE);
    }

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        dll->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        dll->tail = node->prev;
    }
    
    State* s = node->s;
    free(node);
    dll->size--;

    return s;
}

/*
void dll_print(DLL* list) 
{
    if (!list->head){
        printf("empty slot\n");
        return;
    }
    Node* current = list->head;
    while (current) {
        printf("%p ", current->s);
        current = current->next;
    }
}
*/

/* Example usage
int main() {
    DLL* list = createSlot();

    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtEnd(list, 40);
    insertAtEnd(list, 50);

    printList(list);
    printf("Number of elements: %d\n", list->size);

    // Delete the second node
    deleteNode(list, list->head);
    printList(list);
    printf("Number of elements: %d\n", list->size);
    deleteNode(list, list->head->next);
    printList(list);
    printf("Number of elements: %d\n", list->size);
    deleteNode(list, list->tail);
    printList(list);
    printf("Number of elements: %d\n", list->size);

    freeList(list);
    return 0;
}
*/