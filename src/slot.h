#ifndef SLOT_H
#define SLOT_H

#include "state.h"

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

Slot*   slot_create ();
void    slot_insert (Slot* list, int value);
void    slot_delete (Slot* list, Node* node);
void    slot_print  (Slot* list);
void    slot_free   (Slot* list);

#endif 