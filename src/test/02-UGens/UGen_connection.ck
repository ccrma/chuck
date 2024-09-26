SinOsc s => dac;
// s => blackhole;
// s => Gain g;

s.goesTo() @=> UGen arr[];

<<< arr.size() >>>;

<<< arr[0] >>>;
<<< arr[1] >>>;
<<< arr[2] >>>;
<<< arr[3] >>>;
<<< g >>>;
<<< dac >>>;
<<< blackhole >>>;

about(Type.of(arr[0]));
about(Type.of(arr[1]));
about(Type.of(arr[2]));
about(Type.of(arr[3]));

<<< Type.of(dac).name() >>>;


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
          <= "Gain:" <= t.isa("Gain") <= " "
          <= "DAC:" <= t.isa("DAC") <= " "                    
          <= "UAna:" <= t.isa("UAna") <= IO.newline();

    //-----------------------------------------------------------------
    // more attributes
    //-----------------------------------------------------------------
    cherr <= "primitive: " <= (t.isPrimitive() ? "YES" : "NO" ) <= IO.newline();
    cherr <= "array: " <= (t.isArray() ? "YES" : "NO" )
          <= " depth: " <= t.arrayDepth() <= IO.newline();
}


// s.help();

<<< "success" >>>;