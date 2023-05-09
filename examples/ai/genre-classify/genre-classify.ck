//------------------------------------------------------------------------------
// name: genre-classify.ck
// desc: a real-time genre-classifier using the features extracted using
//       GTZAN dataset + features-extract.ck
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// USAGE: run in any real-time mode (default)
//        > chuck classify-genres.ck
//
// uncomment the next line to learn more about the KNN2 object:
// KNN2.help();
//
// date: Spring 2023
// authors: Ge Wang (https://ccrma.stanford.edu/~ge/)
//          Yikai Li
//------------------------------------------------------------------------------

// input: pre-extracted features file with labels
me.dir() + "data/gtzan-23.txt" => string FEATURES_FILE;
// if have arguments, override filename
if( me.args() > 0 ) me.arg(0) => FEATURES_FILE;
//------------------------------------------------------------------------------
// expected features file format:
//------------------------------------------------------------------------------
// VALUE VALUE ... VALUE LABEL
// VALUE VALUE ... VALUE LABEL
// ...   ...   ... ...   LABEL
// VALUE VALUE ... VALUE LABEL
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// unit analyzer network: this must match the features in the features file
//------------------------------------------------------------------------------
// audio input into a FFT
adc => FFT fft;
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


//-----------------------------------------------------------------------------
// setting analysis parameters -- also should match what was used during extration
//-----------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// load feature data; read important global values like numPoints and numCoeffs
//------------------------------------------------------------------------------
// values to be read from file
0 => int numPoints; // number of points in data
0 => int numCoeffs; // number of dimensions in data
// file read PART 1: read over the file to get numPoints and numCoeffs
loadFile( FEATURES_FILE ) @=> FileIO @ fin;
// check
if( !fin.good() ) me.exit();
// check dimension
if( numCoeffs != NUM_DIMENSIONS )
{
    // error
    <<< "[error] expecting:", NUM_DIMENSIONS, "dimensions; but features file has:", numCoeffs >>>;
    // stop
    me.exit();
}

// labels of all data points
string inLabels[numPoints];
// label indices of all data points
int inLabelsInt[inLabels.size()];
// feature vectors of data points
float inFeatures[numPoints][numCoeffs];
// keys
string labels[0];
// use as map: labels to numbers
int label2int[0];

//------------------------------------------------------------------------------
// array for storing features
//------------------------------------------------------------------------------
// how much time to aggregate features for each file
// (this does not need to match extraction; might play with this number)
.5::second => dur EXTRACT_TIME;
// given EXTRACT_TIME and HOP, how many frames per file?
(EXTRACT_TIME / HOP) $ int => int numFrames;

// use this for new input
float features[numFrames][numCoeffs];
// average values of coefficients across frames
float featureMean[numCoeffs];
// for printing
int lengths[0];
// for printing (how much to indent)
int indents[0];


//------------------------------------------------------------------------------
// read the data
//------------------------------------------------------------------------------
readData( fin );


//------------------------------------------------------------------------------
// set up our KNN object to use for classification
// (KNN2 is a fancier version of the KNN object)
// -- run KNN2.help(); in a separate program to see its available functions --
//------------------------------------------------------------------------------
KNN2 knn;
// k nearest neighbors
10 => int K;
// results vector
float knnResult[labels.size()];
// knn train
knn.train( inFeatures, inLabelsInt );




//------------------------------------------------------------------------------
// real-time classification loop
//------------------------------------------------------------------------------
while( true )
{
    // aggregate features over a period of time
    for( int frame; frame < numFrames; frame++ )
    {
        //-------------------------------------------------------------
        // a single upchuck() will trigger analysis on everything
        // connected upstream from combo via the upchuck operator (=^)
        // the total number of output dimensions is the sum of
        // dimensions of all the connected unit analyzers
        //-------------------------------------------------------------
        combo.upchuck();  
        // get features
        for( int d; d < NUM_DIMENSIONS; d++) 
        {
            // store them in current frame
            combo.fval(d) => features[frame][d];
        }
        // advance time
        HOP => now;
    }
    
    // compute means for each coefficient across frames
    for( int d; d < NUM_DIMENSIONS; d++ )
    {
        // zero out
        0.0 => featureMean[d];
        // loop over frames
        for( int j; j < numFrames; j++ )
        {
            // add
            features[j][d] +=> featureMean[d];
        }
        // average
        numFrames /=> featureMean[d];
    }
    
    //-------------------------------------------------
    // predict using KNN2; results filled in knnResults
    //-------------------------------------------------
    knn.predict( featureMean, K, knnResult );
    
    // print results
    chout <= "-------------------------------------------------" <= IO.newline();
    // print for each label
    for( int i; i < knnResult.size(); i++ )
    {
        // print label
        chout <= labels[i] <= ":";
        // print indentatation
        for( int ii; ii < indents[i]; ii++ ) { chout <= " "; }
        // print probability
        chout <= knnResult[i] <= IO.newline();
    }
}
//------------------------------------------------------------------------------
// end of real-time classification loop
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// function: load data file
//------------------------------------------------------------------------------
fun FileIO loadFile( string filepath )
{
    // reset
    0 => numPoints;
    0 => numCoeffs;
    
    // load data
    FileIO fio;
    if( !fio.open( filepath, FileIO.READ ) )
    {
        // error
        <<< "cannot open file:", filepath >>>;
        // close
        fio.close();
        // return
        return fio;
    }
    
    string str;
    string line;
    // read file int array
    while( fio.more() )
    {
        // read each line
        fio.readLine().trim() => str;
        // check if empty line
        if( str != "" )
        {
            numPoints++;
            str => line;
        }
    }
    
    // a string tokenizer
    StringTokenizer tokenizer;
    // set to last non-empty line
    tokenizer.set( line );
    // -1 (to account for label)
    -1 => numCoeffs;
    // see how many, including label name
    while( tokenizer.more() )
    {
        tokenizer.next();
        numCoeffs++;
    }
    
    // check
    if( numPoints == 0 || numCoeffs <= 0 )
    {
        <<< "no data in file:", filepath >>>;
        fio.close();
        return fio;
    }
    
    // print
    <<< "# of data points:", numPoints, "dimensions:", numCoeffs >>>;
    
    // done for now
    return fio;
}


//------------------------------------------------------------------------------
// function: read the data
//------------------------------------------------------------------------------
fun void readData( FileIO fio )
{
    // rewind the file reader
    fio.seek( 0 );
    
    // read file int array
    string str;
    int ci, ri;
    while( fio => str )
    {
        // check for last
        if( (ci != 0) && ((ci % numCoeffs) == 0) )
        {
            // read in label
            str => inLabels[ri];
            // set in map
            1 => label2int[str];
            // increment row
            ri++;
            // reset column
            0 => ci;
        }
        else
        {
            // store feature value
            Std.atof(str) => inFeatures[ri][ci];
            // increment column
            ci++;
        }
    }
    
    // get keys from map
    label2int.getKeys( labels );
    // assign index
    for( int i; i < labels.size(); i++ )
    { i => label2int[labels[i]]; }
    // convert in labels to ints
    for( int i; i < inLabels.size(); i++ )
    {
        // get index as int
        label2int[inLabels[i]] => inLabelsInt[i];
    }
    
    // max length
    0 => int maxLength;
    // get lengths of label names for printing
    for( int i; i < labels.size(); i++ )
    {
        // compare with max length
        if( labels[i].length() > maxLength )
            labels[i].length() => maxLength;
        // append to array
        lengths << labels[i].length();
    }
    
    // get lengths of label names for printing
    for( int i; i < lengths.size(); i++ )
    {
        // get indent for length
        indents << (maxLength + 1 ) - lengths[i];
    }
}
