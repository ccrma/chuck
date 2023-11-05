// If ancestor.ck is the top-level shred, then me.ancestor() will be null.
// Otherwise it will be a Shred.
<<< "The top-level shred's ID is:", me.id() >>>;
<<< "The top-level shred's ancestor is:", me.ancestor(), "\n" >>>;

fun void findTheAncestor(int depth) {
    if (depth <= 0) return;

    <<< "The sporked shred's ID is:", me.id() >>>;
    // this will always be the same as the top-level shred id
    <<< "The sporked shred's ancestor ID is:", me.ancestor().id(), "\n" >>>;

    spork~ findTheAncestor(depth-1);
    samp => now;
}

// Recursively spork function this 10 times.
// me.ancestor() will always point to the top-level shred.
spork~ findTheAncestor(10);
samp => now;

