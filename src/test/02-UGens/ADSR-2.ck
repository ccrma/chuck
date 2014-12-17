ADSR u => blackhole;

if( u.state() != ADSR.DONE ) { <<< "failure1" >>>; me.exit(); }

u.keyOn();
if( u.state() != ADSR.ATTACK ) { <<< "failure2" >>>; me.exit(); }

u.attackTime()+1::samp => now;
if( u.state() != ADSR.DECAY ) { <<< "failure3" >>>; me.exit(); }

u.decayTime()+1::samp => now;
if( u.state() != ADSR.SUSTAIN ) { <<< "failure4" >>>; me.exit(); }

u.keyOff();
if( u.state() != ADSR.RELEASE ) { <<< "failure5" >>>; me.exit(); }

u.decayTime()+1::samp => now;
if( u.state() != ADSR.DONE ) { <<< "failure6" >>>; me.exit(); }

u =< blackhole;
null @=> u;

<<< "success" >>>;
