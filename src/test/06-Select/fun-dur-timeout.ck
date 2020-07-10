fun dur d1() {
    return 1::ms;
}

select {
    case d1(): <<< "success" >>>;
    case (1::ms + 1::samp): <<< "failure" >>>;
}