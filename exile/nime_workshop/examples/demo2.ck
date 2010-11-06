// set the global gain
.1 => dac.gain;

// connect
sinosc a => dac;
110.0 => a.freq;
1::second => now;
sinosc b => dac;
220.0 => b.freq;
1::second => now;
sinosc c => dac;
440.0 => c.freq;
1::second => now;
sinosc d => dac;
880.0 => d.freq;
1::second => now;
sinosc e => dac;
1760.0 => e.freq;
1::second => now;

// disconnect
a =< dac;
1::second => now;
b =< dac;
1::second => now;
c =< dac;
1::second => now;
d =< dac;
1::second => now;
e =< dac;
1::second => now;

