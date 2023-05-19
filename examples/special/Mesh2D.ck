///////////////////////////////////////////////////////////////////////////////
// Mesh2D: Two-dimensional rectilinear waveguide mesh class.                 //
//                                                                           //
// Part of the Synthesis Toolkit in C++                                      //
// https://ccrma.stanford.edu/software/stk/                                  //
//                                                                           //
// This class implements a rectilinear, two-dimensional digital waveguide    //
// mesh structure. For details, see Van Duyne and Smith, "Physical Modeling  //
// with the 2-D Digital Waveguide Mesh", Proceedings of the 1993             //
// International Computer Music Conference.                                  //
//                                                                           //
// This is a digital waveguide model, making its use possibly subject to     //
// patents held by Stanford University, Yamaha, and others.                  //
///////////////////////////////////////////////////////////////////////////////

// Options:
//
// x: X dimension size in samples. int [2-12], default 5
// y: Y dimension size in samples. int [2-12], default 4
// xpos: x strike position. float [0-1], default 0.5
// ypos: y strike position. float [0-1], default 0.5
// decay: decay factor. float [0-1], default 0.999

Mesh2D mesh1 => dac.left;
Mesh2D mesh2 => dac.right;
0.5 => mesh1.gain => mesh2.gain;

while (true)
{
	Math.random2(2,12) => mesh1.x;
	Math.random2(2,12) => mesh1.y;
	Math.randomf() => mesh1.xpos;
	Math.randomf() => mesh1.ypos;
	1 => mesh1.noteOn;
	250::ms => now;
	Math.random2(2,12) => mesh2.x;
	Math.random2(2,12) => mesh2.y;
	Math.randomf() => mesh2.xpos;
	Math.randomf() => mesh2.ypos;
	1 => mesh2.noteOn;
	250::ms => now;
	
}