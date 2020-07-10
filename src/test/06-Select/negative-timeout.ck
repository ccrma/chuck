now => time t1;
1::ms => now;
now => time t2;

// t1 is now in the past

select {
    case t1: <<< "failure" >>>;
    case t2: {
        <<< "success" >>>;
    }
}

if (t2 != now) {
    <<< "failure: time shouldn't have advanced" >>>;
}

select {
    case t1: <<< "failure" >>>;
    case 1::ms: {
        <<< "success" >>>;
    }
}


Event e;

fun void s() {
    1::ms => now;
    e.signal();
}

spork ~ s();

select {
    case e: <<< "success" >>>;
    case t1: <<< "failure" >>>;
    case 2::ms: <<< "failure" >>>;
}

now => t2;

spork ~ s();

select {
    case e: <<< "failure" >>>;
    case t1: <<< "failure" >>>;
    case t2: <<< "success" >>>;
}

if (t2 != now) {
    <<< "failure: time shouldn't have advanced" >>>;
}

spork ~ s();

// this should crash now with NegativeTimeException
select {
    case e: <<< "failure" >>>;
    case t1: <<< "failure" >>>;
}