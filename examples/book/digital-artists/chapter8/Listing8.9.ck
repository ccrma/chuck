// Listing 8.9 Jazz band:
//   save as "score.ck"

// start piano         // (1) Starts off with piano chords...
Machine.add(me.dir() + "/piano.ck") => int pianoID;
4.8::second => now;

// start drums         // (2) ...then adds hi-hat percussion...
me.dir() + "/drums.ck" => string drumsPath;
Machine.add(drumsPath ) => int drumsID;
4.8::second => now;

// start bass          // (3) ...then adds in the walking bass line...
Machine.add(me.dir() + "/bass.ck") => int bassID;
4.8::second => now;

// start flute solo    // (4) ...then adds the flute solo. 
Machine.add(me.dir() + "/flute.ck") => int fluteID;
4.8::second => now;

// remove drums
Machine.remove(drumsID); // (5) Cuts the percussion for a bit...
4.8::second => now;

// add drums back in
Machine.add(drumsPath) => drumsID;   // (6) ...then adds it back in
