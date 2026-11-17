#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"

#include "Combat/Core/CombatApi.h"
#include "main_02026528.h"
#include <nitro/mi/cpumem.h>

// The charge star effect: a star over the top of the sub screen, a glow on the owner, and up to five
// trail sparks (three to five by charge level) that fly from the top of the screen to the owner and
// cross over onto the main screen.

typedef struct BtlStarTrail {
    /* 0x00 */ s32  x;
    /* 0x04 */ s32  y;
    /* 0x08 */ s16  timer;
    /* 0x0A */ u16  pad;
    /* 0x0C */ BOOL isActive;
} BtlStarTrail; // Size: 0x10

typedef struct BtlEffStar {
    /* 0x000 */ BattleSprite     sprites[4]; // star, owner glow, trail on the sub screen, trail on the main screen
    /* 0x180 */ Ov003TouchActor* owner;
    /* 0x184 */ BtlStarTrail     trails[5];
    /* 0x1D4 */ s8               level;
    /* 0x1D5 */ s8               count;
    /* 0x1D6 */ s16              timer;
    /* 0x1D8 */ BOOL             isActive;
} BtlEffStar; // Size: 0x1DC

typedef struct BtlEffStarArgs {
    /* 0x0 */ Ov003TouchActor* owner;
    /* 0x4 */ s8               level;
} BtlEffStarArgs; // Size: 0x8

static s32 BtlEff_Star_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlEff_Star = {"Tsk_BtlEff_Star", BtlEff_Star_RunTask, sizeof(BtlEffStar)};

static const SpriteAnimEntry data_ov003_020d8714 = {28, 30, 29, 0};
static const SpriteAnimEntry data_ov003_020d870c = {1, 3, 2, 0};

static void func_ov003_0209fd48(s32 engine, BattleSprite* sprite) {
    BtlSprite_LoadDirect(engine, sprite, &Iden_BtlMenu, 25, 27, 26, 0x3F, 12);
    BtlSprite_SetPriority(sprite, 0);
}

static void func_ov003_0209fd98(BtlEffStar* star) {
    s32 i;

    star->count = 0;
    for (i = 0; i < 5; i++) {
        star->trails[i].isActive = FALSE;
    }
    func_ov003_0209fd48(1, &star->sprites[2]);
    func_ov003_0209fd48(0, &star->sprites[3]);
}

/// Launches the next trail spark, until the charge level's spark count is reached.
static void func_ov003_0209fddc(BtlEffStar* star) {
    s32           max = 0;
    BtlStarTrail* trail;

    switch (star->level) {
        case 1:
            max = 3;
            break;
        case 2:
            max = 4;
            break;
        case 3:
            max = 5;
            break;
    }
    if (star->count < max) {
        trail           = &star->trails[star->count];
        trail->x        = 0x80000;
        trail->y        = 0;
        trail->timer    = 20;
        trail->isActive = TRUE;
        star->count++;
    }
}

/// Eases every flying spark towards the owner.
static void func_ov003_0209fe58(BtlEffStar* star) {
    s32           x = star->owner->x, y = star->owner->y;
    s32           i;
    BtlStarTrail* trail;

    for (i = 0; i < star->count; i++) {
        trail = &star->trails[i];
        if (trail->isActive) {
            if (trail->timer > 0) {
                func_020265d4(&trail->x, x, (u16)trail->timer);
                func_020265d4(&trail->y, y + 0xC0000, (u16)trail->timer);
                trail->timer--;
            }
            if (trail->timer <= 0) {
                trail->isActive = FALSE;
            }
        }
    }
}

/// Draws every flying spark on the screen(s) it is over.
static void func_ov003_0209fef0(BtlEffStar* star) {
    s32           i;
    BtlStarTrail* trail;

    for (i = 0; i < star->count; i++) {
        trail = &star->trails[i];
        if (trail->isActive) {
            if (trail->y < 0xD0000) {
                BtlSprite_SetPosition(&star->sprites[2], (s16)(trail->x >> 12), (s16)(trail->y >> 12));
                BtlSprite_Render(&star->sprites[2]);
            }
            if (trail->y > 0xB0000) {
                BtlSprite_SetPosition(&star->sprites[3], (s16)(trail->x >> 12), (s16)((trail->y >> 12) - 192));
                BtlSprite_Render(&star->sprites[3]);
            }
        }
    }
}

// Nonmatching: the descriptor flag halfword load and stores are scheduled differently.
// All assigned descriptor values and subsequent calls agree with retail.
static s32 BtlEff_Star_Init(TaskPool* pool, Task* task, void* args) {
    BtlEffStar*       star   = task->data;
    BtlEffStarArgs*   params = args;
    SpriteAnimationEx anim;

    MI_CpuSet(star, 0, sizeof(*star));
    star->level    = params->level;
    star->owner    = params->owner;
    star->timer    = 0;
    star->isActive = TRUE;
    BtlSprite_LoadFromTable(1, &star->sprites[0], &Iden_BtlMenu, &data_ov003_020d8714, 0, 0x3F, 16);
    BtlSprite_SetAnimFromTable(&star->sprites[0], 0, 1);
    BtlSprite_SetPriority(&star->sprites[0], 0);
    BtlSprite_SetPosition(&star->sprites[0], 128, 0);
    BtlSprite_InitAnim(&anim.anim, 2, &Iden_BtlBadge);
    anim.anim.unk_18     = 2;
    anim.anim.unk_26     = 2;
    anim.anim.packIndex  = 1;
    anim.anim.unk_1C     = 1;
    anim.anim.animIndex  = 1;
    anim.anim.unk_1E     = 2048;
    anim.anim.bits_10_11 = 1;
    anim.anim.unk_28     = 3;
    anim.anim.unk_20     = 4;
    anim.animTable       = &data_ov003_020d870c;
    BtlSprite_Load(&star->sprites[1], &anim);
    BtlSprite_SetAnimFromTable(&star->sprites[1], 0, 1);
    BtlSprite_SetPriority(&star->sprites[1], 0);
    BtlSprite_SetSortKey(&star->sprites[1], 0x3E9000);
    func_ov003_0209fd98(star);
    return 1;
}

static s32 BtlEff_Star_Update(TaskPool* pool, Task* task, void* args) {
    BtlEffStar* star = task->data;

    if (gBattle->flags & (BATTLE_FLAG_SPECIAL | BATTLE_FLAG_FEAT)) {
        return 0;
    }
    if (star->timer % 4 == 0) {
        func_ov003_0209fddc(star);
    }
    star->timer++;
    if (star->timer > 40) {
        return 0;
    }
    func_ov003_0209fe58(star);
    BtlSprite_Update(&star->sprites[0]);
    BtlSprite_Update(&star->sprites[1]);
    BtlSprite_Update(&star->sprites[2]);
    BtlSprite_Update(&star->sprites[3]);
    return 1;
}

static s32 BtlEff_Star_Render(TaskPool* pool, Task* task, void* args) {
    BtlEffStar* star = task->data;

    BtlSprite_Render(&star->sprites[0]);
    BtlSprite_SetPosition(&star->sprites[1], (s16)(star->owner->x >> 12), (s16)(star->owner->y >> 12));
    BtlSprite_Render(&star->sprites[1]);
    func_ov003_0209fef0(star);
    return 1;
}

static s32 BtlEff_Star_Release(TaskPool* pool, Task* task, void* args) {
    BtlEffStar* star = task->data;

    BtlSprite_Release(&star->sprites[0]);
    BtlSprite_Release(&star->sprites[1]);
    BtlSprite_Release(&star->sprites[2]);
    BtlSprite_Release(&star->sprites[3]);
    return 1;
}

static s32 BtlEff_Star_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlEff_Star_Init,
        .update     = BtlEff_Star_Update,
        .render     = BtlEff_Star_Render,
        .cleanup    = BtlEff_Star_Release,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov003_020a0244(s32 level, Ov003TouchActor* owner) {
    BtlEffStarArgs params;

    params.owner = owner;
    params.level = level;
    return EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlEff_Star, NULL, 0, NULL, &params);
}
