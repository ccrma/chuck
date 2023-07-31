// test array.getKeys()

true => int passing;

int a[0];
string a_keys[0];

a.getKeys(a_keys);

if( a_keys.size() != 0 )
{
    <<< "key array should be empty" >>>;
    false => passing;
}

float b[0];
string b_keys[0];
["key1", "key2"] @=> string b_want[];

1 => b["key1"];
2 => b["key2"];

b.getKeys(b_keys);

checkKeys(b_keys, b_want) && passing => passing;

vec3 c[0];
string c_keys[0];
["key2"] @=> string c_want[];

@(0,0,0) => c["key1"];
@(0,0,1) => c["key2"];
@(0,0,2) => c["key1"];
@(0,0,3) => c["key1"];
c.erase("key1");

c.getKeys(c_keys);
checkKeys(c_keys, c_want) && passing => passing;

string c_keys2[0];
c.getKeys(c_keys2);
checkKeys(c_keys2, c_want) && passing => passing;

if( passing ) <<< "success" >>>;

fun int checkKeys(string got[], string want[])
{
    if( got.size() != want.size() )
    {
        printFailure(got, want);
        return false;
    }

    true => int passing;
    for( 0 => int i; i < want.size(); i++ )
    {
	    false => int contains;
	    for( 0 => int j; j < got.size(); j++ )
        {
	        if (got[i] == want[j])
            {
		        true => contains;
	        }
	    }
	    if( !contains ) false => passing;
    }

    if (!passing)
    {
	    printFailure(got, want);
	    return false;
    }

    return true;
}

fun void printFailure( string got[], string want[] )
{
    <<< "failure in get_keys(), got:" >>>;
    for( 0 => int i; i < got.size(); i++ ) <<< got[i] >>>;
    <<< "want:" >>>;
    for( 0 => int i; i < want.size(); i++ ) <<< want[i] >>>;
}
