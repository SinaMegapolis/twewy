#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/File/BinMgr.h"
#include "Engine/Math/Random.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The damage/heal number popup: a task that floats a signed amount above the actor it hit.
// The amount is split into up to five decimal digits (`func_ov003_0209af30`), each digit is
// drawn with the animation of its decimal value offset by the element's base animation, and
// the popup rises to its apex before falling back and expiring.

typedef struct BtlPop_Num {
    /* 0x00 */ s32          engine;    // 0 = main screen, 1 = sub screen
    /* 0x04 */ BattleSprite sprite;
    /* 0x64 */ s16          digits[5]; // decimal digits, least significant first, already
                                       // offset by the element's animation base
    /* 0x6E */ s16  critAnim;          // animation drawn right of the number on a critical hit
    /* 0x70 */ s16  elementAnim;       // animation drawn left of the number for the element
    /* 0x74 */ s32  x;
    /* 0x78 */ s32  y;
    /* 0x7C */ s32  z;
    /* 0x80 */ s32  zVelocity; // per-frame z step, halved once the apex is reached
    /* 0x84 */ s32  xVelocity;
    /* 0x88 */ s32  yVelocity;
    /* 0x8C */ s8   digitCount;
    /* 0x8D */ u8   flags; // bit 0: crit icon, bit 1: element icon
    /* 0x8E */ s16  timer; // frames spent alive
    /* 0x90 */ BOOL isActive;
} BtlPop_Num;              // Size: 0x94

typedef struct BtlPop_NumArgs {
    /* 0x00 */ s32  engine;
    /* 0x04 */ s32  x;
    /* 0x08 */ s32  y;
    /* 0x0C */ s32  z;
    /* 0x10 */ s32  value;      // amount shown (a negative amount is a hit)
    /* 0x14 */ u8   element;    // element animation base, 0..3
    /* 0x15 */ u8   subType;    // non-zero: draw the element icon
    /* 0x18 */ BOOL isCritical; // draw the critical icon
} BtlPop_NumArgs;               // Size: 0x1C

static s32 BtlPop_Num_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlPop_Num = {"Tsk_BtlPop_Num", BtlPop_Num_RunTask, sizeof(BtlPop_Num)};

extern const BinIdentifier data_ov003_020d7800;

/// Splits `value` (clamped to 0..99999) into decimal digits, least significant first, and
/// returns how many digits are needed to show it. A value of -1 stores a lone `-` glyph.
static s32 func_ov003_0209af30(s32* digits, s32 value) {
    if (value == -1) {
        digits[0] = 30;
        return 1;
    }
    if (value > 99999) {
        value = 99999;
    }

    s32 rest  = value % 10000;
    digits[4] = value / 10000;
    digits[3] = rest / 1000;
    rest      = rest % 1000;
    digits[2] = rest / 100;
    rest      = rest % 100;
    digits[1] = rest / 10;
    digits[0] = rest % 10;

    if (value >= 10000) {
        return 5;
    }
    if (value >= 1000) {
        return 4;
    }
    if (value >= 100) {
        return 3;
    }
    if (value >= 10) {
        return 2;
    }
    return 1;
}

/// Loads the popup sprite and starts it on the digit animation set of `engine`.
static void func_ov003_0209b04c(s32 engine, BattleSprite* sprite, s32 arg2, s32 variant) {
    BtlSprite_LoadDirectNoProto(engine, sprite, &data_ov003_020d7800, 0x25, 0x27, 0x26, variant, arg2);
    BtlSprite_SetPriority(sprite, 0);
    BtlSprite_SetSortKey(sprite, 1);
}

static s32 BtlPop_Num_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_Num*     num    = task->data;
    BtlPop_NumArgs* params = args;
    s32             digits[5];
    s32             offset = 0;
    s32             animIndex;

    MI_CpuSet(num, 0, sizeof(BtlPop_Num));
    num->isActive   = TRUE;
    num->flags      = 0;
    num->digitCount = func_ov003_0209af30(digits, params->value);
    num->engine     = params->engine;
    num->zVelocity  = RNG_Next(0x4CE) - 0x2333;
    num->x          = params->x;
    num->y          = params->y;
    num->z          = params->z;
    num->xVelocity  = RNG_Next(0x199B) - 0xCCD;
    num->yVelocity  = RNG_Next(0xCCD) - 0x666;

    switch (params->element) {
        case 0:
            offset = 0;
            break;
        case 1:
            offset = 10;
            break;
        case 2:
            offset = 20;
            break;
        case 3:
            offset = 30;
            break;
    }

    if (num->engine != 1) {
        animIndex = 0x3D;
        g_CombatGlobal->popupCountMain++;
    } else {
        animIndex = 0x3F;
        g_CombatGlobal->popupCountSub++;
    }
    func_ov003_0209b04c(num->engine, &num->sprite, 0, animIndex);

    for (s32 i = 0; i < num->digitCount; i++) {
        num->digits[i] = offset + digits[i];
    }

    if (params->element != 3) {
        if (params->isCritical != FALSE) {
            num->flags |= 1;
            num->critAnim = (params->element * 3) + 40;
        }
        if (params->subType != 0) {
            num->flags |= 2;
            num->elementAnim = params->subType + ((params->element * 3) + 40);
        }
    }
    return 1;
}

static s32 BtlPop_Num_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_Num* num = task->data;

    num->zVelocity += 0x4CD;
    num->z += num->zVelocity;
    if (num->z >= 0) {
        num->z         = 0;
        num->zVelocity = -num->zVelocity / 2;
    } else {
        num->x += num->xVelocity;
        num->y += num->yVelocity;
    }
    if (num->timer >= 42) {
        num->isActive = FALSE;
    } else {
        num->timer++;
    }
    return num->isActive;
}

// Nonmatching: regalloc
static s32 BtlPop_Num_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_Num* num = task->data;
    s32         posX;
    s32         screenY;
    s16         posY;
    s32         offset;
    s32         i;
    s32         half;

    if (num->engine != 1) {
        posX    = CombatStage_ToScreenX(0, num->x);
        screenY = CombatStage_ToScreenY(0, num->y, num->z);
    } else {
        posX    = CombatStage_ToScreenX(1, num->x);
        screenY = CombatStage_ToScreenY(1, num->y, num->z);
    }
    posY = screenY - 8;
    half = (num->digitCount * 7) / 2 - 7;

    for (i = 0, offset = 0; i < num->digitCount; i++, offset += 7) {
        BtlSprite_SetAnim(&num->sprite, num->digits[i], 0);
        BtlSprite_Update(&num->sprite);
        BtlSprite_SetPosition(&num->sprite, (s16)(half + posX - offset), posY);
        BtlSprite_Render(&num->sprite);
    }
    if (num->flags & 2) {
        BtlSprite_SetAnim(&num->sprite, num->elementAnim, 0);
        BtlSprite_Update(&num->sprite);
        BtlSprite_SetPosition(&num->sprite, (s16)(half + posX - ((num->digitCount - 1) * 7)), posY);
        BtlSprite_Render(&num->sprite);
    }
    if (num->flags & 1) {
        BtlSprite_SetAnim(&num->sprite, num->critAnim, 0);
        BtlSprite_Update(&num->sprite);
        BtlSprite_SetPosition(&num->sprite, (s16)(half + posX + 7), posY);
        BtlSprite_Render(&num->sprite);
    }
    return 1;
}

static s32 BtlPop_Num_Release(TaskPool* pool, Task* task, void* args) {
    BtlPop_Num* num = task->data;

    BtlSprite_Release(&num->sprite);
    if (num->engine != 1) {
        g_CombatGlobal->popupCountMain--;
    } else {
        g_CombatGlobal->popupCountSub--;
    }
    return 1;
}

static s32 BtlPop_Num_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_Num_Init,
        .update     = BtlPop_Num_Update,
        .render     = BtlPop_Num_Render,
        .cleanup    = BtlPop_Num_Release,
    };
    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209b564(s32 engine, s32 value, BattleActor* actor, s32 element) {
    BtlPop_NumArgs params;
    s32            popupCount;

    if (engine != 1) {
        popupCount = g_CombatGlobal->popupCountMain;
    } else {
        popupCount = g_CombatGlobal->popupCountSub;
    }
    switch (popupCount < 10) {
        case FALSE: {
            return;
        } break;
    }

    params.x          = actor->position.x + (actor->centerOffsetX << 12);
    params.y          = actor->position.y;
    params.z          = actor->position.z - (actor->centerHeight << 12);
    params.value      = value;
    params.engine     = engine;
    params.element    = element;
    params.subType    = 0;
    params.isCritical = FALSE;
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlPop_Num, NULL, 0, NULL, &params);
}

void func_ov003_0209b61c(s32 engine, s32 value, BattleActor* actor) {
    BtlPop_NumArgs params;
    s32            popupCount;

    if (engine != 1) {
        popupCount = g_CombatGlobal->popupCountMain;
    } else {
        popupCount = g_CombatGlobal->popupCountSub;
    }

    switch (popupCount < 10) {
        case FALSE:
            return;
    }

    params.x = actor->position.x + (actor->centerOffsetX << 12);
    if (value < 0) {
        value = -value;
    }
    params.y      = actor->position.y;
    params.z      = actor->position.z - (actor->centerHeight << 12);
    params.value  = value;
    params.engine = engine;

    // the popup is coloured by the hit's affinity
    params.subType    = actor->hit.affinity == HIT_AFFINITY_NORMAL ? 0 : (u8)actor->hit.variant;
    params.isCritical = actor->hit.critical == TRUE ? TRUE : FALSE;
    switch (actor->hit.affinity) {
        case HIT_AFFINITY_ABSORB:
            params.element = 1;
            break;
        case HIT_AFFINITY_RESIST:
            params.element = 0;
            break;
        case HIT_AFFINITY_WEAK:
            params.element = 2;
            break;
        default:
            params.element = 0;
            break;
    }
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlPop_Num, NULL, 0, NULL, &params);
}
