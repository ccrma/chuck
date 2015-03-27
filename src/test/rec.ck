
me.arg(0) @=> string filename;
me.arg(1) => Std.atof => float length;

dac => WvOut2 w => blackhole;

me.arg(0) => w.wavFilename;

// temporary workaround to automatically close file on remove-shred
null @=> w;

length::second => now;

// remove everything
Machine.shreds() @=> int shreds[];
for(0 => int i; i < shreds.size(); i++)
{
    if(shreds[i] != me.id())
        Machine.remove(shreds[i]);
}
