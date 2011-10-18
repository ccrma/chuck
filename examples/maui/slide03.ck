MAUI_Slider freq;
"midi note" => freq.name;
freq.display();
freq.range( 1, 127 );

Blit s => dac;
4 => s.harmonics;
freq.value() => Std.mtof => s.freq;

while( true )
{
    // wait on change
    freq => now;
    // control
    freq.value() => Std.mtof => s.freq;
}
