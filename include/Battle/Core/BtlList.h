#ifndef TWEWY_BATTLE_CORE_BTLLIST_H
#define TWEWY_BATTLE_CORE_BTLLIST_H

#include "nitro/types.h"

/// A link of a battle list, embedded in the object it carries.
typedef struct BattleListNode {
    /* 0x0 */ struct BattleListNode* prev;
    /* 0x4 */ struct BattleListNode* next;
    /* 0x8 */ void*                  item; // the object the node is linked for, usually its owner
} BattleListNode;                          // Size: 0xC

/// A doubly linked list of battle objects: actors, effects, colliders and the like.
typedef struct BattleList {
    /* 0x0 */ BattleListNode* head;
    /* 0x4 */ BattleListNode* tail;
} BattleList; // Size: 0x8

/**
 * @brief Gets the first item of a list
 *
 * @param list The list to read
 * @return The item of the head node, or NULL if the list is empty
 */
void* BtlList_GetHead(BattleList* list);

/**
 * @brief Gets the last item of a list
 *
 * @param list The list to read
 * @return The item of the tail node, or NULL if the list is empty
 */
void* BtlList_GetTail(BattleList* list);

/**
 * @brief Gets the item after a node
 *
 * @param node A node linked into a list
 * @return The item of the next node, or NULL if `node` is the tail
 */
void* BtlList_GetNext(BattleListNode* node);

/**
 * @brief Gets the item before a node
 *
 * @param node A node linked into a list
 * @return The item of the previous node, or NULL if `node` is the head
 */
void* BtlList_GetPrev(BattleListNode* node);

/**
 * @brief Empties a list without touching its nodes
 *
 * @param list The list to reset
 */
void BtlList_Init(BattleList* list);

/**
 * @brief Links a node at the end of a list
 *
 * @param list The list to append to
 * @param node The node to link; must not be in a list already
 * @param item The object the node carries
 */
void BtlList_Append(BattleList* list, BattleListNode* node, void* item);

/**
 * @brief Unlinks a node from a list
 *
 * Lets a loop remove the item it is visiting and carry on with the next one.
 *
 * @param list The list the node is in
 * @param node The node to unlink
 * @return The item that followed `node`, or NULL if it was the tail
 */
void* BtlList_Remove(BattleList* list, BattleListNode* node);

/**
 * @brief Links a node in front of another one
 *
 * @param list The list to insert into
 * @param before The node to insert in front of, or NULL to append at the end
 * @param node The node to link; must not be in a list already
 * @param item The object the node carries
 */
void BtlList_InsertBefore(BattleList* list, BattleListNode* before, BattleListNode* node, void* item);

#endif // TWEWY_BATTLE_CORE_BTLLIST_H
