#ifndef TWEWY_BATTLE_CORE_BTLSTAGE_H
#define TWEWY_BATTLE_CORE_BTLSTAGE_H

#include "Display.h"

typedef enum BattleStageFlags {
    BATTLE_STAGE_FLAG_BG_IN_USE = 1 << 0, // a task owns this screen's background; the battle waits for it to clear
} BattleStageFlags;

/// One screen's battle stage. Includes the field background it shows, the field rectangle actors move in, the sloped top
/// edge of the floor (main screen only), and the camera that looks at it. Positions are 20.12 fixed point; field
/// positions are relative to the field's top-left corner, BG positions are in the 512x512 background.
typedef struct BattleStage {
    /* 0x00 */ u16 fieldId; // index into BtlField_UG_Defs (main) / BtlField_RG_Defs (sub)
    /* 0x02 */ u16 flags;   // BattleStageFlags
    /* 0x04 */ s32 left;    // field rectangle in the background
    /* 0x08 */ s32 top;
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
    /* 0x14 */ s32 viewTop;    // highest point the camera may show, relative to the field's top edge
    /* 0x18 */ s32 floorTop;   // main stage: top of the floor at the field's left edge, relative to the field's top
    /* 0x1C */ u16 floorSlope; // main stage: angle of the floor's top edge, 0x10000 = a full turn
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s32 centerX;    // field position battles centre things on; starts at the middle of the field
    /* 0x24 */ s32 centerY;
    /* 0x28 */ s32 viewX;      // BG position at the centre of the screen
    /* 0x2C */ s32 focusY;     // eased towards targetY
    /* 0x30 */ s32 focusZ;     // eased towards targetZ
    /* 0x34 */ s32 targetX;    // BG position the camera moves towards, see BtlStage_SetCameraTarget
    /* 0x38 */ s32 targetY;
    /* 0x3C */ s32 targetZ;
    /* 0x40 */ s32 viewY;  // BG position at the centre of the screen
    /* 0x44 */ s32 shakeX; // jitter added to the view after it is clamped
    /* 0x48 */ s32 shakeY;
    /* 0x4C */ s32 shakeZ;
    /* 0x50 */ s32 targetOffsetZ; // added to every camera target's height
    /* 0x54 */ s32 panOffsetY;    // vertical view offset of the opening pan; starts at viewTop and eases to 0
} BattleStage;                    // Size: 0x58

/**
 * @brief Lays out the main screen's stage from the field definition of its background
 *
 * Reads the layout of `fieldId` from BtlField_UG_Defs: the field rectangle, the sloped top edge of the
 * floor that BtlStage_IsOutsideFloor and the clamps keep actors below, and how far up the camera may look.
 */
void BtlStage_InitMain(void);

/**
 * @brief Lays out the sub screen's stage from the field definition of its background
 *
 * Reads the layout of `fieldId` from BtlField_RG_Defs.
 */
void BtlStage_InitSub(void);

/**
 * @brief Sets the field rectangle of a stage and centres the field on it
 *
 * Also clears the camera's shake offset.
 *
 * @param engine The screen whose stage is set
 * @param x0 Left edge of the field, in pixels
 * @param y0 Top edge of the field, in pixels
 * @param x1 Right edge of the field, in pixels
 * @param y1 Bottom edge of the field, in pixels
 */
void BtlStage_SetFieldRect(DisplayEngine engine, s32 x0, s32 y0, s32 x1, s32 y1);

/**
 * @brief Sets the point a stage's camera moves towards
 *
 * @param engine The screen whose camera moves
 * @param x Horizontal position relative to the field's left edge, 20.12 fixed point
 * @param y Vertical position relative to the field's top edge, 20.12 fixed point
 * @param z Height, 20.12 fixed point; the stage's targetOffsetZ is added
 */
void BtlStage_SetCameraTarget(DisplayEngine engine, s32 x, s32 y, s32 z);

/**
 * @brief Checks whether a box sticks out of the main stage's floor
 *
 * The top edge is the sloped top of the floor at `x`.
 *
 * @param x Horizontal centre of the box in the field, 20.12 fixed point
 * @param y Vertical centre of the box in the field, 20.12 fixed point
 * @param marginX Half the box width, 20.12 fixed point
 * @param marginY Half the box height, 20.12 fixed point
 * @return TRUE if any edge of the box is outside the field, FALSE if it is inside
 */
BOOL BtlStage_IsOutsideFloor(s32 x, s32 y, s32 marginX, s32 marginY);

/**
 * @brief Pushes a vertical position down below the top of the main stage's floor
 *
 * @param value The vertical position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edge, 20.12 fixed point
 * @param x Horizontal position the sloped top edge is measured at, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampToFloorTop(s32* value, s32 margin, s32 x);

/**
 * @brief Clamps a vertical position between the top of the main stage's floor and the field's bottom edge
 *
 * @param value The vertical position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edges, 20.12 fixed point
 * @param x Horizontal position the sloped top edge is measured at, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampYToFloor(s32* value, s32 margin, s32 x);

/**
 * @brief Clamps a horizontal position between the left and right edges of the main stage's field
 *
 * @param value The horizontal position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edges, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampXToField(s32* value, s32 margin);

/**
 * @brief Clamps a position into the main stage's floor
 *
 * Clamps `x` first, then `y` against the floor's top edge at the clamped `x`.
 *
 * @param x The horizontal position to clamp, 20.12 fixed point; updated in place
 * @param y The vertical position to clamp, 20.12 fixed point; updated in place
 * @param marginX Distance to keep from the left and right edges, 20.12 fixed point
 * @param marginY Distance to keep from the top and bottom edges, 20.12 fixed point
 * @return TRUE if either coordinate was moved
 */
BOOL BtlStage_ClampToFloor(s32* x, s32* y, s32 marginX, s32 marginY);

/**
 * @brief Clamps a vertical position between the top and bottom edges of the main stage's field
 *
 * Unlike BtlStage_ClampYToFloor, the top edge is the field's own, not the sloped top of the floor.
 *
 * @param value The vertical position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edges, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampYToField(s32* value, s32 margin);

/**
 * @brief Picks the field background of the battle and lays out both stages for it
 *
 * The background comes from the save, except for the encounters that bring their own.
 */
void BtlStage_Init(void);

/**
 * @brief Starts the two background tasks (main and sub screen) of the current battle
 *
 * Boss encounters start their own background tasks from other overlays; every other battle starts
 * the field tasks.
 *
 * @param flag Passed to the tasks; 0 also refreshes the area's leading brand in the save first
 */
void BtlStage_StartFieldTasks(s32 flag);

/**
 * @brief Starts the sandstorm transition task
 */
void BtlStage_StartSandstorm(void);

/**
 * @brief Projects a field position onto a screen
 *
 * @param engine The screen whose camera is used
 * @param screenX Receives the horizontal screen position, in pixels
 * @param screenY Receives the vertical screen position, in pixels
 * @param x Horizontal field position, 20.12 fixed point
 * @param y Vertical field position, 20.12 fixed point
 * @param z Height offset added to `y`, 20.12 fixed point
 */
void BtlStage_WorldToScreen(DisplayEngine engine, s16* screenX, s16* screenY, s32 x, s32 y, s32 z);

/**
 * @brief Projects a horizontal field position onto a screen
 *
 * @param engine The screen whose camera is used
 * @param x Horizontal field position, 20.12 fixed point
 * @return The horizontal screen position, in pixels
 */
s16 BtlStage_ToScreenX(DisplayEngine engine, s32 x);

/**
 * @brief Projects a vertical field position onto a screen
 *
 * @param engine The screen whose camera is used
 * @param y Vertical field position, 20.12 fixed point
 * @param z Height offset added to `y`, 20.12 fixed point
 * @return The vertical screen position, in pixels
 */
s16 BtlStage_ToScreenY(DisplayEngine engine, s32 y, s32 z);

/**
 * @brief Converts a horizontal screen position into a field position
 *
 * @param engine The screen whose camera is used
 * @param screenX Horizontal screen position, in pixels
 * @return The horizontal field position, 20.12 fixed point
 */
s32 BtlStage_ToWorldX(DisplayEngine engine, s32 screenX);

/**
 * @brief Converts a vertical screen position into a field position
 *
 * @param engine The screen whose camera is used
 * @param screenY Vertical screen position, in pixels
 * @param height Height offset subtracted from the result, 20.12 fixed point
 * @return The vertical field position, 20.12 fixed point
 */
s32 BtlStage_ToWorldY(DisplayEngine engine, s32 screenY, s32 height);

#endif // TWEWY_BATTLE_CORE_BTLSTAGE_H
