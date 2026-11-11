#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatEnemyShot.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020e0cbc;

s32 func_ov003_020d23e8(Ov003EnemyShotSource* source, u16 angle, s32 speed, s32 acceleration, u16 angleStep, s32 yScale) {
    Ov003EnemyShotSpiralArgs args;
    MI_CpuSet(&args, 0, sizeof(args));
    args.source       = source;
    args.angle        = angle;
    args.speed        = speed;
    args.acceleration = acceleration;
    args.angleStep    = angleStep;
    args.yScale       = yScale;
    TaskPool* pool;
    if (source->core.owner->base.sprite.sprite.bits_0_1 == 0)
        pool = &g_CombatGlobal->unk_00000;
    else
        pool = &g_CombatGlobal->taskPool;
    return EasyTask_CreateTask(pool, &data_ov003_020e0cbc, NULL, 0, NULL, &args);
}

static s32 func_ov003_020d2484(Ov003EnemyShotSpiral* self, Ov003EnemyShotSpiralArgs* args) {
    const Ov003EnemyShotVisual* visual = args->source->visual;
    const SpriteAnimEntry*      entry  = &visual->entries[visual->entry];
    s32                         engine = args->source->core.owner->base.sprite.sprite.bits_0_1;
    MI_CpuSet(self, 0, sizeof(*self));
    MI_CpuCopyU8(args->source, &self->source, sizeof(self->source));
    self->angle        = args->angle;
    self->angleStep    = args->angleStep;
    self->yScale       = args->yScale;
    self->speed        = args->speed;
    self->acceleration = args->acceleration;
    self->velocityX    = FX_Mul(FX_CosIdx(self->angle), self->speed);
    self->velocityY    = FX_Mul(FX_SinIdx(self->angle), self->speed);
    BtlSprite_LoadDirect(engine, &self->sprite, visual->file, entry->charDataIndex, entry->animDataIndex, entry->cellDataIndex,
                         visual->paletteEntry, entry->animIndex);
    BtlSprite_SetAnim(&self->sprite, self->source.animation, self->source.animationMode);
    self->source.core.sprite = &self->sprite;
    func_ov003_020cdfbc(&self->source, args->source->core.owner->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED, 0);
    func_ov003_020cccf0(engine, &self->source);
    return 1;
}

static s32 func_ov003_020d25e8(Ov003EnemyShotSpiral* self) {
    if (func_ov003_020ccb9c(&self->source, &self->sprite))
        return 0;
    func_ov003_020cdfbc(&self->source, 0, 1);
    self->source.core.position.x += self->velocityX;
    self->source.core.position.y += FX_Mul(self->velocityY, self->yScale);
    self->angle += self->angleStep;
    self->speed += self->acceleration;
    self->velocityX = FX_Mul(FX_CosIdx(self->angle), self->speed);
    self->velocityY = FX_Mul(FX_SinIdx(self->angle), self->speed);
    s16 x, y;
    BtlStage_WorldToScreen(func_ov003_020c37f8(&self->sprite) ? 1 : 0, &x, &y, self->source.core.position.x,
                           self->source.core.position.y, self->source.core.position.z);
    self->source.core.hitResult = BtlHit_AttackAtScreen(BtlHit_GetAttackInfo(self->source.core.attackId),
                                                        &self->source.core.owner->base.actor.baseArgs, x, y);
    BtlSprite_Update(&self->sprite);
    return 1;
}

static s32 func_ov003_020d2740(Ov003EnemyShotSpiral* self) {
    s16 x, y;
    BtlStage_WorldToScreen(func_ov003_020c37f8(&self->sprite) ? 1 : 0, &x, &y, self->source.core.position.x,
                           self->source.core.position.y, self->source.core.position.z);
    BtlSprite_SetPosition(&self->sprite, x, y);
    BtlSprite_SetSortKey(&self->sprite, 0x7FFFFFFF - self->source.core.position.y);
    BtlSprite_Render(&self->sprite);
    return 1;
}

static s32 func_ov003_020d27b8(Ov003EnemyShotSpiral* self) {
    BtlSprite_Release(&self->sprite);
    return 1;
}

static s32 func_ov003_020d27cc(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EnemyShotSpiral* self = task->data;
    switch (stage) {
        case 0:
            return func_ov003_020d2484(self, args);
        case 1:
            return func_ov003_020d25e8(self);
        case 2:
            return func_ov003_020d2740(self);
        case 3:
            return func_ov003_020d27b8(self);
        default:
            return 1;
    }
}

static char             data_ov003_020e6d60[24] = "Tsk_BtlEnmShot_Spiral";
static const TaskHandle data_ov003_020e0cbc     = {data_ov003_020e6d60, func_ov003_020d27cc, sizeof(Ov003EnemyShotSpiral)};
