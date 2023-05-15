//------------------------------------------------------------------------------
// name: pulse.ck
// desc: basic demo using PulseOsc UGen to do pulse width modulation
//
// This UGen defines an oscillator that uses Pulse Width Modulation (PWM), a
// process that allows for the control of the width of output pulses while
// keeping the frequency constant. The on/off ratio of the signal is called the
// "duty cycle". A pulse wave may be thought of as a square wave with adjustable
// time between cycles. A square wave has a duty cycle of 50% -- alternating
// between "on" and "off" in equal proportions. A duty cycle of 75% would mean
// the pulse signal is "on" three times as long as it is "off" in one cycle.
// The PulseOsc UGen can be used to create sound, or to control other signals!
//
// author: Alex Han
// date: Spring 2023
//------------------------------------------------------------------------------

// connect
PulseOsc pulse => dac;
// set gain
pulse.gain( .2 );
// set frequency
pulse.freq( 110 );

// The key characteristic of PulseOsc is the ability to set the pulse width
// (i.e. duty cycle). When the pulse wave is used to create sound, varying
// the duty cycle affects the timbre. This demo shows how the timbre changes
// significantly with small changes to the pulse width.

// time loop
while( true )
{
    for( int i; i < 20; i++ )
    {
        // .width() takes a float from 0.0 to 1.0
        pulse.width( .05 * i );
        // print
        chout <= "current duty cycle: " <= pulse.width();
        // floating point equality
        if( Math.equal( pulse.width(), .5) )
        {chout <= " <- this is just a square wave!";}
        // newline
        chout <= IO.newline();
        // next time step
        .5::second => now;
    }
}

// for more PWM fun...
// 1) smoothly vary the pulse width (using a small time scale)
// 2) filter the sound (e.g., with LPF)
