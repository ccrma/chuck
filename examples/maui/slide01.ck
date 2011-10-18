MAUI_Slider slider;
"param" => slider.name;
slider.display();
slider.range( 1, 127 );
60 => slider.value;

Blit s => dac;
4 => s.harmonics;

while( true )
{
    slider.value() => Std.mtof => s.freq;
    10::ms => now;
}
