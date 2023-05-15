//---------------------------------------------------------------------
// name: type-query.ck
// desc: this example shows working with the Type type in ChucK
//       to query Types currently in the ChucK type system
//
// version: needs 1.5.0.0 or higher
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// get types currently in ChucK type system
Type.getTypes(
    Type.PRIMITIVE, Type.BUILTIN
) @=> Type types[];

// print results of query
for( int i; i < types.size(); i++ )
{
    cherr <= "(builtin primitive types): " <= types[i].name() <= " (" <= types[i].origin() <= ")" <= IO.newline();
}

// get types currently in ChucK type system
Type.getTypes(
    Type.OBJECT, Type.BUILTIN
) @=> types;

// print results of query
for( int i; i < types.size(); i++ )
{
    cherr <= "(builtin object types): " <= types[i].name() <= " (" <= types[i].origin() <= ")" <= IO.newline();
}

// get types currently in ChucK type system
Type.getTypes(
    Type.OBJECT, Type.CHUGIN
) @=> types;

// print results of query
for( int i; i < types.size(); i++ )
{
    cherr <= "(chugin types): " <= types[i].name() <= " (" <= types[i].origin() <= ")" <= IO.newline();
}

// get all subclasses of UGen
UGen.typeOf().children() @=> types;
// print results
for( int i; i < types.size(); i++ )
{
    cherr <= "(UGens): " <= types[i].name() <= " (" <= types[i].origin() <= ")" <= IO.newline();
}
