Hid hi;
HidMsg msg;

if( !hi.openTiltSensor() )
{
    <<< "tilt sensor unavailable", "" >>>;
    me.exit();
}

<<< "tilt sensor ready", "" >>>;

while( true )
{
    hi.read( Hid.ACCELEROMETER, 0, msg );
    
    // x: positive to left of user, negative to right of user
    // y: positive towards user, negative away from user
    // z: positive up, negative down
    <<< "acc x:", msg.x, "acc y:", msg.y, "acc z:", msg.z >>>;
    
    // theta: positive in counter-clockwise rotation in x-z plane
    // phi: positive rotation clockwise towards user y-z plane
    
    <<< "theta:", Math.atan2( msg.y, msg.z ) / pi, 
        "phi:", Math.atan2( Math.sqrt( msg.z * msg.z + msg.y * msg.y ), -msg.x ) / pi >>>;
    
    .25::second => now;
}
