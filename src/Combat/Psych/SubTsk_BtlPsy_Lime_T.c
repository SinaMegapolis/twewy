#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "SndMgrSeIdx.h"
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc960;

s32 func_ov003_020b292c(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (func_ov003_020a5b18(effect->state))
        return 0;
    if (!BtlTouch_IsGesture(BTL_TOUCH_GESTURE_TAP))
        return 0;
    Ov003EffectState** arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dc960.taskName);
    *arg                   = effect;
    context->handle        = &data_ov003_020dc960;
    context->arg           = arg;
    context->post          = NULL;
    return 1;
}

// Nonmatching: the initial animation call schedules its sprite address before the final zero argument.
static s32 func_ov003_020b2998(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EffectState** arg = args;
    Ov003Lime*         self;
    switch (stage) {
        default:
        case 0: {
            self                   = task->data;
            BattleTouchWork* touch = &g_CombatGlobal->touchWork;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect  = *arg;
            self->phase   = 0;
            self->entered = 0;
            func_ov003_020a6464(1);
            self->target.z = -0x8000;
            self->target.x = BtlStage_ToWorldX(0, touch->x);
            self->target.y = BtlStage_ToWorldY(0, touch->y, self->target.z);
            BtlStage_ClampToFloor(&self->target.x, &self->target.y, 0, 0);
            self->position.z = -0x20000;
            self->position.x = BtlStage_ToWorldX(0, 0);
            self->position.y = self->target.y;
            BtlStage_ClampToFloor(&self->position.x, &self->position.y, 0, 0);
            self->angle = func_ov003_020a62b8(self->position.x, self->position.z, self->target.x, self->target.z);
            self->speed = self->effect->params[0];
            CombatPsyAnim_Load(&self->sprite, 2, 1, 0xAF, 0);
            BtlSprite_SetAnim(&self->sprite, 0, 0);
            CombatHit_LoadSpark(0, &self->icon, 2);
            func_ov003_020a5aa4(self->effect);
            func_ov003_020a5a94(self->effect);
            func_ov003_020a6494(touch->x);
            self->effect->unk_0A++;
            CombatSe_PlayPanned(SEIDX_SE_PSY_RHYME_FLY01_01, touch->x);
            return 1;
        }
        case 1:
            self = task->data;
            if (self->phase == 0 || self->phase == 2) {
                self->position.x += FX_Mul(self->speed, COS2(FX_SinCosTable_, self->angle));
                self->position.z += FX_Mul(self->speed, SIN2(FX_SinCosTable_, self->angle));
            }
            switch (self->phase) {
                case 0:
                    if (!self->entered && BtlStage_ToScreenX(0, self->position.x) > 0 &&
                        BtlStage_ToScreenY(0, self->position.y, self->position.z) > 0)
                    {
                        CombatSe_PlayAtMainX(0xA2, self->position.x);
                        self->entered = 1;
                        break;
                    }
                    if (func_ov003_020a62ec(self->position.x, self->position.z, self->target.x, self->target.z) <= 0x4000)
                        self->phase = 1;
                    break;
                case 1:
                    if (func_ov003_020a6628(&self->effect->streak, 0x150, self->position.x, self->position.y, self->position.z,
                                            0, 0) == 1)
                    {
                        self->phase = 3;
                        BtlSprite_SetAnim(&self->sprite, 2, 1);
                        CombatSe_PlayAtMainX(0xAF, self->position.x);
                        break;
                    }
                    self->phase = 2;
                    self->angle = (u16)(0x10000 - self->angle);
                    BtlSprite_SetAnim(&self->sprite, 1, 0);
                    CombatSe_PlayAtMainX(0xA3, self->position.x);
                    break;
                case 3:
                    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                        BtlSprite_SetAnim(&self->sprite, 3, 1);
                        self->phase = 4;
                    }
                    break;
                case 4:
                    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                        self->phase = 2;
                        self->angle = (u16)(0x10000 - self->angle);
                        BtlSprite_SetAnim(&self->sprite, 1, 0);
                        CombatSe_PlayAtMainX(0xA3, self->position.x);
                    }
                    break;
                case 2:
                    if (BtlStage_ToScreenX(0, self->position.x) > 0x140 ||
                        BtlStage_ToScreenY(0, self->position.y, self->position.z) < 0)
                        return 0;
                    break;
            }
            BtlSprite_Update(&self->sprite);
            BtlSprite_Update(&self->icon);
            return 1;
        case 2: {
            self  = task->data;
            s32 y = BtlStage_ToScreenY(0, self->position.y, self->position.z);
            s32 x = BtlStage_ToScreenX(0, self->position.x);
            BtlSprite_SetPosition(&self->sprite, x, y);
            BtlSprite_Render(&self->sprite);
            CombatHit_DrawSpark(&self->icon, self->position.x, self->position.y);
            return 1;
        }
        case 3:
            self = task->data;
            func_ov003_020a6464(0);
            BtlSprite_Release(&self->sprite);
            BtlSprite_Release(&self->icon);
            self->effect->unk_0A--;
            return 1;
    }
}

static char             data_ov003_020e4d98[24] = "SubTsk_BtlPsy_Lime_T";
static const TaskHandle data_ov003_020dc960     = {data_ov003_020e4d98, func_ov003_020b2998, sizeof(Ov003Lime)};
