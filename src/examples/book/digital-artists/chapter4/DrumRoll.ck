// Sound Network
SndBuf snare => dac;

string snare_samples[3]; // array for holding samples

"sounds/snare_01.wav" => snare_samples[0];
"sounds/snare_02.wav" => snare_samples[1];
"sounds/snare_03.wav" => snare_samples[2];

snare_samples[0] => snare.read;
snare.samples() => snare.pos;
.25 => snare.gain;

fun int drumRoll(int index) {
    <<<index>>>;
    if (index >= 1) 
    {
        0 => snare.pos;
        index::ms => now;
        return drumRoll(index-1);
    }
    else if (index == 0)
    {
        return 0;
    }
}

drumRoll(40);
