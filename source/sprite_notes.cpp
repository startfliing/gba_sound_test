#include "sprite_notes.hpp"
#include "quarter.h"

// Global array to track active note sprites
static SpriteNoteEntry spriteNotes[MAX_NOTE_SPRITES];

// Initialize the sprite system - sets up OAM and loads sprite tiles/palette
void initNoteSprites()
{
    // Initialize all sprite entries as inactive
    for(int i = 0; i < MAX_NOTE_SPRITES; i++)
    {
        spriteNotes[i].active = 0;
        spriteNotes[i].spriteOamIdx = i;
        spriteNotes[i].noteIdx = -1;
        
        // Hide the sprite in OAM
        oam_mem[i].attr0 = ATTR0_Y(160) | ATTR0_HIDE | ATTR0_TALL;
    }
    
    // Load quarter note tiles into sprite charblock (charblock 4)
    // Sprites use charblocks 4-5, we'll use charblock 4
    memcpy16(tile_mem[4], quarterTiles, quarterTilesLen/2);
    
    // Load quarter note palette into sprite palette memory (bank 0)
    memcpy16(pal_obj_mem, quarterPal, quarterPalLen/2);
}

// Calculate Y position on screen based on note value
// Staff has 5 lines, we need to map notes to positions between/on lines
static int getNoteYPosition(note noteObj)
{
    // This maps note values to Y coordinates
    // Assuming staff lines are at specific Y positions
    // Note value 0-11 represents C-B, octaveVal adds -2 to +4 octaves
    
    // For now, simple mapping: each note gets a Y position relative to octave
    // You may need to adjust these based on your actual staff layout
    int baseY = 80;  // Starting Y position for middle octave
    int noteOffset = noteObj.noteVal * 2;  // 2 pixels per note
    int octaveOffset = noteObj.octaveVal * 24;  // 24 pixels per octave
    
    return baseY + noteOffset + octaveOffset;
}

// Calculate X position based on note index in song
static int getNoteXPosition(int noteIdx)
{
    // Space notes horizontally
    // Each note takes up about 40 pixels
    return 40 + (noteIdx * 40);
}

// Internal helper: Configure OAM entry for sprite display
static void configureNoteOAM(int oamIdx, int screenX, int screenY, int tileIdx)
{
    // Quarter note is 16x32 pixels = OBJ_SIZE_16x32
    // Using charblock 4, palette bank 0
    oam_mem[oamIdx].attr0 = ATTR0_Y(screenY) | ATTR0_REG | ATTR0_TALL;
    oam_mem[oamIdx].attr1 = ATTR1_X(screenX) | ATTR1_SIZE_16x32;
    oam_mem[oamIdx].attr2 = ATTR2_ID(tileIdx) | ATTR2_PALBANK(0) | ATTR2_PRIO(0);
}

int placeNoteSprite(int noteIdx, note noteObj, int screenX, int screenY)
{
    // Find next available sprite slot
    int spriteId = -1;
    for(int i = 0; i < MAX_NOTE_SPRITES; i++)
    {
        if(!spriteNotes[i].active)
        {
            spriteId = i;
            break;
        }
    }
    
    // No available sprites
    if(spriteId == -1)
        return -1;
    
    // Mark as active and store note index
    spriteNotes[spriteId].active = 1;
    spriteNotes[spriteId].noteIdx = noteIdx;
    
    // Quarter note tiles in sprite charblock 4 start at tile 0
    // (we loaded them at the beginning of charblock 4)
    int tileIdx = 1;
    
    // Configure the OAM entry
    configureNoteOAM(spriteNotes[spriteId].spriteOamIdx, screenX, screenY, tileIdx);
    
    return spriteId;
}

void updateNoteSprite(int spriteId, note noteObj, int screenX, int screenY)
{
    if(spriteId < 0 || spriteId >= MAX_NOTE_SPRITES)
        return;
    
    if(!spriteNotes[spriteId].active)
        return;
    
    int oamIdx = spriteNotes[spriteId].spriteOamIdx;
    configureNoteOAM(oamIdx, screenX, screenY, 1);
}

void clearNoteSprite(int spriteId)
{
    if(spriteId < 0 || spriteId >= MAX_NOTE_SPRITES)
        return;
    
    spriteNotes[spriteId].active = 0;
    spriteNotes[spriteId].noteIdx = -1;
    
    // Hide the sprite by moving it off-screen
    int oamIdx = spriteNotes[spriteId].spriteOamIdx;
    oam_mem[oamIdx].attr0 = ATTR0_Y(160) | ATTR0_HIDE | ATTR0_TALL;
}

void clearAllNoteSprites()
{
    for(int i = 0; i < MAX_NOTE_SPRITES; i++)
    {
        clearNoteSprite(i);
    }
}

SpriteNoteEntry* getSpriteEntry(int spriteId)
{
    if(spriteId < 0 || spriteId >= MAX_NOTE_SPRITES)
        return NULL;
    
    return &spriteNotes[spriteId];
}
