// 147_spork.ck
// thanks to Michael Heuer <heuermh@gmail.com> for providing this example code
// desc: Exercises the segmentation fault caused by sporking 147 shreds. 

fun void blink() {
    while (true) {
        1::ms => now;
    }
}

Shred shred;
spork ~ blink() @=> shred;

0 => int count;

while (count <= 147) {
    1::ms => now;
    shred.id() => Machine.remove;
    spork ~ blink() @=> shred;
    
    1 +=> count;
}

<<< "success" >>>;
