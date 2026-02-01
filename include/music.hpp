#ifndef __MUSIC__
#define __MUSIC__

#include "tonc.h"

struct note{
    u8 noteVal; //formatted as 4 bits octave | 4 bits note
    s8 octaveVal;
    u8 duration;
    u8 pad;
};

note createNote(u8 noteVal, s8 octave, u8 duration);
void note_play(note noteObj);
void incNoteVal(note* currNote);
void decNoteVal(note* currNote);

void playCurrSong(note* songStart, int noteCt);


#endif