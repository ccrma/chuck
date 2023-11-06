// test shred.parent()

// check shred parent
fun void validateParent(int main_shred_id)
{
    // id of parent
    me.parent().id() => int parent_id;

    // make sure same id
    if( parent_id != main_shred_id )
    {
        <<< "FAILURE: parent shred should be the top-level shred" >>>;
        me.exit();
    }
}

// this should be the top-level shred (and has no parent)
if( me.parent() != null )
{
    <<< "FAILURE, the top-level shred should have no parent" >>>;
    me.exit();
}

// spork 
spork ~ validateParent( me.id() );
// yield to let validateParent(...) run
me.yield();
// print
<<< "success" >>>;
