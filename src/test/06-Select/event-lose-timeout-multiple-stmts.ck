Event e;

fun void esignal() {
    2::ms => now;
    e.signal();
}

spork ~ esignal();

select {
    case e: <<< "failure" >>>;
    case 1::ms: {
        <<< "no success yet" >>>;
        <<< "success" >>>;
    }
}