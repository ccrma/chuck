// test implicit cast in return type

fun float foo()
{
    // return int in a func that returns float
    return 3;
}

if( foo() == 3 )
    <<< "success" >>>;
