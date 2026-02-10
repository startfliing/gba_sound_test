#include "music.hpp"
#include "terminal.hpp"

const char *names[]=
{   "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "  };


note createNote(u8 noteVal, s8 octave, u8 duration){
    return {
        noteVal,
        octave,
        duration,
        0
    };
}

u8 getNoteVal(note noteObj){
    return noteObj.noteVal;
}

s8 getNoteOct(note noteObj){
    return noteObj.octaveVal;
}

void note_play(note noteObj)
{
    // Display note and scroll
    u8 noteVal = getNoteVal(noteObj);
    s8 octave = getNoteOct(noteObj);
    //Terminal::log("%% %%", names[noteVal], octave);

    // Play the actual note
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(noteVal, octave);
    VBlankIntrDelay(8*noteObj.duration);
}

void incNoteVal(note* currNote){
    if(currNote->noteVal == NOTE_B && currNote->octaveVal == 3) return;

    currNote->noteVal = wrap(currNote->noteVal + 1, NOTE_C, NOTE_B+1);
    if(currNote->noteVal == NOTE_C){
        currNote->octaveVal = (s8)clamp(currNote->octaveVal + 1, -2, 4);
    }
}

void decNoteVal(note* currNote){
    if(currNote->noteVal == NOTE_C && currNote->octaveVal == -2) return;

    currNote->noteVal = wrap(currNote->noteVal - 1, NOTE_C, NOTE_B+1);
    if(currNote->noteVal == NOTE_B){
        currNote->octaveVal = (s8)clamp(currNote->octaveVal - 1, -2, 4);
    }
}

void playCurrSong(note* songStart, int noteCt){
    for(int i = 0; i < noteCt; i++){
        note_play(songStart[i]);
    }
}
