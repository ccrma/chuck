/*----------------------------------------------------------------------------
  ChucK Concurrent, On-the-fly Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: midiio_rtmidi.h
// desc: midi io header
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#ifndef __MIDI_IO_H__
#define __MIDI_IO_H__

#include "chuck_def.h"
#ifndef __DISABLE_MIDI__
#include "rtmidi.h"
#endif
#include "util_buffers.h"




//-----------------------------------------------------------------------------
// stuff
//-----------------------------------------------------------------------------
#define MIDI_NOTEON         0x90
#define MIDI_NOTEOFF        0x80
#define MIDI_POLYPRESS      0xa0
#define MIDI_CTRLCHANGE     0xb0
#define MIDI_PROGCHANGE     0xc0
#define MIDI_CHANPRESS      0xd0
#define MIDI_PITCHBEND      0xe0
#define MIDI_ALLNOTESOFF    0x7b




//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------
union MidiMsg
{
    t_CKBYTE data[4];
//    t_CKUINT dw;
};




// forward reference
class RtMidiOut;
class RtMidiIn;




//-----------------------------------------------------------------------------
// name: class MidiOut
// desc: midi out
//-----------------------------------------------------------------------------
class MidiOut
{
public:
    MidiOut();
    ~MidiOut();

public:
    t_CKBOOL open( t_CKUINT device_num = 0 );
    t_CKBOOL open( const std::string & name );
    t_CKBOOL close();
    t_CKBOOL good() { return m_valid; }
    t_CKINT  num() { return m_valid ? (t_CKINT)m_device_num : -1; }
    
public:
    void     set_suppress( t_CKBOOL print_or_not )
    { m_suppress_output = print_or_not; }
    t_CKBOOL get_suppress()
    { return m_suppress_output; }

public:
    t_CKUINT send( t_CKBYTE status );
    t_CKUINT send( t_CKBYTE status, t_CKBYTE data1 );
    t_CKUINT send( t_CKBYTE status, t_CKBYTE data1, t_CKBYTE data2 );
    t_CKUINT send( const MidiMsg * msg );

public:
    t_CKUINT noteon( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  velocity );
    t_CKUINT noteoff( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  velocity );
    t_CKUINT polypress( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  pressure );
    t_CKUINT ctrlchange( t_CKUINT  channel, t_CKUINT  ctrl_num, t_CKUINT  ctrl_val );
    t_CKUINT progchange( t_CKUINT  channel, t_CKUINT  patch );
    t_CKUINT chanpress( t_CKUINT  channel, t_CKUINT  pressure );
    t_CKUINT pitchbend( t_CKUINT  channel, t_CKUINT  bend_val );
    t_CKUINT allnotesoff( t_CKUINT  channel );

public:
    RtMidiOut * mout;
    std::vector<unsigned char> m_msg;
    t_CKUINT m_device_num;
    t_CKBOOL m_valid;
    t_CKBOOL m_suppress_output;
};




//-----------------------------------------------------------------------------
// name: class MidiIn
// desc: midi
//-----------------------------------------------------------------------------
class MidiIn : public Chuck_Event
{
public:
    MidiIn();
    ~MidiIn();

public:
    t_CKBOOL open( Chuck_VM * vm, t_CKUINT device_num = 0 );
    t_CKBOOL open( Chuck_VM * vm, const std::string & name );
    t_CKBOOL close();
    t_CKBOOL good() { return m_valid; }
    t_CKINT  num() { return m_valid ? (t_CKINT)m_device_num : -1; }

public:
    void     set_suppress( t_CKBOOL print_or_not )
    { m_suppress_output = print_or_not; }
    t_CKBOOL get_suppress()
    { return m_suppress_output; }

public:
    t_CKBOOL empty();
    t_CKUINT recv( MidiMsg * msg );

public:
    CBufferAdvance * m_buffer;
    t_CKUINT m_read_index;
    RtMidiIn * min;
    t_CKBOOL m_valid;
    t_CKUINT m_device_num;
    Chuck_Object * SELF;
    t_CKBOOL m_suppress_output;
};


void probeMidiIn();
void probeMidiOut();




//-----------------------------------------------------------------------------
// name: class MidiInManager
// desc: singleton manager
//-----------------------------------------------------------------------------
class MidiInManager
{
public:
    static t_CKBOOL open( MidiIn * min, Chuck_VM * vm, t_CKINT device_num );
    static t_CKBOOL open( MidiIn * min, Chuck_VM * vm, const std::string & name );
    static t_CKBOOL close( MidiIn * min );
    static void cleanup_buffer( Chuck_VM * vm );

    static void cb_midi_input( double deltatime, std::vector<unsigned char> * msg,
                               void *userData );
protected:
    MidiInManager();
    ~MidiInManager();
    
    static t_CKBOOL add_vm( Chuck_VM * vm, t_CKINT device_num, t_CKBOOL suppress_output );

    static std::vector<RtMidiIn *> the_mins;
    static std::vector< std::map< Chuck_VM *, CBufferAdvance * > > the_bufs;

public:
    static std::map< Chuck_VM *, CBufferSimple * > m_event_buffers;
};




//-----------------------------------------------------------------------------
// name: class MidiOutManager
// desc: singleton manager
//-----------------------------------------------------------------------------
class MidiOutManager
{
public:
    static t_CKBOOL open( MidiOut * mout, t_CKINT device_num );
    static t_CKBOOL open( MidiOut * mout, const std::string & name );
    static t_CKBOOL close( MidiOut * mout );

protected:
    MidiOutManager();
    ~MidiOutManager();

    static std::vector<RtMidiOut *> the_mouts;
};




//-----------------------------------------------------------------------------
// name: class MidiRW
// desc: reads and writes midi messages from file
//-----------------------------------------------------------------------------
class MidiRW
{
public:
    MidiRW();
    ~MidiRW();

public:
    t_CKBOOL open( const char * filename );
    t_CKBOOL close();

public:
    t_CKBOOL read( MidiMsg * msg, t_CKTIME * time );
    t_CKBOOL write( MidiMsg * msg, t_CKTIME * time );

protected:
    FILE * file;
};

// closes all MidiRW file handles
t_CKBOOL midirw_detach( );




//-----------------------------------------------------------------------------
// name: class MidiMsgOut
// desc: reads midi messages from file
//-----------------------------------------------------------------------------
class MidiMsgOut
{
public:
    MidiMsgOut();
    ~MidiMsgOut();

public:
    t_CKBOOL open( const char * filename );
    t_CKBOOL close();

public:
    t_CKBOOL write( MidiMsg * msg, t_CKTIME * time );

protected:
    FILE * file;
};




//-----------------------------------------------------------------------------
// name: class MidiMsgIn
// desc: reads midi messages from file
//-----------------------------------------------------------------------------
class MidiMsgIn
{
public:
    MidiMsgIn();
    ~MidiMsgIn();

public:
    t_CKBOOL open( const char * filename );
    t_CKBOOL close();

public:
    t_CKBOOL read( MidiMsg * msg, t_CKTIME * time );

protected:
    FILE * file;
};




//-----------------------------------------------------------------------------
// name: adapted from MCD/y-score-reader.h
// desc: midi score reader/data structures
//
// authors: Ge Wang (ge@ccrma.stanford.edu)
//          Tom Lieber
//
//    date: Winter 2010
//    version: 1.1
//-----------------------------------------------------------------------------
#include <vector>
#include <deque>
#include <string>
#include <map>




//-----------------------------------------------------------------------------
// name: struct MidiNoteEvent
// desc: a MIDI note event, parsed for start and end time, with simultaneous
//       notes combined into the 'simultaneous' linked list
//-----------------------------------------------------------------------------
struct MidiNoteEvent
{
    // data
    unsigned short data1;
    unsigned short data2;
    unsigned short data3;
    // time data
    double time;
    double untilNext;
    // link to simultaneous events
    MidiNoteEvent * simultaneous;
    // link to parent (could be self)
    MidiNoteEvent * parent;
    
    // more data
    double startTime;
    double endTime;
    bool shouldBend;
    int bendAmount;
    int programChange;
    bool phrasemark;
    
    // constructor
    MidiNoteEvent() : data1(0), data2(0), data3(0), time(0.0), untilNext(0.0),
    simultaneous(NULL), parent(NULL), startTime(0), endTime(0), shouldBend(false),
    bendAmount(0), programChange(0), phrasemark(false) { }
    
    // type of message
    int type() const { return data1 & 0x70; }
    // channel
    int getChannel() const { return data1 & 0x0f; }
};




//-----------------------------------------------------------------------------
// name: struct MidiLyricEvent
// desc: a MIDI lyric event, with "end" time given by the time of the next
//       event in the list
//-----------------------------------------------------------------------------
struct MidiLyricEvent
{
    std::string lyric;
    // time data
    double time;
    double endTime;
    
    // constructor
    MidiLyricEvent() : time(0), endTime(0) { }
};




// forward references
namespace stk { class MidiFileIn; }




//-----------------------------------------------------------------------------
// name: class YScoreReader()
// desc: loads a MIDI score
//-----------------------------------------------------------------------------
class MidiScoreReader
{
public:
    MidiScoreReader();
    ~MidiScoreReader();
    
public:
    bool load( const char * filename, float velScale = 0.0f );
    void cleanup();
    
public:
    // rewind to beginning
    void rewind();
    // get current top event (NO simultaneous)
    const MidiNoteEvent * getTopEvent( long track, long offset = 0 );
    // get current event (YES simultaneous) + move to next
    const MidiNoteEvent * scanEvent( long track, bool & isNewSet );
    // advance to next event
    bool seek( long track, long numToAdvance = 1 );
    // advance to next note on
    bool seekToNoteOn( long track );
    // get notes in a time window
    void getEvents( long track, double startTime, double endTime, std::vector<const MidiNoteEvent *> & result,
                    bool includeSimultaneous = false );
    // isDone
    bool isDone( long track, double currTime );
    
    
public:
    // get number of tracks
    long getNumTracks() const;
    // get number of events per track
    long getNumEvents( long track ) const;
    // get number of tracks with more than 0 events
    long getNumTracksNonZero() const;
    // get a vector of tracks indices that have more than 0 events
    const std::vector<long> & getTracksNonZero() const;
    // get BPM
    double getBPM() const;
    // get an entire track's note events
    const std::vector<MidiNoteEvent *> & getNoteEvents( long track );
    // get an entire track's lyric events
    const std::vector<MidiLyricEvent *> & getLyricEvents( long track );
    // get count
    long getCount( long track, const std::string & key );
    // get lowest note for a track
    long getLowestNote( long track, const MidiNoteEvent *start );
    // get highest note for a track
    long getHighestNote( long track, const MidiNoteEvent *start );
    
public: // TODO: these may as well be a separate object
    // push a event to remember
    void enqueue( const MidiNoteEvent * event );
    // get the front event
    const MidiNoteEvent * front() const;
    // dequeue the event
    void dequeue();
    // push to front
    void enqueue_front( const MidiNoteEvent * event );
    
public: // track naming
    // sets track name
    void setTrackName( long track, const std::string & name );
    // gets track name ("" if not present)
    std::string getTrackName( long track );
    // gets the track with the given name (-1 if not present)
    long getTrackForName( const std::string & name );
    
protected: // for use while loading a track
    // load a track
    bool loadTrack( long track, std::vector<MidiNoteEvent *> & data, std::vector<MidiLyricEvent *> & lyricData );
    // apply control to existing event
    bool applyControl( long track, long data2, long data3, MidiNoteEvent * e );
    // increment count for the given key (used for counting the number of notes, glissandi, etc)
    void incrementCount( long track, const std::string & key );
    // handle note events, managing m_activeNotes and setting the event's endTime
    void handleNoteOn( MidiNoteEvent * e, long note );
    void handleNoteOff( long note, double time );
    
protected:
    // midi
    stk::MidiFileIn * m_midiFile;
    // vector of tracks of events
    std::vector< std::vector<MidiNoteEvent *> > m_events;
    // vector of tracks of events
    std::vector< std::vector<MidiLyricEvent *> > m_lyricEvents;
    // vector of indices
    std::vector<unsigned long> m_indices;
    // stack of events
    std::deque<const MidiNoteEvent *> m_queue;
    // vector of event
    std::vector<const MidiNoteEvent *> m_result;
    // search pointer
    std::vector<unsigned long> m_searchIndices;
    // count map
    std::vector< std::map< std::string, long > > m_countMaps;
    // HACK: scale the velocity (0-1)
    float m_velocity_scale;
    // track names
    std::map<std::string, long> m_nameToTrack;
    std::map<long, std::string> m_trackToName;
    // number of non-zero tracks
    long m_numNonZeroTracks;
    // vector of indices of non-zero tracks
    std::vector<long> m_nonZeroTrackIndices;
    
private: // used during track loading
    // active notes, map from MIDI note to event
    std::map<long, MidiNoteEvent *> m_activeNotes;
};




#endif
