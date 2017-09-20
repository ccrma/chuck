// Listing 3.3 Accessing (reading and writing) data in an array

// declare and initialize an array
[57, 57, 64, 64, 66, 66, 64] @=> int a[];

// array look up by index
a[2] => int myNote;   // (1) Looks up note in array by integer index

// print it out to check
<<< myNote >>>;       // (2) Prints it

// want to change data? no problem! (print too)
61 => a[2];           // (3) Changes array element value at index
<<< myNote, a[2] >>>;
