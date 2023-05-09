//------------------------------------------------------------------------------
// name: x-validate.ck
// desc: a cross validation example using extracted features
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// USAGE: this is purely for cross validation on extracted features, so ...
//        run chuck in non-real-time mode (this actually can be much
//        faster than real-time mode, since it doesn't synch to audio):
//
//        cross validation from FILE
//            > chuck --silent x-validate.ck:FILE
//
// date: Spring 2023
// authors: Yikai Li
//          Ge Wang (https://ccrma.stanford.edu/~ge/)
//------------------------------------------------------------------------------

// input: pre-extracted features file with labels
"" => string FEATURES_FILE;
// if have arguments, override filename
if( me.args() > 0 ) me.arg(0) => FEATURES_FILE;
else
{ <<< "[usage]: chuck --silent x-validate:FILE", "" >>>; me.exit();}
//------------------------------------------------------------------------------
// expected features file format:
//------------------------------------------------------------------------------
// VALUE VALUE ... VALUE LABEL
// VALUE VALUE ... VALUE LABEL
// ...   ...   ... ...   LABEL
// VALUE VALUE ... VALUE LABEL
//------------------------------------------------------------------------------


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


//------------------------------------------------------------------------------
// cross validation
//------------------------------------------------------------------------------
// number of folds
20 => int numFolds;
// number of folds to use for testing
4 => int numTestFolds;
// number of folds to use for training
numFolds - numTestFolds => int numTrainFolds;
// number of points in each fold
(numPoints / numFolds) $ int => int numPointsPerFold;
// feature vectors of training data
float trainFeatures[numTrainFolds * numPointsPerFold][numCoeffs];
// labels of training data
int trainLabelsInt[numTrainFolds * numPointsPerFold];
// feature vectors of testing data
float testFeatures[numTestFolds * numPointsPerFold][numCoeffs];
// labels of testing data
int testLabelsInt[numTestFolds * numPointsPerFold];
// normalize the data
normalizeData();
// shuffle the data
shuffleData();
// cross validation
for( 0 => int i; i < numFolds / numTestFolds; i++)
{
    // prepare training and testing data
    prepareData( i );
    // train
    knn.train( trainFeatures, trainLabelsInt );
    // test
    0.0 => float accuracy;
    for( 0 => int j; j < testLabelsInt.size(); j++ )
    {
        // predict
        knn.predict( testFeatures[j], K, knnResult );
        // aggregate accuracy
        knnResult[ testLabelsInt[j] ] +=> accuracy;
    }
    // print accuracy
    chout <= "fold " + i + " accuracy: " + ( accuracy / testLabelsInt.size() ) <= IO.newline();
}


//------------------------------------------------------------------------------
// function: normalizeData()
//------------------------------------------------------------------------------
fun void normalizeData()
{
    // for each dimension
    for( 0 => int i; i < numCoeffs; i++ )
    {
        // find min and max
        inFeatures[0][i] => float min;
        inFeatures[0][i] => float max;
        for( 1 => int j; j < numPoints; j++ )
        {
            if( inFeatures[j][i] < min ) inFeatures[j][i] => min;
            if( inFeatures[j][i] > max ) inFeatures[j][i] => max;
        }
        max - min => float range;
        // normalize
        for( 0 => int j; j < numPoints; j++ )
            (inFeatures[j][i] - min) / range => inFeatures[j][i];
    }
}


//------------------------------------------------------------------------------
// function: shuffleData()
//------------------------------------------------------------------------------
fun void shuffleData()
{
    // prepare swap data
    float swapFeatures[numCoeffs];
    int swapLabelInt;
    // shuffle the data
    for( numPoints - 1 => int i; i > 0; i-- )
    {
        // random index
        Math.random2( 0, i ) => int j;
        // swap features
        for( 0 => int k; k < numCoeffs; k++ )
        {
            inFeatures[i][k] => swapFeatures[k];
            inFeatures[j][k] => inFeatures[i][k];
            swapFeatures[k] => inFeatures[j][k];
        }
        // swap labels
        inLabelsInt[i] => swapLabelInt;
        inLabelsInt[j] => inLabelsInt[i];
        swapLabelInt => inLabelsInt[j];
    }
}


//------------------------------------------------------------------------------
// function: prepareData( int fold )
//------------------------------------------------------------------------------
fun void prepareData( int fold )
{
    // test indices
    fold * numTestFolds * numPointsPerFold => int testStart;
    testStart + numTestFolds * numPointsPerFold => int testEnd;
    // index
    0 => int train_i;
    0 => int test_i;
    // prepare training and testing data
    for( 0 => int i; i < numPoints; i++ )
    {
        // test
        if( i >= testStart && i < testEnd )
        {
            // copy features
            for( 0 => int j; j < numCoeffs; j++ )
                inFeatures[i][j] => testFeatures[test_i][j];
            // copy label
            inLabelsInt[i] => testLabelsInt[test_i];
            // increment
            test_i++;
        }
        // train
        else
        {
            // copy features
            for( 0 => int j; j < numCoeffs; j++ )
                inFeatures[i][j] => trainFeatures[train_i][j];
            // copy label
            inLabelsInt[i] => trainLabelsInt[train_i];
            // increment
            train_i++;
        }
    }
}


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
}
