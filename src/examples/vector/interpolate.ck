// name: interpolate.ck
// desc: using vec3 as an non-linear, slewing interpolator
//       this is quite useful to making a percentage progress
//       towards a goal, in an amount proportional to the slew
//       and some delta time

// connect
SinOsc foo => dac;

// the interpolator
vec3 i;
// the interpolation rate
10::ms => dur irate;
// set initial .value, .goal, .slew
i.set( 440, 440, .05 * (second/irate) );

// spork interpolate function
spork ~ interpolate( irate );

// main shred sets the goal
while( true )
{
    // set interpolator goal
    i.update( Math.random2f(200,1000) );
    // every so often
    500::ms => now;
}

// function to drive interpolator(s) concurrently
fun void interpolate( dur delta )
{
    while( true )
    {
        // interpolate (can also get new value via .value)
        i.interp( delta ) => foo.freq;
        // advance time by rate
        delta => now;
    }
}
