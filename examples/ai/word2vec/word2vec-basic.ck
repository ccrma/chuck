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
"glove-wiki-gigaword-50.txt" => string filepath;

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
<<< "vector of the word 'stanford' is:", "" >>>;
float vector[50];
model.getVector("stanford", vector);
for (0 => int i; i < vector.cap(); i++)
{
    chout <= vector[i] <= " ";
}
chout <= IO.newline();
<<< "=========================================================", "" >>>;

// get similar words via word
<<< "10 words closest to 'stanford' are:", "" >>>;
string words1[10];
model.similar("stanford", 10, words1);
for (0 => int i; i < words1.cap(); i++)
{
    <<< words1[i], "" >>>;
}
<<< "=========================================================", "" >>>;

// get similar words via vector
<<< "10 words closest to 'stanford's vector are:", "" >>>;
string words2[10];
model.similar(vector, 10, words2);
for (0 => int i; i < words2.cap(); i++)
{
    <<< words2[i], "" >>>;
}
<<< "=========================================================", "" >>>;

//// expr
//<<< "similar words to expression 'rome - italy + china' are:", "" >>>;
//model.eval("rome - italy + china", 10) @=> string words3[];
//for (0 => int i; i < words3.cap(); i++)
//{
//    <<< words3[i], "" >>>;
//}
//<<< "=========================================================", "" >>>;