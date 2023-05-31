//---------------------------------------------------------------------
// name: type-type.ck
// desc: this example shows working with the Type type in ChucK;
//       a Type instance represents a specific type currently in
//       in the ChucK VM
//
// version: needs 1.5.0.0 or higher
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// print some info about a Type
fun void about( Type t )
{
    // divider
    cherr <= "--------------------------------------------------" <= IO.newline();
    // check
    if( t == null )
    {
        cherr <= "null Type passed to about()!" <= IO.newline();
        return;
    }

    //-----------------------------------------------------------------
    // use .name() to get full type name e.g., 'int[][]'
    //-----------------------------------------------------------------
    cherr <= "type name using .name(): " <= t.name() <= IO.newline();
    //-----------------------------------------------------------------
    // use .baseName() to get base type name (without the array part)
    //-----------------------------------------------------------------
    cherr <= "type name using .baseName(): " <= t.baseName();
    repeat( t.arrayDepth() ) cherr <= "[]";
    cherr <= IO.newline();

    //-----------------------------------------------------------------
    // parents
    //-----------------------------------------------------------------
    cherr <= "inherits: ";
    // get parent
    t.parent() @=> Type @ p;
    // nothing
    if( p == null ) cherr <= "(nothing)" <= IO.newline();
    else
    {
        while( p != null )
        {
            // name of parent
            cherr <= p.name();
            // get parent's parent
            p.parent() @=> p;
            // null?
            cherr <= ((p != null) ? " -> " : IO.newline());
        }
    }

    //-----------------------------------------------------------------
    // is a kind of?
    //-----------------------------------------------------------------
    cherr <= "isa...Object:" <= t.isa("Object") <= " "
          <= "UGen:" <= t.isa("UGen") <= " "
          <= "UAna:" <= t.isa("UAna") <= IO.newline();

    //-----------------------------------------------------------------
    // more attributes
    //-----------------------------------------------------------------
    cherr <= "primitive: " <= (t.isPrimitive() ? "YES" : "NO" ) <= IO.newline();
    cherr <= "array: " <= (t.isArray() ? "YES" : "NO" )
          <= " depth: " <= t.arrayDepth() <= IO.newline();
}

// make an array
int array[2][2];
about( array.typeOf() );
about( Type.of( array ) );

// a patch
SinOsc foo;

// get the type of classes
SinOsc.typeOf() @=> Type @ tSinOsc;
// print info
about( tSinOsc );
// get the type of an instance
foo.typeOf() @=> Type @ tfoo;
// print info (should be the same as above)
about( tfoo );

// get type of any value or variable
about( Type.of( 1 ) );
about( Type.of( 2.0 ) );
about( Type.of( now ) );
about( Type.of( 3::second ) );
about( Type.of( #(1,2) ) );
about( Type.of( %(1,pi/2) ) );
about( Type.of( @(1,2,3) ) );
about( Type.of( @(1,2,3,4) ) );
about( Type.of( dac ) );

// get type by name
about( Type.find("MFCC") );

// get the type of the Type type; should be itself
Type.typeOf() @=> Type @ tType;

// get all subclasses of StkInstrument
StkInstrument.typeOf().children() @=> Type kids[];
// print results
for( int i; i < kids.size(); i++ )
{
    cherr <= "StkInstrument: " <= kids[i].name() <= IO.newline();
}

// get all subclasses of UAna
UAna.typeOf().children() @=> kids;
// print results
for( int i; i < kids.size(); i++ )
{
    cherr <= "UAna: " <= kids[i].name() <= IO.newline();
}
