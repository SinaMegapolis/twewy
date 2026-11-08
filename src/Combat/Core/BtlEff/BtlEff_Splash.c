#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/Math/Random.h"
#include <nitro/fx.h>
#include <nitro/mi/cpumem.h>

// The splash effect: up to sixteen particles that burst out of one point in random directions and
// vanish after 20 frames. Sub-screen splashes use the active partner's particle sprites.

typedef struct BtlSplashParticle {
    /* 0x00 */ CombatSprite sprite;
    /* 0x60 */ s32          angle;
    /* 0x64 */ s32          speed;
    /* 0x68 */ Vec          position;
} BtlSplashParticle; // Size: 0x74

typedef struct BtlEffSplash {
    /* 0x000 */ s32               engine;
    /* 0x004 */ BtlSplashParticle particles[16];
    /* 0x744 */ s16               count;
    /* 0x746 */ u16               pad746;
    /* 0x748 */ Vec               position;
    /* 0x754 */ s16               timer;
    /* 0x756 */ u16               pad756;
    /* 0x758 */ BOOL              independent; // not tied to the battle's pause/arming flags
    /* 0x75C */ BOOL              isActive;
} BtlEffSplash;                                // Size: 0x760

typedef struct BtlEffSplashArgs {
    /* 0x00 */ s32  engine;
    /* 0x04 */ Vec  position;
    /* 0x10 */ s16  count;
    /* 0x12 */ u16  pad;
    /* 0x14 */ BOOL independent;
} BtlEffSplashArgs; // Size: 0x18

static s32 BtlEff_Splash_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlEff_Splash = {"Tsk_BtlEff_Splash", BtlEff_Splash_RunTask, sizeof(BtlEffSplash)};

extern const BinIdentifier data_ov003_020d77d8, data_ov003_020d77e0, data_ov003_020d77e8, data_ov003_020d77f0;

/// Loads the sub-screen particle sprite of the active partner.
static void func_ov003_0209f7c4(CombatSprite* sprite, u16 anim) {
    switch (g_CombatGlobal->unk3D88D) {
        case 1:
            CombatSprite_LoadDirect(1, sprite, &data_ov003_020d77e8, 40, 42, 41, 44, anim);
            break;
        case 2:
            CombatSprite_LoadDirect(1, sprite, &data_ov003_020d77f0, 44, 46, 45, 47, anim);
            break;
        case 0:
        default:
            CombatSprite_LoadDirect(1, sprite, &data_ov003_020d77e0, 35, 37, 36, 38, anim);
            break;
    }
}

/// Spreads the particles evenly around a random start angle, each with a random speed and sprite.
static void func_ov003_0209f8b0(BtlEffSplash* splash) {
    s32                angle = RNG_Next(256) << 8;
    u16                anim;
    s32                i;
    BtlSplashParticle* particle = splash->particles;

    for (i = 0; i < splash->count; i++) {
        particle             = &splash->particles[i];
        anim                 = RNG_Next(5);
        particle->position.x = splash->position.x;
        particle->position.y = splash->position.y;
        particle->position.z = splash->position.z;
        particle->angle      = angle + (RNG_Next(256) << 8) / (splash->count * 2);
        particle->speed      = (RNG_Next(13) + 6) << 12;
        if (splash->engine == 1) {
            func_ov003_0209f7c4(&particle->sprite, anim);
        } else {
            CombatSprite_LoadDirect(2, &particle->sprite, &data_ov003_020d77d8, 86, 88, 87, 89, anim);
        }
        CombatSprite_SetAnim(&particle->sprite, anim, 1);
        angle += 0x10000 / splash->count;
    }
}

static void func_ov003_0209f9c4(BtlEffSplash* splash) {
    s32                i;
    BtlSplashParticle* particle;

    for (i = 0; i < splash->count; i++) {
        particle = &splash->particles[i];
        BtlMgr_MoveByAngle(&particle->position.x, &particle->position.y, particle->angle, particle->speed);
        CombatSprite_Update(&particle->sprite);
    }
}

static void func_ov003_0209fa14(BtlEffSplash* splash) {
    s32                x, y, key;
    s32                i;
    BtlSplashParticle* particle;

    for (i = 0; i < splash->count; i++) {
        particle = &splash->particles[i];

        if (particle->sprite.sprite.bits_0_1 == 1) {
            x   = CombatStage_ToScreenX(1, particle->position.x);
            y   = CombatStage_ToScreenY(1, particle->position.y, particle->position.z);
            key = 0x7FFFFFFF - (particle->position.y + 0x8000);
        } else {
            x   = CombatStage_ToScreenX(0, particle->position.x);
            y   = CombatStage_ToScreenY(0, particle->position.y, particle->position.z);
            key = y + 8;
        }
        CombatSprite_SetPosition(&particle->sprite, x, y);
        func_ov003_02082730(&particle->sprite, key);
        CombatSprite_Render(&particle->sprite);
    }
}

static void func_ov003_0209fae0(BtlEffSplash* splash) {
    s32                i;
    BtlSplashParticle* particle;

    for (i = 0; i < splash->count; i++) {
        particle = &splash->particles[i];
        CombatSprite_Release(&particle->sprite);
    }
}

static s32 BtlEff_Splash_Init(TaskPool* pool, Task* task, void* args) {
    BtlEffSplash*     splash = task->data;
    BtlEffSplashArgs* params = args;

    MI_CpuSet(splash, 0, sizeof(*splash));
    splash->independent = params->independent;
    splash->isActive    = TRUE;
    splash->timer       = 20;
    splash->engine      = params->engine;
    splash->position.x  = params->position.x;
    splash->position.y  = params->position.y;
    splash->position.z  = params->position.z;
    splash->count       = params->count;
    func_ov003_0209f8b0(splash);
    return 1;
}

static s32 BtlEff_Splash_Update(TaskPool* pool, Task* task, void* args) {
    BtlEffSplash* splash = task->data;

    if (!splash->independent && (g_CombatGlobal->unk3D878 & 6)) {
        return 1;
    }
    if (splash->timer > 0) {
        splash->timer--;
    } else {
        splash->isActive = FALSE;
    }
    func_ov003_0209f9c4(splash);
    return splash->isActive;
}

static s32 BtlEff_Splash_Render(TaskPool* pool, Task* task, void* args) {
    BtlEffSplash* splash = task->data;

    if (!splash->independent && (g_CombatGlobal->unk3D878 & 6)) {
        return 1;
    }
    func_ov003_0209fa14(splash);
    return 1;
}

static s32 BtlEff_Splash_Release(TaskPool* pool, Task* task, void* args) {
    func_ov003_0209fae0(task->data);
    return 1;
}

static s32 BtlEff_Splash_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlEff_Splash_Init,
        .update     = BtlEff_Splash_Update,
        .render     = BtlEff_Splash_Render,
        .cleanup    = BtlEff_Splash_Release,
    };

    return stages.iter[stage](pool, task, args);
}

void func_ov003_0209fc88(s32 engine, s32 x, s32 y, s32 z, s16 count, s32 independent) {
    BtlEffSplashArgs params;

    params.engine      = engine;
    params.position.x  = x;
    params.position.y  = y;
    params.position.z  = z;
    params.count       = count;
    params.independent = independent;
    if (independent) {
        EasyTask_CreateTask(&g_CombatGlobal->unk_34230, &Tsk_BtlEff_Splash, NULL, 0, NULL, &params);
    } else if (g_CombatGlobal->unk3D878 & 0x80) {
        g_CombatGlobal->unk3D878 &= ~0x80;
        EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlEff_Splash, NULL, 0, NULL, &params);
    }
}
