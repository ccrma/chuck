fun void validateParent(int main_shred_id) {
    me.parent().id() => int parent_id;

    if (parent_id != main_shred_id) {
      <<< "FAILURE, spork~ validateParent(...)'s parent shred should be the top-level shred" >>>;
      me.exit();
    }
}

// this should be the top-level shred (and has no parent)
if (me.parent() != null) {
   <<< "FAILURE, the top-level shred should have no parent" >>>;
   me.exit();
}

spork~ validateParent(me.id());
me.yield(); // yield to let validateParent(...) run
<<< "success" >>>;