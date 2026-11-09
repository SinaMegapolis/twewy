#ifndef TWEWY_BATTLE_CORE_BTLTOUCH_H
#define TWEWY_BATTLE_CORE_BTLTOUCH_H

#include "Battle/Core/BtlActor.h"

/// What the stylus did this frame, as recognized by BtlTouch_Update
typedef enum BattleTouchGesture {
    BTL_TOUCH_GESTURE_NONE      = 0, // stylus is up
    BTL_TOUCH_GESTURE_DOWN      = 1, // stylus is down, nothing recognized yet
    BTL_TOUCH_GESTURE_TAP       = 2, // quick short touch
    BTL_TOUCH_GESTURE_RAPID_TAP = 3, // the fifth quick tap in a row
    BTL_TOUCH_GESTURE_SLASH     = 4, // quick touch that travelled far
    BTL_TOUCH_GESTURE_HOLD      = 5, // long touch that stays where it started
    BTL_TOUCH_GESTURE_DRAG      = 6, // long touch that moves
    BTL_TOUCH_GESTURE_SCRIBBLE  = 7, // fast back and forth strokes
} BattleTouchGesture;

/// Which gestures BtlTouch_Update may recognize
typedef enum BattleTouchFlags {
    BTL_TOUCH_ALLOW_TAP       = (1 << 0),
    BTL_TOUCH_ALLOW_RAPID_TAP = (1 << 1),
    BTL_TOUCH_ALLOW_SLASH     = (1 << 2),
    BTL_TOUCH_ALLOW_HOLD      = (1 << 3),
    BTL_TOUCH_ALLOW_DRAG      = (1 << 4),
    BTL_TOUCH_ALLOW_SCRIBBLE  = (1 << 5),
} BattleTouchFlags;

/// What kind of actor the stylus came down on
typedef enum BattleTouchTargetKind {
    BTL_TOUCH_TARGET_NONE   = 0,
    BTL_TOUCH_TARGET_PLAYER = 1,
    BTL_TOUCH_TARGET_ENEMY  = 2,
    BTL_TOUCH_TARGET_OBJECT = 3,
} BattleTouchTargetKind;

/// The battle's touch recognizer state, shared by psych effects and actor selection. Positions are
/// bottom screen pixels.
typedef struct BattleTouchWork {
    /* 0x00 */ BOOL         active;   // the recognizer has run since BtlTouch_Clear
    /* 0x04 */ u16          flags;    // BattleTouchFlags
    /* 0x06 */ u16          gesture;  // BattleTouchGesture of this frame
    /* 0x08 */ s16          startX;   // where the stylus came down
    /* 0x0A */ s16          startY;
    /* 0x0C */ s16          endX;     // where the stylus came up
    /* 0x0E */ s16          endY;
    /* 0x10 */ s32          distance; // from start to end, 20.12 fixed point
    /* 0x14 */ u16          angle;    // from start to end, 0x10000 = a full turn
    /* 0x16 */ s16          tapTimer; // frames left to continue a run of rapid taps
    /* 0x18 */ s16          tapCount; // quick taps of the current run
    /* 0x1A */ u8           reserved1A[2];
    /* 0x1C */ BOOL         watching; // the touch has stayed near where it started
    /* 0x20 */ s16          watchFrames;
    /* 0x22 */ u8           reserved22[2];
    /* 0x24 */ s32          reserved24;
    /* 0x28 */ s16          sweepCenterX; // middle of the last scribble
    /* 0x2A */ s16          sweepCenterY;
    /* 0x2C */ s16          sweepMinX;    // extent of the current scribble
    /* 0x2E */ s16          sweepMaxX;
    /* 0x30 */ s16          sweepMinY;
    /* 0x32 */ s16          sweepMaxY;
    /* 0x34 */ s16          sweepFrames;   // fast-moving frames of the current scribble
    /* 0x36 */ s16          holdFrames;    // how long the stylus has been down
    /* 0x38 */ s16          releaseFrames; // how long the stylus has been up
    /* 0x3A */ s16          x;             // stylus position
    /* 0x3C */ s16          y;
    /* 0x3E */ s16          velocityX;     // stylus movement since the last frame
    /* 0x40 */ s16          velocityY;
    /* 0x42 */ u8           reserved42[2];
    /* 0x44 */ s32          targetKind;  // BattleTouchTargetKind of `target`
    /* 0x48 */ BattleActor* target;      // actor the stylus came down on
    /* 0x4C */ BattleActor* held;        // actor under the stylus while it is down
    /* 0x50 */ BattleActor* released;    // actor under the stylus when it came up
    /* 0x54 */ s16          stillFrames; // frames the stylus has stayed within 4 pixels of the anchor
    /* 0x56 */ s16          anchorX;
    /* 0x58 */ s16          anchorY;
    /* 0x5A */ u8           reserved5A[2];
} BattleTouchWork; // Size: 0x5C

/**
 * @brief Forgets the current gesture and touched actors
 *
 * The gesture flags and stylus position are kept.
 */
void BtlTouch_Clear(void);

/**
 * @brief Checks the gesture recognized this frame
 *
 * @param gesture The BattleTouchGesture to check for
 * @return TRUE if the recognizer is running and recognized `gesture`
 */
BOOL BtlTouch_IsGesture(u16 gesture);

/**
 * @brief Checks what the stylus came down on
 *
 * @param kind The BattleTouchTargetKind to check for
 * @return TRUE if the stylus came down on that kind of actor (or on none, for BTL_TOUCH_TARGET_NONE)
 */
BOOL BtlTouch_IsTargetKind(s32 kind);

/**
 * @brief Checks whether this frame's slash crosses an actor
 *
 * @param actor The actor to test against the stroke
 * @return TRUE if the gesture is BTL_TOUCH_GESTURE_SLASH and its stroke touches the actor's box
 */
BOOL BtlTouch_SlashCrosses(BattleActor* actor);

/**
 * @brief Gets the actor tapped this frame
 *
 * @param kind Which kind of actor to look for; of several enemies or objects, the one whose middle
 *             is closest to the stylus wins
 * @return The tapped actor of `kind`, or NULL if the gesture is not BTL_TOUCH_GESTURE_TAP or none was hit
 */
BattleActor* BtlTouch_GetTapped(BattleActorKind kind);

/**
 * @brief Gets the actor being dragged this frame
 *
 * @param kind Which kind of actor to look for
 * @return The actor under the stylus, or NULL if the gesture is not BTL_TOUCH_GESTURE_DRAG or that
 *         actor is not of `kind`
 */
BattleActor* BtlTouch_GetDragged(BattleActorKind kind);

/**
 * @brief Gets the actor slashed this frame
 *
 * @param kind Which kind of actor to look for; of several enemies or objects, the one whose middle
 *             is closest to the middle of the stroke wins
 * @return The slashed actor of `kind`, or NULL if the gesture is not BTL_TOUCH_GESTURE_SLASH or none was hit
 */
BattleActor* BtlTouch_GetSlashed(BattleActorKind kind);

/**
 * @brief Gets the actor rapidly tapped this frame
 *
 * @param kind Which kind of actor to look for; of several enemies or objects, the one whose middle
 *             is closest to the stylus wins
 * @return The tapped actor of `kind`, or NULL if the gesture is not BTL_TOUCH_GESTURE_RAPID_TAP or
 *         none was hit
 */
BattleActor* BtlTouch_GetRapidTapped(BattleActorKind kind);

/**
 * @brief Sets up the touch recognizer for a battle
 *
 * Allows taps, slashes and drags; BtlBadgeMgr allows rapid taps and scribbles for the pins that use them.
 */
void BtlTouch_Init(void);

/**
 * @brief Reads the stylus and advances the gesture recognizer by one frame
 *
 * Updates the touched actors and sets the gesture BtlTouch_IsGesture reports. Taps and slashes also
 * start their stylus effect tasks.
 */
void BtlTouch_Update(void);

#endif // TWEWY_BATTLE_CORE_BTLTOUCH_H
