// name: dtmf.ck
// desc: dual tone multiple frequency
//       (can be played into analog phones to place calls)
// author: Perry Cook

SinOsc row => dac;
SinOsc col => dac;

// frequencies
[1209.0, 1336.0, 1477.0] @=> float cols[];
[697.0, 770.0, 852.0, 941.0] @=> float rows[];

// if you want to look up by number ( 0 - 9, *, # )
fun int key2col( int key )
{ if( !key ) return 1; return (key - 1) % 3; }
fun int key2row( int key )
{ if( !key ) return 3; return (key - 1) / 3; }

0 => int i;
int r,c,n;

// go!
while (i < 7) {
    .5 => row.gain;
    .5 => col.gain;
    Math.random2(0,3) => r;
    Math.random2(0,2) => c;
    1 + r * 3 + c => n;

    if (n==11) 0 => n;
    if (n==10) {
        <<< r , c, "*" >>>;
    }
    else if (n==12) {
        <<< r , c, "#" >>>;
    }
    else
        <<< r , c, n >>>;

    rows[r] => row.freq;
    cols[c] => col.freq;

    0.1 :: second => now;
    0.0 => row.gain;
    0.0 => col.gain;
    0.05 :: second => now;
    i + 1 => i;
}
