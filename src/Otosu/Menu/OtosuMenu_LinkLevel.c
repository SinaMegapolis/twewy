#include "OtosuMenuShared.h"

static PrcStepResult OtosuMenu_LinkLevel_Step_UpdateIcon(PrcCtx* ctx, void* object);
static void          OtosuMenu_LinkLevel_Load(PrcCtx* ctx, OtosuMenuObj* menuObj, s32 arg2);
static void          OtosuMenu_LinkLevel_Destroy(PrcCtx* ctx, OtosuMenuObj* menuObj);
static void          OtosuMenu_LinkLevel_Update(PrcCtx* ctx, OtosuMenuObj* menuObj);
static void          OtosuMenu_LinkLevel_Render(PrcCtx* ctx, OtosuMenuObj* menuObj);

const Ov002_U16_4 OtosuMenu_BroadcastBssid = {0xFFFF, 0xFFFF, 0xFFFF, 0x0000};

static PrcStepFn OtosuMenu_LinkLevel_StepTable[1] = {OtosuMenu_LinkLevel_Step_UpdateIcon};

PrcFrameDesc OtosuMenu_LinkLevel_FrameDesc = {
    .enter     = OtosuMenu_LinkLevel_Load,
    .stepTable = OtosuMenu_LinkLevel_StepTable,
    .update    = OtosuMenu_LinkLevel_Update,
    .render    = OtosuMenu_LinkLevel_Render,
    .exit      = OtosuMenu_LinkLevel_Destroy,
};

static SpriteFrameInfo* OtosuMenu_LinkLevel_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation OtosuMenu_LinkLevel_Anim = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = 0x0000,
    .posX              = -13,
    .posY              = 0x000C,
    .frameInfoCallback = OtosuMenu_LinkLevel_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = (BinIdentifier*)&data_ov002_02091acc,
    .unk_18            = 0x0000,
    .packIndex         = 0x0000,
    .unk_1C            = 0x0015,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0018,
    .unk_22            = 0x0002,
    .unk_24            = 0x0000,
    .unk_26            = 0x0016,
    .unk_28            = 0x0017,
    .animIndex         = 0x0001,
};

void OtosuMenu_LinkLevel_Load(PrcCtx* ctx, OtosuMenuObj* menuObj, s32 arg2) {
    SpriteAnimation anim = OtosuMenu_LinkLevel_Anim;

    anim.dataType  = menuObj->unk_1158C;
    anim.bits_0_1  = 1;
    anim.animIndex = 1;
    anim.posX      = 0x80;
    anim.posY      = 0x60;
    anim.unk_1C    = 0x11;
    anim.unk_20    = 0x14;
    anim.unk_22    = 2;
    anim.unk_26    = 0x12;
    anim.unk_28    = 0x13;
    if (_Sprite_Load(&menuObj->linkLevelIcon, &anim) == 0) {
        OS_WaitForever();
    }
}

void OtosuMenu_LinkLevel_Destroy(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    Sprite_Destroy(&menuObj->linkLevelIcon);
}

void OtosuMenu_LinkLevel_Update(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    Sprite_UpdateAndCheck(&menuObj->linkLevelIcon);
}

void OtosuMenu_LinkLevel_Render(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    if (menuObj->unk_462E8 != 0) {
        Sprite_Render(&menuObj->linkLevelIcon);
    }
}

static const u16 OtosuMenu_LinkLevel_AnimByLevel[4] = {5, 4, 3, 2};

PrcStepResult OtosuMenu_LinkLevel_Step_UpdateIcon(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    u16 options[4];
    options = OtosuMenu_LinkLevel_AnimByLevel;

    s32 index = func_ov040_0209cb5c();

    if ((index < 0) || (index > 3)) {
        OS_WaitForever();
    }
    Sprite_ChangeAnimation(&menuObj->linkLevelIcon, menuObj->linkLevelIcon.animData, options[index],
                           menuObj->linkLevelIcon.cellTable);
    return PRC_STEP_CONTINUE;
}
