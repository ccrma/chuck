// name: playmidi.ck
// desc: example to render a MIDI file using ChucK

// path
NRev reverb => dac;
// just a bit of reverb
0.025 => reverb.mix;

// the MIDI file in object
MidiFileIn min;
// the MIDI message shuttle
MidiMsg msg;

// the filename
string filename;
// if no command line arguments provided
if( me.args() == 0 ) me.sourceDir() + "bwv772.mid" => filename;
// else use that filename
else me.arg(0) => filename;

// open the file
if( !min.open(filename) )
{
    // error!
    cherr <= "unable to open MIDI file: '" <= filename <= "'\n";
    // done
    me.exit();
}

// print out 
cherr <= "----------" <= IO.newline();
cherr <= "MIDI file: " <= IO.newline()
      <= " |- " <= filename <= IO.newline()
      <= " |- contains " <= min.numTracks() <= " tracks" <= IO.newline();
// print
cherr <= "----------" <= IO.newline();
cherr <= "playing..." <= IO.newline();
cherr <= "----------" <= IO.newline();

// flag
int done;

// for each track, spork a separate shred with the track number
for( int t; t < min.numTracks(); t++ )
    spork ~ doTrack( t, 1 );

// keeping track of how many tracks are done
while( done < min.numTracks() )
    1::second => now;

// done
cherr <= "done; cleaning up..." <= IO.newline();

// close the file
min.close();

// entry point for each shred assigned to a track
fun void doTrack( int track, float speed )
{
    // hack polyphony
    Wurley s[4];
    // for each voice
    for( int i; i < s.size(); i++ )
    {
        // set gain
        .5 => s[i].gain;
        // connect to output
        s[i] => reverb;
    }
    // voice number for quick polyphony
    int v;
    
    // read through all MIDI messages on track
    while( min.read( msg, track ) )
    {
        // this means no more MIDI events at current time; advance time
        if( msg.when > 0::second )
            msg.when * speed => now; // speed of 1 is nominal
        
        // catch NOTEON messages (lower nibble == 0x90)
        if( (msg.data1 & 0xF0) == 0x90 && msg.data2 > 0 && msg.data3 > 0 )
        {
            // get the pitch and convert to frequencey; set
            msg.data2 => Std.mtof => s[v].freq;
            // velocity data; note on
            msg.data3/127.0 => s[v].noteOn;
            // cycle the voices
            (v+1)%s.size() => v;

            // log
            cherr <= "NOTE ON track:" <= track <= " pitch:" <= msg.data2 <=" velocity:" <= msg.data3 <= IO.newline(); 
        }
        // other messages
        else
        {
            // log
            // cherr <= "----EVENT (unhandled) track:" <= track <= " type:" <= (msg.data1&0xF0)
            //      <= " data2:" <= msg.data2 <= " data3:" <= msg.data3 <= IO.newline(); 
        }
    }
    
    // done with track
    done++;
}
