#include "Battle/Core/BtlList.h"

/// Appends `node` at the tail.
static void BtlList_LinkTail(BattleListNode* node, BattleListNode** head, BattleListNode** tail) {
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
static void BtlList_Unlink(BattleListNode* node, BattleListNode** head, BattleListNode** tail) {
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
static void BtlList_LinkBefore(BattleListNode* node, BattleListNode** head, BattleListNode** tail, BattleListNode* before) {
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
    BtlList_LinkTail(node, head, tail);
}

void* BtlList_GetHead(BattleList* list) {
    return list->head != NULL ? list->head->item : NULL;
}

void* BtlList_GetTail(BattleList* list) {
    return list->tail != NULL ? list->tail->item : NULL;
}

void* BtlList_GetNext(BattleListNode* node) {
    return node->next != NULL ? node->next->item : NULL;
}

void* BtlList_GetPrev(BattleListNode* node) {
    return node->prev != NULL ? node->prev->item : NULL;
}

void BtlList_Init(BattleList* list) {
    list->head = NULL;
    list->tail = NULL;
}

void BtlList_Append(BattleList* list, BattleListNode* node, void* item) {
    node->item = item;
    BtlList_LinkTail(node, &list->head, &list->tail);
}

/// Unlinks `node` and returns the item that followed it.
void* BtlList_Remove(BattleList* list, BattleListNode* node) {
    BattleListNode* next = node->next;

    BtlList_Unlink(node, &list->head, &list->tail);
    return next != NULL ? next->item : NULL;
}

void BtlList_InsertBefore(BattleList* list, BattleListNode* before, BattleListNode* node, void* item) {
    node->item = item;
    BtlList_LinkBefore(node, &list->head, &list->tail, before);
}
