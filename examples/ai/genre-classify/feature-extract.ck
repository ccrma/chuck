//---------------------------------------------------------------------
// name: feature-extract.ck
// desc: sets up automated extraction over a training dataset
//       this example can be adapted to other labeled audio dataset
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// DATA: by default, this uses the GTZAN dataset:
//       download and unzip updated version:
//       https://chuck.stanford.edu/chai/data/gtzan/
//
// USAGE: this is purely for feature extraction on training data, so...
//        run chuck in non-real-time mode (this actually can be much
//        faster than real-time mode, since it doesn't synch to audio):
//
//        extract and print
//            > chuck --silent 2-extract.ck
//
//        extract and write to FILE
//            > chuck --silent 2-extract.ck:FILE
//
// date: Spring 2023
// authors: Ge Wang (https://ccrma.stanford.edu/~ge/)
//          Yikai Li
//---------------------------------------------------------------------


// output file (if empty, will print to console)
"" => string OUTPUT_FILE;
// get from arguments
if( me.args() > 0 ) me.arg(0) => OUTPUT_FILE;

// check
if( Machine.silent() == false )
{
    // print helpful message
    <<< "-----------------", "" >>>;
    <<< "[feature-extract]: chuck is currently running in REAL-TIME mode;", "" >>>;
    <<< "[feature-extract]: this step has no audio; may run much faster in SILENT mode!", "" >>>;
    <<< "[feature-extract]: to run in SILENT mode, restart chuck with --silent flag", "" >>>;
    <<< "-----------------", "" >>>;
}


//---------------------------------------------------------------------
// analysis network -- this determines which feature will be extracted
//---------------------------------------------------------------------
// audio input into a FFT
SndBuf audioFile => FFT fft;
// a thing for collecting multiple features into one vector
FeatureCollector combo => blackhole;
// add spectral feature: Centroid
fft =^ Centroid centroid =^ combo;
// add spectral feature: Flux
fft =^ Flux flux =^ combo;
// add spectral feature: RMS
fft =^ RMS rms =^ combo;
// add spectral feature: MFCC
fft =^ MFCC mfcc =^ combo;


//---------------------------------------------------------------------
// setting analysis parameters -- important for tuning your extraction
//---------------------------------------------------------------------
// set number of coefficients in MFCC (how many we get out)
// 13 is a commonly used value; using less here for printing
20 => mfcc.numCoeffs;
// set number of mel filters in MFCC
10 => mfcc.numFilters;

// do one .upchuck() so FeatureCollector knows how many total dimension
combo.upchuck();
// get number of total feature dimensions
combo.fvals().size() => int NUM_DIMENSIONS;

// set FFT size
2048 => fft.size;
// set window type and size
Windowing.hann(fft.size()) => fft.window;
// our hop size (how often to perform analysis)
2048::samp => dur HOP;


//---------------------------------------------------------------------
// training data -- preparation specific to a train dataset
//---------------------------------------------------------------------
// labels (and filename roots)
["blues", "classical", "country", "disco", "hiphop",
 "jazz", "metal", "pop", "reggae", "rock"] @=> string labels[];
// how many per label
100 => int NUM_EXAMPLES_PER_LABEL;
// how much time to aggregate features for each file
10::second => dur EXTRACT_TIME;
// given EXTRACT_TIME and HOP, how many frames per file?
(EXTRACT_TIME / HOP) $ int => int numFrames;
// relative path
"data/gtzan/genres_original/" => string PATH;

// a feature frame
float featureFrame[NUM_DIMENSIONS];
// how many input files
0 => int NUM_FILES;

// output reference, default is error stream (cherr)
cherr @=> IO @ theOut;
// instantiate
FileIO fout;
// output file
if( OUTPUT_FILE != "" )
{
    // print
    <<< "opening file for output:", OUTPUT_FILE >>>;
    // open
    fout.open( OUTPUT_FILE, FileIO.WRITE );
    // test
    if( !fout.good() )
    {
        <<< "cannot open file for writing...", "" >>>;
        me.exit();
    }
    // override
    fout @=> theOut;
}


//---------------------------------------------------------------------
// extraction -- iterating over entire training dataset
//---------------------------------------------------------------------

// filename
string filename;
// loop over labels
for( int i; i < labels.cap(); i++)
{
    // get current label
    labels[i] => string label;
    // loop over examples under each label
    for( int j; j < NUM_EXAMPLES_PER_LABEL; j++ )
    {
        // construct filepath
        me.dir() + PATH + label + "/" + label + ".000" + (j<10?"0":"") + j + ".wav" => filename;
        // extract the file
        if( !extractFeatures( filename, label, theOut ) )
        {
            // issue warning
            cherr <= "PROBLEM during extraction: " <= filename <= IO.newline();
            // bail out
            me.exit();
        }
    }
}


//---------------------------------------------------------------------
// function: extract and print features from a single file
//---------------------------------------------------------------------
fun int extractFeatures( string inputFilePath, string label, IO out )
{    
    // increment
    NUM_FILES++;
    // log 
    cherr <= "[" <= NUM_FILES <= "] extracting features: " <= inputFilePath <= IO.newline();

    // load by block to speed up IO
    2048 => audioFile.chunks;
    // read the audio file
    inputFilePath => audioFile.read;
    // zero out
    featureFrame.zero();

    // let one FFT-size of time pass (to buffer)
    fft.size()::samp => now;
    // loop over frames
    for( int i; i < numFrames; i++ )
    {
        //-------------------------------------------------------------
        // a single upchuck() will trigger analysis on everything
        // connected upstream from combo via the upchuck operator (=^)
        // the total number of output dimensions is the sum of
        // dimensions of all the connected unit analyzers
        //-------------------------------------------------------------
        combo.upchuck();
        // for each dimension
        for( int d; d < NUM_DIMENSIONS; d++ )
        {
            // copy
            combo.fval(d) +=> featureFrame[d];
        }
        // advance time
        HOP => now;
    }
    
    //-------------------------------------------------------------
    // average into a single feature vector per file
    // NOTE: this can be easily modified to N feature vectors
    // spread out over the length of an audio file; for now
    // we will just do one feature vector per file
    //-------------------------------------------------------------
    for( int d; d < NUM_DIMENSIONS; d++ )
    {
        // average by total number of frames
        numFrames /=> featureFrame[d];
        // print the MFCC results
        out <= featureFrame[d] <= " ";
    }    
    // print label name and endline
    out <= label <= IO.newline();
    
    // done
    return true;
}