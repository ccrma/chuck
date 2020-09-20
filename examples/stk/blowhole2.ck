// patch
BlowHole hole => dac;

// scale
[0, 2, 4, 7, 9, 11] @=> int scale[];

// infinite time loop
while( true )
{
  // change parameters
  if( Math.random2f(0,1) > .75 )
  {
    Math.random2f( 0, 128 ) => float stiffness;
    Math.random2f( 0, 128 ) => float noisegain;
    Math.random2f( 0, 128 ) => float tonehole;
    Math.random2f( 0, 128 ) => float register;
    Math.random2f( 0, 128 ) => float pressure;

    <<< "going...", "" >>>;
    <<< "reed stiffness:", stiffness, "/ 128.0" >>>;
    <<< "noise gain:", noisegain, "/ 128.0" >>>;
    <<< "tonehole state:", tonehole, "/ 128.0" >>>;
    <<< "register state:", register, "/ 128.0" >>>;
    <<< "breath pressue:", pressure, "/ 128.0" >>>;

    // reed stiffness
    hole.controlChange( 2, stiffness );
    // noise gain
    hole.controlChange( 4, noisegain );
    // tonehole state
    hole.controlChange( 11, tonehole );
    // register state
    hole.controlChange( 1, register );
    // breath pressue
    hole.controlChange( 128, pressure );
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
