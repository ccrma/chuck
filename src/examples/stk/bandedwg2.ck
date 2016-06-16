// STK BandedWG

// patch
BandedWG bwg => dac;

// scale
[0, 2, 4, 7, 8, 11] @=> int scale[];

// infinite time loop
while( true )
{
    // ding!
    Math.random2f( 0, 1 ) => bwg.pluck;
    Math.random2f( 0, 128 ) => float bowPressure;
    Math.random2f( 0, 128 ) => float bowMotion;
    Math.random2f( 0, 128 ) => float strikePosition;
    Math.random2f( 0, 128 ) => float vibratoFreq;
    Math.random2f( 0, 128 ) => float gain;
    Math.random2f( 0, 128 ) => float bowVelocity;
    Math.random2f( 0, 128 ) => float setStriking;
    Math.random2f( 0, 3 ) => float preset;

    Math.random2f( 0, 1) => bwg.bowRate;

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
    scale[Math.random2(0,scale.size()-1)] => int winner;
    57 + Math.random2(0,2)*12 + winner => Std.mtof => bwg.freq;
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
    bwg.controlChange(16, Math.random2(0, 3));

    <<< "preset:", bwg.preset() >>>;    
    <<< "strike position:", bwg.strikePosition() >>>;
    <<< "bow rate:", bwg.bowRate() >>>;
    <<< "bow Pressure:", bwg.bowPressure() >>>;

    // set freq
    57 + Math.random2(0,2)*12 + winner => Std.mtof => bwg.freq;
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
