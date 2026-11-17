#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlPhase.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "CriSndMgr.h"
#include "Engine/IO/Input.h"
#include "Engine/IO/TouchInput.h"
#include "Save.h"
#include "SndMgr.h"

// The pause menu: three sprites (the frame, the "PAUSED" banner and the resuming banner) that
// are shown while the battle is paused with START. The task also pauses and resumes the BGM
// stream through `CriSndMgr_Pause` and swallows the touch input while the menu is up.

typedef struct BtlPause {
    /* 0x000 */ BattleSprite sprites[3];
    /* 0x120 */ s32          resumed;
} BtlPause; // Size: 0x124

static s32 BtlPause_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlPause = {"Tsk_BtlPause", BtlPause_RunTask, sizeof(BtlPause)};

// The two touch reads are one comparison chain over the touch state.
// Nonmatching: the target CSEs the touch base pointer between the x and y loads; mwcc reloads it.
static void func_ov003_020a210c(BtlPause* self) {
    s16  touchX;
    s16  touchY;
    BOOL overButton;

    if (gSaveData.unk_3140 & 1) {
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    touchX = gBattle->touchWork.x;
    if (touchX >= 0x38 && touchX <= 0xC8) {
        touchY = gBattle->touchWork.y;
        if (touchY >= 0x56 && touchY <= 0x6A) {
            overButton = 1;
        } else {
            overButton = 0;
        }
    } else {
        overButton = 0;
    }
    if (overButton == 0) {
        self->resumed = 0;
        return;
    }
    if (self->resumed == 0) {
        BtlSe_Play(SEIDX_BTL_SYSTEM_CURSOR);
        BtlSprite_SetAnim(&self->sprites[2], 2, 1);
        BtlSprite_Restart(&self->sprites[2]);
        self->resumed = 1;
        return;
    }
    BtlSe_Play(SEIDX_BTL_SYSTEM_EXECUTE);
    BtlMgr_SetState(BATTLE_STATE_QUIT);
}

static s32 BtlPause_Init(TaskPool* pool, Task* task, void* args) {
    BtlPause* self = task->data;
    s32       anim;

    self->resumed = 0;
    BtlSprite_LoadDirect(1, &self->sprites[0], &Iden_BtlMenu, 40, 42, 41, 0x3F, 0);
    BtlSprite_SetPosition(&self->sprites[0], 128, 96);
    BtlSprite_SetSortKey(&self->sprites[0], 0);
    BtlSprite_SetPriority(&self->sprites[0], 0);
    // The second banner shares the file and the frame range, only its first animation differs
    // between the "new game" and the resumed battle.
    anim = (gSaveData.unk_3140 & 1) ? 3 : 1;
    BtlSprite_LoadDirect(0, &self->sprites[1], &Iden_BtlMenu, 40, 42, 41, 0x3D, anim);
    BtlSprite_LoadDirect(0, &self->sprites[2], &Iden_BtlMenu, 40, 42, 41, 0x3D, 2);
    BtlSprite_SetAnim(&self->sprites[2], 2, 1);
    BtlSprite_SetPosition(&self->sprites[1], 128, 96);
    BtlSprite_SetPosition(&self->sprites[2], 128, 96);
    BtlSprite_SetSortKey(&self->sprites[1], 0);
    BtlSprite_SetSortKey(&self->sprites[2], 0);
    BtlSprite_SetPriority(&self->sprites[1], 0);
    BtlSprite_SetPriority(&self->sprites[2], 0);
    return 1;
}

static s32 BtlPause_Update(TaskPool* pool, Task* task, void* args) {
    BtlPause* self = task->data;

    if (gBattle->flags & BATTLE_FLAG_NO_PAUSE) {
        return 1;
    }
    if (InputStatus.buttonState.pressedButtons & INPUT_BUTTON_START) {
        s32 paused = gBattle->paused == 0;

        gBattle->paused = paused;
        if (gBattle->paused != 0) {
            self->resumed = 0;
            CriSndMgr_Pause(gBattle->bgmHandle, 1);
        } else {
            BtlBadgeMgr_ClearDrag();
            CriSndMgr_Pause(gBattle->bgmHandle, 0);
        }
        func_02027200(gBattle->paused);
        BtlSe_Play(SEIDX_SE_PAUSE);
    }
    if (gBattle->paused != 0 && gBattle->state == BATTLE_STATE_FIGHT) {
        func_ov003_020a210c(self);
    }
    if (gBattle->paused != 0) {
        BtlSprite_Update(&self->sprites[0]);
        BtlSprite_Update(&self->sprites[1]);
        BtlSprite_Update(&self->sprites[2]);
    }
    return 1;
}

static s32 BtlPause_Render(TaskPool* pool, Task* task, void* args) {
    BtlPause* self = task->data;

    if (gBattle->paused != 0) {
        if (gBattle->partnerId != 3) {
            BtlSprite_Render(&self->sprites[0]);
        }
        BtlSprite_Render(&self->sprites[1]);
        if (self->resumed != 0) {
            BtlSprite_Render(&self->sprites[2]);
        }
    }
    return 1;
}

static s32 BtlPause_Release(TaskPool* pool, Task* task, void* args) {
    BtlPause* self = task->data;

    BtlSprite_Release(&self->sprites[0]);
    BtlSprite_Release(&self->sprites[1]);
    BtlSprite_Release(&self->sprites[2]);
    return 1;
}

static s32 BtlPause_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPause_Init,
        .update     = BtlPause_Update,
        .render     = BtlPause_Render,
        .cleanup    = BtlPause_Release,
    };

    return stages.iter[stage](pool, task, args);
}
