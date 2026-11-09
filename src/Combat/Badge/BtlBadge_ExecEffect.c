#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"

#include <nitro/mi/cpumem.h>

// The pin-execution badge (`Tsk_BtlBadge_ExecEffect`): the actor sprite plus the two charge
// sprites and the finishing animation, all following the actor it was spawned from.

/// The battle sprite page the badge family loads from (`Apl_Hir/Grp_BtlBadge.bin`).
extern const BinIdentifier data_ov003_020dc044;

static s32 func_ov003_020a72cc(TaskPool* pool, Task* task, void* args, s32 stage);

/// The payload of the task: the actor the badge follows followed by its three sprites.
typedef struct BtlBadgeExecEffect {
    /* 0x000 */ Ov003TouchActor* actor;
    union {
        struct {
            /* 0x004 */ BattleSprite mae;    // the charge sprite before the actor
            /* 0x064 */ BattleSprite ato;    // the charge sprite after the actor
            /* 0x0C4 */ BattleSprite finish; // the finishing animation
        };
        /* 0x004 */ BattleSprite sprites[3];
    };
} BtlBadgeExecEffect;

/// The task parameter: the actor the badge follows.
typedef struct BtlBadgeExecEffectArgs {
    /* 0x0 */ Ov003TouchActor* actor;
} BtlBadgeExecEffectArgs;

static char             data_ov003_020e490c[0x18] = "Tsk_BtlBadge_ExecEffect";
static const TaskHandle data_ov003_020dc5d8       = {data_ov003_020e490c, func_ov003_020a72cc, sizeof(BtlBadgeExecEffect)};

void func_ov003_020a7284(Ov003TouchActor* actor) {
    BtlBadgeExecEffectArgs sp8;

    sp8.actor = actor;
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov003_020dc5d8, NULL, 0, NULL, &sp8);
}

static s32 func_ov003_020a72cc(TaskPool* pool, Task* task, void* args, s32 stage) {
    switch (stage) {
        default:
        case 0: {
            BtlBadgeExecEffect* self = task->data;
            SpriteAnimationEx   anim;

            MI_CpuSet(self, 0, sizeof(*self));
            self->actor = *(Ov003TouchActor**)args;
            BtlSprite_InitAnim(&anim.anim, 2, &data_ov003_020dc044);
            anim.anim.bits_10_11 = 0;
            anim.anim.unk_1C     = 3;
            anim.anim.animIndex  = 1;
            anim.anim.unk_28     = 1;
            anim.anim.unk_20     = 4;
            anim.animTable       = NULL;
            anim.anim.unk_18     = 2;
            anim.anim.unk_26     = 0;
            anim.anim.packIndex  = 1;
            BtlSprite_Load(&self->mae, &anim);
            BtlSprite_SetAnim(&self->mae, 0, 0);
            BtlSprite_SetPriority(&self->mae, 0);
            BtlSprite_SetSortKey(&self->mae, 0x3E7000);
            BtlSprite_Load(&self->ato, &anim);
            BtlSprite_SetAnim(&self->ato, 1, 0);
            BtlSprite_SetPriority(&self->ato, 0);
            BtlSprite_SetSortKey(&self->ato, 0x3E7000);
            BtlSprite_Load(&self->finish, &anim);
            BtlSprite_SetAnim(&self->finish, 2, 1);
            BtlSprite_SetPriority(&self->finish, 0);
            BtlSprite_SetSortKey(&self->finish, 0x3F3000);
            return 1;
        }
        case 1: {
            BtlBadgeExecEffect* self = task->data;
            s32                 i;

            if (g_CombatGlobal->unk3D884 != 0 || (g_CombatGlobal->unk3D878 & 6) != 0 || (g_CombatGlobal->unk3D878 & 0x40) != 0)
            {
                return 1;
            }
            if (self->actor->unk_050 > 0) {
                self->actor->unk_050--;
            }
            if (self->actor->unk_052 > 0) {
                self->actor->unk_052--;
            }
            if (self->actor->unk_054 != 0) {
                self->actor->unk_054 = 0;
                BtlSprite_Restart(&self->finish);
            }
            for (i = 0; i < 3; i++) {
                BtlSprite_Update(&self->sprites[i]);
            }
            return 1;
        }
        case 2: {
            BtlBadgeExecEffect* self = task->data;
            s32                 posX;
            s32                 posY;
            s32                 i;

            if (BtlDebug_GetSetting(BTL_DEBUG_SETTING_MODE) == BTL_DEBUG_MODE_PLAYER_ONLY) {
                return 1;
            }
            // the battle must be idle: not waiting and not paused (the target branches on both)
            if ((g_CombatGlobal->unk3D878 & 6) != 0 || g_CombatGlobal->unk3D884 != 0 || (g_CombatGlobal->unk3D878 & 6) != 0) {
                return 1;
            }
            posX = (s32)(self->actor->x * 0x10) >> 16;
            posY = (s32)(self->actor->y * 0x10) >> 16;
            for (i = 0; i < 3; i++) {
                BtlSprite_SetPosition(&self->sprites[i], posX, posY);
            }
            if (func_ov003_020a3edc(self->actor) != 0) {
                if (self->actor->unk_050 > 0) {
                    BtlSprite_Render(&self->mae);
                } else if (self->actor->unk_052 > 0) {
                    BtlSprite_Render(&self->ato);
                }
                if (SpriteMgr_IsAnimationFinished(&self->finish.sprite) == 0) {
                    BtlSprite_Render(&self->finish);
                }
            } else {
                if (self->actor->unk_050 > 0) {
                    BtlSprite_RenderTranslucent(&self->mae, 8);
                } else if (self->actor->unk_052 > 0) {
                    BtlSprite_RenderTranslucent(&self->ato, 8);
                }
                if (SpriteMgr_IsAnimationFinished(&self->finish.sprite) == 0) {
                    BtlSprite_RenderTranslucent(&self->finish, 8);
                }
            }
            return 1;
        }
        case 3: {
            BtlBadgeExecEffect* self = task->data;
            s32                 i;

            for (i = 0; i < 3; i++) {
                BtlSprite_Release(&self->sprites[i]);
            }
            return 1;
        }
    }
}
