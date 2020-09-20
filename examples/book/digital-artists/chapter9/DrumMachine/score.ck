// Listing 9.16 score.ck controls individual players and tempo via BPM
// score.ck, on the fly drumming with global BPM conducting
// Here we make our BPM instance that controls tempo
BPM tempo;              // (1) Makes a global BPM to control from here.
tempo.tempo(120.0);

// Add in instruments one at a time, musically
Machine.add(me.dir()+"/kick.ck") => int kickID;  // (2) Adds instruments one at a time.

8.0 * tempo.quarterNote => now;

Machine.add(me.dir()+"/snare.ck") => int snareID;

8.0 * tempo.quarterNote => now;

Machine.add(me.dir()+"/hihat.ck") => int hatID;
Machine.add(me.dir()+"/cowbell.ck") => int cowID;

8.0 * tempo.quarterNote => now;

Machine.add(me.dir()+"/clap.ck") => int clapID;

8.0 * tempo.quarterNote => now;

// now have some fun with tempo
<<< "Set tempo to 80BPM" >>>;
80.0 => float newtempo;      // (3) Changes tempo to 80 BPM...

tempo.tempo(newtempo);
8.0 * tempo.quarterNote => now;

<<< "Now set tempo to 160BPM" >>>;
160.0 => newtempo;           // (4) then changes it to 160 BPM.
tempo.tempo(newtempo);
8.0 * tempo.quarterNote => now;

/* if you want to run OTFBPM.ck to change tempo
as these run, then comment out the lines below */

<<< "Gradually decrease tempo" >>>;
while (newtempo > 60.0) {    // (5) Gradually moves tempo down.

    newtempo - 20 => newtempo;
    tempo.tempo(newtempo);
    <<< "tempo = ", newtempo >>>;
    4.0 * tempo.quarterNote => now;
}

// bring out the instruments, gradually
Machine.remove(kickID);     // (6) Pulls out instruments one at a time

8.0 * tempo.quarterNote => now;
Machine.remove(snareID);
Machine.remove(hatID);

8.0 * tempo.quarterNote => now;
Machine.remove(cowID);

8.0 * tempo.quarterNote => now;
Machine.remove(clapID);
