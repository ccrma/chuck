// Listing 5.12 Functions on arrays

// (1) global note array
[60,62,63,65,67,69,70,72] @=> int scale[];
// function to modify an element of an array
fun void arrayAdder( int temp[], int index)
{                           // (2) arrayAdder function to modify it
    1 +=> temp[index];
}

// test it all out
<<< scale[0], scale[1], scale[2], scale[3] >>>;

arrayAdder(scale, 2);
<<< scale[0], scale[1], scale[2], scale[3] >>>;
<<< "scale[6] = ", scale[6] >>>;

arrayAdder(scale, 6);
<<< "scale[6] = ", scale[6] >>>;
