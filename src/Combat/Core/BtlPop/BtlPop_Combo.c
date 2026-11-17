#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/File/BinMgr.h"
#include "Save.h"
#include "main_02026528.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The combo-count popup: a task that shows the running hit combo next to the main actor, then
// re-shows it with a flash every time the combo grows. The combo itself lives in
// `gBattle->comboCount`; hitting the actor's hit flags feed it through the shared
// `data_ov003_020e71bc` command mailbox.

typedef struct BtlPop_Combo {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ u16          digits[4];     // current combo digits, least significant first
    /* 0x68 */ u16          prevDigits[4]; // digits of the combo shown when the popup was last
                                           // refreshed; they slide in as the "gain" digits
    /* 0x70 */ u16 shownCombo;             // combo the popup was last refreshed with
    /* 0x72 */ u16 combo;                  // current combo count
    /* 0x74 */ s16 digitCount;
    /* 0x76 */ s16 prevDigitCount;
    /* 0x78 */ s32 x;                                      // slide offset of the digits
    /* 0x7C */ s32 y;                                      // unused outside the initialiser
    /* 0x80 */ s32 z;                                      // slide offset of the second digit row
    /* 0x84 */ s32 unk84;
    /* 0x88 */ void (*update)(struct BtlPop_Combo* combo); // per-state-frame callback
    /* 0x8C */ s16  state;
    /* 0x8E */ s16  timer;
    /* 0x90 */ BOOL isVisible;
    /* 0x94 */ u16  flashTimer;
    /* 0x96 */ s16  holdFrames; // combo + the per-psych bonus, in frames
    /* 0x98 */ BOOL isActive;
} BtlPop_Combo;                 // Size: 0x9C

/// The combo command mailbox at `data_ov003_020e71bc`: `func_ov003_0209c150` posts a combo
/// increase into it and the popup task consumes it the next frame.
typedef struct BtlPop_ComboCmd {
    /* 0x0 */ s16 type;  // 1 = add combo, 2 = hide the popup
    /* 0x2 */ s16 value; // combo increase, or the combo the popup should show
} BtlPop_ComboCmd;       // Size: 0x4

static s32 BtlPop_Combo_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlPop_Combo = {"Tsk_BtlPop_Combo", BtlPop_Combo_RunTask, sizeof(BtlPop_Combo)};

static BtlPop_ComboCmd data_ov003_020e71bc;

static void func_ov003_0209ba40(BtlPop_Combo* combo);
static void func_ov003_0209ba94(BtlPop_Combo* combo);

/// Installs the per-state-frame callback and restarts the state machine.
static void func_ov003_0209b760(BtlPop_Combo* combo, void (*update)(BtlPop_Combo* combo)) {
    combo->update = update;
    combo->state  = 0;
    combo->timer  = 0;
}

/// Splits `value` (clamped to 0..9999) into decimal digits, least significant first, and
/// returns how many digits are needed to show it.
static s16 func_ov003_0209b774(s32* digits, s32 value) {
    if (value > 9999) {
        value = 9999;
    }

    digits[3] = value / 1000;
    s32 rest  = value % 1000;
    digits[2] = rest / 100;
    rest      = rest % 100;
    digits[1] = rest / 10;
    digits[0] = rest % 10;

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

/// Re-splits `combo` into the digit row the popup draws.
static void func_ov003_0209b838(BtlPop_Combo* combo, s32 value) {
    s32 digits[4];
    combo->digitCount = func_ov003_0209b774(digits, value);
    for (s32 i = 0; i < combo->digitCount; i++) {
        combo->digits[i] = (u16)digits[i];
    }
}

/// Re-splits the previous combo into the row that slides in behind the current one. A combo of
/// zero clears the row instead.
static void func_ov003_0209b88c(BtlPop_Combo* combo, s32 value) {
    if (value == 0) {
        combo->prevDigitCount = 0;
    } else {
        s32 digits[4];
        combo->prevDigitCount = func_ov003_0209b774(digits, value);
        for (s32 i = 0; i < combo->prevDigitCount; i++) {
            combo->prevDigits[i] = digits[i];
        }
    }
}

/// Restarts the popup on the current combo.
static void func_ov003_0209b8f4(BtlPop_Combo* combo) {
    combo->x          = 0;
    combo->z          = -0x16000;
    combo->isActive   = TRUE;
    combo->shownCombo = 0;
    combo->combo      = gBattle->comboCount;
    func_ov003_0209b760(combo, func_ov003_0209ba94);
}

/// Consumes a combo command from the shared mailbox: a type-1 command banks the combo increase
/// and (re)starts the popup, a type-2 command hides it.
static void func_ov003_0209b93c(BtlPop_Combo* combo, BtlPop_ComboCmd* cmd) {
    switch (cmd->type) {
        case 2: {
            cmd->type       = 0;
            combo->isActive = FALSE;
        } break;

        case 1: {
            cmd->type         = 0;
            combo->holdFrames = cmd->value;
            if (CombatAbility_IsActive(34) != 0) {
                combo->holdFrames += (s16)CombatAbility_GetValue(34);
            }

            if (combo->isActive == FALSE) {
                if (gBattle->comboCount > 1) {
                    func_ov003_0209b8f4(combo);
                } else {
                    combo->timer = 0;
                    func_ov003_0209b760(combo, func_ov003_0209ba40);
                }
            } else if (combo->state == 4) {
                combo->combo = gBattle->comboCount;
                combo->timer = 0;

                if (combo->combo != 1) {
                    combo->state = 2;
                } else {
                    func_ov003_0209b88c(combo, 0);
                    combo->state = 0;
                }
            }
        } break;
    }
}

/// Idle state: waits out the combo hold time, then resets the shared combo counter and hides
/// the popup.
static void func_ov003_0209ba40(BtlPop_Combo* combo) {
    if (combo->timer == 0) {
        combo->timer = combo->holdFrames + 20;
    }
    combo->timer--;
    if (combo->timer <= 0) {
        gBattle->comboCount = 0;
        combo->timer        = 0;
        func_ov003_0209b760(combo, NULL);
    }
}

/// The popup's state machine: slide the digits in, flash on a combo increase, hold, then fade.
static void func_ov003_0209ba94(BtlPop_Combo* combo) {
    switch (combo->state) {
        case 0: {
            if (combo->timer == 0) {
                combo->isVisible  = TRUE;
                combo->flashTimer = 0;
                func_ov003_0209b838(combo, combo->combo);
                combo->timer = 8;
            }
            if (combo->timer > 0) {
                combo->timer--;
                func_020265d4(&combo->x, 0x25000, combo->timer);
            }
            if (combo->timer <= 0) {
                combo->x     = 0x25000;
                combo->state = 1;
            }
        } break;

        case 1: {
            if (combo->timer == 0) {
                combo->timer = 2;
            }
            if (combo->timer > 0) {
                combo->timer--;
                func_020265d4(&combo->x, 0x20000, combo->timer);
            }
            if (combo->timer <= 0) {
                combo->x     = 0x20000;
                combo->state = 4;
            }
        } break;

        case 2: {
            if (combo->timer == 0) {
                combo->isVisible = TRUE;
                func_ov003_0209b838(combo, combo->combo);
                combo->timer = 10;
                combo->x     = 0x1E000;
                combo->z     = 0;
            }
            if (combo->timer > 0) {
                func_020265d4(&combo->z, -0x18000, (u16)combo->timer);
            }
            combo->timer--;
            if (combo->timer <= 0) {
                combo->state = 3;
                combo->timer = 0;
            }
        } break;

        case 3: {
            if (combo->timer == 0) {
                func_ov003_0209b838(combo, combo->combo);
                combo->timer = 2;
                combo->x     = 0x1E000;
            }
            if (combo->timer > 0) {
                func_020265d4(&combo->z, -0x16000, combo->timer);
            }
            combo->timer--;
            if (combo->timer <= 0) {
                combo->state = 4;
                combo->timer = 0;
            }
        } break;

        case 4: {
            if (combo->timer == 0) {
                combo->x          = 0x20000;
                combo->shownCombo = combo->combo;
                func_ov003_0209b88c(combo, combo->shownCombo);
                combo->timer = combo->holdFrames + 20;
            }
            combo->isVisible = (combo->timer > combo->holdFrames + 5);
            if (combo->isVisible == FALSE) {
                combo->flashTimer = 0;
            }
            if ((gBattle->flags & BATTLE_FLAG_LEVEL_UP) == 0) {
                combo->timer--;
                if (combo->timer <= 0) {
                    combo->state = 5;
                }
            }
            if (gBattle->comboCount > combo->combo) {
                combo->state = 2;
                combo->timer = 0;
                combo->combo = gBattle->comboCount;
            }
        } break;

        case 5: {
            if (combo->timer == 0) {
                func_ov003_0209d4b0();
                gBattle->comboCount = 0;
                combo->timer        = 5;
                func_ov003_0209b88c(combo, 0);
            }
            if (combo->timer > 0) {
                func_020265d4(&combo->x, 0, combo->timer);
            }
            combo->timer--;
            if (combo->timer <= 0) {
                if (gBattle->comboCount > 1) {
                    func_ov003_0209b8f4(combo);
                } else {
                    combo->isActive = FALSE;
                    func_ov003_0209b760(combo, NULL);
                }
            }
        } break;
    }
}

// Nonmatching
static s32 BtlPop_Combo_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_Combo* combo = task->data;

    MI_CpuSet(combo, 0, sizeof(BtlPop_Combo));
    gBattle->comboCount = 0;
    combo->x            = 0;
    combo->y            = 0xB4000;
    combo->z            = 0;
    combo->unk84        = 0;
    combo->timer        = 0;
    combo->state        = 0;
    combo->digitCount   = 0;
    combo->isActive     = FALSE;

    BtlSprite_LoadDirect(0, &combo->sprite, &Iden_BtlMenu, 0x25, 0x27, 0x26, 0x3D, 0x36);
    BtlSprite_SetPriority(&combo->sprite, 0);
    BtlSprite_SetSortKey(&combo->sprite, 0);
    return 1;
}

static s32 BtlPop_Combo_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_Combo* combo = task->data;

    func_ov003_0209b93c(combo, &data_ov003_020e71bc);
    if (combo->update != NULL) {
        combo->update(combo);
    }
    return 1;
}

/// Draws the old digit row at the fixed "gain" position and the current row at the popup's
/// screen position, plus the plate sprite behind them.
// Nonmatching: `posX`/`posY`/`animOffset`/`i`/`offset` occupy a rotated register set
// (target r9/r5/r8/r10/r7); the instruction sequence itself is identical.
static s32 BtlPop_Combo_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_Combo* combo = task->data;
    s32           posX;
    s16           posY;
    s32           animOffset;
    s32           offset;

    if (combo->isActive == FALSE) {
        return 1;
    }

    if (combo->prevDigitCount > 0) {
        BtlSprite_SetAnim(&combo->sprite, 0x36, 0);
        BtlSprite_SetPosition(&combo->sprite, 0x20, 0xAA);
        BtlSprite_Update(&combo->sprite);
        BtlSprite_Render(&combo->sprite);
    }
    for (s32 i = 0, offset = 0; i < combo->prevDigitCount; i++, offset += 7) {
        BtlSprite_SetAnim(&combo->sprite, (s16)(combo->prevDigits[i] + 0x14), 0);
        BtlSprite_SetPosition(&combo->sprite, (s16)(0x20 - (offset + 7)), 170);
        BtlSprite_Update(&combo->sprite);
        BtlSprite_Render(&combo->sprite);
    }

    posX = (combo->x * 0x10) >> 0x10;
    posY = (s16)((combo->z >> 0xC) + 0xC0);
    if (combo->isVisible != FALSE) {
        animOffset = (combo->flashTimer & 17) ? 0 : 20;
        combo->flashTimer++;
    } else {
        animOffset = 20;
    }

    if (combo->state != 2 && combo->state != 3 && (combo->state != 4 || combo->timer != 0)) {
        BtlSprite_SetAnim(&combo->sprite, 0x36, 0);
        BtlSprite_SetPosition(&combo->sprite, posX, posY);
        BtlSprite_Update(&combo->sprite);
        BtlSprite_Render(&combo->sprite);
    }
    for (s32 i = 0, offset = 0; i < combo->digitCount; i++, offset += 7) {
        BtlSprite_SetAnim(&combo->sprite, (s16)(animOffset + combo->digits[i]), 0);
        BtlSprite_SetPosition(&combo->sprite, (s16)(posX - (offset + 7)), posY);
        BtlSprite_Update(&combo->sprite);
        BtlSprite_Render(&combo->sprite);
    }
    return 1;
}

static s32 BtlPop_Combo_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlPop_Combo_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_Combo_Init,
        .update     = BtlPop_Combo_Update,
        .render     = BtlPop_Combo_Render,
        .cleanup    = BtlPop_Combo_Release,
    };
    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

/// Posts a combo increase through the mailbox and records the longest combo of the battle.
void func_ov003_0209c150(s16 combo) {
    data_ov003_020e71bc.type  = 1;
    data_ov003_020e71bc.value = combo;
    gBattle->comboCount++;
    if (gSaveData.battleMaxHits < gBattle->comboCount) {
        gSaveData.battleMaxHits = gBattle->comboCount;
    }
}
