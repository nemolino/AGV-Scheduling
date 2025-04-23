#include "solver.h"

// Slot is a doubly linked list with pointers to head and tail
Slot* slot_create() 
{
    Slot* list = (Slot*)safe_malloc(sizeof(Slot));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

// Insert at the end of the list
void slot_insert(Slot* list, State* s)
{
    Node* new_node = (Node*)safe_malloc(sizeof(Node));
    new_node->s = s;
    new_node->prev = list->tail;
    new_node->next = NULL;
    
    if (list->tail) {
        list->tail->next = new_node;
    } else {
        list->head = new_node;
    }
    list->tail = new_node;
    list->size++;
}

// Delete a specific node (given a pointer to the node)
State* slot_delete(Slot* list, Node* node) 
{   
    if (node == NULL) {
        printf("slot_delete : node == NULL\n");
        exit(EXIT_FAILURE);
    }

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    
    State* s = node->s;
    free(node);
    list->size--;

    return s;
}

void slot_print(Slot* list) 
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

void slot_free(Slot* list) 
{
    Node* current = list->head;
    while (current) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

/* Example usage
int main() {
    Slot* list = createSlot();

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