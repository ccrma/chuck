fun vec3 hasReturn()
{
    return @(1,0,0);
}

//fun vec3 missingReturn()
//{
// missing return
//}

// missing return statements default to 0 as appropriate
// for return type; this tests that and that the stack
// is okay afterwards, even for larger-than-int return types
// like vec3
// <<< hasReturn(), missingReturn() >>>;

// before the fix in 1.5.0.0, the above would incorrectly print
// #(0.0000,0.0000,1.0000) #(0.0000,0.0000,0.0000)
// this implies the stack pointer is off, due to missingReturn
// not accounting for the size of its return type, vec3

// as of 1.5.0.9, non-void functions are required to return value
<<< hasReturn() >>>;
