// STK BandedWG

// patch
BandedWG bwg => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Math.random2f( 0, 1 ) => bwg.bowRate;
    Math.random2f( 0, 1 ) => bwg.bowPressure;
    Math.random2f( 0, 1 ) => bwg.strikePosition;
    Math.random2( 0, 3 ) => bwg.preset;

    // set freq
    scale[Math.random2(0,scale.size()-1)] => int winner;
    57 + Math.random2(0,2)*12 + winner => Std.mtof => bwg.freq;

    // print some parameters
    <<< "---", "" >>>;
    <<< "preset:", bwg.preset() >>>;    
    <<< "bow rate:", bwg.bowRate() >>>;
    <<< "bow Pressure:", bwg.bowPressure() >>>;
    <<< "strike position:", bwg.strikePosition() >>>;
    <<< "frequency:", bwg.freq() >>>;
    <<< "---", "" >>>;

    <<< "bow -> pluck", "" >>>;
    Math.random2f( .5, 1 ) => bwg.pluck;
    // advance time
    2::second => now;

    <<< "pluck -> bow", "" >>>;
    .8 => bwg.startBowing;
    Math.random2f(2,4)::second => now;
    1.0 => bwg.stopBowing;
    1::second => now;
}
