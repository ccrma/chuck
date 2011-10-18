SinOsc s => blackhole;
.125 => s.freq;

MAUI_Gauge gauge;
gauge.display();

while( true )
{
    ( s.last() + 1.0 ) / 2.0 => gauge.value;
	20::ms => now;
}
