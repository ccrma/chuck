
NRev reverb => dac;
0.01 => reverb.mix;

MidiFileIn min;
MidiMsg msg;

string filename;
if(me.args() == 0)
    me.sourceDir() + "/bwv772.mid" => filename;
else
    me.arg(0) => filename;

if(!min.open(filename))
{
    cherr <= "unable to open MIDI file: '" <= filename <= "'\n";
    me.exit();
}

chout <= filename <= ": " <= min.numTracks() <= " tracks\n";

int done;

for(int t; t < min.numTracks(); t++)
    spork ~ track(t);

while(done < min.numTracks())
    1::second => now;

min.close();

fun void track(int t)
{
    // hack polyphony
    Mandolin s[4];
    for(int i; i < s.cap(); i++) s[i] => reverb;
    
    int v;
    
    while(min.read(msg, t))
    {
        if(msg.when > 0::second)
            msg.when => now;
        
        if((msg.data1 & 0xF0) == 0x90 && msg.data2 > 0 && msg.data3 > 0)
        {
            msg.data2 => Std.mtof => s[v].freq;
            msg.data3/127.0 => s[v].noteOn;
                        
            (v+1)%s.cap() => v;
        }
    }
    
    done++;
}
