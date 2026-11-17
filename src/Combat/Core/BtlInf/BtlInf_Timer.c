#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Save.h"

#include "Combat/Core/BtlInf.h"

#include <nitro/mi/cpumem.h>

// The battle clock: shows the battle clear time as mm:ss.hh, saturating at 99:59.99.

typedef struct BtlInf_Timer {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ u16          minutes[2]; // least significant digit first
    /* 0x64 */ u16          seconds[2];
    /* 0x68 */ u16          hundredths[2];
    /* 0x6C */ u16          unk_6C;
    /* 0x6E */ u16          pad_6E;
    /* 0x70 */ void (*update)(struct BtlInf_Timer* timer);
    /* 0x74 */ u16  state;
    /* 0x76 */ u16  count;
    /* 0x78 */ BOOL isVisible;
} BtlInf_Timer; // Size: 0x7C

static s32 BtlInf_Timer_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlInf_Timer = {"Tsk_BtlInf_Timer", BtlInf_Timer_RunTask, sizeof(BtlInf_Timer)};

static void func_ov003_0209de24(BtlInf_Timer* timer, void (*update)(BtlInf_Timer*)) {
    timer->update = update;
    timer->state  = 0;
    timer->count  = 0;
}

/// Splits the battle clear time into its displayed digits.
// Nonmatching: division scheduling and regalloc
static void func_ov003_0209de38(BtlInf_Timer* timer) {
    u32 frames     = gSaveData.battleClearTime;
    u16 hundredths = (frames * 100 / 60) % 100;
    u16 elapsed    = frames / 60;
    u16 minutes    = (elapsed / 60) % 100;
    u16 seconds    = elapsed % 60;

    if (frames >= 100 * 60 * 60) {
        timer->hundredths[0] = 9;
        timer->hundredths[1] = 9;
        timer->seconds[0]    = 9;
        timer->seconds[1]    = 5;
        timer->minutes[0]    = 9;
        timer->minutes[1]    = 9;
    } else {
        timer->hundredths[0] = hundredths % 10;
        timer->hundredths[1] = hundredths / 10;
        timer->seconds[0]    = seconds % 10;
        timer->seconds[1]    = seconds / 10;
        timer->minutes[0]    = minutes % 10;
        timer->minutes[1]    = minutes / 10;
    }
}

static s32 BtlInf_Timer_Init(TaskPool* pool, Task* task, void* args) {
    BtlInf_Timer* timer = task->data;

    MI_CpuSet(timer, 0, sizeof(BtlInf_Timer));
    gBattle->comboCount = 0;
    timer->isVisible    = TRUE;
    timer->count        = 0;
    timer->state        = 0;
    timer->unk_6C       = 0;
    func_ov003_0209de24(timer, func_ov003_0209de38);
    BtlSprite_LoadDirect(0, &timer->sprite, &Iden_BtlMenu, 0x25, 0x27, 0x26, 0x3D, 0x37);
    BtlSprite_SetPriority(&timer->sprite, 0);
    BtlSprite_SetSortKey(&timer->sprite, 0);
    return 1;
}

static s32 BtlInf_Timer_Update(TaskPool* pool, Task* task, void* args) {
    BtlInf_Timer* timer = task->data;

    if (timer->update != NULL) {
        timer->update(timer);
    }
    return 1;
}

#define DRAW_TIMER_DIGIT(digit, x)                             \
    BtlSprite_SetAnim(&timer->sprite, (s16)((digit) + 20), 0); \
    BtlSprite_SetPosition(&timer->sprite, (x), 170);           \
    BtlSprite_Update(&timer->sprite);                          \
    BtlSprite_Render(&timer->sprite)

static s32 BtlInf_Timer_Render(TaskPool* pool, Task* task, void* args) {
    BtlInf_Timer* timer = task->data;

    if (timer->isVisible == FALSE) {
        return 1;
    }
    DRAW_TIMER_DIGIT(timer->hundredths[0], 240);
    DRAW_TIMER_DIGIT(timer->hundredths[1], 232);
    DRAW_TIMER_DIGIT(timer->seconds[0], 221);
    DRAW_TIMER_DIGIT(timer->seconds[1], 213);
    DRAW_TIMER_DIGIT(timer->minutes[0], 204);
    if (timer->minutes[1] != 0) {
        DRAW_TIMER_DIGIT(timer->minutes[1], 196);
    }
    BtlSprite_SetAnim(&timer->sprite, 0x37, 0);
    BtlSprite_SetPosition(&timer->sprite, 196, 170);
    BtlSprite_Update(&timer->sprite);
    BtlSprite_Render(&timer->sprite);
    return 1;
}
#undef DRAW_TIMER_DIGIT

static s32 BtlInf_Timer_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlInf_Timer_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlInf_Timer_Init,
        .update     = BtlInf_Timer_Update,
        .render     = BtlInf_Timer_Render,
        .cleanup    = BtlInf_Timer_Release,
    };
    return stages.iter[stage](pool, task, args);
}
