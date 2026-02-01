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
    Terminal::log("%% %%", names[noteVal], octave);

    // Play the actual note
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(noteVal, octave);
    VBlankIntrDelay(8*noteObj.duration);
}