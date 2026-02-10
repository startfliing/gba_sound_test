#include "tonc.h"

#include "terminal.hpp"
#include "image.h"
#include "staff.h"
#include "soundbank_bin.h"
#include "music.hpp"

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

int main()
{
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

	REG_BG0CNT = BG_BUILD(0,17,1,0,0,0,0); //size 1 for wide bg
    //REG_BG0CNT = Terminal::setCNT(0,1,16);

    memcpy16(pal_bg_mem, staffPal, staffPalLen/2);
    memcpy16(tile_mem[0], staffTiles, staffTilesLen/2);
    memcpy16(&se_mem[17], staffMap, staffMapLen/2);


    note currSong[24];
    int currNoteInd = 0;
    note currNote = {0,0,4};

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

        // Play note

        if(key_hit(KEY_UP) || key_hit(KEY_DOWN)){
            key_tri_vert() == -1 ? incNoteVal(&currNote) : decNoteVal(&currNote);
            //Terminal::eraseLine();
            note_play(currNote);
        }

        if(key_hit(KEY_B) && currNoteInd != 0){
            currNoteInd--;
            currNote = currSong[currNoteInd];
            //Terminal::eraseLine();
        }

        if(key_hit(KEY_A) && currNoteInd != 23){
            currSong[currNoteInd] = currNote;
            currNoteInd++;
            note_play(currNote);
        }

		if(key_hit(KEY_START)){
            //Terminal::reset();
			playCurrSong(currSong, currNoteInd);
            note_play(currNote);
		}

        if(key_hit(KEY_SELECT)){
            //Terminal::reset();
			currNoteInd = 0;
            note_play(currNote);
		}
    
    }
    return 0;
}
