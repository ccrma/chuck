// validate that Shred.parent() correctly gets the parent shred
// through multiple layers of recursion.

0 => int count; // count the number of recursive calls

fun void validateParent( int want_parent, int depth )
{
    count++;
    me.parent().id() => int got_parent;

    if( want_parent != got_parent )
    {
        <<< "FAILURE, spork~ validateParent(...)'s parent shred is not matching" >>>;
        me.exit();
    }

    if( depth > 1 )
    {
        spork~ validateParent( me.id(), depth-1 );
    }
    samp => now;
}

// this should be the top-level shred (and has no parent)
if( me.parent() != null )
{
    <<< "FAILURE, the top-level shred should have no parent" >>>;
    me.exit();
}

100 => int recursive_calls;
spork~ validateParent( me.id(), recursive_calls );

// pass time (or yield) to let children shreds run
samp => now;

// check count
if( count != recursive_calls )
{
    <<< "FAILURE: incorrect number of recursive calls made. got:",
        count, "want:", recursive_calls >>>;
    me.exit();
}

// done
<<< "success" >>>;

