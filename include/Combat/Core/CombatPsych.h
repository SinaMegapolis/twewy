#ifndef TWEWY_COMBAT_CORE_COMBATPSYCH_H
#define TWEWY_COMBAT_CORE_COMBATPSYCH_H

#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/CombatAttack.h"
#include "Engine/EasyTask.h"
#include <nitro/fx/fx_vector.h>

typedef struct Ov003EffectSlots {
    union {
        struct {
            s32 unk_00;
            s32 unk_04;
            s32 unk_08;
        };
        Vec position;
    };
    /* 0x0C */ union {
        s32 unk_0C;
        void (*onAbsorb)(void*);
    };
    /* 0x10 */ union {
        s32   unk_10;
        void* callbackArg;
    };
    /* 0x14 */ u8 cooldown;
    /* 0x15 */ u8 unk_15;
} Ov003EffectSlots; // Size: 0x18

typedef union Ov003PsychParamsHead {
    struct {
        u16 unk_00;
        u16 unk_02;
    };
    u32 raw; // actor creation copies PsyData records a word at a time
} Ov003PsychParamsHead;

typedef struct Ov003PsychParams {
    /* 0x00 */ Ov003PsychParamsHead head;
    /* 0x04 */ u8                   unk_04;
    /* 0x05 */ u8                   unk_05; // psych kind
    /* 0x06 */ u16                  unk_06;
    /* 0x08 */ u16                  unk_08;
    /* 0x0A */ s16                  unk_0A;
    /* 0x0C */ s16                  unk_0C;
    /* 0x0E */ u16                  unk_0E;
    /* 0x10 */ s16                  unk_10;
    /* 0x12 */ u16                  unk_12;
    /* 0x14 */ u16                  unk_14;
    /* 0x16 */ s16                  unk_16;
    /* 0x18 */ u16                  unk_18;
    /* 0x1A */ u16                  unk_1A;
    /* 0x1C */ s16                  unk_1C;
    /* 0x1E */ u16                  unk_1E;
    /* 0x20 */ u16                  unk_20;
    /* 0x22 */ s16                  unk_22;
    /* 0x24 */ u16                  unk_24;
    /* 0x26 */ u16                  unk_26;
    /* 0x28 */ u16                  unk_28;
    /* 0x2A */ u16                  unk_2A;
    /* 0x2C */ u16                  unk_2C;
    /* 0x2E */ u8                   unk_2E;
    /* 0x2F */ u8                   unk_2F;
    /* 0x30 */ u8                   unk_30;
    /* 0x31 */ u8                   unk_31;
    /* 0x32 */ u16                  unk_32;
} Ov003PsychParams; // Size: 0x34

typedef struct Ov003EffectContext {
    /* 0x0 */ s32 (*callback)(void*);   // run on start with `arg`
    /* 0x4 */ const TaskHandle* handle; // optional task spawned on start
    /* 0x8 */ void*             arg;    // passed to `callback`/the task
    /* 0xC */ s32 (*post)(void*);       // run after the effect start callback
} Ov003EffectContext;                   // Size: 0x10

typedef struct Ov003EffectState {
    union {
        struct {
            /* 0x00 */ s32              state; // table index; 0x3C while starting
            /* 0x04 */ s32              index; // per-actor velocity ramp slot
            /* 0x08 */ s16              unk_08;
            /* 0x0A */ s16              unk_0A;
            /* 0x0C */ BattleAttackArgs args; // what the psych's hits deal
            /* 0x14 */ u8               flag;
            /* 0x15 */ u8               unk_15[3];
        };
        Ov003AttackStreak streak;
    };
    union {
        /* 0x18 */ u8  data[0x10];
        /* 0x18 */ s32 params[4]; // kind-specific PsyData words
    };
    /* 0x28 */ s32                result;
    /* 0x2C */ Ov003EffectContext context;
} Ov003EffectState; // Size: 0x3C

typedef struct Ov003TouchActor {
    /* 0x000 */ s32              state; // 0..4, 0xFF = free
    /* 0x004 */ s32              request;
    /* 0x008 */ u8               slot;
    /* 0x009 */ u8               unk_009[1];
    /* 0x00A */ u16              kind; // 0xFFFE = the player actor
    /* 0x00C */ u16              unk_00C;
    /* 0x00E */ u8               unk_00E;
    /* 0x00F */ u8               unk_00F;
    /* 0x010 */ Ov003PsychParams params;
    /* 0x044 */ s16              unk_044;
    /* 0x046 */ s16              unk_046;
    /* 0x048 */ s16              unk_048;
    /* 0x04A */ u8               unk_04A[2];
    /* 0x04C */ s32              unk_04C;
    /* 0x050 */ s16              unk_050;
    /* 0x052 */ s16              unk_052;
    /* 0x054 */ s32              unk_054;
    /* 0x058 */ BattleSprite     sprite;
    /* 0x0B8 */ s16              radius; // collision radius, pixels
    /* 0x0BC */ s32              x;      // 20.12 fixed point
    /* 0x0C0 */ s32              y;
    /* 0x0C4 */ s32              velocityX;
    /* 0x0C8 */ s32              velocityY;
    /* 0x0CC */ Ov003EffectState effect;
    /* 0x108 */ BattleListNode   node;
} Ov003TouchActor; // Size: 0x114

typedef struct Ov003AnimSprite {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ s16          timer;
    /* 0x62 */ s16          unk_62;
    /* 0x64 */ s32          animTimer;
    /* 0x68 */ s32          decay;
    /* 0x6C */ u16          angle;
    /* 0x6E */ s16          unk_6E;
} Ov003AnimSprite; // Size: 0x70

void func_ov003_020a5540(Ov003EffectState* effect, Ov003TouchActor* actor);
void func_ov003_020a563c(Ov003EffectState* effect);
s32  func_ov003_020a58c4(void);
s32  func_ov003_020a58d4(void);
void func_ov003_020a58e4(Ov003EffectState* effect, u16 arg1, u8 arg2);
/// Records the touch-slot origins used by the effect selection.
void func_ov003_020a593c(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_ov003_020a59dc(void);
u8   func_ov003_020a5a1c(void);
/// Finds the badge that owns the effect state at `effect`.
Ov003TouchActor* func_ov003_020a5a34(Ov003EffectState* effect);
void             func_ov003_020a5a94(Ov003EffectState* effect);
s32              func_ov003_020a5aa4(Ov003EffectState* effect);
s32              func_ov003_020a5ab4(Ov003EffectState* effect);
s32              func_ov003_020a5ac4(Ov003TouchActor** slots, s32 count, Ov003EffectState* effect, s32 requireReady);
/// Sums the per-actor amount of every actor running effect `state`.
s32   func_ov003_020a5b18(s32 state);
void* func_ov003_020a5d2c(s32 size, const char* name);
/// Staggers the countdown timers of `count` sprite slots.
void func_ov003_020a5d88(Ov003AnimSprite* sprites, s32 count, s32 step);

#endif // TWEWY_COMBAT_CORE_COMBATPSYCH_H
