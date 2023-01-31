//---------------------------------------------------------------------
// name: pca.ck
// desc: principle component analysis utility
//
// uncomment for PCA API:
// PCA.help();
//
// author: Yikai Li
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// prepare
float input[10][10], output[10][3];
for( 0 => int i; i < 10; i++ )
{
    for( 0 => int j; j < 10; j++ )
    {
        // put noise in
        Math.randomf() => input[i][j];
    }
}

// dimension reduce input (e.g., 10x10) to output (e.g., 10x3)
// PCA.transform() is a static method...
// and can be called with or without PCA instance
PCA.reduce( input, 3, output );

// print
for ( 0 => int i; i < 10; i++ )
{
    for ( 0 => int j; j < 3; j++ )
    {
        // prin t
        chout <= output[i][j] <= ",";
    }
    chout <= IO.newline();
}