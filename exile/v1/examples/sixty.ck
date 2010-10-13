// oscillator demo
// - philipd

// control oscillators - send to blackhole
sinosc modw => blackhole;
phasor x => blackhole; 

0.125 => modw.freq;
60.0 => x.freq;

// audio signals
x => sinosc s    => Envelope se => dac;
x => triosc t    => Envelope te => dac; 
t => sinosc ts   => Envelope tse => dac;
ts => triosc tst => Envelope tste => dac;
x => pulseosc p  => Envelope pe => dac;
x => sawosc w    => Envelope we => dac;
x => sqrosc q    => Envelope qe => dac;

// oscillator sync mode
2 => s.sync;     // 2 = sync to input
2 => t.sync;
2 => ts.sync;
2 => tst.sync;
2 => p.sync;
2 => w.sync;
2 => q.sync;

// pulse width
.25 => p.width;

// envelope time
0.1 => se.time => te.time => tse.time => tste.time => 
       pe.time => we.time => qe.time;

// pulse width modulation
fun void vary()
{
    while ( true )
    {
        modw.last => t.width;
        modw.last => p.width;
        modw.last => w.width;
        1::ms => now;
    }
}

// go forth...
spork ~vary();

// alternate the envelopes
0 => int c; 
while ( true )
{
    if ( c % 7 == 0 ) { 1.0 => se.target; 0.0 => qe.target;}
    if ( c % 7 == 1 ) { 1.0 => te.target; 0.0 => se.target;}
    if ( c % 7 == 2 ) { 1.0 => tse.target; 0.0 => te.target;} 
    if ( c % 7 == 3 ) { 1.0 => tste.target; 0.0 => tse.target;} 
    if ( c % 7 == 4 ) { 1.0 => pe.target; 0.0 => tste.target;}
    if ( c % 7 == 5 ) { 1.0 => we.target; 0.0 => pe.target;}
    if ( c % 7 == 6 ) { 1.0 => qe.target; 0.0 => we.target;}
    c++;
    1::second => now;	
}
