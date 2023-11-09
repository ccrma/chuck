[1] @=> int arr_prim[];

if(!Type.of(arr_prim).isArray()) me.exit();

vec2 arr_vec2[0];
if(!Type.of(arr_vec2).isArray()) me.exit();

SinOsc arr_sin[0];
if(!Type.of(arr_sin).isArray()) me.exit();


if(!Type.find("int[]").isArray()) me.exit();
if(!Type.find("vec2[][][][]").isArray()) me.exit();
if(!Type.find("SinOsc[][]").isArray()) me.exit();
if(Type.find("float").isArray()) me.exit;

<<< "success" >>>;