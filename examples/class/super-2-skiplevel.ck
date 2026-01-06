// name: super-2-skiplevel.ck
// desc: usage of `super` keyword when functions are overriden every
//       level of the inheritance
// authors: Nick and Ge, Fall 2025 

// W
class W 
{
    // a function
    fun void boo( int i ) { <<< "W", i >>>; }
}

// child of W
class X extends W
{
    // X does not overrie boo (but X's child Y does override)
    // fun void boo( int i ) { <<< "X", i >>>; super.boo(3); }
}

// child of X; grandchild of W
class Y extends X
{
    // calling superclass boo from class pre-constructor code
    // FYI this should call W's boo(), skipping X
    super.boo(1);

    // overriding boo()
    fun void boo( int i ) { <<< "Y", i >>>; }
}

// instantiate a Y
Y y;

// call
y.boo( 4 );
