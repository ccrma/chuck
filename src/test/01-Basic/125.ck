fun int sporkReturn() {
    spork ~ {
        return 1;
    };
    1::samp => now;
    return 0;
}

<<< sporkReturn() >>>;
