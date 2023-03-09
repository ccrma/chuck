//---------------------------------------------------------------------
// name: type-query.ck
// desc: this example shows working with the Type type in ChucK
//       to query Types currently in the ChucK type system
//
// version: needs 1.4.2.1 or higher
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// instantiate an array of Type refs
Type @ types[0];

// get types currently in ChucK type system
Type.getTypes(
    types, // to be filled with results
    false, // include objects
    true, // include primitive types
    false, // include special types (@array and @function)
    true, // include builtin types
    false, // include types from chugins
    false, // include types from ck files in lib path
    false // include user-defined classes
);

// print results of query
for( int i; i < types.size(); i++ )
{
    cherr <= "(builtin primitive types): " <= types[i].name() <= IO.newline();
}

// get types currently in ChucK type system
Type.getTypes(
    types, // to be filled with results
    true, // include objects
    false, // include primitive types
    false, // include special types (@array and @function)
    true, // include builtin types
    false, // include types from chugins
    false, // include types from ck files in lib path
    false // include user-defined classes
);

// print results of query
for( int i; i < types.size(); i++ )
{
    cherr <= "(builtin object types): " <= types[i].name() <= IO.newline();
}

// get types currently in ChucK type system
Type.getTypes(
    types, // to be filled with results
    true, // include objects
    false, // include primitive types
    false, // include special types (@array and @function)
    false, // include builtin types
    true, // include types from chugins
    false, // include types from ck files in lib path
    false // include user-defined classes
);

// print results of query
for( int i; i < types.size(); i++ )
{
    cherr <= "(chugin types): " <= types[i].name() <= IO.newline();
}

// get all subclasses of UGen
UGen.typeOf().children( types );
// print results
for( int i; i < types.size(); i++ )
{
    cherr <= "(UGens): " <= types[i].name() <= IO.newline();
}
