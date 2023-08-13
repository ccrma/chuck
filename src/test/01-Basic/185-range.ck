// test Std.range functionality
// requires 1.5.1.1 (nshaheed)

[0, 1, 2, 3] @=> int want[];
Std.range(4) @=> int got[];

if (want.size() != got.size()) {
    <<< "want[] size different from got[] size" >>>;
    me.exit();
}

for (int i; i < want.size(); i++) {
    if (want[i] != got[i]) {
        <<< "want[", i, "is different from got[", i, "]" >>>;
        me.exit();
    }
}

// Test empty
Std.range(0) @=> got;

if (got.size() != 0) {
    <<< "Std.range(0) does not return an empty array" >>>;
    me.exit();
}

// Test start/stop range
[3,4,5,6] @=> want;
Std.range(3,7) @=> got;

if (want.size() != got.size()) {
    <<< "want[] size different from got[] size" >>>;
    me.exit();
}

for (int i; i < want.size(); i++) {
    if (want[i] != got[i]) {
        <<< "want[", i, "is different from got[", i, "]" >>>;
        me.exit();
    }
}

// validate negative
print( Std.range(-5) );
// validate reverse
print( Std.range(7,-4) );

// validate step
print( Std.range(-1,5,1) );
print( Std.range(-2,-10,-1) );
print( Std.range(0,20,4) );
print( Std.range(5,-20,-4) );
// 4 works the same as -4
print( Std.range(5,-20,4) );

// validate zero length
print( Std.range(1,1,2) );
print( Std.range(1,1,-1) );
print( Std.range(-1,-1,-1) );

// step == 0
print( Std.range(1,1,0) );

// formatted array print
fun void print( int array[] )
{
    // print open
    cherr <= "[ ";
    // print each element
    for( int x : array ) cherr <= x <= ", ";
    // print close
    cherr <= "]";
    // new line
    cherr <= IO.nl();
}
