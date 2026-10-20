#ifndef DEBUG_LAUNCHER_H
#define DEBUG_LAUNCHER_H

#include "EasyList.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/Input.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Text.h"
#include "SpriteMgr.h"
#include <nitro/gx.h>

typedef struct {
    /* 0x00 */ s32         unk_00;
    /* 0x04 */ const char* function;
    /* 0x08 */ const char* description;
    /* 0x0C */ u32         overlayId;
    /* 0x10 */ OverlayCB   overlayCB;
} DebugLauncherOption; // Size: 0x14

typedef struct {
    /* 0x00 */ s32                        unk_00;
    /* 0x04 */ const char*                creator;
    /* 0x08 */ const char*                description;
    /* 0x0C */ const DebugLauncherOption* options;
} DebugLauncherCategory; // Size: 0x10

typedef struct {
    /* 0x00000 */ BOOL             active;
    /* 0x00004 */ s32              overlay;
    /* 0x00008 */ OverlayCB        overlayCB;
    /* 0x0000C */ InputButtons     buttonState;
    /* 0x00014 */ s32              unk_14;
    /* 0x00018 */ s32              unk_18;
    /* 0x0001C */ s32              unk_1C;
    /* 0x00020 */ s32              selectedCategoryIndex;
    /* 0x00024 */ u32*             unk_24;
    /* 0x00028 */ char             unk_28[0x38];
    /* 0x00060 */ s32              unk_60;
    /* 0x00064 */ s32              unk_64;
    /* 0x00068 */ s32              selectedOptionIndex;
    /* 0x0006C */ u32*             unk_6C;
    /* 0x00070 */ char             unk_70[0x5C];
    /* 0x000CC */ ResourceManager  unk_CC;
    /* 0x1164C */ ResourceManager* unk_1164C;
    /* 0x11650 */ TaskPool         taskPool;
    /* 0x116D0 */ char             unk_116D0[0x4018];
    /* 0x156E8 */ TextObject       unk_156E8;
    /* 0x157FC */ char             unk_157FC[0x15B04 - 0x157FC];
    /* 0x15B04 */ TextObject       unk_15B04;
    /* 0x15C18 */ char             unk_15C18[0x15F14 - 0x15C18];
    /* 0x15F14 */ EasyList         unk_list_15F14;
    /* 0x15F54 */ char             unk_15F54[0x200];
} DebugLauncherState; // Size: 0x16154

extern void func_ov000_02082854(void* state);

extern void ProcessOverlay_FontTest(void* state);

extern void ProcessOverlay_OtosuMenu_SinglePlayerEnter(void* state);
extern void ProcessOverlay_OtosuMenu_MultiplayerEnter(void* state);
extern void ProcessOverlay_OtosuMenu_MultiplayerRanking(void* state);
extern void ProcessOverlay_OtosuMenu_SinglePlayerRanking(void* state);
extern void ProcessOverlay_OtosuMenu_ConnectionError(void* state);
extern void ProcessOverlay_OtosuMenu_RoleSelection(void* state);
extern void ProcessOverlay_OtosuMenu_FontList(void* state);
extern void ProcessOverlay_OtosuMenu_DataDeletion(void* state);
extern void ProcessOverlay_OtosuMenu_DataCorrupted(void* state);
extern void ProcessOverlay_OtosuMenu_DataLoadFailure(void* state);
extern void ProcessOverlay_OtosuMenu_DataSaveFailure(void* state);
extern void ProcessOverlay_Continue(void* state);

extern void ProcessOverlay_Tutorial(void* state);

extern void func_ov027_020e860c(void* state);
extern void func_ov027_020e9670(void* state);

extern void func_ov028_020e82d0(void* state);

extern void func_ov029_02082f9c(void* state);

extern void func_ov030_020b0fe8(void* state);
extern void func_ov030_020b0e0c(void* state);
extern void func_ov030_020b17b4(void* state);
extern void func_ov030_020b6dec(void* state);
extern void ProcessOverlay_FldNpcMesSel(void* state);
extern void ProcessOverlay_FldMesSel(void* state);
extern void ProcessOverlay_LocaTitleSeq(void* state);
extern void ProcessOverlay_FlashSel(void* state);
extern void ProcessOverlay_OpenEnd(void* state);

extern void ProcessOverlay_GrpCheck_HwSprites(void* state);
extern void ProcessOverlay_GrpCheck_SwSprites(void* state);
extern void func_ov041_02082bc4(void* state);

extern void func_ov042_020824a0(void* state);

extern void ProcessOverlay_TakTest(void* state);
extern void ProcessOverlay_MenuTop(void* state);
extern void ProcessOverlay_MenuBadge(void* state);
extern void func_ov043_0209bce4(void* state);
extern void ProcessOverlay_MenuEquip(void* state);
extern void func_ov043_020aeee0(void* state);
extern void func_ov043_020bd414(void* state);
extern void func_ov043_020c04f0(void* state);
extern void func_ov043_020c6644(void* state);

extern void ProcessOverlay_Result(void* state);

extern void ProcessOverlay_Tusin(void* state);
extern void ProcessOverlay_TusinSet(void* state);
extern void func_ov045_02091034(void* state);

const DebugLauncherOption Options_Horii[2] = {
    // "Feature: Noise Report", "Description: Menu screen noise report"
    {1, "ï¿½@ï¿½\\:ï¿½mï¿½Cï¿½Yï¿½ï¿½ï¿½|ï¿½[ï¿½g", "ï¿½ï¿½ï¿½ï¿½:ï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[ï¿½ï¿½Ê‚Ìƒmï¿½Cï¿½Yï¿½ï¿½ï¿½|ï¿½[ï¿½g",                  28, func_ov028_020e82d0},
    {0,                   NULL,                            NULL, OVERLAY_ID_UNLOADED,                NULL},
};

const DebugLauncherOption Options_Suyama[2] = {
    // "Feature: Staff Roll", "Description: Staff roll screen"
    {1, "ï¿½@ï¿½\\:ï¿½Xï¿½^ï¿½bï¿½tï¿½ï¿½ï¿½[ï¿½ï¿½", "ï¿½ï¿½ï¿½ï¿½:ï¿½Xï¿½^ï¿½bï¿½tï¿½ï¿½ï¿½[ï¿½ï¿½ï¿½ï¿½ï¿½",                  42, func_ov042_020824a0},
    {0,                   NULL,                     NULL, OVERLAY_ID_UNLOADED,                NULL},
};

const DebugLauncherOption Options_Takami[14] = {
    // "Feature: test2", "Description: test2"
    {1,            "ï¿½@ï¿½\\:test2",                  "ï¿½ï¿½ï¿½ï¿½:test2",                  43,   ProcessOverlay_TakTest},
    // "Feature: Menu Top", "Description: First screen of the menu"
    {2,   "ï¿½@ï¿½\\:ï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[ï¿½gï¿½bï¿½v",         "ï¿½ï¿½ï¿½ï¿½:ï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[ï¿½ÌÅï¿½ï¿½Ì‰ï¿½ï¿½",                  43,   ProcessOverlay_MenuTop},
    // "Feature: Badge Select", "Description: Screen for organizing badges"
    {3,   "ï¿½@ï¿½\\:ï¿½oï¿½bï¿½Wï¿½Zï¿½ï¿½ï¿½Nï¿½g",       "ï¿½ï¿½ï¿½ï¿½:ï¿½oï¿½bï¿½Wï¿½ï¿½Òï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½",                  43, ProcessOverlay_MenuBadge},
    // "Feature: Scenario Select", "Description: Scenario selection screen"
    {4, "ï¿½@ï¿½\\:ï¿½Vï¿½iï¿½ï¿½ï¿½Iï¿½Zï¿½ï¿½ï¿½Nï¿½g",    "ï¿½ï¿½ï¿½ï¿½:ï¿½Vï¿½iï¿½ï¿½ï¿½Iï¿½ï¿½Iï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½",                  43,      func_ov043_0209bce4},
    // "Feature: Equipment", "Description: Equipment selection screen"
    {5,           "ï¿½@ï¿½\\:ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½",       "ï¿½ï¿½ï¿½ï¿½:ï¿½ï¿½ï¿½ï¿½ï¿½Ñ‚ï¿½Iï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½",                  43, ProcessOverlay_MenuEquip},
    // "Feature: Result", "Description: Result screen after battle"
    {6,         "ï¿½@ï¿½\\:ï¿½ï¿½ï¿½Uï¿½ï¿½ï¿½g", "ï¿½ï¿½ï¿½ï¿½:ï¿½oï¿½gï¿½ï¿½ï¿½Iï¿½ï¿½ï¿½ï¿½Ìƒï¿½ï¿½Uï¿½ï¿½ï¿½gï¿½ï¿½ï¿½",                  44,    ProcessOverlay_Result},
    // "Feature: Shop", "Description: Shop screen"
    {7,         "ï¿½@ï¿½\\:ï¿½Vï¿½ï¿½ï¿½bï¿½v",            "ï¿½ï¿½ï¿½ï¿½:ï¿½Vï¿½ï¿½ï¿½bï¿½vï¿½ï¿½ï¿½",                  43,      func_ov043_020aeee0},
    // "Feature: Department Store", "Description: Department store screen"
    {8,         "ï¿½@ï¿½\\:ï¿½fï¿½pï¿½[ï¿½g",            "ï¿½ï¿½ï¿½ï¿½:ï¿½fï¿½pï¿½[ï¿½gï¿½ï¿½ï¿½",                  43,      func_ov043_020bd414},
    // "Feature: Save", "Description: Save screen"
    {9,           "ï¿½@ï¿½\\:ï¿½Zï¿½[ï¿½u",              "ï¿½ï¿½ï¿½ï¿½:ï¿½Zï¿½[ï¿½uï¿½ï¿½ï¿½",                  43,      func_ov043_020c04f0},
    // "Feature: Test2", "Description: Test2"
    {1,          "ï¿½@ï¿½\\:ï¿½eï¿½Xï¿½g2",                "ï¿½ï¿½ï¿½ï¿½:ï¿½eï¿½Xï¿½g2",                  43,      func_ov043_020c6644},
    // "Feature: Communication Menu", "Description: Communication menu screen"
    {2,    "ï¿½@ï¿½\\:ï¿½ÊMï¿½ï¿½ï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",       "ï¿½ï¿½ï¿½ï¿½:ï¿½ÊMï¿½ï¿½ï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[ï¿½ï¿½ï¿½",                  45,     ProcessOverlay_Tusin},
    // "Feature: Communication Setup", "Description: Communication setup screen"
    {3,        "ï¿½@ï¿½\\:ï¿½ÊMï¿½Zï¿½bï¿½g",           "ï¿½ï¿½ï¿½ï¿½:ï¿½ÊMï¿½Zï¿½bï¿½gï¿½ï¿½ï¿½",                  45,  ProcessOverlay_TusinSet},
    // "Feature: Friend List", "Description: Friend list screen"
    {4,   "ï¿½@ï¿½\\:ï¿½tï¿½ï¿½ï¿½ï¿½ï¿½hï¿½ï¿½ï¿½Xï¿½g",      "ï¿½ï¿½ï¿½ï¿½:ï¿½tï¿½ï¿½ï¿½ï¿½ï¿½hï¿½ï¿½ï¿½Xï¿½gï¿½ï¿½ï¿½",                  45,      func_ov045_02091034},
    {0,                     NULL,                          NULL, OVERLAY_ID_UNLOADED,                     NULL},
};

const DebugLauncherOption Options_Fukuda[13] = {
    // "Features: Font", "Description: Test"
    {1,           "ï¿½@ï¿½\\:ï¿½tï¿½Hï¿½ï¿½ï¿½g",                    "ï¿½ï¿½ï¿½ï¿½:ï¿½eï¿½Xï¿½g",                   1,                      ProcessOverlay_FontTest},
    // "Feature: Font", "Description: Font list"
    {2,           "ï¿½@ï¿½\\:ï¿½tï¿½Hï¿½ï¿½ï¿½g",                "ï¿½ï¿½ï¿½ï¿½:ï¿½tï¿½Hï¿½ï¿½ï¿½gï¿½ê——",                   2,            ProcessOverlay_OtosuMenu_FontList},
    // "Feature: Title (?)", "Description: Data Deletion Screen"
    {3,     "ï¿½@ï¿½\\:ï¿½^ï¿½Cï¿½gï¿½ï¿½ï¿½iï¿½Hï¿½j",              "ï¿½ï¿½ï¿½ï¿½:ï¿½fï¿½[ï¿½^ï¿½íœï¿½ï¿½ï¿½",                   2,        ProcessOverlay_OtosuMenu_DataDeletion},
    // "Feature: Title (?)", "Description: Data Corruption Screen"
    {4,     "ï¿½@ï¿½\\:ï¿½^ï¿½Cï¿½gï¿½ï¿½ï¿½iï¿½Hï¿½j",             "ï¿½ï¿½ï¿½ï¿½:ï¿½fï¿½[ï¿½^ï¿½jï¿½ï¿½ï¿½ï¿½ï¿½",                   2,       ProcessOverlay_OtosuMenu_DataCorrupted},
    // "Feature: BayBadge Menu", "Description: Single Player Entrance"
    {5, "ï¿½@ï¿½\\:ï¿½xï¿½Cï¿½oï¿½bï¿½Wï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",               "ï¿½ï¿½ï¿½ï¿½:ï¿½Pï¿½lï¿½pï¿½Ì“ï¿½ï¿½ï¿½ï¿½",                   2,   ProcessOverlay_OtosuMenu_SinglePlayerEnter},
    // "Feature: BayBadge Menu", "Description: Single Player Return (Ranking)"
    {6, "ï¿½@ï¿½\\:ï¿½xï¿½Cï¿½oï¿½bï¿½Wï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",    "ï¿½ï¿½ï¿½ï¿½:ï¿½Pï¿½lï¿½pï¿½Ì–ß‚ï¿½iï¿½ï¿½ï¿½ï¿½ï¿½Lï¿½ï¿½ï¿½Oï¿½j",                   2, ProcessOverlay_OtosuMenu_SinglePlayerRanking},
    // "Feature: BayBadge Menu", "Description: Gateway to online battles"
    {7, "ï¿½@ï¿½\\:ï¿½xï¿½Cï¿½oï¿½bï¿½Wï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",                "ï¿½ï¿½ï¿½ï¿½:ï¿½ÊMï¿½Îï¿½Ì“ï¿½ï¿½ï¿½ï¿½",                   2,    ProcessOverlay_OtosuMenu_MultiplayerEnter},
    // "Feature: BayBadge Menu", "Description: Return from game (Ranking)"
    {8, "ï¿½@ï¿½\\:ï¿½xï¿½Cï¿½oï¿½bï¿½Wï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[", "ï¿½ï¿½ï¿½ï¿½:ï¿½Qï¿½[ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ì–ß‚ï¿½iï¿½ï¿½ï¿½ï¿½ï¿½Lï¿½ï¿½ï¿½Oï¿½j",                   2,  ProcessOverlay_OtosuMenu_MultiplayerRanking},
    // "Feature: BayBadge Menu", "Description: Error entry point"
    {9, "ï¿½@ï¿½\\:ï¿½xï¿½Cï¿½oï¿½bï¿½Wï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",             "ï¿½ï¿½ï¿½ï¿½:ï¿½Gï¿½ï¿½ï¿½[ï¿½pï¿½Ì“ï¿½ï¿½ï¿½ï¿½",                   2,     ProcessOverlay_OtosuMenu_ConnectionError},
    // "Feature: Title (?)", "Description: Data Load Failure Screen"
    {1,     "ï¿½@ï¿½\\:ï¿½^ï¿½Cï¿½gï¿½ï¿½ï¿½iï¿½Hï¿½j",        "ï¿½ï¿½ï¿½ï¿½:ï¿½fï¿½[ï¿½^ï¿½Ç‚İï¿½ï¿½İï¿½ï¿½sï¿½ï¿½ï¿½",                   2,     ProcessOverlay_OtosuMenu_DataLoadFailure},
    // "Feature: Title (?)", "Description: Data Save Failure Screen"
    {2,     "ï¿½@ï¿½\\:ï¿½^ï¿½Cï¿½gï¿½ï¿½ï¿½iï¿½Hï¿½j",      "ï¿½ï¿½ï¿½ï¿½:ï¿½fï¿½[ï¿½^ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½İï¿½ï¿½sï¿½ï¿½ï¿½",                   2,     ProcessOverlay_OtosuMenu_DataSaveFailure},
    // "Feature: BayBadge Menu", "Description: Role Selection Screen"
    {3, "ï¿½@ï¿½\\:ï¿½xï¿½Cï¿½oï¿½bï¿½Wï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",               "ï¿½ï¿½ï¿½ï¿½:ï¿½ï¿½ï¿½ï¿½ï¿½Iï¿½ï¿½ï¿½ï¿½ï¿½",                   2,       ProcessOverlay_OtosuMenu_RoleSelection},
    {0,                       NULL,                             NULL, OVERLAY_ID_UNLOADED,                                         NULL},
};

const DebugLauncherOption Options_Bul[2] = {
    // "Feature: Opening and Title", "Description:"
    {3, "ï¿½@ï¿½\\:ï¿½Iï¿½[ï¿½vï¿½jï¿½ï¿½ï¿½Oï¿½Æƒ^ï¿½Cï¿½gï¿½ï¿½", "ï¿½ï¿½ï¿½ï¿½:",                  37, ProcessOverlay_OpenEnd},
    {0,                          NULL,    NULL, OVERLAY_ID_UNLOADED,                   NULL},
};

const DebugLauncherOption Options_Abe[2] = {
    {1, "mini108", "\0",                   0, func_ov000_02082854},
    {0,      NULL, NULL, OVERLAY_ID_UNLOADED,                NULL},
};

const DebugLauncherOption Options_Sugata[2] = {
    // "Feature: StreetPass Sample", "Description: StreetPass mode"
    {3, "ï¿½@ï¿½\\:ï¿½ï¿½ï¿½ê‚¿ï¿½ï¿½ï¿½ï¿½ï¿½Tï¿½ï¿½ï¿½vï¿½ï¿½", "ï¿½ï¿½ï¿½ï¿½:ï¿½ï¿½ï¿½ê‚¿ï¿½ï¿½ï¿½ï¿½ï¿½ÊMï¿½ï¿½ï¿½[ï¿½h",                  41, func_ov041_02082bc4},
    {0,                      NULL,                      NULL, OVERLAY_ID_UNLOADED,                NULL},
};

const DebugLauncherOption Options_Kitawaki[9] = {
    {1,  "‹@”\\:ƒtƒB[ƒ‹ƒh<->í“¬",    "à–¾:ƒtƒB[ƒ‹ƒh‚Åí“¬‹N‚±‚é",                  30,         func_ov030_020b0fe8},
    {2,     "‹@”\\:Fld/ƒtƒB[ƒ‹ƒh",        "à–¾:3DƒtƒB[ƒ‹ƒhƒeƒXƒg",                  30,         func_ov030_020b0e0c},
    {3,                "‹@”\\:imd",             "à–¾:imd disp test",                  30,         func_ov030_020b6dec},
    {4,         "‹@”\\:sound test",                          "à–¾:",                  29,         func_ov029_02082f9c},
    {5,       "‹@”\\:event select", "à–¾:debug—p‚ÌƒCƒxƒ“ƒgƒZƒŒƒNƒg",                  30,         func_ov030_020b17b4},
    {6,              "‹@”\\:flash",              "à–¾:flash select",                  30,     ProcessOverlay_FlashSel},
    {7, "‹@”\\:NPC message select",        "à–¾:NPC message select",                  30, ProcessOverlay_FldNpcMesSel},
    {8,     "‹@”\\:message select",            "à–¾:message select",                  30,    ProcessOverlay_FldMesSel},
    {0,                       NULL,                             NULL, OVERLAY_ID_UNLOADED,                        NULL},
};

const DebugLauncherOption Options_Furukawa[4] = {
    {1,      "ï¿½@ï¿½\\:ChkBtl/ï¿½oï¿½gï¿½ï¿½ï¿½`ï¿½Fï¿½bï¿½N",         "ï¿½ï¿½ï¿½ï¿½:ï¿½oï¿½gï¿½ï¿½ï¿½`ï¿½Fï¿½bï¿½N",                  27,     func_ov027_020e860c},
    {2, "ï¿½@ï¿½\\:Continue/ï¿½Rï¿½ï¿½ï¿½eï¿½Bï¿½jï¿½ï¿½ï¿½[ï¿½ï¿½ï¿½", "ï¿½ï¿½ï¿½ï¿½:ï¿½Rï¿½ï¿½ï¿½eï¿½Bï¿½jï¿½ï¿½ï¿½[ï¿½ï¿½ï¿½jï¿½ï¿½ï¿½[",                  25, ProcessOverlay_Continue},
    {3, "ï¿½@ï¿½\\:Tutorial/ï¿½`ï¿½ï¿½ï¿½[ï¿½gï¿½ï¿½ï¿½Aï¿½ï¿½ï¿½ï¿½ï¿½",         "ï¿½ï¿½ï¿½ï¿½:ï¿½`ï¿½ï¿½ï¿½[ï¿½gï¿½ï¿½ï¿½Aï¿½ï¿½",                  26, ProcessOverlay_Tutorial},
    {0,                               NULL,                          NULL, OVERLAY_ID_UNLOADED,                    NULL},
};

const DebugLauncherOption Options_Mori[6] = {
    // "Function: Graphic Check", "Description: Hardware Sprites"
    {1,  "ï¿½@ï¿½\\:ï¿½Oï¿½ï¿½ï¿½tï¿½Bï¿½bï¿½Nï¿½`ï¿½Fï¿½bï¿½N",  "ï¿½ï¿½ï¿½ï¿½:ï¿½nï¿½[ï¿½hï¿½Eï¿½Fï¿½Aï¿½Xï¿½vï¿½ï¿½ï¿½Cï¿½g",                  38, ProcessOverlay_GrpCheck_HwSprites},
    // "Function: Graphic Check", "Description: Software Sprites"
    {2,  "ï¿½@ï¿½\\:ï¿½Oï¿½ï¿½ï¿½tï¿½Bï¿½bï¿½Nï¿½`ï¿½Fï¿½bï¿½N", "ï¿½ï¿½ï¿½ï¿½:ï¿½\\ï¿½tï¿½gï¿½Eï¿½Fï¿½Aï¿½Xï¿½vï¿½ï¿½ï¿½Cï¿½g",                  38, ProcessOverlay_GrpCheck_SwSprites},
    {3, "ï¿½@ï¿½\\:ChkBtl/ï¿½oï¿½gï¿½ï¿½ï¿½`ï¿½Fï¿½bï¿½N",          "ï¿½ï¿½ï¿½ï¿½:ï¿½oï¿½gï¿½ï¿½ï¿½`ï¿½Fï¿½bï¿½N",                  27,               func_ov027_020e9670},
    {4,          "ï¿½@ï¿½\\:ï¿½ï¿½ï¿½Pï¿½^ï¿½Cï¿½gï¿½ï¿½",                    "ï¿½ï¿½ï¿½ï¿½:test",                  30,       ProcessOverlay_LocaTitleSeq},
    {5, "ï¿½@ï¿½\\:ï¿½Iï¿½[ï¿½vï¿½jï¿½ï¿½ï¿½Oï¿½Æƒ^ï¿½Cï¿½gï¿½ï¿½",                        "ï¿½ï¿½ï¿½ï¿½:",                  37,            ProcessOverlay_OpenEnd},
    {0,                          NULL,                           NULL, OVERLAY_ID_UNLOADED,                              NULL},
};

const DebugLauncherCategory Categories[11] = {
    {21,     "ï¿½ì¬ï¿½Fï¿½kï¿½e", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Kitï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½", Options_Kitawaki},
    {16,      "ï¿½ì¬ï¿½Fï¿½Ãï¿½", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Furï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½", Options_Furukawa},
    {12, "ï¿½ì¬ï¿½Fï¿½oï¿½ï¿½ï¿½bï¿½g", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Bulï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",      Options_Bul},
    {23,       "ï¿½ì¬ï¿½Fï¿½X", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Morï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",     Options_Mori},
    {11,     "ï¿½ì¬ï¿½Fï¿½ï¿½ï¿½{", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Abeï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",      Options_Abe},
    {30,     "ï¿½ì¬ï¿½Fï¿½ï¿½ï¿½ï¿½", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Takï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",   Options_Takami},
    {29,     "ï¿½ì¬ï¿½Fï¿½ï¿½ï¿½c", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Sugï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",   Options_Sugata},
    {16,     "ï¿½ì¬ï¿½Fï¿½ï¿½ï¿½c", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Fukï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",   Options_Fukuda},
    {18,     "ï¿½ì¬ï¿½Fï¿½xï¿½ï¿½", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Horï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",    Options_Horii},
    {29,     "ï¿½ì¬ï¿½Fï¿½{ï¿½R", "ï¿½ï¿½ï¿½ï¿½ï¿½FApl_Suyï¿½fï¿½Bï¿½ï¿½ï¿½Nï¿½gï¿½ï¿½",   Options_Suyama},
    { 0,            NULL,                        NULL,             NULL},
};

// const s32 data_ov046_02083a34[3] = {0, 0x1000, 0};
// const s32 data_ov046_02083a40[3] = {0, 0, -0x1000};

void func_ov046_02083670(u16* unkptr, BOOL param_2);
void func_ov046_0208368c(DebugLauncherState* state);

#endif // DEBUG_LAUNCHER_H