Event e;

fun void esignal() {
    1::ms => now;
    e.signal();
}

spork ~ esignal();

select {
    case e: <<< "success" >>>;
    case 2::ms: <<< "failure" >>>;
}