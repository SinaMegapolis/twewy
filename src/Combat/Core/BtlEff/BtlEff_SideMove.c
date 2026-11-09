#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatBossExp.h"
#include "Combat/Core/CombatCutin.h"
#include "Engine/EasyTask.h"
#include <nitro/mi/cpumem.h>

// The boss defeat burst: the cut-in sprite animations of the boss and its partner are kept in four slots, and a task
// (`Tsk_BossExp1`, defined with the boss explosion) is started with them once the cut-in teardown reaches it. The
// stage function at the end drives one of the sprites that fly out of the boss.

typedef struct BossExp1State {
    /* 0x00 */ s32           reserved;
    /* 0x04 */ BossExp1Anims anims;
    /* 0xC8 */ s32           task;  // -1 until the burst task is running
    /* 0xCC */ s16           timer;
    /* 0xCE */ s16           phase; // 0 waiting to start the task, 1 waiting for it to finish
} BossExp1State;                    // Size: 0xD0

/// One sprite that flies out of the boss.
typedef struct BossExp1Piece {
    /* 0x00 */ BattleSprite     sprite;
    /* 0x60 */ Ov003CutinActor* actor;
    /* 0x64 */ Vec              position;
    /* 0x70 */ s32              velocityX;
    /* 0x74 */ s32              velocityY;
    /* 0x78 */ s32              fall;
    /* 0x7C */ s16              frame;
    /* 0x7E */ u16              reserved7E;
} BossExp1Piece; // Size: 0x80

extern const BinIdentifier data_ov003_020d7800;
extern const TaskHandle    Tsk_BossExp1;

static s32 BtlEff_SideMove_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle BtlEff_SideMove = {"BtlEff_SideMove", BtlEff_SideMove_RunTask, sizeof(BossExp1Piece)};

/// The animation table of the flying pieces.
static const SpriteAnimEntry data_ov003_020e0b80[2] = {
    {31, 33, 32, 0},
    {31, 33, 32, 1},
};

BossExp1State data_ov003_020e7254;

void func_ov003_020c9018(const BossExp1Anims* anims) {
    data_ov003_020e7254.anims = *anims;
    data_ov003_020e7254.task  = -1;
    data_ov003_020e7254.timer = 0;
    data_ov003_020e7254.phase = 0;
}

void func_ov003_020c9064(const SpriteAnimationEx* anim) {
    data_ov003_020e7254.anims.slots[0] = *anim;
    data_ov003_020e7254.anims.slots[1] = *anim;
    data_ov003_020e7254.anims.slots[2] = *anim;
    data_ov003_020e7254.anims.slots[3] = *anim;
    data_ov003_020e7254.task           = -1;
    data_ov003_020e7254.timer          = 0;
    data_ov003_020e7254.phase          = 0;
}

void func_ov003_020c9118(Ov003EnemyActor* actor) {
    Ov003EnemyActor*  partner = actor->partner;
    SpriteAnimationEx anim;
    SpriteAnimationEx copy;

    func_ov003_020c439c(actor, &anim);
    anim.anim.posX                     = 0x80;
    anim.anim.posY                     = actor->actor.height / 2 + 0x60;
    copy                               = anim;
    data_ov003_020e7254.anims.slots[0] = copy;
    data_ov003_020e7254.anims.slots[1] = copy;
    data_ov003_020e7254.anims.slots[2] = copy;
    data_ov003_020e7254.anims.slots[3] = copy;
    if (partner != NULL) {
        func_ov003_020c439c(partner, &anim);
        anim.anim.posX = 0x80;
        anim.anim.posY = actor->actor.height / 2 + 0x60;
        if (func_ov003_020c37f8(&partner->sprite)) {
            copy                               = anim;
            data_ov003_020e7254.anims.slots[2] = copy;
            data_ov003_020e7254.anims.slots[3] = copy;
        } else {
            copy                               = anim;
            data_ov003_020e7254.anims.slots[0] = copy;
            data_ov003_020e7254.anims.slots[1] = copy;
        }
    }
    data_ov003_020e7254.task  = -1;
    data_ov003_020e7254.timer = 0;
    data_ov003_020e7254.phase = 0;
}

s32 func_ov003_020c9338(Ov003EnemyActor* actor) {
    s32 result = 1;

    switch (data_ov003_020e7254.phase) {
        case 0:
            if (data_ov003_020e7254.timer > 0 && data_ov003_020e7254.task == -1 && !(g_CombatGlobal->unk3D878 & 0x20000)) {
                data_ov003_020e7254.anims.actor = actor;
                data_ov003_020e7254.task =
                    EasyTask_CreateTask(&g_CombatGlobal->unk_34230, &Tsk_BossExp1, NULL, 0, NULL, &data_ov003_020e7254.anims);
                if (data_ov003_020e7254.task != -1) {
                    g_CombatGlobal->unk3D878 |= 0x20000;
                    g_CombatGlobal->unk3D878 |= 0x10000;
                    data_ov003_020e7254.timer = 0;
                    data_ov003_020e7254.phase++;
                    if (actor->actor.flags & ACTOR_FLAG_KILLED) {
                        func_ov003_020c5130(actor);
                    }
                }
            }
            data_ov003_020e7254.timer++;
            break;
        case 1:
            if (!(g_CombatGlobal->unk3D878 & 2)) {
                result = 0;
            } else {
                data_ov003_020e7254.timer++;
            }
            break;
    }
    return result;
}

static s32 BtlEff_SideMove_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    BossExp1Piece* self   = task->data;
    s32            result = 1;

    if (self != NULL) {
        switch (stage) {
            case 0: {
                s16 x;
                s16 y;

                MI_CpuSet(self, 0, sizeof(*self));
                self->actor      = args;
                self->position.x = self->actor->base.actor.position.x;
                self->position.y = self->actor->base.actor.position.y;
                self->position.z = self->actor->base.actor.position.z;
                self->velocityX  = (self->actor->base.unk_1AC - self->actor->base.actor.position.x) / 30;
                self->velocityY  = (self->actor->base.unk_1B0 - self->actor->base.actor.position.y) / 20;
                self->fall       = -0xA000;
                self->frame      = 0;
                BtlSprite_LoadFromTable(1, &self->sprite, &data_ov003_020d7800, data_ov003_020e0b80, 0, 0x3F, 0);
                BtlSprite_SetAnimFromTable(&self->sprite, 0, 1);
                if (self->actor->base.actor.position.x > g_CombatGlobal->partner->position.x) {
                    BtlSprite_SetFlip(&self->sprite, 1);
                    self->position.x -= 0x18000;
                } else {
                    BtlSprite_SetFlip(&self->sprite, 0);
                    self->position.x += 0x18000;
                }
                self->position.y += 0x8000;
                break;
            }
            case 1:
                if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
                    result = 0;
                }
                BtlSprite_Update(&self->sprite);
                self->position.x += self->velocityX;
                self->position.y += self->velocityY;
                self->position.z += self->fall + FX32_CONVERT(self->frame);
                self->frame++;
                break;
            case 2: {
                s16 x;
                s16 y;

                BtlStage_WorldToScreen(1, &x, &y, self->position.x, self->position.y, self->position.z);
                BtlSprite_SetPosition(&self->sprite, x, y);
                BtlSprite_SetSortKey(&self->sprite, 0x7FFFFFFF - (self->position.y + 0x8000));
                BtlSprite_Render(&self->sprite);
                break;
            }
            case 3:
                BtlSprite_Release(&self->sprite);
                break;
        }
    }
    return result;
}
