//-----------------------------------------------------------------------------
// name: LiSa
// desc: Live sampling utilities for ChucK
//
// author: Dan Trueman, 2007
//
// to run (in command line chuck):
//     %> chuck readme-LiSa2.ck
//
// to run (in miniAudicle):
//     (just run it!)
//-----------------------------------------------------------------------------

/*
 LiSa allows for multiple voice playback from a live-sampled buffer. Useful
 for granular sampling (a la [munger~] from PeRColate) and looping (a la 
 LoopLoop, Jamman, Echoplex, etc....). The methods are overloaded, taking a
 "voice" number as a first arg. if no voice number is specified, LiSa 
 assumes 0=>voice.

  Below is a simple example to show how to crossfade two voices. See also 
  the LiSa_munger directory for other approaches.

  Below the example find a (lengthy) command summary.
*/

//-----------------------------------------------------------------------------


//signal chain; record a sine wave, play it back
SinOsc s => Envelope e => LiSa loopme => dac;
s => dac;
440. => s.freq;
0.2 => s.gain;

//alloc memory
6::second => loopme.duration;

//play s for a bit
500::ms => now;

//sweep the freq for fun
Envelope pitchmod => blackhole;
pitchmod.duration(2000::ms);
pitchmod.value(s.freq());
pitchmod.target(880.);

//confirm that the length of the buffer is what you expect
<<<"buffer duration = ", loopme.duration() / 44100.>>>;

//set times for recording fade in/out and sample loop length
100::ms => dur recfadetime;
1000::ms => dur mylooplen;
e.duration(recfadetime);

//start recording input; record 1 seconds worth
loopme.record(1);
e.keyOn(); //can also do without the Envelope and use loopme.recramp(dur) to set a recording ramp

now + (mylooplen - recfadetime) => time later;
while(now < later) {
    
    pitchmod.value() => s.freq;
    10::ms => now;
    
}
e.keyOff();
recfadetime => now;
loopme.record(0);

//disconnect input and hangout a bit
s =< dac;
1000::ms => now;

//now, manipulate the sample
//	get a voicenumber; note that this voice won't actually be reserved until you play it
loopme.getVoice() => int voice1;

//we'll play voice 1 forward, and then crossfade it with voice 2 backwards
loopme.pan(voice1, 0.);
loopme.play(voice1, 1);
(mylooplen - recfadetime) => now;

//just as voice 1 is going to fade, bring in voice 2
loopme.getVoice() => int voice2;
loopme.rate(voice2, -1.);
loopme.playPos(voice2, mylooplen); 
loopme.voiceGain(voice2, 0.2);
loopme.pan(voice2, 1.);
loopme.play(voice2, 1);

//wait until voice 1 had finished fading, then turn off
recfadetime => now;
loopme.play(voice1, 0);

//wait for voice 2 to finish
1000::ms => now;

//-----------------------------------------------------------------------------

//LiSa Command Summary:
//
//	mylisa.duration(dur); required -- sets max length of buffer
//	mylisa.duration(); returns max length of buffer
//	mylisa.record(1/0); turn on/off recording into buffer
//	mylisa.getVoice() => voice (int); returns first free voice number
//	mylisa.maxVoices(int); sets maximum # of allowable voices
//	mylisa.play(voice, 1/0); turn on/off play for particular voice
//	mylisa.rampUp(voice, dur); turn on play for voice with ramp
//	mylisa.rampDown(voice, dur); ramp down voice and then turn off play
//	mylisa.rate(voice, float); sets play rate for "voice"
//	mylisa.playPos(voice, dur); sets playback position for "voice" within buffer
//	mylisa.playPos(voice); returns playback position for "voice"
//	mylisa.recordPos(dur); sets record position
//	mylisa.recordPos(); gets record position
//	mylisa.recRamp(dur); sets ramping for the edges of the record buffer
//	mylisa.loopStart(dur, voice); sets loopstart point for "voice"
//	mylisa.loopStart(voice); get loopstart
//	mylisa.loopEnd(voice, dur); sets loopend point for "voice"
//	mylisa.loopEnd(voice); gets loopend
//	mylisa.loop(voice, 1/0); turn on/off looping for "voice"
// 	mylisa.loop(voice); get looping state
//	mylisa.bi(voice, 1/0); turn on/off bidirectional looping for "voice"
//	mylisa.bi(voice); get bi state
//	mylisa.voiceGain(voice, float); sets gain "voice"
//	mylisa.voiceGain(voice); gets gain for "voice"
//	mylisa.loopEndRec(dur); set looping end point for recording
//	mylisa.loopEndRec(); get ...
//	mylisa.feedback(float); set feedback amount [0,1] for recording
//	mylisa.feedback(); get...
//	mylisa.clear(); clear recording buffer
//  mylisa.pan(); returns pan value of voice 0
//  mylisa.pan(float where); pans voice 0 where can be [0., 7.], to place voice across LiSa's 8 outputs
//  mylisa.pan(int voice); returns pan value of voice
//  mylisa.pan(int voice, float where); pans specified voice where can be [0., 7.], to place voice across LiSa's 8 outputs
//  note that LiSa's 8 outputs can be sent to a multichannel dac, or simply to other ugens, if it is desirable to process the
//       channels in different ways. these 8 channels are available regardless of whether the dac is running > 2 chans
//
//All of these commands should work without the "voice" arg; 0=>voice will be assumed

//see online documentation for most up-do-date listing...

//-----------------------------------------------------------------------------


