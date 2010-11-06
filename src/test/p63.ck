// spork spork spork

fun void wait5() { 
	5::second => now;
	<<<"five">>>;
	<<<"success">>>;
}

fun void wait10() { 
	10::second => now;
	<<<"ten">>>;
	<<<"success">>>;
}

spork ~ wait5();
spork ~ wait10();
2::second => now;

<<<"success">>>;