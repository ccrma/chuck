// name: ambisonic-encoding.ck
// desc: utilizes an array of Gain to encode a virtual source in nth order ambisonics
// author: everett m. carpenter
// date: spring 2026

// NOTE: various orientations exist across ambisonic literature, Math.sh is implemented
//       so that an azimuth of 0 is directly infront of the listener, and increasing azimuth
//       will move the projection counterclockwise. additionally, an elevation of 0 is located
//       on the horizontal plane, -90 degrees equates to the south pole, and 90 degrees the north.

// let's do second order (Math.sh() supports up to 5th)
2 => int order;
0.0 => float azimuth; 
0.0 => float elevation;

// nth order ambisonics requires (n+1)^2 channels for three dimensional encoding
SinOsc sing(352.0) => Gain encoder[(order+1)*(order+1)] => blackhole;

while(true)
{
    Math.sh(order, azimuth, elevation) @=> float sphericalHarmonics[];

    // Math.sh() will return a (n+1)^2 sized array, one value for the projection upon each harmonic
    for(int i; i < sphericalHarmonics.size(); i++)
    {
        sphericalHarmonics[i] => encoder[i].gain;
    }

    // rotate horizontally
    15.0 +=> azimuth;

    // wait some time
    500::ms => now;
}