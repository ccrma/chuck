//-----------------------------------------------------------------------------
// name: cage.ck
// desc: 4'33" by John Cage
//
// authors: Primrose Ohling
//          Nick Shaheed
// date: Fall 2025
//
// note: alternative implementation:
//
//   --------------------------------
//    4::minute + 33::second => now;
//   --------------------------------
//
// At this point, a philosophical question arises as to which 
// implementation more authentically embodies the spirit of 4'33".
// The one-line alternate version (above) could be considered the most
// minimal version possible -- perhaps the Platonic ideal of 4'33" as
// expressed in the strongly-timed syntax and semantics of the ChucK
// programming language, in sofar as the above program *is* the passage
// of a specific duration of time "in silence"; the absence of unit
// generators implies a zero-amplitude audio output. The computer musician
// writing/running this code draws parallels to the pianist sitting in
// front of a piano, which is not only capable of making sound but also
// carries strong culture expections to make sound, and yet the pianist
// chooses, in this case, to proactively NOT make sound. This implementation,
// therefore, could be an embodiment of 4'33" from the point of view of the 
// performer(s) onstage.
//
// On the other hand, the implementation below, which contains not only
// the alternate version in its entirety, but also goes to lengths
// in order to connect the live audio input (adc) to the audio output (dac),
// foregrounding not the sound made by the software program, but rather
// the incidental sounds of the room. This is fitting of 4'33" in a
// different sense. In Cage's original work, the incidential sounds of
// the room that accompanies the absence of instrumental sounds onstage,
// is intentionally a vital part of the piece itself, inviting audiences to
// contemplate the nature of musical performances as well as what may
// constitute a performance in the first place. Under this interpretation,
// the implementation below nods to the acoustic, social, and cultural
// contexts in which the piece is performed.
//
//                      -- Nick Shaheed & Ge Wang, Fall 2025
//
//-----------------------------------------------------------------------------
// (note from Primrose) please reduce buffer size as much as possible
//     terminal> chuck --bufsize:16 cage.ck
//-----------------------------------------------------------------------------

// connect audio input (through unity gain) to output
adc => Gain unity => dac;

// advance time by 4'33"
4::minute + 33::second => now;

// disconnect unit generators
adc =< unity =< dac;
