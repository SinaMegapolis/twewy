#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc8d0;
static s32              func_ov003_020b03b4(Ov003EnemyAirFlow* self);
static s32              func_ov003_020b0470(Ov003EnemyAirFlow* self);

s32 func_ov003_020b0188(Ov003EffectState* effect, Ov003EnemyActor* unit, s32* releaseFlag) {
    s32 allowed = func_ov003_020a60ec(unit, 4);
    if (allowed == 0)
        return allowed;
    Ov003EnemyAirFlowArgs args;
    args.effect      = effect;
    args.unit        = unit;
    args.releaseFlag = releaseFlag;
    return func_ov003_020a6000(&data_ov003_020dc8d0, &args);
}

static s32 func_ov003_020b01d8(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EnemyAirFlowArgs* arg = args;
    Ov003EnemyAirFlow*     self;
    switch (stage) {
        default:
        case 0: {
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect          = arg->effect;
            self->unit            = arg->unit;
            self->releaseFlag     = arg->releaseFlag;
            self->phase           = 0;
            self->radius          = 0;
            self->angle           = 0;
            self->zVelocity       = -0x6000;
            self->landed          = 0;
            self->firstTick       = 1;
            Ov003EnemyActor* unit = self->unit;
            self->origin.x        = unit->actor.position.x;
            self->origin.y        = unit->actor.position.y;
            self->origin.z        = unit->actor.position.z;
            func_ov003_020a6164(unit, 4);
            CombatPsyAnim_Load(&self->sprite, 2, 1, 0xBD, 0);
            CombatSe_PlayAtMainX(0x89, unit->actor.position.x);
            return 1;
        }
        case 1: {
            Ov003EnemyAirFlow* self   = task->data;
            Ov003EnemyActor*   unit   = self->unit;
            s32                result = 1;
            if (!self->firstTick && !func_ov003_020a60a8(unit, 4)) {
                result = 0;
                goto done;
            }
            switch (self->phase) {
                case 0:
                    result = func_ov003_020b0470(self);
                    break;
                case 1:
                    result = func_ov003_020b03b4(self);
                    break;
            }
            BtlSprite_Update(&self->sprite);
            self->firstTick = 0;
        done:
            return result;
        }
        case 2: {
            Ov003EnemyAirFlow* self = task->data;
            Ov003EnemyActor*   unit = self->unit;
            if (self->phase == 0) {
                s32 y = BtlStage_ToScreenY(0, unit->actor.position.y, unit->actor.position.z);
                s32 x = BtlStage_ToScreenX(0, unit->actor.position.x);
                BtlSprite_SetPosition(&self->sprite, x, y);
                BtlSprite_SetSortKey(&self->sprite, unit->actor.position.y);
                BtlSprite_Render(&self->sprite);
            }
            return 1;
        }
        case 3: {
            self                  = task->data;
            Ov003EnemyActor* unit = self->unit;
            BtlSprite_Release(&self->sprite);
            func_ov003_020a6198(unit, 4);
            return 1;
        }
    }
}

static s32 func_ov003_020b03b4(Ov003EnemyAirFlow* self) {
    Ov003EnemyActor* unit = self->unit;
    unit->actor.position.z += self->zVelocity;
    self->zVelocity += 0x6000;
    if (unit->actor.position.z < 0)
        return 1;
    unit->actor.position.z = 0;
    Ov003PsychEffectArgs args;
    func_ov003_020a88c0(&args, unit->actor.position.x, unit->actor.position.y, unit->actor.position.z, 2, 0xBE);
    func_ov003_020a88a8(&args);
    func_ov003_020a8768();
    CombatSe_PlayAtMainX(0x8A, unit->actor.position.x);
    self->landed = 1;
    func_ov003_020a6568(&self->effect->streak, 0x166, unit, unit->actor.position.x, 0, 0);
    return 0;
}

static s32 func_ov003_020b0470(Ov003EnemyAirFlow* self) {
    s32 oldX, oldY, oldZ;

    Ov003EnemyActor* unit = self->unit;
    s32              x    = BtlStage_ToScreenX(0, self->origin.x);
    if (x <= 128)
        self->angle = (u16)(self->angle - 0x1000);
    else
        self->angle = (u16)(self->angle + 0x1000);
    self->radius += 0x1000;
    if (self->radius > 0x10000)
        self->radius = 0x10000;
    oldX                   = unit->actor.position.x;
    oldY                   = unit->actor.position.y;
    oldZ                   = unit->actor.position.z;
    unit->actor.position.x = self->origin.x + FX_Mul(self->radius, COS2(FX_SinCosTable_, self->angle));
    unit->actor.position.y = self->origin.y + FX_Mul(self->radius, SIN2(FX_SinCosTable_, self->angle));
    unit->actor.position.z += self->zVelocity;
    x     = BtlStage_ToScreenX(0, unit->actor.position.x);
    s32 y = BtlStage_ToScreenY(0, unit->actor.position.y, unit->actor.position.z);
    if (!func_ov003_020a6374(x, y, 8)) {
        unit->actor.position.x = oldX;
        unit->actor.position.y = oldY;
        unit->actor.position.z = oldZ;
    }
    if (*self->releaseFlag) {
        self->phase     = 1;
        self->zVelocity = 0;
    }
    return 1;
}
static char             data_ov003_020e4cb8[28] = "SubTsk_BtlPsy_EnemyAirFlow";
static const TaskHandle data_ov003_020dc8d0     = {data_ov003_020e4cb8, func_ov003_020b01d8, sizeof(Ov003EnemyAirFlow)};
