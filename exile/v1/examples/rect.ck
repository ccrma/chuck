// mic to full rectifier to dac
adc => fullrect h => dac;

// infinite time-loop
while( true )
{
    100::ms => now;
}
