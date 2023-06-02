//---------------------------------------------------------------------
// name: read-wav-raw.ck
// desc: example that illustrates manually parsing and rendering a
//       mono WAV file. This demonstrates using FileIO in binary mode.
//       FYI in practice, use SndBuf for easier audio file playback!
// requires: chuck-1.5.0.1 or higher
//
// author: Perry R. Cook
// date: Summer 2023
// credit "saigon44.wav": opening words from /Apocalypse Now/
//---------------------------------------------------------------------

// warn people what is about to happen
<<< "raw mono WAV file reader!!", "" >>>;

// default file 
me.dir() + "saigon44.wav" => string filename;
// look at command line
if( me.args() > 0 ) me.arg(0) => filename;

// instantiate a file IO object
FileIO fio;

// open a file for reading in BINARY mode
fio.open( filename, IO.READ | IO.BINARY );
// ensure it's ok
if( !fio.good() )
{
    // print error message
    cherr <= "can't open file: " <= filename <= " for reading..."
          <= IO.newline();
    // bail
    me.exit();
}

// variable to read into
int val;
// header
"0123456789abcdefghijklmnopqrstuvwxyzABCD" @=> string header;

// .wav header details:
// struct wavhdr {
//    char  riff[4];        /* "RIFF"                                  */
//    int   flength;        /* file length in bytes                    */
//    char  wave[4];        /* "WAVE"                                  */
//    char  fmt[4];         /* "fmt "                                  */
//    int   block_size;     /* in bytes (generally 16)                 */
//    short format_tag;     /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
//    short num_chans;      /* 1=mono, 2=stereo                        */
//    int   srate;          /* Sampling rate in samples per second     */
//    int   bytes_per_sec;  /* bytes per second                        */
//    short bytes_per_samp; /* 2=16-bit mono, 4=16-bit stereo          */
//    short bits_per_samp;  /* Number of bits per sample               */
//    char  data[4];        /* "data"                                  */
//    int   dlength;        /* data length in bytes (filelength - 44)  */
// };

// read in the WAV file header, field by field
// FYI since chuck-1.5.0.1, in binary mode, readInt() for IO.INT8,
// IO.INT16, or IO.INT32 by default returns unsigned integers; to read
// signed integers, call readInt() with IO.SINT8, IO.SINT16, or IO.SINT32
// e.g., IO.SINT16 is used below when reading signed 16-bits samples

// "RIFF"
fio.readInt( IO.INT8 ) => val; header.setCharAt(0,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(1,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(2,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(3,val);
// file length
fio.readInt( IO.INT32 ) => int flength;
// "WAVE"
fio.readInt( IO.INT8 ) => val; header.setCharAt(8,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(9,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(10,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(11,val);
// "fmt "
fio.readInt( IO.INT8 ) => val; header.setCharAt(12,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(13,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(14,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(15,val);
// file info
fio.readInt( IO.INT32 ) => int blockSize;
fio.readInt( IO.INT16 ) => int format;
fio.readInt( IO.INT16 ) => int numChannels;
fio.readInt( IO.INT32 ) => int sRate;
fio.readInt( IO.INT32 ) => int bytesPerSecond;
fio.readInt( IO.INT16 ) => int bytesPerSample;
fio.readInt( IO.INT16 ) => int bitsPerSample;
// "data"
fio.readInt( IO.INT8 ) => val; header.setCharAt(36,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(37,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(38,val);
fio.readInt( IO.INT8 ) => val; header.setCharAt(39,val);
// size of data region in bytes
fio.readInt( IO.INT32 ) => int dLength;

<<< "***", header, "***" >>>;
<<< "---------------------", "" >>>;
<<< "File Length  =", flength >>>;
<<< "Block Size   =", blockSize >>>;
<<< "Format       =", format >>>;
<<< "Num Chans    =", numChannels >>>;
<<< "Sample Rate  =", sRate >>>;
<<< "Bytes/Sec    =", bytesPerSecond >>>;
<<< "Bytes/Sample =", bytesPerSample >>>;
<<< "Bits/Sample  =", bitsPerSample >>>;
<<< "Data Length  =", dLength >>>;
<<< "---------------------", "" >>>;

// check
if( numChannels > 1 )
{
    // print error message
    <<< "WAV file contains", numChannels, "channels..." >>>;
    <<< "this examples only supports mono WAV files...", "" >>>;
    // bail
    me.exit();
}

// render the waveform
Impulse imp => dac;
// number of samples
dLength / bytesPerSample => int numSamples;
// array
float samples[numSamples];
// sample number
0 => int n;

// loop until end
while( n < numSamples )
// while( fio.more() )
// (^ this may not always work; WAV files may contain debris beyond data region)
{
    // read the next sample: signed 16-bit integer
    fio.readInt( IO.SINT16 ) => int sample;
    // map sample value from [-32768,32767] to [-1,1]
    Math.remap(sample, -32768, 32767, -1, 1) => samples[n++] => imp.next;
    // print dot every 1000 samples
    if( n % 1000 == 0 ) {
        chout <= "."; chout.flush();
    }
    // print marker every second
    if( n % sRate == 0 ) {
        chout <= "|"; chout.flush();
    }
    
    // advance one sample
    samp => now;
}
