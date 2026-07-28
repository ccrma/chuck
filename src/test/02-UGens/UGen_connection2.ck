SinOsc s => dac;

s.goesTo() @=> UGen arr[];

// This is two because dac is left and right?
<<< arr.size() >>>;

<<< arr[0] >>>;
<<< arr[1] >>>;
<<< dac.chan(0) >>>;
<<< dac.chan(1) >>>;