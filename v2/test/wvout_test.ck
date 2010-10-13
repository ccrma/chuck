
impulse i => WvOut w => blackhole;
	i => dac;

"special:auto" => w.aifFilename;
<<<"fetch">>>;
<<<w.filename()>>>;
<<<"assignment">>>;
w.filename() => string doop;
<<<"doop">>>;
// time loop
while( true )
{
	1.0 => i.next;
	2::second => now;
}
