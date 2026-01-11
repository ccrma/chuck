// Generated from simple.toml
60.0 / 120 => float spb;
spb :: second => dur beat;
0.6 => float g_complexity;

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
  0.3 => osc_0.gain;
  0.1 => pan_0.pan;
  env_0.set(0.05, 0, 0, 0.3);
  900 => filt_0.freq;
  0.1 => rev_0.mix;
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    if( Math.random2f(0.0, 1.0) > 0.8 ) { continue; }
    Std.mtof(applyScale(62, 5)) + 0 => osc_0.freq;
    env_0.keyOn();
    1 * beat => now;
    env_0.keyOff();
    0.3 :: second => now;
    // note 1
    if( Math.random2f(0.0, 1.0) > 0.8 ) { continue; }
    Std.mtof(applyScale(65, 5)) + 0 => osc_0.freq;
    env_0.keyOn();
    1 * beat => now;
    env_0.keyOff();
    0.3 :: second => now;
    // note 2
    if( Math.random2f(0.0, 1.0) > 0.8 ) { continue; }
    Std.mtof(applyScale(69, 5)) + 0 => osc_0.freq;
    env_0.keyOn();
    2 * beat => now;
    env_0.keyOff();
    0.3 :: second => now;
  }
}

fun void voice_1() {
  1 * beat => now;
  Noise osc_1;
  ADSR env_1;
  Pan2 pan_1;
  osc_1 => env_1 => pan_1 => dac;
  0.1 => osc_1.gain;
  -0.2 => pan_1.pan;
  env_1.set(0, 0, 0, 0);
  for( 0 => int rep; rep < 1; rep++ ) {
    // note 0
    env_1.keyOn();
    0.5 * beat => now;
    env_1.keyOff();
    // note 1
    env_1.keyOn();
    0.5 * beat => now;
    env_1.keyOff();
  }
}

// spawn voices
spork ~ voice_0();
spork ~ voice_1();
// keep alive (could be tightened with computed piece length)
while(true) 1::second => now;
