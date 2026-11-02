#ifndef DMA_H
#define DMA_H

#include "Field/FieldRegion.h"
#include <nitro/types.h>

typedef struct {
    union {
        /* 0x00 */ u32 unk_00;
        /* 0x00 */ u8  unk_00_08;
    };
    /* 0x04 */ void* addr;
    /* 0x08 */ s32   size;
    /* 0x0C */ void* data;
} DmaRequest;

typedef struct {
    /* 0x00 */ DmaRequest  active;
    /* 0x10 */ DmaRequest* queue[16];
} DmaMgr;

extern DmaMgr data_02066a5c;

extern DmaRequest* data_02066aac[16];

typedef void (*DMAFunc)(DmaRequest*);

void DMA_Init(s32 size);

/**
 * @brief Purge data cache, process all active DMA operations, then reset DMA state.
 */
void DMA_Flush(void);

BOOL func_02001b44(s32 idx, s32 arg1, const void* req, s32 arg3);

#endif // DMA_H