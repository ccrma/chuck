class Crash extends Chugen {
    fun float tick(float v) {
        return v;
    }
}

SinOsc s => Crash c => dac;
440.0 => s.freq;
1::second => now;

<<< "success" >>>;
