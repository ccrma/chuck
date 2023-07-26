1 => global auto a;
<<< a, "" >>>;

[1,2,3] @=> global auto b[];
<<< b[2], "" >>>;

[ [1,2,3], [4,5,6] ] @=> global auto c[][];
<<< c[0][2], "" >>>;

new SinOsc @=> global auto z;
440 => z.freq;
<<< z.freq()$int, "" >>>;

