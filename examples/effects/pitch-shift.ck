//------------------------------------------------------------------------------
// name: pitshift.ck
// desc: basic demo of the PitShift (pitch shift) UGen
//
// The PitShift UGen uses delay lines to create a simple pitch shift effect. The
// input signal is copied, pitch-shifted, and mixed with the original signal.
// The amount of the pitch-shifted signal that is present can be set using .mix()
// The amount of pitch shifting for the modified signal can be set using .shift()
// When .shift() = 1, the modified signal has the same pitch as the original.
// .shift() can be positive or negative.
// The example here demonstrates a pitch shifting for two different kinds of signal
// The first one uses a sawtooth wave as input signal, sweeping the pitch shift
// amount between -2.5 and 2.5.
// The second loops a SndBuf (audio file) with random pitch shift amounts.
//
// author: Alex Han
// date: Spring 2023
//------------------------------------------------------------------------------

SawOsc s => PitShift ps => dac; // for example 1
SndBuf buf => PitShift ps2 => dac; // for example 2

s.gain(0);
s.freq(440);

buf.gain(0);
"special:dope" => buf.read;

ps.mix(.5);
ps2.mix(1);

// pitch-shifted sawtooth, sweeping shift amount between -2.5 and 2.5
fun void sawShift()
{
    s.gain(.2);

    while(true)
    {
        for(-250 => int i; i < 250; i++)
        {
            ps.shift(i*.01);
            chout <= "current pitch shift amount: " <= ps.shift();
            chout <= IO.newline();
            50::ms => now;
        }
    }

}

// pitch shifted audio file playback ("dope!") random shift amount each time
fun void dopeShift()
{
    1 => buf.gain;

    while(true)
    {
        0 => buf.pos;
        ps2.shift(Math.random2f(-5.0, 5.0));
        chout <= "current pitch shift amount: " <= ps2.shift();
        chout <= IO.newline();
        buf.length() => now;
    }

}

// uncomment either of these to try
spork ~ sawShift();
// spork ~ dopeShift();

// time loop
while(true)
{
    1::second => now;
}