// name: net relay - audio over UDP
// desc: the netin ugen takes audio on UDP
//       and sends to out to another location
//       with an added delay

// relay with delay
netin nin => Delay d => netout nout => blackhole;
// set the delay
100::ms => d.max => d.delay;
// set the incoming port
8890 => nin.port;
// set the outgoing destination
"nowhere.com" => nout.addr;
8890 => nout.port;

// infinite time loop
while( true ) { 100::ms => now; }
