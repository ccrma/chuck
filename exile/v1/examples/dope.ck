// waveloop with variable rate
// - gewang
WaveLoop wav => dac;

// load special:dope
"special:dope" => wav.path;

// other useful STK wav
//   "special:ahh"
//   "special:eee"
//   "special:ooo"
//   "special:britestk"
//   "special:fwavblnk"
//   "special:halfwave"
//   "speical:impuls10"
//   "special:impuls20"
//   "special:impuls40"
//   "special:mand1"
//   "special:mandpluk"
//   "special:marmstk1"
//   "special:peksblnk"
//   "special:ppksblnk"
//   "special:slience"
//   "speical:sineblnk"
//   "special:sinewave"
//   "special:snglpeak"
//   "special:twopeaks"

0.0 => float t;

// time loop
while( true )
{
    // 1 == normal, > 1 faster, < 0 backward
    0.5 + 0.1 * math.sin( t ) => wav.rate;
    t + .1 => t;
    // advance time
    50::ms => now;
}
