#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlFeat.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SndMgr.h"
#include <nitro/mi/cpumem.h>

// The splash feature (`Tsk_BtlFeat_SplashEnm`): a short script that throws splash effects onto the enemies of both
// screens. Mode 1 walks both enemy lists and splashes one enemy of each screen in turn, mode 0 splashes at random
// positions around the middle of the screen, speeding up from `start` to `end` frames between splashes.

struct BtlFeatSplashEnm;

/// One step of a script: returns 2 to run the next step at once, 1 when the task is done and 0 to wait a frame.
typedef s32 (*BtlFeatSplashEnmStep)(struct BtlFeatSplashEnm*);

/// The task parameter.
typedef struct BtlFeatSplashEnmArgs {
    /* 0x00 */ u16 start; // frames between two splashes at the start
    /* 0x02 */ u16 end;   // ... and at the end
    /* 0x04 */ u16 count; // splashes in total (mode 0) or per enemy list (mode 1)
    /* 0x06 */ u16 rounds;
    /* 0x08 */ u16 mode;
    /* 0x0A */ u16 reserved0A;
    /* 0x0C */ s32 sound;
} BtlFeatSplashEnmArgs; // Size: 0x10

typedef struct BtlFeatSplashEnm {
    /* 0x00 */ BtlFeatSplashEnmStep* steps;
    /* 0x04 */ BtlFeatSplashEnmArgs  args;
    /* 0x14 */ u16                   delay;     // frames between the next two splashes
    /* 0x16 */ u16                   reserved16;
    /* 0x18 */ Ov003CutinActor*      subEnemy;  // the enemy of the sub screen list that is next
    /* 0x1C */ Ov003CutinActor*      mainEnemy; // ... and of the main screen list
    /* 0x20 */ s16                   step;
    /* 0x22 */ u16                   timer;
    /* 0x24 */ u16                   shots;
    /* 0x26 */ u16                   ticks;
    /* 0x28 */ u16                   rounds;
    /* 0x2A */ u8                    reserved2A[0x40 - 0x2A];
} BtlFeatSplashEnm; // Size: 0x40

extern s32 data_ov003_020e71e0;

static s32 func_ov003_020bc088(BtlFeatSplashEnm* self);
static s32 func_ov003_020bc0ac(BtlFeatSplashEnm* self);
static s32 func_ov003_020bc080(BtlFeatSplashEnm* self);
static s32 func_ov003_020bc25c(BtlFeatSplashEnm* self);
static s32 func_ov003_020bc280(BtlFeatSplashEnm* self);

/// The script of mode 1: both enemy lists in turn.
static BtlFeatSplashEnmStep data_ov003_020e50d4[3] = {func_ov003_020bc088, func_ov003_020bc0ac, func_ov003_020bc080};
/// The script of mode 0: random splashes.
static BtlFeatSplashEnmStep data_ov003_020e50e0[3] = {func_ov003_020bc25c, func_ov003_020bc280, func_ov003_020bc080};

static s32 BtlFeat_SplashEnm_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlFeat_SplashEnm = {"Tsk_BtlFeat_SplashEnm", BtlFeat_SplashEnm_RunTask, sizeof(BtlFeatSplashEnm)};

static void func_ov003_020bc068(BtlFeatSplashEnm* self) {
    self->step++;
}

static void func_ov003_020bc078(BtlFeatSplashEnm* self, BtlFeatSplashEnmStep* steps) {
    self->steps = steps;
}

static s32 func_ov003_020bc080(BtlFeatSplashEnm* self) {
    return 1;
}

static s32 func_ov003_020bc088(BtlFeatSplashEnm* self) {
    self->timer  = 0;
    self->shots  = 0;
    self->ticks  = 0;
    self->rounds = 0;
    func_ov003_020bc068(self);
    return 2;
}

static s32 func_ov003_020bc0ac(BtlFeatSplashEnm* self) {
    self->ticks++;
    if (self->timer == 0) {
        self->timer = self->args.start;
        if (self->mainEnemy == NULL && self->subEnemy == NULL) {
            self->rounds++;
            self->subEnemy  = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8A4);
            self->mainEnemy = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8AC);
        }
        if (!(self->rounds < self->args.rounds && self->shots < self->args.count)) {
            func_ov003_020bc068(self);
            return 2;
        }
        if ((self->shots & 1) && self->mainEnemy != NULL) {
            s32 x = self->mainEnemy->base.actor.position.x;
            func_ov003_0209fc88(1, x, self->mainEnemy->base.actor.position.y, self->mainEnemy->base.actor.position.z, 4, 1);
            if (self->args.sound != -1) {
                SndMgr_PlaySEWithPan(self->args.sound, func_ov003_020843b0(1, x));
            }
            self->mainEnemy = func_ov003_02084984(&self->mainEnemy->base.node);
        } else if (self->subEnemy != NULL) {
            s32 x = self->subEnemy->base.actor.position.x;
            func_ov003_0209fc88(2, x, self->subEnemy->base.actor.position.y, self->subEnemy->base.actor.position.z, 4, 1);
            if (self->args.sound != -1) {
                SndMgr_PlaySEWithPan(self->args.sound, func_ov003_020843b0(0, x));
            }
            self->subEnemy = func_ov003_02084984(&self->subEnemy->base.node);
        }
        self->shots++;
    } else {
        self->timer--;
    }
    return 0;
}

static s32 func_ov003_020bc25c(BtlFeatSplashEnm* self) {
    self->timer = 0;
    self->shots = 0;
    self->delay = self->args.start;
    func_ov003_020bc068(self);
    return 0;
}

static s32 func_ov003_020bc280(BtlFeatSplashEnm* self) {
    s32 x;
    s32 y;
    s32 z;
    u16 timer;
    s32 randX;
    s32 randY;
    u16 bitX;
    u16 bitY;
    s32 odd;

    if (self->shots == 0) {
        self->shots = self->delay;
    } else {
        self->shots--;
        return 0;
    }
    randX = RNG_Next(0x81);
    randY = RNG_Next(0x61);
    timer = self->timer;
    bitX  = (timer >> 1) & 1;
    odd   = timer & 1;
    x     = (s32)I2F_RND(bitX * 0x80) + (randX << 12);
    bitY  = (timer >> 2) & 1;
    y     = (s32)I2F_RND(bitY * 0x60) + (randY << 12);
    if (odd) {
        z = func_ov003_02084460(0, (y * 0x10) >> 16, 0);
        func_ov003_0209fc88(2, x, z, 0, 4, 1);
        if (self->args.sound != -1) {
            SndMgr_PlaySEWithPan(self->args.sound, func_ov003_020843b0(0, x));
        }
    } else {
        z = func_ov003_02084460(1, (y * 0x10) >> 16, 0);
        func_ov003_0209fc88(1, x, z, 0, 4, 1);
        if (self->args.sound != -1) {
            SndMgr_PlaySEWithPan(self->args.sound, func_ov003_020843b0(1, x));
        }
    }
    self->timer++;
    timer       = self->timer;
    self->delay = (u16)((double)self->args.start * (1.0 - (double)timer / self->args.count) +
                        (double)self->args.end * ((double)timer / self->args.count));
    if (self->timer == self->args.count) {
        func_ov003_020bc068(self);
    }
    return 0;
}

static s32 BtlFeat_SplashEnm_Init(TaskPool* pool, Task* task, void* args) {
    BtlFeatSplashEnm*     self  = task->data;
    BtlFeatSplashEnmArgs* param = args;

    MI_CpuSet(self, 0, sizeof(*self));
    self->args      = *param;
    self->subEnemy  = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8A4);
    self->mainEnemy = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8AC);
    switch (param->mode) {
        case 1:
            func_ov003_020bc078(self, data_ov003_020e50d4);
            break;
        case 0:
            func_ov003_020bc078(self, data_ov003_020e50e0);
            break;
    }
    return 1;
}

static s32 BtlFeat_SplashEnm_Update(TaskPool* pool, Task* task, void* args) {
    BtlFeatSplashEnm* self = task->data;
    s32               result;

    if (self->steps[self->step] == NULL) {
        return 0;
    }
    do {
        result = self->steps[self->step](self);
    } while (result == 2);
    if (result == 1) {
        return 0;
    }
    return data_ov003_020e71e0 != 1;
}

static s32 BtlFeat_SplashEnm_Render(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 BtlFeat_SplashEnm_Release(TaskPool* pool, Task* task, void* args) {
    return 0;
}

static s32 BtlFeat_SplashEnm_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlFeat_SplashEnm_Init,
        .update     = BtlFeat_SplashEnm_Update,
        .render     = BtlFeat_SplashEnm_Render,
        .cleanup    = BtlFeat_SplashEnm_Release,
    };

    return stages.iter[stage](pool, task, args);
}

void func_ov003_020bc690(u16 start, u16 count, s32 sound) {
    BtlFeatSplashEnmArgs args;

    args.start = start;
    args.end   = start;
    args.count = count;
    args.mode  = 0;
    args.sound = sound;
    EasyTask_CreateTask(&data_ov003_020e71b8->unk_34230, &Tsk_BtlFeat_SplashEnm, NULL, 0, NULL, &args);
}
