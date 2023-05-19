// FIRConvolveHomer.ck by Perry R. Cook, 10/12
// Use FIR to convolve mic input with "dope!"

adc => Gain g => FIR imp => dac;
SndBuf s => dac; 
"special:dope" => s.read;

s.samples() => imp.order;
for (0 => int i; i < imp.order(); i++) {
    imp.coeff(i,s.last());
    1.0 :: samp => now;
}

5.0 :: second => now;


