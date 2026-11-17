#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"

#include <nitro/fx.h>
#include <nitro/mi/cpumem.h>

// The level-up gauge badge (`Tsk_BtlBadge_Gauge`): the badge sprite, its gauge bar and the flash
// that plays when the actor is hit, all following the touch actor's remaining lifetime.

/// The battle sprite page the badge family loads from (`Apl_Hir/Grp_BtlBadge.bin`).
extern const BinIdentifier data_ov003_020dc044;

static s32 func_ov003_020a7688(TaskPool* pool, Task* task, void* args, s32 stage);

/// The payload of the task: the touch actor followed by the badge sprites.
typedef struct BtlBadgeGauge {
    /* 0x000 */ Ov003TouchActor actor;
    /* 0x114 */ BattleSprite    badge;
    /* 0x174 */ BattleSprite    bar;
    /* 0x1D4 */ BattleSprite    flash;
    /* 0x234 */ BattleSprite    glow;
    /* 0x294 */ s32             glowing; // the bar is showing its filled animation
    /* 0x298 */ s32             unk_298;
} BtlBadgeGauge;                         // Size: 0x29C

static char             data_ov003_020e4924[0x14] = "Tsk_BtlBadge_Gauge";
static const TaskHandle data_ov003_020dc5e4       = {data_ov003_020e4924, func_ov003_020a7688, sizeof(BtlBadgeGauge)};

void func_ov003_020a7640(Ov003PinAction* action) {
    Ov003PinAction* args = action;

    EasyTask_CreateTask(&gBattle->commonPool, &data_ov003_020dc5e4, NULL, 0, NULL, &args);
}

static s32 func_ov003_020a7688(TaskPool* pool, Task* task, void* args, s32 stage) {
    switch (stage) {
        case 0: {
            BtlBadgeGauge*    self  = task->data;
            Ov003TouchActor*  actor = &self->actor;
            SpriteAnimationEx anim;

            MI_CpuSet(self, 0, sizeof(*self));
            BtlBadgeActor_Init(actor, *(void**)args);
            BtlSprite_InitAnim(&anim.anim, 2, &data_ov003_020dc044);
            anim.anim.bits_10_11 = 0;
            anim.anim.unk_1E     = 0;
            anim.anim.packIndex  = 1;
            anim.anim.unk_18     = 2;
            anim.animTable       = NULL;
            anim.anim.unk_20     = 4;
            anim.anim.unk_28     = 3;
            anim.anim.unk_26     = 1;
            BtlSprite_Load(&self->badge, &anim);
            BtlSprite_SetAnim(&self->badge, 7, 0);
            BtlSprite_SetPriority(&self->badge, 0);
            BtlSprite_SetSortKey(&self->badge, 0x3E9000);
            BtlSprite_Load(&self->bar, &anim);
            BtlSprite_SetAnim(&self->bar, 9, 0);
            BtlSprite_SetPriority(&self->bar, 0);
            BtlSprite_SetSortKey(&self->bar, 0x3EA000);
            BtlSprite_Load(&self->flash, &anim);
            BtlSprite_SetAnim(&self->flash, 0x15, 0);
            BtlSprite_SetPriority(&self->flash, 0);
            BtlSprite_SetSortKey(&self->flash, 0x3F2000);
            return 1;
        }
        case 1: {
            BtlBadgeGauge*   self  = task->data;
            Ov003TouchActor* actor = &self->actor;
            s32              state;

            if (gBattle->specialFreeze != 0 || (gBattle->flags & (BATTLE_FLAG_SPECIAL | BATTLE_FLAG_FEAT)) != 0 ||
                (gBattle->flags & BATTLE_FLAG_ACTORS_FROZEN) != 0)
            {
                return 1;
            }
            state = BtlBadgeActor_RunRequest(actor);
            if (state == 3 || state == 7) {
                if (actor->unk_044 == actor->unk_046) {
                    self->glowing = 1;
                    BtlSprite_SetAnim(&self->bar, 9, 0);
                } else {
                    self->glowing = 0;
                    BtlSprite_SetAnim(&self->bar, 8, 0);
                }
                if (gBattle->state == BATTLE_STATE_FIGHT) {
                    SpriteAnimationEx anim;

                    BtlSprite_InitAnim(&anim.anim, 2, &data_ov003_020dc044);
                    anim.anim.bits_10_11 = 0;
                    anim.anim.unk_1C     = 3;
                    anim.anim.animIndex  = 1;
                    anim.anim.unk_28     = 1;
                    anim.anim.unk_20     = 4;
                    anim.animTable       = NULL;
                    anim.anim.unk_18     = 2;
                    anim.anim.unk_26     = 3;
                    anim.anim.packIndex  = 1;
                    BtlSprite_Load(&self->glow, &anim);
                    BtlSprite_SetAnim(&self->glow, 0x16, 1);
                    BtlSprite_SetPriority(&self->glow, 0);
                    BtlSprite_SetSortKey(&self->glow, 0x3F2000);
                }
            }
            func_ov003_020a3d98(actor);
            switch (self->actor.state) {
                case 0:
                case 1:
                case 2:
                    break;
                case 3:
                    if (self->glowing != 0 && actor->unk_044 < actor->unk_046) {
                        self->glowing = 0;
                        BtlSprite_SetAnim(&self->bar, 8, 0);
                    }
                    if (actor->unk_044 <= 0) {
                        BtlBadgeActor_Request(actor, 2);
                        BtlSe_Play(SEIDX_SE_PSY_EMPTY);
                    }
                    break;
                case 4:
                case 5:
                    break;
            }
            if (Sprite_HasAnimation(&self->glow.sprite) != 0) {
                if (SpriteMgr_IsAnimationFinished(&self->glow.sprite) != 0) {
                    BtlSprite_Release(&self->glow);
                } else {
                    BtlSprite_Update(&self->glow);
                }
            }
            BtlSprite_Update(&self->flash);
            BtlSprite_Update(&self->bar);
            BtlSprite_Update(&self->badge);
            BtlSprite_Update(&self->actor.sprite);
            return 1;
        }
        case 2: {
            BtlBadgeGauge*   self  = task->data;
            Ov003TouchActor* actor = &self->actor;
            s32              state;
            s32              posX;
            s32              posY;

            if (func_ov003_020a3dbc(actor) == 0) {
                return 1;
            }
            if (actor->unk_044 != actor->unk_046) {
                BtlSprite_SetAffineTransform(&self->bar, 0, FX_Divide(actor->unk_044 << 12, actor->unk_046 << 12), 0x1000, 0);
            } else {
                BtlSprite_SetAffineTransform(&self->bar, 0, 0x1000, 0x1000, 0);
            }
            posX = (s32)(actor->x * 0x10) >> 16;
            posY = (s32)(actor->y * 0x10) >> 16;
            BtlSprite_SetPosition(&self->flash, posX, posY);
            BtlSprite_SetPosition(&self->bar, posX, posY);
            BtlSprite_SetPosition(&self->badge, posX, posY);
            BtlSprite_SetPosition(&self->actor.sprite, posX, posY);
            state = actor->state;
            if (func_ov003_020a3edc(actor) != 0) {
                if (state == 5) {
                    BtlSprite_Render(&self->flash);
                } else {
                    BtlSprite_Render(&self->bar);
                    BtlSprite_Render(&self->badge);
                }
                BtlSprite_Render(&self->actor.sprite);
            } else {
                if (state == 5) {
                    BtlSprite_RenderTranslucent(&self->flash, 8);
                } else {
                    BtlSprite_RenderTranslucent(&self->bar, 8);
                    BtlSprite_RenderTranslucent(&self->badge, 8);
                }
                BtlSprite_RenderTranslucent(&self->actor.sprite, 8);
            }
            if (Sprite_HasAnimation(&self->glow.sprite) != 0) {
                BtlSprite_SetPosition(&self->glow, posX, posY);
                if (func_ov003_020a3edc(actor) != 0) {
                    BtlSprite_Render(&self->glow);
                } else {
                    BtlSprite_RenderTranslucent(&self->glow, 8);
                }
            }
            return 1;
        }
        case 3: {
            BtlBadgeGauge*   self  = task->data;
            Ov003TouchActor* actor = &self->actor;

            BtlSprite_Release(&self->flash);
            BtlSprite_Release(&self->bar);
            BtlSprite_Release(&self->badge);
            BtlSprite_Release(&self->actor.sprite);
            func_ov003_020a3dac(actor);
            return 1;
        }
    }
}
