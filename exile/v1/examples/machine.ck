// on-the-fly shred management

while( true )
{
    machine.add( "moe.ck" ) => int moe;
    500::ms => now;
    machine.replace( moe, "wind.ck" ) => int wind;
    500::ms => now;
    machine.remove( wind );
}

