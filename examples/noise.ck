// white noise to audio output - cover your ears
noise n => dac;

// go!
until( false ) { 100::ms => now; }
