//------------------------------------------------------------------------------
// name: subnoise_control.ck
// desc: demo showcasing the subnoise UGen as a control signal to modulate
//       the frequency of a SinOsc
// 
// SubNoise generates a random value between [-1, 1] every `rate` audio ticks.
// You can get/set the rate in code via the .rate() method.
// This example showcases uses SubNoise to randomly change the frequency of a
// SinOsc every half-second.
//
// author: azaday
// date: Spring 2023
//------------------------------------------------------------------------------
SubNoise sn => blackhole;
SinOsc osc => dac;

// tell subnoise to only change its output every half second 
.5::second => dur period;
(period/samp) $ int   => sn.rate;
<<< "setting subnoise rate: " + sn.rate() >>>;

repeat (10) {
    // remap subnoise sample from [-1, 1] to an osc frequency in [220, 880]hz
    Math.remap(sn.last(), -1.0, 1.0, 220.0, 880.0) => osc.freq;
    <<< "remapping osc frequency to: " + osc.freq() >>>;
    period => now;
}
