//---------------------------------------------------------------------
// name: polymorph.ck
// desc: example showing polymorphism using a base class and two
//       subclasses that each implement their own play() function;
//       a generic array of base class references is initialized
//       with the more specific subclass instances; this allows
//       for .play() to be called during the loop, the behavior of
//       which can take on different forms depending on the specific
//       type of underlying subclass.
//
//       philosophically, polymorphism in programming amounts to
//       a structured way to "break" the established contract between
//       a type system and the programmer, allowing objects of
//       apparently the same type to exhibit different behaviors.
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------
// generic base class
class Motif
{
    // default method to be potentially overridden in subclasses
    fun void play()
    {
        <<< "base class Motif.play()", "" >>>;
    }
}

// subclass that inherits from Motif
// can think of MoA as a more specific kind of Motif
class MoA extends Motif
{
    // define play behavior for MoA by overriding play()
    fun void play()
    {
        <<< "do Motif A", "" >>>;
    }
}

// subclass that inherits from Motif
// can think of MoB as a more specific kind of Motif
class MoB extends Motif
{
    // define play behavior for MoB by overriding play()
    fun void play()
    {
        <<< "do Motif B", "" >>>;
    }
}

// instantiate array of base class Motif references
// each element is a specific kind of Motif
[new MoA, new MoB, new MoB, new MoA] @=> Motif @ motifs[];

// array index variable
0 => int which;

// loop
while( true )
{
    // will automatically call the right play() depending on
    // the underlying type (MoA vs. MoB) as instantiated above
    spork ~ motifs[which].play();
    
    // advance time
    300::ms => now;

    // cycle which / print divider every cycel
    which++; if( which >= motifs.size() )
    { 0 => which; cherr <= "--" <= IO.newline(); }    
}
