//------------------------------------------------------------------------------
// name: word2vec-prompt.ck
// desc: word2vec interative prompt explorer
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// NOTE: this requires ConsoleInput which needs command line chuck
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

// make a ConsoleInput
ConsoleInput in;
// tokenizer
StringTokenizer tok;
// line
string line[0];

// our default model
Word2Vec model;

// loading any default here
"glove-wiki-gigaword-50.txt" => string filepath;
<<< "loading model:", filepath, "..." >>>;
// load model
if( !model.load( me.dir() + filepath ) )
{
    <<< "cannot load model file...", "" >>>;
    me.exit();
}
// print info about model
print( model );

ModalBar bar => NRev reverb => dac;
SndBuf doh => reverb => dac;
"special:dope" => doh.read;
0 => doh.gain;

// print info about a loaded Word2Vec model
fun void print( Word2Vec model )
{
    // print info
    <<< "dictionary size:", model.size() >>>;
    <<< "embedding dimensions:", model.dim() >>>;
    <<< "using KDTree for search:", model.useKDTree() ? "YES" : "NO" >>>;        
    // get bounds
    float mins[0], maxs[0];
    model.minMax( mins, maxs );
    for( int i; i < model.dim(); i++ )
    { cherr <= "dimension " <= i <= " range: [" <= mins[i] <= ", "
            <= maxs[i] <= "]" <= IO.newline(); }
}

// loop
while( true )
{
    // prompt
    in.prompt( "word2vec2chuck => " ) => now;
    
    // read
    while( in.more() )
    {
        // get it
        tok.set( in.getLine() );
        // clear array
        line.clear();
        // print tokens
        while( tok.more() )
        {
            // put into array
            line << tok.next().lower();
            // print it
            // <<< line[line.size()-1], "" >>>;
        }
        // if non-empty
        if( line.size() )
        {
            // execute
            execute( line) => int result;
        }
    }
}


fun int execute( string line[] )
{
    // check
    line[0] => string command;
    // index
    1 => int index;
        
    // which command
    if( command == "quit" || command == "exit" )
    {
        // exit shred
        me.exit();
    }
    else if( command == "help" || command == "h" )
    {
        <<< "    available word2vec2chuck commands:", "" >>>;
        <<< "       load / l -- load a pre-trained model", "" >>>;
        <<< "       eval / e -- evaluate a word vector expression", "" >>>;
        <<< "       analog / a -- query a logical analogy", "" >>>;
        <<< "       vector / v -- display vector associated with a word", "" >>>;
        <<< "       go / g -- from a word to another across a duration", "" >>>;
        <<< "       similar / s -- from a vector (model size) return similar words", "" >>>;
        <<< "       print / p -- print model info", "" >>>;
        <<< "       help / h -- print this message", "" >>>;
        <<< "       exit / quit -- exit from word2vec2chuck", "" >>>;
        <<< "       load-50 -- load GloVe 50D model (glove-wiki-gigaword-50)", "" >>>;
        <<< "       load-3 -- load PCA 3D model (glove-wiki-gigaword-50-pca-3)", "" >>>;
        <<< "       load-2 -- load t-SNE 2D model (glove-wiki-gigaword-50-tsne-2)", "" >>>;
    }
    else if( command == "load" || command == "l" )
    {
        if( line.size() < 2 )
        {
            <<< "usage: load [model]", "" >>>;
        }
        else
        {
            // get filepath to load
            line[1] => filepath;
            <<< "loading model:", filepath >>>;
            <<< "(this could take a few seconds)...", "" >>>;
            // load
            if( !model.load( me.dir() + filepath ) )
            {
                <<< "cannot load model file...", "" >>>;
                return 1;
            }
            // print info
            print( model );
        }
    }
    else if( command == "print" || command == "p" )
    {
        if( model.size() == 0 )
        {
            <<< "   no model has been loaded yet.", "" >>>;
        }
        else
        {
            // print info
            print( model );
        }
    }
    else if( command == "load-50" || command == "load-3" || command == "load-2" )
    {
        string filepath;
        if( command == "load-50" ) "glove-wiki-gigaword-50.txt" => filepath;
        else if( command == "load-3" ) "glove-wiki-gigaword-50-pca-3.txt" => filepath;
        else "glove-wiki-gigaword-50-tsne-2.txt" => filepath;

        <<< "loading model:", filepath >>>;
        <<< "(this could take a few seconds)...", "" >>>;
        // load
        if( !model.load( me.dir() + filepath ) )
        {
            <<< "cannot load model file...", "" >>>;
            return 1;
        }
        // print info
        print( model );
    }
    else if( command == "eval" || command == "e" )
    {
        if( line.size() < 2 )
        {
            <<< "usage: eval [word expression]", "" >>>;
            <<< "       for example: eval rome - italy + china", "" >>>;
            <<< "       (note: don't forget white spaces around + and -)", "" >>>;
        }
        else
        {
            // expression string
            line[1] => string expr;
            // concatenate
            for( 2 => int i; i < line.size(); i++ )
            { expr + " " + line[i] => expr; }
            // eval
            W2V.eval( model, expr, 10 ) @=> string words[];
            // print search results
            for (0 => int i; i < words.size(); i++)
            { <<< "    ", words[i], "" >>>; }
        }
    }
    else if( command == "analog" || command == "a" )
    {
        if( line.size() < 4 )
        {
            <<< "usage: analog [wordA] [wordB] [wordC]", "" >>>;
            <<< "       for example: analog cat kitten dog", "" >>>;
        }
        else
        {
            // expression string
            line[1] => string a;
            line[2] => string b;
            line[3] => string c;
            // eval
            W2V.analogy( model, a, b, c, 10 ) @=> string words[];
            // print
            <<< a, "is to", b, "as", c, "is to..." >>>;
            for (0 => int i; i < words.size(); i++)
            {
                // trivially reject a b c in the output
                if( words[i] == a || words[i] == b || words[i] == c ) continue;
                    <<< "    ", words[i], "" >>>;
            }
        }
    }
    else if( command == "vector" || command == "v" )
    {
        if( line.size() < 2 )
        {
            <<< "usage: vector [word]", "" >>>;
        }
        else
        {
            float vector[model.dim()];
            model.getVector( line[1], vector );
            // print
            cherr <= line[1] <= ": ";
            for( int i; i < vector.size(); i++ ) { cherr <= vector[i] <= " "; }
            cherr <= IO.newline();
        }
    }
    else if( command == "go" || command == "g" )
    {
        if( line.size() < 5 )
        {
            <<< "usage: go [word/start] [word/end] [second] [steps] [k results]", "" >>>;
        }
        else
        {
            <<< "going from", line[1], "to", line[2], "over", line[3], "seconds in", line[4], "steps" >>>;
            float coord1[model.dim()], coord2[model.dim()];
            model.getVector(line[1], coord1);
            model.getVector(line[2], coord2);
            
            // print coordinate 1
            // cherr <= line[1] <= ": ";
            // for( int i; i < coord1.size(); i++ ) { cherr <= coord1[i] <= " "; }
            // cherr <= IO.newline();

            // print coordinate 2
            // cherr <= line[2] <= ": ";
            // for( int i; i < coord2.size(); i++ ) { cherr <= coord2[i] <= " "; }
            // cherr <= IO.newline();

            // walk
            1 => int k;
            if( line.size() > 5 ) Std.atoi(line[5]) => k;
            go( line[1], line[2], Std.atof(line[3])::second, Std.atoi(line[4]), k );
        }
    }
    else if( command == "similar" || command == "s" )
    {
        if( line.size() != model.dim()+1)
        {
            <<< "usage: similar [float] [float] ... model dimension", "" >>>;
        }
        else 
        {
            string results[10];
            model.getSimilar( [Std.atof(line[1]),Std.atof(line[2])], 10, results );
            for( int i; i < results.size(); i++ )<<< results[i]>>>;
        }
    }
    else
    {
        // not found
        <<< "    command not found:", command >>>;
    }
    
    return 0; // 0 is good
}


fun void sonifyDoh( float params[], float vstart[], float vend[] )
{
    if( params.size() < 2 )
    {
        <<< "not enough params!", "" >>>;
        return;
    }
    
    // use param 1 for volume
    Math.remap(params[0], vstart[0], vend[0], .1, 1 ) => doh.gain;
    // use param 2 for rate
    Math.remap(params[1], vstart[1], vend[1], 1, 1.5 ) => doh.rate;

    // trigger from beginning
    0 => doh.pos;
}


// this goes from one word to another word
// > go fun disaster 5 30 1
// > go technology famine 4 20
fun void go( string start, string end, dur T, int steps, int k )
{
    float vstart[model.dim()], vend[model.dim()];
    model.getVector(start, vstart);
    model.getVector(end, vend);

    // get bounds for mapping
    float mins[0], maxs[0];
    model.minMax( mins, maxs );

    // duplicate    
    W2V.dup( vend ) @=> float vdiff[];
    W2V.dup( vstart ) @=> float v[];
    
    for( int i; i < vstart.size(); i++ ) { cherr <= vstart[i] <= " "; } cherr <= IO.newline();
    for( int i; i < vend.size(); i++ ) { cherr <= vend[i] <= " "; } cherr <= IO.newline();

    W2V.minus( vdiff, vstart );
    
    // increment
    1.0 / steps => float inc;
    // time step
    T * inc => dur Tstep;
    // scale
    0 => float factor;

    string results[k];
    float coord[model.dim()];

    // take the steps
    for( int i; i < steps+1; i++ )
    {
        // scale difference vector
        W2V.scale( v, vdiff, factor );
        inc +=> factor;
        // add
        W2V.add( v, vstart );
        // search
        model.getSimilar( v, k, results);
        results[Math.random2(0,k-1)] => string word;
        model.getVector( word, coord );

        // print
        <<< i, "->", word >>>;
        
        // sonify
        sonifyDoh( coord, vstart, vend);
        // for( int i; i < v.size(); i++ ) { cherr <= v[i] <= " "; }
        // cherr <= IO.newline();
        
        // advance time
        Tstep => now;
    }
}


// word2vec helper
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
