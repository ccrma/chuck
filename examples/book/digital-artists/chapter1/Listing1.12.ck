// Listing 1.12 Random triangle wave music

/* Random Triangle Wave Music
by ChucK Programmer */

TriOsc t=> dac; // Use a triangle wave for variety.

while (true)  // (1) infinite loop runs forever
{
              // (2) randomly choose frequency from 30 to 1000
    Math.random2(30,1000) => t.freq;

              // (3) randomly choose duration from 30 to 1000 ms
    Math.random2f(30,1000) :: ms => now;
}
