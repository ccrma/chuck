class Crash extends Chugen {
    fun float tick(float v) {
        return v;
    }
}

global SinOsc s => global Crash c => dac;
true => s.buffered;
440.0 => s.freq;
1::second => now;

<<< "success" >>>;
