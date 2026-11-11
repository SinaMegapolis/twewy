#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc77c;

s32 func_ov003_020acd6c(TaskPool* pool, Task* task, void* args) {
    Ov003Blur* self = task->data;
    MI_CpuSet(self, 0, sizeof(*self));
    self->active       = 1;
    self->captured     = 0;
    self->fixedOpacity = 0x15000;
    CombatPsyAnim_Load(&self->sprite, 2, 1, 0xAA, 0);
    return 1;
}

s32 func_ov003_020acdc0(TaskPool* pool, Task* task, void* args) {
    Ov003Blur* self = task->data;
    self->fixedOpacity -= 0x1800;
    self->opacity = self->fixedOpacity >> 12;
    if (self->opacity < 0)
        self->active = 0;
    if (self->captured == 0) {
        BattleActor* player = g_CombatGlobal->player;
        self->position.x    = player->position.x;
        self->position.y    = player->position.y;
        self->position.z    = player->position.z;
        BtlSprite_SetFlip(&self->sprite, player->isFlipped ? 1 : 0);
        self->captured = 1;
    }
    BtlSprite_Update(&self->sprite);
    return self->active;
}

s32 func_ov003_020ace54(TaskPool* pool, Task* task, void* args) {
    Ov003Blur* self = task->data;
    if (self->opacity > 0) {
        s32 x = BtlStage_ToScreenX(0, self->position.x);
        s32 y = BtlStage_ToScreenY(0, self->position.y, self->position.z);
        BtlSprite_SetPosition(&self->sprite, x, y);
        BtlSprite_SetSortKey(&self->sprite, self->position.y + 0x1F4000);
        BtlSprite_RenderTranslucent(&self->sprite, self->opacity);
    }
    return 1;
}

s32 func_ov003_020acebc(TaskPool* pool, Task* task, void* args) {
    Ov003Blur* self = task->data;
    BtlSprite_Release(&self->sprite);
    return 1;
}

// Nonmatching: the compiler uses the saved volatile-register slot for the one-word argument.
s32 func_ov003_020aced0(Ov003EffectState* effect) {
    Ov003EffectState* arg = effect;
    return func_ov003_020a6000(&data_ov003_020dc77c, &arg);
}

void func_ov003_020acef4(s32 taskId) {
    Ov003DragAttack* self = func_ov003_020a6084(taskId);
    if (self != NULL)
        self->phase = 2;
}

void func_ov003_020acf0c(s32 taskId, u16 angle) {
    Ov003DragAttack* self = func_ov003_020a6084(taskId);
    if (self != NULL)
        self->angle = angle;
}

static s32 func_ov003_020acf24(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EffectState** arg = args;
    Ov003DragAttack*   self;
    switch (stage) {
        default:
        case 0: {
            self                = task->data;
            BattleActor* player = g_CombatGlobal->player;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect      = *arg;
            self->phase       = 0;
            self->opacity     = 0xF000;
            self->opacityStep = 0x3000;
            self->angle       = 0;
            s32 animation;
            switch (self->effect->state) {
                case 0x1C:
                    animation      = self->effect->index + 0xA6;
                    self->attackId = 0x14B;
                    break;
                case 0x1D:
                    animation      = self->effect->index + 0xA8;
                    self->attackId = 0x14C;
                    break;
                default:
                    return 0;
            }
            CombatPsyAnim_Load(&self->sprite, 2, 1, animation, 0);
            BtlSe_PlayAtMainX(0x64, player->position.x);
            func_ov003_02087804(0);
            return 1;
        }
        case 1: {
            Ov003DragAttack* self = task->data;
            Ov003EnemyActor* unit;
            BattleActor*     player = g_CombatGlobal->player;
            BtlSprite_Update(&self->sprite);
            switch (self->phase) {
                case 0:
                    self->opacity += self->opacityStep;
                    if (self->opacity > 0x1F000) {
                        self->opacity = 0x1F000;
                        self->phase   = 1;
                    }
                    break;
                case 1:
                    switch (self->effect->state) {
                        case 0x1C:
                            if (func_ov003_020a6628(&self->effect->streak, (u16)self->attackId, player->position.x,
                                                    player->position.y, player->position.z, 0, 0) == 1)
                                BtlSe_PlayAtMainX(0x65, player->position.x);
                            break;
                        case 0x1D: {
                            unit = BtlList_GetHead(&g_CombatGlobal->unk3D8A4);
                            while (unit != NULL) {
                                s32 dz = player->position.z - unit->actor.position.z;
                                if (dz < 0)
                                    dz = -dz;
                                if (dz <= 0x3C000 &&
                                    func_ov003_020a62ec(player->position.x, player->position.y, unit->actor.position.x,
                                                        unit->actor.position.y) <= (unit->actor.radius / 2) * 4096 + 0x18000)
                                {
                                    if (func_ov003_020a6568(&self->effect->streak, (u16)self->attackId, unit,
                                                            player->position.x, 0, 0) == 1)
                                    {
                                        s32 angle = func_ov003_020a62b8(player->position.x, player->position.y,
                                                                        unit->actor.position.x, unit->actor.position.y);
                                        func_ov003_020a61e4(unit, 1, self->attackId, &self->effect->streak.args, angle,
                                                            self->effect->params[0], self->effect->params[1]);
                                    }
                                }
                                unit = BtlList_GetNext(&unit->node);
                            }
                            break;
                        }
                    }
                    break;
                case 2:
                    self->opacity -= self->opacityStep;
                    if (self->opacity < 0) {
                        self->opacity = 0;
                        return 0;
                    }
                    break;
            }
            return 1;
        }
        case 2: {
            self                = task->data;
            BattleActor* player = g_CombatGlobal->player;
            BtlSprite_SetPosition(&self->sprite, player->screenX, (s16)(player->screenY - 24));
            BtlSprite_SetAffineTransform(&self->sprite, self->angle, 0x1000, 0x1000, 1);
            if (self->opacity >= 0x1000)
                BtlSprite_RenderTranslucent(&self->sprite, self->opacity >> 12);
            return 1;
        }
        case 3:
            self = task->data;
            BtlSprite_Release(&self->sprite);
            return 1;
    }
}

static char             data_ov003_020e4ba0[32] = "Tsk_BtlPsy_DragAttack_Effect";
static const TaskHandle data_ov003_020dc77c     = {data_ov003_020e4ba0, func_ov003_020acf24, sizeof(Ov003DragAttack)};
