// Validate that Shred.ancestor() correctly gets the top-level shred through multiple layers of recursion.

0 => int count; // count the number of recursive calls

fun void validateAncestor(int want_ancestor, int depth) {
    count++;
    me.ancestor().id() => int got_ancestor;

    if (want_ancestor != got_ancestor) {
      <<< "FAILURE, spork~ validateAncestor(...)'s ancestor shred should be the top-level shred" >>>;
      me.exit();
    }

    if (depth > 1) {
       spork~ validateAncestor(want_ancestor, depth-1);
    }
    samp => now;
}

// this should be the top-level shred (and has no ancestor)
if (me.ancestor() != null) {
   <<< "FAILURE, the top-level shred should have no ancestor" >>>;
   me.exit();
}

100 => int recursive_calls;
spork~ validateAncestor(me.id(), recursive_calls);

samp => now;

if (count != recursive_calls) {
   <<< "FAILURE, incorrect number of recursive calls made. Got", count, "want", recursive_calls >>>;
   me.exit();
}
<<< "success" >>>;

