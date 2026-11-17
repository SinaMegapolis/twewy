#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlBar.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "main_02026528.h"

#include <nitro/mi/cpumem.h>

// The BS (beat/sync) bar HUD: the partner's sync gauge with its two "banked" segments, drawn with
// the same ten-sprite layout as the HP bar (`BtlBarHP`). The gauge itself lives in `Battle`
// (0x3D8D8 value, 0x3D8DA max, 0x3D8DC/0x3D8DE the two banked segments); this unit splits it into
// the bar's pixel widths and drives the fill/drain animation.

/// The BS bar's task data (0x400 bytes): the shared bar layout plus a trailing word.
typedef struct BtlBarBS {
    /* 0x000 */ BtlBar bar;
    /* 0x3FC */ s32    unk_3FC;
} BtlBarBS; // Size: 0x400

static s32 BtlBarBS_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlBarBS = {"Tsk_BtlBarBS", BtlBarBS_RunTask, sizeof(BtlBarBS)};

/// Selects the animation of the six gauge sprites: the "filling" set while the gauge is below a
/// quarter, the idle set otherwise.
static void func_ov003_020999c4(BtlBar* bar) {
    if (bar->unk_3F8 != 0) {
        BtlSprite_SetAnim(&bar->sprites[6], 1, 0);
        BtlSprite_SetAnim(&bar->sprites[5], 6, 0);
        BtlSprite_SetAnim(&bar->sprites[9], 2, 0);
        BtlSprite_SetAnim(&bar->sprites[1], 1, 0);
        BtlSprite_SetAnim(&bar->sprites[0], 6, 0);
        BtlSprite_SetAnim(&bar->sprites[4], 2, 0);
    } else {
        BtlSprite_SetAnim(&bar->sprites[6], 8, 0);
        BtlSprite_SetAnim(&bar->sprites[5], 10, 0);
        BtlSprite_SetAnim(&bar->sprites[9], 11, 0);
        BtlSprite_SetAnim(&bar->sprites[1], 8, 0);
        BtlSprite_SetAnim(&bar->sprites[0], 10, 0);
        BtlSprite_SetAnim(&bar->sprites[4], 11, 0);
    }
}

/// Fills the gauge: spreads the second banked segment over the width that is still free and
/// advances the two sampled segment widths.
// Nonmatching: `oldB` and `bankedA` are allocated to lr/r12 the other way round than the retail
// build; everything else is instruction-identical.
static void func_ov003_02099a98(BtlBar* bar) {
    s32 oldB    = bar->unk_3DC;
    s32 oldA    = bar->unk_3DA;
    s32 bankedB = gBattle->bossDamageByPlayer;
    s32 bankedA = gBattle->bossDamageByPartner;
    s32 delta   = (bankedB - oldB) + (bankedA - oldA);
    s32 free    = (bankedB + bankedA) - gBattle->bossMaxHp * 3 / 4;

    // The gauge width is recomputed per store; the retail build has a division by 4 and then a
    // separate division by 2 for each of the three segments.
    bar->unk_3DC                = gBattle->bossMaxHp * 3 / 4 / 2;
    bar->unk_3DA                = gBattle->bossMaxHp * 3 / 4 / 2;
    gBattle->bossDamageByPlayer = gBattle->bossMaxHp * 3 / 4 / 2;

    if (delta > 0) {
        gBattle->bossDamageByPlayer += (s16)(free * (bankedB - oldB) / delta);
    }
    gBattle->bossDamageByPartner = gBattle->bossMaxHp - gBattle->bossHp - gBattle->bossDamageByPlayer;
}

/// Drains the gauge: the mirror image of `func_ov003_02099a98`.
// Nonmatching: same lr/r12 swap as `func_ov003_02099a98`.
static void func_ov003_02099b94(BtlBar* bar) {
    s32 oldB    = bar->unk_3DC;
    s32 oldA    = bar->unk_3DA;
    s32 bankedB = gBattle->bossDamageByPlayer;
    s32 bankedA = gBattle->bossDamageByPartner;
    s32 delta   = (oldB - bankedB) + (oldA - bankedA);
    s32 free    = gBattle->bossMaxHp * 3 / 4 - (bankedB + bankedA);

    bar->unk_3DC                = gBattle->bossMaxHp * 3 / 4 / 2;
    bar->unk_3DA                = gBattle->bossMaxHp * 3 / 4 / 2;
    gBattle->bossDamageByPlayer = gBattle->bossMaxHp * 3 / 4 / 2;

    if (delta > 0) {
        gBattle->bossDamageByPlayer -= (s16)(free * (oldB - bankedB) / delta);
    }
    gBattle->bossDamageByPartner = gBattle->bossMaxHp - gBattle->bossHp - gBattle->bossDamageByPlayer;
}

/// Clamps both banked segments to at least 3/8 of the gauge's maximum.
/// Clamps both banked segments to at least 3/8 of the gauge's maximum.
static void func_ov003_02099c90(void) {
    s32 span    = gBattle->bossMaxHp * 3 / 4;
    s32 width   = span / 2;
    s32 bankedB = gBattle->bossDamageByPlayer;

    if (bankedB < width) {
        gBattle->bossDamageByPlayer = width;
        gBattle->bossDamageByPartner -= (s16)(width - bankedB);
    }
    {
        s32 bankedA = gBattle->bossDamageByPartner;

        if (bankedA < width) {
            gBattle->bossDamageByPartner = width;
            gBattle->bossDamageByPlayer -= (s16)(width - bankedA);
        }
    }
}

/// Converts a banked segment value to a bar width in pixels, for a gauge that is filling.
static s32 func_ov003_02099d24(s16 value) {
    s32 span  = gBattle->bossMaxHp * 3 / 4;
    s16 local = value - (s16)(span / 2);
    s32 unit  = gBattle->bossMaxHp / 4;

    if (local > unit) {
        local = unit;
    }
    return (((local << 12) / unit) * 0xA0 + 2 << 4) >> 16;
}

/// Converts a banked segment value to a bar width in pixels.
static s32 func_ov003_02099d98(s16 value) {
    s32 span  = gBattle->bossMaxHp * 3 / 4;
    s16 local = value;

    if (local > span) {
        local = span;
    }
    return (((local << 12) / span) * 0xA0 + 2 << 4) >> 16;
}

/// Seeds the sync gauge from the partner actor and snaps the bar to it.
static void func_ov003_02099dec(BtlBarBS* bs, BattleActor* actor) {
    BtlBar* bar = &bs->bar;

    gBattle->bossHp    = actor->currentHp;
    gBattle->bossMaxHp = actor->maxHp;
    actor->flags |= ACTOR_FLAG_BOSS_GAUGE_HP;
    if (actor->linkedActor != NULL) {
        actor->linkedActor->flags |= ACTOR_FLAG_BOSS_GAUGE_HP;
    }

    bar->flags   = 0;
    bar->unk_3DA = gBattle->bossDamageByPartner;
    bar->unk_3DC = gBattle->bossDamageByPlayer;

    if (gBattle->bossHp <= gBattle->bossMaxHp / 4) {
        func_ov003_02099a98(bar);
        bar->unk_3F8 = 1;
        bar->unk_3EC = func_ov003_02099d24(gBattle->bossDamageByPartner);
        bar->unk_3EE = func_ov003_02099d24(gBattle->bossDamageByPlayer);
    } else {
        bar->unk_3F8 = 0;
        bar->unk_3EC = func_ov003_02099d98(gBattle->bossDamageByPartner);
        bar->unk_3EE = func_ov003_02099d98(gBattle->bossDamageByPlayer);
    }

    func_ov003_020999c4(bar);
    bar->unk_3E4 = bar->unk_3EC << 12;
    bar->unk_3E8 = bar->unk_3EE << 12;
    bar->unk_3DE = 0;
    bar->unk_3E0 = 0;
    bs->unk_3FC  = 0;
}

/// Per-frame bar state machine: runs the fill/clamp/drain helper for the current gauge value and
/// publishes the segment widths and visibility flags the render stage consumes.
// Nonmatching: the two width selects keep `fixed` and the sampled width in the opposite scratch
// registers than the retail build (r0/r1); the logic and instruction count are identical.
static void func_ov003_02099f40(BtlBar* bar) {
    s32 width;
    s32 target;
    s32 fixed;
    s32 other;

    bar->flags = 0;
    if (bar->unk_3F8 == 0) {
        if (gBattle->bossHp <= gBattle->bossMaxHp / 4) {
            func_ov003_02099a98(bar);
            bar->unk_3F8 = 1;
            bar->unk_3EC = 0;
            bar->unk_3EE = 0;
            bar->unk_3E4 = 0;
            bar->unk_3E8 = 0;
            bar->unk_3C0 = 0;
            bar->unk_3C2 = 0;
            func_ov003_020999c4(bar);
        }
    } else if (gBattle->bossHp > gBattle->bossMaxHp / 4) {
        func_ov003_02099b94(bar);
        bar->unk_3F8 = 0;
        bar->unk_3EC = func_ov003_02099d98(gBattle->bossDamageByPartner);
        bar->unk_3EE = func_ov003_02099d98(gBattle->bossDamageByPlayer);
        bar->unk_3E4 = bar->unk_3EC << 12;
        bar->unk_3E8 = bar->unk_3EE << 12;
        bar->unk_3C0 = 0;
        bar->unk_3C2 = 0;
        func_ov003_020999c4(bar);
    } else {
        func_ov003_02099c90();
    }

    if (bar->unk_3DA < gBattle->bossDamageByPartner) {
        bar->unk_3DE = 0x1E;
        bar->unk_3E4 = bar->unk_3EC << 12;
        bar->unk_3F0 = 1;
    } else if (bar->unk_3DA > gBattle->bossDamageByPartner) {
        bar->unk_3DE = 0x1E;
        bar->unk_3E4 = bar->unk_3EC << 12;
        bar->unk_3F0 = 2;
    } else if (bar->unk_3DE <= 0) {
        bar->unk_3F0 = 0;
    }
    if (bar->unk_3DC < gBattle->bossDamageByPlayer) {
        bar->unk_3E0 = 0x1E;
        bar->unk_3E8 = bar->unk_3EE << 12;
        bar->unk_3F4 = 1;
    } else if (bar->unk_3DC > gBattle->bossDamageByPlayer) {
        bar->unk_3E0 = 0x1E;
        bar->unk_3E8 = bar->unk_3EE << 12;
        bar->unk_3F4 = 2;
    } else if (bar->unk_3E0 <= 0) {
        bar->unk_3F4 = 0;
    }

    bar->unk_3DA = gBattle->bossDamageByPartner;
    bar->unk_3DC = gBattle->bossDamageByPlayer;

    if (bar->unk_3F8 != 0) {
        bar->unk_3EC = func_ov003_02099d24(gBattle->bossDamageByPartner);
    } else {
        bar->unk_3EC = func_ov003_02099d98(gBattle->bossDamageByPartner);
    }
    fixed = (bar->unk_3E4 << 4) >> 16;

    width        = (bar->unk_3F0 == 2) ? fixed : bar->unk_3EC;
    bar->unk_3C0 = width;
    if (width > 0x50) {
        bar->flags &= ~0x400;
    } else {
        bar->flags |= 0x400;
    }

    other = (bar->unk_3F0 == 2) ? bar->unk_3EC : fixed;
    if (other > 0x50) {
        bar->flags |= 2;
        bar->unk_3C6 = other - 0x50;
    } else {
        bar->flags &= ~2;
    }

    if (bar->unk_3F0 == 1) {
        bar->flags |= 4;
        bar->unk_3C8 = fixed;
        bar->flags |= 0x20;
        bar->unk_3D2 = bar->unk_3EC - 0x50;
    } else if (bar->unk_3F0 == 2) {
        bar->flags |= 0x40;
        bar->unk_3CC = bar->unk_3EC;
        bar->flags |= 0x200;
        bar->unk_3D6 = fixed - 0x50;
    }

    if (bar->unk_3F8 != 0) {
        bar->unk_3EE = func_ov003_02099d24(gBattle->bossDamageByPlayer);
    } else {
        bar->unk_3EE = func_ov003_02099d98(gBattle->bossDamageByPlayer);
    }
    fixed = (bar->unk_3E8 << 4) >> 16;

    target       = (bar->unk_3F4 == 2) ? fixed : bar->unk_3EE;
    bar->unk_3C2 = -target;
    if (target > 0x50) {
        bar->flags &= ~0x800;
    } else {
        bar->flags |= 0x800;
    }

    other = (bar->unk_3F4 == 2) ? bar->unk_3EE : fixed;
    if (other > 0x50) {
        bar->flags |= 1;
        bar->unk_3C4 = -(other - 0x50);
    } else {
        bar->flags &= ~1;
    }

    if (bar->unk_3F4 == 1) {
        bar->flags |= 0x10;
        bar->unk_3CA = -fixed;
        bar->flags |= 8;
        bar->unk_3D0 = -(bar->unk_3EE - 0x50);
    } else if (bar->unk_3F4 == 2) {
        bar->flags |= 0x100;
        bar->unk_3CE = -bar->unk_3EE;
        bar->flags |= 0x80;
        bar->unk_3D4 = -(fixed - 0x50);
    }

    if (bar->unk_3DE > 0) {
        func_02026590(&bar->unk_3E4, bar->unk_3EC << 12, (u16)bar->unk_3DE);
        bar->unk_3DE--;
    }
    if (bar->unk_3E0 > 0) {
        func_02026590(&bar->unk_3E8, bar->unk_3EE << 12, (u16)bar->unk_3E0);
        bar->unk_3E0--;
    }

    if (gBattle->bossHp <= 0) {
        bar->flags &= ~0x400;
        bar->flags &= ~0x800;
    }
}

/// Loads the ten bar sprites, then seeds the gauge from the partner actor in `args`.
static s32 BtlBarBS_Init(TaskPool* pool, Task* task, void* args) {
    BtlBar* bar = task->data;

    MI_CpuSet(bar, 0, sizeof(BtlBarBS));

    BtlSprite_LoadDirect(1, &bar->sprites[5], &Iden_BtlMenu, 46, 48, 47, 0x3F, 6);
    BtlSprite_LoadDirect(1, &bar->sprites[6], &Iden_BtlMenu, 46, 48, 47, 0x3F, 1);
    BtlSprite_LoadDirect(1, &bar->sprites[9], &Iden_BtlMenu, 46, 48, 47, 0x3F, 2);
    BtlSprite_LoadDirect(1, &bar->sprites[8], &Iden_BtlMenu, 46, 48, 47, 0x3F, 3);
    BtlSprite_LoadDirect(1, &bar->sprites[7], &Iden_BtlMenu, 46, 48, 47, 0x3F, 4);
    BtlSprite_LoadDirect(0, &bar->sprites[0], &Iden_BtlMenu, 43, 45, 44, 0x3D, 6);
    BtlSprite_LoadDirect(0, &bar->sprites[1], &Iden_BtlMenu, 43, 45, 44, 0x3D, 1);
    BtlSprite_LoadDirect(0, &bar->sprites[4], &Iden_BtlMenu, 43, 45, 44, 0x3D, 2);
    BtlSprite_LoadDirect(0, &bar->sprites[3], &Iden_BtlMenu, 43, 45, 44, 0x3D, 3);
    BtlSprite_LoadDirect(0, &bar->sprites[2], &Iden_BtlMenu, 43, 45, 44, 0x3D, 4);

    BtlSprite_SetSortKey(&bar->sprites[5], 9);
    BtlSprite_SetSortKey(&bar->sprites[6], 7);
    BtlSprite_SetSortKey(&bar->sprites[9], 5);
    BtlSprite_SetSortKey(&bar->sprites[0], 9);
    BtlSprite_SetSortKey(&bar->sprites[1], 7);
    BtlSprite_SetSortKey(&bar->sprites[4], 5);

    BtlSprite_SetPriority(&bar->sprites[5], 0);
    BtlSprite_SetPriority(&bar->sprites[6], 0);
    BtlSprite_SetPriority(&bar->sprites[7], 0);
    BtlSprite_SetPriority(&bar->sprites[8], 0);
    BtlSprite_SetPriority(&bar->sprites[9], 0);
    BtlSprite_SetPriority(&bar->sprites[0], 0);
    BtlSprite_SetPriority(&bar->sprites[1], 0);
    BtlSprite_SetPriority(&bar->sprites[2], 0);
    BtlSprite_SetPriority(&bar->sprites[3], 0);
    BtlSprite_SetPriority(&bar->sprites[4], 0);

    func_ov003_02099dec((BtlBarBS*)bar, args);
    func_ov003_020999c4(bar);
    return 1;
}

static s32 BtlBarBS_Update(TaskPool* pool, Task* task, void* args) {
    BtlBar* bar = task->data;

    func_ov003_02099f40(bar);
    BtlSprite_Update(&bar->sprites[5]);
    BtlSprite_Update(&bar->sprites[6]);
    BtlSprite_Update(&bar->sprites[7]);
    BtlSprite_Update(&bar->sprites[8]);
    BtlSprite_Update(&bar->sprites[9]);
    BtlSprite_Update(&bar->sprites[0]);
    BtlSprite_Update(&bar->sprites[1]);
    BtlSprite_Update(&bar->sprites[2]);
    BtlSprite_Update(&bar->sprites[3]);
    BtlSprite_Update(&bar->sprites[4]);
    return 1;
}

static s32 BtlBarBS_Render(TaskPool* pool, Task* task, void* args) {
    BtlBar* bar = task->data;

    BtlSprite_SetPosition(&bar->sprites[5], 8, 192);
    BtlSprite_Render(&bar->sprites[5]);

    if (bar->flags & 0x400) {
        BtlSprite_SetPosition(&bar->sprites[6], 8, (s16)(bar->unk_3C0 + 0xC0));
        BtlSprite_Render(&bar->sprites[6]);
    }
    if (bar->flags & 1) {
        BtlSprite_SetPosition(&bar->sprites[9], 8, (s16)(bar->unk_3C4 + 0xC0));
        BtlSprite_Render(&bar->sprites[9]);
    }
    if (bar->flags & 4) {
        BtlSprite_SetSortKey(&bar->sprites[8], 8);
        BtlSprite_SetPosition(&bar->sprites[8], 8, (s16)(bar->unk_3C8 + 0xC0));
        BtlSprite_Render(&bar->sprites[8]);
    }
    if (bar->flags & 8) {
        BtlSprite_SetSortKey(&bar->sprites[8], 6);
        BtlSprite_SetPosition(&bar->sprites[8], 8, (s16)(bar->unk_3D0 + 0x110));
        BtlSprite_Render(&bar->sprites[8]);
    }
    if (bar->flags & 0x40) {
        BtlSprite_SetSortKey(&bar->sprites[7], 8);
        BtlSprite_SetPosition(&bar->sprites[7], 8, (s16)(bar->unk_3CC + 0xC0));
        BtlSprite_Render(&bar->sprites[7]);
    }
    if (bar->flags & 0x80) {
        BtlSprite_SetSortKey(&bar->sprites[7], 6);
        BtlSprite_SetPosition(&bar->sprites[7], 8, (s16)(bar->unk_3D4 + 0x110));
        BtlSprite_Render(&bar->sprites[7]);
    }

    BtlSprite_SetPosition(&bar->sprites[0], 8, 0);
    BtlSprite_Render(&bar->sprites[0]);

    if (bar->flags & 0x800) {
        BtlSprite_SetPosition(&bar->sprites[1], 8, bar->unk_3C2);
        BtlSprite_Render(&bar->sprites[1]);
    }
    if (bar->flags & 2) {
        BtlSprite_SetPosition(&bar->sprites[4], 8, bar->unk_3C6);
        BtlSprite_Render(&bar->sprites[4]);
    }
    if (bar->flags & 0x10) {
        BtlSprite_SetSortKey(&bar->sprites[3], 8);
        BtlSprite_SetPosition(&bar->sprites[3], 8, bar->unk_3CA);
        BtlSprite_Render(&bar->sprites[3]);
    }
    if (bar->flags & 0x20) {
        BtlSprite_SetSortKey(&bar->sprites[3], 6);
        BtlSprite_SetPosition(&bar->sprites[3], 8, (s16)(bar->unk_3D2 - 0x50));
        BtlSprite_Render(&bar->sprites[3]);
    }
    if (bar->flags & 0x100) {
        BtlSprite_SetSortKey(&bar->sprites[2], 8);
        BtlSprite_SetPosition(&bar->sprites[2], 8, bar->unk_3CE);
        BtlSprite_Render(&bar->sprites[2]);
    }
    if (bar->flags & 0x200) {
        BtlSprite_SetSortKey(&bar->sprites[2], 6);
        BtlSprite_SetPosition(&bar->sprites[2], 8, (s16)(bar->unk_3D6 - 0x50));
        BtlSprite_Render(&bar->sprites[2]);
    }
    return 1;
}

static s32 BtlBarBS_Release(TaskPool* pool, Task* task, void* args) {
    BtlBar* bar = task->data;

    BtlSprite_Release(&bar->sprites[5]);
    BtlSprite_Release(&bar->sprites[6]);
    BtlSprite_Release(&bar->sprites[9]);
    BtlSprite_Release(&bar->sprites[8]);
    BtlSprite_Release(&bar->sprites[7]);
    BtlSprite_Release(&bar->sprites[0]);
    BtlSprite_Release(&bar->sprites[1]);
    BtlSprite_Release(&bar->sprites[2]);
    BtlSprite_Release(&bar->sprites[3]);
    BtlSprite_Release(&bar->sprites[4]);
    return 1;
}

static s32 BtlBarBS_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlBarBS_Init,
        .update     = BtlBarBS_Update,
        .render     = BtlBarBS_Render,
        .cleanup    = BtlBarBS_Release,
    };

    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209ab9c(BattleActor* actor) {
    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlBarBS, NULL, 0, NULL, actor);
}
