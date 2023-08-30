SinOsc foo => dac;
.25 => foo.gain;

// test audio clicks when we Machine.add
.5::second => now;

// test Machine.add, with arguments
Machine.add( me.dir() + "193-machine-add-this.ck.disabled:foo:1:2" );

2::second => now;
