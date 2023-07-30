fun void foo()
{
    [1,2,3] @=> int array[];
    for( int a : array )
    {
        if( a == 3 ) return;
    }

    // trap
    <<< "shouldn't get here" >>>;
}

foo();

// done
<<< "success" >>>;

