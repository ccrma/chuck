//---------------------------------------------------------------------
// name: gen-all.ck
// desc: generate HTML documentation for chuck website reference
//       see: https://chuck.stanford.edu/doc/reference/
//
// version: need chuck version 1.5.0.0 or higher
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2023
//---------------------------------------------------------------------

// instantiate a CKDoc object
CKDoc doc; // documentation orchestra
// set the examples root
"../examples/" => doc.examplesRoot;

// add group
doc.addGroup(
    // class names
    ["Object", "Event", "Shred", "Math", "Machine", "Std", "string", "@array" ],
    // group name
    "Base Classes",
    // file name
    "base", 
    // group description
    "Basic classes (Object, Event, Shred, etc.) and standard libraries (Math, Machine, Std)"
);

// add group
doc.addGroup(
    // class names
    [ "UGen", "Gain", "Impulse", "Step", "SndBuf", "SndBuf2", "ADSR", "Envelope",
      "Delay", "DelayL", "DelayA", /*"DelayP",*/ "Echo", "Noise", "CNoise",
      "Osc", "SinOsc", "TriOsc", "SawOsc", "PulseOsc", "SqrOsc", "Phasor",
      "HalfRect", "FullRect", "Chugraph", "Chugen", "UGen_Multi",
      "UGen_Stereo", "Mix2", "Pan2" ],
    // group name
    "Basic Unit Generators",
    // file name
    "ugens-basic",
    // group description
    "Basic unit generator (UGen) base classes, simple signal generators, envelopes, and more."
);

// add group
doc.addGroup(
    [ "FilterBasic",
      "LPF", "HPF", "BPF", "BRF", "BiQuad", "ResonZ",
      "OnePole", "OneZero", "TwoPole", "TwoZero", "PoleZero" ],
    // group name
    "Basic Filter UGens",
    // file name
    "ugens-filters",
    // group description
    "Basic filter unit generators."
);

// add group
doc.addGroup(
    [ "JCRev", "NRev", "PRCRev", "Chorus", "Modulate", "PitShift",
      "SubNoise", "BLT", "Blit", "BlitSaw", "BlitSquare", "FilterStk",
      "WvIn", "WaveLoop", "WvOut", "WvOut2", "StkInstrument", "BandedWG",
      "BlowBotl", "BlowHole", "Bowed", "Brass", "Clarinet", "Flute",
      "Mandolin", "ModalBar", "Moog", "Saxofony", "Shakers", "Sitar",
      "StifKarp", "VoicForm", "KrstlChr", "FM", "BeeThree", "FMVoices",
      "HevyMetl", "HnkyTonk", "FrencHrn", "PercFlut", "Rhodey", "TubeBell",
      "Wurley" ],
    // group name
    "Synthesis Toolkit (STK) UGens",
    // file name
    "ugens-stk",
    "Utilities and physical models from the Synthesis Toolkit (STK) by Perry R. Cook and Gary Scavone."
);

// add group
doc.addGroup(
    [ "LiSa", "LiSa2", "liSa4", "LiSa6", "LiSa8", "LiSa10", "LiSa16",
      "GenX", "Gen5", "Gen7", "Gen9", "Gen10", "Gen17", "CurveTable",
      "WarpTable", "Dyno" ],
    // group name
    "Advanced Unit Generators",
    // file name
    "ugens-advanced",
    // group description
    "Advanced and specialty unit generators"
);

// add group
doc.addGroup(
    [ "ABSaturator", "AmbPan3", "Bitcrusher", "Elliptic", "ExpDelay", "ExpEnv", "FIR", 
      "FoldbackSaturator", "GVerb", "KasFilter", "MagicSine", "Mesh2D", "Multicomb", 
      "Pan4", "Pan8", "Pan16", "PitchTrack", "PowerADSR", "Sigmund", "Spectacle", 
      "WinFuncEnv", "WPDiodeLadder", "WPKorg35" ],
    // group name
    "Chugins Library",
    // file name
    "chugins",
    // group descriptions
    "Default chugins library offering unit generators and utilities."
);

// add group
doc.addGroup(
    [ "Centroid", "Chroma", "Flux", "Kurtosis", "MFCC", "RMS", "RollOff", "SFM", "ZeroX",
    "AutoCorr", "DCT", "FeatureCollector", "FFT", "Flip", "IDCT", "IFFT", "UnFlip", "XCorr" ],
    // group name
    "Unit Analyzers",
    // file name
    "uanae",
    // group description
    "Unit analyzer (<a target=\"_blank\" href=\"../../uana/\">UAna</a>) objects are specialized UGens that can perform audio analysis and pass analysis information to other UAnae. (See also: <a href=\"ai.html\">AI Tools API reference</a>.)"
);

// add group
doc.addGroup(
    [ "MLP", "KNN", "KNN2", "HMM", "SVM", "Word2Vec", "PCA", "Wekinator", "AI" ],
    // group name
    "AI Tools",
    // file name
    "ai",
    // group description
    "ChucK for AI (<a target=\"_blank\" href=\"../../chai/\">ChAI</a>) is a set of AI/ML tools for building interactive AI musical instruments and systems. (See also: <a href=\"uanae.html\">Unit Analyzer API reference</a>.)"
);

// add group
doc.addGroup(
    [ "IO", "FileIO", /* "StdOut", "StdErr", */
      "OscIn", "OscOut", "OscMsg",
      "Hid", "HidMsg", "KBHit", "SerialIO",
      "MidiIn", "MidiOut", "MidiMsg", "MidiFileIn" ],
    // group name
    "Input & Output",
    // file name
    "io",
    // group descriptions
    "Communication tools, including file input/output, Open Sound Control, MIDI, MIDI file reading, serial i/o."
);

// add group
doc.addGroup(
    // class names
    [ "CKDoc", "StringTokenizer", "ConsoleInput" ],
    // group name
    "Utilities",
    // file name
    "utils",
    // group description
    "Additional Utiilities"
);

// generate
doc.outputToDir( ".", "ChucK Class Library Reference" );

// TODO: print what was generated (or add option to)
// TODO: print what types are in the type system but wasn't documented
