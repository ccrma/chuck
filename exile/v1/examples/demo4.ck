// sweep midi keyboards
// some hand - assembled midi instructions here
// -gewang

0xb04444 => int prog;
0x90007f => int msg;
0x80007f => int off;

prog => midiout;
while( msg < 0x907f7f )
{
    msg => midiout;
    msg + 0x000100 => msg;
    off + 0x000100 => off;
    40::ms => now;
    off => midiout;
}

