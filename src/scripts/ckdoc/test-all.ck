// instantiate
CKDoc doc; // documentation orchestra
// set the examples root
"https://chuck.stanford.edu/doc/examples/" => doc.examplesRoot;

// get types currently in ChucK type system
Type.getTypes( Type.OBJECT, Type.BUILTIN ) @=> Type types[];
// add as group
doc.addGroup( types, "Base Types (compiled in)", "base", "Base types in the ChucK" );

// clear
types.clear();

// get types currently in ChucK type system
Type.getTypes( Type.OBJECT, Type.CHUGIN ) @=> types;
// add as group
doc.addGroup( types, "Base Types (chugins)", "chugins",
              "ChucK Objects loaded from chugins" );

// generate
doc.outputToDir( ".", "ChucK Class Library Reference" );
