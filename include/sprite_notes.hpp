#ifndef __SPRITE_NOTES__
#define __SPRITE_NOTES__

#include "tonc.h"
#include "music.hpp"

// Maximum number of note sprites on screen at once
#define MAX_NOTE_SPRITES 128

// Sprite metadata for tracking note sprites
typedef struct {
    int active;          // 0 = inactive/free, 1 = active
    int spriteOamIdx;    // Index into OAM attribute memory
    int noteIdx;         // Index in the song (which note in currSong[])
} SpriteNoteEntry;

// Initialize sprite system - call once at startup
void initNoteSprites();

// Allocate and place a note sprite on screen
// Returns sprite ID (index) or -1 if no sprites available
int placeNoteSprite(int noteIdx, note noteObj, int screenX, int screenY);

// Update an existing note sprite's position and appearance
void updateNoteSprite(int spriteId, note noteObj, int screenX, int screenY);

// Hide/deactivate a sprite
void clearNoteSprite(int spriteId);

// Clear all note sprites
void clearAllNoteSprites();

// Get sprite entry for direct manipulation if needed
SpriteNoteEntry* getSpriteEntry(int spriteId);

#endif // __SPRITE_NOTES__
