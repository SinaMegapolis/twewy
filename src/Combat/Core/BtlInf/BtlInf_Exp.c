#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Save.h"

#include "Combat/Core/BtlInf.h"

// The exp panel: a label and up to five digits showing the exp needed for the next level,
// shown for three seconds whenever that value changes.

typedef struct BtlInf_Exp {
    /* 0x000 */ BattleSprite label;
    /* 0x060 */ BattleSprite digits[5]; // least significant first
    /* 0x240 */ s32          value;     // exp to the next level currently displayed
    /* 0x244 */ s16          digitCount;
    /* 0x246 */ s16          timer;     // frames left to show the panel
} BtlInf_Exp;                           // Size: 0x248

static s32 BtlInf_Exp_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlInf_Exp = {"Tsk_BtlInf_Exp", BtlInf_Exp_RunTask, sizeof(BtlInf_Exp)};

/// Splits `value` (clamped to 0..99999) into decimal digits, least significant first, and returns
/// how many digits are needed to show it.
static s32 func_ov003_0209d8a0(s32* digits, s32 value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 99999) {
        value = 99999;
    }

    s32 remainder = value % 10000;
    digits[4]     = value / 10000;
    digits[3]     = remainder / 1000;
    remainder %= 1000;
    digits[2] = remainder / 100;
    remainder %= 100;
    digits[1] = remainder / 10;
    digits[0] = remainder % 10;

    if (value >= 10000) {
        return 5;
    }
    if (value >= 1000) {
        return 4;
    }
    if (value >= 100) {
        return 3;
    }
    return value >= 10 ? 2 : 1;
}

/// Latches the current exp to the next level and points the digit sprites at its digits.
static void func_ov003_0209d9a8(BtlInf_Exp* exp) {
    s32 digits[5];
    s32 i;

    exp->value      = gSaveData.battleExpToNextLevel;
    exp->digitCount = func_ov003_0209d8a0(digits, exp->value);
    for (i = 0; i < exp->digitCount; i++) {
        BtlSprite_SetAnim(&exp->digits[i], (s16)digits[i], 0);
    }
}

static s32 BtlInf_Exp_Init(TaskPool* pool, Task* task, void* args) {
    BtlInf_Exp* exp = task->data;
    s32         i;

    BtlSprite_LoadDirect(1, &exp->label, &Iden_BtlMenu, 7, 9, 8, 0x3F, 0);
    BtlSprite_SetPosition(&exp->label, 6, 15);
    BtlSprite_SetPriority(&exp->label, 0);
    for (i = 0; i < 5; i++) {
        BtlSprite_LoadDirect(1, &exp->digits[i], &Iden_BtlMenu, 0x25, 0x27, 0x26, 0x3F, 0);
        BtlSprite_SetPriority(&exp->digits[i], 0);
    }
    func_ov003_0209d9a8(exp);
    exp->timer = 0;
    return 1;
}

static s32 BtlInf_Exp_Update(TaskPool* pool, Task* task, void* args) {
    BtlInf_Exp* exp = task->data;
    s32         i;

    if (gSaveData.battleMaxLevel < 100 && exp->value != gSaveData.battleExpToNextLevel) {
        exp->timer = 180;
        func_ov003_0209d9a8(exp);
    }
    if (exp->timer > 0) {
        exp->timer--;
    }
    BtlSprite_Update(&exp->label);
    for (i = 0; i < exp->digitCount; i++) {
        BtlSprite_Update(&exp->digits[i]);
    }
    return 1;
}

static s32 BtlInf_Exp_Render(TaskPool* pool, Task* task, void* args) {
    BtlInf_Exp* exp = task->data;
    s32         i;
    s16         x;

    if (exp->timer <= 0) {
        return 1;
    }
    BtlSprite_Render(&exp->label);
    x = 6 + (exp->digitCount - 1) * 7;
    for (i = 0; i < exp->digitCount; i++) {
        BtlSprite_SetPosition(&exp->digits[i], (s16)(x - i * 7), 15);
        BtlSprite_Render(&exp->digits[i]);
    }
    return 1;
}

static s32 BtlInf_Exp_Release(TaskPool* pool, Task* task, void* args) {
    BtlInf_Exp* exp = task->data;
    s32         i;

    BtlSprite_Release(&exp->label);
    for (i = 0; i < 5; i++) {
        BtlSprite_Release(&exp->digits[i]);
    }
    return 1;
}

static s32 BtlInf_Exp_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlInf_Exp_Init,
        .update     = BtlInf_Exp_Update,
        .render     = BtlInf_Exp_Render,
        .cleanup    = BtlInf_Exp_Release,
    };
    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}
