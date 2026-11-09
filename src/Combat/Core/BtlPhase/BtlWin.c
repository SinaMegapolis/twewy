#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlPhase.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "EasyFade.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "main_02026528.h"
#include <nitro/mi/cpumem.h>

// The victory screen (`Tsk_BtlWin`): two banner sprites slide in from the sides of the screen, the partner (or the
// boss, when it has one) calls out a victory line, and after a short wait the banners slide out again, the screen
// fades and the battle ends.

struct BtlWin;

/// One phase of the screen; it swaps itself out through `func_ov003_020a0f08`.
typedef void (*BtlWinState)(struct BtlWin*);

typedef struct BtlWin {
    /* 0x00 */ BattleSprite sprites[2]; // [0] main screen, [1] sub screen
    /* 0xC0 */ s32          x;          // the main screen banner
    /* 0xC4 */ s32          subX;       // the sub screen banner
    /* 0xC8 */ BtlWinState  state;
    /* 0xCC */ s16          timer;
    /* 0xCE */ s16          phase;
    /* 0xD0 */ s32          running;
} BtlWin; // Size: 0xD4

extern const BinIdentifier data_ov003_020d7818;

static s32 BtlWin_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlWin = {"Tsk_BtlWin", BtlWin_RunTask, sizeof(BtlWin)};

static void func_ov003_020a147c(BtlWin* self);
static void func_ov003_020a14c8(BtlWin* self);

static void func_ov003_020a0f08(BtlWin* self, BtlWinState state) {
    self->state = state;
    self->phase = 0;
    self->timer = 0;
}

/// Loads a banner sprite for `engine`.
static void func_ov003_020a0f1c(s32 engine, BattleSprite* sprite) {
    SpriteAnimationEx anim;

    BtlSprite_InitAnim(&anim.anim, engine, &data_ov003_020d7818);
    anim.anim.bits_10_11 = 0;
    anim.anim.unk_18     = 2;
    anim.anim.packIndex  = 2;
    anim.anim.unk_1C     = 1;
    anim.anim.unk_26     = 2;
    anim.anim.unk_28     = 3;
    anim.anim.unk_20     = 4;
    anim.anim.animIndex  = 1;
    anim.animTable       = NULL;
    BtlSprite_Load(sprite, &anim);
    BtlSprite_SetSortKey(sprite, 0);
    BtlSprite_SetPriority(sprite, 0);
}

/// The generic victory line; `weak` picks the line for a hard fought win.
static void func_ov003_020a0fac(s32 weak) {
    u16 line;

    if (weak) {
        CombatSe_Play(SEIDX_NEKU_BTL_WIN04);
        return;
    }
    line = RNG_Next(0xFFFF) % 3;
    switch (line) {
        case 0:
            CombatSe_Play(SEIDX_NEKU_BTL_WIN01);
            break;
        case 1:
            CombatSe_Play(SEIDX_NEKU_BTL_WIN02);
            break;
        case 2:
            CombatSe_Play(SEIDX_NEKU_BTL_WIN03);
            break;
    }
}

/// The partner's own victory line.
static void func_ov003_020a1038(s32 weak) {
    s32 line;

    if (weak) {
        switch (g_CombatGlobal->unk3D88D) {
            case 0:
                CombatSe_Play(SEIDX_SHIKI_BTL_WIN04);
                break;
            case 1:
                CombatSe_Play(SEIDX_YOSHUA_BTL_WIN04);
                break;
            case 2:
                CombatSe_Play(SEIDX_BIITO_BTL_WIN04);
                break;
        }
        return;
    }
    line = RNG_Next(3);
    if (g_CombatGlobal->unk3D88D == 0) {
        switch (line) {
            case 0:
                CombatSe_Play(SEIDX_SHIKI_BTL_WIN01);
                break;
            case 1:
                CombatSe_Play(SEIDX_SHIKI_BTL_WIN02);
                break;
            case 2:
                CombatSe_Play(SEIDX_SHIKI_BTL_WIN03);
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 1) {
        switch (line) {
            case 0:
                CombatSe_Play(SEIDX_YOSHUA_BTL_WIN01);
                break;
            case 1:
                CombatSe_Play(SEIDX_YOSHUA_BTL_WIN02);
                break;
            case 2:
                CombatSe_Play(SEIDX_YOSHUA_BTL_WIN03);
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 2) {
        switch (line) {
            case 0:
                CombatSe_Play(SEIDX_BIITO_BTL_WIN01);
                break;
            case 1:
                CombatSe_Play(SEIDX_BIITO_BTL_WIN02);
                break;
            case 2:
                CombatSe_Play(SEIDX_BIITO_BTL_WIN03);
                break;
        }
    }
}

/// Plays the victory line of the boss of this battle. Returns 0 when the battle has no boss line.
static s32 func_ov003_020a11c8(void) {
    s32 result = 1;

    switch (g_CombatGlobal->unk3D88E) {
        case 13:
            CombatSe_Play(SEIDX_KONISHI_N_BTL_OTHER01);
            break;
        case 33:
            CombatSe_Play(result + 0x500);
            break;
        case 16:
            CombatSe_Play(result + 0x500);
            break;
        case 18:
            CombatSe_Play(SEIDX_MINAMI_N_BTL_OVR01);
            break;
        case 19:
            CombatSe_Play(SEIDX_HIGASHI_N_BTL_OVR02);
            break;
        case 20:
            CombatSe_Play(SEIDX_KITA_N_BTL_OVR01);
            break;
        case 21:
            CombatSe_Play(SEIDX_KITA_N_BTL_OVR01);
            break;
        case 23:
            CombatSe_Play(SEIDX_BIITO_BTL_OVR01);
            break;
        case 24:
            CombatSe_Play(SEIDX_BIITO_BTL_OVR01);
            break;
        case 25:
            CombatSe_Play(SEIDX_BIITO_BTL_OVR01);
            break;
        case 26:
            CombatSe_Play(SEIDX_BIITO_BTL_OVR01);
            break;
        case 27:
            CombatSe_Play(SEIDX_KITA_BTL_OVR01);
            break;
        case 28:
            CombatSe_Play(SEIDX_KARIYA_BTL_OVR01);
            break;
        case 29:
            CombatSe_Play(SEIDX_YASHIRO_BTL_OVR01);
            break;
        case 30:
            if (RNG_Next(2) != 0) {
                CombatSe_Play(SEIDX_KARIYA_BTL_OVR01);
            } else {
                CombatSe_Play(SEIDX_YASHIRO_BTL_OVR01);
            }
            break;
        case 31:
            if (RNG_Next(2) != 0) {
                CombatSe_Play(SEIDX_KARIYA_B_BTL_OVR01);
            } else {
                CombatSe_Play(SEIDX_YASHIRO_B_BTL_OVR01);
            }
            break;
        case 32:
            CombatSe_Play(SEIDX_HANEKOMA_N_EVENT_YOROKOBIS01);
        default:
            result = 0;
            break;
    }
    return result;
}

/// Picks and plays the victory line.
static void func_ov003_020a1374(void) {
    s32 weak = 0;
    s32 other;

    if (func_ov003_020a11c8() == 0) {
        if (g_CombatGlobal->unk3D8D0 < g_CombatGlobal->unk3D8D2 / 2) {
            weak = 1;
        }
        if (g_CombatGlobal->unk3D88D != 0 && g_CombatGlobal->unk3D88D != 1 && g_CombatGlobal->unk3D88D != 2) {
            other = 1;
        } else {
            other = 0;
        }
        if (other || RNG_Next(2) != 0) {
            func_ov003_020a0fac(weak);
        } else {
            func_ov003_020a1038(weak);
        }
    }
}

/// The banners slide in.
static void func_ov003_020a13f8(BtlWin* self) {
    if (self->timer == 0) {
        self->timer = 30;
        CombatSe_PlayPanned(SEIDX_SE_NOISEERASED01, 0x80);
    }
    if (self->timer > 0) {
        func_020265d4(&self->x, 0x80000, (u16)self->timer);
        func_020265d4(&self->subX, 0x80000, self->timer);
        self->timer--;
    }
    if (self->timer <= 0) {
        func_ov003_020a0f08(self, func_ov003_020a147c);
    }
}

/// The victory line plays while the banners wait.
static void func_ov003_020a147c(BtlWin* self) {
    if (self->timer == 0) {
        func_ov003_020a1374();
        self->timer = 100;
    }
    self->timer--;
    if (self->timer <= 0) {
        func_ov003_020a0f08(self, func_ov003_020a14c8);
    }
}

/// The banners slide out and the screen fades.
static void func_ov003_020a14c8(BtlWin* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->timer = 30;
                CombatSe_PlayPanned(SEIDX_SE_NOISEERASED02, 0x80);
            }
            if (self->timer > 0) {
                func_020265d4(&self->x, 0x180000, (u16)self->timer);
                func_020265d4(&self->subX, -0x180000, self->timer);
                self->timer--;
            }
            if (self->timer > 0) {
                return;
            }
            EasyFade_FadeBothDisplays(0, 0x10, 0x800);
            CombatSe_Play(SEIDX_SE_BATTLE_OUT);
            self->phase++;
            self->timer = 0;
            break;
        case 1:
            if (EasyFade_IsFading()) {
                return;
            }
            BtlMgr_SetState(7);
            self->running = 0;
            break;
    }
}

static s32 BtlWin_Init(TaskPool* pool, Task* task, void* args) {
    BtlWin* self = task->data;

    MI_CpuSet(self, 0, sizeof(*self));
    func_ov003_020a0f1c(1, &self->sprites[0]);
    func_ov003_020a0f1c(0, &self->sprites[1]);
    self->x       = -0x180000;
    self->subX    = 0x180000;
    self->running = 1;
    func_ov003_020a0f08(self, func_ov003_020a13f8);
    return 1;
}

static s32 BtlWin_Update(TaskPool* pool, Task* task, void* args) {
    BtlWin* self = task->data;

    if (self->state != NULL) {
        self->state(self);
    }
    BtlSprite_Update(&self->sprites[0]);
    BtlSprite_Update(&self->sprites[1]);
    return self->running;
}

static s32 BtlWin_Render(TaskPool* pool, Task* task, void* args) {
    BtlWin* self = task->data;

    BtlSprite_SetPosition(&self->sprites[1], (self->subX * 0x10) >> 16, 0x60);
    BtlSprite_Render(&self->sprites[1]);
    if (g_CombatGlobal->unk3D88D == 3) {
        return 1;
    }
    BtlSprite_SetPosition(&self->sprites[0], (self->x * 0x10) >> 16, 0x60);
    BtlSprite_Render(&self->sprites[0]);
    return 1;
}

static s32 BtlWin_Release(TaskPool* pool, Task* task, void* args) {
    BtlWin* self = task->data;

    BtlSprite_Release(&self->sprites[0]);
    BtlSprite_Release(&self->sprites[1]);
    return 1;
}

static s32 BtlWin_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlWin_Init,
        .update     = BtlWin_Update,
        .render     = BtlWin_Render,
        .cleanup    = BtlWin_Release,
    };

    return stages.iter[stage](pool, task, args);
}
