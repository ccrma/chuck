//  Listing 9.22 initialize.ck adds MandoPlayer.ck class and then MandoScore.ck score
// initialize.ck for MandoPlayer class and demo MandoScore
Machine.add(me.dir()+"/MandoPlayer.ck");  // (1) Adds MandoPlayer class definition...
Machine.add(me.dir()+"/MandoScore.ck");   // (2) ...then adds score, which does the rest.


