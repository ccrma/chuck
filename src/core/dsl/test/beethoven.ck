// Generated from beethoven.toml
60.0 / 108 => float spb;
spb :: second => dur beat;
0.5 => float g_complexity;

// Placeholder scale helper (currently pass-through)
fun int applyScale(int midi, int scale) { return midi; }

fun void voice_0() {
  0 * beat => now;
  SawOsc osc_0;
  ADSR env_0;
  LPF filt_0;
  NRev rev_0;
  Pan2 pan_0;
  osc_0 => filt_0 => env_0 => rev_0 => pan_0 => dac;
  0.22 => osc_0.gain;
  0.15 => pan_0.pan;
  env_0.set(0.005, 0.02, 0.7, 0);
  2200 => filt_0.freq;
  0.8 => filt_0.Q;
  0.1 => rev_0.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(67, 0)) + 0 => osc_0.freq;
    env_0.keyOn();
    0.5 * beat => now;
    env_0.keyOff();
    // note 1
    Std.mtof(applyScale(67, 0)) + 0 => osc_0.freq;
    env_0.keyOn();
    0.5 * beat => now;
    env_0.keyOff();
    // note 2
    Std.mtof(applyScale(67, 0)) + 0 => osc_0.freq;
    env_0.keyOn();
    0.5 * beat => now;
    env_0.keyOff();
    // note 3
    Std.mtof(applyScale(63, 0)) + 0 => osc_0.freq;
    env_0.keyOn();
    1.5 * beat => now;
    env_0.keyOff();
  }
}

fun void voice_1() {
  0 * beat => now;
  TriOsc osc_1;
  ADSR env_1;
  LPF filt_1;
  NRev rev_1;
  Pan2 pan_1;
  osc_1 => filt_1 => env_1 => rev_1 => pan_1 => dac;
  0.18 => osc_1.gain;
  -0.15 => pan_1.pan;
  env_1.set(0.005, 0.02, 0.7, 0);
  900 => filt_1.freq;
  0.7 => filt_1.Q;
  0.1 => rev_1.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(55, 0)) + 0 => osc_1.freq;
    env_1.keyOn();
    0.5 * beat => now;
    env_1.keyOff();
    // note 1
    Std.mtof(applyScale(55, 0)) + 0 => osc_1.freq;
    env_1.keyOn();
    0.5 * beat => now;
    env_1.keyOff();
    // note 2
    Std.mtof(applyScale(55, 0)) + 0 => osc_1.freq;
    env_1.keyOn();
    0.5 * beat => now;
    env_1.keyOff();
    // note 3
    Std.mtof(applyScale(51, 0)) + 0 => osc_1.freq;
    env_1.keyOn();
    1.5 * beat => now;
    env_1.keyOff();
  }
}

fun void voice_2() {
  3.5 * beat => now;
  SawOsc osc_2;
  ADSR env_2;
  LPF filt_2;
  NRev rev_2;
  Pan2 pan_2;
  osc_2 => filt_2 => env_2 => rev_2 => pan_2 => dac;
  0.22 => osc_2.gain;
  0.15 => pan_2.pan;
  env_2.set(0.005, 0.02, 0.7, 0);
  2200 => filt_2.freq;
  0.8 => filt_2.Q;
  0.1 => rev_2.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(67, 0)) + 0 => osc_2.freq;
    env_2.keyOn();
    0.5 * beat => now;
    env_2.keyOff();
    // note 1
    Std.mtof(applyScale(67, 0)) + 0 => osc_2.freq;
    env_2.keyOn();
    0.5 * beat => now;
    env_2.keyOff();
    // note 2
    Std.mtof(applyScale(67, 0)) + 0 => osc_2.freq;
    env_2.keyOn();
    0.5 * beat => now;
    env_2.keyOff();
    // note 3
    Std.mtof(applyScale(63, 0)) + 0 => osc_2.freq;
    env_2.keyOn();
    1.5 * beat => now;
    env_2.keyOff();
  }
}

fun void voice_3() {
  3.5 * beat => now;
  TriOsc osc_3;
  ADSR env_3;
  LPF filt_3;
  NRev rev_3;
  Pan2 pan_3;
  osc_3 => filt_3 => env_3 => rev_3 => pan_3 => dac;
  0.18 => osc_3.gain;
  -0.15 => pan_3.pan;
  env_3.set(0.005, 0.02, 0.7, 0);
  900 => filt_3.freq;
  0.7 => filt_3.Q;
  0.1 => rev_3.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(55, 0)) + 0 => osc_3.freq;
    env_3.keyOn();
    0.5 * beat => now;
    env_3.keyOff();
    // note 1
    Std.mtof(applyScale(55, 0)) + 0 => osc_3.freq;
    env_3.keyOn();
    0.5 * beat => now;
    env_3.keyOff();
    // note 2
    Std.mtof(applyScale(55, 0)) + 0 => osc_3.freq;
    env_3.keyOn();
    0.5 * beat => now;
    env_3.keyOff();
    // note 3
    Std.mtof(applyScale(51, 0)) + 0 => osc_3.freq;
    env_3.keyOn();
    1.5 * beat => now;
    env_3.keyOff();
  }
}

fun void voice_4() {
  7 * beat => now;
  SawOsc osc_4;
  ADSR env_4;
  LPF filt_4;
  NRev rev_4;
  Pan2 pan_4;
  osc_4 => filt_4 => env_4 => rev_4 => pan_4 => dac;
  0.22 => osc_4.gain;
  0.15 => pan_4.pan;
  env_4.set(0.005, 0.02, 0.7, 0);
  2200 => filt_4.freq;
  0.8 => filt_4.Q;
  0.1 => rev_4.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(65, 0)) + 0 => osc_4.freq;
    env_4.keyOn();
    0.5 * beat => now;
    env_4.keyOff();
    // note 1
    Std.mtof(applyScale(65, 0)) + 0 => osc_4.freq;
    env_4.keyOn();
    0.5 * beat => now;
    env_4.keyOff();
    // note 2
    Std.mtof(applyScale(65, 0)) + 0 => osc_4.freq;
    env_4.keyOn();
    0.5 * beat => now;
    env_4.keyOff();
    // note 3
    Std.mtof(applyScale(62, 0)) + 0 => osc_4.freq;
    env_4.keyOn();
    1.5 * beat => now;
    env_4.keyOff();
  }
}

fun void voice_5() {
  7 * beat => now;
  TriOsc osc_5;
  ADSR env_5;
  LPF filt_5;
  NRev rev_5;
  Pan2 pan_5;
  osc_5 => filt_5 => env_5 => rev_5 => pan_5 => dac;
  0.18 => osc_5.gain;
  -0.15 => pan_5.pan;
  env_5.set(0.005, 0.02, 0.7, 0);
  900 => filt_5.freq;
  0.7 => filt_5.Q;
  0.1 => rev_5.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(53, 0)) + 0 => osc_5.freq;
    env_5.keyOn();
    0.5 * beat => now;
    env_5.keyOff();
    // note 1
    Std.mtof(applyScale(53, 0)) + 0 => osc_5.freq;
    env_5.keyOn();
    0.5 * beat => now;
    env_5.keyOff();
    // note 2
    Std.mtof(applyScale(53, 0)) + 0 => osc_5.freq;
    env_5.keyOn();
    0.5 * beat => now;
    env_5.keyOff();
    // note 3
    Std.mtof(applyScale(50, 0)) + 0 => osc_5.freq;
    env_5.keyOn();
    1.5 * beat => now;
    env_5.keyOff();
  }
}

fun void voice_6() {
  10.5 * beat => now;
  SawOsc osc_6;
  ADSR env_6;
  LPF filt_6;
  NRev rev_6;
  Pan2 pan_6;
  osc_6 => filt_6 => env_6 => rev_6 => pan_6 => dac;
  0.22 => osc_6.gain;
  0.15 => pan_6.pan;
  env_6.set(0.005, 0.02, 0.7, 0);
  2200 => filt_6.freq;
  0.8 => filt_6.Q;
  0.1 => rev_6.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(65, 0)) + 0 => osc_6.freq;
    env_6.keyOn();
    0.5 * beat => now;
    env_6.keyOff();
    // note 1
    Std.mtof(applyScale(65, 0)) + 0 => osc_6.freq;
    env_6.keyOn();
    0.5 * beat => now;
    env_6.keyOff();
    // note 2
    Std.mtof(applyScale(65, 0)) + 0 => osc_6.freq;
    env_6.keyOn();
    0.5 * beat => now;
    env_6.keyOff();
    // note 3
    Std.mtof(applyScale(62, 0)) + 0 => osc_6.freq;
    env_6.keyOn();
    1.5 * beat => now;
    env_6.keyOff();
  }
}

fun void voice_7() {
  10.5 * beat => now;
  TriOsc osc_7;
  ADSR env_7;
  LPF filt_7;
  NRev rev_7;
  Pan2 pan_7;
  osc_7 => filt_7 => env_7 => rev_7 => pan_7 => dac;
  0.18 => osc_7.gain;
  -0.15 => pan_7.pan;
  env_7.set(0.005, 0.02, 0.7, 0);
  900 => filt_7.freq;
  0.7 => filt_7.Q;
  0.1 => rev_7.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    Std.mtof(applyScale(53, 0)) + 0 => osc_7.freq;
    env_7.keyOn();
    0.5 * beat => now;
    env_7.keyOff();
    // note 1
    Std.mtof(applyScale(53, 0)) + 0 => osc_7.freq;
    env_7.keyOn();
    0.5 * beat => now;
    env_7.keyOff();
    // note 2
    Std.mtof(applyScale(53, 0)) + 0 => osc_7.freq;
    env_7.keyOn();
    0.5 * beat => now;
    env_7.keyOff();
    // note 3
    Std.mtof(applyScale(50, 0)) + 0 => osc_7.freq;
    env_7.keyOn();
    1.5 * beat => now;
    env_7.keyOff();
  }
}

// spawn voices
spork ~ voice_0();
spork ~ voice_1();
spork ~ voice_2();
spork ~ voice_3();
spork ~ voice_4();
spork ~ voice_5();
spork ~ voice_6();
spork ~ voice_7();
// keep alive (could be tightened with computed piece length)
while(true) 1::second => now;
