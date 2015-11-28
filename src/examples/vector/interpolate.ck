// name: interpolate.ck
// desc: using vec3 as an non-linear, slewing interpolator
//       this is quite useful to making a percentage progress
//       towards a goal, in an amount proportional to the slew
//       and some delta time

// connect
SinOsc foo => dac;

// the interpolator
vec3 interpolator;
// the interpolation rate
10::ms => dur irate;
// set initial value, goal, slew
interpolator.set( 440, 440, .05 * (second/irate) );

// spork interpolator
spork ~ interpolate( irate );

// main shred sets the goal
while( true )
{
    // set goal
    interpolator.update( Math.random2f(200,1000) );
    // every so often
    500::ms => now;
}

// function to drive interpolation concurrently
fun void interpolate( dur delta )
{
    while( true )
    {
        // interpolate
        interpolator.interp( delta );
        // get value
        interpolator.value => foo.freq;
        // advance time by rate
        irate => now;
    }
}