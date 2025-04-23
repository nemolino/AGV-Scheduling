#include "slot.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct Slot {
    Node* head;
    Node* tail;
    int size;
} Slot;

// Slot is a doubly linked list with pointers to head and tail
Slot* slot_create() 
{
    Slot* list = (Slot*)malloc(sizeof(Slot));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

// Insert at the end of the list
void slot_insert(Slot* list, int value) 
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->prev = list->tail;
    newNode->next = NULL;
    
    if (list->tail) {
        list->tail->next = newNode;
    } else {
        list->head = newNode;
    }
    list->tail = newNode;
    list->size++;
}

// Delete a specific node (given a pointer to the node)
void slot_delete(Slot* list, Node* node) 
{
    if (!node) return;

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

    free(node);
    list->size--;
}

void slot_print(Slot* list) 
{
    Node* current = list->head;
    while (current) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
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