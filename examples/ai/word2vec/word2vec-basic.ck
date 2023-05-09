//------------------------------------------------------------------------------
// name: word2vec-basic.ck
// desc: word2vec basic usage
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// NOTE: download pre-trained word vector model from:
//       https://chuck.stanford.edu/chai/data/glove/
//         | (can use any of these)
//       glove-wiki-gigaword-50.txt (400000 words x 50 dimensions)
//       glove-wiki-gigaword-50-pca-3.txt (400000 words x 3 dimensions)
//       glove-wiki-gigaword-50-tsne-2.txt (400000 words x 2 dimensions)
//
// author: Yikai Li and Ge Wang
// date: Spring 2023
//------------------------------------------------------------------------------

// instantiate
Word2Vec model;

// pre-trained model to load
"glove-wiki-gigaword-50.txt" => string filepath;

<<< "loading model:", filepath >>>;
<<< "(this could take a few seconds)...", "" >>>;
// NOTE: while loading can take a long time initially; this is internally cached;
// subsequent loads on the same filepath will use cached dictionary, as long as
// ChucK VM keeps running
// HINT: while caching should "just work" in miniAudicle, in command-line chuck
// one could keep one chuck running (using --loop) and chuk code to it from a 
// second terminal (using chuck + code). for example:
//------------------------------------------------------------------------------
//     terminal-1> chuck --loop  *****  terminal-2> chuck + word2vec-basic
//------------------------------------------------------------------------------
// load a pre-trained model (see URLs above for download)
if( !model.load( me.dir() + filepath ) )
{
    <<< "cannot load model file...", "" >>>;
    me.exit();
}

// print info
<<< "dictionary size:", model.size() >>>;
<<< "embedding dimensions:", model.dim() >>>;
<<< "using KDTree for search:", model.useKDTree() ? "YES" : "NO" >>>;
float mins[0], maxs[0];
// get bounds for each dimension
model.minMax( mins, maxs );
// print the min/max for each dimension
for( int i; i < model.dim(); i++ )
{ cherr <= "dimension " <= i <= ": [" <= mins[i]
        <= " <-range-> " <= maxs[i] <= "]" <= IO.newline(); }

string words[10];
<<< "=========================================================", "" >>>;

// get vector
<<< "vector associated with 'cat' is:", "" >>>;
float vector[model.dim()];
model.getVector("cat", vector);
for( int i; i < vector.size(); i++ ) { chout <= vector[i] <= " "; }
chout <= IO.newline();

<<< "=========================================================", "" >>>;

// get most similar by word
<<< words.size(), "words most similar to 'stanford' are:", "" >>>;
model.getSimilar("stanford", words.size(), words);
for( int i; i < words.size(); i++ )
{ <<< words[i], "" >>>; }

<<< "=========================================================", "" >>>;

// get similar words via vector
<<< words.size(), "words closest to the vector of 'cat' are:", "" >>>;
model.getSimilar( vector, words.size(), words );
for( int i; i < words.size(); i++ )
{ <<< words[i], "" >>>; }

<<< "=========================================================", "" >>>;

// eval an expression
<<< "similar words to expression 'rome - italy + china':", "" >>>;
W2V.eval( model, "rome - italy + china", 10 ) @=> words;
for( 0 => int i; i < words.size(); i++ )
{ <<< words[i], "" >>>; }

<<< "=========================================================", "" >>>;

"dog" => string A; "puppy" => string B; "cat" => string C;
// logical analogy
<<< A, "is to", B, "as", C, "is to..." >>>;
// e.g., dog:puppy::cat:[?] (this returns words for [?])
W2V.analogy( model, A, B, C, 10 ) @=> words;
for( 0 => int i; i < words.size(); i++ )
{ <<< words[i], "" >>>; }

<<< "=========================================================", "" >>>;




//------------------------------------------------------------------------------
// word2vec helper
//------------------------------------------------------------------------------
class W2V
{
    fun static void copy( float to[], float from[] )
    {
        to.size(from.size());
        // add into element
        for( int i; i < to.size(); i++ ) from[i] => to[i];
    }
    
    fun static float[] dup( float from[] )
    {
        float to[from.size()];
        // add into element
        for( int i; i < to.size(); i++ ) from[i] => to[i];
        // return
        return to;
    }
    
    // result stored in x
    fun static void add( float x[], float y[] )
    {
        // smaller of the two
        x.size() < y.size() ? x.size() : y.size() => int size;
        // add into element
        for( int i; i < size; i++ ) y[i] +=> x[i];
    }
    
    // result stored in x
    fun static void minus( float x[], float y[] )
    {
        // smaller of the two
        x.size() < y.size() ? x.size() : y.size() => int size;
        // add into element
        for( int i; i < size; i++ ) y[i] -=> x[i];
    }
    
    fun static void scale( float x[], float scalar )
    {
        // scale
        for( int i; i < x.size(); i++ ) scalar *=> x[i];
    }
    
    fun static void scale( float result[], float x[], float scalar )
    {
        // scale
        for( int i; i < x.size(); i++ ) scalar * x[i] => result[i];
    }
    
    fun static string[] eval( Word2Vec @ w, string expr, int k )
    {
        // init 
        int pos;
        1.0 => float multiplier;
        string word;
        float wordVector[w.dim()];
        float exprVector[w.dim()];
        
        // compute
        while( true )
        {
            expr.find(" ") => pos;
            if( pos == -1 )
            {
                w.getVector(expr, wordVector);
                for( 0 => int i; i < w.dim(); i++ )
                {
                    wordVector[i] * multiplier +=> exprVector[i];
                }
                break;
            }
            else 
            {
                expr.substring(0, pos) => word;
                w.getVector(word, wordVector);
                for( 0 => int i; i < w.dim(); i++ )
                {
                    wordVector[i] * multiplier +=> exprVector[i];
                }
                expr.substring(pos + 1) => expr;
                if( expr.charAt(0) == '+' )
                {
                    1.0 => multiplier;
                    expr.substring(2) => expr;
                }
                else
                {
                    -1.0 => multiplier;
                    expr.substring(2) => expr;
                }
            }
        }
        
        // return
        string results[k];
        w.getSimilar( exprVector, k, results );
        
        return results;
    }
    
    // logical analogy: A to B is as C is to [what this function returns]
    fun static string[] analogy( Word2Vec @ w, string A, string B, string C, int k )
    {
        // the vector sum
        B + " - " + A + " + " + C => string v;
        return eval( w, v, k );
    }
}
