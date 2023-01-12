//------------------------------------------------------------------------------
// name: word2vec-basic.ck
// desc: word2vec basic usage
//
// NOTE: download sample model from:
//       https://chuck.stanford.edu/chai/datasets/glove/
//           glove-wiki-gigaword-50.txt (50 dimension GloVe pretrained model)
//           glove-wiki-gigaword-50-pca-3.txt (3 dimension reduced, using PCA )
//           glove-wiki-gigaword-50-tsne-2.txt (2 dimension reduced, using t-SNE)
// sorting: part of ChAI (ChucK for AI) [BETA]
//
// author: Yikai Li and Ge Wang
// date: Spring 2023
//------------------------------------------------------------------------------

// instantiate
Word2Vec model;

// pre-trained model to load
"glove-wiki-gigaword-50-pca-3.txt" => string filepath;

<<< "loading model:", filepath >>>;
// NOTE: while loading can take a long time initially; this is internally cached;
// subsequent loads on the same filepath will use cached dictionary, as long as
// ChucK VM keeps running
// HINT: while caching should "just work" in miniAudicle, in command-line chuck
// one could keep one chuck running (using --loop) and check code to it from a 
// second terminal (using chuck + code). for example:
//------------------------------------------------------------------------------
// terminal-1> chuck --loop    *****     terminal-2> chuck + word2vec-basic
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

<<< "=========================================================", "" >>>;

// get vector
<<< "vector associated with the word 'stanford' is:", "" >>>;
float vector[model.dim()];
model.getVector("stanford", vector);
for( int i; i < vector.size(); i++ ) { chout <= vector[i] <= " "; }
chout <= IO.newline();

<<< "=========================================================", "" >>>;

// get similar words via vector
<<< "10 words closest to the vector of 'stanford' are:", "" >>>;
string words[10];
model.similar(vector, words.size(), words);
for( int i; i < words.size(); i++ )
{
    <<< words[i], "" >>>;
}

<<< "=========================================================", "" >>>;

// get most similar by word
<<< "10 words most similar to 'cat' are:", "" >>>;
model.similar("cat", words.size(), words);
for( int i; i < words.size(); i++ )
{
    <<< words[i], "" >>>;
}

<<< "=========================================================", "" >>>;
