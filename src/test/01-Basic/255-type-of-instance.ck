SinOsc foo @=> UGen @ bar;

// static typing
if( foo.typeOf().name() != "SinOsc" && bar.typeOf().name() != "UGen" )
{ <<< "error" >>>; me.exit(); }

// get the instanced type
if( foo.typeOfInstance().name() == "SinOsc" && 
    bar.typeOfInstance().name() == "SinOsc" ) <<< "success" >>>;
