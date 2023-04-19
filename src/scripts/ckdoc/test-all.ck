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
CKDoc doc;

// set destination
doc.outputDir(".");
// generate CSS
doc.genCSS( types, "ckdoc.css" );
// generate HTML group
doc.genHTMLGroup( types, "All", "all.html", "ckdoc.css" );

// generate HTML index
// doc.genHTMLIndex( types, "index.html", "ckdoc.css" );
