// Listing 5.18 Recursive drum roll using only an Impulse UGen

Impulse imp => dac;               // (1) Impulse (click) generator to dac.

fun int impRoll(int index) {      // (2) impRoll function definition.
    if (index >=1)
    {
        1.0 => imp.next;
        index::ms => now;         // (3) Duration is the recursion variable.
        return impRoll(index-1);  // (4) Recursive call from impRoll to impRoll.
    }
    else {
        return 0;
    }
}

impRoll(20);                      // (5) Tests all with different starting durations.
second => now;

impRoll(40);
second => now;

impRoll(60);
second => now;
