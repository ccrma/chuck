// 20.ck : assignment inside functions

fun int foo( int x )
{
    x => external int a;
    return a;
}

if( foo( 3 ) == 3 )
    <<<"success">>>;
