#include "Combat/Core/BtlCutIn.h"
#include "Combat/Core/BtlFeat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"
#include "Combat/Core/CombatEnemyShot.h"
#include "Display.h"
#include "EasyFade.h"
#include "Engine/Core/Memory.h"
#include "SndMgr.h"
#include <nitro/gx.h>
#include <nitro/mi/cpumem.h>
#include <nitro/reg.h>
#include <stddef.h>

// Shared status storage still comes from retail assembly.
typedef struct BtlActSpArgs {
    union {
        struct {
            u16 character, level;
        };

        u32 selection;
    };

    s32 skipFeature;
} BtlActSpArgs;

typedef struct BtlActSp {
    /* 0x00 */ u32 taskId;
    /* 0x04 */ void (*update)(struct BtlActSp*);
    /* 0x08 */ s16 phase;
    /* 0x0A */ s16 timer;
    /* 0x0C */ union {
        struct {
            u16 character, level;
        };

        u32 selection;
    };

    /* 0x10 */ s32 skipFeature;
    /* 0x14 */ s32 active;
} BtlActSp;

typedef struct BtlActSpCutins {
    const TaskHandle* entries[3];
} BtlActSpCutins;

typedef struct BtlActSpFeatures {
    const TaskHandle* entries[3][3];
} BtlActSpFeatures;

typedef char check_ActSpArgs[(sizeof(BtlActSpArgs) == 8) ? 1 : -1];
typedef char check_ActSp[(sizeof(BtlActSp) == 0x18) ? 1 : -1];
typedef char check_ActSp_phase[(offsetof(BtlActSp, phase) == 8) ? 1 : -1];
typedef char check_ActSp_selection[(offsetof(BtlActSp, selection) == 0xC) ? 1 : -1];
typedef char check_ActSp_active[(offsetof(BtlActSp, active) == 0x14) ? 1 : -1];

typedef struct BtlActSpAttackTable {
    u16 entries[3][3];
} BtlActSpAttackTable;

extern const TaskHandle data_ov024_020f6b7c;

typedef struct BtlActSpStatus {
    s8  level;
    s8  character;
    u8  reserved[6];
    s32 released;
} BtlActSpStatus;

typedef char          check_ActSpStatus[(sizeof(BtlActSpStatus) == 0xC) ? 1 : -1];
typedef char          check_ActSpStatus_released[(offsetof(BtlActSpStatus, released) == 8) ? 1 : -1];
extern BtlActSpStatus data_ov003_020e71d8;

static s32 BtlActSp00_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlActSp00 = {"Tsk_BtlActSp00", BtlActSp00_RunTask, sizeof(BtlActSp)};

static void func_ov003_020c1918(BtlActSp*);

extern const TaskHandle Tsk_BtlCutIn00, Tsk_BtlCutIn01, Tsk_BtlCutIn02;
extern const TaskHandle Tsk_BtlFeat00, Tsk_BtlFeat01, Tsk_BtlFeat02;
extern const TaskHandle data_ov005_020f0570, data_ov005_020f0708, data_ov005_020f0764;
extern const TaskHandle data_ov006_020f0ac0, data_ov006_020f0b60, data_ov006_020f0d0c;

static const BtlActSpCutins data_ov003_020ddab8 = {
    {&Tsk_BtlCutIn00, &Tsk_BtlCutIn01, &Tsk_BtlCutIn02}
};
static const BtlActSpAttackTable data_ov003_020ddad4 = {
    {{0x22, 0x24, 0x26}, {0x28, 0x2A, 0x2C}, {0x2E, 0x30, 0x32}}
};
static const BtlActSpAttackTable data_ov003_020ddae6 = {
    {{0x21, 0x23, 0x25}, {0x27, 0x29, 0x2B}, {0x2D, 0x2F, 0x31}}
};
static const BtlActSpFeatures data_ov003_020ddaf8 = {
    {{&Tsk_BtlFeat00, &Tsk_BtlFeat01, &Tsk_BtlFeat02},
     {&data_ov005_020f0570, &data_ov005_020f0708, &data_ov005_020f0764},
     {&data_ov006_020f0ac0, &data_ov006_020f0b60, &data_ov006_020f0d0c}}
};

static void func_ov003_020c1224(BtlActSp* self, void (*update)(BtlActSp*)) {
    self->update = update;
    self->phase  = 0;
    self->timer  = 0;
}

// Nonmatching: attack-loop and signed healing arithmetic register allocation.
static void func_ov003_020c1238(u16 character, u16 level) {
    BtlActSpAttackTable subAttacks  = data_ov003_020ddad4;
    BtlActSpAttackTable mainAttacks = data_ov003_020ddae6;
    BattleActor*        mainActor   = gBattle->player;
    BattleAttackArgs    args;

    args.power            = mainActor->attack;
    args.powerPercent     = 0x64;
    args.effectPercent    = 0x64;
    args.comboFrames      = 0x5A;
    Ov003EnemyActor* unit = BtlList_GetHead(&gBattle->enemiesSub);
    if (unit) {
        u16 attack = subAttacks.entries[character][level];
        do {
            BattleActor* owner = gBattle->partner;
            BtlHit_AttackActor(BtlHit_GetAttackInfo(attack), &owner->baseArgs, &unit->actor, owner->position.x);
            unit = BtlList_GetNext(&unit->node);
        } while (unit);
    }
    unit = BtlList_GetHead(&gBattle->enemiesMain);
    if (unit) {
        u16 attack = mainAttacks.entries[character][level];
        do {
            BattleActor* owner = gBattle->player;
            BtlHit_AttackActor(BtlHit_GetAttackInfo(attack), &args, &unit->actor, owner->position.x);
            unit = BtlList_GetNext(&unit->node);
        } while (unit);
    }
    // Retail callers supply level 0..2; the tables use the same range.
    s16 heal;
    switch (level) {
        case 0:
            heal = gBattle->teamMaxHp * 20 / 100;
            break;
        case 1:
            heal = gBattle->teamMaxHp * 50 / 100;
            break;
        case 2:
            heal = gBattle->teamMaxHp * 100 / 100;
            break;
    }
    s32 missing = gBattle->teamMaxHp - gBattle->teamHp;
    if (missing < heal) {
        heal = missing;
    }
    s32 rounded = heal + ((u32)heal >> 31);
    BtlMgr_AddTeamHp(0, (heal & 1) + (rounded >> 1));
    BtlMgr_AddTeamHp(1, (s16)((rounded << 15) >> 16));
    BtlActor_ClearPeriodicEffect(gBattle->partner);
    BtlActor_ClearPeriodicEffect(gBattle->player);
}

// Nonmatching: display-setting store scheduling.
static void func_ov003_020c14ec(void) {
    Display_SetMainBrightness(0x10);
    Display_SetSubBrightness(0x10);
    Display_CommitSynced();
    g_DisplaySettings.unk_000 = 0;
    REG_POWER_CNT &= ~0x8000;
    g_DisplaySettings.controls[0].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[0].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[0].dimension = GX2D3D_MODE_3D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_3D);
    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 1, 0, 0x4004);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 3, 1, 0x420C);
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 4, 1, 1, 0x404);
    g_DisplaySettings.engineState[0].bgSettings[0].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[0].bgSettings[0].mosaic   = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic   = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic   = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic   = 0;
    Display_SetMainLayers(0x1F);
    g_DisplaySettings.controls[1].bgMode = GX_BGMODE_0;
    GXs_SetGraphicsMode(GX_BGMODE_0);
    Display_InitSubBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_256, 6, 5, 0, 0x4694);
    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_256, 0, 1, 0, 0x4084);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_256, 2, 3, 1, 0x428C);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_256, 4, 5, 1, 0x494);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[0].mosaic   = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic   = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic   = 0;
    g_DisplaySettings.engineState[1].bgSettings[3].mosaic   = 0;
    Display_SetSubLayers(0x1E);
    g_DisplaySettings.engineState[0].blendMode = 1;
    Display_SetBlendLayers(0, 1, 0x3E);
    Display_SetBlendCoeffs(0, 6, 0xA);
    g_DisplaySettings.engineState[1].blendMode = 1;
    Display_SetBlendLayers(1, 1, 0x3E);
    Display_SetBlendCoeffs(1, 6, 0xA);
    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.engineState[0].windowObj            = 8;
    g_DisplaySettings.engineState[0].windowObjEffects     = 0;
    g_DisplaySettings.engineState[0].windowOutside        = 0x17;
    g_DisplaySettings.engineState[0].windowOutsideEffects = 0;
    g_DisplaySettings.controls[1].windows |= 4;
    g_DisplaySettings.engineState[1].windowObj            = 8;
    g_DisplaySettings.engineState[1].windowObjEffects     = 0;
    g_DisplaySettings.engineState[1].windowOutside        = 0x17;
    g_DisplaySettings.engineState[1].windowOutsideEffects = 0;
    Display_Commit();
}

static void func_ov003_020c17bc(void) {
    g_DisplaySettings.engineState[0].blendMode = 1;
    Display_SetBlendLayers(0, 1, 0x3E);
    Display_SetBlendCoeffs(0, 6, 0xA);
    g_DisplaySettings.engineState[1].blendMode = 1;
    Display_SetBlendLayers(1, 1, 0x3E);
    Display_SetBlendCoeffs(1, 6, 0xA);
    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.engineState[0].windowObj            = 0;
    g_DisplaySettings.engineState[0].windowObjEffects     = 0;
    g_DisplaySettings.engineState[0].windowOutside        = 0x1F;
    g_DisplaySettings.engineState[0].windowOutsideEffects = 0;
    g_DisplaySettings.controls[1].windows |= 4;
    g_DisplaySettings.engineState[1].windowObj            = 0;
    g_DisplaySettings.engineState[1].windowObjEffects     = 0;
    g_DisplaySettings.engineState[1].windowOutside        = 0x1F;
    g_DisplaySettings.engineState[1].windowOutsideEffects = 0;
    Display_Commit();
}

// Nonmatching: fade-stage register allocation.
static void func_ov003_020c1848(BtlActSp* self) {
    s16 phase = self->phase;

    switch (phase) {
        case 0:
            SndMgr_PlaySEWithPan(SEIDX_SE_FINISHMOVE, 0x80);
            EasyFade_FadeBothDisplays(2, 0x10, 0x1000);
            gBattle->flags |= BATTLE_FLAG_FIELDS_STOPPED;
            gBattle->flags |= BATTLE_FLAG_SANDSTORM_STOPPED;
            self->phase++;
            return;
        case 1:
            if (gBattle->flags & BATTLE_FLAG_SANDSTORM) {
                return;
            }
            if (gBattle->stages[0].flags & BATTLE_STAGE_FLAG_BG_IN_USE) {
                return;
            }
            if (gBattle->stages[1].flags & BATTLE_STAGE_FLAG_BG_IN_USE) {
                return;
            }
            func_ov003_020c14ec();
            func_ov003_020c1224(self, func_ov003_020c1918);
            return;
    }
}

static void func_ov003_020c1918(BtlActSp* self) {
    BtlActSpCutins   cutins   = data_ov003_020ddab8;
    BtlActSpFeatures features = data_ov003_020ddaf8;

    switch (self->phase) {
        case 0:
            PaletteMgr_UnknownLoop(g_PaletteManagers[1]);
            gBattle->flags |= BATTLE_FLAG_NO_PAUSE;
            gBattle->specialFreeze = 1;
            gBattle->unk3D8C8      = 0;
            EasyTask_CleanupAllTasks(&gBattle->psychPool);
            Mem_ValidateSequences(&gMainHeap);
            if (self->character == 3) {
                self->taskId = BtlCutIn00_CreateKariyaYashiroTask(&gBattle->specialPool);
            } else {
                self->taskId = EasyTask_CreateTask(&gBattle->specialPool, cutins.entries[self->level], NULL, 0, NULL, NULL);
            }
            self->phase++;
            BtlCutIn_SetSpecialKind(1);
            BtlFeat_HaltEnm();
            return;
        case 1:
            if (EasyTask_ValidateTaskId(&gBattle->specialPool, &self->taskId)) {
                return;
            }
            gBattle->specialFreeze = 0;
            gBattle->unk3D888      = 1;
            gBattle->flags |= BATTLE_FLAG_FEAT;
            BtlFeat_SaveEnmPos();
            if (self->skipFeature) {
                self->phase++;
                return;
            }
            if (self->character == 3) {
                self->taskId = EasyTask_CreateTask(&gBattle->specialPool, &data_ov024_020f6b7c, NULL, 0, NULL, NULL);
            } else {
                self->taskId = EasyTask_CreateTask(&gBattle->specialPool, features.entries[self->character][self->level], NULL,
                                                   0, NULL, NULL);
            }
            self->phase++;
            self->timer = 2;
            return;
        case 2:
            if (EasyTask_ValidateTaskId(&gBattle->specialPool, &self->taskId)) {
                return;
            }
            data_ov003_020e71d8.released = 1;
            self->phase++;
            self->timer--;
            if (self->timer) {
                return;
            }
            PaletteMgr_UnknownLoop(g_PaletteManagers[1]);
            return;
        case 3:
            if (EasyTask_ValidateTaskId(&gBattle->specialPool, &self->taskId)) {
                return;
            }
            gBattle->flags &= ~BATTLE_FLAG_FEAT;
            BtlFeat_RestoreEnmPos();
            gBattle->unk3D888 = 0;
            self->phase++;
            self->timer = 0;
            gBattle->flags &= ~BATTLE_FLAG_FIELDS_STOPPED;
            gBattle->flags &= ~BATTLE_FLAG_SANDSTORM_STOPPED;
            BtlStage_StartFieldTasks(1);
            BtlStage_StartSandstorm();
            if (self->character == 3) {
                return;
            }
            EasyFade_FadeBothDisplays(0, 0, 0x800);
            return;
        case 4:
            if (self->character == 3) {
                if (!self->timer) {
                    Ov003EnemyActor* unit  = BtlList_GetHead(&gBattle->enemiesSub);
                    BattleActor*     owner = gBattle->partner;
                    BtlHit_AttackCharacter(BtlHit_GetAttackInfo(0x12D), &unit->actor.baseArgs, owner->position.x);
                    unit  = BtlList_GetHead(&gBattle->enemiesMain);
                    owner = gBattle->player;
                    BtlHit_AttackCharacter(BtlHit_GetAttackInfo(0x12E), &unit->actor.baseArgs, owner->position.x);
                }
                gBattle->flags &= ~BATTLE_FLAG_NO_PAUSE;
                self->active = 0;
                EasyFade_FadeBothDisplays(0, 0, 0x800);
                self->timer++;
                return;
            }
            if (!self->skipFeature) {
                func_ov003_020c1238(self->character, self->level);
            } else {
                func_ov003_020c17bc();
            }
            gBattle->flags &= ~BATTLE_FLAG_NO_PAUSE;
            self->active = 0;
            return;
    }
}

// Nonmatching: argument-load scheduling and register allocation.
static s32 BtlActSp00_Init(TaskPool* pool, Task* task, void* args) {
    BtlActSp*     self  = task->data;
    BtlActSpArgs* spawn = args;

    gBattle->flags |= BATTLE_FLAG_SPECIAL;
    MI_CpuSet(self, 0, sizeof(*self));
    data_ov003_020e71d8.released  = 0;
    self->selection               = spawn->selection;
    self->skipFeature             = spawn->skipFeature;
    data_ov003_020e71d8.level     = spawn->level;
    data_ov003_020e71d8.character = self->character;
    self->active                  = 1;
    func_ov003_020c1224(self, func_ov003_020c1848);
    return 1;
}

static s32 BtlActSp00_Update(TaskPool* pool, Task* task, void* args) {
    BtlActSp* self = task->data;

    if (self->update) {
        self->update(self);
    }
    return self->active;
}

static s32 BtlActSp00_Render(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 BtlActSp00_Release(TaskPool* pool, Task* task, void* args) {
    EasyTask_CleanupAllTasks(&gBattle->specialPool);
    BtlFeat_RestoreEnmPos();
    gBattle->flags &= ~BATTLE_FLAG_SPECIAL;
    gBattle->flags &= ~BATTLE_FLAG_SPECIAL_LOCKED;
    Mem_ValidateSequences(&gMainHeap);
    return 1;
}

static s32 BtlActSp00_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlActSp00_Init,
        .update     = BtlActSp00_Update,
        .render     = BtlActSp00_Render,
        .cleanup    = BtlActSp00_Release,
    };

    return stages.iter[stage](pool, task, args);
}

void func_ov003_020c1f18(TaskPool* pool, u16 level) {
    BtlActSpArgs args;
    u16          character;

    switch (gBattle->partnerId) {
        case 1:
            character = 1;
            break;
        case 2:
            character = 2;
            break;
        default:
        case 0:
            character = 0;
            break;
    }
    args.level       = level;
    args.character   = character;
    args.skipFeature = 0;
    EasyTask_CreateTask(pool, &Tsk_BtlActSp00, NULL, 0, NULL, &args);
}

s32 func_ov003_020c1f94(TaskPool* pool, s32 character) {
    BtlActSpArgs args;

    switch (character) {
        case 1:
            args.character = 1;
            break;
        case 2:
            args.character = 2;
            break;
        default:
        case 0:
            args.character = 0;
            break;
    }
    args.level       = 1;
    args.skipFeature = 1;
    return EasyTask_CreateTask(pool, &Tsk_BtlActSp00, NULL, 0, NULL, &args);
}

s32 func_ov003_020c2008(TaskPool* pool) {
    BtlActSpArgs args;

    args.character     = 3;
    args.level         = 0;
    args.skipFeature   = 0;
    BattleActor* owner = gBattle->partner;
    owner->flags &= ~ACTOR_FLAG_GUARDING;
    return EasyTask_CreateTask(pool, &Tsk_BtlActSp00, NULL, 0, NULL, &args);
}
