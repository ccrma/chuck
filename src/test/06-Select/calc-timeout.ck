0.5::ms + 0.5::ms => dur d;

select {
    case d: <<< "success" >>>;
    case (1::ms + 1::samp): <<< "failure" >>>;
}

fun dur d1() {
    return 1::ms;
}

select {
    case d1(): <<< "success" >>>;
    case (1::ms + 1::samp): <<< "failure" >>>;
}