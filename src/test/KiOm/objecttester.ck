KiOm k;
k.initPort(0);
k.initDcTime(300::ms);
k.initGate(1);

spork ~ k.poller();
spork ~ k.b1timer();
spork ~ k.b2timer();

k.calibrate();
while(true){
	100::ms => now;
  <<< k.x(), k.y(), k.z() >>>;
}
