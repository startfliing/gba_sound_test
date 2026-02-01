#include "tonc.h"

#include "terminal.hpp"
#include "image.h"
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
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

	REG_BG1CNT = BG_BUILD(0,17,0,0,0,0,0);
    REG_BG0CNT = Terminal::setCNT(0,1,16);


    memcpy16(pal_bg_mem, imagePal, imagePalLen/2);
    LZ77UnCompVram(imageTiles, tile_mem[0]);
    memcpy16(&se_mem[17], imageMap, imageMapLen/2);

    int currNote = 0;
    int currOctave = 0;

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

    theLick();

    while(1)
    {
        VBlankIntrWait();
        key_poll();

        // Change octave:
        currOctave += bit_tribool(key_hit(-1), KI_R, KI_L);
        currOctave = wrap(currOctave, -2, 6);

        // Play note
        if(key_hit(KEY_DIR|KEY_A))
        {
            if(key_hit(KEY_UP))
                note_play(createNote(0, currOctave, 2));
            if(key_hit(KEY_LEFT))
                note_play(createNote(2, currOctave, 2));
            if(key_hit(KEY_RIGHT))
                note_play(createNote(4, currOctave, 2));
            if(key_hit(KEY_DOWN))
                note_play(createNote(5, currOctave, 2));
            if(key_hit(KEY_A))
                note_play(createNote(7, currOctave, 2));
        }

		if(key_hit(KEY_START)){
			Terminal::reset();
		}

        // Play ditty
        if(key_hit(KEY_B))
            theLick();      
    }
    return 0;
}
