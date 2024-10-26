[1] @=> int arr_prim[];

if(!Type.of(arr_prim).isArray()) me.exit();

vec2 arr_vec2[0];
if(!Type.of(arr_vec2).isArray()) me.exit();

SinOsc arr_sin[0];
if(!Type.of(arr_sin).isArray()) me.exit();

// test Type.find() with arrays
if(!Type.find("int[]").isArray()) me.exit();
if(!Type.find("vec2[][][][]").isArray()) me.exit();
if(!Type.find("SinOsc[][]").isArray()) me.exit();
if(Type.find("float").isArray()) me.exit;

// test array type names
[ "a", "b", "c"] @=> string arr_s1[];
if(arr_s1.typeOf().name() != "string[]") me.exit();
// note: the follow would use to return more generic "@array"
// as of 1.5.4.0, Type.of() returns more specific type names
if(Type.of(arr_s1).name() != "string[]") me.exit();

// 2D arrays
[ ["a"], ["b"] ] @=> string arr_s2[][];
if(arr_s2.typeOf().name() != "string[][]") me.exit();
if(Type.of(arr_s2).name() != "string[][]") me.exit();

// if we got here, then...
<<< "success" >>>;
