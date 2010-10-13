// demo2.ck 
// really lame!

// set the global gain
.1 => dac.gain;

/*
// connect
SinOsc a => dac;
110.0 => a.freq;
1::second => now;
SinOsc b => dac;
220.0 => b.freq;
1::second => now;
SinOsc c => dac;
440.0 => c.freq;
1::second => now;
SinOsc d => dac;
880.0 => d.freq;
1::second => now;
SinOsc e => dac;
1760.0 => e.freq;
1::second => now;
*/

SinOsc oscarray[5];
for(0 => int i; i<5; i++) {

	oscarray[i] => dac;
	Math.pow(2, i) * 110.0 => oscarray[i].freq;

}

/*
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
*/

for(0 => int i; i<5; i++) {

	oscarray[i] =< dac;
	1::second => now;
	
}












