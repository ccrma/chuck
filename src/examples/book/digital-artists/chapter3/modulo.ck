// modulo

for ( 0 => int counter; counter < 16; counter++ )
{
    <<< " ===================== " >>>;
    <<< counter, "modulo 2 is: ", counter%2 >>>;
    <<< counter, "modulo 4 is: ", counter%4 >>>;
    <<< counter, "modulo 8 is: ", counter%8 >>>;
    1::second => now;  
}

