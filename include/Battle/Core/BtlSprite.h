#ifndef BATTLE_CORE_BTLSPRITE_H
#define BATTLE_CORE_BTLSPRITE_H

#include "Engine/File/BinMgr.h"
#include "SpriteMgr.h"

/// One animation of a battle sprite: the pack entries of the sprite's file it is drawn from.
typedef struct SpriteAnimEntry {
    u16 charDataIndex; // char data entry
    u16 animDataIndex; // animation data entry
    u16 cellDataIndex; // cell table entry
    s16 animIndex;     // animation within the animation data
} SpriteAnimEntry;

typedef enum BattleSpriteFlags {
    BTL_SPRITE_FLAG_FLIPPED       = (1 << 0),  // facing mirrored: OAM h-flip, negated affine scaleX
    BTL_SPRITE_FLAG_OBJ_WINDOW    = (1 << 1),  // drawn in OBJ window mode
    BTL_SPRITE_FLAG_TRANSLUCENT   = (1 << 2),  // drawn semi-transparent; takes priority over BTL_SPRITE_FLAG_OBJ_WINDOW
    BTL_SPRITE_FLAG_PALETTE_FLASH = (1 << 12), // flash palette active until paletteTimer runs out
    BTL_SPRITE_FLAG_LOAD_FAILED   = (1 << 15), // Sprite_Load failed; nothing to draw
} BattleSpriteFlags;

/// Which palette a battle sprite is drawn with
typedef enum BattleSpritePaletteMode {
    BTL_SPRITE_PALETTE_OWN     = 0, // the sprite's own palette
    BTL_SPRITE_PALETTE_FLASH   = 1, // the battle's flash palette
    BTL_SPRITE_PALETTE_FLICKER = 2, // alternates between the two every frame
} BattleSpritePaletteMode;

/// A sprite drawn in battle: actors, effects, shots and the HUD.
typedef struct {
    /* 0x00 */ Sprite                 sprite;
    /* 0x40 */ SpriteAnimEntry*       animTable;         // animations for BtlSprite_SetAnimFromTable, NULL for direct loads
    /* 0x44 */ s16                    animTableIndex;    // animation being shown, -1 = none yet
    /* 0x46 */ u16                    flags;             // BattleSpriteFlags
    /* 0x48 */ s32                    sortKey;           // frame info sortKey: lower draws in front, -1 = unsorted
    /* 0x4C */ struct OamAffineParam* affine;            // frame info affine (3D path)
    /* 0x50 */ UnkSmallInternal*      basePaletteSource; // the sprite's own palette, restored after a flash
    /* 0x54 */ PaletteResource*       basePalette;
    /* 0x58 */ s32                    paletteMode;       // BattleSpritePaletteMode
    /* 0x5C */ s16                    paletteTimer;      // frames of BTL_SPRITE_FLAG_PALETTE_FLASH left
} BattleSprite;

/// Load arguments of a battle sprite.
typedef struct {
    SpriteAnimation        anim;
    const SpriteAnimEntry* animTable; // 0x2C, becomes BattleSprite.animTable
} SpriteAnimationEx;

/// Which file, animation table, first animation and palette a battle sprite is loaded from
typedef struct BattleSpriteDesc {
    /* 0x0 */ const BinIdentifier*   iden;
    /* 0x4 */ const SpriteAnimEntry* table;
    /* 0x8 */ u16                    entry;        // first animation
    /* 0xA */ u16                    paletteEntry; // palette data entry
    /* 0xC */ u16                    charTiles;    // char VRAM to reserve, in 32-byte units
} BattleSpriteDesc;

/**
 * @brief Switches the sprite to another animation of its animation table
 *
 * Swaps in the entry's animation data and cells, and for sprites that stream char data per cell
 * (BtlSprite_LoadFromTable), the entry's char data too.
 *
 * @param cSprite The sprite to change
 * @param animTableIndex Index into `animTable` of the animation to show
 * @param playOnce Bit 0 set plays the animation once, clear loops it
 * @return 1 if that animation was already showing and nothing changed, 0 if it switched
 */
s32 BtlSprite_SetAnimFromTable(BattleSprite* cSprite, u16 animTableIndex, s32 playOnce);

/**
 * @brief Switches the sprite to another animation of the animation data it already has loaded
 *
 * @param cSprite The sprite to change
 * @param animIndex The animation within the loaded animation data
 * @param playOnce Bit 0 set plays the animation once, clear loops it
 * @return 1 if that animation was already showing and nothing changed, 0 if it switched
 */
s32 BtlSprite_SetAnim(BattleSprite* cSprite, u16 animIndex, s32 playOnce);

/**
 * @brief Draws the sprite rotated and scaled, through an affine group of its screen
 *
 * A flipped sprite (BTL_SPRITE_FLAG_FLIPPED) has its horizontal scale negated, so the transform keeps
 * its facing.
 *
 * @param cSprite The sprite to transform
 * @param rotation Angle, 0x10000 = a full turn
 * @param scaleX Horizontal scale, 0x1000 = 1.0; negative mirrors horizontally
 * @param scaleY Vertical scale, 0x1000 = 1.0; negative mirrors vertically
 * @param doubleSize TRUE doubles the drawing area so a rotated or enlarged sprite is not clipped
 */
void BtlSprite_SetAffineTransform(BattleSprite* cSprite, u32 rotation, s32 scaleX, s32 scaleY, s32 doubleSize);

/**
 * @brief Recolours the sprite with another palette from its own file, keeping the palette slot it holds
 *
 * @param cSprite The sprite to recolour
 * @param paletteEntry Pack entry of the palette data in the sprite's file
 */
void BtlSprite_SetPaletteSource(BattleSprite* cSprite, s32 paletteEntry);

/**
 * @brief Moves the sprite on its screen
 *
 * @param cSprite The sprite to move
 * @param posX Horizontal screen position, in pixels
 * @param posY Vertical screen position, in pixels
 */
void BtlSprite_SetPosition(BattleSprite* cSprite, s32 posX, s32 posY);

/**
 * @brief Sets where the sprite is drawn among the other sprites of its screen
 *
 * Actors pass 0x7FFFFFFF minus their depth position so those nearer the bottom of the screen draw on top.
 *
 * @param cSprite The sprite to order
 * @param sortKey Lower keys draw in front of higher ones, -1 leaves the sprite unsorted
 */
void BtlSprite_SetSortKey(BattleSprite* cSprite, s32 sortKey);

/**
 * @brief Sets which BG layers the sprite draws in front of
 *
 * @param cSprite The sprite to change
 * @param priority OBJ priority 0 to 3, 0 = in front of every BG layer
 */
void BtlSprite_SetPriority(BattleSprite* cSprite, s32 priority);

/**
 * @brief Sets which way the sprite faces
 *
 * @param cSprite The sprite to turn
 * @param flipped TRUE mirrors the sprite horizontally and sets BTL_SPRITE_FLAG_FLIPPED, FALSE restores it
 */
void BtlSprite_SetFlip(BattleSprite* cSprite, s32 flipped);

/**
 * @brief Picks which palette the sprite is drawn with
 *
 * @param cSprite The sprite to recolour
 * @param mode The palette to use; BTL_SPRITE_PALETTE_FLICKER alternates in BtlSprite_Render
 */
void BtlSprite_SetPaletteMode(BattleSprite* cSprite, BattleSpritePaletteMode mode);

/**
 * @brief Draws the sprite once semi-transparent, regardless of its flags
 *
 * @param cSprite The sprite to draw
 * @param alpha Blend alpha for this draw, 0 (invisible) to 31 (opaque)
 */
void BtlSprite_RenderTranslucent(BattleSprite* cSprite, s32 alpha);

/**
 * @brief Frame info callback installed on every battle sprite by BtlSprite_InitAnim
 *
 * @param sprite The sprite being updated or drawn; its owner is the BattleSprite
 * @param arg The callback argument (unused)
 * @param mode SPRITE_FRAME_UPDATE advances one animation step; SPRITE_FRAME_RENDER draws the current
 * cell with the BattleSprite's sortKey and affine
 * @return The filled frame info, or NULL for any other mode
 */
SpriteFrameInfo* BtlSprite_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

/**
 * @brief Fills load arguments with the battle sprite defaults
 *
 * @param anim The load arguments to fill
 * @param engine The screen the sprite is drawn on (DISPLAY_MAIN, DISPLAY_SUB or DISPLAY_EXTENDED)
 * @param iden The file the sprite's resources come from
 */
void BtlSprite_InitAnim(SpriteAnimation* anim, s32 engine, const BinIdentifier* iden);

/**
 * @brief Loads the sprite and resets its battle state (no animation, unsorted, no flags, own palette)
 *
 * @param cSprite The sprite to load
 * @param anim The load arguments; its owner is set to `cSprite` and its animTable becomes the sprite's
 * @return 1 on success; 0 on failure, with BTL_SPRITE_FLAG_LOAD_FAILED set
 */
s32 BtlSprite_Load(BattleSprite* cSprite, SpriteAnimationEx* anim);

/**
 * @brief Loads a sprite whose animations come from an animation table
 *
 * Char data is streamed to VRAM per cell, so BtlSprite_SetAnimFromTable can switch to entries
 * with different char data.
 *
 * @param engine The screen the sprite is drawn on (DISPLAY_MAIN, DISPLAY_SUB or DISPLAY_EXTENDED)
 * @param cSprite The sprite to load
 * @param iden The file the sprite's resources come from
 * @param table The sprite's animation table
 * @param entry Index into `table` of the animation to start on
 * @param paletteEntry Pack entry of the palette data in the file
 * @param charTiles Char VRAM to reserve, in 32-byte units; 0 = the size of the largest cell
 * @return 1 on success; 0 on failure, with BTL_SPRITE_FLAG_LOAD_FAILED set
 */
s32 BtlSprite_LoadFromTable(s32 engine, BattleSprite* cSprite, const BinIdentifier* iden, const SpriteAnimEntry* table,
                            u16 entry, u16 paletteEntry, u16 charTiles);

/**
 * @brief Loads a sprite with all its char data at once and no animation table
 *
 * On failure the sprite gets BTL_SPRITE_FLAG_LOAD_FAILED.
 *
 * @param engine The screen the sprite is drawn on (DISPLAY_MAIN, DISPLAY_SUB or DISPLAY_EXTENDED)
 * @param cSprite The sprite to load
 * @param iden The file the sprite's resources come from
 * @param charEntry Pack entry of the char data in the file
 * @param animEntry Pack entry of the animation data
 * @param cellEntry Pack entry of the cell table
 * @param paletteEntry Pack entry of the palette data
 * @param animIndex The animation within the animation data to start on
 */
void BtlSprite_LoadDirect(s32 engine, BattleSprite* cSprite, const BinIdentifier* iden, s32 charEntry, u16 animEntry,
                          u16 cellEntry, u16 paletteEntry, u16 animIndex);

#define BtlSprite_LoadFromTableNoProto                                                                                   \
    ((void (*)(s32, BattleSprite*, const BinIdentifier*, const SpriteAnimEntry*, s32, s32, s32))BtlSprite_LoadFromTable)

#define BtlSprite_LoadDirectNoProto                                                                     \
    ((void (*)(s32, BattleSprite*, const BinIdentifier*, s32, s32, s32, s32, s32))BtlSprite_LoadDirect)

/**
 * @brief Clears the sprite's base Sprite to the unloaded state
 *
 * @param cSprite The sprite to clear
 */
void BtlSprite_Init(BattleSprite* cSprite);

/**
 * @brief Runs one frame of the sprite's animation
 *
 * While BTL_SPRITE_FLAG_PALETTE_FLASH is set, counts down `paletteTimer` and restores the sprite's own
 * palette when it runs out.
 *
 * @param cSprite The sprite to update
 */
void BtlSprite_Update(BattleSprite* cSprite);

/**
 * @brief Draws the sprite
 *
 * BTL_SPRITE_FLAG_TRANSLUCENT draws it semi-transparent, otherwise BTL_SPRITE_FLAG_OBJ_WINDOW draws it as
 * an OBJ window; BTL_SPRITE_PALETTE_FLICKER picks the flash palette on odd frames.
 *
 * @param cSprite The sprite to draw
 */
void BtlSprite_Render(BattleSprite* cSprite);

/**
 * @brief Restores the sprite's own palette and frees its resources
 *
 * @param cSprite The sprite to release
 */
void BtlSprite_Release(BattleSprite* cSprite);

/**
 * @brief Restarts the current animation from its first frame
 *
 * @param cSprite The sprite to restart
 */
void BtlSprite_Restart(BattleSprite* cSprite);

#endif // BATTLE_CORE_BTLSPRITE_H
