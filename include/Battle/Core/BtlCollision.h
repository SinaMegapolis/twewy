#ifndef TWEWY_BATTLE_CORE_BTLCOLLISION_H
#define TWEWY_BATTLE_CORE_BTLCOLLISION_H

#include "Battle/Core/BtlList.h"

typedef enum {
    BTLCOLLISION_LAYER_PLAYER   = 0,
    BTLCOLLISION_LAYER_ENEMY    = 1,
    BTLCOLLISION_LAYER_UNKNOWN2 = 2,
    BTLCOLLISION_LAYER_EFFECT   = 3,
    BTLCOLLISION_LAYER_UNKNOWN4 = 4,
    BTLCOLLISION_LAYER_UNKNOWN5 = 5
} BtlCollisionLayer;

/// A cylinder that pushes overlapping colliders apart.
typedef struct BtlCollider {
    /* 0x00 */ BtlCollisionLayer layer;
    /* 0x04 */ s32               x;
    /* 0x08 */ s32               y; // stored doubled
    /* 0x0C */ s32               z; // base height
    /* 0x10 */ s32               radius;
    /* 0x14 */ s32               height;
    /* 0x18 */ u16               flags;     // BtlColliderFlag
    /* 0x1A */ u16               reserved1A;
    /* 0x1C */ u32               hitLayers; // bit N: touched a layer N collider last update
    /* 0x20 */ s32               pushX;     // offset away from the last collider hit
    /* 0x24 */ s32               pushY;
    /* 0x28 */ BattleListNode    node;
} BtlCollider; // Size: 0x34

/**
 * @brief Registers a collider on a layer
 *
 * @param collider The collider to add
 * @param layer The layer to add the collider to
 * @param radius The radius of the collider in whole units
 * @param height The height of the collider in whole units
 */
void BtlCollision_Add(BtlCollider* collider, BtlCollisionLayer layer, s32 radius, s32 height);

/**
 * @brief Unregisters a collider from its layer
 *
 * @param collider The collider to remove
 */
void BtlCollision_Remove(BtlCollider* collider);

/**
 * @brief Set whether a collider is disabled, skipping it in normal frame updates
 *
 * @param collider The collider to modify
 * @param disabled TRUE to disable, FALSE to enable
 */
void BtlCollision_SetDisabled(BtlCollider* collider, BOOL disabled);

/**
 * @brief Sets the position of a collider
 *
 * @param collider The collider to move
 * @param x Horizontal position
 * @param y Depth position (stored doubled)
 * @param z Base height
 */
void BtlCollision_SetPosition(BtlCollider* collider, s32 x, s32 y, s32 z);

/**
 * @brief Applies half the pending push offsets from the last collision to the given collider's position
 *
 * @param collider The collider whose push to apply
 * @param x Owner's horizontal position to adjust
 * @param y Owner's depth position to adjust
 * @param z Owner's height (unused)
 */
void BtlCollision_ApplyPush(BtlCollider* collider, s32* x, s32* y, s32* z);

/**
 * @brief Returns the list that colliders on the given layer are stored in
 *
 * @param layer The layer to get the list for
 * @return The list corresponding to the given layer
 */
BattleList* BtlCollision_GetLayerList(BtlCollisionLayer layer);

/**
 * @brief Collides every collider in listA with every one in listB, ignoring the disabled flag.
 *
 * listB is walked from its tail and stops on reaching the listA collider, so a list
 * collided against itself tests each pair once and never a collider against itself.
 *
 * @param listA The first list of colliders to test
 * @param listB The second list of colliders to test
 */
void BtlCollision_CollideAll(BattleList* listA, BattleList* listB);

/**
 * @brief Initializes the collision system, clearing all layer lists
 */
void BtlCollision_Init(void);

/**
 * @brief Runs one frame of collision, checking player vs player, player vs enemies/effects,
 * player vs bosses (ignoring the disabled flag on both sides), and enemies/effects vs each other.
 */
void BtlCollision_Update(void);

#endif // TWEWY_BATTLE_CORE_BTLCOLLISION_H
