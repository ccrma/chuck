chout <= "creating a KasFilter \n";
chout.flush();

BlitSaw s => KasFilter kf => dac;

110 => s.freq;
chout <= "setting variables \n";
chout.flush();

.9 => kf.gain;
.7 => kf.resonance;
.3 => kf.accent;
chout <= "testing sound using a sweep \n";
chout.flush();

for (110 => float x; x < 5000; 1.0003 *=>x)
{
	x => kf.freq;
	ms => now;
}
chout <= "testing get functions \n";
chout <= "freq = " <= kf.freq() <= "\n";
chout <= "resonance = " <= kf.resonance() <= "\n";
chout <= "accent = " <= kf.accent() <= "\n";
chout.flush();

0 => s.freq;
0 => s.op;
2::second => now;
