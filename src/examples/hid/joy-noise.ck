// name: joynoise.ck
// desc: using joystick to control noise (first 3 axes + buttons)
// author: Ge Wang

// make our Hid objects
Hid hi;
HidMsg msg;

// which joystick
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open joystick
if( !hi.openJoystick( device ) ) me.exit();
<<< "joystick '" + hi.name() + "' ready...", "" >>>;

// patch
Noise n => BiQuad f => Envelope e => Pan2 p => dac;
e => Echo echo => p;
e => Echo echo2 => p;
e => Echo echo3 => p;

// set biquad pole radius
.99 => f.prad;
// set biquad gain
.05 => f.gain;
// set equal zeros
1 => f.eqzs;
// our float
0.0 => float t;
// set echo
echo.delay() * 2 => echo2.max => echo2.delay;
echo.delay() * 3 => echo3.max => echo3.delay;
echo.gain( .75 );
echo.gain() * .25 => echo2.gain;
echo2.gain() * .25 => echo3.gain;
// set gain
.5 => n.gain;
// attack
10::ms => e.duration;

// variables
0 => float value;
1 => float factor;
0 => float q;
0 => int count;

// initialize
set( value, factor, q );

// infinite time loop
while( true )
{
    // wait on event
    hi => now;
    // recv message
    while( hi.recv( msg ) )
    {
        // axis 0: map to pan
        if( msg.isAxisMotion() && msg.which == 0 )
        { msg.axisPosition => p.pan; }

        // axis 1: map to pfreq
        if( msg.isAxisMotion() && msg.which == 1 )
        { -msg.axisPosition => value; set( value, factor, q ); }

        // axis 2: map to Q
        if( msg.isAxisMotion() && msg.which == 2 )
        { msg.axisPosition => q; set( value, factor, q ); }

        // button down (any)
        if( msg.isButtonDown()  )
        { count++; if( count ) e.keyOn(); msg.which + 1 => factor; set( value, factor, q ); }

        // button up (any)
        if( msg.isButtonUp() )
        { count--; if( !count ) e.keyOff(); }
    }
}

// do actual mapping control
fun void set( float value, float factor, float q )
{
    (value * 440 + 480) * factor => f.pfreq;

    // hacked q mapping
    if( q >= 0 )
    { .99+.009*(Math.pow(2,q)-1) => f.prad; .25+.5*(1-q) => e.gain; }
    else
    { .99-.09*(Math.pow(2,-q)-1) => f.prad; 1 => e.gain; }

    if( count )
        <<< "pan:", p.pan(), "resonance:", f.pfreq(), "radius:", f.prad() >>>;
}
