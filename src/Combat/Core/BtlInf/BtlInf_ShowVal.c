#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/File/DatMgr.h"
#include "Player/Inventory.h"
#include "Player/Inventory/Items.h"

#include "Combat/Core/BtlInf.h"

#include <nitro/mi/cpumem.h>
#include <stdarg.h>

// The value display: draws up to six watched values per screen as columns of digits.

typedef struct BtlInf_ShowValArgs {
    /* 0x00 */ const u16* mainValues[6]; // NULL leaves the row empty
    /* 0x18 */ const u16* subValues[6];
    /* 0x30 */ u16        count;
    /* 0x32 */ u16        reserved;
} BtlInf_ShowValArgs; // Size: 0x34

typedef struct BtlInf_ShowVal {
    /* 0x000 */ BattleSprite       mainSprite;
    /* 0x060 */ BattleSprite       subSprite;
    /* 0x0C0 */ BtlInf_ShowValArgs args;
    /* 0x0F4 */ s32                mainDigits[6][4]; // least significant first
    /* 0x154 */ u16                mainLengths[6];
    /* 0x160 */ s32                subDigits[6][4];
    /* 0x1C0 */ u16                subLengths[6];
    /* 0x1CC */ u16                state;
    /* 0x1CE */ u16                reserved;
    /* 0x1D0 */ void (*update)(struct BtlInf_ShowVal*);
    /* 0x1D4 */ s16  timer;
    /* 0x1D6 */ s16  phase;
    /* 0x1D8 */ BOOL isVisible;
} BtlInf_ShowVal; // Size: 0x1DC

static s32 BtlInf_ShowVal_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlInf_ShowVal = {"Tsk_BtlInf_ShowVal", BtlInf_ShowVal_RunTask, sizeof(BtlInf_ShowVal)};

extern const BinIdentifier Iden_ItemData;

static void func_ov003_020a2a88(BtlInf_ShowVal* showVal, void (*update)(BtlInf_ShowVal*)) {
    showVal->update = update;
    showVal->timer  = 0;
    showVal->phase  = 0;
}

/// Splits `value` into four decimal digits, least significant first, and returns how many digits
/// are needed to show it.
static s32 func_ov003_020a2aa0(s32* digits, s32 value) {
    s32 remainder = value;

    digits[3] = remainder / 1000;
    remainder %= 1000;
    digits[2] = remainder / 100;
    remainder %= 100;
    digits[1] = remainder / 10;
    digits[0] = remainder % 10;

    if (value >= 1000) {
        return 4;
    }
    if (value >= 100) {
        return 3;
    }
    return value >= 10 ? 2 : 1;
}

/// Re-reads every watched value into its digits.
static void func_ov003_020a2b54(BtlInf_ShowVal* showVal) {
    u16 i;

    for (i = 0; i < showVal->args.count; i++) {
        const u16* mainValue = showVal->args.mainValues[i];
        const u16* subValue  = showVal->args.subValues[i];

        if (mainValue != NULL) {
            showVal->mainLengths[i] = func_ov003_020a2aa0(showVal->mainDigits[i], *mainValue);
        }
        if (subValue != NULL) {
            showVal->subLengths[i] = func_ov003_020a2aa0(showVal->subDigits[i], *subValue);
        }
    }
}

static s32 BtlInf_ShowVal_Init(TaskPool* pool, Task* task, void* args) {
    BtlInf_ShowVal* showVal = task->data;

    MI_CpuSet(showVal, 0, sizeof(BtlInf_ShowVal));
    showVal->args       = *(BtlInf_ShowValArgs*)args;
    gBattle->comboCount = 0;
    showVal->isVisible  = TRUE;
    showVal->phase      = 0;
    showVal->timer      = 0;
    showVal->state      = 0;
    func_ov003_020a2a88(showVal, func_ov003_020a2b54);
    BtlSprite_LoadDirect(1, &showVal->mainSprite, &Iden_BtlMenu, 0x25, 0x27, 0x26, 0x3D, 0x14);
    BtlSprite_SetPriority(&showVal->mainSprite, 0);
    BtlSprite_SetSortKey(&showVal->mainSprite, 0);
    BtlSprite_LoadDirect(0, &showVal->subSprite, &Iden_BtlMenu, 0x25, 0x27, 0x26, 0x3D, 0x14);
    BtlSprite_SetPriority(&showVal->subSprite, 0);
    BtlSprite_SetSortKey(&showVal->subSprite, 0);
    return 1;
}

static s32 BtlInf_ShowVal_Update(TaskPool* pool, Task* task, void* args) {
    BtlInf_ShowVal* showVal = task->data;

    if (showVal->update != NULL) {
        showVal->update(showVal);
    }
    return 1;
}

// Nonmatching: loop and regalloc
static s32 BtlInf_ShowVal_Render(TaskPool* pool, Task* task, void* args) {
    BtlInf_ShowVal* showVal = task->data;

    if (!showVal->isVisible) {
        return 1;
    }
    s16 y = 170;
    u16 i;
    for (i = 0; i < showVal->args.count; i++) {
        if (showVal->args.mainValues[i] == NULL) {
            y -= 16;
            continue;
        }
        {
            s16 x = 236;
            u16 digit;
            for (digit = 0; digit < showVal->mainLengths[i]; digit++) {
                BtlSprite_SetAnim(&showVal->mainSprite, (s16)(showVal->mainDigits[i][digit] + 20), 0);
                BtlSprite_SetPosition(&showVal->mainSprite, x, y);
                BtlSprite_Update(&showVal->mainSprite);
                BtlSprite_Render(&showVal->mainSprite);
                x -= 8;
            }
        }
        y -= 16;
    }
    s16 subY = 154;
    u16 subIndex;
    for (subIndex = 0; subIndex < showVal->args.count; subIndex++) {
        if (showVal->args.subValues[subIndex] == NULL) {
            subY -= 16;
            continue;
        }
        {
            s16 x = 236;
            u16 digit;
            for (digit = 0; digit < showVal->subLengths[subIndex]; digit++) {
                BtlSprite_SetAnim(&showVal->subSprite, (s16)(showVal->subDigits[subIndex][digit] + 20), 0);
                BtlSprite_SetPosition(&showVal->subSprite, x, subY);
                BtlSprite_Update(&showVal->subSprite);
                BtlSprite_Render(&showVal->subSprite);
                x -= 8;
            }
        }
        subY -= 16;
    }
    return 1;
}

static s32 BtlInf_ShowVal_Release(TaskPool* pool, Task* task, void* args) {
    BtlInf_ShowVal* showVal = task->data;

    BtlSprite_Release(&showVal->mainSprite);
    BtlSprite_Release(&showVal->subSprite);
    return 1;
}

static s32 BtlInf_ShowVal_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlInf_ShowVal_Init,
        .update     = BtlInf_ShowVal_Update,
        .render     = BtlInf_ShowVal_Render,
        .cleanup    = BtlInf_ShowVal_Release,
    };
    return stages.iter[stage](pool, task, args);
}

s32 func_ov003_020a2f7c(TaskPool* pool, ...) {
    BtlInf_ShowValArgs args;
    const u16*         mainValue;
    const u16*         subValue;
    u16                count;
    va_list            values;

    va_start(values, pool);
    mainValue = va_arg(values, const u16*);
    subValue  = va_arg(values, const u16*);
    count     = 0;
    while (mainValue != NULL || subValue != NULL) {
        args.mainValues[count] = mainValue;
        args.subValues[count]  = subValue;
        count++;
        mainValue = va_arg(values, const u16*);
        subValue  = va_arg(values, const u16*);
    }
    va_end(values);
    args.count = count;
    return EasyTask_CreateTask(pool, &Tsk_BtlInf_ShowVal, NULL, 0, NULL, &args);
}

void func_ov003_020a300c(void) {
    Ov003TouchEvent* events = gBattle->touchEvents;
    s32              i;

    for (i = 0; i < 14; i++) {
        events[i].unk_00 = 0xFF;
    }
}

void func_ov003_020a303c(const u16* items, s32 touchIndex) {
    RawItemData item;
    s32         i;

    for (i = 0; i < 4; i++) {
        if (func_02023b7c(items[i])) {
            Data_LoadToBuffer(1, item, &Iden_ItemData, Inventory_GetCategorizedIndex(items[i]));
            if (item.unk_10 != 0xFF) {
                CombatAbility_Queue(item.unk_10, touchIndex, -1);
            }
        }
    }
}
