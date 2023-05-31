SinOsc s => ExpEnv eenv => dac;
1000 => s.freq;
0.999 => eenv.radius;

100*ms => eenv.T60;  1 => eenv.keyOn;  second => now;
second => eenv.T60;  1 => eenv.keyOn;  second => now;
3*second => eenv.T60; 1 => eenv.keyOn; 3*second => now;

while (1)  {
   Math.random2f(0.995,0.9995) => eenv.radius;
   Math.random2f(0.3,1.0) => eenv.gain;
   Std.mtof(Math.random2(0,10)*2+72) => s.freq;
   1 => eenv.keyOn;
   0.1::second => now;
}
