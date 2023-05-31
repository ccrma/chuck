// FIR filter demo program, Perry R. Cook, Oct 2012
// This shows how to load filter coefficients
// explicitly, in this case with a Sinc lowpass filter

Noise n => Gain g => FIR f => WvOut w => dac;
w.wavFilename("temp.wav");

127 => f.order;
f.coeff(64,1.0);
for (1 => int i; i < 63; i++)  {
    (i * pi)/4.0 => float temp;
    f.coeff(64-i,Math.cos(i*pi/128)*(Math.sin(temp)/temp));
    <<< i, f.coeff(64+i,f.coeff(64-i)) >>>;
}

0.2 => g.gain;

5.0 :: second => now;

w.closeFile();
