#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc9a8;

s32 func_ov003_020b4048(Ov003EffectState* effect, s32 x, s32 y, s32 z, s32 vx, s32 vy, s32 vz, s32 animation, s32 trail) {
    Ov003BrokenMeteorArgs args;
    args.effect     = effect;
    args.position.x = x;
    args.position.y = y;
    args.position.z = z;
    args.velocity.x = vx;
    args.velocity.y = vy;
    args.velocity.z = vz;
    args.animation  = animation;
    args.trail      = trail;
    return func_ov003_020a6000(&data_ov003_020dc9a8, &args);
}

static s32 func_ov003_020b40a0(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003BrokenMeteorArgs* arg = args;
    Ov003BrokenMeteor*     self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect     = arg->effect;
            self->position.x = arg->position.x;
            self->position.y = arg->position.y;
            self->position.z = self->baseZ = arg->position.z;
            self->velocity.x               = arg->velocity.x;
            self->velocity.y               = arg->velocity.y;
            self->velocity.z               = arg->velocity.z;
            self->trail                    = arg->trail;
            CombatPsyAnim_Load(&self->sprite, 2, 1, arg->animation, 1);
            self->loopAnimation = arg->animation - 1;
            CombatHit_LoadSpark(0, &self->icon, 2);
            return 1;
        case 1:
            self = task->data;
            self->position.x += self->velocity.x;
            self->position.y += self->velocity.y;
            self->position.z += self->velocity.z;
            if (self->position.z >= self->baseZ)
                return 0;
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
                CombatPsyAnim_SetAnim(&self->sprite, self->loopAnimation, 0);
            if (self->trail)
                func_ov003_020a6628(&self->effect->streak, 0x14F, self->position.x, self->position.y, self->position.z, 0, 0);
            self->velocity.z += 0x333;
            BtlSprite_Update(&self->sprite);
            BtlSprite_Update(&self->icon);
            return 1;
        case 2: {
            self  = task->data;
            s32 y = CombatStage_ToScreenY(0, self->position.y, self->position.z);
            s32 x = CombatStage_ToScreenX(0, self->position.x);
            BtlSprite_SetPosition(&self->sprite, x, y);
            BtlSprite_SetSortKey(&self->sprite, self->position.y + 0x1F4000);
            BtlSprite_Render(&self->sprite);
            CombatHit_DrawSpark(&self->icon, self->position.x, self->position.y);
            return 1;
        }
        case 3:
            self = task->data;
            BtlSprite_Release(&self->sprite);
            BtlSprite_Release(&self->icon);
            return 1;
    }
}

static char             data_ov003_020e4e34[28] = "SubTsk_BtlPsy_BrokenMeteor";
static const TaskHandle data_ov003_020dc9a8     = {data_ov003_020e4e34, func_ov003_020b40a0, sizeof(Ov003BrokenMeteor)};
