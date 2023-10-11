// dot member
// related: https://github.com/ccrma/chuck/issues/378

// a vec4
@(1,2,3,4) => vec4 v4;

fun vec4 r()
{
    // assign fields in v4
    5 => v4.x;
    6 => v4.y;
    7 => v4.z;
    1 +=> v4.w;

    // return
    return v4;
}

// do this a lot (used to stack overflow)
for( int i; i < 100000; i++ )
{
    // call and access a field (make sure this cleans up properly)
    r().x;
}

<<< r().x, r().y, r().z, r().w >>>;
