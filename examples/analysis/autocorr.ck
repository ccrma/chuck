// autocorr.ck: finding the pitch of a note
// author: Curtis Ullerich

SinOsc s => Flip flip =^ AutoCorr corr => blackhole;
1024 => flip.size;
true => corr.normalize; // output in [-1, 1]
second/samp => float srate;

fun float estimatePitch() {
  corr.upchuck();
  // For simplicity, ignore bins for notes that are "too high" to care about. Stop
  // at the mid-point because it's symmetrical.
  (srate/Std.mtof(90)) $ int => int maxBin;
  for (maxBin => int bin; bin < corr.fvals().size()/2; bin++) {
    if (corr.fval(bin) >= corr.fval(maxBin)) {
      bin => maxBin;
    }
  }
  return srate/maxBin;
}

for (int i; i < 10; i++) {
  Math.random2f(Std.mtof(40), Std.mtof(80)) => s.freq;
  flip.size()::samp => now;
  <<< "pitch", s.freq(), "estimated to be", estimatePitch(), "hz" >>>;
}
