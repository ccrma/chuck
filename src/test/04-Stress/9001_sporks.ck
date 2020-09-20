// 9001_sporks.ck
// thanks to Michael Heuer <heuermh@gmail.com> for providing this example code

fun void blink() {
    while (true) {
        1::samp => now;
    }
}

Shred shred;
spork ~ blink() @=> shred;

0 => int count;

while (count <= 9001) {
    1::samp => now;
    shred.id() => Machine.remove;
    spork ~ blink() @=> shred;
    
    1 +=> count;
}

<<< "success" >>>;
