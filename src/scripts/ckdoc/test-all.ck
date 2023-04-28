// an array of Types
Type types[0];
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

// instantiate
CKDoc doc; // documentation orchestra

// set the examples root
"https://chuck.stanford.edu/doc/examples/" => doc.examplesRoot;

// add as group
doc.addGroup( types, "Base Types (compiled in)", "base", "Base types in the ChucK" );

types.clear();
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

// add as group
doc.addGroup( types, "Base Types (chugins)", "chugins", "ChucK Objects loaded from chugins" );

// generate
doc.outputToDir( ".", "ChucK Class Library Reference" );
