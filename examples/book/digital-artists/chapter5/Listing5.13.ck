// Listing 5.13 Using the arrayAdder() function to convert a scale from minor to major

// make a mandolin and hook it to audio out
Mandolin mand => dac;        // (1) Mandolin instrument.

// global scale array        // (2) Scale array of note numbers.
[60,62,63,65,67,69,70,72] @=> int scale[];

// function to modify an element of an array
fun void arrayAdder( int temp[], int index)  
{                            // (3) arrayAdder function definition.
    1 +=> temp[index];
}

//play scale on mandolin
fun void playScale(int temp[]) { // (4) playScale function definition.
    for (0 => int i; i < temp.cap(); i++)
    {
        Std.mtof(temp[i]) => mand.freq;
        <<< i, temp[i] >>>;
        1 => mand.noteOn;
        0.4 :: second => now;
    }
    second => now;
}

// play our scale on our mandolin
<<< "Original Scale" >>>;
playScale(scale);    // (5) Tests playScale.

// modify our scale
arrayAdder(scale,2); // (6) Call arrayAdder to change two elements.
arrayAdder(scale,6);

// play scale again, sounds different, huh?
<<< "Modified Scale:" >>>;
playScale(scale);    // (7) When we call playScale again. It's different!
