#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc930;
static const s32        data_ov003_020dc93c[3];
static void             func_ov003_020b26cc(Ov003IcePillarR* self);

/// The three ice pillars spawned by IcePillar_R, plus the shared task state.
///
/// `count` pillars rise around the touch point: `x[i]`/`y[i]` are the beam
/// centres, `xOffset` sways them sideways and flips sign every step. `z` drops
/// the attack volume onto the floor over the last phase.

s32 func_ov003_020b1fd4(Ov003EffectContext* context, Ov003EffectState* effect) {
    Ov003EffectState** arg;

    if (!BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP))
        return 0;
    arg             = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dc930.taskName);
    *arg            = effect;
    context->handle = &data_ov003_020dc930;
    context->arg    = arg;
    context->post   = NULL;
    return 1;
}

// objdiff flags the four intra-TU `bl func_ov003_020b26cc` relocations only because this object
// emits one .text section per function while the delinked target keeps a single one; the encoded
// instructions (and their post-link displacement) are identical.
static s32 func_ov003_020b202c(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EffectState** arg = args;
    switch (stage) {
        default:
        case 0: {
            Ov003IcePillarR* self  = task->data;
            BattleTouchWork* touch = func_ov003_020a62a0();
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect = *arg;
            self->count  = (u8)self->effect->params[0];
            self->x[0]   = BtlStage_ToWorldX(0, touch->x);
            self->y[0]   = BtlStage_ToWorldY(0, touch->y, 0);
            BtlStage_ClampXY(&self->x[0], &self->y[0], 0, 0);
            self->z       = 0;
            self->xOffset = 0x2000;
            self->x[1]    = self->x[0] + 0x20000;
            self->y[1]    = self->y[0];
            BtlStage_ClampXY(&self->x[1], &self->y[1], 0, 0);
            self->x[2] = self->x[0] - 0x20000;
            self->y[2] = self->y[0];
            BtlStage_ClampXY(&self->x[2], &self->y[2], 0, 0);
            func_ov003_020a6464(1);
            func_ov003_020a6494(touch->x);
            func_ov003_020a5aa4(self->effect);
            func_ov003_020a5a94(self->effect);
            self->phase = 0;
            self->timer = 0x3C;
            CombatPsyAnim_Load(&self->spriteA, 2, 1, data_ov003_020dc93c[self->effect->index], 1);
            BtlSprite_SetAnim(&self->spriteA, 2, 1);
            CombatPsyAnim_Load(&self->spriteB, 2, 1, data_ov003_020dc93c[self->effect->index], 0);
            BtlSprite_SetAnim(&self->spriteB, 0, 0);
            func_ov003_02087804(0);
            CombatSe_PlayAtMainX(0x87, self->x[0]);
            return 1;
        }
        case 1: {
            Ov003IcePillarR* self = task->data;
            s32              i;
            BtlSprite_Update(&self->spriteA);
            BtlSprite_Update(&self->spriteB);
            if (Sprite_HasAnimation(&self->spriteC.sprite)) {
                if (SpriteMgr_IsAnimationFinished(&self->spriteC.sprite))
                    BtlSprite_Release(&self->spriteC);
                else
                    BtlSprite_Update(&self->spriteC);
            }
            if (func_ov003_020a63a8(self->effect))
                return 0;
            for (i = 0; i < self->count; i++)
                func_ov003_020a6628(&self->effect->streak, 0x149, self->x[i], self->y[i], self->z, 0, 0);
            switch (self->phase) {
                case 0:
                    if (SpriteMgr_IsAnimationFinished(&self->spriteA.sprite)) {
                        if (--self->timer == 0)
                            return 0;
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP)) {
                            self->phase++;
                            self->timer = 0x3C;
                            BtlSprite_SetAnim(&self->spriteA, 3, 1);
                            CombatSe_PlayAtMainX(0x87, self->x[0]);
                        }
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP))
                            func_ov003_020b26cc(self);
                    }
                    break;
                case 1:
                    if (SpriteMgr_IsAnimationFinished(&self->spriteA.sprite)) {
                        if (--self->timer == 0)
                            return 0;
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP)) {
                            self->phase++;
                            self->timer = 0x3C;
                            BtlSprite_SetAnim(&self->spriteA, 4, 1);
                            CombatSe_PlayAtMainX(0x87, self->x[0]);
                        }
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP))
                            func_ov003_020b26cc(self);
                    }
                    break;
                case 2:
                    if (SpriteMgr_IsAnimationFinished(&self->spriteA.sprite)) {
                        if (--self->timer == 0)
                            return 0;
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP)) {
                            self->phase++;
                            self->timer = 0x3C;
                            BtlSprite_SetAnim(&self->spriteA, 5, 1);
                            CombatSe_PlayAtMainX(0x87, self->x[0]);
                        }
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP))
                            func_ov003_020b26cc(self);
                    }
                    break;
                case 3:
                    if (SpriteMgr_IsAnimationFinished(&self->spriteA.sprite)) {
                        if (--self->timer == 0)
                            return 0;
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP)) {
                            self->phase++;
                            self->timer = 0x3C;
                            BtlSprite_SetAnim(&self->spriteA, 6, 1);
                            BtlSprite_SetAnim(&self->spriteB, 1, 1);
                            CombatSe_PlayAtMainX(0x88, self->x[0]);
                            CombatPsyAnim_Load(&self->spriteC, 2, 1, data_ov003_020dc93c[self->effect->index], 1);
                            BtlSprite_SetAnim(&self->spriteC, 7, 1);
                        }
                        if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP))
                            func_ov003_020b26cc(self);
                    }
                    break;
                case 4:
                    // The retail code discards the animation result here and only advances the fall.
                    SpriteMgr_IsAnimationFinished(&self->spriteA.sprite);
                    self->z -= 0x7000;
                    if (self->z < -0xC8000)
                        return 0;
                    break;
            }
            return 1;
        }
        case 2: {
            Ov003IcePillarR* self = task->data;
            s32              i;
            for (i = 0; i < self->count; i++) {
                s32 x = BtlStage_ToScreenX(0, self->x[i]);
                s32 y = BtlStage_ToScreenY(0, self->y[i], 0);
                // The mid pillar renders only while the effect has not finished.
                if (self->phase >= 0 && !SpriteMgr_IsAnimationFinished(&self->spriteB.sprite)) {
                    BtlSprite_SetPosition(&self->spriteB, x, y);
                    BtlSprite_SetSortKey(&self->spriteB, self->y[i] + 0x1F6000);
                    BtlSprite_Render(&self->spriteB);
                }
                if (Sprite_HasAnimation(&self->spriteC.sprite)) {
                    BtlSprite_SetPosition(&self->spriteC, x, y);
                    BtlSprite_SetSortKey(&self->spriteC, self->y[i] + 0x1F5000);
                    BtlSprite_Render(&self->spriteC);
                }
                x = BtlStage_ToScreenX(0, self->x[i]);
                y = BtlStage_ToScreenY(0, self->y[i], self->z);
                BtlSprite_SetPosition(&self->spriteA, x, y);
                BtlSprite_SetSortKey(&self->spriteA, self->y[i] + 0x1F4000);
                BtlSprite_Render(&self->spriteA);
            }
            return 1;
        }
        case 3: {
            Ov003IcePillarR* self = task->data;
            BtlSprite_Release(&self->spriteB);
            BtlSprite_Release(&self->spriteA);
            if (Sprite_HasAnimation(&self->spriteC.sprite))
                BtlSprite_Release(&self->spriteC);
            func_ov003_020a6464(0);
            return 1;
        }
    }
}

/// Sways every pillar centre sideways and flips the sway direction for the
/// next step, so the three beams weave around the touch point.
static void func_ov003_020b26cc(Ov003IcePillarR* self) {
    s32 i;
    for (i = 0; i < self->count; i++)
        self->x[i] += self->xOffset;
    self->xOffset = -self->xOffset;
}

static char             data_ov003_020e4d68[0x18] = "Tsk_BtlPsy_IcePillar_R";
static const TaskHandle data_ov003_020dc930       = {data_ov003_020e4d68, func_ov003_020b202c, sizeof(Ov003IcePillarR)};
static const s32        data_ov003_020dc93c[3]    = {0x8D, 0x8D, 0x8E};
