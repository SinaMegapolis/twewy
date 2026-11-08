#include "Combat/Core/CombatEnemyGroup.h"

/// The sound archive each noise group loads.
const s8 CombatEnemyGroup_SoundArc[0x40] = {
    0x07, 0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0D, 0x0E, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x1B, 0x1A,
    0x0A, 0x0B, 0x0F, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x1F, 0x23, 0x23, 0x24, 0x24, 0x1E, 0x22, 0x1C, 0x1C, 0x1D, 0x1D, 0x04, 0x00, 0x00, 0x00, 0x00,
};

u8  data_ov003_020e6a30[8] = {0, 0, 0, 0, 1, 0, 0, 0};
s32 data_ov003_020e6a38[3] = {0, 1, 0};

extern char data_ov009_02124840[];
extern char data_ov009_02124904[];
extern char data_ov009_02124a34[];
extern char data_ov009_02124af4[];
extern char data_ov009_02124c44[];
extern char data_ov009_02124cc4[];
extern char data_ov009_02124d58[];
extern char data_ov009_02124dc0[];
extern char data_ov009_02124e84[];
extern char data_ov009_02124ff8[];
extern char data_ov009_02125240[];
extern char data_ov009_02125328[];
extern char data_ov009_021253ec[];
extern char data_ov009_02125564[];
extern char data_ov010_02129160[];
extern char data_ov010_02129320[];
extern char data_ov011_0212c460[];
extern char data_ov012_02127980[];
extern char data_ov013_02127540[];
extern char data_ov014_02134e0c[];
extern char data_ov015_021283f8[];
extern char data_ov017_02142d9c[];
extern char data_ov017_02142f00[];
extern char data_ov017_02142f6c[];
extern char data_ov017_02142fc4[];
extern char data_ov017_021430a0[];
extern char data_ov017_021430e8[];
extern char data_ov017_02143138[];
extern char data_ov017_02143164[];
extern char data_ov017_02143168[];
extern char data_ov017_0214316c[];
extern char data_ov017_02143170[];
extern char data_ov017_02143174[];
extern char data_ov018_020ff42c[];
extern char data_ov018_020ff4b0[];
extern char data_ov019_020fa7a0[];
extern char data_ov020_020f70a0[];
extern char data_ov020_020f70a4[];
extern char data_ov021_020f7940[];
extern char data_ov021_020f7944[];
extern char data_ov021_020f79a0[];
extern char data_ov021_020f7a24[];
extern char data_ov022_020f5a60[];
extern char data_ov024_020f6ba0[];
extern char data_ov024_020f6bd4[];

void** CombatEnemyGroup_SpriteDescs[61] = {
    (void**)data_ov009_02124af4, (void**)data_ov009_02124c44, (void**)data_ov009_02124cc4, (void**)data_ov009_02125564,
    (void**)data_ov009_02124d58, (void**)data_ov010_02129320, (void**)data_ov011_0212c460, (void**)data_ov009_02124dc0,
    (void**)data_ov009_02124ff8, (void**)data_ov009_02124904, (void**)data_ov012_02127980, (void**)data_ov013_02127540,
    (void**)data_ov009_02125240, (void**)data_ov009_02124840, (void**)data_ov009_02124e84, (void**)data_ov014_02134e0c,
    (void**)data_ov009_02124a34, (void**)data_ov015_021283f8, (void**)data_ov015_021283f8, (void**)data_ov015_021283f8,
    (void**)data_ov015_021283f8, (void**)data_ov015_021283f8, (void**)data_ov015_021283f8, (void**)data_ov015_021283f8,
    (void**)data_ov015_021283f8, (void**)data_ov015_021283f8, (void**)data_ov015_021283f8, (void**)data_ov015_021283f8,
    (void**)data_ov015_021283f8, (void**)data_ov015_021283f8, (void**)data_ov009_02125328, (void**)data_ov009_021253ec,
    (void**)data_ov018_020ff42c, (void**)data_ov018_020ff4b0, (void**)data_ov010_02129160, (void**)data_ov017_02142d9c,
    (void**)data_ov017_02142f00, (void**)data_ov017_02142f6c, (void**)data_ov017_02142fc4, (void**)data_ov017_021430a0,
    (void**)data_ov017_02143138, (void**)data_ov017_021430e8, (void**)data_ov017_021430e8, (void**)data_ov017_02143170,
    (void**)data_ov017_02143174, (void**)data_ov017_02143168, (void**)data_ov017_0214316c, (void**)data_ov017_02143164,
    (void**)data_ov019_020fa7a0, (void**)data_ov020_020f70a4, (void**)data_ov020_020f70a0, (void**)data_ov021_020f7944,
    (void**)data_ov021_020f7940, (void**)data_ov022_020f5a60, (void**)data_ov021_020f79a0, (void**)data_ov024_020f6ba0,
    (void**)data_ov024_020f6ba0, (void**)data_ov024_020f6bd4, (void**)data_ov024_020f6bd4, (void**)data_ov021_020f7a24,
    (void**)data_ov003_020e6a30,
};
