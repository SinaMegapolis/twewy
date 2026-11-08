#include "Combat/Core/CombatPsyAnim.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"

/// One record of the psych animation table at `CombatPsyAnim_Table`, indexed by the pack id that
/// `CombatPsyAnim_Load` receives. Fields 0x04..0x16 feed a `SpriteAnimationEx`.
typedef struct Ov003TouchIconParams {
    /* 0x00 */ const BinIdentifier* binIden;
    /* 0x04 */ u16                  unk_04;
    /* 0x06 */ s16                  unk_06;
    /* 0x08 */ SpriteAnimEntry      entry;
    /* 0x10 */ u16                  unk_10;
    /* 0x12 */ u16                  unk_12;
    /* 0x14 */ u16                  unk_14;
    /* 0x16 */ u16                  unk_16;
} Ov003TouchIconParams; // Size: 0x18

static char data_ov003_020e4528[0x1C] = "Apl_Hir/Grp_BtlCircle.bin";

static char data_ov003_020e4544[0x1C] = "Apl_Hir/Grp_BtlPsy_Hold.bin";

static char data_ov003_020e4560[0x1C] = "Apl_Hir/Grp_BtlPsy_Lime.bin";

static char data_ov003_020e457c[0x1C] = "Apl_Hir/Grp_BtlPsy_Bomb.bin";

static char data_ov003_020e4598[0x20] = "Apl_Hir/Grp_BtlPsy_Element.bin";

static char data_ov003_020e45b8[0x20] = "Apl_Hir/Grp_BtlPsy_Recover.bin";

static char data_ov003_020e45d8[0x20] = "Apl_Hir/Grp_BtlPsy_Barrier.bin";

static char data_ov003_020e45f8[0x24] = "Apl_Hir/Grp_BtlPsy_BlackHoll.bin";

static char data_ov003_020e461c[0x24] = "Apl_Hir/Grp_BtlPsy_ShockWave.bin";

static char data_ov003_020e4640[0x24] = "Apl_Hir/Grp_BtlPsy_Territory.bin";

static char data_ov003_020e4664[0x24] = "Apl_Hir/Grp_BtlPsy_IcePillar.bin";

static char data_ov003_020e4688[0x24] = "Apl_Hir/Grp_BtlPsy_FieldShake.bin";

static char data_ov003_020e46ac[0x24] = "Apl_Hir/Grp_BtlPsy_DragAttack.bin";

static char data_ov003_020e46d0[0x24] = "Apl_Hir/Grp_BtlPsy_StreetLive.bin";

static char data_ov003_020e46f4[0x24] = "Apl_Hir/Grp_BtlPsy_MeteorFall.bin";

static char data_ov003_020e4718[0x24] = "Apl_Hir/Grp_BtlPsy_ChargeLaser.bin";

static char data_ov003_020e473c[0x24] = "Apl_Hir/Grp_BtlPsy_AttackPaint.bin";

static char data_ov003_020e4760[0x24] = "Apl_Hir/Grp_BtlPsy_ElectricGen.bin";

static char data_ov003_020e4784[0x24] = "Apl_Hir/Grp_BtlPsy_ElementBind.bin";

static char data_ov003_020e47a8[0x24] = "Apl_Hir/Grp_BtlPsy_SlashThunder.bin";

static char data_ov003_020e47cc[0x24] = "Apl_Hir/Grp_BtlPsy_AbsorbEffect.bin";

static char data_ov003_020e47f0[0x28] = "Apl_Hir/Grp_BtlPsy_Psychokinesis.bin";

static char data_ov003_020e4818[0x28] = "Apl_Hir/Grp_BtlPsy_LockOnThunder.bin";

static char data_ov003_020e4840[0x28] = "Apl_Hir/Grp_BtlPsy_BadgeFormation.bin";

static const BinIdentifier data_ov003_020da74c = {3, (char*)&data_ov003_020e46ac};

static const BinIdentifier data_ov003_020da754 = {3, (char*)&data_ov003_020e4818};

static const BinIdentifier data_ov003_020da75c = {3, (char*)&data_ov003_020e45f8};

static const BinIdentifier data_ov003_020da764 = {3, (char*)&data_ov003_020e4688};

static const BinIdentifier data_ov003_020da76c = {3, (char*)&data_ov003_020e4598};

static const BinIdentifier data_ov003_020da774 = {3, (char*)&data_ov003_020e4784};

static const BinIdentifier data_ov003_020da77c = {3, (char*)&data_ov003_020e4664};

static const BinIdentifier data_ov003_020da784 = {3, (char*)&data_ov003_020e4840};

static const BinIdentifier data_ov003_020da78c = {3, (char*)&data_ov003_020e4640};

static const BinIdentifier data_ov003_020da794 = {3, (char*)&data_ov003_020e457c};

static const BinIdentifier data_ov003_020da79c = {3, (char*)&data_ov003_020e47f0};

static const BinIdentifier data_ov003_020da7a4 = {3, (char*)&data_ov003_020e47cc};

static const BinIdentifier data_ov003_020da7ac = {3, (char*)&data_ov003_020e4760};

static const BinIdentifier data_ov003_020da7b4 = {3, (char*)&data_ov003_020e4528};

static const BinIdentifier data_ov003_020da7bc = {3, (char*)&data_ov003_020e461c};

static const BinIdentifier data_ov003_020da7c4 = {3, (char*)&data_ov003_020e45b8};

static const BinIdentifier data_ov003_020da7cc = {3, (char*)&data_ov003_020e4560};

static const BinIdentifier data_ov003_020da7d4 = {3, (char*)&data_ov003_020e45d8};

static const BinIdentifier data_ov003_020da7dc = {3, (char*)&data_ov003_020e46d0};

static const BinIdentifier data_ov003_020da7e4 = {3, (char*)&data_ov003_020e47a8};

static const BinIdentifier data_ov003_020da7ec = {3, (char*)&data_ov003_020e46f4};

static const BinIdentifier data_ov003_020da7f4 = {3, (char*)&data_ov003_020e473c};

static const BinIdentifier data_ov003_020da7fc = {3, (char*)&data_ov003_020e4544};

static const BinIdentifier data_ov003_020da804 = {3, (char*)&data_ov003_020e4718};

static const Ov003TouchIconParams CombatPsyAnim_Table[258] = {
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  5,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  5,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da754, 3,  6,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da754, 3,  6,    {1, 3, 2, 0}, 1,  5, 128, 0},
    {&data_ov003_020da754, 3,  6,    {1, 3, 2, 0}, 1,  6, 128, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 0,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 1,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 2,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  2,    {1, 3, 2, 0}, 0,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  2,    {1, 3, 2, 0}, 1,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  2,    {1, 3, 2, 0}, 2,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 0,  5, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 1,  5, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 2,  5, 256, 0},
    {&data_ov003_020da7e4, 3,  3,    {1, 3, 2, 0}, 0,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  3,    {1, 3, 2, 0}, 1,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  3,    {1, 3, 2, 0}, 2,  4, 256, 0},
    {&data_ov003_020da76c, 3,  3,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  1,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  2,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  5,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  6,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  7,  16, 0},
    {&data_ov003_020da7bc, 3,  1,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  1,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  1,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  2,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  2,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  2,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  3,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  3,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  3,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  4,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  4,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  4,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  6,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  6,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  6,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  5,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  5,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  5,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  7,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  7,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  8,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  8,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  9,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  9,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 10,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 10,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 2,  5,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 3,  5,  32, 0},
    {&data_ov003_020da7bc, 3, 12,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 12,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 13,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 13,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 14,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  9,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 17,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da7bc, 3, 17,    {1, 3, 2, 0}, 0,  5,  64, 0},
    {&data_ov003_020da7bc, 3, 18,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da7bc, 3, 18,    {1, 3, 2, 0}, 0,  5,  64, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 3,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 2,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 3,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 1,  5,  32, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 4,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 2,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 15,    {1, 3, 2, 0}, 1,  4,  64, 0},
    {&data_ov003_020da7bc, 3, 15,    {1, 3, 2, 0}, 1,  5,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {16, 18, 17, 0}, 0, 19,  72, 0},
    {&data_ov003_020da7ec, 3,  1, {16, 18, 17, 0}, 1, 19,  72, 0},
    {&data_ov003_020da7ec, 3,  1,    {1, 3, 2, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {1, 3, 2, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {4, 6, 5, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {4, 6, 5, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {7, 9, 8, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {7, 9, 8, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {10, 12, 11, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {10, 12, 11, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {13, 15, 14, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {13, 15, 14, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7f4, 3,  1,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da7f4, 3,  2,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da7f4, 3,  1,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da7f4, 3,  1,    {1, 3, 2, 0}, 1,  5, 128, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 1,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 2,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 3,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 4,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 0,  4,   4, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 1,  4,   8, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 2,  4,  16, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 3,  4,  16, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 3,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 2,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 5,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 6,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 7,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 5,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 8,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 9,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  3,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da7d4, 3,  1,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  1,    {4, 6, 5, 0}, 0,  7,  96, 0},
    {&data_ov003_020da7d4, 3,  2,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  2,    {4, 6, 5, 0}, 0,  7,  48, 0},
    {&data_ov003_020da7d4, 3,  3,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  3,    {4, 6, 5, 0}, 0,  7,  48, 0},
    {&data_ov003_020da7d4, 3,  4,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  4,    {4, 6, 5, 0}, 0,  7,  48, 0},
    {&data_ov003_020da77c, 3,  1,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da77c, 3,  2,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da77c, 3,  3,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da77c, 3,  3,    {1, 3, 2, 0}, 0,  5,  64, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 1,  5, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 1,  6, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 2,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 3,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 3,  5, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 3,  6, 128, 0},
    {&data_ov003_020da794, 3,  6,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da794, 3,  4,    {1, 3, 2, 0}, 1,  4, 208, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 4,  4,  40, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 5,  4,  40, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 6,  4,  40, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 3,  4,  40, 0},
    {&data_ov003_020da794, 3,  6,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da794, 3,  6,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da794, 3,  3,    {1, 3, 2, 0}, 0,  4,  40, 0},
    {&data_ov003_020da794, 3,  3,    {1, 3, 2, 0}, 1,  4,  40, 0},
    {&data_ov003_020da794, 3,  4,    {1, 3, 2, 0}, 0,  4, 208, 0},
    {&data_ov003_020da794, 3,  5,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da75c, 3,  1,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da75c, 3,  1,    {1, 3, 2, 0}, 1,  4,  64, 0},
    {&data_ov003_020da75c, 3,  1,    {1, 3, 2, 0}, 2,  4,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0,  8,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0,  9,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0, 10,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0, 11,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {1, 3, 2, 0}, 0,  7,  64, 0},
    {&data_ov003_020da79c, 3,  2,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da79c, 3,  3,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da79c, 3,  3,    {1, 3, 2, 0}, 0,  4,  32, 0},
    {&data_ov003_020da79c, 3,  1,    {1, 3, 2, 0}, 0,  4,  34, 0},
    {&data_ov003_020da7cc, 3,  1,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 2,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 3,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 4,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 5,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 6,  4,   1, 0},
    {&data_ov003_020da7b4, 3,  1,    {1, 3, 2, 0}, 2,  4,   4, 0},
    {&data_ov003_020da7b4, 3,  1,    {1, 3, 2, 0}, 3,  4,   6, 0},
    {&data_ov003_020da774, 3,  1,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da774, 3,  1,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da774, 3,  1,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da774, 3,  2,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da774, 3,  2,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da774, 3,  2,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da764, 3,  1,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da764, 3,  1,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da764, 3,  2,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da764, 3,  2,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da764, 3,  2,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {5, 7, 6, 0}, 0,  8,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 8,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 5,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 1,  4,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 2,  4,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 3,  4,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 4,  4,  16, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 5,  4,   4, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 6,  4,  20, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 7,  4,  20, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 8,  4,  20, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  9,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 2,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 3,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 5,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 2,  9,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 4,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 6,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 7,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 8,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 6,  9,  80, 0},
    {&data_ov003_020da78c, 3,  1,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da78c, 3,  1,    {1, 3, 2, 0}, 1,  4,  16, 0},
    {&data_ov003_020da78c, 3,  1,    {1, 3, 2, 0}, 5,  4,  16, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 0,  4, 220, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 0,  5, 220, 0},
    {&data_ov003_020da804, 3,  1,    {6, 8, 7, 0}, 0,  9,  64, 0},
    {&data_ov003_020da804, 3,  1,    {6, 8, 7, 0}, 0, 10,  64, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 2,  4, 220, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 2,  5, 220, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 0,  4,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 0,  5,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 1,  5,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 2,  5,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 0,  6,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 1,  6,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 2,  6,  32, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 5,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 6,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 7,  4,  96, 0},
    {&data_ov003_020da784, 3,  1,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da784, 3,  1,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da784, 3,  1,    {1, 3, 2, 0}, 2,  4,  48, 0},
};

/// Loads the icon of touch entry `index` into `cSprite`; `arg2` selects the looped/one-shot form
/// and `arg4` is passed through to the animation setup.
// Nonmatching: the SpriteAnimationEx field stores are scheduled differently.
s32 CombatPsyAnim_Load(CombatSprite* cSprite, s32 arg1, s32 arg2, s32 index, u16 arg4) {
    const Ov003TouchIconParams* params = &CombatPsyAnim_Table[index];
    SpriteAnimationEx           anim;

    CombatSprite_InitAnim(&anim.anim, arg1, params->binIden);
    anim.anim.bits_7_9  = 6;
    anim.anim.unk_18    = params->unk_04;
    anim.anim.packIndex = params->unk_06;
    if (arg2 == 0) {
        anim.anim.bits_10_11 = 1;
        anim.anim.unk_1C     = params->entry.charDataIndex;
        anim.anim.unk_1E     = params->unk_14 << 5;
        anim.anim.unk_26     = params->entry.frameDataIndex;
        anim.anim.unk_28     = params->entry.paletteDataIndex;
        anim.anim.unk_20     = params->unk_12;
        anim.anim.animIndex  = params->entry.animDataIndex + 1;
        anim.animTable       = &params->entry;
    } else {
        anim.anim.bits_10_11 = 0;
        anim.anim.unk_1C     = params->entry.charDataIndex;
        anim.anim.unk_26     = params->entry.frameDataIndex;
        anim.anim.unk_28     = params->entry.paletteDataIndex;
        anim.anim.unk_20     = params->unk_12;
        anim.anim.animIndex  = 1;
        anim.animTable       = NULL;
    }
    if (CombatSprite_Load(cSprite, &anim) == 0) {
        return 0;
    }
    CombatSprite_SetAnim(cSprite, params->unk_10, arg4);
    return 1;
}

void CombatPsyAnim_SetAnim(CombatSprite* cSprite, s32 index, s32 arg2) {
    const Ov003TouchIconParams* params = &CombatPsyAnim_Table[index];

    CombatSprite_SetAnim(cSprite, params->unk_10, arg2);
}
