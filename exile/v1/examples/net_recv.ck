// net_recv.ck
// audio over UDP
// see net_send.ck and net_relay.ck

// netin to dac
netin n => dac;
// set the UDP port to listen
8890 => n.port;

// infinite time loop
while(true) { 100::ms => now; }
