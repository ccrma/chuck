// name: dcblocker.ck
// desc: using a PoleZero filter for a DC blocker, which can
//       remove/attenuate extremely low frequencies
//
// note: this example makes no sound; it only prints

// connect patch (no sound)
SinOsc foo => PoleZero blocker => blackhole;

// set low frequency
.1 => foo.freq;

// set DC blocker parameter; controls sharpness of the cutoff;
// closer to 1 means narrower rejection band around DC
.95 => blocker.blockZero;

// time loop
while( true )
{
    // print before and after DC blocker
    cherr <= "before DC blocker: " <= foo.last()
          <= " after DC blocker: " <= blocker.last()
          <= IO.newline();
    // advance time
    100::ms => now;
}
