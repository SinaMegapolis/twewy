#ifndef TWEWY_COMBAT_CORE_COMBATANIM_H
#define TWEWY_COMBAT_CORE_COMBATANIM_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "Combat/Core/CombatSpriteSlot.h"
#include "SpriteMgr.h"

/// Playback cursor over a 4-byte-per-entry animation script (`func_ov003_020d38ac`).
typedef struct Ov003AnimScript {
    /* 0x00 */ const struct Ov003AnimScriptStep* steps;
    /* 0x04 */ u16                               index;
    /* 0x06 */ u16                               loop;
    /* 0x08 */ u16                               flags;
} Ov003AnimScript; // Size: 0xA

typedef struct Ov003AnimScriptStep {
    /* 0x00 */ s16 dataId; // -1 terminates the script
    /* 0x02 */ u16 entry;
} Ov003AnimScriptStep;     // Size: 0x4

/// Sprite-loading command shared by `func_ov003_020d36fc`/`func_ov003_020d37a0`. The tables are
/// arrays of 8-byte `SpriteAnimEntry` records; only `table` is actually read by the loader.
typedef struct Ov003SpriteLoadCmd {
    /* 0x00 */ const SpriteAnimEntry* entries;
    /* 0x04 */ const SpriteAnimEntry* table;
    /* 0x08 */ s32                    unk_08;
    /* 0x0C */ u16                    unk_0C;
} Ov003SpriteLoadCmd; // Size: 0x10

/// Scripted translation stepping shared with overlays 017/020/021 (owned by those units).
typedef struct Ov003Shake {
    /* 0x00 */ s16                          frame; // frames spent on the current step
    /* 0x02 */ s16                          index;
    /* 0x04 */ const struct Ov003ShakeStep* steps;
    /* 0x08 */ s32                          offsetX;
    /* 0x0C */ s32                          offsetY;
} Ov003Shake; // Size: 0x10

// Each 12-byte step holds a duration and position. On rollover, the runtime reads
// the next step's position while advancing the current index.
typedef struct Ov003ShakeStep {
    /* 0x00 */ s16 duration;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s32 offsetX;
    /* 0x08 */ s32 offsetY;
} Ov003ShakeStep; // Size: 0xC

void func_ov003_020d3858(s32 engine, Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script,
                         const Ov003AnimScriptStep* steps, void* paletteSrc, u16 palIdx, u16 flip);
s32  func_ov003_020d38ac(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script);
void func_ov003_020d3978(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script,
                         const Ov003AnimScriptStep* steps, u16 loop);
s32  func_ov003_020d39ac(Ov003AnimScript* script);
void CombatAnim_LayerInitLoaded(Ov003AnimLayer*, Data*, s16, u32, u32, u32);
void CombatAnim_LayerInit(Ov003AnimLayer*, BinIdentifier*, s32, void**, const Ov003AnimFrame*);
void CombatAnim_LayerSetFrames(Ov003AnimLayer*, const Ov003AnimFrame*);
void CombatAnim_LayerChain(Ov003AnimLayer*, Ov003AnimLayer*);
void CombatAnim_LayerUpdate(Ov003AnimLayer*);
void CombatAnim_LayerRelease(Ov003AnimLayer*);
PaletteResource* CombatAnim_SlotReloadPalette(Ov003SpriteSlot* slot);
void CombatAnim_SlotInit(Ov003SpriteSlot* slot, BinIdentifier* iden, const Ov003SpriteAnim* anims, void* charDest, u32 arg4,
                         u32 arg5);
void CombatAnim_SlotSetPalette(Ov003SpriteSlot* slot, PaletteResource* palette, BinIdentifier* iden, u32 engine, u8 palStart,
                               u8 palCount);
void CombatAnim_SlotSetAnims(Ov003SpriteSlot* slot, const Ov003SpriteAnim* anims);
void CombatAnim_SlotUpdate(Ov003SpriteSlot* slot);
void CombatAnim_SlotRelease(Ov003SpriteSlot* slot);
void CombatAnim_ShakeStart(Ov003Shake* shake, const Ov003ShakeStep* steps);
void CombatAnim_ShakeUpdate(Ov003Shake* shake);
void CombatAnim_ShakeApply(Ov003Shake* shake, CombatActor* actor);
void CombatAnim_ShakeStop(Ov003Shake* shake);

#endif // TWEWY_COMBAT_CORE_COMBATANIM_H
