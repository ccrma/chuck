//------------------------------------------------------------------------------
// Pulse Width Modulation
// name: pulse.ck
// desc: Basic demo of the PulseOsc UGen
//
// details:
// This UGen defines an oscillator that uses Pulse Width Modulation (PWM), a
// process that allows for the control of the width of output pulses while 
// keeping the frequency constant. The on/off ratio of the signal is called the 
// "duty cycle". A pulse wave may be thought of as a square wave with adjustable 
// time between cycles. A square wave has a duty cycle of 50% -- alternating 
// between "on" and "off" in equal proportions. A duty cycle of 75% would mean 
// the pulse signal is "on" three times as long as it is "off" in one cycle.
// The PulseOsc UGen can be used to create sound, or to control other signals!
//
// Alex Han, Spring 2023
//------------------------------------------------------------------------------

// Setting up
PulseOsc pulse => dac;

pulse.gain(.2);
pulse.freq(440);

// The key characteristic of PulseOsc is the ability to set the pulse width (i.e. duty cycle)
// When the pulse wave is used to create sound, varying the duty cycle affects the timbre.
// This demo shows how the timbre changes significantly with small changes to the pulse width:

while(true)
{
    for(int i; i < 20; i++)
    {
        pulse.width(.05 * i); // .width() takes a float from 0.0 to 1.0
        chout <= "current duty cycle: " <= pulse.width();
        if(pulse.width() == .5) {chout <= " <- this is just a square wave!";}
        chout <= IO.newline();
        1::second => now;
    }
}
