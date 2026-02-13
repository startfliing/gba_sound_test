#include "tonc.h"

#include "terminal.hpp"
#include "image.h"
#include "staff.h"
#include "soundbank_bin.h"
#include "music.hpp"
#include "quarter.h"
#include "sprite_notes.hpp"

// === FUNCTIONS ======================================================


//note, octave, length
note Song[] = {
    {2, 0, 2},
    {4, 0, 2},
    {5, 0, 2},
    {7, 0, 4},
    {4, 0, 4},
    {0, 0, 2},
    {2, 0, 4}
};

// Play a little ditty
void theLick()
{

    for(note noteObj : Song)
    {
        note_play(noteObj);
    }
}

int size1Ind(int x, int y){
    return (y*32)+x;
}

// Draw the currently editing note as a sprite
// Draw the note sprite at the correct position based on scrolling
// Calculate Y position based on note and octave
int getNoteYPosition(note noteObj) {
    // G octave -2 is y=100
    // Each note step (excluding accidentals) decreases y by 4
    // Assume noteObj.note: 0=C, 1=D, 2=E, 3=F, 4=G, 5=A, 6=B, 7=accidental
    // octave: -2, -1, 0, 1, 2, etc.
    // G octave -2: note=4, octave=-2
    int baseY = 100;
    int baseNote = 4; // G
    int baseOctave = -2;
    int noteSteps = (noteObj.octaveVal - baseOctave) * 7 + (noteObj.noteVal - baseNote);
    // Ignore accidentals (if noteObj.note == 7)
    if(noteObj.noteVal == 7) noteSteps = 0;
    return baseY - (noteSteps * 4);
}

void drawNote(int spriteId, note noteObj, int noteInd, int scrollOffset){
    // Calculate X position: equidistant for quarter notes
    const int NOTE_SPACING = 16; // pixels between notes
    const int CENTER_X = 80;     // Center of screen
    int screenX = CENTER_X + (noteInd * NOTE_SPACING) - scrollOffset;
    int screenY = getNoteYPosition(noteObj);
    if(spriteId >= 0)
    {
        updateNoteSprite(spriteId, noteObj, screenX, screenY);
    }
}

int main()
{
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

	REG_BG0CNT = BG_BUILD(0,17,1,0,1,0,0); //size 1 for wide bg
    REG_BG1CNT = BG_BUILD(0,19,1,0,0,0,0);
    //REG_BG0CNT = Terminal::setCNT(0,1,16);

    memcpy16(pal_bg_mem, staffPal, staffPalLen/2);
    memcpy16(tile_mem[0], staffTiles, staffTilesLen/2);
    memcpy16(&se_mem[17], staffMap, staffMapLen/2);

    // Initialize sprite system for note display
    oam_init(oam_mem, 128);
    initNoteSprites();

    // Support more notes for infinite scrolling
    const int MAX_NOTES = 128;
    note currSong[MAX_NOTES];
    int spriteIds[MAX_NOTES];  // Track sprite ID for each note in song
    for(int i = 0; i < MAX_NOTES; i++)
        spriteIds[i] = -1;

    int currNoteInd = 0;
    note currNote = {0,0,4};
    int scrollOffset = 0;

    // Place initial sprite for editing
    int currSpriteId = placeNoteSprite(0, currNote, 80, 100);
    drawNote(currSpriteId, currNote, 0, scrollOffset);

    // turn sound on
    REG_SNDSTAT= SSTAT_ENABLE;
    // snd1 on left/right ; both full volume
    REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 7);
    // DMG ratio to 100%
    REG_SNDDSCNT= SDS_DMG100;

    // no sweep
    REG_SND1SWEEP= SSW_OFF;
    // envelope: vol=12, decay, max step time (7) ; 50% duty
    REG_SND1CNT= SSQR_ENV_BUILD(8, 0, 7) | SSQR_DUTY1_2;
    REG_SND1FREQ= 0;

    note_play(currNote);

    while(1)
    {
        VBlankIntrWait();
        key_poll();

        // Calculate scroll offset to keep editing note centered
        const int NOTE_SPACING = 16;
        const int CENTER_X = 80;
        scrollOffset = currNoteInd * NOTE_SPACING;
        REG_BG0HOFS = scrollOffset;

        // Only draw notes within visible screen window, hide others
        const int SCREEN_LEFT = 0;
        const int SCREEN_RIGHT = 240;
        for(int i = 0; i < currNoteInd; i++) {
            if(spriteIds[i] >= 0) {
                int noteX = 80 + (i * NOTE_SPACING) - scrollOffset;
                if(noteX >= SCREEN_LEFT && noteX < SCREEN_RIGHT) {
                    drawNote(spriteIds[i], currSong[i], i, scrollOffset);
                } else {
                    // Move sprite off-screen to hide
                    updateNoteSprite(spriteIds[i], currSong[i], 240, 100);
                }
            }
        }
        // Draw editing note (always visible)
        drawNote(currSpriteId, currNote, currNoteInd, scrollOffset);

        // Play note
        if(key_hit(KEY_UP) || key_hit(KEY_DOWN)){
            key_tri_vert() == -1 ? incNoteVal(&currNote) : decNoteVal(&currNote);
            note_play(currNote);
        }

        if(key_hit(KEY_B) && currNoteInd != 0){
            // Remove sprite for previous note
            clearNoteSprite(spriteIds[currNoteInd]);
            spriteIds[currNoteInd] = -1;

            currNoteInd--;
            currNote = currSong[currNoteInd];
            currSpriteId = spriteIds[currNoteInd];
        }

        if(key_hit(KEY_A) && currNoteInd < MAX_NOTES-1){
            currSong[currNoteInd] = currNote;
            spriteIds[currNoteInd] = currSpriteId;

            currNoteInd++;
            currNote = currSong[currNoteInd];

            // Allocate new sprite for next note
            int noteX = CENTER_X + (currNoteInd * NOTE_SPACING) - scrollOffset;
            currSpriteId = placeNoteSprite(currNoteInd, currNote, noteX, 100);
            note_play(currNote);
        }

        if(key_hit(KEY_START)){
            playCurrSong(currSong, currNoteInd);
            note_play(currNote);
        }

        if(key_hit(KEY_SELECT)){
            currNoteInd = 0;
            currNote = currSong[0];
            currSpriteId = spriteIds[0];
            clearAllNoteSprites();
            note_play(currNote);
        }
    }
    return 0;
}
