// test speedy trig functions
0.01 => float inc;

// our error bound (for testing float equality)
0.000001 => float epsilon;

-1 * (Math.PI / 2) => float i;
Math.PI / 2 => float dest;

while (i < dest) {
    testEquality(i, Math.sin(i), Math.ssin(i), "sin");
    inc +=> i;
}

-1 * (Math.PI / 2) => i;
Math.PI / 2 => dest;

while (i < dest) {
    testEquality(i, Math.cos(i), Math.scos(i), "cos");
    inc +=> i;
}

-1 * (Math.PI / 2) + 0.2 => i;
Math.PI / 2 - 0.1 => dest;

while (i < dest) {
    testEquality(i, Math.tan(i), Math.stan(i), "tan");
    inc +=> i;
}

-1 * (Math.PI / 2) => i;
Math.PI / 2 => dest;

while (i < dest) {
    testEquality(i, Math.sinh(i), Math.ssinh(i), "sinh");
    inc +=> i;
}


-1 * (Math.PI / 2) => i;
Math.PI / 2 => dest;

while (i < dest) {
    testEquality(i, Math.cosh(i), Math.scosh(i), "cosh");
    inc +=> i;
}

-1 * (Math.PI / 2) => i;
Math.PI / 2 => dest;

while (i < dest) {
    testEquality(i, Math.tanh(i), Math.stanh(i), "tanh");
    inc +=> i;
}

-1 * (Math.PI / 2) => i;
Math.PI / 2 - 0.35 => dest;

while (i < dest) {
    testEquality(i, Math.exp(i), Math.sexp(i), "exp");
    inc +=> i;
}

<<< "success" >>>;

// test equality
fun int testEquality(float input, float x, float y, string fname)
{
    if( !fequal(x,y,epsilon) )
    // if( !Math.equal(x,y,epsilon) )
    {
       cherr <= "ERROR: speed approximation of " <= fname <= " are too different at input "
             <= input <= ". Expected "
	     <= x <= ", but got " <= y <= IO.newline();
       return false;
    }
    return true;
}

// custom float approximate equality test
fun int fequal(float x, float y, float epsilon)
{
    Math.fabs(x) => x;
    Math.fabs(y) => y;
    x-y => Math.fabs => float diff;
    return diff < epsilon;
}
