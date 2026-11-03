#include "Combat/Core/BtlCutIn.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"

static char                data_ov003_020e5170[20];
static Ov003ActorSnapshots data_ov003_020e71c8;

static u16 func_ov003_020bce6c(CombatList* list) {
    u16              count = 0;
    Ov003EnemyActor* unit  = func_ov003_0208495c(list);
    while (unit != NULL) {
        if (!(unit->actor.flags & 4))
            count++;
        unit = func_ov003_02084984(&unit->node);
    }
    return count;
}

static u16 func_ov003_020bceac(CombatList* list, Ov003ActorSnapshot** snapshots) {
    u16                 index   = 0;
    u16                 count   = func_ov003_020bce6c(list);
    Ov003ActorSnapshot* entries = Mem_AllocHeapTail(&gDebugHeap, count * sizeof(*entries));
    Mem_SetSequence(&gDebugHeap, entries, data_ov003_020e5170);
    *snapshots            = entries;
    Ov003EnemyActor* unit = func_ov003_0208495c(list);
    while (unit != NULL) {
        if (!(unit->actor.flags & 4)) {
            entries[index].actor = unit->actor;
            entries[index].unit  = unit;
            index++;
        }
        unit = func_ov003_02084984(&unit->node);
    }
    return count;
}

static void func_ov003_020bcf64(CombatList* list, s32 command) {
    Ov003EnemyActor* unit = func_ov003_0208495c(list);
    while (unit != NULL) {
        CombatActor_SetPendingCommand(&unit->actor, command);
        unit = func_ov003_02084984(&unit->node);
    }
}

static void func_ov003_020bcf98(CombatList* list, Ov003ActorSnapshot* snapshots, u32 count) {
    u16 index = 0;
    if (count == 0)
        return;
    do {
        Ov003EnemyActor* unit = func_ov003_0208495c(list);
        while (unit != NULL) {
            if (snapshots[index].unit == unit) {
                unit->actor = snapshots[index].actor;
                break;
            }
            unit = func_ov003_02084984(&unit->node);
        }
        index++;
    } while (index < count);
}

void func_ov003_020bd020(void) {
    data_ov003_020e71c8.mainCount = func_ov003_020bceac(&data_ov003_020e71b8->unk3D8AC, &data_ov003_020e71c8.main);
    data_ov003_020e71c8.subCount  = func_ov003_020bceac(&data_ov003_020e71b8->unk3D8A4, &data_ov003_020e71c8.sub);
}

void func_ov003_020bd078(void) {
    if (data_ov003_020e71c8.main != NULL) {
        func_ov003_020bcf98(&data_ov003_020e71b8->unk3D8AC, data_ov003_020e71c8.main, data_ov003_020e71c8.mainCount);
        Mem_Free(&gDebugHeap, data_ov003_020e71c8.main);
        data_ov003_020e71c8.main = NULL;
    }
    if (data_ov003_020e71c8.sub != NULL) {
        func_ov003_020bcf98(&data_ov003_020e71b8->unk3D8A4, data_ov003_020e71c8.sub, data_ov003_020e71c8.subCount);
        Mem_Free(&gDebugHeap, data_ov003_020e71c8.sub);
        data_ov003_020e71c8.sub = NULL;
    }
}

void func_ov003_020bd114(void) {
    func_ov003_020bcf64(&data_ov003_020e71b8->unk3D8AC, 6);
    func_ov003_020bcf64(&data_ov003_020e71b8->unk3D8A4, 6);
}

void func_ov003_020bd150(s32 mode, Ov003SnapshotCallback callback, void* args) {
    Ov003ActorSnapshot* entries;
    u16                 count;
    if (mode == 1) {
        count   = data_ov003_020e71c8.mainCount;
        entries = data_ov003_020e71c8.main;
    } else {
        count   = data_ov003_020e71c8.subCount;
        entries = data_ov003_020e71c8.sub;
    }
    for (u16 i = 0; i < count; i++)
        callback(mode, &entries[i], args);
}

static char                data_ov003_020e5170[20] = "S_FeatEnm_SavePos";
static Ov003ActorSnapshots data_ov003_020e71c8;

char Path_BtlSpecial3[]        = "Apl_Fuk/Grp_BtlSpecial3.bin";
char Path_BtlSpecial2_Shiki[]  = "Apl_Fuk/Grp_BtlSpecial2_Shiki.bin";
char Path_BtlSpecial2_Bito[]   = "Apl_Fuk/Grp_BtlSpecial2_Bito.bin";
char Path_BtlBgEff22_SCR[]     = "Apl_Fuk/Grp_BtlBgEff22_SCR.bin";
char Path_BtlSpecial2_Yoshua[] = "Apl_Fuk/Grp_BtlSpecial2_Yoshua.bin";
char Path_BtlSpecialYoshua1[]  = "Apl_Fuk/Grp_BtlSpecialYoshua1.bin";
char Path_BtlSpecial2[]        = "Apl_Fuk/Grp_BtlSpecial2.bin";
char Path_BtlSpecialBito[]     = "Apl_Fuk/Grp_BtlSpecialBito.bin";
char Path_BtlSpecialYoshua[]   = "Apl_Fuk/Grp_BtlSpecialYoshua.bin";
char Path_BtlSpecialYoshua2b[] = "Apl_Fuk/Grp_BtlSpecialYoshua2b.bin";
char Path_BtlSpecial2_Sini[]   = "Apl_Fuk/Grp_BtlSpecial2_Sini.bin";
char Path_BtlSpecialSiki[]     = "Apl_Fuk/Grp_BtlSpecialSiki.bin";
char Path_BtlSpecialYoshua2a[] = "Apl_Fuk/Grp_BtlSpecialYoshua2a.bin";

const BinIdentifier Iden_BtlSpecial2_Bito   = {6, Path_BtlSpecial2_Bito};
const BinIdentifier Iden_BtlSpecialYoshua1  = {5, Path_BtlSpecialYoshua1};
const BinIdentifier Iden_BtlBgEff22_SCR     = {6, Path_BtlBgEff22_SCR};
const BinIdentifier Iden_BtlSpecialSiki     = {4, Path_BtlSpecialSiki};
const BinIdentifier Iden_BtlSpecial2_Shiki  = {4, Path_BtlSpecial2_Shiki};
const BinIdentifier Iden_BtlSpecialYoshua   = {5, Path_BtlSpecialYoshua};
const BinIdentifier Iden_BtlSpecialBito     = {6, Path_BtlSpecialBito};
const BinIdentifier Iden_BtlSpecialYoshua2a = {5, Path_BtlSpecialYoshua2a};
const BinIdentifier Iden_BtlSpecial3        = {3, Path_BtlSpecial3};
const BinIdentifier Iden_BtlSpecial2_Yoshua = {5, Path_BtlSpecial2_Yoshua};
const BinIdentifier Iden_BtlSpecial2_Sini   = {3, Path_BtlSpecial2_Sini};
const BinIdentifier Iden_BtlSpecial2        = {3, Path_BtlSpecial2};
const BinIdentifier Iden_BtlSpecialYoshua2b = {5, Path_BtlSpecialYoshua2b};
