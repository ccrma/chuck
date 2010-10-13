// patch
BlowHole hole => dac;

// scale
[0, 2, 4, 7, 9, 11] @=> int scale[];

// infinite time loop
while( true )
{
  // change parameters
  if( Std.rand2f(0,1) > .75 )
  {
    Std.rand2f( 0, 128 ) => float stiffness;
    Std.rand2f( 0, 128 ) => float noisegain;
    Std.rand2f( 0, 128 ) => float tonehole;
    Std.rand2f( 0, 128 ) => float register;
    Std.rand2f( 0, 128 ) => float pressure;

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
  scale[Std.rand2(0,scale.cap()-1)] => int note;
  33 + Std.rand2(0,4)*12 + note => Std.mtof => hole.freq;
  <<< "note: ", Std.ftom( hole.freq() ) >>>;

  // go
  .8 => hole.noteOn;

  // advance time
  1::second => now;
}
