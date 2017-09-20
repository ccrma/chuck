// patch
BlowHole hole => dac;

// scale
[0, 2, 4, 7, 9, 11] @=> int scale[];

<<< "reed stiffness:", hole.reed() >>>;
<<< "noise gain:", hole.noiseGain() >>>;
<<< "tonehole state:", hole.tonehole() >>>;
<<< "register state:", hole.vent() >>>;
<<< "breath pressure:", hole.pressure() >>>;

// infinite time loop
while( true )
{
  // change parameters
  if( Math.random2f(0,1) > .75 )
  {
    Math.random2f( 0, 1 ) => hole.reed;
    Math.random2f( 0, 1 ) => hole.noiseGain;
    Math.random2f( 0, 1 ) => hole.tonehole;
    Math.random2f( 0, 1 ) => hole.vent;
    Math.random2f( 0, 1 ) => hole.pressure;

    // print
    <<< "going...", "" >>>;
    <<< "reed stiffness:", hole.reed() >>>;
    <<< "noise gain:", hole.noiseGain() >>>;
    <<< "tonehole state:", hole.tonehole() >>>;
    <<< "register state:", hole.vent() >>>;
    <<< "breath pressure:", hole.pressure() >>>;
  }

  // set freq
  scale[Math.random2(0,scale.size()-1)] => int note;
  33 + Math.random2(0,4)*12 + note => Std.mtof => hole.freq;
  <<< "note: ", Std.ftom( hole.freq() ) >>>;

  // go
  .8 => hole.noteOn;

  // advance time
  1::second => now;
}
