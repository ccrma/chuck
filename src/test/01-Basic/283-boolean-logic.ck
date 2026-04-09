// just to be sure...
// requires chuck-1.5.5.8

// previous this did not work due to how logical AND and OR
// were optimized for early termination; what was left on the
// stack was not limited to 0 or 1, but could take on the value
// of the RHS; this in practice "works" except in cases such
// as this:
if( (2 && 3) == true )
{
    <<< "success" >>>;
}