#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include "SpriteMgr.h"
#include "main_02026528.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The damage-rate popup: one sprite per rate digit, the tens digit only being shown when the
// rate has a non-zero tens digit.

typedef struct BtlPop_Rate {
    /* 0x00 */ CombatSprite sprite;     // hundreds digit
    /* 0x60 */ CombatSprite tensSprite; // tens digit
    /* 0xC0 */ s32          unkC0;      // x offset (half-pixels) applied to the tens digit
    /* 0xC4 */ BOOL         isTensVisible;
    /* 0xC8 */ s32          x;          // world position the popup tracks
    /* 0xCC */ s32          y;
    /* 0xD0 */ s32          z;          // display Z, eased towards `targetZ`
    /* 0xD4 */ s32          targetZ;
    /* 0xD8 */ s16          timer;      // Z-slide timer; the popup ends below -30
    /* 0xDA */ s16          rate;       // rate displayed by this popup
    /* 0xDC */ BOOL         isActive;
} BtlPop_Rate;                          // Size: 0xE0

typedef struct BtlPop_RateArgs {
    /* 0x00 */ s32 engine; // screen selector, also the sprite loader flag
    /* 0x04 */ s32 x;
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
    /* 0x10 */ s16 rate;
} BtlPop_RateArgs; // Size: 0x14

static s32 BtlPop_Rate_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlPop_Rate = {"Tsk_BtlPop_Rate", BtlPop_Rate_RunTask, sizeof(BtlPop_Rate)};

/// Animation table of the rate digits: every entry shares the same char, palette and
/// frame indices and only varies the animation index.
static const SpriteAnimEntry data_ov003_020d8050[14] = {
    {10, 12, 11,  0},
    {10, 12, 11,  1},
    {10, 12, 11,  2},
    {10, 12, 11,  3},
    {10, 12, 11,  4},
    {10, 12, 11,  5},
    {10, 12, 11,  6},
    {10, 12, 11,  7},
    {10, 12, 11,  8},
    {10, 12, 11,  9},
    {10, 12, 11, 10},
    {10, 12, 11, 11},
    {10, 12, 11, 12},
    {10, 12, 11, 13},
};

extern BinIdentifier data_ov003_020d7800;

/// Splits a rate into its hundreds and tens digit.
// Nonmatching: regalloc
static void func_ov003_0209c1a4(s32* digits, s32 value) {
    digits[1] = value / 100;
    digits[0] = value % 100 / 10;
}

/// Selects the hundreds/tens digit animations and stores the horizontal offset between
/// the two digit sprites.
static void func_ov003_0209c1f0(BtlPop_Rate* rate, s32 value) {
    s32 digits[2];

    func_ov003_0209c1a4(digits, value);
    rate->unkC0         = digits[1] - 2;
    rate->isTensVisible = digits[0] != 0;
    CombatSprite_SetAnimFromTable(&rate->sprite, (s16)rate->unkC0, 0);
    CombatSprite_SetAnimFromTable(&rate->tensSprite, (s16)(digits[0] + 4), 0);
}

// Nonmatching: argument scheduling
static s32 BtlPop_Rate_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_Rate*     rate   = task->data;
    BtlPop_RateArgs* params = args;
    s32              animIndex;
    s32              z;

    MI_CpuSet(rate, 0, sizeof(BtlPop_Rate));
    rate->rate          = params->rate;
    rate->x             = params->x;
    rate->y             = params->y;
    z                   = params->z;
    rate->z             = z;
    rate->targetZ       = z;
    rate->unkC0         = 2;
    rate->isTensVisible = FALSE;
    rate->timer         = 20;
    rate->isActive      = TRUE;

    if (params->engine == 0) {
        animIndex = 0x3D;
    } else {
        animIndex = 0x3F;
    }

    CombatSprite_LoadFromTableNoProto(params->engine, &rate->sprite, &data_ov003_020d7800, data_ov003_020d8050, 0, animIndex,
                                      0x24);
    func_ov003_02082738(&rate->sprite, 0);
    func_ov003_02082730(&rate->sprite, 1);
    CombatSprite_LoadFromTableNoProto(params->engine, &rate->tensSprite, &data_ov003_020d7800, data_ov003_020d8050, 4,
                                      animIndex, 4);
    func_ov003_02082738(&rate->tensSprite, 0);
    func_ov003_02082730(&rate->tensSprite, 1);
    func_ov003_0209c1f0(rate, params->rate);
    return 1;
}

static s32 BtlPop_Rate_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_Rate* rate = task->data;

    if (rate->timer > 0) {
        func_020265d4(&rate->z, rate->targetZ - 0x16000, (u16)rate->timer);
    }
    rate->timer--;
    if (rate->timer < -30) {
        rate->isActive = FALSE;
    }
    CombatSprite_Update(&rate->sprite);
    CombatSprite_Update(&rate->tensSprite);
    return rate->isActive;
}

static s32 BtlPop_Rate_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_Rate* rate = task->data;
    s32          posX;
    s32          posY;

    if (rate->sprite.sprite.bits_0_1 == 0) {
        posX = func_ov003_020843b0(0, rate->x);
        posY = func_ov003_020843ec(0, rate->y, rate->z);
    } else {
        posX = func_ov003_020843b0(1, rate->x);
        posY = func_ov003_020843ec(1, rate->y, rate->z);
    }
    if (posX < 24) {
        posX = 24;
    }
    if (posX > 232) {
        posX = 232;
    }
    CombatSprite_SetPosition(&rate->sprite, posX, posY);
    CombatSprite_Render(&rate->sprite);
    if (rate->isTensVisible) {
        CombatSprite_SetPosition(&rate->tensSprite, (s16)(posX + rate->unkC0 * 2), posY);
        CombatSprite_Render(&rate->tensSprite);
    }
    return 1;
}

static s32 BtlPop_Rate_Release(TaskPool* pool, Task* task, void* args) {
    BtlPop_Rate* rate = task->data;

    if (rate->sprite.sprite.bits_0_1 == 0) {
        if (rate->rate == (s16)data_ov003_020e71b8->unk3D900) {
            data_ov003_020e71b8->unk3D900 = 0;
        }
    } else {
        if (rate->rate == (s16)data_ov003_020e71b8->unk3D902) {
            data_ov003_020e71b8->unk3D902 = 0;
        }
    }
    CombatSprite_Release(&rate->sprite);
    CombatSprite_Release(&rate->tensSprite);
    return 1;
}

static s32 BtlPop_Rate_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_Rate_Init,
        .update     = BtlPop_Rate_Update,
        .render     = BtlPop_Rate_Render,
        .cleanup    = BtlPop_Rate_Release,
    };
    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209c568(s32 engine) {
    BtlPop_RateArgs params;
    s16             rate          = data_ov003_020e71b8->unk3D8EE;
    BOOL            isRateChanged = FALSE;

    if (rate >= 200) {
        if (engine == 0) {
            if ((s16)data_ov003_020e71b8->unk3D900 != rate) {
                isRateChanged                 = TRUE;
                params.x                      = ((CombatActor*)data_ov003_020e71b8->unk3D898)->position.x;
                params.y                      = ((CombatActor*)data_ov003_020e71b8->unk3D898)->position.y;
                params.z                      = ((CombatActor*)data_ov003_020e71b8->unk3D898)->position.z - 0x20000;
                data_ov003_020e71b8->unk3D900 = data_ov003_020e71b8->unk3D8EE;
            }
        } else if ((s16)data_ov003_020e71b8->unk3D902 != rate) {
            isRateChanged                 = TRUE;
            params.x                      = ((CombatActor*)data_ov003_020e71b8->unk3D89C)->position.x;
            params.y                      = ((CombatActor*)data_ov003_020e71b8->unk3D89C)->position.y;
            params.z                      = ((CombatActor*)data_ov003_020e71b8->unk3D89C)->position.z - 0x20000;
            data_ov003_020e71b8->unk3D902 = data_ov003_020e71b8->unk3D8EE;
        }

        if (isRateChanged) {
            params.engine = engine;
            params.rate   = data_ov003_020e71b8->unk3D8EE;
            EasyTask_CreateTask(&data_ov003_020e71b8->unk_10118, &Tsk_BtlPop_Rate, NULL, 0, NULL, &params);
        }
    }
}
