#include "Combat/Core/CombatCollision.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include <nitro/fx.h>
#include <nitro/math.h>

static CombatList CombatCollision_Lists[3];

/// The collider list of `side`.
static CombatList* CombatCollision_GetList(s32 side) {
    switch (side) {
        case 0:
            return &CombatCollision_Lists[0];
        case 1:
        case 2:
        case 3:
            return &CombatCollision_Lists[1];
        case 4:
        case 5:
            return &CombatCollision_Lists[2];
        default:
            return &CombatCollision_Lists[1];
    }
}

/// Clears the hit mask of every collider in `list`.
static void CombatCollision_ClearHits(CombatList* list) {
    Ov003Collider* collider = CombatList_GetHead(list);

    if (collider == NULL) {
        return;
    }
    do {
        collider->hitMask = 0;
        collider          = CombatList_GetNext(&collider->node);
    } while (collider != NULL);
}

/// Pushes two colliders apart when they overlap.
static void CombatCollision_Resolve(Ov003Collider* a, Ov003Collider* b) {
    s32 radii = a->radius + b->radius;
    s32 dx    = a->x - b->x;
    s32 dy    = a->y - b->y;
    s32 distance;
    s32 overlap;
    s32 dz;
    s32 angle;

    if (ABS(dx) >= radii) {
        return;
    }
    if (ABS(dy) >= radii) {
        return;
    }
    distance = FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
    overlap  = radii - distance;
    if (overlap <= 0) {
        return;
    }
    dz = a->z - b->z;
    if (!(dz < a->height && -dz < b->height)) {
        return;
    }
    a->hitMask |= 1 << b->side;
    b->hitMask |= 1 << a->side;
    angle    = FX_Atan2Idx(dy, dx);
    a->pushY = 0;
    a->pushX = 0;
    BtlMgr_MoveByAngle(&a->pushX, &a->pushY, angle, overlap);
    b->pushX = -a->pushX;
    b->pushY = -a->pushY;
}

void CombatCollision_Add(Ov003Collider* collider, s32 side, s32 radius, s32 height) {
    collider->radius  = radius << 12;
    collider->height  = height << 12;
    collider->side    = side;
    collider->hitMask = 0;
    collider->pushX   = 0;
    collider->pushY   = 0;
    CombatList_Append(CombatCollision_GetList(collider->side), &collider->node, collider);
}

void CombatCollision_Remove(Ov003Collider* collider) {
    CombatList_Remove(CombatCollision_GetList(collider->side), &collider->node);
}

/// Switches a collider on or off.
void CombatCollision_SetDisabled(Ov003Collider* collider, s32 off) {
    if (off != 0) {
        collider->flags |= 1;
    } else {
        collider->flags &= ~1;
    }
}

void CombatCollision_SetPosition(Ov003Collider* collider, s32 x, s32 y, s32 z) {
    collider->x = x;
    collider->y = y << 1;
    collider->z = z;
}

/// Adds half of the collider's push to a position.
void CombatCollision_ApplyPush(Ov003Collider* collider, s32* x, s32* y, s32* z) {
    if (collider->flags & 1) {
        return;
    }
    *x += collider->pushX >> 1;
    *y += collider->pushY >> 1;
}

CombatList* CombatCollision_GetSideList(s32 side) {
    return CombatCollision_GetList(side);
}

/// Collides every collider of `listA` with every other one of `listB`.
void CombatCollision_CollideLists(CombatList* listA, CombatList* listB) {
    Ov003Collider* a = CombatList_GetHead(listA);

    if (a == NULL) {
        return;
    }
    do {
        Ov003Collider* b = CombatList_GetTail(listB);

        if (b != NULL) {
            do {
                if (a == b) {
                    break;
                }
                CombatCollision_Resolve(a, b);
                b = CombatList_GetPrev(&b->node);
            } while (b != NULL);
        }
        a = CombatList_GetNext(&a->node);
    } while (a != NULL);
}

/// The same, skipping colliders that are switched off.
static void CombatCollision_CollideEnabled(CombatList* listA, CombatList* listB) {
    Ov003Collider* a = CombatList_GetHead(listA);

    if (a == NULL) {
        return;
    }
    do {
        if (a->flags & 1) {
            a = CombatList_GetNext(&a->node);
        } else {
            Ov003Collider* b = CombatList_GetTail(listB);

            if (b != NULL) {
                do {
                    if (a == b) {
                        break;
                    }
                    if (b->flags & 1) {
                        b = CombatList_GetPrev(&b->node);
                    } else {
                        CombatCollision_Resolve(a, b);
                        b = CombatList_GetPrev(&b->node);
                    }
                } while (b != NULL);
            }
            a = CombatList_GetNext(&a->node);
        }
    } while (a != NULL);
}

void CombatCollision_Reset(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        CombatList_Init(&CombatCollision_Lists[i]);
    }
}

/// Runs a frame of collision.
void CombatCollision_Update(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        CombatCollision_ClearHits(&CombatCollision_Lists[i]);
    }
    CombatCollision_CollideEnabled(&CombatCollision_Lists[0], &CombatCollision_Lists[0]);
    CombatCollision_CollideEnabled(&CombatCollision_Lists[0], &CombatCollision_Lists[1]);
    CombatCollision_CollideLists(&CombatCollision_Lists[0], &CombatCollision_Lists[2]);
    CombatCollision_CollideEnabled(&CombatCollision_Lists[1], &CombatCollision_Lists[1]);
}
