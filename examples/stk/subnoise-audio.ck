//------------------------------------------------------------------------------
// name: subnoise_audio.ck
// desc: demo showcasing the subnoise UGen as a white noise audio signal
// 
// SubNoise generates a random value between [-1, 1] every `rate` audio ticks.
// You can get/set the rate in code via the .rate() method.
// This example doubles the subnoise rate every second, up to a max of 
// 2^12 = 4096 ticks. 
// Notice how as `rate` increases, the audio becomes increasingly distorted
// and lowpassed, becase we are emulating downsampling the original white noise.
//
// author: azaday
// date: Spring 2023
//------------------------------------------------------------------------------
SubNoise sn => dac;
1 => sn.rate;

repeat (12) {
    <<< "subnoise rate: " + sn.rate() >>>;
    sn.rate() * 2 => sn.rate;
    1::second => now;
}
