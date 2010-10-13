// wait too long

<<<"sleepy">>>;
for ( 0 => int i; i < 13; 1 +=> i ) { 
	<<<i*5>>>;
	5::second => now;
}

<<<"success">>>;
