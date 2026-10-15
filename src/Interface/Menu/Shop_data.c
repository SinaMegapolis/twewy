#include "ShopPart.h"

// Data owned by the ov043 Shop dispatcher unit (`src/Interface/Menu/Shop.c`).
//
// These were previously `extern` in ShopPart.h/Shop.h and resolved against the
// delinked object; they are defined here so the unit owns its own .rodata/.data.
// Layout and addresses are recorded in docs/decomp/ov043-shop-boundaries.md.

// ---------------------------------------------------------------------------
// .data — bin paths and bin names (referenced by the BinIdentifier table and by
// Mem_SetSequence).
// ---------------------------------------------------------------------------
char data_ov043_020cc5c0[12] = "Seq_Shop()";
char data_ov043_020cc5cc[20] = "Data/BadgeData.bin";
char data_ov043_020cc5e0[24] = "Apl_Tak/FoodData.bin";
char data_ov043_020cc5f8[24] = "Apl_Tak/ShopData.bin";
char data_ov043_020cc610[24] = "Apl_Tak/Grp_Item.bin";
char data_ov043_020cc628[24] = "Apl_Tak/ItemData.bin";
char data_ov043_020cc640[24] = "Apl_Tak/ShopItem.bin";
char data_ov043_020cc658[24] = "Apl_Tak/Grp_Badge.bin";
char data_ov043_020cc670[24] = "Apl_Tak/ShopQuest.bin";
char data_ov043_020cc688[24] = "Apl_Kit/Grp_FldBG.bin";
char data_ov043_020cc6a0[28] = "Apl_Tak/TreasureData.bin";
char data_ov043_020cc6bc[28] = "Apl_Tak/ShopCastData.bin";
char data_ov043_020cc6d8[28] = "Apl_Tak/Grp_Menu_BGD.bin";
char data_ov043_020cc6f4[28] = "Apl_Tak/BeBadge_Parm.bin";
char data_ov043_020cc710[28] = "Apl_Kit/Grp_FldEveNpc.bin";
char data_ov043_020cc72c[28] = "Apl_Tak/Grp_DummyItem.bin";
char data_ov043_020cc748[28] = "Apl_Tak/Grp_BrandLogo.bin";
char data_ov043_020cc764[28] = "Apl_Tak/Grp_Shop_BGD00.bin";
char data_ov043_020cc780[28] = "Apl_Tak/Grp_Shop_OBD00.bin";
char data_ov043_020cc79c[28] = "Apl_Tak/Grp_Shop_OBD01.bin";
char data_ov043_020cc7b8[28] = "Apl_Tak/Grp_Shop_OBD02.bin";
char data_ov043_020cc7d4[28] = "Apl_Tak/Grp_Shop_OBU00.bin";
char data_ov043_020cc7f0[28] = "Apl_Fuk/Grp_FldMessWin.bin";
char data_ov043_020cc80c[32] = "Apl_Tak/Grp_Menu_fontSCR.bin";


char data_ov043_020cc82c[12] = "badge_data";
char data_ov043_020cc838[12] = "item_data";
char data_ov043_020cc844[12] = "food_data";
char data_ov043_020cc850[16] = "treasure_data";
char data_ov043_020cc860[16] = "shopitem_data";
char data_ov043_020cc870[16] = "shopQuest_data";
char data_ov043_020cc880[16] = "beBadge_data";

// The Shop menu root state, allocated on the debug heap at menu init.
ShopState* data_ov043_020cd288;

// The Shop task pool sequence name.
s8* data_ov043_020cc5bc = (s8*)data_ov043_020cc5c0;

// ---------------------------------------------------------------------------
// .rodata — overlay reload callbacks, indexed by overlay-display id.
// ---------------------------------------------------------------------------
const OverlayCB data_ov043_020ca18c[3] = {
    (OverlayCB)func_ov043_020aeb14,
    (OverlayCB)func_ov043_020aec98,
    (OverlayCB)func_ov043_020aee2c,
};

// ---------------------------------------------------------------------------
// .rodata — shop bin identifiers (id 0x2b = shop bin), used with
// DatMgr_LoadRawData / DatMgr_LoadPackEntry.  The address labels are the
// delinker's splits of this one table.
// ---------------------------------------------------------------------------
const BinIdentifier data_ov043_020ca198[3] = {
    { 0x2b, data_ov043_020cc80c },
    { 0x2b, data_ov043_020cc764 },
    { 0x2b, data_ov043_020cc6d8 },
};
const u32 data_ov043_020ca1b0 = 0x2B;
char* const data_ov043_020ca1b4 = data_ov043_020cc780;
const BinIdentifier data_ov043_020ca1b8 = { 0x2b, data_ov043_020cc79c };
const BinIdentifier data_ov043_020ca1c0[3] = {
    { 0x2b, data_ov043_020cc7b8 },
    { 0x2b, data_ov043_020cc80c },
    { 0x2b, data_ov043_020cc688 },
};
const BinIdentifier data_ov043_020ca1d8 = { 0x2b, data_ov043_020cc7d4 };
const BinIdentifier data_ov043_020ca1e0 = { 0x2b, data_ov043_020cc7f0 };
const BinIdentifier data_ov043_020ca1e8 = { 0x2b, data_ov043_020cc710 };
const BinIdentifier data_ov043_020ca1f0 = { 0x2b, data_ov043_020cc658 };
const BinIdentifier data_ov043_020ca1f8 = { 0x2b, data_ov043_020cc610 };
const BinIdentifier data_ov043_020ca200 = { 0x2b, data_ov043_020cc72c };
const BinIdentifier data_ov043_020ca208 = { 0x2b, data_ov043_020cc748 };
const BinIdentifier data_ov043_020ca210 = { 0x2b, data_ov043_020cc5cc };
const BinIdentifier data_ov043_020ca218 = { 0x2b, data_ov043_020cc628 };
const BinIdentifier data_ov043_020ca220 = { 0x2b, data_ov043_020cc5e0 };
const BinIdentifier data_ov043_020ca228 = { 0x2b, data_ov043_020cc6a0 };
const BinIdentifier data_ov043_020ca230 = { 0x2b, data_ov043_020cc5f8 };
const BinIdentifier data_ov043_020ca238 = { 0x2b, data_ov043_020cc6bc };
const BinIdentifier data_ov043_020ca240 = { 0x2b, data_ov043_020cc640 };
const BinIdentifier data_ov043_020ca248 = { 0x2b, data_ov043_020cc670 };
const BinIdentifier data_ov043_020ca250 = { 0x2b, data_ov043_020cc6f4 };
const BinIdentifier data_ov043_020ca258 = { 0x2b, data_ov043_020cc72c };

// ---------------------------------------------------------------------------
// .rodata — shop menu layout table (paired x/y rects read by the row hit-tests)
// and the two sound-effect tables.
// ---------------------------------------------------------------------------
const u16 data_ov043_020ca260[36] = {
    0x000F, 0x0015, 0x0030, 0x0010, 0x00CF, 0x0001, 0x0011, 0x0009, 0x0030, 0x0010,
    0x003F, 0x0013, 0x000F, 0x003D, 0x00CB, 0x0022, 0x0020, 0x0028, 0x0020, 0x0024,
    0x00DC, 0x0014, 0x00C9, 0x0015, 0x00C9, 0x0062, 0x002C, 0x006C, 0x0095, 0x006C,
    0x0007, 0x0038, 0x0069, 0x009A, 0x00CB, 0x0000,
};

const s32 data_ov043_020ca2a8[6] = { 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B };

const s32 data_ov043_020ca2c0[40] = {
    0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B, 0x42C, 0x42D, 0x42E, 0x42F,
    0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x43C, 0x43D, 0x43E, 0x43F,
    0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449,
    0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F, 0x450, 0x451, 0x452, 0x453,
};
