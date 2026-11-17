#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dca90;
static Ov003EnemyActor* func_ov003_020b6308(Ov003EffectState* effect);

s32 func_ov003_020b5f74(Ov003EffectContext* context, Ov003EffectState* effect) {
    Ov003EnemyActor* unit = func_ov003_020b6308(effect);
    if (unit == NULL)
        return 0;
    if (!func_ov003_020a60ec(unit, 5))
        return 0;
    Ov003PsychUnitArgs* arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dca90.taskName);
    arg->effect             = effect;
    arg->unit               = unit;
    context->handle         = &data_ov003_020dca90;
    context->arg            = arg;
    context->post           = NULL;
    return 1;
}

static s32 func_ov003_020b5fe4(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003PsychUnitArgs* arg = args;
    Ov003PsychoT*       self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect               = arg->effect;
            self->hit.activated        = 0;
            self->hit.finished         = 0;
            self->hit.effect           = self->effect;
            self->hit.target           = NULL;
            self->hit.targetPosition.x = 0;
            self->hit.targetPosition.y = 0;
            self->hit.targetPosition.z = 0;
            self->hit.refCount         = 0;
            func_ov003_020a5a94(self->effect);
            func_ov003_020a6464(1);
            self->phase = 0;
            self->timer = self->effect->params[0];
            func_ov003_020b4e48(arg->unit, &self->hit);
            func_ov003_020a647c(arg->unit->actor.position.x);
            CombatPsyAnim_Load(&self->sprite, 2, 1, 0x10, 1);
            return 1;
        case 1: {
            Ov003PsychoT* self  = task->data;
            BattleTouch*  touch = func_ov003_020a62a0();
            switch (self->phase) {
                case 0:
                    if (--self->timer == 0) {
                        self->phase = 1;
                        break;
                    }
                    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_TAP)) {
                        func_ov003_020a6494(touch->x);
                        Ov003EnemyActor* unit = func_ov003_020b6308(self->effect);
                        if (unit != NULL) {
                            func_ov003_020b4e48(unit, &self->hit);
                            self->timer = self->effect->params[0];
                        } else {
                            self->hit.target = NULL;
                            if (self->hit.target == NULL) {
                                self->hit.targetPosition.x = BtlStage_ToWorldX(0, touch->x);
                                self->hit.targetPosition.y = BtlStage_ToWorldY(0, touch->y, 0);
                                self->hit.targetPosition.z = -0x10000;
                            }
                            self->hit.activated = 1;
                            self->phase         = 2;
                            BtlSe_PlayAtMainX(0x9E, touch->x);
                        }
                    }
                    break;
                case 1:
                    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_TAP)) {
                        func_ov003_020a6494(touch->x);
                        self->hit.target = NULL;
                        if (self->hit.target == NULL) {
                            self->hit.targetPosition.x = BtlStage_ToWorldX(0, touch->x);
                            self->hit.targetPosition.y = BtlStage_ToWorldY(0, touch->y, 0);
                            self->hit.targetPosition.z = 0;
                        }
                        self->hit.activated = 1;
                        self->phase         = 2;
                        BtlSe_PlayAtMainX(0x9E, touch->x);
                    }
                    break;
                case 2:
                    BtlSprite_Update(&self->sprite);
                    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
                        CombatPsyAnim_SetAnim(&self->sprite, 0x11, 0);
                    if (self->hit.refCount == 0)
                        self->phase = 3;
                    break;
                case 3:
                    return 0;
            }
            if (!func_ov003_020a5aa4(self->effect) || func_ov003_020a63a8(self->effect)) {
                self->hit.finished = 1;
                self->phase        = 3;
            }
            return 1;
        }
        case 2: {
            self = task->data;
            if (self->phase != 2)
                return 1;
            s32 x, y;
            if (self->hit.target != NULL) {
                x = self->hit.target->screenX;
                y = self->hit.target->screenY - self->hit.target->centerHeight;
            } else {
                x = BtlStage_ToScreenX(0, self->hit.targetPosition.x);
                y = BtlStage_ToScreenY(0, self->hit.targetPosition.y, self->hit.targetPosition.z);
            }
            BtlSprite_SetPosition(&self->sprite, (s16)x, (s16)y);
            BtlSprite_Render(&self->sprite);
            return 1;
        }
        case 3:
            self = task->data;
            func_ov003_020a6464(0);
            BtlSprite_Release(&self->sprite);
            return 1;
    }
}

static Ov003EnemyActor* func_ov003_020b6308(Ov003EffectState* effect) {
    Ov003EnemyActor* primary   = (Ov003EnemyActor*)BtlTouch_GetTapped(ACTOR_KIND_OBJECT);
    Ov003EnemyActor* secondary = (Ov003EnemyActor*)BtlTouch_GetTapped(ACTOR_KIND_ENEMY);
    switch (effect->index) {
        case 0:
        case 1:
            secondary = NULL;
            break;
        case 2:
            break;
        default:
            return NULL;
    }
    if (primary != NULL)
        return primary;
    if (secondary != NULL)
        return secondary;
    return NULL;
}

static char             data_ov003_020e4edc[20] = "Tsk_BtlPsy_Psycho_T";
static const TaskHandle data_ov003_020dca90     = {data_ov003_020e4edc, func_ov003_020b5fe4, sizeof(Ov003PsychoT)};
