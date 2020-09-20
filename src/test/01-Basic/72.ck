// test string == and !=

null$string => string nullStr;
"abc" => string abc;
"abc" => string abc2;
"def" => string def;

failIfFalse(abc == abc2);
failIfTrue(abc == def);
failIfTrue(abc == null);
failIfFalse(nullStr == null);

failIfTrue(abc != abc2);
failIfFalse(abc != def);
failIfFalse(abc != null);
failIfTrue(nullStr != null);

<<< "success" >>>;
    
fun void failIfTrue(int b)
{
    if(b)
    {
        <<< "failure" >>>;
        me.exit();
    }
}

fun void failIfFalse(int b)
{
    if(!b)
    {
        <<< "failure" >>>;
        me.exit();
    }
}
