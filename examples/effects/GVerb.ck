///////////////////////////////////////////////////////////////////////////////
// GVERB is a very smooth reverberator with the ability to produce very long //
// reverb times.                                                             //
//                                                                           //
// GVERB is based on the original "gverb/gigaverb" by Juhana Sadeharju       //
// (kouhia at nic.funet.fi). The code for this version was adapted from      //
// RTcmix (http://rtcmix.org), which in turn adapted it from the Max/MSP     //
// version by Olaf Mtthes (olaf.matthes at gmx.de).                          //
///////////////////////////////////////////////////////////////////////////////

// Options
// roomsize: (float) [1.0 - 300.0], default 30.0
// revtime: (dur), default 5::second
// damping: (float), [0.0 - 1.0], default 0.8
// spread: (float), default 15.0
// inputbandwidth: (float) [0.0 - 1.0], default 0.5
// dry (float) [0.0 - 1.0], default 0.6
// early (float) [0.0 - 1.0], default 0.4
// tail (float) [0.0 - 1.0], default 0.5

adc => GVerb gverb => dac;
50 => gverb.roomsize;
2::second => gverb.revtime;
0.3 => gverb.dry;
0.2 => gverb.early;
0.5 => gverb.tail;

while (true)
{
  second => now;
}
