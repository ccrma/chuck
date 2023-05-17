class Mode extends Chubgraph {
    SinOsc s => ExpEnv e => outlet;
    fun void freq(float freq) { freq => s.freq; }
    fun void T60(dur tsixty) { tsixty => e.T60; }
    fun void keyOn() { 1 => e.keyOn; }
}

Mode mod => dac;

1000 => mod.freq;
second/2 => mod.T60;
mod.keyOn();
second => now;

700 => mod.freq;
2*second => mod.T60;
mod.keyOn();
2*second => now;

1200 => mod.freq;
3::second => mod.T60;
mod.keyOn();
3*second => now;


