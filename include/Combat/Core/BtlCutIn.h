#ifndef TWEWY_COMBAT_CORE_BTLCUTIN_H
#define TWEWY_COMBAT_CORE_BTLCUTIN_H

#include "Combat/Core/CombatCutin.h"
#include "Engine/EasyTask.h"
#include "Engine/File/DatMgr.h"

extern const BinIdentifier Iden_BtlSpecial2;
extern const BinIdentifier Iden_BtlSpecial3;
extern const BinIdentifier Iden_BtlSpecial2_Sini;
extern const BinIdentifier Iden_BtlSpecialSiki;
extern const BinIdentifier Iden_BtlSpecial2_Shiki;
extern const BinIdentifier Iden_BtlSpecialYoshua;
extern const BinIdentifier Iden_BtlSpecialYoshua1;
extern const BinIdentifier Iden_BtlSpecialYoshua2a;
extern const BinIdentifier Iden_BtlSpecial2_Yoshua;
extern const BinIdentifier Iden_BtlSpecialYoshua2b;
extern const BinIdentifier Iden_BtlSpecialBito;
extern const BinIdentifier Iden_BtlBgEff22_SCR;
extern const BinIdentifier Iden_BtlSpecial2_Bito;

/// The partner's cut-in.
extern const TaskHandle Tsk_BtlCutIn00;

/// The first fusion cut-in.
extern const TaskHandle Tsk_BtlCutIn01;

/// The second fusion cut-in.
extern const TaskHandle Tsk_BtlCutIn02;

/// Spawns the Kariya and Yashiro variant of the partner's cut-in in `pool`.
s32 BtlCutIn00_CreateKariyaYashiroTask(TaskPool* pool);

#endif // TWEWY_COMBAT_CORE_BTLCUTIN_H
