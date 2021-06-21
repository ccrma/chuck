// name: krstlchr.ck
// desc: KrystlChr (Krystal Choir) 4-Op FM Demo (TX81Z Algorithm 7)
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above

// array of krystal choir instruments
KrstlChr c[7];
// to learn more about FrencHrn, uncomment this:
// KrstlChr.apropos();

Pan2 p[7];
NRev r[2] => dac;
// connect up
for (int i; i < 7; i++) {
   c[i] => p[i] => r;
   -1.0 + 0.333*i => p[i].pan;
   Math.random2f(2.5,4.0) => c[i].lfoSpeed; // different modulations
}

// turn down the volum... for safety
0.12 => r[0].gain => r[1].gain; 

[48, 55, 60, 65, 72, 79, 81] @=> int chord1[];
[46, 56, 63, 67, 70, 82, 89] @=> int chord2[];

<<< "Play some nice open chords...", "" >>>;
rollChord(chord1); second => now;
allOff(); second => now;
rollChord(chord2); second => now;
allOff(); second => now;
Std.mtof(50) => c[3].freq;
1 => c[3].noteOn; second => now;
    
<<< "Then loop on random open circle of 4ths notes...", "" >>>;
while( true )
{
    Math.random2(0,6) => int which;
    c[which].noteOff;
    Math.random2(0,6) => which;
    Math.random2(0,10)*5 => int space;
    Std.mtof(38+space) => c[which].freq;
    if (maybe) 
        1 => c[which].noteOff;
    else
        1 => c[which].noteOn;
    second => now;
}

fun void rollChord( int chord[] )
{
    for( int i; i < 7; i++ )
    {
        Std.mtof(chord[i]) => c[i].freq;
        1 => c[i].noteOn;
        Math.random2f(0.1,0.3)::second => now;
    }
}

fun void allOff()
{
    for (int i; i < 7; i++) 1 => c[i].noteOff;
}
