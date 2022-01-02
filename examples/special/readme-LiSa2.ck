//--------------------------------------------------------------------
// name: readme-LiSa2.ck
// desc: Live Sampling (LiSa) utilities for ChucK
//
// author: Dan Trueman
// date: 2007
//
// to run (in command line chuck):
//     %> chuck readme-LiSa2.ck
//
// to run (in miniAudicle):
//     (just run it!)
//--------------------------------------------------------------------
// LiSa allows for multiple voice playback from a live-sampled buffer.
// Useful for granular sampling (a la [munger~] from PeRColate) and
// looping (a la LoopLoop, Jamman, Echoplex, etc....). The methods
// are overloaded, taking a "voice" number as a first arg. if no
// voice number is specified, LiSa assumes 0 => voice.
//
// Below is a simple example to show how to crossfade two voices.
// See also the LiSa_munger examples for other approaches.
//
// Below the example find a (lengthy) command summary.
//--------------------------------------------------------------------

// signal chain; record a sine wave, play it back
SinOsc s => Envelope e => LiSa10 loopme => dac;
// direct
s => e => dac;
// set frequency
440. => s.freq;
// set gain
0.25 => s.gain;

// print channel info
<<< "LiSa channels:", loopme.channels() >>>;

// allocate memory in LiSa
6::second => loopme.duration;

// play s for a bit
500::ms => now;

// sweep the freq for fun
Envelope pitchmod => blackhole;
pitchmod.duration( 2000::ms );
pitchmod.value( s.freq() );
pitchmod.target( 880.0 );

// confirm that the length of the buffer is what you expect
<<< "buffer duration = ", loopme.duration() / second >>>;

// set times for recording fade in/out and sample loop length
100::ms => dur recfadetime;
1000::ms => dur mylooplen;
// set envelope duration
e.duration( recfadetime );

// print
<<< "start recording input into LiSa...", "" >>>;
// start recording input; record 1 seconds worth
loopme.record(1);
// open envelope (can also do without the Envelope and use
// loopme.recramp(dur) to set a recording ramp)
e.keyOn();

// calcuate later
now + (mylooplen - recfadetime) => time later;
// go until now is later (or later)
while(now < later)
{
    // set frequency
    pitchmod.value() => s.freq;
    // advance time
    10::ms => now;
}
// close envelope
e.keyOff();
// let fade time pass
recfadetime => now;
// print
<<< "stop recording input into LiSa...", "" >>>;
// stop recording input
loopme.record(0);

// disconnect direct input...
s =< dac;
// print
<<< "disconnect sine and hanging out...", "" >>>;
// and hang out for a bit
1000::ms => now;

// now, manipulate the sample
// get a voicenumber; note that this voice won't actually be
// reserved until you play it
loopme.getVoice() => int voice1;

// we'll play voice 1 forward; then crossfade with voice 2 backwards
// set gain
loopme.voiceGain( voice1, .5 );
// set pan (hard left channel)
loopme.pan( voice1, 0 );
// play voice 1
loopme.play( voice1, 1 );
// print
<<< "playing LiSa voice 1 ( id:", voice1, ")" >>>;
// let time pass
(mylooplen - recfadetime) => now;

// just as voice 1 is going to fade, bring in voice 2
loopme.getVoice() => int voice2;
// set play rate to go backwards
loopme.rate( voice2, -1.0 );
// set play head at the end
loopme.playPos( voice2, mylooplen ); 
// set gain
loopme.voiceGain( voice2, 0.5 );
// set pan (hard right channel)
loopme.pan( voice2, 1 );
// play
loopme.play( voice2, 1 );
// print
<<< "playing LiSa voice 2 ( id:", voice2, ")" >>>;

// wait until voice 1 had finished fading...
recfadetime => now;
// turn off voice 1
loopme.play( voice1, 0 );
// print
<<< "stopping LiSa voice 1...", "" >>>;

// wait for voice 2 to finish
1000::ms => now;

// print
<<< "program ending...", "" >>>;

//--------------------------------------------------------------------
// LiSa Command Summary:
//
//  mylisa.duration(dur); required -- sets max length of buffer
//  mylisa.duration(); returns max length of buffer
//  mylisa.record(1/0); turn on/off recording into buffer
//  mylisa.getVoice() => voice (int); returns first free voice number
//  mylisa.maxVoices(int); sets maximum # of allowable voices
//  mylisa.play(voice, 1/0); turn on/off play for particular voice
//  mylisa.rampUp(voice, dur); turn on play for voice with ramp
//  mylisa.rampDown(voice, dur); ramp down voice and then turn off play
//  mylisa.rate(voice, float); sets play rate for "voice"
//  mylisa.playPos(voice, dur); sets playback position for "voice" within buffer
//  mylisa.playPos(voice); returns playback position for "voice"
//  mylisa.recordPos(dur); sets record position
//  mylisa.recordPos(); gets record position
//  mylisa.recRamp(dur); sets ramping for the edges of the record buffer
//  mylisa.loopStart(dur, voice); sets loopstart point for "voice"
//  mylisa.loopStart(voice); get loopstart
//  mylisa.loopEnd(voice, dur); sets loopend point for "voice"
//  mylisa.loopEnd(voice); gets loopend
//  mylisa.loop(voice, 1/0); turn on/off looping for "voice"
//  mylisa.loop(voice); get looping state
//  mylisa.bi(voice, 1/0); turn on/off bidirectional looping for "voice"
//  mylisa.bi(voice); get bi state
//  mylisa.voiceGain(voice, float); sets gain "voice"
//  mylisa.voiceGain(voice); gets gain for "voice"
//  mylisa.loopEndRec(dur); set looping end point for recording
//  mylisa.loopEndRec(); get ...
//  mylisa.feedback(float); set feedback amount [0,1] for recording
//  mylisa.feedback(); get...
//  mylisa.clear(); clear recording buffer
//  mylisa.pan(); returns pan value of voice 0
//  mylisa.pan(float where); pans voice 0 where can be [0., 7.], to place voice across LiSa's 8 outputs
//  mylisa.pan(int voice); returns pan value of voice
//  mylisa.pan(int voice, float where); pans specified voice where can be [0., 7.], to place voice across LiSa's 8 outputs
//
// Note that that LiSa defaults to mono;
// As of chuck 1.4.1.0 multi-channel LiSas can be instantiated
// using the following UGens:
//   LiSa2 (stereo)
//   LiSa4 (quad),
//   LiSa6 (6-channel, laptop orchestra edition)
//   LiSa8 (8-channel),
//   LiSa10 (10-channel, for compatibility)
//   LiSa16 (16-channel)
// The outputs of these LiSas can be sent to a multichannel dac, or
// simply to other ugens, if it is desirable to process the channels
// in different ways. These multiple channels are available
// regardless of whether the dac is running > 2 chans. LiSaX's
// multi-channel output can be manually connected through .chan(n).
//
// All of these commands should work without the "voice" arg;
// 0 => voice will be assumed see online documentation for most
// up-do-date listing...
//--------------------------------------------------------------------
