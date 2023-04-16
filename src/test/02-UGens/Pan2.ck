Step step[2] => Pan2 u => blackhole;
1::samp => now;

u.last() => float last;
u.chan(0).last() => float last_left;

u =< blackhole;
null @=> u;

if (last == last_left)
   <<< "success" >>>;