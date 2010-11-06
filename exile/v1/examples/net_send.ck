// net_send.ck
// audio over UDP
// see net_recv.ck and net_relay.ck

// set up the patch - blackhole is for driving the netout
sinosc s => netout n => blackhole;
// set the oscillator frequency
220.0 => s.freq;
// replace the following with the destination
"127.0.0.1" => n.addr => stdout;

// infinite time loop
while( true ) { 100::ms => now; }

