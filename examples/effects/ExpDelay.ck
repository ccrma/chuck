////////////////////////////////////////////////////
// ExpDelay - Feedback delay at exponentially     //
//            changing delay times                //
//                                                //
// by Joel Matthys                                //
// (c) 2014, GPL 2.0                              //
//                                                //
////////////////////////////////////////////////////
//
// Settings:
// mix (float): 0-1, dry/wet mix
// max (dur): maximum possible delay duration
// delay (dur): duration of delay
// reps (int): number of repetitions
// durcurve (float) [0.0001-inf]: set steepness of delay curve
//       1 = steady
//       <1 = starts fast and slows down
//       >1 = starts slow and speeds up 
// ampcurve (float) [0.0001-inf]: set steepness of amplitude decay

ModalBar pop => ExpDelay ed => dac;
3::second => ed.max;
3::second => ed.delay;

while (true)
{
  Math.random2(54,66) => Std.mtof => pop.freq;
  1 => pop.noteOn;
  Math.random2f(0.5,2) => ed.durcurve;
  Math.random2f(0.5,2) => ed.ampcurve;
  Math.random2(5,30) => ed.reps;
  <<< "durcurve:",ed.durcurve(), "ampcurve:",ed.ampcurve(), "reps:",ed.reps() >>>;
  4::second => now;
}
