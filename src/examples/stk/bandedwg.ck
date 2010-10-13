// STK BandedWG

// patch
BandedWG bwg => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2f( 0, 1 ) => bwg.bowRate;
    Std.rand2f( 0, 1 ) => bwg.bowPressure;
    Std.rand2f( 0, 1 ) => bwg.strikePosition;
    Std.rand2(0, 3) => bwg.preset;

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] => int winner;
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => bwg.freq;

    // print some parameters
    <<< "---", "" >>>;
    <<< "preset:", bwg.preset() >>>;    
    <<< "bow rate:", bwg.bowRate() >>>;
    <<< "bow Pressure:", bwg.bowPressure() >>>;
    <<< "strike position:", bwg.strikePosition() >>>;
    <<< "frequency:", bwg.freq() >>>;
    <<< "---", "" >>>;

    <<< "bow -> pluck", "" >>>;
    Std.rand2f( .5, 1 ) => bwg.pluck;
    // advance time
    2::second => now;

    <<< "pluck -> bow", "" >>>;
    .8 => bwg.startBowing;
    Std.rand2f(2,4)::second => now;
    1.0 => bwg.stopBowing;
    1::second => now;
}
