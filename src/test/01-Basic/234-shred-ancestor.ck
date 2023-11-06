// validate that Shred.ancestor() correctly gets the top-level 
// shred through multiple layers of recursion

// count the number of recursive calls
0 => int count;

fun void validateAncestor(int want_ancestor, int depth)
{
    count++;
    me.ancestor().id() => int got_ancestor;

    if( want_ancestor != got_ancestor )
    {
        <<< "FAILURE, spork~ validateAncestor(...)'s ancestor shred should be the top-level shred" >>>;
        me.exit();
    }

    if( depth > 1 )
    {
        spork ~ validateAncestor(want_ancestor, depth-1);
    }
    // let time pass to let children shreds run
    samp => now;
}

// this should be the top-level shred (and has no ancestor)
if( me.ancestor() != me )
{
    <<< "FAILURE, the top-level shred should have be me" >>>;
    me.exit();
}

100 => int recursive_calls;
spork ~ validateAncestor( me.id(), recursive_calls );

// let time pass to let children shreds run
samp => now;

// check count
if( count != recursive_calls )
{
    <<< "FAILURE: incorrect number of recursive calls made.",
        "got:", count, "want:", recursive_calls >>>;
    me.exit();
}

// done
<<< "success" >>>;

