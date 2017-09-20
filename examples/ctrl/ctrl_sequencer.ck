// um, by philipd
// (what is it?)

class Player
{ 
    UGen @ base;
    fun void connect( UGen target )
    {
        base => target;
    }

    fun void noteOn ( float note, float vel ) {}
    fun void noteOff ( float vel ) {}
}

class Note
{ 
    float note;
    float vel;
    dur length;

    fun void set ( float nt, float vl, dur ln )
    { 
        nt => note;
        vl => vel;
        ln => length;
    }

    fun void playOn ( Player p)
    { 
        if ( note > 0 )
        { 
            p.noteOn( note , vel );
        }
    }

    fun void playOnAlt( Player p, float noff, float vmul )
    {
        p.noteOn( note+noff, vel*vmul );
    }
}


class MandPlayer extends Player
{
    Mandolin m @=> base;

    fun void noteOn ( float note, float vel )
    { 
        Std.mtof ( note ) => m.freq;
        vel => m.pluck;
    }
}

class FlutePlayer extends Player
{ 
    PercFlut f @=> base; 
    fun void noteOn ( float note, float vel )
    { 
        Std.mtof ( note ) => f.freq;
        vel => f.noteOn;
    }
}

class ClarPlayer extends Player
{
    Clarinet c @=> base;
    fun void noteOn ( float note, float vel )
    { 
        Std.mtof ( note ) => c.freq;
        vel => c.startBlowing;
    }
    fun void noteOff ( float vel )
    { 
        vel => c.stopBlowing;
    }
}


12 => int seqn;
Note sequence[seqn];
int order[seqn];
[0,2,4,7,9, 12, 14, 16, 19, 21] @=> int scale[];
10 => int nscale;
[0.25::second, 0.125::second, 0.125::second,  0.375::second] @=> dur times[];
4 => int ntimes;

//fun initialize ( Note sequence, int n ) { 
fun void newsequence()
{ 
    for ( 0 => int i; i < seqn ; i++ )
    {
        i => order[i];		
        55 + scale[Math.random2(0, nscale - 1)] => int note;
        times[Math.random2(0, ntimes - 1)] => dur mydur;
        Math.random2f( 0.75, 0.9 ) => float vel; 
        sequence[i].set( note, vel, mydur );
    }
}

fun void swap( )
{ 
    Math.random2(0,seqn-1) => int a;
    ( a + Math.random2(1,seqn-1) ) % seqn => int b;
    order[a] => int tmp;
    order[b] => order[a];
    tmp => order[a];
}

MandPlayer mand;
ClarPlayer clar;
Gain g => JCRev j => Echo e => dac;
0.95 => j.gain;
0.1 => j.mix;

1.15::second => e.max;
1.0::second => e.delay;
0.3 => e.mix;

mand.connect(g);
clar.connect(g);

0.6 => g.gain;

newsequence();

while ( true )
{
    for ( 0 => int j ; j < seqn; j++ )
    { 
        sequence[order[j]] @=> Note cur;
        cur.playOn ( mand );
        cur.playOnAlt ( clar, 12 , 0.7 );
        2.0 * cur.length => now;
        cur.playOnAlt( mand, 7, 0.33);
        cur.length => now;
    }

    for ( Math.random2(0,2) => int j ; j > 0 ; j-- )
        swap();

    if ( Math.random2(0, 10) > 8 )
        newsequence();
}

