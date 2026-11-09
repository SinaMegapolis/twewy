#include "Battle/Core/BtlCollision.h"

#include "Combat/Core/BtlMgr.h"
#include "FixedPoint.h"

#include <nitro/fx.h>
#include <nitro/math.h>

#define BTLCOLLIDER_FLAG_DISABLED (1 << 0)

typedef enum {
    BTLCOLLISION_LIST_PLAYER = 0,
    BTLCOLLISION_LIST_ENEMY  = 1, // enemies and effects
    BTLCOLLISION_LIST_BOSS   = 2, // large boss hitboxes
    BTLCOLLISION_LIST_COUNT  = 3
} BtlCollisionList;

static CombatList BtlCollision_LayerLists[BTLCOLLISION_LIST_COUNT];

static CombatList* BtlCollision_FindList(BtlCollisionLayer layer) {
    switch (layer) {
        case BTLCOLLISION_LAYER_PLAYER:
            return &BtlCollision_LayerLists[BTLCOLLISION_LIST_PLAYER];
        case BTLCOLLISION_LAYER_ENEMY:
        case BTLCOLLISION_LAYER_UNKNOWN2:
        case BTLCOLLISION_LAYER_EFFECT:
            return &BtlCollision_LayerLists[BTLCOLLISION_LIST_ENEMY];
        case BTLCOLLISION_LAYER_UNKNOWN4:
        case BTLCOLLISION_LAYER_UNKNOWN5:
            return &BtlCollision_LayerLists[BTLCOLLISION_LIST_BOSS];
        default:
            return &BtlCollision_LayerLists[BTLCOLLISION_LIST_ENEMY];
    }
}

static void BtlCollision_ClearHits(CombatList* list) {
    BtlCollider* collider = CombatList_GetHead(list);

    while (collider != NULL) {
        collider->hitLayers = 0;

        collider = CombatList_GetNext(&collider->node);
    }
}

static void BtlCollision_CollidePair(BtlCollider* a, BtlCollider* b) {
    s32 radii = a->radius + b->radius;
    s32 dx    = a->x - b->x;
    s32 dy    = a->y - b->y;

    if (ABS(dx) < radii && ABS(dy) < radii) {
        s32 distance = FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
        s32 overlap  = radii - distance;

        if (overlap > 0) {
            s32 dz = a->z - b->z;

            if (dz < a->height && -dz < b->height) {
                a->hitLayers |= 1 << b->layer;
                b->hitLayers |= 1 << a->layer;

                s32 angle = FX_Atan2Idx(dy, dx);
                a->pushX = a->pushY = 0;
                BtlMgr_MoveByAngle(&a->pushX, &a->pushY, angle, overlap);
                b->pushX = -a->pushX;
                b->pushY = -a->pushY;
            }
        }
    }
}

void BtlCollision_Add(BtlCollider* collider, BtlCollisionLayer layer, s32 radius, s32 height) {
    collider->radius    = I2F(radius);
    collider->height    = I2F(height);
    collider->layer     = layer;
    collider->hitLayers = 0;
    collider->pushX     = 0;
    collider->pushY     = 0;
    CombatList_Append(BtlCollision_FindList(collider->layer), &collider->node, collider);
}

void BtlCollision_Remove(BtlCollider* collider) {
    CombatList_Remove(BtlCollision_FindList(collider->layer), &collider->node);
}

void BtlCollision_SetDisabled(BtlCollider* collider, BOOL disabled) {
    if (disabled) {
        collider->flags |= BTLCOLLIDER_FLAG_DISABLED;
    } else {
        collider->flags &= ~BTLCOLLIDER_FLAG_DISABLED;
    }
}

void BtlCollision_SetPosition(BtlCollider* collider, s32 x, s32 y, s32 z) {
    collider->x = x;
    collider->y = y << 1;
    collider->z = z;
}

void BtlCollision_ApplyPush(BtlCollider* collider, s32* x, s32* y, s32* z) {
    if ((collider->flags & BTLCOLLIDER_FLAG_DISABLED) == 0) {
        *x += collider->pushX >> 1;
        *y += collider->pushY >> 1;
    }
}

CombatList* BtlCollision_GetLayerList(BtlCollisionLayer layer) {
    return BtlCollision_FindList(layer);
}

void BtlCollision_CollideAll(CombatList* listA, CombatList* listB) {
    BtlCollider* a = CombatList_GetHead(listA);

    while (a != NULL) {
        BtlCollider* b = CombatList_GetTail(listB);

        while (b != NULL) {
            if (a == b) {
                break;
            }
            BtlCollision_CollidePair(a, b);
            b = CombatList_GetPrev(&b->node);
        }
        a = CombatList_GetNext(&a->node);
    }
}

static void BtlCollision_CollideEnabled(CombatList* listA, CombatList* listB) {
    BtlCollider* a = CombatList_GetHead(listA);

    while (a != NULL) {
        if (a->flags & BTLCOLLIDER_FLAG_DISABLED) {
            a = CombatList_GetNext(&a->node);
        } else {
            BtlCollider* b = CombatList_GetTail(listB);

            while (b != NULL) {
                if (a == b) {
                    break;
                }
                if (b->flags & BTLCOLLIDER_FLAG_DISABLED) {
                    b = CombatList_GetPrev(&b->node);
                } else {
                    BtlCollision_CollidePair(a, b);
                    b = CombatList_GetPrev(&b->node);
                }
            }
            a = CombatList_GetNext(&a->node);
        }
    }
}

void BtlCollision_Init(void) {
    for (s32 i = 0; i < BTLCOLLISION_LIST_COUNT; i++) {
        CombatList_Init(&BtlCollision_LayerLists[i]);
    }
}

void BtlCollision_Update(void) {
    for (s32 i = 0; i < BTLCOLLISION_LIST_COUNT; i++) {
        BtlCollision_ClearHits(&BtlCollision_LayerLists[i]);
    }

    BtlCollision_CollideEnabled(&BtlCollision_LayerLists[BTLCOLLISION_LIST_PLAYER],
                                &BtlCollision_LayerLists[BTLCOLLISION_LIST_PLAYER]);

    BtlCollision_CollideEnabled(&BtlCollision_LayerLists[BTLCOLLISION_LIST_PLAYER],
                                &BtlCollision_LayerLists[BTLCOLLISION_LIST_ENEMY]);

    BtlCollision_CollideAll(&BtlCollision_LayerLists[BTLCOLLISION_LIST_PLAYER],
                            &BtlCollision_LayerLists[BTLCOLLISION_LIST_BOSS]);

    BtlCollision_CollideEnabled(&BtlCollision_LayerLists[BTLCOLLISION_LIST_ENEMY],
                                &BtlCollision_LayerLists[BTLCOLLISION_LIST_ENEMY]);
}
