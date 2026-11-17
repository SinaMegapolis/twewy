#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Engine/Math/Random.h"
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

static s32          func_ov003_020ae46c(TaskPool* pool, Task* task, void* args, s32 stage);
static void         func_ov003_020ae890(Ov003Element* self);
static void         func_ov003_020ae8b4(Ov003Element* self);
static void         func_ov003_020aedf0(Ov003Element* self);
static void         func_ov003_020aee9c(Ov003Element* self);
static void         func_ov003_020aefc0(Ov003Element* self);
static BattleActor* func_ov003_020af1e0(Ov003Element* self);

static char             data_ov003_020e4c04[24] = "SubTsk_BtlPsy_Element";
static const TaskHandle data_ov003_020dc7ac     = {data_ov003_020e4c04, func_ov003_020ae46c, sizeof(Ov003Element)};

static const Ov003ElementKind data_ov003_020dc7b8[8] = {
    { 0x49, 0x4A, 0x4B, 0x15D, 0x21},
    { 0x49, 0x4A, 0x4B, 0x16E, 0x21},
    { 0x4C, 0x4D, 0x4E, 0x15E, 0x22},
    { 0x4C, 0x4D, 0x4E, 0x16F, 0x23},
    {0x56C, 0x47, 0x48, 0x15F, 0x24},
    {0x56C, 0x47, 0x48, 0x15F, 0x25},
    {0x56C, 0x47, 0x48, 0x15F, 0x26},
    {0x56C, 0x47, 0x48, 0x15F, 0x27},
};

s32 func_ov003_020ae414(Ov003EffectState* effect, s32 mode, s32 kind, s16 x, s16 y, s32 angle, s32 speed, s32 scale,
                        s32 uses) {
    Ov003ElementArgs args;
    args.effect = effect;
    args.mode   = mode;
    args.kind   = kind;
    args.x      = x;
    args.y      = y;
    args.angle  = angle;
    args.speed  = speed;
    args.scale  = scale;
    args.uses   = uses;
    return func_ov003_020a6000(&data_ov003_020dc7ac, &args);
}

// Nonmatching: typed table merges retail interior symbols.
static s32 func_ov003_020ae46c(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003ElementArgs* arg = args;
    Ov003Element*     self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect          = arg->effect;
            self->mode            = arg->mode;
            self->kind            = arg->kind;
            self->position.x      = BtlStage_ToWorldX(0, arg->x);
            self->position.z      = 0x1000;
            self->position.y      = BtlStage_ToWorldY(0, arg->y, self->position.z);
            self->angle           = arg->angle;
            self->speed           = arg->speed;
            self->scale           = arg->scale;
            self->remainingUses   = arg->uses;
            self->target          = NULL;
            self->remainingTicks  = 360;
            self->opacity         = 0x1F000;
            self->radius          = FX_Mul(self->scale, 0x10000);
            self->firstTick       = 1;
            self->deflectionAngle = 0;
            self->deflectionSpeed = 0;
            self->velocity.x      = FX_Mul(self->speed, COS2(FX_SinCosTable_, self->angle));
            self->velocity.y      = FX_Mul(self->speed, SIN2(FX_SinCosTable_, self->angle));
            switch (self->mode) {
                case 2:
                    self->target = func_ov003_020a6438();
                    break;
                case 5:
                    self->target = func_ov003_020af1e0(self);
                    break;
                case 3:
                    self->angle     = RNG_Next(4) * 0x4000;
                    self->turnTimer = 0;
                    break;
                case 4:
                    self->paused    = 0;
                    self->turnTimer = 0;
                    break;
            }
            CombatPsyAnim_Load(&self->sprite, 2, 1, data_ov003_020dc7b8[self->kind].animation, 1);
            self->effect->unk_0A++;
            func_ov003_020a5aa4(self->effect);
            func_ov003_020a5a94(self->effect);
            s32 sound = data_ov003_020dc7b8[self->kind].startSound;
            if (sound != 0x56C && sound < 0x56C)
                BtlSe_PlayAtMainX(sound, self->position.x);
            return 1;
        case 1: {
            Ov003Element* self = task->data;
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                s32 sound = data_ov003_020dc7b8[self->kind].loopSound;
                if (sound < 0x56C)
                    BtlSe_PlayAtMainX(sound, self->position.x);
                BtlSprite_Restart(&self->sprite);
            }
            BtlSprite_Update(&self->sprite);
            if (self->firstTick) {
                self->firstTick = 0;
                return 1;
            }
            if (self->remainingUses > 0) {
                func_ov003_020ae890(self);
                func_ov003_020ae8b4(self);
                if (self->mode != 2 && self->mode != 5)
                    func_ov003_020aee9c(self);
                func_ov003_020aefc0(self);
                func_ov003_020aedf0(self);
                if (--self->remainingTicks == 0)
                    self->remainingUses = 0;
            } else {
                self->opacity -= 0x1000;
                if (self->opacity <= 0)
                    return 0;
            }
            return 1;
        }
        case 2: {
            self  = task->data;
            s32 x = BtlStage_ToScreenX(0, self->position.x);
            s32 y = BtlStage_ToScreenY(0, self->position.y, self->position.z);
            BtlSprite_SetPosition(&self->sprite, x, y);
            s32 scale                         = self->scale;
            self->affine.rotation             = 0;
            self->affine.scaleX               = (self->sprite.flags & BTL_SPRITE_FLAG_FLIPPED) ? -scale : scale;
            self->affine.scaleY               = scale;
            self->affine.unk_0C               = 0;
            self->affine.unk_0E               = 0;
            self->sprite.affine               = &self->affine;
            self->sprite.sprite.unk_0A.unk_00 = 1;
            if (self->remainingTicks > 0 && self->remainingTicks < 36 && self->remainingTicks % 18 > 10)
                return 1;
            BtlSprite_RenderTranslucent(&self->sprite, self->opacity >> 12);
            return 1;
        }
        case 3:
            self = task->data;
            BtlSprite_Release(&self->sprite);
            self->effect->unk_0A--;
            return 1;
    }
}

static void func_ov003_020ae890(Ov003Element* self) {
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
}

static void func_ov003_020ae8b4(Ov003Element* self) {
    self->position.x += FX_Mul(self->deflectionSpeed, COS2(FX_SinCosTable_, self->deflectionAngle));
    self->position.y += FX_Mul(self->deflectionSpeed, SIN2(FX_SinCosTable_, self->deflectionAngle));
    if (self->deflectionSpeed > 0) {
        self->deflectionSpeed -= FX_Mul(self->scale, 0x19A);
        if (self->deflectionSpeed < 0)
            self->deflectionSpeed = 0;
    }
}

// Nonmatching: Preview temporaries and register allocation differ.
static void func_ov003_020ae980(Ov003Element* self) {
    if (self->turnTimer == 0) {
        u16 angle;
        for (;;) {
            angle  = self->angle + (RNG_Next(3) + 1) * 0x4000;
            s32 dy = FX_Mul(FX_Mul(self->speed, FX_SinIdx(angle)), 0x4000);
            s32 dx = FX_Mul(FX_Mul(self->speed, FX_CosIdx(angle)), 0x4000);
            s32 y  = self->position.y;
            s32 x  = BtlStage_ToScreenX(0, self->position.x + dx);
            y      = BtlStage_ToScreenY(0, y + dy, self->position.z);
            if (x >= 0 && x <= 256 && y >= 0 && y <= 192)
                break;
        }
        self->angle      = angle;
        self->turnTimer  = 4;
        self->velocity.x = FX_Mul(self->speed, COS2(FX_SinCosTable_, self->angle));
        self->velocity.y = FX_Mul(self->speed, SIN2(FX_SinCosTable_, self->angle));
    } else {
        self->turnTimer--;
    }
}

// Nonmatching: Preview temporaries and register allocation differ.
static void func_ov003_020aeb0c(Ov003Element* self) {
    if (self->turnTimer == 0) {
        self->paused = !self->paused;
        if (self->paused) {
            self->turnTimer  = RNG_Next(11) + 20;
            self->velocity.x = 0;
            self->velocity.y = 0;
            return;
        }
        u16 angle;
        s32 frames;
        for (;;) {
            angle  = RNG_Next(0x10001);
            frames = RNG_Next(14) + 3;
            s32 dy = FX_Mul(FX_Mul(self->speed, FX_SinIdx(angle)), frames * 0x1000);
            s32 dx = FX_Mul(FX_Mul(self->speed, FX_CosIdx(angle)), frames * 0x1000);
            s32 y  = self->position.y;
            s32 x  = BtlStage_ToScreenX(0, self->position.x + dx);
            y      = BtlStage_ToScreenY(0, y + dy, self->position.z);
            if (x >= 0 && x <= 256 && y >= 0 && y <= 192)
                break;
        }
        self->angle      = angle;
        self->turnTimer  = frames;
        self->velocity.x = FX_Mul(self->speed, COS2(FX_SinCosTable_, self->angle));
        self->velocity.y = FX_Mul(self->speed, SIN2(FX_SinCosTable_, self->angle));
    } else {
        self->turnTimer--;
    }
}

// Nonmatching: Vector temporary layout and load scheduling differ.
static void func_ov003_020aecbc(Ov003Element* self) {
    if (self->turnTimer != 0) {
        self->turnTimer--;
        return;
    }
    self->turnTimer             = 3;
    Vec          current        = {self->position.x, self->position.y, self->position.z};
    BattleActor* target         = self->target;
    Vec          targetPosition = {target->position.x, target->position.y, target->position.z - target->centerHeight * 4096};
    Vec          toward;
    Vec_Sub(&targetPosition, &current, &toward);
    Vec_Normalize(&toward, &toward);
    Vec velocity = {self->velocity.x, self->velocity.y, 0};
    Vec_Normalize(&velocity, &velocity);
    Vec_Add(&velocity, &toward, &toward);
    Vec_Normalize(&toward, &toward);
    self->velocity.x = FX_Mul(self->speed, toward.x);
    self->velocity.y = FX_Mul(self->speed, toward.y);
    self->position.z += FX_Mul(self->speed, toward.z);
}

// Nonmatching: Empty mode branches use different return sequences.
static void func_ov003_020aedf0(Ov003Element* self) {
    switch (self->mode) {
        case 2:
            if (self->target != NULL && !(self->target->flags & ACTOR_FLAG_DEFEATED))
                func_ov003_020aecbc(self);
            else
                self->remainingUses = 0;
            break;
        case 3:
            func_ov003_020ae980(self);
            break;
        case 4:
            func_ov003_020aeb0c(self);
            break;
        case 5:
            if (self->target != NULL && !(self->target->flags & ACTOR_FLAG_DEFEATED)) {
                func_ov003_020aecbc(self);
            } else {
                self->target = func_ov003_020af1e0(self);
                if (self->target == NULL)
                    self->remainingUses = 0;
            }
            break;
    }
}

static void func_ov003_020aee9c(Ov003Element* self) {
    s32 low  = BtlStage_ToWorldX(0, 0) + self->radius;
    s32 high = BtlStage_ToWorldX(0, 256) - self->radius;
    if (self->position.x < low) {
        self->position.x      = low;
        self->velocity.x      = -self->velocity.x;
        self->deflectionSpeed = 0;
        self->remainingUses--;
    } else if (self->position.x > high) {
        self->position.x      = high;
        self->velocity.x      = -self->velocity.x;
        self->deflectionSpeed = 0;
        self->remainingUses--;
    }
    low  = BtlStage_ToWorldY(0, 0, self->position.z) + self->radius;
    high = BtlStage_ToWorldY(0, 192, self->position.z) - self->radius;
    if (self->position.y < low) {
        self->position.y      = low;
        self->velocity.y      = -self->velocity.y;
        self->deflectionSpeed = 0;
        self->remainingUses--;
    } else if (self->position.y > high) {
        self->position.y      = high;
        self->velocity.y      = -self->velocity.y;
        self->deflectionSpeed = 0;
        self->remainingUses--;
    }
}

// Nonmatching: Unified descriptor table relocations and register allocation differ.
static void func_ov003_020aefc0(Ov003Element* self) {
    s32 attackId = data_ov003_020dc7b8[self->kind].attackId;
    s32 hit      = 0;
    if (attackId == 0x16F && func_ov003_020a51c0())
        attackId = 0x170;
    Ov003EnemyActor* unit = BtlList_GetHead(&gBattle->enemiesMain);
    while (unit != NULL) {
        s32 x       = BtlStage_ToScreenX(0, self->position.x) * 4096;
        s32 y       = BtlStage_ToScreenY(0, self->position.y, self->position.z) * 4096;
        s32 targetX = unit->actor.screenX * 4096;
        s32 targetY = (unit->actor.screenY - unit->actor.centerHeight) * 4096;
        if (func_ov003_020a62ec(x, y, targetX, targetY) <= self->radius + (unit->actor.radius / 2) * 4096 &&
            func_ov003_020a6568(&self->effect->streak, (u16)attackId, unit, self->position.x, 0, 0) == 1)
        {
            u32 angle = func_ov003_020a62b8(x, y, targetX, targetY);
            hit       = 1;
            if (self->mode == 0) {
                self->deflectionAngle = (u16)(angle + 0x8000);
                self->deflectionSpeed = 0x3000;
            } else if (self->mode == 1) {
                if (angle <= 0x2000 || angle > 0xE000)
                    self->velocity.x = -self->velocity.x;
                if (angle >= 0x6000 && angle <= 0xA000)
                    self->velocity.x = -self->velocity.x;
                if (angle >= 0x2000 && angle <= 0x6000)
                    self->velocity.y = -self->velocity.y;
                if (angle >= 0xA000 && angle <= 0xE000)
                    self->velocity.y = -self->velocity.y;
            }
            self->remainingUses -= 3;
            break;
        }
        unit = BtlList_GetNext(&unit->node);
    }
    if (hit == 1) {
        s32 sound = data_ov003_020dc7b8[self->kind].hitSound;
        if (sound < 0x56C)
            BtlSe_PlayAtMainX(sound, self->position.x);
    }
}

// Nonmatching: Projection and list setup scheduling differ.
static BattleActor* func_ov003_020af1e0(Ov003Element* self) {
    BattleActor*     best    = NULL;
    s32              nearest = 0x7FFFFFFF;
    s32              x       = BtlStage_ToScreenX(0, self->position.x);
    s32              y       = BtlStage_ToScreenY(0, self->position.y, self->position.z);
    Ov003EnemyActor* unit    = BtlList_GetHead(&gBattle->enemiesMain);
    while (unit != NULL) {
        if (!(unit->actor.flags & ACTOR_FLAG_DEFEATED)) {
            s32 distance = func_ov003_020a632c(x, y, unit->actor.screenX, unit->actor.screenY - unit->actor.centerHeight);
            if (distance < nearest) {
                best    = &unit->actor;
                nearest = distance;
            }
        }
        unit = BtlList_GetNext(&unit->node);
    }
    return best;
}
