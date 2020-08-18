// name: use-case.cpp
// desc: ideal use to instantiate and use ChucK as library


// include ChucK lib header
#include "chuck.h"

// instantiate a chuck instance
// -- should be possible to create multiple instances
// -- contains VM, compiler, and state
// -- does not contain audio I/O specific code
ChucK g_chuck;

// audio callback (outside of lib)
void cb( t_CKSAMPLE * in, t_CKSAMPLE * out, int numFrames )
{
    // call up to ChucK
    g_chuck.run( in, out, numFrames );
}

int main( int argc, char ** argv )
{
    // initialize
    
    
    return 0;
}
