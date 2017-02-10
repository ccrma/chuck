// fail for access of non-local non-global primitive
spork ~ {
    int i;
    spork ~ {
        i + 1;
    };
};