// shared globals
2 => float f;

spork ~ {
    2.2 => f;
};

f => float f1;
1::samp => now;
// f should have been modified by spork
f => float f2;
if( f1 != f2 ) {
    <<< "success" >>>;
}
