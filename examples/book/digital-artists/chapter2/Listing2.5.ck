// Listing 2.5 Using dac.chan() to connect to multiple speakers
// NOTE:  This will cause an error if you don't have a 
//        multi-channel sound card, and have it selected
//        as your audio output device in miniAudicle>Preferences

SinOsc s0 => dac.chan(0);
SinOsc s1 => dac.chan(1);
SinOsc s2 => dac.chan(2);
SinOsc s3 => dac.chan(3);

