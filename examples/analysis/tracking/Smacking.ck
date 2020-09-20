//--
// name: Smacking.ck
// desc: class and process for tracking smack events from sudden motion sensor
//       only works with SMS-enabled laptops
//
// author: Rebecca Fiebrink + Ge Wang
// date: 2007
//--


// public class
public class Smacking
{
    static Event @ the_event;
}

// initialize separately (due to a bug)
new Event @=> Smacking.the_event;

// smack detection threshold (less -> more sensitive)
15 => int threshold;
// pol rate
.01::second => dur poll_rate;
// recover time (after smack detection)
.10::second => dur recover_time;

// instantiate a Hid object
Hid hi;
HidMsg msg;

// open tilt sensor
if( !hi.openTiltSensor() )
{
    <<< "tilt sensor unavailable", "" >>>;
    me.exit();
}

// "doh"
<<< "hit me!", "" >>>;

// remember
HidMsg last;
// initialize
hi.read( 9, 0, last );

// infinite event loop
while( true )
{
    // poll the tilt sensor, expect to get back 3 element array of ints
    hi.read( 9, 0, msg );
    
    // compute difference
    (Math.abs(msg.x - last.x) + Math.abs(msg.y - last.y) + Math.abs(msg.z - last.z)) => int diff;
    // check threshold
    if (diff > threshold) {
        // smack detected!
        // <<< "ouch!" >>>;
        // fire!
        Smacking.the_event.broadcast();
        // go
        recover_time => now;
    } else {
        // uncomment the following to dynamically calibrate the smack sensor
        // msg.x => last.x;    	    	
        // msg.y => last.y;
        // msg.z => last.z;
        
        // advance
        poll_rate => now;
    }
}
