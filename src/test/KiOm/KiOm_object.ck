//KiOm_object.ck 
//Adam R. Tindale, Graham Percival

// see objecttester.ck for usage

public class KiOm{

	MidiIn min;
	MidiMsg msg;
//	min.open(0);
	
	// stores the current val of xyz acceleromenter
	int xyz[4];
	0 => xyz[1] => xyz[2] => xyz[3];

	// other variables for the fancy stuff
	Event b1Timer;
	Event b2Timer;
	Event KiOm_x;
	Event KiOm_y;
	Event KiOm_z;
	Event KiOm_button1;
	Event KiOm_button2;
	Event KiOm_button1_dc;
	Event KiOm_button2_dc;

	dur b1dur;
	dur b2dur;

	300::ms => dur doubleclick;
	0 => int gate;

	// calibration data -- adjusts for gravity
	int cal[4];
	0 => cal[1] => cal[2] => cal[3];

	fun void initPort(int p){
		min.open(p);
	}

	fun void initDcTime(dur d){
		d => doubleclick;
	}

	fun void initGate(int g){
		g => gate;
	}
	
	fun void init(){
		spork ~ poller();
		spork ~ b1timer();
		spork ~ b2timer();
	}

	fun void calibrate(){
// calibration logic was derived and tested using KiOm F.
// There is a rumor that this KiOm simply outputs random
// numbers, so if this doesn't work, I'm blaming Ajay.  :)

// Even disregarding the almost-dead battery on this KiOm,
// it isn't one of the new "always-output-value" KiOms,
// so this code sometimes doesn't work.  It should be
// great on the improved KiOms, though.  -gp
		int calcount[4];
		min => now;
		while(min.recv(msg))
			if (msg.data1 == 176) {
				// record difference between 64 (no accel) and
				// actual accel.  KiOm should be at rest.
				64 - msg.data3 +=> cal[msg.data2];
				calcount[msg.data2]++;
			}
		// find average accel reported by KiOm while at rest.
		for (1=>int calx; calx<=3; calx++)
			if (calcount[calx]>0)
				cal[calx]/calcount[calx] => cal[calx];
// Will remove this soon, but I figured it might be
// interesting to see.
<<< "Calibration: ", cal[1], cal[2], cal[3] >>>;
	}

	fun int x(){
		return xyz[1];
	}
	fun int y(){
		return xyz[2];
	}
	fun int z(){
		return xyz[3];
	}

	fun void b1timer(){

		dur lastclick;
		time beg;
		time end;
		while (true){
			
			//find the current time
			now => beg;
			
			// wait for the stop signal
			b1Timer => now;
			
			// get the current time after the signal
			now => end;
			
			// find the elapsed time and store it
			end - beg => lastclick;

			lastclick => b1dur;
		}
	}
	fun void b2timer(){

		dur lastclick;
		time beg;
		time end;
		while (true){
			now => beg;
			b2Timer=> now;
			now => end;
			end - beg => lastclick;
			lastclick => b2dur;
		}
	}	

	fun void poller(){
		1 => int mybool; 
		0 => int b1last;

		while(true){
			
			min => now;
			while( min.recv(msg) ) 
			{
				// buttons
				if (msg.data1 == 144)
				{
				          //button 1
					  if (msg.data2 == 1)
					{
						KiOm_button1.broadcast();
						
						b1Timer.signal();
						me.yield();

						if (b1dur < doubleclick && b1last == 1){
						//	<<<"doubleclick">>>;
							2 => b1last; 
						}
						else
						{	
						//	<<<"singleclick">>>;
							1 => b1last;
						}
						continue;
					}
					// button 2 freezes the current state
// only button1 does double clicks?  Is this deliberate?  -gp
					if (msg.data2 == 2)
					{
						KiOm_button1.broadcast();

						b2Timer.signal();
						// <<<"b2">>>;
					}

				}// end of buttons
				
				//accelerometers
				if (msg.data1 == 176){
					if (gate==1)
						if ((msg.data3 + cal[msg.data2] <= xyz[msg.data2]+1) &&
							(msg.data3 + cal[msg.data2] >= xyz[msg.data2]-1))
							continue;
					// stores vals in xyz, subject to calibration.
					// if no calibration has occured, cal[x] will be 0
					// and will thus have no effect.
					msg.data3 + cal[msg.data2] => xyz[msg.data2];
					//throw some events
					if(msg.data2 ==1){
						KiOm_x.broadcast();	
						continue;
					}
					if(msg.data2 ==2){
						KiOm_y.broadcast();
						continue;
					}
					if(msg.data2 ==3){
						KiOm_z.broadcast();	
					}
				}
			}
		}
	}
}
