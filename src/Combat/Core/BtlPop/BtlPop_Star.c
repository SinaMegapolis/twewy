#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include "main_02026528.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The star/score popup: a short-lived task that shows a number (1..99) as star-digit sprites over
// the unit it belongs to, rising slightly while it fades out.

typedef struct BtlPop_Star {
    /* 0x00 */ CombatSprite sprite;
    /* 0x60 */ u16          digits[2];  // digit sprite indices, least significant first
    /* 0x64 */ u16          starAnim;   // star sprite drawn over the last digit
    /* 0x66 */ s16          digitCount; // 1 or 2
    /* 0x68 */ s16          x;
    /* 0x6A */ s16          y;
    /* 0x6C */ s32          riseOffset; // subpixel rise, eased towards -0xC000
    /* 0x70 */ u16          pad_70;
    /* 0x72 */ s16          timer;
    /* 0x74 */ BOOL         isActive;
} BtlPop_Star; // Size: 0x78

typedef struct BtlPop_StarArgs {
    /* 0x00 */ s16 value; // amount shown
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 y;
} BtlPop_StarArgs; // Size: 0x6

static s32 BtlPop_Star_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlPop_Star = {"Tsk_BtlPop_Star", BtlPop_Star_RunTask, sizeof(BtlPop_Star)};

extern const BinIdentifier data_ov003_020d7800;

/// Splits `value` (clamped to 0..99) into decimal digits, least significant first, and returns
/// how many digits are needed. A value of -1 stores a lone `-` glyph.
static s32 func_ov003_0209cd0c(s32* digits, s32 value) {

    if (value == -1) {
        digits[0] = 30;
        return 1;
    }
    if (value > 99) {
        value = 99;
    }
    digits[1] = value / 10;
    digits[0] = value % 10;
    if (value >= 10) {
        return 2;
    }
    return 1;
}

/// Loads the popup sprite on the star-digit animation set.
static void func_ov003_0209cd74(CombatSprite* sprite, s32 variant) {
    CombatSprite_LoadDirectNoProto(1, sprite, &data_ov003_020d7800, 0x25, 0x27, 0x26, 0x3F, variant);
    func_ov003_02082738(sprite, 0);
    func_ov003_02082730(sprite, 1);
}

static s32 BtlPop_Star_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_Star*     star   = task->data;
    BtlPop_StarArgs* params = args;
    s32              digits[2];

    MI_CpuSet(star, 0, sizeof(BtlPop_Star));
    star->digitCount = func_ov003_0209cd0c(digits, params->value);
    star->x          = params->x;
    star->y          = params->y;
    star->riseOffset = 0;
    star->pad_70     = 0;
    star->timer      = 30;
    star->isActive   = TRUE;
    func_ov003_0209cd74(&star->sprite, 0);
    star->starAnim = 49;
    for (s32 i = 0; i < star->digitCount; i++) {
        star->digits[i] = digits[i] + 20;
    }
    return 1;
}

static s32 BtlPop_Star_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_Star* star = task->data;

    if (g_CombatGlobal->unk3D878 & 6) {
        return 0;
    }
    if (star->timer > 0) {
        func_020265d4(&star->riseOffset, -0xC000, (u16)star->timer);
        star->timer--;
    }
    if (star->timer <= 0) {
        star->isActive = FALSE;
    }
    return star->isActive;
}

// Nonmatching: regalloc
static s32 BtlPop_Star_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_Star* star = task->data;
    s16          posY;
    s32          posX;
    s32          half;
    s32          i;
    s32          offset;

    posY = (s16)(star->y + (star->riseOffset >> 12));
    posX = star->x;
    half = (star->digitCount * 7) / 2 - 7;

    for (i = 0, offset = 0; i < star->digitCount; i++, offset += 7) {
        CombatSprite_SetAnim(&star->sprite, star->digits[i], 0);
        CombatSprite_Update(&star->sprite);
        CombatSprite_SetPosition(&star->sprite, (s16)(half + posX - offset), posY);
        CombatSprite_Render(&star->sprite);
    }
    CombatSprite_SetAnim(&star->sprite, star->starAnim, 0);
    CombatSprite_Update(&star->sprite);
    CombatSprite_SetPosition(&star->sprite, (s16)(half + posX - ((star->digitCount - 1) * 7)), posY);
    CombatSprite_Render(&star->sprite);
    return 1;
}

static s32 BtlPop_Star_Release(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Release(task->data);
    return 1;
}

static s32 BtlPop_Star_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_Star_Init,
        .update     = BtlPop_Star_Update,
        .render     = BtlPop_Star_Render,
        .cleanup    = BtlPop_Star_Release,
    };
    return stages.iter[stage](pool, task, args);
}

s32 func_ov003_0209d038(s32 value, s16 x, s32 y) {
    BtlPop_StarArgs params;

    params.value = value;
    params.x     = x;
    params.y     = y;
    return EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlPop_Star, NULL, 0, NULL, &params);
}
