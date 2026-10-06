#ifndef OTOSUMENU_SHARED_H
#define OTOSUMENU_SHARED_H

#include "CriSndMgr.h"
#include "Display.h"
#include "EasyFade.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "OtosuMenu.h"
#include "Save.h"
#include "SndMgr.h"
#include "SpriteMgr.h"
#include "Util/SysFont.h"

#include <nitro/mi/cpumem.h>

extern void func_0200d8f0(void);
extern void func_ov030_020ae92c(void);

#define OVMGR_U8(base, off)  (*(u8*)((u8*)(base) + (off)))
#define OVMGR_U16(base, off) (*(u16*)((u8*)(base) + (off)))
#define OVMGR_U32(base, off) (*(u32*)((u8*)(base) + (off)))
#define OVMGR_S32(base, off) (*(s32*)((u8*)(base) + (off)))

typedef struct {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
} Ov002_U16_5;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
    };
    u16 data[3];
} Ov002_U16_3;

typedef struct {
    u16 unk0;
    u16 unk2;
} Ov002_U16_2;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
    };
    u16 data[4];
} Ov002_U16_4;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
        u16 unk8;
        u16 unkA;
    };
    u16 data[6];
} Ov002_U16_6;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
        u16 unk8;
        u16 unkA;
        u16 unkC;
        u16 unkE;
    };
    u16 data[8];
} Ov002_U16_8x;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
        u16 unk8;
        u16 unkA;
        u16 unkC;
        u16 unkE;
        u16 unk10;
    };
    u16 data[9];
} Ov002_U16_9;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
        u16 unk8;
        u16 unkA;
        u16 unkC;
        u16 unkE;
        u16 unk10;
        u16 unk12;
    };
    u16 data[10];
} Ov002_U16_10;

typedef struct {
    u16 data[15];
} Ov002_U16_15;

typedef struct {
    u16 data[30];
} Ov002_U16_30;

typedef struct {
    u16 data[16];
} Ov002_U16_16;

typedef struct {
    u16 data[4][4];
} Ov002_U16_4x4;

typedef struct {
    u16 data[50];
} Ov002_U16_50;

typedef struct {
    s16 data[4][8];
} Ov002_S16_4x8;

typedef struct {
    u16 data[18][2];
} Ov002_U16_18x2;

/** @brief A BG layer reference held by the H2 scroll task: which engine/layer, plus flag bits. */
typedef struct {
    /* 0x0 */ DisplayEngine  engine;
    /* 0x4 */ DisplayBGLayer layer;
    /* 0x8 */ s32            flags;
} Ov002_BgRef;

/** @brief Enter argument of a title-screen enemy object (H1). */
typedef struct {
    /* 0x0 */ u16 index;
    /* 0x2 */ u16 posX;
} Ov002_TitleEnmArg;

/** @brief Enter argument of the title-screen boss object (H2): start X and the two BG layers it scrolls. */
typedef struct {
    /* 0x0 */ u16          posX;
    /* 0x4 */ Ov002_BgRef* upper;
    /* 0x8 */ Ov002_BgRef* lower;
} Ov002_TitleBossArg;

typedef struct {
    u16 data[25];
} Ov002_U16_25;

typedef struct {
    u16 data[27];
} Ov002_U16_27;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
        u16 unk8;
        u16 unkA;
        u16 unkC;
        u16 unkE;
        u16 unk10;
        u16 unk12;
        u16 unk14;
        u16 unk16;
    };
    u16 data[12];
} Ov002_U16_12;

typedef union {
    struct {
        u16 unk0;
        u16 unk2;
        u16 unk4;
        u16 unk6;
        u16 unk8;
        u16 unkA;
        u16 unkC;
        u16 unkE;
        u16 unk10;
        u16 unk12;
        u16 unk14;
        u16 unk16;
        u16 unk18;
        u16 unk1A;
        u16 unk1C;
        u16 unk1E;
        u16 unk20;
        u16 unk22;
        u16 unk24;
        u16 unk26;
        u16 unk28;
        u16 unk2A;
        u16 unk2C;
        u16 unk2E;
        u16 unk30;
        u16 unk32;
        u16 unk34;
        u16 unk36;
        u16 unk38;
        u16 unk3A;
        u16 unk3C;
        u16 unk3E;
        u16 unk40;
        u16 unk42;
        u16 unk44;
        u16 unk46;
        u16 unk48;
        u16 unk4A;
        u16 unk4C;
        u16 unk4E;
        u16 unk50;
        u16 unk52;
        u16 unk54;
        u16 unk56;
        u16 unk58;
        u16 unk5A;
    };
    u16 data[46];
} Ov002_U16_46;

typedef struct {
    u16 data[20];
} Ov002_U16_20;

typedef struct {
    BinIdentifier binIden;
    u16           unk8[4];
    u16           unk10[8];
} Ov002_BinPack;

typedef struct {
    u16   unk0;
    u16   unk2;
    u16   unk4;
    u16   unk6;
    u16   unk8;
    u16   unkA;
    void* unkC;
} Ov002_Config91cc4;

typedef struct {
    u8  filler14[0x14];
    u16 unk14;
} Data_02075110;

typedef struct {
    u8  fillerD84[0xD84];
    u8  unkD84;
    u8  fillerD85[0xD88 - 0xD85];
    u32 unkD88;
} Data_02072d10;

typedef struct {
    u8  filler3414[0x3414];
    u16 unk3414[4];
} Data_02071d10;

extern u8            data_020750fc[];
extern u8            data_02075102[];
extern Data_02075110 data_02075110;
extern u16           data_02075124[];
extern Data_02072d10 data_02072d10;
extern Data_02071d10 data_02071d10;

/// MARK: Data shared between the Otosu menu TUs

extern const BinIdentifier data_ov002_02091aac;
extern const BinIdentifier data_ov002_02091acc;
extern const BinIdentifier data_ov002_02091c1c;
extern const BinIdentifier data_ov002_02091c24;
extern const BinIdentifier data_ov002_02091c2c;
extern const BinIdentifier data_ov002_02091c34;
extern const Ov002_U16_4   data_ov002_02092160;
extern PrcStepFn           PrcSteps_FadeBrightImmediate[];
extern PrcStepFn           PrcSteps_FadeBright[];
extern PrcStepFn           PrcSteps_FadeDark[];
extern char                data_ov002_02092be4[];
extern PrcFrameDesc        data_ov002_02092c58;
extern PrcFrameDesc        data_ov002_02092c9c;
extern char                data_ov002_02092cd0[];
extern PrcFrameDesc        data_ov002_02092ce0;
extern PrcFrameDesc        data_ov002_02092cf4;
extern char                data_ov002_02092df4[];
extern char                data_ov002_02092e04[];
extern PrcFrameDesc        data_ov002_02092e18;
extern PrcFrameDesc        data_ov002_02092e2c;
extern PrcFrameDesc        data_ov002_02092e40;
extern PrcFrameDesc        data_ov002_02092e54;
extern PrcStepFn           data_ov002_02092e68[];
extern PrcStepFn           data_ov002_02092e98[];
extern PrcStepFn           data_ov002_02092ec8[];
extern PrcStepFn           data_ov002_02092efc[];
extern PrcFrameDesc        data_ov002_02092f44;
extern PrcFrameDesc        data_ov002_02092ff0;
extern PrcFrameDesc        data_ov002_02093008;
extern PrcFrameDesc        data_ov002_02093020;
extern PrcFrameDesc        data_ov002_02093034;
extern PrcFrameDesc        data_ov002_02093048;
extern PrcFrameDesc        data_ov002_0209305c;
extern PrcFrameDesc        data_ov002_020931e8;
extern PrcFrameDesc        data_ov002_02093208;
extern PrcFrameDesc        data_ov002_02093228;
extern PrcFrameDesc        data_ov002_02093240;
extern PrcFrameDesc        data_ov002_02093254;
extern PrcFrameDesc        data_ov002_02093268;
extern PrcFrameDesc        data_ov002_020932b8;
extern PrcFrameDesc        data_ov002_02093310;
extern PrcFrameDesc        data_ov002_02093324;
extern PrcFrameDesc        data_ov002_02093438;
extern PrcFrameDesc        data_ov002_0209344c;
extern PrcFrameDesc        data_ov002_02093460;
extern PrcFrameDesc        data_ov002_02093498;
extern PrcFrameDesc        data_ov002_020934b0;
extern u16                 data_ov002_020935c0[0x10];
extern u16                 data_ov002_020935e0[0x40];
extern s32                 data_ov002_02093660;

/// MARK: Functions

u8*              func_ov002_020824a0(void);
SpriteFrameInfo* func_ov002_020824ac(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_02082548(OtosuMenuObj* menuObj);
void             func_ov002_02082610(OtosuMenuObj* menuObj);
void             func_ov002_0208264c(OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208266c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02082698(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208275c(PrcCtx* ctx, void* object);
PrcStepResult    OtosuPrcStep_FadeStart_BrightImmediate(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeStart_DarkImmediate(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeWait_Immediate(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeStart_Bright(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeStart_Dark(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeWait(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeStart_Neutral(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeWait_Neutral(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeStart_Neutral2(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeWait_Neutral2(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeStart_NeutralSlow(PrcCtx* ctx, void* unused);
PrcStepResult    OtosuPrcStep_FadeWait_NeutralSlow(PrcCtx* ctx, void* unused);
void             func_ov002_02082a44(PrcCtx*, void*);
void             func_ov002_02082ab4(OtosuMenuObj* menuObj);
u8               func_ov002_02082bec(OtosuMenuObj* menuObj);
void             func_ov002_02082d44(OtosuMenuObj* menuObj);
void             func_ov002_02082dbc(void* arg0, const Ov002_U16_5* arg1, void* arg2, void* arg3);
void             func_ov002_02082e70(void* arg0, s32* arg1, void* arg2, void* arg3);
void             func_ov002_02082f18(OtosuMenuObj* menuObj, s32 arg1, s32 arg2, void* arg3);
void             func_ov002_02083484(OtosuMenuObj* menuObj, u16* arg1);
void             func_ov002_02083694(OtosuMenuObj* menuObj);
void             func_ov002_02083a74(OtosuMenuObj* menuObj);
void             func_ov002_02084494(OtosuMenuObj* menuObj, u8 arg1, u16* arg2);
void             func_ov002_02084c84(OtosuMenuObj* menuObj, u16* arg1);
void             func_ov002_020850c0(OtosuMenuObj* menuObj, s32 arg1, s32 arg2, s32* arg3, u16* arg4);
void             func_ov002_02085710(OtosuMenuObj* menuObj);
u16              func_ov002_0208597c(u16* arg0);
void             func_ov002_02085a44(OtosuMenuObj* menuObj);
void             func_ov002_02085ac4(OtosuMenuObj* menuObj);
OtosuMenuObj*    OtosuMenu_Init(void);
OtosuMenuObj*    func_ov002_02085df8(void);
void             OtosuMenu_InitForSinglePlayerEnter(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForMultiplayerEnter(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForMultiplayerRankings(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForSinglePlayerRankings(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForConnectionError(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForRoleSelection(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForFontList(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForDataDeletion(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForDataCorrupted(OtosuMenuObj* menuObj);
void             func_ov002_02086290(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForDataLoadFailure(OtosuMenuObj* menuObj);
void             OtosuMenu_InitForDataSaveFailure(OtosuMenuObj* menuObj);
void             OtosuMenu_Update(OtosuMenuObj* menuObj);
void             OtosuMenu_Destroy(OtosuMenuObj* menuObj);
void             ProcessOverlay_OtosuMenu_DataDeletion(void* menuObj);
void             ProcessOverlay_OtosuMenu_DataCorrupted(void* menuObj);
void             func_ov002_02086acc(void* menuObj);
void             ProcessOverlay_OtosuMenu_DataLoadFailure(void* menuObj);
void             ProcessOverlay_OtosuMenu_DataSaveFailure(void* menuObj);
void             func_ov002_02086b8c(s32 arg0, OtosuMenuObj* menuObj);
void             func_ov002_02086bac(void);
void             func_ov002_02086bb0(void);
void             func_ov002_02086bc4(s32 arg0, OtosuMenuObj* menuObj);
void             func_ov002_02086c5c(void);
void             func_ov002_02086c60(void);
PrcStepResult    func_ov002_02086c64(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02086c84(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02086cec(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02086e4c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02086eb4(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02086ee8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_020870ac(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_020870c8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208749c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02087508(PrcCtx* ctx, void* unused);
void             func_ov002_02087524(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_0208757c(PrcCtx* ctx, void* arg1);
void             func_ov002_020875c0(void);
void             func_ov002_020875c4(void);
PrcStepResult    func_ov002_020875c8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02087728(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_020878f8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_020879f8(PrcCtx* ctx, void* object);
void             func_ov002_02087a18(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_02087a40(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_02087aa8(OtosuMenuObj* menuObj);
void             func_ov002_02087c80(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_02087ca0(void);
void             func_ov002_02087ca4(OtosuMenuObj* menuObj, s32 arg1);
void             func_ov002_02087eb4(void* arg0, s32 arg1);
void             func_ov002_0208800c(OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_020880a0(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_02088230(PrcCtx* ctx, void* object);
void             func_ov002_0208824c(void* arg0, OtosuMenuObj* menuObj);
s32              func_ov002_0208825c(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02088310(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02088358(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02088368(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj);
void             func_ov002_020883a4(OtosuMenuObj* menuObj);
void             func_ov002_020883d8(OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208847c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02088524(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02088610(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_020886ec(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02088998(PrcCtx* ctx, void* object);
void             func_ov002_02088aa4(void* arg0, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_02088b28(PrcCtx* ctx, void* object);
void             func_ov002_02088d00(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj);
s32              func_ov002_02088dc0(void* arg0, OtosuMenuObj* menuObj);
s32              func_ov002_02088e9c(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02088f90(void* arg0, OtosuMenuObj* menuObj);
s32              func_ov002_02088fa0(PrcCtx* ctx, OtosuMenuObj* menuObj);
s32              func_ov002_02088fd8(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_020890c0(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_020890f8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208920c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089364(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_020893b8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_020893d8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089498(PrcCtx* ctx, void* object);
void             func_ov002_020894f8(void);
void             func_ov002_020894fc(PrcCtx* ctx, void* arg1);
void             func_ov002_0208950c(void);
void             func_ov002_02089510(void);
PrcStepResult    func_ov002_02089514(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208958c(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_020895c8(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_02089624(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_020896e4(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089798(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089860(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089920(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089a94(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_02089ac8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089adc(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089af0(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089b1c(PrcCtx* ctx, void* object);
void             func_ov002_02089b3c(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_02089b54(void);
void             func_ov002_02089b80(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02089ba8(void);
void             func_ov002_02089bac(void);
PrcStepResult    func_ov002_02089bb0(PrcCtx* ctx, void* unused);
void             func_ov002_02089bc0(void* arg0, void* arg1);
s32              func_ov002_02089bd0(void);
s32              func_ov002_02089bd8(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02089c88(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02089cd0(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_02089ce0(s32 arg0, void* arg1, void* arg2, s32 arg3);
PrcStepResult    func_ov002_02089d40(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_02089e3c(PrcCtx* ctx, void* object);
void             func_ov002_02089f3c(OtosuMenuObj* menuObj, s32 arg1);
PrcStepResult    func_ov002_0208a050(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208a070(PrcCtx* ctx, void* object);
void             func_ov002_0208a0f8(OtosuMenuObj* menuObj);
void             func_ov002_0208a198(OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208a250(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208a748(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208a764(PrcCtx* ctx, void* object);
void             func_ov002_0208a780(OtosuMenuObj* menuObj);
void             func_ov002_0208a8d4(void* arg0, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208aa4c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208aabc(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208ab58(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208af78(PrcCtx* ctx, void* object);
void             func_ov002_0208afc4(void* arg1, void* arg2, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208b0a4(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208b154(PrcCtx* ctx, void* object);
void             func_ov002_0208b230(void* arg0, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208b240(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208b270(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208b570(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_0208b5c4(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_0208b610(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208b670(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208b6d0(PrcCtx* ctx, void* arg0);
PrcStepResult    func_ov002_0208b734(PrcCtx* ctx, void* unused);
void             func_ov002_0208b790(void);
void             func_ov002_0208b860(void);
void             func_ov002_0208bd40(void);
void             func_ov002_0208c228(void);
SpriteFrameInfo* func_ov002_0208c6f8(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_0208c794(PrcCtx* ctx, OtosuMenuObj* menuObj, s32 arg2);
void             func_ov002_0208c864(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_0208c878(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_0208c88c(PrcCtx* ctx, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208c8b0(PrcCtx* ctx, void* object);
SpriteFrameInfo* func_ov002_0208c92c(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_0208c9c8(void* arg0, void* arg1, s16* arg2);
void             func_ov002_0208ca70(PrcCtx* ctx, void* arg1);
void             func_ov002_0208ca80(PrcCtx* ctx, void* arg1);
void             func_ov002_0208ca90(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208caa0(PrcCtx* ctx, void* unused);
void             func_ov002_0208caa8(OtosuMenuObj* menuObj);
void             func_ov002_0208cb50(OtosuMenuObj* menuObj);
void             func_ov002_0208cc5c(OtosuMenuObj* menuObj);
s32              func_ov002_0208cdb0(OtosuMenuObj* menuObj, u16 arg1);
void             func_ov002_0208ce00(OtosuMenuObj* menuObj);
void             func_ov002_0208cf84(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_0208cfe0(PrcCtx* ctx, OtosuMenuObj* menuObj);
void             func_ov002_0208d008(void);
void             func_ov002_0208d00c(void);
PrcStepResult    func_ov002_0208d010(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d09c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d144(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_0208d154(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d22c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d27c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d290(PrcCtx* ctx, void* object);
void             func_ov002_0208d2a4(void* arg0, OtosuMenuObj* menuObj);
s32              func_ov002_0208d2b4(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_0208d368(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_0208d3b0(void* arg0, OtosuMenuObj* menuObj);
void             func_ov002_0208d3c0(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj);
void             func_ov002_0208d3fc(OtosuMenuObj* menuObj);
void             func_ov002_0208d430(OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208d4cc(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d5c0(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d6bc(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208d778(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208da24(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208db58(PrcCtx* ctx, void* object);
void             func_ov002_0208dda4(void* arg1, void* arg2, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208de64(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208df6c(PrcCtx* ctx, void* object);
void             func_ov002_0208e070(void* arg0, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208e080(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e0b8(PrcCtx* ctx, void* object);
void             func_ov002_0208e194(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208e200(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e30c(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e348(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e410(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e514(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e658(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e6ac(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e6f8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e750(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e7d8(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208e838(PrcCtx* ctx, void* object);
SpriteFrameInfo* func_ov002_0208e890(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_0208e92c(PrcCtx* ctx, void* arg1, Ov002_TitleEnmArg* arg);
void             func_ov002_0208eb04(PrcCtx* ctx, s32 arg1);
void             func_ov002_0208eb3c(PrcCtx* ctx, void* arg1);
void             func_ov002_0208ebe4(PrcCtx* ctx, s32 arg1);
PrcStepResult    func_ov002_0208ec1c(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208ec34(PrcCtx* ctx, void* arg1);
void             func_ov002_0208ed78(PrcCtx* ctx, void* arg1, void* arg2);
void             func_ov002_0208edac(void);
void             func_ov002_0208edb0(PrcCtx* ctx, void* arg1);
void             func_ov002_0208ee98(void);
PrcStepResult    func_ov002_0208ee9c(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_0208eeac(PrcCtx* ctx, void* arg1);
SpriteFrameInfo* func_ov002_0208f020(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_0208f0bc(PrcCtx* ctx, void* arg1, void* arg2);
void             func_ov002_0208f5d8(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f630(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f688(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208f6e0(PrcCtx* ctx, void* unused);
PrcStepResult    func_ov002_0208f6f0(PrcCtx* ctx, void* unused);
SpriteFrameInfo* func_ov002_0208f6f8(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_0208f794(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f828(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f838(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f848(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208f858(PrcCtx* ctx, void* unused);
void             func_ov002_0208f860(void);
void             func_ov002_0208f864(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f874(void);
void             func_ov002_0208f878(void);
PrcStepResult    func_ov002_0208f87c(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_0208f89c(PrcCtx* ctx, void* arg0);
PrcStepResult    func_ov002_0208f918(PrcCtx* ctx, void* object);
PrcStepResult    func_ov002_0208f92c(PrcCtx* ctx, void* object);
void             func_ov002_0208f9d0(void);
void             func_ov002_0208f9d4(PrcCtx* ctx, void* arg1);
void             func_ov002_0208f9e4(void);
void             func_ov002_0208f9e8(void);
void             func_ov002_0208f9ec(OtosuMenuObj* menuObj, void* arg1, void* arg2);
void             func_ov002_0208fa6c(OtosuMenuObj* menuObj, void* arg1, void* arg2);
void             func_ov002_0208fbf4(OtosuMenuObj* menuObj);
void             func_ov002_0208ff0c(void* arg0);
void             func_ov002_0208ff18(OtosuMenuObj* menuObj);
void             func_ov002_0208ff6c(OtosuMenuObj* menuObj);
PrcStepResult    func_ov002_0208ffac(PrcCtx* ctx, void* object);
PrcStepResult    OtosuPrcStep_CheckButtonInput(PrcCtx* ctx, void* object);
SpriteFrameInfo* func_ov002_020901a0(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_0209023c(OtosuMenuObj* menuObj);
void             func_ov002_020902a4(s32 arg0, OtosuMenuObj* menuObj);
void             func_ov002_02090310(s32 arg0, OtosuMenuObj* menuObj);
void             func_ov002_0209034c(void);
void             func_ov002_02090350(void);
void             func_ov002_02090354(OtosuMenuObj* menuObj);
void             func_ov002_020904cc(OtosuMenuObj* menuObj, u16* arg1, u16* arg2);
PrcStepResult    func_ov002_0209095c(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090b30(PrcCtx* ctx, void*);
PrcStepResult    func_ov002_02090b44(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090c28(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090d0c(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090df0(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090ee8(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090f4c(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02090fd8(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_020910b0(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091110(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091208(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091300(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_020913d8(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_020914b0(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091510(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091608(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091700(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091710(PrcCtx* ctx, void* arg1);
PrcStepResult    func_ov002_02091720(PrcCtx* ctx, void* arg1);
SpriteFrameInfo* func_ov002_02091760(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_020917fc(PrcCtx* ctx, Sprite* sprites);
void             func_ov002_020918ec(s32 arg0, s32 arg1);
void             func_ov002_02091918(s32 arg0, s32 arg1);
void             func_ov002_02091944(s32 arg0, s32 arg1);
PrcStepResult    func_ov002_02091970(PrcCtx* ctx, void* arg1);
SpriteFrameInfo* func_ov002_02091978(Sprite* sprite, s32 arg, s32 mode);
void             func_ov002_02091a14(s32 arg0, void* arg1);
void             func_ov002_02091a6c(s32 arg0, void* arg1);
void             func_ov002_02091a7c(s32 arg0, void* arg1);
void             func_ov002_02091a8c(s32 arg0, void* arg1);
PrcStepResult    func_ov002_02091a9c(PrcCtx* ctx, void* arg1);

#endif // OTOSUMENU_SHARED_H
