// name: spherical-harmonics.ck
// desc: explanation of Math.sh() 
// author: everett m. carpenter
// date: spring 2026

// three arguments must be given to Math.sh()
3 => int order; // analogous to block size in the FFT, this determines how many projections upon bases we are calculating
23.0 => float azimuth; // horizontal angle in degrees (counter clockwise is positive)
0.0 => float elevation; // vertical angle in degrees (positive is upwards, 90 is north pole, negative downwards, 0 is the equator)

// Math.sh() will return an array of size (order+1)^2
Math.sh(order, azimuth, elevation) @=> float projections[];

for(int acn; acn < projections.size(); acn++)
{
    // due to Math.sh() being writting with ambisonics in mind, projections are returned in Ambisonic Channel Order (ACN)
    chout <= "ACN: " <= acn;
    // but the order and degree of a projection are calculable from ACN
    Math.floor(Math.sqrt(acn)) $ int => int order;
    acn - order * order - order => int degree;

    // print
    chout <= " Order: " <= order <= " Degree: " <= degree <= " Value: " <= projections[acn] <= IO.nl();
}