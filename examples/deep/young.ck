//----------------------------------------------------------------------
// name: young.ck
// desc: an interactive ChucK rendition of
//       La Monte Young's Composition 1960 #7
//
// note: one possible, non-interactive, alternative implementation:
//
//     SawOsc b(Std.mtof(59)) => Gain b_gain(0.5) => dac;
//     SawOsc fs(Std.mtof(66)) => Gain fs_gain(0.5) => dac;
//
//     Math.randomf()::eon => dur a_very_long_time;
//     a_very_long_time => now;
//
// author: Nick Shaheed (https://nicholasshaheed.com/)
//   date: Fall 2025
//----------------------------------------------------------------------

Hid hi;
HidMsg msg;

// open keyboard
if( !hi.openKeyboard( 0 ) ) me.exit();

SawOsc b(Std.mtof(59)) => Gain b_gain(0.5) => Envelope b_env => dac;
SawOsc fs(Std.mtof(66)) => Gain fs_gain(0.5) => Envelope fs_env => dac;

// print out piece title and instruction
"Composition 1960 #7" => string title;
print(title);
500::ms => now;

"[spacebar] to be held for a long time" => string instr;
print(instr);

// tracking whether the spacebar has been pressed yet
false => int holding;

// infinite event loop
while( true )
{
    // wait on event
    hi => now;

    // get one or more messages
    while( hi.recv( msg ) )
    {
        // respond to spacebar, if it's pressed begin play fifth, if
        // it's released, the piece is over.
        if( msg.which == 44 )
        {
           if( !holding && msg.isButtonDown() )
           {
              true => holding;
              b_env.keyOn();
              fs_env.keyOn();
           }
           if( holding && msg.isButtonUp() )
           {
              b_env.keyOff();
              fs_env.keyOff();

              chout <= IO.newline();
              500::ms => now;
              "       La Monte Young" => string composer;
              "       July 1960" => string date;

              print(composer);
              print(date);
              500::ms => now;

              me.exit();
           }
        }
    }
}

// incrementally print out characters in a string
fun void print(string str)
{
    // iterate
    for( int i:Std.range(str.length()) )
    {
        chout <= str.substring(i,1);
        // we need to flush the stdout buffer after every
        // character so that it actually prints
        chout.flush();
        50::ms => now;
    }
    // endline
    chout <= IO.newline();
}
