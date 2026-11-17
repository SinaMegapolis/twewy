#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Engine/Math/Random.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc718;
static s32              func_ov003_020abdb8(Ov003ObsBomb* self);

void func_ov003_020abb34(Ov003EffectState* effect, Ov003EnemyActor* unit) {
    Ov003PsychUnitArgs args;
    args.effect = effect;
    args.unit   = unit;
    func_ov003_020a6000(&data_ov003_020dc718, &args);
}

static s32 func_ov003_020abb5c(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003PsychUnitArgs* arg = args;
    Ov003ObsBomb*       self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->args.effect = arg->effect;
            self->args.unit   = arg->unit;
            self->phase       = 0;
            CombatPsyAnim_Load(&self->sprite, 2, 1, 0x9F, 1);
            return 1;
        case 1: {
            Ov003ObsBomb* self = task->data;
            switch (self->phase) {
                case 0:
                    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                        self->phase = 1;
                        self->timer = 0;
                        CombatPsyAnim_SetAnim(&self->sprite, 0xA0, 1);
                        BtlSe_PlayAtMainX(0x43, self->args.unit->actor.position.x);
                    }
                    break;
                case 1:
                    s32 limit = self->args.effect->params[0];
                    self->timer++;
                    if (self->timer == limit)
                        return 0;
                    if ((self->args.unit->actor.flags & ACTOR_FLAG_INTANGIBLE) ||
                        (self->args.unit->actor.flags & ACTOR_FLAG_DEFEATED))
                        return 0;
                    if (func_ov003_020abdb8(self)) {
                        Ov003PsychEffectArgs effect;
                        func_ov003_020a88c0(&effect, self->args.unit->actor.position.x, self->args.unit->actor.position.y,
                                            self->args.unit->actor.position.z, 2, 0xA1);
                        func_ov003_020a899c(&effect, 0);
                        func_ov003_020a88a8(&effect);
                        func_ov003_020a61e4(self->args.unit, 0, 10, &self->args.effect->streak.args, RNG_Next(0xFFFF), 0x7000,
                                            -0x2000);
                        BtlSe_PlayAtMainX(0x44, self->args.unit->actor.position.x);
                        return 0;
                    }
                    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                        BtlSe_PlayAtMainX(0x43, self->args.unit->actor.position.x);
                        BtlSprite_Restart(&self->sprite);
                    }
                    break;
            }
            BtlSprite_Update(&self->sprite);
            return 1;
        }
        case 2: {
            Ov003ObsBomb*    self = task->data;
            Ov003EnemyActor* unit = self->args.unit;
            BtlSprite_SetPosition(&self->sprite, unit->actor.screenX, (s16)(unit->actor.screenY - unit->actor.centerHeight));
            BtlSprite_SetSortKey(&self->sprite, unit->actor.position.y + 0x1F4000);
            BtlSprite_Render(&self->sprite);
            return 1;
        }
        case 3:
            self = task->data;
            BtlSprite_Release(&self->sprite);
            return 1;
    }
}

/// Adjusts the contact attack's dimensions to the selected object's dimensions.
static s32 func_ov003_020abdb8(Ov003ObsBomb* self) {
    BattleAttackInfo info  = *BtlHit_GetAttackInfo(0x190);
    s16              depth = self->args.unit->actor.height;
    s16              width = self->args.unit->actor.radius;
    info.halfWidth         = width;
    info.halfHeight        = width / 2;
    info.halfDepth         = depth;
    return BtlHit_AttackAt(&info, &self->args.effect->streak.args, self->args.unit->actor.position.x,
                           self->args.unit->actor.position.y, self->args.unit->actor.position.z) == BTL_HIT_RESULT_LANDED;
}

static char             data_ov003_020e4af8[24] = "SubTsk_BtlPsy_ObsBomb";
static const TaskHandle data_ov003_020dc718     = {data_ov003_020e4af8, func_ov003_020abb5c, sizeof(Ov003ObsBomb)};
