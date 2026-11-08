#include "Combat/Core/BtlPhase.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include "EasyFade.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "Save.h"
#include "main_02026528.h"
#include <nitro/mi/cpumem.h>

// The "ready" banner shown when a battle starts (`Tsk_BtlReady`): three pairs of sprites (one per screen) slide in
// from the sides, a line is spoken by the partner, and after a short wait the banners slide out again and the
// battle begins.

struct BtlReady;

/// One phase of the banner; it swaps itself out through `func_ov003_020a060c`.
typedef void (*BtlReadyState)(struct BtlReady*);

typedef struct BtlReady {
    /* 0x000 */ CombatSprite  sprites[6]; // 0/1 background, 2/3 "READY", 4/5 battle number; even entries are main screen
    /* 0x240 */ s32           x;          // the main screen sprites
    /* 0x244 */ s32           subX;       // the sub screen sprites
    /* 0x248 */ s16           y;
    /* 0x24A */ u16           reserved24A;
    /* 0x24C */ BtlReadyState state;
    /* 0x250 */ s16           phase;
    /* 0x252 */ s16           timer;
    /* 0x254 */ s32           speaks; // whether the partner calls out a line
    /* 0x258 */ s32           running;
} BtlReady;                           // Size: 0x25C

extern const BinIdentifier data_ov003_020d7818;

static s32 BtlReady_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlReady = {"Tsk_BtlReady", BtlReady_RunTask, sizeof(BtlReady)};

static void func_ov003_020a0954(BtlReady* self);
static void func_ov003_020a09f4(BtlReady* self);
static void func_ov003_020a0ac0(BtlReady* self);

/// The animations of the banner sprites: the first two are the frame and the "READY" text, the rest the battle numbers.
static const SpriteAnimEntry data_ov003_020d8780[18] = {
    {1, 3, 2,  0},
    {1, 3, 2,  1},
    {1, 3, 2,  2},
    {1, 3, 2,  3},
    {1, 3, 2,  4},
    {1, 3, 2,  5},
    {1, 3, 2,  6},
    {1, 3, 2,  7},
    {1, 3, 2,  8},
    {1, 3, 2,  9},
    {1, 3, 2, 10},
    {1, 3, 2, 11},
    {1, 3, 2, 12},
    {1, 3, 2, 13},
    {1, 3, 2, 14},
    {1, 3, 2, 15},
    {1, 3, 2, 16},
    {1, 3, 2, 17},
};

static void func_ov003_020a060c(BtlReady* self, BtlReadyState state) {
    self->state = state;
    self->timer = 0;
    self->phase = 0;
}

/// Loads sprite `index` of the banner table for `engine`, `x` tiles off to the side.
static void func_ov003_020a0624(s32 engine, CombatSprite* sprite, u16 index, u16 x) {
    SpriteAnimationEx anim;
    u16               charIndex;
    u16               paletteIndex;
    u16               frameIndex;
    s16               animIndex;

    CombatSprite_InitAnim(&anim.anim, engine, &data_ov003_020d7818);
    anim.anim.bits_10_11 = 1;
    charIndex            = data_ov003_020d8780[index].charDataIndex;
    paletteIndex         = data_ov003_020d8780[index].paletteDataIndex;
    frameIndex           = data_ov003_020d8780[index].frameDataIndex;
    animIndex            = data_ov003_020d8780[index].animDataIndex;
    anim.anim.unk_1E     = x << 5;
    anim.anim.unk_18     = 2;
    anim.anim.unk_1C     = charIndex;
    anim.anim.packIndex  = 1;
    anim.anim.unk_26     = frameIndex;
    anim.anim.unk_28     = paletteIndex;
    anim.anim.unk_20     = 4;
    anim.anim.animIndex  = animIndex + 1;
    anim.animTable       = data_ov003_020d8780;
    CombatSprite_Load(sprite, &anim);
    CombatSprite_SetAnimFromTable(sprite, index, 0);
    func_ov003_02082730(sprite, 0);
    func_ov003_02082738(sprite, 0);
}

/// The generic line.
static void func_ov003_020a0708(s32 weak) {
    if (weak) {
        CombatSe_Play(SEIDX_NEKU_BTL_OP_DANGER01);
        return;
    }
    if (RNG_Next(2) != 0) {
        CombatSe_Play(SEIDX_NEKU_BTL_OP01);
    } else {
        CombatSe_Play(SEIDX_NEKU_BTL_OP03);
    }
}

/// The partner's own line.
static void func_ov003_020a0750(s32 weak) {
    s32 line;

    if (weak) {
        switch (g_CombatGlobal->unk3D88D) {
            case 0:
                CombatSe_Play(SEIDX_SHIKI_BTL_OP_DANGER01);
                break;
            case 1:
                CombatSe_Play(SEIDX_YOSHUA_BTL_OP_DANGER01);
                break;
            case 2:
                CombatSe_Play(SEIDX_BIITO_BTL_OP_DANGER01);
                break;
        }
        return;
    }
    line = RNG_Next(2);
    if (g_CombatGlobal->unk3D88D == 0) {
        switch (line) {
            case 0:
                CombatSe_Play(SEIDX_SHIKI_BTL_OP01);
                break;
            case 1:
                CombatSe_Play(SEIDX_SHIKI_BTL_OP03);
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 1) {
        switch (line) {
            case 0:
                CombatSe_Play(SEIDX_YOSHUA_BTL_OP01);
                break;
            case 1:
                CombatSe_Play(SEIDX_YOSHUA_BTL_OP03);
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 2) {
        switch (line) {
            case 0:
                CombatSe_Play(SEIDX_BIITO_BTL_OP01);
                break;
            case 1:
                CombatSe_Play(SEIDX_BIITO_BTL_OP03);
                break;
        }
    }
}

/// Picks and plays the opening line.
static void func_ov003_020a0898(void) {
    s32 weak = 0;
    s32 other;

    if (g_CombatGlobal->unk3D8D0 < g_CombatGlobal->unk3D8D2 / 2) {
        weak = 1;
    }
    if (g_CombatGlobal->unk3D88D != 0 && g_CombatGlobal->unk3D88D != 1 && g_CombatGlobal->unk3D88D != 2) {
        other = 1;
    } else {
        other = 0;
    }
    if (other || RNG_Next(2) != 0) {
        func_ov003_020a0708(weak);
    } else {
        func_ov003_020a0750(weak);
    }
}

/// Waits before the banner appears.
static void func_ov003_020a0910(BtlReady* self) {
    if (self->timer == 0) {
        self->timer = 0x82;
    }
    self->timer--;
    if (self->timer <= 0) {
        func_ov003_020a060c(self, func_ov003_020a0954);
    }
}

/// The banners slide in.
static void func_ov003_020a0954(BtlReady* self) {
    if (self->timer == 0) {
        self->timer = 30;
        if (self->speaks != 0) {
            CombatSe_Play(SEIDX_SE_READY01);
        }
    }
    if (self->timer > 0) {
        func_020265d4(&self->x, 0x80000, (u16)self->timer);
        func_020265d4(&self->subX, 0x80000, self->timer);
        self->timer--;
    }
    if (self->timer <= 0) {
        func_ov003_020a060c(self, func_ov003_020a09f4);
    }
}

/// The line plays while the banners wait, then the screen fades.
static void func_ov003_020a09f4(BtlReady* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->timer = 0x82;
            }
            if (self->speaks != 0 && self->timer == 0x46) {
                func_ov003_020a0898();
            }
            self->timer--;
            if (self->timer > 0) {
                return;
            }
            EasyFade_FadeBothDisplays(2, 0xD, 0x1000);
            self->phase++;
            self->timer = 0;
            break;
        case 1:
            EasyFade_FadeBothDisplays(0, 0, 0x800);
            BtlMgr_SetState(2);
            CombatTouch_Clear();
            func_ov003_020a060c(self, func_ov003_020a0ac0);
            break;
    }
}

/// The banners slide out.
static void func_ov003_020a0ac0(BtlReady* self) {
    if (self->timer == 0) {
        self->timer = 30;
        CombatSe_Play(SEIDX_SE_READY02);
    }
    if (self->timer > 0) {
        func_020265d4(&self->x, 0x180000, (u16)self->timer);
        func_020265d4(&self->subX, -0x80000, self->timer);
        self->timer--;
    }
    if (self->timer <= 0) {
        self->running = 0;
    }
}

static s32 BtlReady_Init(TaskPool* pool, Task* task, void* args) {
    BtlReady* self = task->data;
    u16       number;

    MI_CpuSet(self, 0, sizeof(*self));
    if (gSaveData.unk_3130 != 3) {
        g_CombatGlobal->unk3D878 |= 0x10000;
    }
    number = g_CombatGlobal->unk3D88C + 2;
    func_ov003_020a0624(1, &self->sprites[0], 0, 0x90);
    func_ov003_020a0624(0, &self->sprites[3], 0, 0x90);
    func_ov003_020a0624(1, &self->sprites[1], 1, 0x6C);
    func_ov003_020a0624(0, &self->sprites[4], 1, 0x6C);
    func_ov003_020a0624(1, &self->sprites[2], number, 0x30);
    func_ov003_020a0624(0, &self->sprites[5], number, 0x30);
    self->x    = -0x80000;
    self->subX = 0x180000;
    if (g_CombatGlobal->unk3D878 & 0x80000000) {
        self->y = 0x50;
    } else {
        self->y = 0x60;
    }
    self->speaks  = !(g_CombatGlobal->unk3D878 & 0x100000);
    self->running = 1;
    func_ov003_020a060c(self, func_ov003_020a0910);
    return 1;
}

static s32 BtlReady_Update(TaskPool* pool, Task* task, void* args) {
    BtlReady* self = task->data;

    if (g_CombatGlobal->unk3D878 & 6) {
        return 0;
    }
    if (self->state != NULL) {
        self->state(self);
    }
    CombatSprite_Update(&self->sprites[0]);
    CombatSprite_Update(&self->sprites[3]);
    if (g_CombatGlobal->unk3D878 & 0x80000000) {
        CombatSprite_Update(&self->sprites[1]);
        CombatSprite_Update(&self->sprites[4]);
        CombatSprite_Update(&self->sprites[2]);
        CombatSprite_Update(&self->sprites[5]);
    }
    return self->running;
}

static s32 BtlReady_Render(TaskPool* pool, Task* task, void* args) {
    BtlReady* self = task->data;
    s32       subX;
    s32       x;

    if (self->speaks == 0) {
        return 1;
    }
    subX = (self->subX * 0x10) >> 16;
    x    = (self->x * 0x10) >> 16;
    CombatSprite_SetPosition(&self->sprites[3], subX, self->y);
    CombatSprite_Render(&self->sprites[3]);
    if (g_CombatGlobal->unk3D878 & 0x80000000) {
        CombatSprite_SetPosition(&self->sprites[4], x, self->y);
        CombatSprite_Render(&self->sprites[4]);
        CombatSprite_SetPosition(&self->sprites[5], x, self->y);
        CombatSprite_Render(&self->sprites[5]);
    }
    if (g_CombatGlobal->unk3D88D == 3) {
        return 1;
    }
    CombatSprite_SetPosition(&self->sprites[0], x, self->y);
    CombatSprite_Render(&self->sprites[0]);
    if (g_CombatGlobal->unk3D878 & 0x80000000) {
        CombatSprite_SetPosition(&self->sprites[1], subX, self->y);
        CombatSprite_Render(&self->sprites[1]);
        CombatSprite_SetPosition(&self->sprites[2], subX, self->y);
        CombatSprite_Render(&self->sprites[2]);
    }
    return 1;
}

static s32 BtlReady_Release(TaskPool* pool, Task* task, void* args) {
    BtlReady* self = task->data;

    CombatSprite_Release(&self->sprites[0]);
    CombatSprite_Release(&self->sprites[3]);
    CombatSprite_Release(&self->sprites[1]);
    CombatSprite_Release(&self->sprites[4]);
    CombatSprite_Release(&self->sprites[2]);
    CombatSprite_Release(&self->sprites[5]);
    if (g_CombatGlobal->unk3D874 == 2) {
        g_CombatGlobal->unk3D878 &= ~0x10000;
    }
    return 1;
}

static s32 BtlReady_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlReady_Init,
        .update     = BtlReady_Update,
        .render     = BtlReady_Render,
        .cleanup    = BtlReady_Release,
    };

    return stages.iter[stage](pool, task, args);
}
