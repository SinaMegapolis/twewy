#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/EasyTask.h"
#include "Save.h"

#include <nitro/mi/cpumem.h>

// The badge sprite of the ranking effects (`Tsk_BtlBadge_Special`). The task payload is a touch
// actor followed by the ranking index and the swap timer; the sprite is rebuilt whenever the
// ranking index of the current high score changes, and the actor reports the hit when the
// player touches it.

// ---------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------

/// The payload of the task: the touch actor plus the ranking index it shows.
typedef struct BtlBadgeSpecial {
    /* 0x000 */ Ov003TouchActor actor;
    /* 0x114 */ s8              index; // ranking index (> 0 while shown)
    /* 0x115 */ char            pad_115[0x1];
    /* 0x116 */ s16             timer; // frames until the hit is reported
} BtlBadgeSpecial;                     // Size: 0x118

/// One record of the ranking sprite table: the animation to play and the sprite ids it uses.
typedef struct BtlBadgeSpecialAnim {
    /* 0x0 */ u16 anim; // animation index
    /* 0x2 */ u16 unk_02;
    /* 0x4 */ u16 spriteA;
    /* 0x6 */ u16 spriteB;
    /* 0x8 */ s16 spriteC;
    /* 0xA */ u16 packIndex;
} BtlBadgeSpecialAnim; // Size: 0xC

// ---------------------------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------------------------

/// The battle sprite page the badge family loads from (`Apl_Hir/Grp_BtlBadge.bin`).
extern const BinIdentifier data_ov003_020d7810;

static s32 func_ov003_0209477c(TaskPool* pool, Task* task, void* args);
static s32 func_ov003_02094850(TaskPool* pool, Task* task, void* args);
static s32 func_ov003_020949e4(TaskPool* pool, Task* task, void* args);
static s32 func_ov003_02094a4c(TaskPool* pool, Task* task, void* args);
static s32 func_ov003_02094a74(TaskPool* pool, Task* task, void* args, s32 stage);

static char      data_ov003_020e3b58[0x18] = "Tsk_BtlBadge_Special";
const TaskHandle data_ov003_020d7d24       = {data_ov003_020e3b58, func_ov003_02094a74, sizeof(BtlBadgeSpecial)};

/// The stage table of the badge: build, update, position and render, release.
static const TaskStages data_ov003_020d7d30 = {
    func_ov003_0209477c,
    func_ov003_02094850,
    func_ov003_020949e4,
    func_ov003_02094a4c,
};

/// The ranking sprite records, indexed by `partner * 3 + ranking index - 1`.
static const BtlBadgeSpecialAnim data_ov003_020d7d40[9] = {
    {2, 1,  3,  2, 1,  4},
    {2, 5,  7,  6, 1,  8},
    {2, 9, 11, 10, 1, 12},
    {3, 1,  3,  2, 1,  4},
    {3, 5,  7,  6, 1,  8},
    {3, 9, 11, 10, 1, 12},
    {4, 1,  3,  2, 1,  4},
    {4, 5,  7,  6, 1,  8},
    {4, 9, 11, 10, 1, 12},
};

// ---------------------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------------------

/// Loads the ranking sprite `index` (1..3) of the current partner into `sprite`.
// Nonmatching: the record address is kept in its own register in the target; mwcc here folds it
// into `mla table, index, 12, table` (every record access then costs a `ldrh [table]`).
static void func_ov003_02094668(BattleSprite* sprite, s32 index) {
    SpriteAnimationEx          anim;
    const BtlBadgeSpecialAnim* record;
    int                        partner = 0;
    s32                        recordIndex;

    if (index <= 0) {
        return;
    }
    if (index > 3) {
        return;
    }
    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            partner = index - 1;
            break;
        case 1:
            partner = index + 2;
            break;
        case 2:
            partner = index + 5;
            break;
    }
    record = &data_ov003_020d7d40[partner];
    BtlSprite_InitAnim(&anim.anim, 2, &data_ov003_020d7810);
    anim.anim.bits_10_11 = 1;
    anim.anim.unk_18     = 2;
    anim.anim.packIndex  = record->anim;
    anim.anim.unk_1C     = record->unk_02;
    anim.anim.unk_1E     = 0x320;
    anim.anim.unk_20     = record->packIndex;
    anim.anim.unk_26     = record->spriteB;
    anim.anim.unk_28     = record->spriteA;
    anim.anim.animIndex  = record->spriteC + 1;
    anim.animTable       = (const SpriteAnimEntry*)&record->unk_02;
    BtlSprite_Load(sprite, &anim);
    BtlSprite_SetPriority(sprite, 0);
    BtlSprite_SetSortKey(sprite, 0x3E8000);
}

/// Builds the touch actor and parks it on the ranking badge.
// Nonmatching: the partner check is materialised as a boolean before the test.
static s32 func_ov003_0209477c(TaskPool* pool, Task* task, void* args) {
    BtlBadgeSpecial* self = task->data;
    s32              val  = 0;

    MI_CpuSet(self, 0, sizeof(*self));
    if (!(g_CombatGlobal->unk3D88D == 0 || g_CombatGlobal->unk3D88D == 1 || g_CombatGlobal->unk3D88D == 2)) {
        return 0;
    }
    self->actor.state   = 3;
    self->actor.kind    = 0xFFFE;
    self->actor.slot    = 6;
    self->actor.x       = gSaveData.battleTouchSlots.playerX << 12;
    self->actor.y       = gSaveData.battleTouchSlots.playerY << 12;
    self->actor.radius  = 0x17;
    self->actor.unk_04C = val;
    self->actor.unk_046 = 1;
    self->actor.unk_044 = val;
    self->index         = 0;
    self->timer         = val;
    BtlList_Append(&g_CombatGlobal->unk3D8BC, &self->actor.node, &self->actor);
    return 1;
}

/// Picks the ranking index to show, rebuilds the sprite when it changes and reports the hit.
static s32 func_ov003_02094850(TaskPool* pool, Task* task, void* args) {
    BtlBadgeSpecial* self = task->data;
    s8               index;

    if (g_CombatGlobal->unk3D884 != 0 || (g_CombatGlobal->unk3D878 & 2) != 0) {
        return 1;
    }
    if (self->index > 0) {
        s32 index = func_ov003_02086fec();

        if (self->index != index) {
            self->index = index;
            BtlSprite_Release(&self->actor.sprite);
            func_ov003_020a0244(self->index, &self->actor);
            BtlSe_Play(SEIDX_SE_GIRL_ESP_FULL);
            func_ov003_02094668(&self->actor.sprite, self->index);
        }
    } else {
        self->index = func_ov003_02086fec();
        self->timer = 0x14;
        if (self->index > 0) {
            self->actor.unk_044 = 1;
            func_ov003_020a0244(self->index, &self->actor);
            BtlSe_Play(SEIDX_SE_GIRL_ESP_FULL);
            func_ov003_02094668(&self->actor.sprite, self->index);
        }
    }
    if (self->index > 0 && self->timer <= 0 && g_CombatGlobal->unk3D874 == 2 && (g_CombatGlobal->unk3D878 & 0x20000) == 0 &&
        func_ov003_020a3f20(&self->actor) != 0)
    {
        g_CombatGlobal->unk3D878 |= 0x20000;
        func_ov003_02087008();
        self->index         = 0;
        self->actor.unk_044 = 0;
        BtlSprite_Release(&self->actor.sprite);
    }
    if (self->index > 0) {
        if (self->timer > 0) {
            self->timer--;
        }
        BtlSprite_Update(&self->actor.sprite);
    }
    return 1;
}

/// Moves the badge onto the actor and draws it.
static s32 func_ov003_020949e4(TaskPool* pool, Task* task, void* args) {
    BtlBadgeSpecial* self = task->data;

    if (func_ov003_020a3dbc(&self->actor) == 0) {
        return 1;
    }
    if (self->index > 0 && self->timer <= 0) {
        BtlSprite_SetPosition(&self->actor.sprite, (s32)((u32)self->actor.x << 4) >> 16, (s32)((u32)self->actor.y << 4) >> 16);
        BtlSprite_Render(&self->actor.sprite);
    }
    return 1;
}

/// Releases the badge sprite.
static s32 func_ov003_02094a4c(TaskPool* pool, Task* task, void* args) {
    BtlBadgeSpecial* self = task->data;

    if (self->index > 0) {
        BtlSprite_Release(&self->actor.sprite);
    }
    return 1;
}

/// The task dispatcher: runs the stage of the touch actor task.
static s32 func_ov003_02094a74(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov003_020d7d30;

    return stages.iter[stage](pool, task, args);
}
