//-------------------------------------------------------------------
// name: randomize()
// desc: random number generation example...
//       to show the effects of alternating between
//       1) explicitly seeding the random number generator (RNG)
//       -- AND --
//       2) implicitly seeding the RNG with a practially
//          unpredicable seed
//
// requires: chuck-1.5.0.4 or higher
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Summer 2023
//-------------------------------------------------------------------

// explicit seed
10513 => int EXPLICIT_SEED;
// how many to print each round
8 => int HOW_MANY;
// how long to wait between each number
100::ms => dur T;

fun void generate( int howMany, dur T, int indents )
{
    int i;
    repeat(howMany)
    {
        T => now;
        repeat(indents) cherr <= " ";
        <<< ++i, Math.random2(1,100) >>>;
    }
}

while( true )
{
    // explicit seeding
    <<< "--------------------------------------", "" >>>;
    <<< "seeding RNG with", EXPLICIT_SEED, "SAME EACH TIME!" >>>;
    <<< "--------------------------------------", "" >>>;
    Math.srandom( EXPLICIT_SEED );
    // generate
    generate( HOW_MANY, T, 0 );

    // wait a bit
    5*T => now;    
    
    <<< "********************", "" >>>;
    <<< "SHAKING THINGS UP!!!", "" >>>;
    <<< "********************", "" >>>;
    Math.randomize();
    // generate
    generate( HOW_MANY, T, 16 );

    // wait a bit
    5*T => now;    
}
