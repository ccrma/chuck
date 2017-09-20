// Listing 7.18 Ragajam Indian music in ChucK

Sitar sitar => Gain master => dac;   // (1) Sitar instrument, master mixer Gain
sitar => Delay delay => Gain sitarFeedback => delay; // (2) Delay line to give Sitar an echo 
delay => master;       // (3) Connects delay line to output mixer

// drone "singers" to accompany
VoicForm singerDrones[4]; // (4) Uses four "singers" as low drones.
NRev droneRev => master;  // (5) Drones get their own reverb...
for( 0 => int i; i < singerDrones.cap(); i++)
{
    singerDrones[i] => droneRev;      // (6) ...connect the singers...
    (.5/singerDrones.cap()) => singerDrones[i].gain; // (7) ...and set their gains appropriately.
}

// singer parameters (drone)
[26,38,45,50] @=> int drones[]; // (8) Pitches for low drone singers.
.5 => droneRev.mix;
for( 0 => int i; i < singerDrones.cap(); i++)
{
    .02 => singerDrones[i].vibratoGain;          // (9) Gives droners a little vibrato.
    "lll" => singerDrones[i].phoneme;            // (10) Sets the "word" they're singing.
    Std.mtof(drones[i]) => singerDrones[i].freq; // (11) Sets their pitches
}

// Shaker percussion, and control function
Shakers shaker => master; // (12) Shaker percussionist, connects to output mixer

fun void shake()
{                   // (13) Function to cause the shaker to play a "note" 
    // drum control
    Math.random2(1,2) => shaker.preset;
    Math.random2f(60.0,128.0) => shaker.objects;
    Math.random2f(.8,1.0) => shaker.decay;
    shaker.noteOn(Math.random2f(0.0, 4.0));
}

// global raga scale
[62,63,65,67,69,70,72,74] @=> int raga[];

// global timing/tempo parameter
.2 => float beattime;

// sitar echo parameters
beattime::second => delay.max;
beattime::second => delay.delay;
.5 => sitarFeedback.gain;

// master volume parameter
.7 => master.gain;

// MAIN PROGRAM, ragajam.ck // warm up with a drone solo 
5::second => now; // // (14) Drones begin the piece

// main loop, add Sitar and Shaker 
while (true) {
    Math.random2(1,3) => float factor; // (15) Random timing for each new "measure"

    // loop
    for( 0 => int i; i < raga.cap(); i++ )
    {
        // sitar control, // (17) Random note from raga array
        Std.mtof(raga[Math.random2(0,raga.cap()-1)]) => sitar.freq;
        sitar.noteOn(Math.randomf()); // (16) Sitar player

        // play a percussion sound
        shake();                  // (18) Tells the shaker to play on beat... 

        // advance time by 1/2 our basic beat
        (0.5*beattime)::second*factor => now;

        // an occasional half-time percussion hit for flavor
        if (Math.random2(1,3) == 1)   // (19) ...and lets him play on off-beat every so often.
        {
            shake();
        }

        // advance time by the other 1/2 beat
        (0.5*beattime)::second*factor => now;
    }
}
