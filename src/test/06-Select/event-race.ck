Event e;
Event e2;

fun void esignal() {
    100::ms => now;
    e.signal();
}

fun void e2signal() {
    100::ms + 1::samp => now;
    e2.signal();
}

spork ~ e2signal();
spork ~ esignal();

select {
    case e2: <<< "failure" >>>;
    case e: <<< "success" >>>;
}