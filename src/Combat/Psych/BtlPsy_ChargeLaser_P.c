#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Engine/Math/Random.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc754;

// Nonmatching: initialize the unused actor slots rather than copying indeterminate pointers.
s32 func_ov003_020ac634(Ov003EffectContext* context, Ov003EffectState* effect) {
    Ov003ChargeLaserArgs args = {0};
    args.mode                 = 0;
    args.effect               = effect;
    if (!func_ov003_020a63e4())
        return 0;
    if (func_ov003_020a5b18(0x3A))
        return 0;
    Ov003ChargeLaserArgs* arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dc754.taskName);
    *arg                      = args;
    context->handle           = &data_ov003_020dc754;
    context->arg              = arg;
    context->post             = NULL;
    return 1;
}

s32 func_ov003_020ac6bc(Ov003EffectContext* context, Ov003EffectState* effect) {
    Ov003ChargeLaserArgs args;
    args.effect = effect;
    args.mode   = 1;
    if (!func_ov003_020a63e4())
        return 0;
    if (func_ov003_020a5b18(0x3B))
        return 0;
    if (!func_ov003_020a5ac4(args.actors, 2, effect, 1))
        return 0;
    Ov003ChargeLaserArgs* arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dc754.taskName);
    *arg                      = args;
    context->handle           = &data_ov003_020dc754;
    context->arg              = arg;
    context->post             = NULL;
    return 1;
}

static s32 func_ov003_020ac76c(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003ChargeLaserArgs* arg = args;
    Ov003ChargeLaser*     self;
    switch (stage) {
        default:
        case 0: {
            self = task->data;
            BattleTouch* touch;
            BattleActor* player = func_ov003_020a6438();
            touch               = func_ov003_020a62a0();
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect = arg->effect;
            self->mode   = arg->mode;
            MI_CpuCopyU8(arg->actors, self->actors, sizeof(self->actors));
            self->animationKind = 0;
            self->soundPlayed   = 0;
            if (self->mode == 0 && self->effect->index > 0)
                self->animationKind = 1;
            self->effect->unk_0A++;
            func_ov003_020a6464(1);
            func_ov003_020a6494(touch->x);
            self->facingLeft = touch->x < player->screenX ? 1 : 0;
            if (self->facingLeft) {
                self->x       = player->screenX - 24;
                self->targetX = self->x - 100;
            } else {
                self->x       = player->screenX + 24;
                self->targetX = self->x + 100;
            }
            self->y             = player->screenY - 39;
            self->phase         = 0;
            self->timer         = 50;
            self->particleTimer = 4;
            return 1;
        }
        case 1: {
            Ov003ChargeLaser* self = task->data;
            if (func_ov003_020a63a8(self->effect))
                return 0;
            switch (self->phase) {
                case 0:
                    if (self->mode == 0)
                        func_ov003_020a5a94(self->effect);
                    else {
                        func_ov003_020a40b0(self->actors[0]);
                        func_ov003_020a40b0(self->actors[1]);
                    }
                    if (!self->soundPlayed) {
                        BtlSe_PlayAtMainX(0x9F, func_ov003_020a6438()->position.x);
                        self->soundPlayed = 1;
                    }
                    if (self->particleTimer == 0) {
                        Ov003PsychEffectArgs effect;
                        func_ov003_020a8908(&effect, self->x, self->y, 2, self->animationKind + 0xF0);
                        func_ov003_020a899c(&effect, 0);
                        func_ov003_020a89b0(&effect, 30);
                        func_ov003_020a89c8(&effect, RNG_Next(16) << 12, 0x1000, 0x1000);
                        func_ov003_020a88a8(&effect);
                        self->particleTimer = 4;
                    }
                    self->particleTimer--;
                    if (!func_ov003_020a63e4()) {
                        self->phase = 2;
                        self->timer = 12;
                        break;
                    }
                    if (--self->timer == 0) {
                        self->phase = 1;
                        self->timer = 5;
                        if (self->mode == 0)
                            func_ov003_020a5aa4(self->effect);
                        else {
                            func_ov003_020a40bc(self->actors[0]);
                            func_ov003_020a40bc(self->actors[1]);
                        }
                        CombatPsyAnim_Load(&self->sprite, 2, 1, self->animationKind + 0xEC, 1);
                        BtlSe_PlayAtMainX(0xA0, func_ov003_020a6438()->position.x);
                    }
                    break;
                case 1:
                    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                        self->phase = 2;
                        self->timer = 12;
                    } else if (self->timer != 0) {
                        self->timer--;
                    } else {
                        func_ov003_020acc60(self);
                    }
                    break;
                case 2:
                    if (--self->timer == 0)
                        return 0;
                    break;
            }
            if (Sprite_HasAnimation(&self->sprite.sprite) && !SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
                BtlSprite_Update(&self->sprite);
            return 1;
        }
        case 2:
            self = task->data;
            if (Sprite_HasAnimation(&self->sprite.sprite) && !SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                BtlSprite_SetPosition(&self->sprite, self->x, self->y);
                BtlSprite_SetSortKey(&self->sprite, 0x384000);
                BtlSprite_SetFlip(&self->sprite, self->facingLeft);
                BtlSprite_Render(&self->sprite);
            }
            return 1;
        case 3:
            self = task->data;
            if (Sprite_HasAnimation(&self->sprite.sprite))
                BtlSprite_Release(&self->sprite);
            self->effect->unk_0A--;
            func_ov003_020a6464(0);
            return 1;
    }
}

static char             data_ov003_020e4b74[28] = "Tsk_BtlPsy_ChargeLaser_P";
static const TaskHandle data_ov003_020dc754     = {data_ov003_020e4b74, func_ov003_020ac76c, sizeof(Ov003ChargeLaser)};
