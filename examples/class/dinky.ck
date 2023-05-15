// this class defines the Dinky instrument
// the variables defined at class level are member variables
//
// to test this: 
//    > chuck dinky.ck try.ck
//
// NOTE (2003): in a future version of chuck...
//       this class will be able to extend UGen
// NOTE (2013): the above is now possible using Chugraphs,
//       Chugens, or Chugins!
// NOTE (2023): finally updated this example using Chugraphs

// the Dinky class
public class Dinky extends Chugraph
{
    // impulse to filter to dac
    Impulse i => BiQuad f => Envelope e => outlet;
    // set the filter's pole radius
    .99 => f.prad;
    // set equal gain zeros
    1 => f.eqzs;
    // set filter gain
    .2 => f.gain;
    // set the envelope
    .004::second => e.duration;

    public void radius( float rad )
    { rad => f.prad; }

    public void connect( UGen ugen )
    { e => ugen; }

    // t is for trigger
    public void t( float freq )
    {
        // set the current sample/impulse
        1.0 => i.next;
        // set filter resonant frequency
        freq => f.pfreq;
        // open the envelope
        e.keyOn();
    }

    // t is for trigger (using MIDI note numbers)
    public void t( int note )
    { t( Std.mtof( note ) ); }

    // another lazy name: c (for close)
    public void c() { e.keyOff(); }
}
