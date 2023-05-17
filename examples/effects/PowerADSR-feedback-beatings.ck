// Eric Heep
// April 3rd, 2017
// poweradsr-feedback-beatings.ck

// set number of sines and power envs
5 => int num;
MagicSine sin[num];
PowerADSR env[num];

// sound chain
for (int i; i < num; i++) {
    sin[i] => env[i] => dac;
    sin[i].gain(0.25/num);
}

fun void loopSin(int idx) {
    dur env_time;
    float curve;
    while (true) {
        Math.random2(1, 2)::second => env_time;

        // curves below 1.0 will be "hill" shaped,
        // curves above 1.0 will resemble an exponential curve
        Math.random2f(0.5, 4.0) => curve;
        sin[idx].freq(Math.random2f(1000, 1100));

        // set all envelope durations
        env[idx].set(env_time, 1::second, 0.5, env_time);

        // set all envelope power curves
        env[idx].setCurves(curve, 1.0, 1.0/curve);

        // begins attack phase
        env[idx].keyOn();
        env_time => now;

        // begins decay phase
        1::second => now;

        // begins release phase
        env[idx].keyOff();
        env_time => now;
    }
}

for (int i; i < num; i++) {
    spork ~ loopSin(i);
}

while (true) {
    1::second => now;
}
