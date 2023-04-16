Step step[2] => Mix2 u => blackhole;
1::samp => now;

u.last() => float last;

u =< blackhole;
null @=> u;

if (last == 1.0)
   <<< "success" >>>;
