#ifndef TWEWY_COMBAT_CORE_COMBATABILITY_H
#define TWEWY_COMBAT_CORE_COMBATABILITY_H

#include <nitro/types.h>

typedef union Ov003AbilityFlags {
    struct {
        /* 0x0 */ u8 valid;
        /* 0x1 */ u8 usable[3];
    };
    u32 raw;
} Ov003AbilityFlags;

typedef struct Ov003AbilitySlot {
    /* 0x0 */ s32 actionId;
    /* 0x4 */ s32 amount;
} Ov003AbilitySlot; // Size: 0x8

typedef struct Ov003AbilityData {
    /* 0x00 */ Ov003AbilityFlags flags;
    /* 0x04 */ Ov003AbilitySlot  slots[4];
} Ov003AbilityData; // Size: 0x24

typedef struct Ov003TouchEvent {
    /* 0x00 */ u32              unk_00; // ability id, 0xFF = free slot
    /* 0x04 */ u32              anyTouch;
    /* 0x08 */ Ov003AbilityData ability;
    /* 0x2C */ u32              slot; // touch slot the event belongs to
} Ov003TouchEvent;                    // Size: 0x30

void CombatAbility_Queue(s32 id, s32 touchIndex, u32 slot);
s32  CombatAbility_IsActiveForPlayer(s32 actionId);
s32  CombatAbility_IsActiveForPartner(s32 actionId);
s32  CombatAbility_GetPlayerValue(s32 actionId);
s32  CombatAbility_GetPartnerValue(s32 actionId);
s32  CombatAbility_IsActive(s32);
s32  CombatAbility_GetValue(s32);

#endif // TWEWY_COMBAT_CORE_COMBATABILITY_H
