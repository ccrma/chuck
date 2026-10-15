MidiOut mout;

if (!mout.open(0)) {
	me.exit();
}

// Full frame midi time code (MTC) message.
// SMPTE timecode: 01:02:03:04.20 (1 hour, 2 minutes, 3 seconds, 4 frames, 20 subframes).
// F0 7F 7F 01 01 01 02 03 04 14 F7
mout.send([ 0xF0, 0x7F, 0x7F, 0x01, 0x01, 0x01, 0x02, 0x03, 0x04, 0x14, 0xF7 ]);
