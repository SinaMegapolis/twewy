#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatEnemyShot.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020e0c24;

s32 func_ov003_020cd988(Ov003EnemyShotChargeSplitArgs* input) {
    Ov003EnemyShotSplitArgs args;
    MI_CpuSet(&args, 0, sizeof(args));
    args.source            = input->source;
    args.childSource       = input->childSource;
    args.travelFrames      = input->travelFrames;
    args.target.x          = input->target.x;
    args.target.y          = input->target.y;
    args.target.z          = input->target.z;
    args.yScale            = input->yScale;
    args.pattern.delay     = input->patterns[1].delay;
    args.pattern.count     = input->patterns[1].count;
    args.pattern.speed     = input->patterns[1].speed;
    args.pattern.velocityZ = input->patterns[1].velocityZ;
    args.pattern.damping   = input->patterns[1].damping;
    func_ov003_020d2bc0(&args);
    args.source->visual    = NULL;
    args.pattern.delay     = input->patterns[0].delay;
    args.pattern.count     = input->patterns[0].count;
    args.pattern.speed     = input->patterns[0].speed;
    args.pattern.velocityZ = input->patterns[0].velocityZ;
    args.pattern.damping   = input->patterns[0].damping;
    return func_ov003_020d2bc0(&args);
}

s32 func_ov003_020cda50(Ov003EnemyShotSource* source, Ov003EnemyShotSource* secondSource, s32 x, s32 y, s32 z) {
    Ov003EnemyShotChargeArgs args;
    MI_CpuSet(&args, 0, sizeof(args));
    args.source       = source;
    args.secondSource = secondSource;
    args.velocity.x   = x;
    args.velocity.y   = y;
    args.velocity.z   = z;
    TaskPool* pool;
    if (!func_ov003_020c37f8(&source->core.owner->base.sprite))
        pool = &g_CombatGlobal->unk_00000;
    else
        pool = &g_CombatGlobal->taskPool;
    return EasyTask_CreateTask(pool, &data_ov003_020e0c24, NULL, 0, NULL, &args);
}

static s32 func_ov003_020cdae4(Ov003EnemyShotCharge* self, Ov003EnemyShotChargeArgs* args) {
    s32 engine = args->source->core.owner->base.sprite.sprite.bits_0_1;
    MI_CpuSet(self, 0, sizeof(*self));
    MI_CpuCopyU8(args->source, &self->source, sizeof(self->source));
    MI_CpuCopyU8(args->secondSource, &self->secondSource, sizeof(self->secondSource));
    self->velocity.x                        = args->velocity.x;
    self->velocity.y                        = args->velocity.y;
    self->velocity.z                        = args->velocity.z;
    const Ov003EnemyShotTableVisual* visual = args->source->tableVisual;
    BtlSprite_LoadFromTable(engine, &self->sprites[0], visual->base.file, visual->base.entries, visual->base.entry,
                            visual->base.paletteEntry, visual->charTiles);
    visual = args->secondSource->tableVisual;
    BtlSprite_LoadFromTable(engine, &self->sprites[1], visual->base.file, visual->base.entries, visual->base.entry,
                            visual->base.paletteEntry, visual->charTiles);
    BtlSprite_SetAnimFromTable(&self->sprites[0], self->source.animation, self->source.animationMode);
    BtlSprite_SetAnimFromTable(&self->sprites[1], self->secondSource.animation, 1);
    BtlSprite_SetFlip(&self->sprites[0], self->source.core.owner->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED);
    self->source.core.sprite = &self->sprites[0];
    func_ov003_020cdfbc(&self->source, args->source->core.owner->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED, 0);
    func_ov003_020cccf0(engine, &self->source);
    self->activeSprite = &self->sprites[0];
    self->activeSource = &self->source;
    return 1;
}

static s32 func_ov003_020cdc28(Ov003EnemyShotCharge* self) {
    BattleSprite*         sprite = self->activeSprite;
    Ov003EnemyShotSource* source = self->activeSource;
    if (func_ov003_020ccb9c(source, sprite))
        return 0;
    switch (self->phase) {
        case 0:
            func_ov003_020cdfbc(source, 0, 1);
            source->core.position.x += self->velocity.x;
            source->core.position.y += self->velocity.y;
            source->core.position.z += self->velocity.z;
            if (source->core.position.z > 0) {
                self->activeSprite = &self->sprites[1];
                BtlSprite_Update(&self->sprites[1]);
                self->activeSource = &self->secondSource;
                source->core.flags = (source->core.flags | 2) & ~1;
                source->core.flags &= ~0x20;
                self->secondSource.core.position.x = source->core.position.x;
                self->secondSource.core.position.y = source->core.position.y;
                self->secondSource.core.position.z = 0;
                func_ov003_020cccf0(sprite->sprite.bits_0_1, self->activeSource);
                self->phase++;
            }
            break;
        case 1:
            self->source.core.hitResult =
                BtlHit_AttackAt(BtlHit_GetAttackInfo(source->core.attackId), &source->core.owner->base.actor.baseArgs,
                                source->core.position.x, source->core.position.y, source->core.position.z);
            if (SpriteMgr_IsAnimationFinished(&sprite->sprite))
                return 0;
            break;
    }
    BtlSprite_Update(sprite);
    return 1;
}

static s32 func_ov003_020cdd80(Ov003EnemyShotCharge* self) {
    BattleSprite*         sprite = self->activeSprite;
    Ov003EnemyShotSource* source = self->activeSource;
    s16                   x, y;
    BtlStage_WorldToScreen(func_ov003_020c37f8(sprite) ? 1 : 0, &x, &y, source->core.position.x, source->core.position.y,
                           source->core.position.z);
    BtlSprite_SetPosition(sprite, x, y);
    s32 depth = source->core.position.y;
    if (depth <= 0)
        depth = 0;
    BtlSprite_SetSortKey(sprite, 0x7FFFFFFF - depth);
    BtlSprite_Render(sprite);
    return 1;
}

static s32 func_ov003_020cde04(Ov003EnemyShotCharge* self) {
    BtlSprite_Release(&self->sprites[0]);
    BtlSprite_Release(&self->sprites[1]);
    return 1;
}

static s32 func_ov003_020cde24(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EnemyShotCharge* self = task->data;
    switch (stage) {
        case 0:
            return func_ov003_020cdae4(self, args);
        case 1:
            return func_ov003_020cdc28(self);
        case 2:
            return func_ov003_020cdd80(self);
        case 3:
            return func_ov003_020cde04(self);
        default:
            return 1;
    }
}

static char             data_ov003_020e6c14[24] = "Tsk_BtlEnmShot_Charge";
static const TaskHandle data_ov003_020e0c24     = {data_ov003_020e6c14, func_ov003_020cde24, sizeof(Ov003EnemyShotCharge)};
