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
// file: util_opsc.h
// desc: utility for open sound control
//
// author: Philip L. Davidson (philipd@alumni.princeton.edu)
//         Ge Wang (gewang@cs.princeton.edu)
// date: Spring 2005
//-----------------------------------------------------------------------------
#ifndef _OSC_H_INCLUDED_
#define _OSC_H_INCLUDED_


// chuck high-style artistry
// let's just cat included files into an enormous code-wad. 
// keep that bubble gum in your hair.


// the included files were from the CNMAT OSC-Kit distrib. 
#ifdef __PLATFORM_WIN32__
#include <windows.h>
#endif

//#include "OSC-pattern-match.h"
// OSC-pattern-match.h

/*
Copyright � 1998. The Regents of the University of California (Regents). 
All Rights Reserved.

Written by Matt Wright, The Center for New Music and Audio Technologies,
University of California, Berkeley.

Permission to use, copy, modify, distribute, and distribute modified versions
of this software and its documentation without fee and without a signed
licensing agreement, is hereby granted, provided that the above copyright
notice, this paragraph and the following two paragraphs appear in all copies,
modifications, and distributions.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

The OpenSound Control WWW page is 
    http://www.cnmat.berkeley.edu/OpenSoundControl
*/


#ifndef __OSC_PATTERN_MATCH_H_INCLUDED__
#define __OSC_PATTERN_MATCH_H_INCLUDED__

#include <cstdio>
/*
    OSC-pattern-match.h
*/
#ifndef TRUE
#define TRUE true
#define FALSE false
#endif

bool PatternMatch (const char *pattern, const char *test);

#endif //__OSC_PATTERN_MATCH_H_INCLUDED__

// - end OSC-pattern-match.h

//#inclue OSC-TimeTag.h 

/*
Copyright (c) 1998.  The Regents of the University of California (Regents).
All Rights Reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and not-for-profit purposes, without
fee and without a signed licensing agreement, is hereby granted, provided that
the above copyright notice, this paragraph and the following two paragraphs
appear in all copies, modifications, and distributions.  Contact The Office of
Technology Licensing, UC Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley,
CA 94720-1620, (510) 643-7201, for commercial licensing opportunities.

Written by Matt Wright, The Center for New Music and Audio Technologies,
University of California, Berkeley.

     IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
     SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
     ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
     REGENTS HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

     REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
     FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING
     DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS".
     REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
     ENHANCEMENTS, OR MODIFICATIONS.

The OpenSound Control WWW page is 
    http://www.cnmat.berkeley.edu/OpenSoundControl
*/

/*
 OSC_timeTag.h: library for manipulating OSC time tags
 Matt Wright, 5/29/97

 Time tags in OSC have the same format as in NTP: 64 bit fixed point, with the
 top 32 bits giving number of seconds sinve midnight 1/1/1900 and the bottom
 32 bits giving fractional parts of a second.  We represent this by a 64-bit
 unsigned long if possible, or else a struct. 

 NB: On many architectures with 64-bit ints, it's illegal (like maybe a bus error)
 to dereference a pointer to a 64-bit int that's not 64-bit aligned.  
*/

#ifndef OSC_TIMETAG
#define OSC_TIMETAG

#ifdef __sgi
    #define HAS8BYTEINT
    /* You may have to change this typedef if there's some other
       way to specify 64 bit ints on your system */
    typedef long long int64;
    typedef unsigned long long uint64;
    typedef unsigned long uint32;
#else
    /* You may have to redefine this typedef if ints on your system 
       aren't 32 bits. */
    typedef unsigned int uint32;
#endif


#ifdef HAS8BYTEINT
    typedef uint64 OSCTimeTag;
#else
    typedef struct {
    uint32 seconds;
    uint32 fraction;
    } OSCTimeTag;
#endif



/* Return a time tag representing the current time (as of when this
   procedure is called). */
OSCTimeTag OSCTT_CurrentTime(void);

/* Return the time tag 0x0000000000000001, indicating to the receiving device
   that it should process the message immediately. */
OSCTimeTag OSCTT_Immediately(void);

/* Return the time tag 0xffffffffffffffff, a time so far in the future that
   it's effectively infinity. */
OSCTimeTag OSCTT_BiggestPossibleTimeTag(void);

/* Given a time tag and a number of seconds to add to the time tag, return
   the new time tag */
OSCTimeTag OSCTT_PlusSeconds(OSCTimeTag original, float secondsOffset);

/* Compare two time tags.  Return negative if first is < second, 0 if
   they're equal, and positive if first > second. */
int OSCTT_Compare(OSCTimeTag left, OSCTimeTag right);

#endif /*  OSC_TIMETAG */


//#include "OSC-client.h"
// OSC-client.h 
/*
Copyright (c) 1996,1997.  The Regents of the University of California (Regents).
All Rights Reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and not-for-profit purposes, without
fee and without a signed licensing agreement, is hereby granted, provided that
the above copyright notice, this paragraph and the following two paragraphs
appear in all copies, modifications, and distributions.  Contact The Office of
Technology Licensing, UC Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley,
CA 94720-1620, (510) 643-7201, for commercial licensing opportunities.

Written by Matt Wright, The Center for New Music and Audio Technologies,
University of California, Berkeley.

     IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
     SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
     ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
     REGENTS HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

     REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
     FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING
     DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS".
     REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
     ENHANCEMENTS, OR MODIFICATIONS.
*/

/* 

   OSC-client.h: library for constructing OpenSoundControl messages.
   Derived from SynthControl.h
   Author: Matt Wright
   Version 0.1: 6/13/97
   Version 0.2: 7/21/2000: Support for type-tagged messages


   General notes:

   This library abstracts away the data format for the OpenSoundControl
   protocol.  Users of this library can construct OpenSoundControl packets
   with a function call interface instead of knowing how to lay out the bits.

   All issues of memory allocation are deferred to the user of this library.
   There are two data structures that the user must allocate.  The first
   is the actual buffer that the message will be written into.  This buffer
   can be any size, but if it's too small there's a possibility that it
   will become overfull.  The other data structure is called an OSCbuf,
   and it holds all the state used by the library as it's constructing
   a buffer.

   All procedures that have the possibility of an error condition return int,
   with 0 indicating no error and nonzero indicating an error.  The variable
   OSC_errorMessage will be set to point to a string containing an error
   message explaining what the problem is.

*/

#include <cstdio>
//#include "sys.h"

//#include "OSC-timetag.h"

/* The int4byte type has to be a 4-byte integer.  You may have to
   change this to long or something else on your system.  */
#ifdef __MWERKS__
  /* In Metrowerks you can set ints to be 2 or 4 bytes on 68K, but long is
     always 4 bytes */
    typedef long int4byte;
    // ISSUE: 64-bit
#else
    typedef int int4byte;
    // ISSUE: 64-bit
#endif

/* The maximum depth of bundles within bundles within bundles within...
   This is the size of a static array.  If you exceed this limit you'll 
   get an error message. */
#define MAX_BUNDLE_NESTING 32


/* Don't ever manipulate the data in the OSCbuf struct directly.  (It's
   declared here in the header file only so your program will be able to
   declare variables of type OSCbuf and have the right amount of memory
   be allocated.) */

typedef struct OSCbuf_struct {
    char *buffer;            /* The buffer to hold the OSC packet */
    int size;                /* Size of the buffer */
    char *bufptr;            /* Current position as we fill the buffer */
    int state;           /* State of partially-constructed message */
    int4byte *thisMsgSize;   /* Pointer to count field before 
                    currently-being-written message */
    int4byte *prevCounts[MAX_BUNDLE_NESTING];
                 /* Pointers to count field before each currently
                    open bundle */
    int bundleDepth;         /* How many sub-sub-bundles are we in now? */
    char *typeStringPtr;    /* This pointer advances through the type
                   tag string as you add arguments. */
    int gettingFirstUntypedArg; /* nonzero if this message doesn't have
                   a type tag and we're waiting for the 1st arg */
} OSCbuf;



/* Initialize the given OSCbuf.  The user of this module must pass in the
   block of memory that this OSCbuf will use for a buffer, and the number of
   bytes in that block.  (It's the user's job to allocate the memory because
   you do it differently in different systems.) */
void OSC_initBuffer(OSCbuf *buf, int size, char *byteArray);


/* Reset the given OSCbuf.  Do this after you send out the contents of
   the buffer and want to start writing new data into it. */
void OSC_resetBuffer(OSCbuf *buf);


/* Is the buffer empty?  (I.e., would it be stupid to send the buffer
   contents to the synth?) */
int OSC_isBufferEmpty(OSCbuf *buf);


/* How much space is left in the buffer? */
int OSC_freeSpaceInBuffer(OSCbuf *buf);

/* Does the buffer contain a valid OSC packet?  (Returns nonzero if yes.) */
int OSC_isBufferDone(OSCbuf *buf);

/* When you're ready to send out the buffer (i.e., when OSC_isBufferDone()
   returns true), call these two procedures to get the OSC packet that's been
   assembled and its size in bytes.  (And then call OSC_resetBuffer() if you
   want to re-use this OSCbuf for the next packet.)  */
char *OSC_getPacket(OSCbuf *buf);
int OSC_packetSize(OSCbuf *buf);



/* Here's the basic model for building up OSC messages in an OSCbuf:

    - Make sure the OSCbuf has been initialized with OSC_initBuffer().

    - To open a bundle, call OSC_openBundle().  You can then write 
      messages or open new bundles within the bundle you opened.
      Call OSC_closeBundle() to close the bundle.  Note that a packet
      does not have to have a bundle; it can instead consist of just a 
      single message.
                                  

    - For each message you want to send:

    - Call OSC_writeAddress() with the name of your message.  (In
      addition to writing your message name into the buffer, this
      procedure will also leave space for the size count of this message.)

        - Alternately, call OSC_writeAddressAndTypes() with the name of
          your message and with a type string listing the types of all the
          arguments you will be putting in this message.
    
    - Now write each of the arguments into the buffer, by calling one of:
        OSC_writeFloatArg()
        OSC_writeFloatArgs()
        OSC_writeIntArg()
        OSC_writeStringArg()

    - Now your message is complete; you can send out the buffer or you can
      add another message to it.
*/

int OSC_openBundle(OSCbuf *buf, OSCTimeTag tt);
int OSC_closeBundle(OSCbuf *buf);
int OSC_closeAllBundles(OSCbuf *buf);

int OSC_writeAddress(OSCbuf *buf, char *name);
int OSC_writeAddressAndTypes(OSCbuf *buf, char *name, char *types);
int OSC_writeFloatArg(OSCbuf *buf, float arg);
int OSC_writeFloatArgs(OSCbuf *buf, int numFloats, float *args);
int OSC_writeIntArg(OSCbuf *buf, int4byte arg);
int OSC_writeStringArg(OSCbuf *buf, char *arg);

extern const char * OSC_errorMessage;

/* How many bytes will be needed in the OSC format to hold the given
   string?  The length of the string, plus the null char, plus any padding
   needed for 4-byte alignment. */ 
int OSC_effectiveStringLength(char *string);

// end OSC-client.h

#include "stdlib.h"
#include "string.h"

// from veldt:platform.h - UDP Transmitter / Receiver Pair

#include "chuck_oo.h"

class OSC_Address_Space;
class UDP_Transmitter;
class UDP_Receiver; 

class OSC_Transmitter
{
protected: 
    char              _buffer[2048];
    OSCbuf            _osc;
    UDP_Transmitter * _out;
    bool              _holdMessage;

public:
    OSC_Transmitter();
    OSC_Transmitter( UDP_Transmitter * out) ;
    virtual ~OSC_Transmitter();

    void init();
    void setHost( char * host, int port);
    void addMessage( char * address, char * args, ...);
    
    void openBundle( OSCTimeTag t);
    void closeBundle();
    
    void startMessage( char * spec );
    void startMessage( char * address, char * args );
    void addInt( int4byte i ); // gewang: changed from int
    void addFloat( float f );
    void addString( char * s);
    bool packetReady();
    void holdMessage( bool arg );
    void tryMessage();
    void kickMessage();

    void presend( char * buffer , int size );
    
};

#define OSCINBUFSIZE 8192
#define OSCMESGSIZE 8192

//default size for array of osc buffers ( ~8k each )
#define OSCINBOXDEF 2
//maximum size for the array  ( ~2mb is enough, no )
#define OSCINBOXMAX 256

struct OSCMesg { 
    char *address;
    char *types;
    char *data;
    char *payload;
    int len;
    double recvtime;
    long int timetag;
};

struct XMutex;
struct XThread;

class UDP_Subscriber
{
public:
    virtual ~UDP_Subscriber() {}

public:
    virtual int& port() = 0; // get/set the value of the subscriber's current port. 
    virtual void onReceive( char * mesg, int mesgLen ) = 0;

protected: 
    virtual bool subscribe( int port );
    virtual bool unsubscribe();
};


class CBufferSimple;

class OSC_Receiver : private UDP_Subscriber
{
protected:
    // UDP_Receiver*  _in;
    // bool           _listening;
    // char           _inbuf[OSCINBUFSIZE];
    // int            _inbufsize;
    // int            _mesglen;

protected:
	void onReceive( char * mesg, int mesgLen);
	int & port();

protected:
	int			   _port;
	int			   _tmp_port;
    XMutex *        _io_mutex;
    XMutex *        _address_mutex;
    // XThread *       _io_thread;
    OSCMesg         _meep;
    OSCMesg *       _inbox;
    int             _inbox_size;
    bool            _started;
    int             _in_read;   // space that is being read
    int             _in_write;  // space that is being written

    OSC_Address_Space **      _address_space;
    int             _address_size;
    int             _address_num;
    
    CBufferSimple * m_event_buffer;
    
public:
    
    OSC_Receiver();
    OSC_Receiver( UDP_Receiver * in );
    virtual ~OSC_Receiver();
    
    //setup //takedown
    void init();
    void bind_to_port(int port = 0);
    void close_sock();
    
    void recv_mesg();

	bool listen( int port );
    bool listen();
	void stopListening(); //unsubscribe;

    void parse(char *, int len);
    void handle_mesg(char *, int len);
    void handle_bundle(char *, int len);
    void set_mesg(OSCMesg *m, char * buf, int len );
    
    OSCMesg *write()  { return _inbox + _in_write;}
    void next_write();
    
    OSCMesg *read()  { return _inbox + _in_read;}    
    OSCMesg *next_read();


    bool has_mesg();
    bool get_mesg(OSCMesg* bucket);

    void add_address ( OSC_Address_Space * o );
    void remove_address ( OSC_Address_Space * o ); 
    OSC_Address_Space * new_event ( char * spec );
    OSC_Address_Space * new_event ( char * addr, char * type );
    void distribute_message( OSCMesg * msg);
};

enum osc_datatype { OSC_UNTYPED, OSC_NOARGS, OSC_INT, OSC_FLOAT, OSC_STRING, OSC_BLOB, OSC_NTYPE };

struct opsc_data
{
    osc_datatype t;    
    OSCTimeTag timetag;

	union {
		int4byte i;  // gewang: changed from int
		unsigned long u;  // gewang: changed from unsigned int
		float f;
	};

    char * s;

    opsc_data() { 
        t = OSC_UNTYPED;
        s = NULL;
    }
    ~opsc_data() { 
        s = NULL;
    }
};

#define OSC_ADDRESS_QUEUE_MAX 32768
//if it's bigger than this, something's not doing it's job...
//so we'll drop the message and send a friendly warning via EM_log

typedef class OSC_Address_Space OSCSrc;


class OSC_Address_Space : public Chuck_Event
{
protected:
    OSC_Receiver * _receiver;
    XMutex*        _buffer_mutex;
    char  _spec[512];
    bool  _needparse;
    char  _address[512];
    char  _type[512];
    opsc_data * _queue;
    opsc_data * _current_data;
    int   _qread;
    int   _qwrite;
    int   _queueSize;
    int   _cur_value;
    opsc_data *_cur_mesg;
    opsc_data *_vals;
    int   _dataSize;
    bool  _noArgs;
    void resizeData(int n);
    void resizeQueue(int n);
    void parseSpec();
	void scanSpec();

public:
    Chuck_Object * SELF; 
    Chuck_String p_str;
    OSC_Address_Space();
    OSC_Address_Space( const char * spec );
    OSC_Address_Space( const char * addr, const char * type );
    virtual ~OSC_Address_Space();

    // initialization
    void   init();
    void   setSpec( const char * c );
    void   setSpec( const char * addr, const char * type );
    void   setReceiver( OSC_Receiver * recv );

    // distribution
    bool   try_queue_mesg ( OSCMesg * o );
    bool   message_matches ( OSCMesg * o );
    void   queue_mesg ( OSCMesg * o );

    // loop functions
    bool   has_mesg();
    bool   next_mesg();

    void   wait ( Chuck_VM_Shred * shred, Chuck_VM * vm );

    // get functions
    bool     vcheck( osc_datatype tt );
    int4byte next_int(); // gewang: changed from int
    float    next_float();
    char *   next_string();
    char *   next_string_dup();
    char *   next_blob();
};


#endif // _OSC_H_INCLUDED_

