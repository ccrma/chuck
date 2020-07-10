now => time t1;

select {
    case t1: <<< "success" >>>;
    case 1::ms: {
        <<< "failure" >>>;
    }
}

select {
    case 1::ms: {
        <<< "failure" >>>;
    }
    case t1: <<< "success" >>>;
}

Event e;

fun void s() {
    1::ms => now;
    e.signal();
}

spork ~ s();

select {
    case e: <<< "failure" >>>;
    case t1: <<< "success" >>>;
}

if (now != t1) {
    <<< "failure: time shouldn't have advanced" >>>;
}