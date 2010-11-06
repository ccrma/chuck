KiOm k;
k.initPort(9);
k.initDcTime(300::ms);
k.calibrate();
float pos[3];

spork ~ k.poller();
spork ~ k.b1timer();
spork ~ k.b2timer();

fun void movex()
{
  time beg;
// whenever I see "beg", I read it as "beg", not "beginning".
// so I obviously can't use "end" for the alternate of "beg".  :)
  time plead;
	now => beg;
	float dist;
	float dt;
  0=>float accel;
float sum;
k.xf() => accel;
float newaccel;

	while(true)
	{
// distance = 1/2 * a * t^2
		while ((newaccel>(accel-1.2))&&(newaccel<(accel+1.2)))
		{
			k.KiOm_x => now;
			k.xf() - 64.0=> newaccel;
		}
		now => plead;
		(beg-plead) / (1::ms) => dt;
		k.xf() - 64.0 => accel;
		dist + accel/2.0*dt*dt => dist;
		now => beg;
//<<< dist >>>;
accel +=>sum;
<<< accel,sum >>>;
	}
}

spork ~ movex();

while(true){
	1000::ms => now;
}
