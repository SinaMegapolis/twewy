#ifndef DEBUG_NOISEREPORT_STRUCTS_H
#define DEBUG_NOISEREPORT_STRUCTS_H

#include "nitro/types.h"
#include "Engine/File/DatMgr.h"

// Generic struct definitions for m2c draft field access patterns
typedef struct {
    Data* unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    u8 pad[0x18];
} DataEntry34; // stride 0x34

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
} DataEntry2C; // stride ~0x34

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    u8 pad[0x20];
} DataEntry30; // stride ~0x30

#endif