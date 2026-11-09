#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlPhase.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "Player/Inventory.h"
#include "Save.h"
#include "SpriteMgr.h"
#include "main_02026528.h"
#include <nitro/fx.h>
#include <nitro/mi/cpumem.h>

// The player (`Tsk_BtlPlayer00`): the character the stylus drags around the main screen in a battle. Every state is a
// function stored in `update`; the states count their animation in `phase`/`timer` and the commands the touch
// recognizer queues (`command`) pick the next one.

struct BtlPlayer00;

typedef void (*BtlPlayer00State)(struct BtlPlayer00* self);

/// One step of a scripted move: at animation tick `frame` the player jumps by (`x`, `z`) pixels.
typedef struct BtlPlayer00Step {
    /* 0x0 */ s16 frame; // -1 ends the table
    /* 0x2 */ s16 x;     // towards the facing, mirrored when flipped
    /* 0x4 */ s16 z;
} BtlPlayer00Step;       // Size: 0x6

typedef struct BtlPlayer00 {
    /* 0x000 */ CombatActor       actor;
    /* 0x07C */ CombatSprite      sprite;
    /* 0x0DC */ BtlCollider       collider;
    /* 0x110 */ CombatSprite      shadow;
    /* 0x170 */ CombatSprite      target; // the marker drawn at the stylus
    /* 0x1D0 */ u16               flags;  // 1: hidden, 2: marker shown, 4: follows the stylus
    /* 0x1D2 */ u8                unk_1D2[2];
    /* 0x1D4 */ s32               targetX;
    /* 0x1D8 */ s32               targetY;
    /* 0x1DC */ BtlPlayer00State  update;
    /* 0x1E0 */ s16               phase;
    /* 0x1E2 */ s16               timer;
    /* 0x1E4 */ u16               comboState;
    /* 0x1E6 */ u8                unk_1E6[2];
    /* 0x1E8 */ Ov003ActionEffect command; // the queued attack the current state plays
    /* 0x204 */ s16               guardTimer;
    /* 0x206 */ s16               hurtTimer;
    /* 0x208 */ s16               gaugeTimer;
    /* 0x20A */ u8                unk_20A[2];
} BtlPlayer00; // Size: 0x20C

extern const BinIdentifier data_ov003_020d77d8;
extern const BinIdentifier data_ov003_020d7800;
extern const s16           data_0205e4e0[];

static s32 BtlPlayer00_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlPlayer00 = {"Tsk_BtlPlayer00", BtlPlayer00_RunTask, sizeof(BtlPlayer00)};

static void func_ov003_02090288(BtlPlayer00* self);
static void func_ov003_02090588(BtlPlayer00* self);
static void func_ov003_02090688(BtlPlayer00* self);
static void func_ov003_020908c4(BtlPlayer00* self);
static void func_ov003_02090a5c(BtlPlayer00* self);
static void func_ov003_02090c18(BtlPlayer00* self);
static void func_ov003_02090ec4(BtlPlayer00* self);
static void func_ov003_02090f50(BtlPlayer00* self);
static void func_ov003_020910ac(BtlPlayer00* self);
static void func_ov003_02091214(BtlPlayer00* self);
static void func_ov003_02091468(BtlPlayer00* self);
static void func_ov003_02091680(BtlPlayer00* self);
static void func_ov003_02091850(BtlPlayer00* self);
static void func_ov003_02091a14(BtlPlayer00* self);
static void func_ov003_02091b7c(BtlPlayer00* self);
static void func_ov003_02091d10(BtlPlayer00* self);
static void func_ov003_02091ea0(BtlPlayer00* self);
static void func_ov003_02091fcc(BtlPlayer00* self);
static void func_ov003_020920c8(BtlPlayer00* self);
static void func_ov003_0209228c(BtlPlayer00* self);

/// The scripted steps of the three attack animations.
static const BtlPlayer00Step data_ov003_020d7960[5] = {
    { 0, -2, 0},
    { 3, -3, 0},
    { 6, -8, 0},
    {12, -3, 0},
    {-1,  0, 0},
};

static const BtlPlayer00Step data_ov003_020d797e[6] = {
    { 0,  0, -3},
    { 3, -9,  0},
    { 6, -8,  1},
    {12,  2,  0},
    {18,  0,  1},
    {-1,  0,  0},
};

static const BtlPlayer00Step data_ov003_020d79a2[7] = {
    { 2, -3, 0},
    { 3, -6, 0},
    { 6, -9, 0},
    {12, -1, 0},
    {19,  2, 0},
    {26,  0, 0},
    {-1,  0, 0},
};

/// The animation table of the player sprite.
static const SpriteAnimEntry data_ov003_020d79cc[48] = {
    { 1,  3,  2,  0},
    { 1,  3,  2,  3},
    { 1,  3,  2,  1},
    { 1,  3,  2,  2},
    { 7,  9,  8,  0},
    { 7,  9,  8,  9},
    { 7,  9,  8, 10},
    { 7,  9,  8,  1},
    { 7,  9,  8,  2},
    { 7,  9,  8,  3},
    { 7,  9,  8,  4},
    { 7,  9,  8,  5},
    { 7,  9,  8,  6},
    { 7,  9,  8,  8},
    { 4,  6,  5,  0},
    {16, 18, 17,  0},
    {16, 18, 17,  1},
    {16, 18, 17,  2},
    {49, 51, 50,  0},
    {49, 51, 50,  1},
    {49, 51, 50,  2},
    {25, 27, 26,  0},
    {28, 30, 29,  0},
    {31, 33, 32,  0},
    {34, 36, 35,  0},
    {34, 36, 35,  4},
    {37, 39, 38,  0},
    {40, 42, 41,  0},
    {43, 45, 44,  0},
    {28, 30, 29,  3},
    {28, 30, 29,  4},
    {28, 30, 29,  5},
    {46, 48, 47,  0},
    {46, 48, 47,  1},
    {46, 48, 47,  2},
    {13, 15, 14,  1},
    {13, 15, 14,  0},
    {19, 21, 20,  1},
    {19, 21, 20,  0},
    {19, 21, 20,  2},
    {22, 24, 23,  1},
    {22, 24, 23,  0},
    {22, 24, 23,  2},
    {52, 54, 53,  0},
    {10, 12, 11,  0},
    {10, 12, 11,  1},
    {10, 12, 11,  2},
    {55, 57, 56,  0},
};

/// Turns the actor towards `x`.
static inline void BtlPlayer00_FaceX(CombatActor* actor, s32 x) {
    if (actor->position.x > x) {
        actor->isFlipped = FALSE;
    }
    if (actor->position.x < x) {
        actor->isFlipped = TRUE;
    }
}

/// The animation tick of the current sprite animation counted from its loop start.
static inline u16 BtlPlayer00_Tick(CombatSprite* sprite) {
    if (SpriteMgr_IsFrameFinished(&sprite->sprite)) {
        return sprite->sprite.currentFrame - sprite->sprite.loopFrame + 1;
    }
    return sprite->sprite.currentFrame - sprite->sprite.loopFrame;
}

static void func_ov003_0208f384(BtlPlayer00* self, BtlPlayer00State state) {
    self->update = state;
    self->phase  = 0;
    self->timer  = 0;
}

/// Plays a scripted move: at the step whose frame is the current timer the player jumps.
static void func_ov003_0208f39c(BtlPlayer00* self, const BtlPlayer00Step* step) {
    s32 timer;

    if (step->frame == -1) {
        return;
    }
    timer = self->timer;
    do {
        if (step->frame == timer) {
            if (self->actor.isFlipped != 0) {
                self->actor.position.x = self->actor.position.x - (step->x << 12);
            } else {
                self->actor.position.x = self->actor.position.x + (step->x << 12);
            }
            self->actor.position.z += step->z << 12;
            self->actor.zVelocity = 0;
            self->targetX         = self->actor.position.x;
            self->targetY         = self->actor.position.y;
            return;
        }
        step++;
    } while (step->frame != -1);
}

/// Footstep sounds of the walk animation.
static void func_ov003_0208f418(CombatSprite* sprite, s32 x) {
    u16 tick;

    if (!(sprite->sprite.bit_11 != 1)) {
        tick = BtlPlayer00_Tick(sprite);
        switch (tick) {
            case 1:
                CombatSe_PlayAtMainX(3, x);
                break;
            case 5:
                CombatSe_PlayAtMainX(4, x);
                break;
        }
    }
}

/// A voice line of the player when hit (`flag` != 0) or when attacking.
static void func_ov003_0208f498(BtlPlayer00* self, s32 flag) {
    if (flag != 0) {
        switch (RNG_Next(2)) {
            case 0:
                CombatSe_PlayAtMainX(0x36D, self->actor.position.x);
                break;
            case 1:
                CombatSe_PlayAtMainX(0x36E, self->actor.position.x);
                break;
        }
        return;
    }
    if (self->hurtTimer <= 0) {
        switch (RNG_Next(3)) {
            case 0:
                CombatSe_PlayAtMainX(0x36A, self->actor.position.x);
                break;
            case 1:
                CombatSe_PlayAtMainX(0x36B, self->actor.position.x);
                break;
            case 2:
                CombatSe_PlayAtMainX(0x36C, self->actor.position.x);
                break;
        }
        self->hurtTimer = 0x2D;
    }
}

/// The partner's voice line.
static void func_ov003_0208f564(BtlPlayer00* self) {
    Ov003Global* global = g_CombatGlobal;

    switch (global->unk3D88D) {
        case 0:
            CombatSe_PlayAtSubX(0x391, ((CombatActor*)global->unk3D89C)->position.x);
            break;
        case 1:
            CombatSe_PlayAtSubX(0x3B9, ((CombatActor*)global->unk3D89C)->position.x);
            break;
        case 2:
            CombatSe_PlayAtSubX(0x3E1, ((CombatActor*)global->unk3D89C)->position.x);
            break;
    }
}

static void func_ov003_0208f5e0(BtlPlayer00* self, s32 flag) {
    if (flag != 0) {
        func_ov003_02087804(1);
        return;
    }
    if (self->guardTimer <= 0) {
        func_ov003_02087804(0);
        self->guardTimer = 0x2D;
    }
}

/// Takes the oldest queued attack as the one to play.
static BOOL func_ov003_0208f624(BtlPlayer00* self) {
    Ov003ActionEffect* effect = func_ov003_02087358();

    if (effect == NULL) {
        return FALSE;
    }
    self->command = *effect;
    func_ov003_020872b4(effect);
    return TRUE;
}

/// Starts the attack the touch recognizer asked for. Returns whether one started.
static BOOL func_ov003_0208f664(BtlPlayer00* self) {
    switch (func_ov003_020a58d4()) {
        case 1:
            if (func_ov003_0208f624(self)) {
                self->command.state->unk_08 = 0;
                func_ov003_0208f384(self, func_ov003_02090c18);
            }
            return TRUE;
        case 12:
            func_ov003_0208f384(self, func_ov003_02090ec4);
            return TRUE;
        case 6:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_020910ac);
            }
            return TRUE;
        case 7:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_02091214);
            }
            return TRUE;
        case 8:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_02091680);
            }
            return TRUE;
        case 9:
            func_ov003_0208f384(self, func_ov003_02091850);
            return TRUE;
        case 10:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_02091a14);
            }
            return TRUE;
        case 11:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_02091b7c);
            }
            return TRUE;
        case 13:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_02091468);
            }
            return TRUE;
        case 2:
            func_ov003_0208f384(self, func_ov003_02091d10);
            return TRUE;
        case 4:
            func_ov003_0208f384(self, func_ov003_02091ea0);
            return TRUE;
        case 5:
            func_ov003_0208f384(self, func_ov003_02091fcc);
            return TRUE;
        case 14:
            func_ov003_0208f384(self, func_ov003_02090a5c);
            return TRUE;
        case 15:
            func_ov003_0208f384(self, func_ov003_020920c8);
            return TRUE;
        case 16:
            func_ov003_0208f384(self, func_ov003_02090588);
            return TRUE;
        case 3:
            if (func_ov003_0208f624(self)) {
                func_ov003_0208f384(self, func_ov003_0209228c);
            }
            return TRUE;
    }
    return FALSE;
}

/// Starts a drag when the stylus travelled far from where it came down.
static BOOL func_ov003_0208f8d4(BtlPlayer00* self) {
    Ov003Global* global;
    s32          dx;
    s32          dy;

    if (!CombatTouch_IsGesture(6) || !CombatTouch_IsTargetKind(1)) {
        return FALSE;
    }
    global = g_CombatGlobal;
    dy     = (global->touchWork.startY - global->touchWork.y) << 12;
    dx     = (global->touchWork.startX - global->touchWork.x) << 12;
    if (FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy)) < 0x8000) {
        return FALSE;
    }
    func_ov003_0208f384(self, func_ov003_02090688);
    return TRUE;
}

/// Walks towards `target`, to its side (`near` keeps the player at most a screen away from the borders).
static void func_ov003_0208f998(BtlPlayer00* self, CombatActor* target, s32 near) {
    s32 targetX;
    s32 goal;
    s32 targetY;

    if (target == NULL) {
        return;
    }
    targetX = target->position.x;
    if (self->actor.position.x > targetX) {
        self->actor.isFlipped = FALSE;
    }
    if (self->actor.position.x < targetX) {
        self->actor.isFlipped = TRUE;
    }
    if (near != 0) {
        s32 x = self->actor.position.x;

        if (x < 0x6C000) {
            goal = target->position.x - 0x21000;
        } else if (x > g_CombatGlobal->unk3D7C0[0].unk_0C - 0x6C000) {
            goal = target->position.x + 0x21000;
        } else {
            near = 0;
        }
    }
    if (near == 0) {
        if (self->actor.isFlipped != 0) {
            goal = target->position.x - 0x21000;
        } else {
            goal = target->position.x + 0x21000;
        }
    }
    targetY = target->position.y;
    if (g_CombatGlobal->unk3D894 < 100) {
        return;
    }
    self->actor.position.x += (goal - self->actor.position.x) / 8;
    self->actor.position.y += (targetY - self->actor.position.y) / 8;
}

/// Follows the stylus: the player glides towards the touch point (`mode` != 0 is the quick follow).
static void func_ov003_0208fa78(BtlPlayer00* self, s32 mode) {
    s32 vx;
    s32 vy;

    if (self->flags & 4) {
        BOOL moved;

        if (!CombatTouch_IsGesture(6)) {
            moved = FALSE;
        } else {
            self->targetX = CombatStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
            self->targetY = CombatStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
            CombatStage_ClampXY(&self->targetX, &self->targetY, 0x8000, 0x8000);
            moved = TRUE;
        }
        if (!moved) {
            self->flags &= ~4;
        }
    }
    if (mode != 0) {
        vx = (self->targetX - self->actor.position.x) / 4;
        vy = (self->targetY - self->actor.position.y) / 4;
        if (vx > 0xE000) {
            vx = 0xE000;
        } else if (vx < -0xE000) {
            vx = -0xE000;
        }
        if (vy > 0xE000) {
            vy = 0xE000;
        } else if (vy < -0xE000) {
            vy = -0xE000;
        }
    } else {
        vx = (self->targetX - self->actor.position.x) / 8;
        vy = (self->targetY - self->actor.position.y) / 8;
        if (vx > 0x8000) {
            vx = 0x8000;
        } else if (vx < -0x8000) {
            vx = -0x8000;
        }
        if (vy > 0x8000) {
            vy = 0x8000;
        } else if (vy < -0x8000) {
            vy = -0x8000;
        }
    }
    self->actor.position.x += vx;
    self->actor.position.y += vy;
    CombatStage_SetCameraTarget(0, self->targetX, self->targetY, 0);
}

/// Casts the hit box `id` of the current attack `x`,`z` pixels in front of the player.
static BOOL func_ov003_0208fc24(BtlPlayer00* self, u16 id, s32 x, s32 z) {
    Ov003EffectState* effect = self->command.state;
    s32               hitX;

    if (self->actor.isFlipped == 0) {
        hitX = self->actor.position.x - (x << 12);
    } else {
        hitX = self->actor.position.x + (x << 12);
    }
    return CombatHit_AttackAt(CombatHit_GetAttackInfo(id), (Ov003AttackArgs*)&effect->unk_0C, hitX, self->actor.position.y,
                              self->actor.position.z + (z << 12)) == 1;
}

/// The same for a multi-hit attack: counts the hits and puts the finishing flag on the last one.
static BOOL func_ov003_0208fc7c(BtlPlayer00* self, u16 id, s32 x, s32 z) {
    Ov003EffectState* effect = self->command.state;
    s32               hits   = effect->unk_08;
    s32               last   = hits >= effect->flag;
    s32               hitX;
    s32               hitZ;
    s32               result;

    if (self->actor.isFlipped == 0) {
        hitX = self->actor.position.x - (x << 12);
    } else {
        hitX = self->actor.position.x + (x << 12);
    }
    hitZ = self->actor.position.z + (z << 12);
    if (last == 0) {
        result = CombatHit_AttackAt(CombatHit_GetAttackInfo(id), (Ov003AttackArgs*)&effect->unk_0C, hitX,
                                    self->actor.position.y, hitZ);
    } else {
        Ov003AttackInfo hit = *CombatHit_GetAttackInfo(id);

        hit.flags |= 0x1000;
        result = CombatHit_AttackAt(&hit, (Ov003AttackArgs*)&effect->unk_0C, hitX, self->actor.position.y, hitZ);
    }
    if (result != 1) {
        return FALSE;
    }
    effect->unk_08++;
    if (last != 0) {
        effect->unk_08 = 0;
        func_ov003_0209fc88(2, hitX, self->actor.position.y, hitZ, 0x10, 0);
    }
    return TRUE;
}

static void func_ov003_0208fd6c(BtlPlayer00* self, s32 x, s32 z, s32 flag) {
    s32 hitX;
    s32 hitZ;

    if (self->actor.isFlipped == 0) {
        hitX = self->actor.position.x - (x << 12);
    } else {
        hitX = self->actor.position.x + (x << 12);
    }
    hitZ = self->actor.position.z + (z << 12);
    CombatSe_PlayAtMainX(0x145, hitX);
    if (flag != 0) {
        func_ov003_0209fc88(2, hitX, self->actor.position.y, hitZ, 0x10, 0);
    }
}

/// The first state of a battle: waits for the battle to start.
static void func_ov003_0208fde0(BtlPlayer00* self) {
    if (!(g_CombatGlobal->unk3D874 != 2)) {
        func_ov003_0208f384(self, func_ov003_02090588);
    }
}

/// Stands up from a hit.
static void func_ov003_0208fe10(BtlPlayer00* self) {
    if (self->actor.periodicEffectMode == 4 && func_ov003_020a58c4() == 3 && func_ov003_0208f624(self)) {
        func_ov003_0208f384(self, func_ov003_0209228c);
        return;
    }
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->actor.flags |= 2;
                BtlPlayer00_FaceX(&self->actor, self->actor.unk_0C);
                BtlCollision_SetDisabled(&self->collider, 0);
                CombatSprite_SetAnimFromTable(&self->sprite, 4, 1);
                CombatSprite_Restart(&self->sprite);
                func_ov003_0208f498(self, 0);
                self->flags &= ~2;
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 1:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 5, 1);
                self->timer = self->actor.unk_10;
            }
            if (self->timer > 0) {
                self->timer--;
            }
            if (self->timer > 0) {
                return;
            }
            if (self->actor.periodicEffectMode == 4) {
                return;
            }
            self->phase++;
            self->timer = 0;
            break;
        case 2:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 6, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->actor.flags &= ~2;
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
}

/// Knocked into the air and falling down.
static void func_ov003_0208fff8(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->actor.flags |= 2;
                self->actor.flags |= 0x4000;
                BtlPlayer00_FaceX(&self->actor, self->actor.unk_0C);
                CombatSprite_SetAnimFromTable(&self->sprite, 8, 1);
                CombatSprite_Restart(&self->sprite);
                func_ov003_0208f498(self, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                CombatSprite_SetAnimFromTable(&self->sprite, 9, 0);
                self->phase++;
                self->timer = 0;
            }
            break;
        case 1:
            if (TouchInput_WasTouchPressed() && !(self->actor.flags & 0x20000) && Inventory_GetOwnedCount(0x2A9) != 0) {
                self->actor.flags = (self->actor.flags | 0x10) & 0xFFFFBFFD;
                func_ov003_0208f384(self, func_ov003_02090288);
                return;
            }
            if (self->actor.position.z >= 0) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 2:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 10, 1);
                self->actor.flags = (self->actor.flags | 0x10) & 0xFFFFBFFD;
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 3:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 11, 0);
                self->timer = 0x1E;
            }
            self->timer--;
            if (self->timer <= 0) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 4:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 12, 1);
                func_ov003_0208f564(self);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                func_ov003_0208f384(self, func_ov003_02090288);
            }
            break;
    }
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, 0);
}

/// Gets up with the help of the revive item.
static void func_ov003_02090288(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            CombatSprite_SetAnimFromTable(&self->sprite, 13, 1);
            func_ov003_02082f7c(&self->actor, 0x10);
            self->phase++;
            break;
        case 1:
            if (self->actor.position.z >= 0) {
                self->actor.flags &= ~0x10;
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, 0);
}

/// The battle ends in a win: the player cheers.
static void func_ov003_02090318(BtlPlayer00* self) {
    if (!(g_CombatGlobal->unk3D874 != 2)) {
        if (self->timer == 0) {
            if (gSaveData.unk_3134 != 0x2B) {
                BtlMgr_SetState(4);
                self->actor.flags |= 4;
                BtlCollision_SetDisabled(&self->collider, 0);
                CombatSprite_SetAnimFromTable(&self->sprite, 7, 1);
                EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Task_BtlOver, NULL, 0, NULL, NULL);
            } else {
                BtlMgr_SetState(6);
            }
        }
        g_CombatGlobal->unk3D904 = 2;
        g_CombatGlobal->unk3D908 = 2;
        self->timer++;
    }
}

/// The battle ends in a loss: the player collapses.
static void func_ov003_0209040c(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            BtlCollision_SetDisabled(&self->collider, 1);
            CombatSprite_SetPaletteMode(&self->sprite, 2);
            CombatSprite_SetAnimFromTable(&self->sprite, 47, 1);
            self->actor.zVelocity = -0x2000;
            self->phase++;
            break;
        case 1:
            if (self->actor.zVelocity > 0) {
                self->actor.zVelocity = 0;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                CombatSprite_SetAnimFromTable(&self->sprite, 2, 0);
                CombatSprite_SetPaletteMode(&self->sprite, 0);
                self->phase++;
            }
            break;
        case 2:
            if (!(g_CombatGlobal->unk3D878 & 8)) {
                BtlCollision_SetDisabled(&self->collider, 0);
                self->actor.flags &= ~0x10;
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, self->actor.position.z);
}

static void func_ov003_0209052c(BtlPlayer00* self) {
    if (func_ov003_0208f664(self)) {
        return;
    }
    if (func_ov003_0208f8d4(self)) {
        return;
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        func_ov003_0208f384(self, func_ov003_02090588);
    }
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, 0);
}

/// Idle: stands, walks in place and waits for the stylus.
static void func_ov003_02090588(BtlPlayer00* self) {
    self->flags &= ~2;
    switch (func_ov003_0208765c(&self->comboState, g_CombatGlobal->unk3D8D6, self->timer)) {
        case 1:
            CombatSprite_SetAnimFromTable(&self->sprite, 0, 0);
            break;
        case 2:
            CombatSprite_SetAnimFromTable(&self->sprite, 1, 0);
            break;
        case 3:
            CombatSprite_SetAnimFromTable(&self->sprite, 3, 1);
            break;
    }
    if (self->comboState == 3 && SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        CombatSprite_SetAnimFromTable(&self->sprite, 2, 0);
    }
    self->timer++;
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, self->actor.position.z);
    if (!func_ov003_0208f664(self)) {
        func_ov003_0208f8d4(self);
    }
}

/// Dragged by the stylus.
static void func_ov003_02090688(BtlPlayer00* self) {
    Ov003Global* global;
    s32          dx;
    s32          dy;
    s32          distance;

    if (self->phase == 0) {
        self->flags |= 2;
        self->phase++;
    }
    if (func_ov003_0208f664(self)) {
        return;
    }
    if (!CombatTouch_IsTargetKind(1)) {
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    if (CombatTouch_IsGesture(6)) {
        self->targetX = CombatStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
        self->targetY = CombatStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
        CombatStage_ClampXY(&self->targetX, &self->targetY, 0x8000, 0x8000);
    }
    dx       = self->targetX - self->actor.position.x;
    dy       = self->targetY - self->actor.position.y;
    distance = FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy)) >> 12;
    if (distance > 0x50 && g_CombatGlobal->unk3D894 >= 100) {
        func_ov003_0208f384(self, func_ov003_020908c4);
        return;
    }
    if (distance > 8) {
        BtlPlayer00_FaceX(&self->actor, self->targetX);
        global = g_CombatGlobal;
        BtlMgr_MoveByAngle(&self->actor.position.x, &self->actor.position.y, FX_Atan2Idx(dy, dx),
                           global->unk3D894 * 0x2CCD / 100);
        CombatStage_SetCameraTarget(0, self->targetX, self->targetY, 0);
        CombatSprite_SetAnimFromTable(&self->sprite, 14, 0);
        func_ov003_0208f418(&self->sprite, self->actor.position.x);
        return;
    }
    if (CombatTouch_IsGesture(6)) {
        CombatSprite_SetAnimFromTable(&self->sprite, 14, 0);
        func_ov003_0208f418(&self->sprite, self->actor.position.x);
        return;
    }
    func_ov003_0208f384(self, func_ov003_02090588);
}

/// The dash: the stylus is far away and the player runs to it.
static void func_ov003_020908c4(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->flags |= 4;
                self->actor.flags |= 0x10;
                BtlCollision_SetDisabled(&self->collider, 1);
                CombatSprite_SetAnimFromTable(&self->sprite, 15, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 1:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 16, 1);
                self->timer++;
            }
            func_ov003_0208fa78(self, 0);
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 2:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 17, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->actor.flags &= ~0x10;
                BtlCollision_SetDisabled(&self->collider, 0);
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
    BtlPlayer00_FaceX(&self->actor, self->targetX);
}

/// The dodge roll.
static void func_ov003_02090a5c(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->flags |= 4;
                self->actor.flags |= 0x10;
                BtlCollision_SetDisabled(&self->collider, 1);
                CombatSprite_SetAnimFromTable(&self->sprite, 18, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 1:
            if (self->timer == 0) {
                g_CombatGlobal->unk3D8C8 = 1;
                CombatSprite_SetAnimFromTable(&self->sprite, 19, 1);
                self->timer++;
            }
            func_ov003_0208fa78(self, 1);
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 2:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 20, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                g_CombatGlobal->unk3D8C8 = 0;
                self->actor.flags &= ~0x10;
                BtlCollision_SetDisabled(&self->collider, 0);
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
    BtlPlayer00_FaceX(&self->actor, self->targetX);
}

/// A slash combo: three animations by the kind of the queued attack, hits at frame 10.
static void func_ov003_02090c18(BtlPlayer00* self) {
    if (self->timer == 0) {
        func_ov003_0208f5e0(self, self->command.state->unk_08 >= self->command.state->flag);
        switch (self->command.angle) {
            case 0:
                CombatSe_PlayAtMainX(0x147, self->actor.position.x);
                CombatSprite_SetAnimFromTable(&self->sprite, 21, 1);
                func_ov003_0209f000(0);
                break;
            case 1:
            case 2:
                CombatSe_PlayAtMainX(0x146, self->actor.position.x);
                CombatSprite_SetAnimFromTable(&self->sprite, 22, 1);
                func_ov003_0209f000(1);
                break;
            case 3:
                CombatSe_PlayAtMainX(0x147, self->actor.position.x);
                CombatSprite_SetAnimFromTable(&self->sprite, 23, 1);
                func_ov003_0209f000(2);
                break;
        }
        CombatSprite_Restart(&self->sprite);
        func_ov003_020a5aa4(self->command.state);
        func_ov003_020a5a94(self->command.state);
    }
    switch (self->command.angle) {
        case 0:
            func_ov003_0208f39c(self, data_ov003_020d7960);
            break;
        case 1:
        case 2:
            func_ov003_0208f39c(self, data_ov003_020d797e);
            break;
        case 3:
            func_ov003_0208f39c(self, data_ov003_020d79a2);
            break;
    }
    if (self->timer <= 7) {
        func_ov003_0208f998(self, self->command.target, 1);
        self->targetX = self->actor.position.x;
        self->targetY = self->actor.position.y;
        CombatStage_SetCameraTarget(0, self->targetX, self->actor.position.y, 0);
    }
    if (self->timer == 10) {
        if (self->command.state->unk_08 < self->command.state->flag) {
            if (func_ov003_0208fc24(self, 0, 0x24, -0x20)) {
                func_ov003_0208fd6c(self, 0x24, -0x20, 0);
            }
        } else {
            if (func_ov003_0208fc24(self, 1, 0x24, -0x20)) {
                func_ov003_0208fd6c(self, 0x24, -0x20, 1);
            }
        }
    }
    if (self->command.state->unk_08 < self->command.state->flag && self->timer >= 10 && func_ov003_0208f624(self)) {
        self->command.state->unk_08++;
        func_ov003_0208f384(self, func_ov003_02090c18);
        return;
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        g_CombatGlobal->unk3D8C8 = 0;
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    self->timer++;
}

static void func_ov003_02090ec4(BtlPlayer00* self) {
    if (self->timer == 0) {
        CombatSprite_SetAnimFromTable(&self->sprite, 43, 1);
        self->timer++;
    }
    if (!(g_CombatGlobal->unk3D8C8 != 0)) {
        if (func_ov003_0208f624(self)) {
            func_ov003_0208f384(self, func_ov003_02090f50);
        } else {
            func_ov003_0208f384(self, func_ov003_02090588);
        }
    }
}

static void func_ov003_02090f50(BtlPlayer00* self) {
    u16 tick;

    if (self->timer == 0) {
        CombatSe_PlayAtMainX(0x363, self->actor.position.x);
        CombatSe_PlayAtMainX(0x9A, self->actor.position.x);
        func_ov003_0209f000(7);
        CombatSprite_SetAnimFromTable(&self->sprite, 28, 1);
        CombatSprite_Restart(&self->sprite);
        func_ov003_020a5aa4(self->command.state);
        func_ov003_020a5a94(self->command.state);
    }
    tick = BtlPlayer00_Tick(&self->sprite);
    if (tick == 1 && self->sprite.sprite.bit_11 == 1) {
        func_ov003_02082f7c(&self->actor, 0x15);
    }
    if (tick >= 1 && tick < 3) {
        if (self->actor.isFlipped == 0) {
            self->actor.position.x -= 0x2000;
        } else {
            self->actor.position.x += 0x2000;
        }
    }
    if (tick >= 4 && tick < 8) {
        func_ov003_0208fc24(self, 9, 0x40, -0x40);
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        g_CombatGlobal->unk3D8C8 = 0;
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    self->timer++;
}

static void func_ov003_020910ac(BtlPlayer00* self) {
    if (self->timer == 0) {
        func_ov003_0208f5e0(self, 0);
        func_ov003_0209e9f8(0, 1, self->actor.position.x, self->actor.position.y, 0, self->actor.isFlipped != 0);
        func_ov003_0209f000(3);
        CombatSprite_SetAnimFromTable(&self->sprite, 24, 1);
        CombatSprite_Restart(&self->sprite);
        func_ov003_020a5aa4(self->command.state);
        func_ov003_020a5a94(self->command.state);
    }
    if (self->timer == 0xE) {
        CombatSe_PlayAtMainX(0x37, self->actor.position.x);
    }
    if (self->timer <= 0xE) {
        func_ov003_0208f998(self, self->command.target, 0);
        self->targetX = self->actor.position.x;
        self->targetY = self->actor.position.y;
        CombatStage_SetCameraTarget(0, self->targetX, self->actor.position.y, 0);
    }
    if (self->timer == 0x11) {
        if (func_ov003_0208fc24(self, 3, 0x3C, -0x18)) {
            CombatSe_PlayAtMainX(0x38, self->actor.position.x);
        }
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        g_CombatGlobal->unk3D8C8 = 0;
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    self->timer++;
}

static void func_ov003_02091214(BtlPlayer00* self) {
    Ov003EffectState* effect = self->command.state;
    s32               speed  = effect->params[0];
    u16               tick;
    s32               x;
    s32               wasFlipped;

    if (self->timer == 0) {
        BtlPlayer00_FaceX(&self->actor, g_CombatGlobal->unk3D8CC);
        func_ov003_0208f5e0(self, 0);
        CombatSe_PlayAtMainX(0x8E, self->actor.position.x);
        func_ov003_0209f000(4);
        CombatSprite_SetAnimFromTable(&self->sprite, 25, 1);
        CombatSprite_Restart(&self->sprite);
        func_ov003_020a5aa4(self->command.state);
        func_ov003_020a5a94(self->command.state);
    }
    tick = BtlPlayer00_Tick(&self->sprite);
    if (tick == 3 && self->sprite.sprite.bit_11 == 1) {
        func_ov003_0209e9f8(0, 1, self->actor.position.x, self->actor.position.y, 0, self->actor.isFlipped != 0);
    }
    if (tick >= 3 && tick < 6) {
        BtlCollision_SetDisabled(&self->collider, 1);
        if (self->actor.isFlipped == 0) {
            BtlMgr_MoveByAngle(&self->actor.position.x, &self->actor.position.y, 0x8000, speed << 12);
        } else {
            BtlMgr_MoveByAngle(&self->actor.position.x, &self->actor.position.y, 0, speed << 12);
        }
        CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, self->actor.position.z);
        func_ov003_0208fc7c(self, 5, 0x3C, -0x18);
    }
    x          = self->actor.position.x;
    wasFlipped = self->actor.isFlipped;
    if (x < 0x10000) {
        self->actor.isFlipped = TRUE;
    } else if (x > g_CombatGlobal->unk3D7C0[0].unk_0C - 0x10000) {
        self->actor.isFlipped = FALSE;
    }
    if (wasFlipped != self->actor.isFlipped) {
        CombatSe_PlayAtMainX(0x8E, self->actor.position.x);
    }
    if (tick == 6 && self->sprite.sprite.bit_11 == 1) {
        BtlCollision_SetDisabled(&self->collider, 0);
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        g_CombatGlobal->unk3D8C8 = 0;
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    self->timer++;
}

static void func_ov003_02091468(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 32, 1);
                self->timer++;
            }
            func_ov003_0208f998(self, self->command.target, 1);
            self->targetX = self->actor.position.x;
            self->targetY = self->actor.position.y;
            CombatStage_SetCameraTarget(0, self->targetX, self->actor.position.y, 0);
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            func_ov003_0208f5e0(self, 0);
            self->phase++;
            self->timer = 0;
            break;
        case 1:
            if (self->timer == 0) {
                CombatSe_PlayAtMainX(0x8B, self->actor.position.x);
                func_ov003_0209f000(9);
                CombatSprite_SetAnimFromTable(&self->sprite, 33, 1);
                CombatSprite_Restart(&self->sprite);
                func_ov003_020a5a94(self->command.state);
                if (func_ov003_020a5aa4(self->command.state) == 0) {
                    g_CombatGlobal->unk3D8C8 = 0;
                }
                self->timer++;
            }
            func_ov003_0208fc7c(self, 4, 0x3C, -0x18);
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            if (self->command.angle == 0) {
                if (func_ov003_0208f624(self)) {
                    self->timer = 0;
                    return;
                }
                self->phase++;
                self->timer = 0;
            } else {
                self->phase++;
                self->timer = 0;
            }
            break;
        case 2:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 34, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                g_CombatGlobal->unk3D8C8 = 0;
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
}

static void func_ov003_02091680(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 29, 1);
                self->timer++;
            }
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            func_ov003_0208f5e0(self, 0);
            self->phase++;
            self->timer = 0;
            break;
        case 1:
            if (self->timer == 0) {
                CombatSe_PlayAtMainX(0x8F, self->actor.position.x);
                func_ov003_0209f000(8);
                CombatSprite_SetAnimFromTable(&self->sprite, 30, 1);
                CombatSprite_Restart(&self->sprite);
                func_ov003_020a5a94(self->command.state);
                if (func_ov003_020a5aa4(self->command.state) == 0) {
                    g_CombatGlobal->unk3D8C8 = 0;
                }
            }
            if (self->timer > 3) {
                func_ov003_0208fc7c(self, 8, 0, -0x20);
            }
            self->timer++;
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            if (func_ov003_0208f624(self)) {
                self->timer = 0;
                return;
            }
            self->phase++;
            self->timer = 0;
            break;
        case 2:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 31, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                g_CombatGlobal->unk3D8C8 = 0;
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
}

static void func_ov003_02091850(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (g_CombatGlobal->unk3D8C8 != 0) {
                return;
            }
            if (func_ov003_0208f624(self)) {
                self->phase++;
                return;
            }
            func_ov003_0208f384(self, func_ov003_02090588);
            break;
        case 1:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 29, 1);
                self->timer++;
            }
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            func_ov003_0208f5e0(self, 0);
            self->phase++;
            self->timer = 0;
            break;
        case 2:
            if (self->timer == 0) {
                CombatSe_PlayAtMainX(0x8F, self->actor.position.x);
                func_ov003_0209f000(8);
                CombatSprite_SetAnimFromTable(&self->sprite, 30, 1);
                CombatSprite_Restart(&self->sprite);
            }
            if (self->timer > 3) {
                func_ov003_0208fc7c(self, 8, 0, -0x20);
            }
            self->timer++;
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            self->phase++;
            self->timer = 0;
            break;
        case 3:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 31, 1);
                self->timer++;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                func_ov003_0208f384(self, func_ov003_02090588);
            }
            break;
    }
}

static void func_ov003_02091a14(BtlPlayer00* self) {
    u16 tick;

    if (self->timer == 0) {
        func_ov003_0208f5e0(self, 0);
        CombatSe_PlayAtMainX(0x39, self->actor.position.x);
        func_ov003_0209f000(5);
        CombatSprite_SetAnimFromTable(&self->sprite, 26, 1);
        CombatSprite_Restart(&self->sprite);
        func_ov003_020a5aa4(self->command.state);
        func_ov003_020a5a94(self->command.state);
    }
    tick = BtlPlayer00_Tick(&self->sprite);
    if (tick < 2) {
        func_ov003_0208f998(self, self->command.target, 1);
        CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, self->actor.position.z);
        self->targetX = self->actor.position.x;
        self->targetY = self->actor.position.y;
    }
    if (tick == 2 && self->sprite.sprite.bit_11 == 1) {
        func_ov003_02082f7c(&self->actor, 0x25);
    }
    if (tick == 2) {
        if (func_ov003_0208fc7c(self, 7, 0x18, -0x40)) {
            CombatSe_PlayAtMainX(0x3A, self->actor.position.x);
        }
    }
    if (tick == 3) {
        g_CombatGlobal->unk3D8C8 = 0;
        func_ov003_0208f384(self, func_ov003_0209052c);
        return;
    }
    self->timer++;
}

static void func_ov003_02091b7c(BtlPlayer00* self) {
    u16 tick;

    if (self->timer == 0) {
        CombatSe_PlayAtMainX(0x8C, self->actor.position.x);
        CombatSys_WarpCharacter(0, self->command.target->position.x, self->command.target->position.y,
                                self->command.target->position.z - 0x60000);
        func_ov003_0208f5e0(self, 0);
        func_ov003_0209f000(6);
        CombatSprite_SetAnimFromTable(&self->sprite, 27, 1);
        CombatSprite_Restart(&self->sprite);
        func_ov003_020a5aa4(self->command.state);
        func_ov003_020a5a94(self->command.state);
    }
    tick = BtlPlayer00_Tick(&self->sprite);
    if (tick == 4 && self->sprite.sprite.bit_11 == 1) {
        CombatSe_PlayAtMainX(0x8D, self->actor.position.x);
    }
    if (tick >= 4 && tick < 7) {
        func_02026590(&self->actor.position.z, 0, (u16)(s16)(0x1C - self->timer));
        self->targetX = self->actor.position.x;
        self->targetY = self->actor.position.y;
        func_ov003_0208fc24(self, 6, 0, -0x18);
    }
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, 0);
    if (tick == 7) {
        g_CombatGlobal->unk3D8C8 = 0;
        func_ov003_0208f384(self, func_ov003_0209052c);
        return;
    }
    self->actor.zVelocity = 0;
    self->timer++;
}

/// Held up in the air by the stylus.
static void func_ov003_02091d10(BtlPlayer00* self) {
    s16 sine;

    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 35, 1);
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                CombatSprite_SetAnimFromTable(&self->sprite, 36, 0);
                self->phase++;
                self->timer = 0;
            } else {
                self->timer++;
            }
            break;
        case 1:
            BtlPlayer00_FaceX(&self->actor, g_CombatGlobal->unk3D8CC);
            if (g_CombatGlobal->unk3D8C8 == 0) {
                CombatSprite_SetAnimFromTable(&self->sprite, 35, 1);
                func_ov003_0208f384(self, func_ov003_0209052c);
            } else {
                self->timer++;
            }
            break;
    }
    sine                  = data_0205e4e0[((u16)(self->timer << 9) >> 4) * 2];
    self->actor.zVelocity = 0;
    self->actor.position.z += ((sine << 3) - 0x10000 - self->actor.position.z) >> 3;
    if (!(self->timer % 60 == 0)) {
        CombatSe_PlayAtMainX(0x150, self->actor.position.x);
    }
}

static void func_ov003_02091ea0(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                BtlPlayer00_FaceX(&self->actor, g_CombatGlobal->unk3D8CC);
                CombatSprite_SetAnimFromTable(&self->sprite, 37, 1);
                self->timer++;
            }
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            CombatSprite_SetAnimFromTable(&self->sprite, 38, 0);
            self->phase++;
            self->timer = 0;
            break;
        case 1:
            BtlPlayer00_FaceX(&self->actor, g_CombatGlobal->unk3D8CC);
            if (g_CombatGlobal->unk3D8C8 != 0) {
                return;
            }
            CombatSprite_SetAnimFromTable(&self->sprite, 39, 1);
            func_ov003_0208f384(self, func_ov003_0209052c);
            break;
    }
}

static void func_ov003_02091fcc(BtlPlayer00* self) {
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                BtlPlayer00_FaceX(&self->actor, g_CombatGlobal->unk3D8CC);
                CombatSprite_SetAnimFromTable(&self->sprite, 40, 1);
                self->timer++;
            }
            if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                return;
            }
            CombatSprite_SetAnimFromTable(&self->sprite, 41, 0);
            self->phase++;
            self->timer = 0;
            break;
        case 1:
            if (g_CombatGlobal->unk3D8C8 != 0) {
                return;
            }
            CombatSprite_SetAnimFromTable(&self->sprite, 42, 1);
            func_ov003_0208f384(self, func_ov003_0209052c);
            break;
    }
}

/// Drags the marker with the stylus: the player is carried along.
static void func_ov003_020920c8(BtlPlayer00* self) {
    if (self->timer == 0) {
        self->flags |= 2;
        self->actor.flags |= 0x10;
        BtlCollision_SetDisabled(&self->collider, 1);
    }
    if (self->timer == 4) {
        CombatSe_PlayAtMainX(0x3B, self->actor.position.x);
        self->flags |= 1;
    }
    if (self->timer > 4) {
        if (CombatTouch_IsGesture(6)) {
            self->targetX = CombatStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
            self->targetY = CombatStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
            CombatStage_ClampXY(&self->targetX, &self->targetY, 0x8000, 0x8000);
        }
        CombatStage_SetCameraTarget(0, self->targetX, self->targetY, 0);
        BtlPlayer00_FaceX(&self->actor, self->targetX);
        self->actor.position.x = self->targetX;
        self->actor.position.y = self->targetY;
        self->actor.screenX    = CombatStage_ToScreenX(0, self->actor.position.x);
        self->actor.screenY    = CombatStage_ToScreenY(0, self->actor.position.y, self->actor.position.z);
    }
    if (g_CombatGlobal->unk3D8C8 == 0) {
        self->flags &= ~1;
        self->actor.flags &= ~0x10;
        BtlCollision_SetDisabled(&self->collider, 0);
        CombatSe_PlayAtMainX(0x3C, self->actor.position.x);
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    self->timer++;
}

/// A special: the queued effect hits for a share of the gauge.
static void func_ov003_0209228c(BtlPlayer00* self) {
    Ov003EffectState* effect = self->command.state;
    u16               anim;
    s32               hitFrame;
    s16               damage;

    switch (effect->params[1]) {
        case 2:
            anim     = 0x2E;
            hitFrame = 9;
            break;
        case 1:
            anim     = 0x2D;
            hitFrame = 0x15;
            break;
        default:
            anim     = 0x2C;
            hitFrame = 0x22;
            break;
    }
    if (self->timer == 0) {
        CombatSe_PlayAtMainX(0x14E, self->actor.position.x);
        self->actor.flags &= ~2;
        CombatSprite_SetAnimFromTable(&self->sprite, anim, 1);
    }
    if (self->timer == hitFrame) {
        func_ov003_020b67dc(effect, &self->actor);
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
        damage = g_CombatGlobal->unk3D8D2 * effect->params[0] / 100;
        if (damage < 1) {
            damage = 1;
        }
        if (g_CombatGlobal->unk3D8F4 != 0) {
            damage = damage * g_CombatGlobal->unk3D8EE / 100;
            func_ov003_0209c568(0);
        }
        if (CombatAbility_IsActive(0x4A)) {
            damage = damage + (s16)(damage * CombatAbility_GetValue(0x4A) / 100);
        }
        BtlMgr_AddTeamHp(0, damage);
        func_ov003_0209b564(0, damage, &self->actor, 1);
        if (func_ov003_020b68ec(effect, &self->actor)) {
            CombatActor_ClearPeriodicEffect(&self->actor);
        }
        if (func_ov003_020b68ec(effect, g_CombatGlobal->unk3D89C)) {
            CombatActor_ClearPeriodicEffect(g_CombatGlobal->unk3D89C);
        }
        func_ov003_020a5aa4(effect);
        if (g_CombatGlobal->unk3D8ED == 0 || g_CombatGlobal->unk3D8F4 != 0) {
            g_CombatGlobal->unk3D8EC = 1;
        }
        func_ov003_0208f384(self, func_ov003_02090588);
        return;
    }
    self->timer++;
}

static s32 BtlPlayer00_Init(TaskPool* pool, Task* task, void* args) {
    BtlPlayer00* self = task->data;
    Ov003Global* global;

    MI_CpuSet(self, 0, sizeof(BtlPlayer00));
    CombatActor_Init(&self->actor, 0);
    g_CombatGlobal->unk3D898 = self;
    g_CombatGlobal->unk3D8C8 = 0;
    self->actor.position.x   = g_CombatGlobal->unk3D7C0[0].unk_0C / 2;
    self->actor.position.y   = g_CombatGlobal->unk3D7C0[0].unk_10 / 2;
    self->actor.position.z   = 0;
    self->actor.unk_70       = 5;
    self->actor.unk_72       = 0x28;
    self->actor.unk_76       = 0x18;
    self->actor.unk_66       = func_ov003_0208ca54(0);
    self->actor.unk_68       = func_ov003_0208cae0(0);
    if (CombatAbility_IsActiveForPlayer(1)) {
        self->actor.unk_66 += (s16)CombatAbility_GetPlayerValue(1);
    }
    if (CombatAbility_IsActiveForPlayer(2)) {
        self->actor.unk_68 += (s16)CombatAbility_GetPlayerValue(2);
    }
    self->comboState = 0;
    self->gaugeTimer = 0;
    self->guardTimer = 0;
    self->hurtTimer  = 0;
    CombatSprite_LoadFromTable(0, &self->sprite, &data_ov003_020d77d8, data_ov003_020d79cc, 2, 0x3A, 0x64);
    CombatHit_LoadSpark(0, &self->shadow, 2);
    CombatSprite_LoadDirect(0, &self->target, &data_ov003_020d7800, 22, 24, 23, 0x3D, 0);
    CombatStage_SetCameraTarget(0, self->actor.position.x, self->actor.position.y, self->actor.position.z);
    BtlCollision_Add(&self->collider, 0, 0xC, 0x30);
    func_ov003_0208f384(self, func_ov003_0208fde0);
    return 1;
}

static s32 BtlPlayer00_Update(TaskPool* pool, Task* task, void* args) {
    BtlPlayer00* self = task->data;
    s32          flags;

    if (g_CombatGlobal->unk3D878 & 0x40) {
        return 1;
    }
    switch (CombatActor_PopPendingCommand(&self->actor)) {
        case ACTOR_CMD_HIT:
            if (CombatAbility_IsActiveForPlayer(0x37)) {
                self->actor.zVelocity = -0x3800;
                func_ov003_0208f384(self, func_ov003_0208fff8);
            } else {
                func_ov003_0208f384(self, func_ov003_0208fe10);
            }
            break;
        case ACTOR_CMD_KNOCKDOWN:
            flags = self->actor.flags;
            if (!(flags & 0x20000) && !(flags & 0x4000) && CombatAbility_IsActiveForPlayer(0x36)) {
                self->actor.zVelocity = 0;
                func_ov003_0208f384(self, func_ov003_0208fe10);
            } else {
                func_ov003_0208f384(self, func_ov003_0208fff8);
            }
            break;
        case ACTOR_CMD_DEFEAT:
            func_ov003_0208f384(self, func_ov003_02090318);
            break;
        case ACTOR_CMD_LEVEL_UP:
            func_ov003_0208f384(self, func_ov003_0209040c);
            break;
    }
    if (self->update != NULL) {
        self->update(self);
    }
    CombatActor_UpdateEffects(0, &self->actor);
    CombatActor_UpdatePhysics(&self->actor);
    func_ov003_02087458(0, &self->gaugeTimer);
    func_ov003_02087540(0);
    if (self->collider.hitLayers != 0) {
        BtlCollision_ApplyPush(&self->collider, &self->actor.position.x, &self->actor.position.y, &self->actor.position.z);
    }
    CombatStage_ClampXY(&self->actor.position.x, &self->actor.position.y, 0x8000, 0x8000);
    BtlCollision_SetPosition(&self->collider, self->actor.position.x, self->actor.position.y, self->actor.position.z);
    CombatSprite_Update(&self->sprite);
    CombatSprite_Update(&self->shadow);
    if (self->flags & 2) {
        CombatSprite_Update(&self->target);
    }
    if (self->guardTimer > 0) {
        self->guardTimer--;
    }
    if (self->hurtTimer > 0) {
        self->hurtTimer--;
    }
    return 1;
}

static s32 BtlPlayer00_Render(TaskPool* pool, Task* task, void* args) {
    BtlPlayer00* self = task->data;

    if (!(self->flags & 1)) {
        CombatActor_Render(&self->actor, &self->sprite);
        CombatHit_DrawSparkOnActor(&self->shadow, &self->actor);
    }
    if (self->flags & 2) {
        s32 x = CombatStage_ToScreenX(0, self->targetX);
        s32 y = CombatStage_ToScreenY(0, self->targetY, 0);

        CombatSprite_SetPosition(&self->target, x, y);
        func_ov003_02082730(&self->target, 0x7FFFFFFF - self->targetY);
        CombatSprite_Render(&self->target);
    }
    return 1;
}

static s32 BtlPlayer00_Release(TaskPool* pool, Task* task, void* args) {
    BtlPlayer00* self = task->data;

    CombatSprite_Release(&self->sprite);
    CombatSprite_Release(&self->shadow);
    CombatSprite_Release(&self->target);
    return 1;
}

static s32 BtlPlayer00_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPlayer00_Init,
        .update     = BtlPlayer00_Update,
        .render     = BtlPlayer00_Render,
        .cleanup    = BtlPlayer00_Release,
    };

    if (func_ov003_0208b690(stage)) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}
