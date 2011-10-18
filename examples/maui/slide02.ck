// instantiate slider
MAUI_Slider freq;
"midi note" => freq.name;
freq.display();
freq.irange( 1, 127 );

// instantiate 2nd slider
MAUI_Slider harm;
"harmonics" => harm.name;
harm.display();
harm.irange( 1, 40 );

// our patch
Blit s => dac;
int last;

// infinite time loop
while( true )
{
    freq.value() => Std.mtof => s.freq;
    if( harm.value() != last )
        harm.value() $ int => last => s.harmonics;
    10::ms => now;
}
