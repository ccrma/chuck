//------------------------------------------------------------------------------
// name: stk_reverb
// desc: use keyboard keys <1, 2, 3> to listen to the built-in stk Reverb UGens!
//       change DEVICE_ID to the appropriate number if this script isn't picking
//       up your keyboard input
//
// 1: JCRev. John Chowning's reverberator class.
// 6 comb filters in parallel, followed by 3 allpass filters, a lowpass filter, 
// and another allpass in series, followed by two allpass filters in parallel 
// with corresponding right and left outputs. 
// 
// 2. NRev. CCRMA's NRev reverberator class. 
// This class is derived from the CLM NRev function, which is based on the use of 
// networks of simple allpass and comb delay filters. 
//
// 3. PRCRev. Perry's simple reverberator class. 
// The least-expensive reverb, but also maybe the lowest quality...
//
// Usage: use the .mix() method to control wet/gain ratio! 
// E.g. `.mix(.05)` sets the reverb UGen's output to be 5% wet and 95% dry.
//
// Performance: Reverbs tend to be very computationally expensive. Good practice
// is to gather all your signals and pipe them through a single main reverb UGen,
// rather than having a separate reverb for each signal.
//
// author: azaday
// date: Spring 2023
//------------------------------------------------------------------------------
0 => int DEVICE_ID;  // CHANGE ME IF KEYBOARD NOT FOUND

// Setup UGen graph
SndBuf buf => JCRev jcRev => dac;
       buf => NRev nRev => dac;
       buf => PRCRev prcRev => dac;    

// set wet/dry ratio
.5 => float revMix;
revMix => jcRev.mix;
revMix => nRev.mix;
revMix => prcRev.mix;

// start with jcRev
<<< "playing audio through JCRev" >>>;
1.0 => jcRev.gain;
0.0 => nRev.gain;
0.0 => prcRev.gain;

// "Dope" will be our source audio signal
"special:dope" => buf.read;

fun void looper(SndBuf @ buf) {
    while (true) {
        buf.length() => now;
        200::ms => now; 
        0 => buf.pos;
    }
} spork ~ looper(buf);

// 3 => int playCount;  // number of times before swapping to next reverb
fun void keyboardListener(int device) {
    Hid hi;
    HidMsg msg;

    0x1e => int KEY_1;  // Keyboard 1 and !
    0x1f => int KEY_2;  // Keyboard 2 and @
    0x20 => int KEY_3;  // Keyboard 3 and #

    // open keyboard (get device number from command line)
    if( !hi.openKeyboard( device ) ) {
        cherr <= "keyboard " + device + " not found" <= IO.newline();
        me.exit();
    }
    <<< "keyboard '" + hi.name() + "' ready", "" >>>;

    while( true ) {
        hi => now;
        while( hi.recv( msg ) ) {
            if( msg.isButtonDown() ) {
                if (msg.key == KEY_1) {
                    <<< "switching to JCRev" >>>;
                    1.0 => jcRev.gain;
                    0.0 => nRev.gain;
                    0.0 => prcRev.gain;
                }

                if (msg.key == KEY_2) {
                    <<< "switching to NRev" >>>;
                    0.0 => jcRev.gain;
                    1.0 => nRev.gain;
                    0.0 => prcRev.gain;
                }

                if (msg.key == KEY_3) {
                    <<< "switching to PRCRev" >>>;
                    0.0 => jcRev.gain;
                    0.0 => nRev.gain;
                    1.0 => prcRev.gain;
                }
                
            }
        }
    }
} spork ~ keyboardListener(DEVICE_ID);

// keey main shred alive
while (1::second => now) {}
