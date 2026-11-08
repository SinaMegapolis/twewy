#ifndef COMBAT_COMBATLIST_H
#define COMBAT_COMBATLIST_H

#include "nitro/types.h"

// Links refer to nodes; the payload may be any battle object.
typedef struct CombatListNode {
    /* 0x0 */ struct CombatListNode* prev;
    /* 0x4 */ struct CombatListNode* next;
    /* 0x8 */ void*                  item;
} CombatListNode; // Size: 0xC

typedef struct CombatList {
    /* 0x0 */ CombatListNode* head;
    /* 0x4 */ CombatListNode* tail;
} CombatList; // Size: 0x8

void* CombatList_GetHead(CombatList*);
void* CombatList_GetTail(CombatList* list);
void* CombatList_GetNext(CombatListNode*);
void* CombatList_GetPrev(CombatListNode*);
void  CombatList_Init(CombatList*);
void  CombatList_Append(CombatList*, CombatListNode*, void*);
void* CombatList_Remove(CombatList*, CombatListNode*);
void  CombatList_InsertBefore(CombatList* list, CombatListNode* before, CombatListNode* node, void* item);

#endif
