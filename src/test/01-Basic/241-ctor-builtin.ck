// constructors
SinOsc foo(440) => Gain g(.5) => dac;

// test
if( Math.equal(g.gain(),.5) )
    <<< "success" >>>;
