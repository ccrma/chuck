// test string == and !=

null$string => external string nullStr;
"abc" => external string abc;
"abc" => external string abc2;
"def" => external string def;

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
