// STK BandedWG

// patch
BandedWG bwg => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Std.rand2f( 0, 1 ) => bwg.pluck;
    Std.rand2f( 0, 128 ) => float bowPressure;
    Std.rand2f( 0, 128 ) => float bowMotion;
    Std.rand2f( 0, 128 ) => float strikePosition;
    Std.rand2f( 0, 128 ) => float vibratoFreq;
    Std.rand2f( 0, 128 ) => float gain;
    Std.rand2f( 0, 128 ) => float bowVelocity;
    Std.rand2f( 0, 128 ) => float setStriking;
    Std.rand2f( 0, 3 ) => float preset;

    Std.rand2f( 0, 1) => bwg.bowRate;

    bwg.controlChange( 2, bowPressure);
    bwg.controlChange( 4, bowMotion);
    bwg.controlChange( 8, strikePosition);
    bwg.controlChange( 11, vibratoFreq);
    bwg.controlChange( 1, gain);
    bwg.controlChange( 128, bowVelocity);
    bwg.controlChange( 64, setStriking);
	
    <<< "---", "" >>>;
    <<< "strike position:", bwg.strikePosition() >>>;
    <<< "bow rate:", bwg.bowRate() >>>;
    <<< "bow Pressure:", bwg.bowPressure() >>>;

    // set freq
    scale[Std.rand2(0,scale.cap()-1)] => int winner;
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => bwg.freq;
    // go
    .8 => bwg.noteOn;

    // advance time
    1::second => now;
    1.0 => bwg.noteOff;
    .5::second => now;

    <<< "pluck -> bow", "" >>>;

    .8 => bwg.startBowing;
    1::second => now;
    1.0 => bwg.stopBowing;

    <<< "--", "" >>>;
    bwg.controlChange(16, Std.rand2(0, 3));

    <<< "preset:", bwg.preset() >>>;    
    <<< "strike position:", bwg.strikePosition() >>>;
    <<< "bow rate:", bwg.bowRate() >>>;
    <<< "bow Pressure:", bwg.bowPressure() >>>;

    // set freq
    57 + Std.rand2(0,2)*12 + winner => Std.mtof => bwg.freq;
    // go
    .8 => bwg.noteOn;

    // advance time
    1::second => now;
    1.0 => bwg.noteOff;
    .5::second => now;

    <<< "pluck -> bow", "" >>>;

    .8 => bwg.startBowing;
    1::second => now;
    1.0 => bwg.stopBowing;
}
