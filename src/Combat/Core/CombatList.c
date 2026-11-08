#include "Combat/Core/CombatList.h"

/// Appends `node` at the tail.
static void CombatList_LinkTail(CombatListNode* node, CombatListNode** head, CombatListNode** tail) {
    if (*head == NULL) {
        *head = node;
    }
    node->prev = *tail;
    if (*tail != NULL) {
        node->prev->next = node;
    }
    node->next = NULL;
    *tail      = node;
}

/// Unlinks `node`.
static void CombatList_Unlink(CombatListNode* node, CombatListNode** head, CombatListNode** tail) {
    if (node->prev == NULL) {
        if (node->next == NULL) {
            *head = NULL;
            *tail = NULL;
            return;
        }
        node->next->prev = NULL;
        *head            = node->next;
    } else if (node->next == NULL) {
        *tail            = node->prev;
        node->prev->next = NULL;
    } else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
}

/// Links `node` in front of `before` (or at the tail when there is no `before`).
static void CombatList_LinkBefore(CombatListNode* node, CombatListNode** head, CombatListNode** tail, CombatListNode* before) {
    if (before != NULL) {
        node->next   = before;
        node->prev   = before->prev;
        before->prev = node;
        if (node->prev != NULL) {
            node->prev->next = node;
        } else {
            *head = node;
        }
        return;
    }
    CombatList_LinkTail(node, head, tail);
}

void* CombatList_GetHead(CombatList* list) {
    return list->head != NULL ? list->head->item : NULL;
}

void* CombatList_GetTail(CombatList* list) {
    return list->tail != NULL ? list->tail->item : NULL;
}

void* CombatList_GetNext(CombatListNode* node) {
    return node->next != NULL ? node->next->item : NULL;
}

void* CombatList_GetPrev(CombatListNode* node) {
    return node->prev != NULL ? node->prev->item : NULL;
}

void CombatList_Init(CombatList* list) {
    list->head = NULL;
    list->tail = NULL;
}

void CombatList_Append(CombatList* list, CombatListNode* node, void* item) {
    node->item = item;
    CombatList_LinkTail(node, &list->head, &list->tail);
}

/// Unlinks `node` and returns the item that followed it.
void* CombatList_Remove(CombatList* list, CombatListNode* node) {
    CombatListNode* next = node->next;

    CombatList_Unlink(node, &list->head, &list->tail);
    return next != NULL ? next->item : NULL;
}

void CombatList_InsertBefore(CombatList* list, CombatListNode* before, CombatListNode* node, void* item) {
    node->item = item;
    CombatList_LinkBefore(node, &list->head, &list->tail, before);
}
