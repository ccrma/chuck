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
// file: util_opsc.cpp
// desc: utility for open sound control
//
// author: Philip L. Davidson (philipd@alumni.princeton.edu)
//         Ge Wang (gewang@cs.princeton.edu)
// date: Spring 2005
//-----------------------------------------------------------------------------
#include "util_opsc.h"

//chuck networking code
#include "util_network.h"
#include "util_thread.h"

#include "chuck_errmsg.h"
#include "chuck_vm.h"

#include <algorithm>
#include <cstdio>
using namespace std;


// FROM PLATFORM.H -UDP TRANSMITTER / RECEIVER 

#if defined(__PLATFORM_WIN32__)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#if defined(__MACOSX_CORE__)
#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in
#elif !defined(SOCKADDR_IN)
#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in
#endif


// squeeze the whole wad of OSC-Kit code in here. 

// OSC-pattern-match.cpp

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



/*
    OSC-pattern-match.c
    Matt Wright, 3/16/98
    Adapted from oscpattern.c, by Matt Wright and Amar Chaudhury
 */

// #include "OSC-pattern-match.h"

static const char *theWholePattern; /* Just for warning messages */

static bool MatchBrackets( const char * pattern, const char * test );
static bool MatchList( const char * pattern, const char * test );

bool PatternMatch( const char *  pattern, const char * test )
{
    theWholePattern = pattern;
   
    if( pattern == 0 || pattern[0] == 0 ) {
        return test[0] == 0;
    } 
   
    if( test[0] == 0 ) {
        if(pattern[0] == '*')
            return PatternMatch (pattern+1,test);
        else
            return FALSE;
    }
   
    switch( pattern[0] )
    {
    case 0      : return test[0] == 0;
    case '?'    : return PatternMatch (pattern + 1, test + 1);
    case '*'    : 
        if( PatternMatch (pattern+1, test) ) {
            return TRUE;
        } else {
            return PatternMatch (pattern, test+1);
        }
    case ']'    :
    case '}'    :
        CK_FPRINTF_STDERR( "[chuck](via OSC): spurious %c in pattern \".../%s/...\"\n", pattern[0], theWholePattern );
        return FALSE;
    case '['    :
        return MatchBrackets (pattern,test);
    case '{'    :
        return MatchList (pattern,test);
    case '\\'   :  
        if(pattern[1] == 0) {
            return test[0] == 0;
        } else if(pattern[1] == test[0]) {
            return PatternMatch (pattern+2,test+1);
        } else {
            return FALSE;
        }
    default     :
        if(pattern[0] == test[0]) {
          return PatternMatch (pattern+1,test+1);
        } else {
          return FALSE;
        }
    }
}


/* we know that pattern[0] == '[' and test[0] != 0 */

static bool MatchBrackets( const char * pattern, const char * test )
{
    bool result;
    bool negated = FALSE;
    const char *p = pattern;

    if(pattern[1] == 0) {
        CK_FPRINTF_STDERR( "[chuck](via OSC): unterminated [ in pattern \".../%s/...\"\n", theWholePattern );
        return FALSE;
    }

    if(pattern[1] == '!') {
        negated = TRUE;
        p++;
    }

    while (*p != ']') {
        if(*p == 0) {
            CK_FPRINTF_STDERR( "[chuck](via OSC): unterminated [ in pattern \".../%s/...\"\n", theWholePattern );
        return FALSE;
        }

        if(p[1] == '-' && p[2] != 0) {
            if(test[0] >= p[0] && test[0] <= p[2]) {
                result = !negated;
                goto advance;
            }
        }
    
        if(p[0] == test[0]) {
            result = !negated;
            goto advance;
        }
        p++;
    }

    result = negated;

advance:

    if(!result)
        return FALSE;

    while (*p != ']') {
        if(*p == 0) {
            CK_FPRINTF_STDERR( "[chuck](via OSC): unterminated [ in pattern \".../%s/...\"\n", theWholePattern );
            return FALSE;
        }
        p++;
    }

    return PatternMatch (p+1,test+1);
}


static bool MatchList( const char * pattern, const char * test )
{
    const char *restOfPattern, *tp = test;

    for(restOfPattern = pattern; *restOfPattern != '}'; restOfPattern++) {
        if(*restOfPattern == 0) {
            CK_FPRINTF_STDERR( "[chuck](via OSC): unterminated { in pattern \".../%s/...\"\n", theWholePattern );
            return FALSE;
        }
    }

    restOfPattern++; /* skip close curly brace */

    pattern++; /* skip open curly brace */

    while( 1 )
    {
        if(*pattern == ',') {
            if(PatternMatch (restOfPattern, tp)) {
                return TRUE;
            } else {
                tp = test;
                ++pattern;
            }
        } else if(*pattern == '}') {
            return PatternMatch (restOfPattern, tp);
        } else if(*pattern == *tp) {
            ++pattern;
            ++tp;
        } else {
            tp = test;
            while (*pattern != ',' && *pattern != '}') {
                pattern++;
            }
            if(*pattern == ',') {
                pattern++;
            }
        }
    }
}

// end OSC-pattern-match.h


// OSC-timetag.cpp

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
 OSC_timeTag.c: library for manipulating OSC time tags
 Matt Wright, 5/29/97

 Version 0.2 (9/11/98): cleaned up so no explicit type names in the .c file.
*/

//#include "OSC-timetag.h"


#ifdef HAS8BYTEINT
#define TWO_TO_THE_32_FLOAT 4294967296.0f

OSCTimeTag OSCTT_Immediately(void) {
    return (OSCTimeTag) 1;
}

OSCTimeTag OSCTT_BiggestPossibleTimeTag(void) {
    return (OSCTimeTag) 0xffffffffffffffff;
}

OSCTimeTag OSCTT_PlusSeconds(OSCTimeTag original, float secondsOffset) {
    int64 offset = (int64) (secondsOffset * TWO_TO_THE_32_FLOAT);

/*    printf("* OSCTT_PlusSeconds %llx plus %f seconds (i.e., %lld offset) is %llx\n", original,
          secondsOffset, offset, original + offset);  */

    return original + offset;
}

int OSCTT_Compare(OSCTimeTag left, OSCTimeTag right)
{
#if 0
    printf("***** OSCTT_Compare(%llx, %llx): %d\n", left, right,
       (left<right) ? -1 : ((left == right) ? 0 : 1));
#endif
    if(left < right) {
        return -1;
    } else if(left == right) {
        return 0;
    } else {
        return 1;
    }
}

#ifdef __sgi
#include <sys/time.h>

#define SECONDS_FROM_1900_to_1970 2208988800 /* 17 leap years */
#define TWO_TO_THE_32_OVER_ONE_MILLION 4295


OSCTimeTag OSCTT_CurrentTime(void) {
    uint64 result;
    uint32 usecOffset;
    struct timeval tv;
    struct timezone tz;

    BSDgettimeofday(&tv, &tz);

    /* First get the seconds right */
    result = (unsigned) SECONDS_FROM_1900_to_1970 + 
         (unsigned) tv.tv_sec - 
         (unsigned) 60 * tz.tz_minuteswest +
             (unsigned) (tz.tz_dsttime ? 3600 : 0);

#if 0
    /* No timezone, no DST version ... */
    result = (unsigned) SECONDS_FROM_1900_to_1970 + 
         (unsigned) tv.tv_sec;
#endif


    /* make seconds the high-order 32 bits */
    result = result << 32;
    
    /* Now get the fractional part. */
    usecOffset = (unsigned) tv.tv_usec * (unsigned) TWO_TO_THE_32_OVER_ONE_MILLION;
    /* printf("** %ld microsec is offset %x\n", tv.tv_usec, usecOffset); */

    result += usecOffset;

/*    printf("* OSCTT_CurrentTime is %llx\n", result); */
    return result;
}

#else /* __sgi */

/* Instead of asking your operating system what time it is, it might be
   clever to find out the current time at the instant your application 
   starts audio processing, and then keep track of the number of samples
   output to know how much time has passed. */

/* Loser version for systems that have no ability to tell the current time: */
OSCTimeTag OSCTT_CurrentTime(void) {
    return (OSCTimeTag) 1;
}

#endif /* __sgi */


#else /* Not HAS8BYTEINT */

OSCTimeTag OSCTT_CurrentTime(void) {
    OSCTimeTag result;
    result.seconds = 0;
    result.fraction = 1;
    return result;
}

OSCTimeTag OSCTT_BiggestPossibleTimeTag(void) {
    OSCTimeTag result;
    result.seconds = 0xffffffff;
    result.fraction = 0xffffffff;
    return result;
}

OSCTimeTag OSCTT_Immediately(void) {
    OSCTimeTag result;
    result.seconds = 0;
    result.fraction = 1;
    return result;
}

OSCTimeTag OSCTT_PlusSeconds(OSCTimeTag original, float secondsOffset) {
    OSCTimeTag result;
    result.seconds = 0;
    result.fraction = 1;
    return result;
}

int OSCTT_Compare(OSCTimeTag left, OSCTimeTag right) {
    /* Untested! */
    int highResult = left.seconds - right.seconds;

    if(highResult != 0) return highResult;

    return left.fraction - right.fraction;
}


#endif /* HAS8BYTEINT */

// end OSC-timetag.cpp

// OSC-client.cpp

/*
Copyright (c) 1996.  The Regents of the University of California (Regents).
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
  Author: Matt Wright
  Version 2.2: Calls htonl in the right places 20000620
  Version 2.3: Gets typed messages right.
 */


/* Here are the possible values of the state field: */

#define EMPTY 0        /* Nothing written to packet yet */
#define ONE_MSG_ARGS 1 /* Packet has a single message; gathering arguments */
#define NEED_COUNT 2   /* Just opened a bundle; must write message name or
                          open another bundle */
#define GET_ARGS 3     /* Getting arguments to a message.  If we see a message
              name or a bundle open/close then the current message
              will end. */
#define DONE 4         /* All open bundles have been closed, so can't write 
                  anything else */


//#include "OSC-client.h"

const char * OSC_errorMessage;


//static int strlen(char *s);
static int OSC_padString(char *dest, const char *str);
static int OSC_padStringWithAnExtraStupidComma(char *dest, const char *str);
static int OSC_WritePadding(char *dest, int i);
static int CheckTypeTag(OSCbuf *buf, char expectedType);

void OSC_initBuffer(OSCbuf *buf, int size, char *byteArray) {
    buf->buffer = byteArray;
    buf->size = size;
    OSC_resetBuffer(buf);
}

void OSC_resetBuffer(OSCbuf *buf) { 
    buf->bufptr = buf->buffer;
    buf->state = EMPTY;
    buf->bundleDepth = 0;
    buf->prevCounts[0] = 0;
    buf->gettingFirstUntypedArg = 0;
    buf->typeStringPtr = 0;
}

int OSC_isBufferEmpty(OSCbuf *buf) {
    return buf->bufptr == buf->buffer;
}

int OSC_freeSpaceInBuffer(OSCbuf *buf) {
    return buf->size - (buf->bufptr - buf->buffer);
}

int OSC_isBufferDone(OSCbuf *buf) {
    return (buf->state == DONE || buf->state == ONE_MSG_ARGS);
}

char *OSC_getPacket(OSCbuf *buf)
{
#ifdef ERROR_CHECK_GETPACKET
    if(buf->state == DONE || buf->state == ONE_MSG_ARGS) {
        return buf->buffer;
    } else {
        OSC_errorMessage = "Packet has unterminated bundles";
        return 0;
    }
#else
    return buf->buffer;
#endif
}

int OSC_packetSize(OSCbuf *buf)
{
#ifdef ERROR_CHECK_PACKETSIZE
    if(buf->state == DONE || buf->state == ONE_MSG_ARGS) {
        return (buf->bufptr - buf->buffer);
    } else {
        OSC_errorMessage = "Packet has unterminated bundles";
        return 0;
    }
#else
    return (buf->bufptr - buf->buffer);
#endif
}

#define CheckOverflow(buf, bytesNeeded) { \
    if((bytesNeeded) > OSC_freeSpaceInBuffer(buf)) { \
    OSC_errorMessage = "buffer overflow"; \
    return 1; \
    } \
}

static void PatchMessageSize(OSCbuf *buf)
{
    int4byte size;
    size = buf->bufptr - ((char *) buf->thisMsgSize) - 4;
    *(buf->thisMsgSize) = htonl(size);
}

int OSC_openBundle(OSCbuf *buf, OSCTimeTag tt)
{
    if(buf->state == ONE_MSG_ARGS) {
        OSC_errorMessage = "Can't open a bundle in a one-message packet";
        return 3;
    }

    if(buf->state == DONE) {
        OSC_errorMessage = "This packet is finished; can't open a new bundle";
        return 4;
    }

    if(++(buf->bundleDepth) >= MAX_BUNDLE_NESTING) {
        OSC_errorMessage = "Bundles nested too deeply; change MAX_BUNDLE_NESTING in OpenSoundControl.h";
        return 2;
    }

    if(CheckTypeTag(buf, '\0')) return 9;

    if(buf->state == GET_ARGS) {
        PatchMessageSize(buf);
    }

    if(buf->state == EMPTY) {
        /* Need 16 bytes for "#bundle" and time tag */
        CheckOverflow(buf, 16);
    } else {
        /* This bundle is inside another bundle, so we need to leave
           a blank size count for the size of this current bundle. */
        CheckOverflow(buf, 20);
        *((int4byte *)buf->bufptr) = 0xaaaaaaaa;
        buf->prevCounts[buf->bundleDepth] = (int4byte *)buf->bufptr;

        buf->bufptr += 4;
    }

    buf->bufptr += OSC_padString(buf->bufptr, "#bundle");

    *((OSCTimeTag *) buf->bufptr) = tt;

    if(htonl(1) != 1)
    {
        /* Byte swap the 8-byte integer time tag */
        int4byte *intp = (int4byte *)buf->bufptr;
        intp[0] = htonl(intp[0]);
        intp[1] = htonl(intp[1]);

#ifdef HAS8BYTEINT
    { /* tt is a 64-bit int so we have to swap the two 32-bit words. 
        (Otherwise tt is a struct of two 32-bit words, and even though
         each word was wrong-endian, they were in the right order
         in the struct.) */
        int4byte temp = intp[0];
        intp[0] = intp[1];
        intp[1] = temp;
    }
#endif
    }

    buf->bufptr += sizeof(OSCTimeTag);

    buf->state = NEED_COUNT;

    buf->gettingFirstUntypedArg = 0;
    buf->typeStringPtr = 0;
    return 0;
}

int OSC_closeBundle(OSCbuf *buf)
{
    if(buf->bundleDepth == 0) {
        /* This handles EMPTY, ONE_MSG, ARGS, and DONE */
        OSC_errorMessage = "Can't close bundle; no bundle is open!";
        return 5;
    }

    if(CheckTypeTag(buf, '\0')) return 9;

    if(buf->state == GET_ARGS) {
        PatchMessageSize(buf);
    }

    if(buf->bundleDepth == 1) {
        /* Closing the last bundle: No bundle size to patch */
        buf->state = DONE;
    } else {
        /* Closing a sub-bundle: patch bundle size */
        int size = buf->bufptr - ((char *) buf->prevCounts[buf->bundleDepth]) - 4;
        *(buf->prevCounts[buf->bundleDepth]) = htonl(size);
        buf->state = NEED_COUNT;
    }

    --buf->bundleDepth;
    buf->gettingFirstUntypedArg = 0;
    buf->typeStringPtr = 0;
    return 0;
}


int OSC_closeAllBundles(OSCbuf *buf)
{
    if(buf->bundleDepth == 0) {
        /* This handles EMPTY, ONE_MSG, ARGS, and DONE */
        OSC_errorMessage = "Can't close all bundles; no bundle is open!";
        return 6;
    }

    if(CheckTypeTag(buf, '\0')) return 9;

    while (buf->bundleDepth > 0) {
        OSC_closeBundle(buf);
    }
    
    buf->typeStringPtr = 0;
    return 0;
}

int OSC_writeAddress(OSCbuf *buf, char *name)
{
    int4byte paddedLength;

    if(buf->state == ONE_MSG_ARGS) {
        OSC_errorMessage = "This packet is not a bundle, so you can't write another address";
        return 7;
    }

    if(buf->state == DONE) {
        OSC_errorMessage = "This packet is finished; can't write another address";
        return 8;
    }

    if(CheckTypeTag(buf, '\0')) return 9;

    paddedLength = OSC_effectiveStringLength(name);

    if(buf->state == EMPTY) {
        /* This will be a one-message packet, so no sizes to worry about */
        CheckOverflow(buf, paddedLength);
        buf->state = ONE_MSG_ARGS;
    } else {
        /* GET_ARGS or NEED_COUNT */
        CheckOverflow(buf, 4+paddedLength);
        if(buf->state == GET_ARGS) {
            /* Close the old message */
            PatchMessageSize(buf);
        }
        buf->thisMsgSize = (int4byte *)buf->bufptr;
        *(buf->thisMsgSize) = 0xbbbbbbbb;
        buf->bufptr += 4;
        buf->state = GET_ARGS;
    }

    /* Now write the name */
    buf->bufptr += OSC_padString(buf->bufptr, name);
    buf->typeStringPtr = 0;
    buf->gettingFirstUntypedArg = 1;

    return 0;
}

int OSC_writeAddressAndTypes(OSCbuf *buf, char *name, char *types)
{
    int result;
    int4byte paddedLength;

    if(CheckTypeTag(buf, '\0')) return 9;

    result = OSC_writeAddress(buf, name);

    if(result) return result;

    paddedLength = OSC_effectiveStringLength(types);

    CheckOverflow(buf, paddedLength);    

    buf->typeStringPtr = buf->bufptr + 1; /* skip comma */
    buf->bufptr += OSC_padString(buf->bufptr, types);
    
    buf->gettingFirstUntypedArg = 0;

    return 0;
}

static int CheckTypeTag(OSCbuf *buf, char expectedType)
{
    if(buf->typeStringPtr) {
        if(*(buf->typeStringPtr) != expectedType) {
            if(expectedType == '\0') {
                OSC_errorMessage =
                    "According to the type tag I expected more arguments.";
            } else if(*(buf->typeStringPtr) == '\0') {
                OSC_errorMessage =
                    "According to the type tag I didn't expect any more arguments.";
            } else {
                OSC_errorMessage =
                    "According to the type tag I expected an argument of a different type.";
                printf("* Expected %c, string now %s\n", expectedType, buf->typeStringPtr);
            }

            return 9; 
        }
    
        ++(buf->typeStringPtr);
    }

    return 0;
}


int OSC_writeFloatArg(OSCbuf *buf, float arg)
{
    int4byte *intp;

    CheckOverflow(buf, 4);

    if(CheckTypeTag(buf, 'f')) return 9;

    /* Pretend arg is a long int so we can use htonl() */
    intp = ((int4byte *) &arg);
    *((int4byte *) buf->bufptr) = htonl(*intp);

    buf->bufptr += 4;

    buf->gettingFirstUntypedArg = 0;

    return 0;
}


int OSC_writeFloatArgs(OSCbuf *buf, int numFloats, float *args)
{
    int i;
    int4byte *intp;

    CheckOverflow(buf, 4 * numFloats);

    /* Pretend args are long ints so we can use htonl() */
    intp = ((int4byte *) args);

    for(i = 0; i < numFloats; i++) {
        if(CheckTypeTag(buf, 'f')) return 9;
        *((int4byte *) buf->bufptr) = htonl(intp[i]);
        buf->bufptr += 4;
    }

    buf->gettingFirstUntypedArg = 0;

    return 0;
}


int OSC_writeIntArg(OSCbuf *buf, int4byte arg)
{
    CheckOverflow(buf, 4);
    if(CheckTypeTag(buf, 'i')) return 9;

    *((int4byte *) buf->bufptr) = htonl(arg);
    buf->bufptr += 4;

    buf->gettingFirstUntypedArg = 0;
    return 0;
}


int OSC_writeStringArg(OSCbuf *buf, char *arg)
{
    int len;

    if(CheckTypeTag(buf, 's')) return 9;

    len = OSC_effectiveStringLength(arg);

    if(buf->gettingFirstUntypedArg && arg[0] == ',') {
        /* This un-type-tagged message starts with a string
           that starts with a comma, so we have to escape it
           (with a double comma) so it won't look like a type
           tag string. */

        CheckOverflow(buf, len+4); /* Too conservative */
        buf->bufptr += 
            OSC_padStringWithAnExtraStupidComma(buf->bufptr, arg);
    } else {
        CheckOverflow(buf, len);
        buf->bufptr += OSC_padString(buf->bufptr, arg);
    }

    buf->gettingFirstUntypedArg = 0;

    return 0;
}


/* String utilities */
/*
static int strlen(char *s) {
    int i;
    for(i=0; s[i] != '\0'; i++);
    return i;
}
*/

#define STRING_ALIGN_PAD 4
int OSC_effectiveStringLength(char *string)
{
    int len = strlen(string) + 1;  /* We need space for the null char. */
    
    /* Round up len to next multiple of STRING_ALIGN_PAD to account for alignment padding */
    if((len % STRING_ALIGN_PAD) != 0) {
        len += STRING_ALIGN_PAD - (len % STRING_ALIGN_PAD);
    }

    return len;
}


static int OSC_padString(char *dest, const char *str)
{
    int i;
    
    for(i = 0; str[i] != '\0'; i++) {
        dest[i] = str[i];
    }
    
    return OSC_WritePadding(dest, i);
}


static int OSC_padStringWithAnExtraStupidComma(char *dest, const char *str)
{
    int i;
    
    dest[0] = ',';
    for(i = 0; str[i] != '\0'; i++) {
        dest[i+1] = str[i];
    }

    return OSC_WritePadding(dest, i+1);
}
 

static int OSC_WritePadding(char *dest, int i)
{
    dest[i] = '\0';
    i++;

    for(; (i % STRING_ALIGN_PAD) != 0; i++) {
        dest[i] = '\0';
    }

    return i;
}


// end OSC-client.cpp



// ACTUAL CODE

/*
//chuck networking code
#include "util_network.h"
#include "util_thread.h"

// FROM PLATFORM.H -UDP TRANSMITTER / RECEIVER 

#if defined(__PLATFORM_WIN32__)
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef __MACOSX_CORE__
#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in
#endif
*/
#include "stdarg.h"

enum udp_stat { UDP_NOINIT, UDP_UNBOUND, UDP_BOUND, UDP_READY, UDP_ERROR, UDP_NUM };


/* in the .h so that OSC can subclass it

class UDP_Subscriber { 
        virtual void onRecieve( char * mesg, int mesgLen ) = 0;
        virtual bool subscribe( int port );
        virtual bool unsubscribe();
        virtual int& port(); //get/set the value of the subscriber's current port. 
}
  
*/

class DefUDP_Subscriber : public UDP_Subscriber { 
public:
        int _port;
        void onReceive ( char * mesg, int mesgLen ) {} // ignore
        int& port() { return _port; }
};

class UDP_Port_Listener { 
private:
    UDP_Receiver*  m_in;
    bool           m_port;
        
    XThread*       m_thread;
    XMutex*        m_mutex;

    char           m_inbuf[OSCINBUFSIZE];
    int            m_inbufsize;

    vector < UDP_Subscriber * > m_subscribers;
    queue < UDP_Subscriber * > m_free;

public:
    UDP_Port_Listener( int port );
    ~UDP_Port_Listener();
    void init();
    bool bind_to_port(int port);
    void close();
    bool listen();
    int recv_mesg();
    bool add ( UDP_Subscriber * );
    bool drop ( UDP_Subscriber * );
    int  nsubs(); 

    bool           listening;    
};




class UDP_Port_Manager
{
private:
    static UDP_Port_Manager * _inst;
    map<int, UDP_Port_Listener *> _listeners;

public:
    static UDP_Port_Manager * instance();
    UDP_Port_Manager();
    void init();
    bool subscribe ( UDP_Subscriber * sub, int port );
    bool unsubscribe ( UDP_Subscriber * sub );
};




class UDP_Receiver
{
protected:
    ck_socket     _sock;
    udp_stat      _status;

    SOCKADDR_IN   _remote_addr;
    int           _remote_addr_len;
    int           _mesgLen;

public:
    UDP_Receiver();
    ~UDP_Receiver() { }
    void init();
    bool bind_to_port(int port);
    int recv_next(char *buffer, int size);   
    void close_sock();
    udp_stat status();    
};




class UDP_Transmitter
{
protected:
    ck_socket   _sock;
    SOCKADDR_IN _host_addr;
    udp_stat    _status;
    
public:
    UDP_Transmitter() {
        _status = UDP_NOINIT; 
    }
    virtual ~UDP_Transmitter() {}
    void init();
    
    void set_host(char * hostaddress, int port);
    int send(char *buffer, int size);
    udp_stat status(void) { return _status; } 
    void close_sock();
};


UDP_Port_Manager * UDP_Port_Manager::_inst = NULL;
UDP_Port_Manager * UDP_Port_Manager::instance()
{
    if( _inst == NULL )
    {
        _inst = new UDP_Port_Manager();
        assert( _inst );
    }
    
    return _inst; 
}

UDP_Port_Manager::UDP_Port_Manager() {
    init();
}

void
UDP_Port_Manager::init() {
    _listeners.clear();
}

bool UDP_Port_Manager::subscribe ( UDP_Subscriber * sub, int port )
{
    if( _listeners.count( port ) == 0 )
    {
        EM_log(CK_LOG_INFO, "PortManager:  starting Listener on %d", port );
        _listeners[port] = new UDP_Port_Listener( port ); //make a new listener on this port
    }

    return _listeners[port]->add( sub );
}

bool UDP_Port_Manager::unsubscribe( UDP_Subscriber * sub )
{
    if( sub->port() < 0 )
    {
        EM_log( CK_LOG_INFO, "error, subscriber's port < 0, return false" );
        return false; 
    }
    
    if( _listeners.count( sub->port() ) == 0 )
    {
        EM_log( CK_LOG_INFO, "error, cannot unsubscribe: port %d not in use", sub->port() );
        return false; 
    }
    else
    {
        int pp = sub->port();
        UDP_Port_Listener * listener = _listeners[pp];
        bool ret = listener->drop( sub );
        //check empty 
        if( listener->nsubs() == 0 ) { 
            EM_log( CK_LOG_INFO, "PortManager: Listener on port %d is empty - removing", sub->port() );         
            delete listener;
            _listeners.erase( pp );
        }
        return ret; 
    }
}

UDP_Port_Listener::UDP_Port_Listener( int port )
    : m_inbufsize( OSCINBUFSIZE ), listening( false )
{
    init();
    m_in->bind_to_port( port );
    listen();
}

UDP_Port_Listener::~UDP_Port_Listener()
{
    //CK_FPRINTF_STDERR( "port listener destructor\n" );
    close();
    // usleep( 10 );    // do we need this ? 
    delete m_in;
    delete m_mutex;
    delete m_thread;
}

void UDP_Port_Listener::init()
{
    memset( (void*)m_inbuf, '\0', m_inbufsize * sizeof(char) );
    m_thread = new XThread();
    m_mutex  = new XMutex();
    m_in = new UDP_Receiver();
    m_in->init();
}

int UDP_Port_Listener::nsubs() { return m_subscribers.size(); }

bool UDP_Port_Listener::add( UDP_Subscriber * sub )
{
    m_mutex->acquire();
    if( find( m_subscribers.begin(), m_subscribers.end(), sub ) == m_subscribers.end() )
    {
        EM_log(CK_LOG_INFO, "UDP_Port_Listener: adding subscriber" );
        m_subscribers.push_back( sub );
        sub->port() = m_port;   
        m_mutex->release();
        return true;
    }
    else
    {
        EM_log(CK_LOG_INFO, "UDP_Port_Listener: add - error subscriber already exists" );
        m_mutex->release();
        return false;
    }
}


bool UDP_Port_Listener::drop( UDP_Subscriber * sub )
{
    // lock
    m_mutex->acquire();
    // find
    vector < UDP_Subscriber * >::iterator m_iter = find( m_subscribers.begin(), m_subscribers.end(), sub );
    // check
    if( m_iter != m_subscribers.end() )
    {
        EM_log(CK_LOG_INFO, "UDP_Port_Listener: dropping subscriber" );
        m_subscribers.erase( m_iter );
        sub->port() = -1;
        // unlock
        m_mutex->release();
        return true;
    }
    else
    {
        EM_log(CK_LOG_INFO, "UDP_Port_Listener: add - error subscriber already exists" );
        // unlock
        m_mutex->release();
        return false;
    }
}


THREAD_RETURN (THREAD_TYPE udp_port_listener_thread)( void * data )
{
    UDP_Port_Listener * upl = (UDP_Port_Listener *) data;

    // priority boost
    if( XThreadUtil::our_priority != 0x7fffffff )
        XThreadUtil::set_priority( XThreadUtil::our_priority );

    EM_log( CK_LOG_INFO, "UDP_Port_Listener:: starting receive loop...\n" );
    int mLen;
    do {
        mLen = upl->recv_mesg();
        usleep( 10 );
    } while( mLen != 0 );

    EM_log( CK_LOG_INFO, "UDP_Port_Listener:: receive loop terminated...\n" );

    return (THREAD_RETURN)0;
}



// returns whether an attempt to start the listening thread was made
bool UDP_Port_Listener::listen()
{
    if( !listening && m_in->status() != UDP_ERROR )
    {
        listening = true;
        m_thread->start( udp_port_listener_thread, (void*) this );
        return true;
    }

    return false;
}

bool UDP_Port_Listener::bind_to_port( int port )
{
    return m_in->bind_to_port( port );
}

void UDP_Port_Listener::close()
{
    if( m_in->status() == UDP_BOUND )
    {
        listening = false;
        m_in->close_sock();
    }
}

int UDP_Port_Listener::recv_mesg()
{
    int mLen = m_in->recv_next( m_inbuf, m_inbufsize );
    
    if( mLen == 0 )
    {
        EM_log( CK_LOG_INFO, "recvLen 0 on socket, returning 0" );
        return 0;
    }

    m_mutex->acquire();
    vector<UDP_Subscriber *>::size_type i;
    for( i = 0 ; i < m_subscribers.size(); i++ )
    {
        m_subscribers[i]->onReceive( m_inbuf, mLen );
    }
    m_mutex->release();

    return mLen;
}


// UDP_Subscriber
bool UDP_Subscriber::subscribe( int p )
{
    if( port() >= 0 )
    {
        return false; // already subscribed
    }
    else if( UDP_Port_Manager::instance()->subscribe( this, p ) )
    { 
        port() = p;
        return true;
    }
    else 
    { 
        port() = -1;
        return false;
    }
}

bool UDP_Subscriber::unsubscribe()
{
    if( port() < 0 )
    {
        return false; // currently unsubscribed
    }
    else if( UDP_Port_Manager::instance()->unsubscribe( this ) )
    {
        port() = -1;
        return true;
    }
    else
    {
        return false; // could not unsubscribe? 
    }
}



UDP_Receiver::UDP_Receiver() 
{ 
    _status = UDP_NOINIT; 
}


void UDP_Receiver::init()
{ 
   //open port 
   _sock = ck_udp_create();    
   _status = ( _sock != NULL ) ?  UDP_UNBOUND : UDP_ERROR;
}

bool UDP_Receiver::bind_to_port(int port)
{
    // fill in the interface information
    _status =  ( ck_bind ( _sock, port ) ) ?  UDP_BOUND : UDP_ERROR;
    // check status
    if( _status == UDP_ERROR )
    {
        EM_log( CK_LOG_SYSTEM, "(via OSC): cannot bind to port %d!", port);
        return false;
    }

    return true;
}

udp_stat UDP_Receiver::status() { return _status; }

int UDP_Receiver::recv_next(char * buffer, int bsize)
{ 
   if( _status != UDP_BOUND ) 
   {
      EM_log( CK_LOG_SYSTEM, "(via OSC): recv -> socket not bound!"); return 0; 
   }

   _remote_addr_len = sizeof(_remote_addr);
   int ret = ck_recvfrom(_sock, buffer, bsize, (struct sockaddr *) &_remote_addr, &_remote_addr_len );

   return ( ret <= 0 ) ? 0 : ret;
}


void UDP_Receiver::close_sock()
{
    ck_close ( _sock );
    _status = UDP_NOINIT;
}


void UDP_Transmitter::init()
{
   //open socket
    _sock = ck_udp_create();
    _status = ( _sock != NULL ) ? UDP_READY : UDP_ERROR; 
}


void UDP_Transmitter::set_host( char * hostaddress, int port )
{
   // fill in the interface information
   struct hostent * host;
   // printf( "gathering host information..." );
   _host_addr.sin_family = AF_INET;
   _host_addr.sin_port = htons(port);
   _host_addr.sin_addr.s_addr = inet_addr(hostaddress);

   if( _host_addr.sin_addr.s_addr == INADDR_NONE ) // the address wasn't in numeric
   {
      host = NULL;
      host = gethostbyname(hostaddress); // get the IP address of the server
      if(host == NULL)
      {
         CK_FPRINTF_STDERR( "[chuck](via OSC): UDP_Transmitter: unknown host: %s\n", hostaddress);
         _status = UDP_ERROR;
         return;
      }
      memcpy(&_host_addr.sin_addr, host->h_addr_list[0], host->h_length);
      // printf("   ...set\n");
   } 
   // else printf("   ...set\n");
   _status = UDP_READY;
}


int UDP_Transmitter::send( char * buffer, int bsize )
{
    if( _status != UDP_READY )
    {
        EM_log( CK_LOG_SYSTEM, "(via OSC): send -> socket not bound!");
        return 0;
    }
    int ret = ck_sendto(_sock, buffer, bsize, (struct sockaddr *) &_host_addr, sizeof(_host_addr) );
    return ( ret <= 0 ) ? 0 : ret ;
}


void UDP_Transmitter::close_sock()
{
    ck_close( _sock );
}


// OSC_TRANSMITTER

OSC_Transmitter::OSC_Transmitter()  { 
    _out = new UDP_Transmitter();
    _holdMessage = false;
    init();
}

OSC_Transmitter::OSC_Transmitter( UDP_Transmitter * out ) { 
    _out = out;
    _holdMessage = false;
}

OSC_Transmitter::~OSC_Transmitter() { delete _out; } 

void 
OSC_Transmitter::init() { 
   if( _out->status() == UDP_NOINIT ) _out->init();
   OSC_initBuffer(&_osc, sizeof(_buffer), _buffer );
}

void 
OSC_Transmitter::setHost( char * hadd, int p ) { 
   _out->set_host(hadd, p);
}

void
OSC_Transmitter::presend( char * buf, int sz ) { 
   _out->send(buf,sz);
}

void
OSC_Transmitter::openBundle( OSCTimeTag t) { 
   OSC_openBundle( &_osc, t);
}


void
OSC_Transmitter::closeBundle() { 
   OSC_closeBundle( &_osc );
   tryMessage();
}


void
OSC_Transmitter::addMessage( char *address, char * args, ...)
{
   if( args == NULL || args[0] == '\0' ) {  // if there's no text
       OSC_writeAddress( &_osc, address );  // nothing
       return;
   }

   va_list      tags;                       // pointer to list of arguments
   va_start(tags, args);                    // parses the string for variables
   
   OSC_writeAddressAndTypes( &_osc, address, args );

   int argnum = strlen(args);
   int osc_err = 0;
   for( int j = 1; !osc_err && j < argnum; j++ ) { 
      switch ( args[j] ) { 
      case 'i':
         osc_err = OSC_writeIntArg    ( &_osc, va_arg(tags, int) );
         break;
      case 'f':
         osc_err = OSC_writeFloatArg  ( &_osc, va_arg(tags, double) );
         break;
      case 's':
         osc_err = OSC_writeStringArg ( &_osc, va_arg(tags, char *) );
         break;  
      }
   }

   if( osc_err ) { 
       EM_log( CK_LOG_SYSTEM, "(via OSC): error writing packet: %d %s...", osc_err, OSC_errorMessage );
       // failure action???
   }

   va_end(tags);     // Results Are Stored In Text
   tryMessage();
}

void
OSC_Transmitter::startMessage( char * spec ) { 
    
    int len = strlen( spec );
    
    char * comma = strchr( spec, ',');
    char * space = strchr ( spec, ' ');
    int coff = ( comma == NULL ) ? len : comma - spec; 
    int spoff = ( space == NULL ) ? len : space - spec; 
    int off = min ( coff, spoff );
    
    if( off < len )
        spec[off] = '\0';
    
    startMessage( spec, spec + off + 1 );
}

void
OSC_Transmitter::startMessage( char * address, char * args )
{ 
    // OSC_writeAddressAndTypes( &_osc, address, args );
    char addrfix[512];
    int addrlen = strlen( address );
    // int mx = strlen(address) + strlen(args) + 1; // either we need to add a comma to args,
    // or it's got junk in it. 
    // char addrfix[] = new char[mx]; // (char*) malloc( mx * sizeof( char ) );
    strcpy ( addrfix, address );
    addrfix[addrlen] = '\0';
    addrfix[addrlen+1] = ',';
    char * argptr = addrfix + (addrlen + 2);
    char * p_args = args;
    while ( *p_args != '\0' ) { 
        if( *p_args != ',' && *p_args != ' ' ) {
            *argptr = *p_args; 
            argptr++;
        }
        p_args++;
    }   
    *argptr = '\0';

    OSC_writeAddressAndTypes( &_osc, addrfix, addrfix + addrlen + 1);  //expects the ',' before spec tag.
//  delete[] addrfix;
    //free ( (void*)addrfix );
    tryMessage();
}

void
OSC_Transmitter::addInt ( int4byte i ) { 
    int osc_err = OSC_writeIntArg ( &_osc, i );
    if( osc_err ) { 
       CK_FPRINTF_STDERR( "[chuck](via OSC): error writing packet: %d %s\n", osc_err, OSC_errorMessage );
       //failure action???
    }    
    tryMessage();
}

void
OSC_Transmitter::addFloat ( float f ) { 
    int osc_err = OSC_writeFloatArg ( &_osc, f );
    if( osc_err ) { 
       CK_FPRINTF_STDERR( "[chuck](via OSC): error writing packet: %d %s\n", osc_err, OSC_errorMessage );
        //failure action???
    }    
    tryMessage();
}

void
OSC_Transmitter::addString ( char * s ) { 
    int osc_err = OSC_writeStringArg ( &_osc, s );
    if( osc_err ) { 
       CK_FPRINTF_STDERR( "[chuck](via OSC): error writing packet: %d %s\n", osc_err, OSC_errorMessage );
        //failure action???
    }  
    tryMessage();
}

bool
OSC_Transmitter::packetReady() { 
    if( _holdMessage ) return false; //message hold is on
    if( _osc.typeStringPtr ) {       //if it's been typed, check that type is complete
        if( CheckTypeTag(&_osc, '\0' ) ) return false;
    } 
    return ( OSC_isBufferDone(&_osc) != 0 );
}

void
OSC_Transmitter::tryMessage() { 
    if( !packetReady() ) return;

    _out->send( OSC_getPacket(&_osc), OSC_packetSize(&_osc) );
    OSC_resetBuffer(&_osc);
}

void
OSC_Transmitter::holdMessage(bool b) { 
    _holdMessage  = b;
}

void
OSC_Transmitter::kickMessage() { 
    if( !OSC_isBufferDone(&_osc) ) { 
        CK_FPRINTF_STDERR( "[chuck](via OSC): error -> sending incomplete packet!\n" );
    }
    _out->send( OSC_getPacket(&_osc), OSC_packetSize(&_osc) );
    OSC_resetBuffer(&_osc);
}


// OSC_RECEIVER

OSC_Receiver::OSC_Receiver( Chuck_VM * vm ):
    // _listening(false),
    // _inbufsize(OSCINBUFSIZE),
    _port(-1),
    _tmp_port(-1),
    _io_mutex(NULL),
    _address_mutex(NULL),
    _inbox(NULL),
    _inbox_size(2),
    _started(false),
    _in_read(0),
    _in_write(1),
    _address_space(NULL),
    _address_size(2),
    _address_num(0),
    m_event_buffer(NULL)
{
    // store vm ref
    m_vmRef = vm;
    // allocate inbox
    _inbox = (OSCMesg *)malloc( sizeof(OSCMesg) * _inbox_size );
    // initialize payload
    for( int i = 0; i < _inbox_size; i++ )
        _inbox[i].payload = NULL;

    // allocate address space
    _address_space = (OSC_Address_Space **)realloc( _address_space, _address_size * sizeof(OSC_Address_Space *) );

    _io_mutex = new XMutex();
    _address_mutex = new XMutex();
    // _io_thread = new XThread();

    //_in = new UDP_Receiver();

    init();
}

OSC_Receiver::OSC_Receiver( Chuck_VM * vm, UDP_Receiver * in )
{
    // _in = in;
    _port = -1;
    // store vm ref
    m_vmRef = vm;
}

void OSC_Receiver::init()
{
    _port = -1;
    // _in->init();
}

OSC_Receiver::~OSC_Receiver()
{
    // stop
    stopListening();
    // clean up
    for( int i = 0; i < _inbox_size; i++ )
    {
        // check
        if( _inbox[i].payload )
        {
            free( _inbox[i].payload );
            _inbox[i].payload = NULL;
        }
    }

    // 1.3.1.1: moving this outside the loop, oops
    free( _inbox );
    
    // clean up
    SAFE_DELETE( _io_mutex );
    SAFE_DELETE( _address_mutex );

    // TODO: do this thread-safely
    // if( m_event_buffer )
    // {
    //     m_vmRef->destroy_event_buffer( m_event_buffer );
    //     m_event_buffer = NULL;
    // }
    
    // delete _in;
}

THREAD_RETURN (THREAD_TYPE osc_recv_thread)( void * data )
{
    OSC_Receiver * oscar = (OSC_Receiver * )data;

    // priority boost
    if( XThreadUtil::our_priority != 0x7fffffff )
        XThreadUtil::set_priority( XThreadUtil::our_priority );

    do {
        oscar->recv_mesg();
        usleep( 10 );
    }while( true );

    return (THREAD_RETURN)0;
}


int&
OSC_Receiver::port() { return _port; }

void 
OSC_Receiver::onReceive( char * mesg, int mesgLen ) { 
    handle_mesg( mesg, mesgLen );
}

void
OSC_Receiver::bind_to_port(int port ) { 
    _tmp_port = port;
//    _in->bind_to_port(port);
}

bool
OSC_Receiver::listen( int port ) { 
    bind_to_port ( port );
    return listen();
}

void
OSC_Receiver::stopListening()
{
    unsubscribe();
}

bool
OSC_Receiver::listen()
{
    if( m_event_buffer == NULL )
        m_event_buffer = m_vmRef->create_event_buffer();
    
    unsubscribe(); // in case we're connected.
    
    return subscribe( _tmp_port );

    /*
    if( !_listening ) { 
        _listening = true;
        _io_thread->start( osc_recv_thread, (void*)this );
    }
    return _listening;
    */
}



void
OSC_Receiver::close_sock() { 
    unsubscribe();
//   _in->close_sock();
}

void
OSC_Receiver::recv_mesg() { 
    // this is when OSC_Receiver was 
    // used as the port-manager.  now this code ( and callback ) are in UDP_Port_Listener interface,
    // which OSC_Receiver subclasses 

    /*
    _mesglen = _in->recv_next( _inbuf, _inbufsize );
    if( _mesglen > 0 ) { 
        //parse();
        handle_mesg(_inbuf, _mesglen);
    }
    */
}
 

bool OSC_Receiver::has_mesg()
{
    // EM_log( CK_LOG_FINER, "OSC has message?" ); 
    return ( _started && ( _in_read+1 ) % _inbox_size != _in_write ); 
}

bool OSC_Receiver::get_mesg( OSCMesg * bucket )
{
    _io_mutex->acquire();

    if( has_mesg() )
    {
        // next write _may_ fuck with _in_read 
        // where when resizing the buffer...
        *bucket = *next_read();
        // CK_FPRINTF_STDERR( "r" ); //read: %d of %d \n", _in_read, _inbox_size);
        _io_mutex->release();
        return true;
    }

    _io_mutex->release();

    return false;
}

void OSC_Receiver::parse( char * mesg, int mesgLen )
{
   // unpack bundles, dump all messages
   // any local prep we need here?

   long i = 0;

    // log
   EM_log( CK_LOG_FINER, "OSC_Receiver: print message length %d", mesgLen );
   
    for( i = 0 ; i < mesgLen; i++ )
    {
      CK_FPRINTF_STDERR( "\t%c(%0x):", mesg[i], mesg[i] );
    }

    CK_FPRINTF_STDERR( "\n" );
}

void
OSC_Receiver::next_write()
{ 
   // called by the UDP client ( blocks ) 

   if( !_started ) { 
      _started = true; 
   } 

   int next = (_in_write+1) % _inbox_size;
   
   if( next == _in_read ) { 
      
      // CK_FPRINTF_STDERR( "OSC::need to resize..." );
         
      _io_mutex->acquire(); //don't let next read return a bogus pointer

      if( _inbox_size < OSCINBOXMAX) { 
         
         // LOCK MUTEX (on next_read)
         // prevents 

         // mesgs might move around, but their data doesn't..(this is good)..
         // if we return an OSCmesg* to an out-of-thread function, it can expire.
         // so we pass in pointer to our own mesg from outside (get_mesg) and copy
         // from the current 'read' in the buffer...buffer may then be resized...
         // since data buffers do not move, we remain okay.

         // perhaps a monolithic chunk of memory is a better option...
         // but what if we want to handle multiple messages at once?

         // orrr..if we follow timetags and need semirandom access to 
         // mesg contents for scheduling bundles..hmm.
         
         // make a separate case for scheduled messages...
         // throw them into a heap...

         // fuck...back to the todo...
      
         int new_size = ( _inbox_size * 2 > OSCINBOXMAX ) ? OSCINBOXMAX : _inbox_size * 2 ;

         bool err = false;

         // okay.  read and write may be anywhere.  
         if( _in_write > _in_read ) {
            // easy case
            _inbox = (OSCMesg* ) realloc ((void*)_inbox, new_size * sizeof(*_inbox) );
            err = (!_inbox);
            // indices are still valid
         }
         else { 
            // write is behind read in the buffer
            // copy into buffer so that contents remain contiguous 
            // note:: copy the buffers between write and read...
            //        they may have valid packet pointers.......

            OSCMesg * new_inbox =(OSCMesg*)malloc ( new_size * sizeof(*new_inbox) );
            err = (!new_inbox);
            
            if( !err ) { 
               int _read_tail = _inbox_size - _in_read;
               memcpy( (void*)new_inbox,               (void*)(_inbox+_in_read),  (_read_tail) * sizeof(*_inbox) );
               memcpy( (void*)(new_inbox+_read_tail),  (void*)_inbox,             (_in_read  ) * sizeof(*_inbox) );
               
               // update indices
               _in_read = 0;
               _in_write = (_in_write + _read_tail);
               
               free ( _inbox );
               _inbox = new_inbox;
            }

            else err = true;
         }

         
         if( !err ) { 
            // new inbox elements need their payload pointers init to NULL
            for( int i = _inbox_size; i < new_size; i++ ) _inbox[i].payload = NULL;           
         }
         else { 
            // CK_FPRINTF_STDERR( "oh no!! buffer reallocation error!\n" );
            exit(0);
         }

         next = (_in_write+1)%new_size; //where we're headed..
         _inbox_size = new_size;

         // CK_FPRINTF_STDERR( "(%d)", _inbox_size );
         // UNLOCK MUTEX
      }
      else { 
         // CK_FPRINTF_STDERR( "max buffer...dropping old mesg %d\n", next );
         _in_read = (next+1)%_inbox_size;         
      }

      _io_mutex->release();
   }

   _in_write = next;
}


OSCMesg*
OSC_Receiver::next_read() { 
   int next = ( _in_read + 1 ) % _inbox_size;  
   if( next == _in_write ) return NULL; // can't intrude on write
 
   _in_read = next; //advance
   OSCMesg* ret= _inbox + _in_read; // return this pointer
   return ret;
}

void
OSC_Receiver::set_mesg(OSCMesg* mrp, char * buf, int len ) { 

   // put pointers in the appropriate places
   // this means that mrp has no idea whether
   // its pointers refer to its own payload
   // or to an external buffer

   mrp->len = len; //total size of message
   mrp->address = buf;
   mrp->types   = mrp->address + strlen(mrp->address) + 4 - strlen(mrp->address)%4;
   if( mrp->types[0] == ',') { 
      // typed message
      mrp->data= mrp->types + strlen(mrp->types) + 4 - strlen(mrp->types)%4;  
   }
   else { 
      // untyped message.  
      // should there be a warning?
      // classes that handle mesgs
      // should take care of that.
      mrp->data  = mrp->types; 
      mrp->types = NULL; 
   }

}

void
OSC_Receiver::handle_mesg( char * buf, int len )
{
   // this is called sequentially by the receiving thread. 
   if( buf[0] == '#' ) { handle_bundle( buf, len ); return; }
  
   OSCMesg * mrp = write();

   if( mrp->payload == NULL ) {
      mrp->payload = (char *)malloc( OSCINBUFSIZE * sizeof(char) );
      // CK_FPRINTF_STDERR( "oscrecv:mallocing %d bytes\n", OSCINBUFSIZE );
   }

   memcpy( (void*)mrp->payload, (const void*)buf, len ); // copy data from buffer to message payload
   set_mesg( mrp, mrp->payload, len ); // set pointers for the message to its own payload
   mrp->recvtime = 0.000; // GetTime(); // set message time

   distribute_message( mrp );  // copy message to any & all matching address spaces

   // the distribute message will copy any necessary data into the addr object's private queue.
   // so we don't need to buffer here.. ( totally wasting the kick-ass expanding buffer i have in this class...drat )  

   // next_write();
}

void OSC_Receiver::handle_bundle( char * b, int len )
{
   // no scheduling for now
   // just immediately unpack everything

   // CK_FPRINTF_STDERR( "bundle (length %d)\n", len );
   
   int off = 16; //skip "#bundle\0timetags"

   while ( off < len ) 
   { 
      char * z = b+off;
      unsigned long size = ntohl(*((unsigned long*)z));

      char * m = z+4;

      if( m[0] == '#' ) handle_bundle(m,size);
      else handle_mesg(m,size);

      off += size+4; //beginning of next message
   }
}

OSC_Address_Space * OSC_Receiver::new_event( char * spec)
{
    OSC_Address_Space * event = new OSC_Address_Space( spec );
    add_address( event );
    return event;
}

OSC_Address_Space * OSC_Receiver::new_event( char * addr, char * type)
{
    OSC_Address_Space * event = new OSC_Address_Space( addr, type );
    add_address( event );
    return event;
}


void OSC_Receiver::add_address( OSC_Address_Space * src )
{
    // lock (added 1.3.1.1)
    _address_mutex->acquire();

    // grow array, in place if possible
    if( _address_num == _address_size )
    {
        _address_size *= 2;
        _address_space = (OSC_Address_Space **)realloc( _address_space, _address_size * sizeof( OSC_Address_Space **) );
    }
    // add the source
    _address_space[_address_num++] = src;

    // set the receiver
    src->setReceiver( this );

    // release (added 1.3.1.1)
    _address_mutex->release();
}

void OSC_Receiver::remove_address( OSC_Address_Space *odd )
{
    // lock (added 1.3.1.1)
    _address_mutex->acquire();

    for( int i = 0 ; i < _address_num ; i++ )
    {
        while( _address_space[i] == odd )
        {
            _address_space[i] = _address_space[--_address_num];
        }
    }
    
    // release (added 1.3.1.1)
    _address_mutex->release();
}

/*
 this function distributes each message to a matching address inside
 the chuck shred.  There are some issues with this. bundle simultaneity
 might be solved via mutex?
*/
void OSC_Receiver::distribute_message( OSCMesg * msg )
{
    // lock (added 1.3.1.1)
    _address_mutex->acquire();

    // iterate
    for( int i = 0 ; i < _address_num ; i++ )
    {
        if( _address_space[i]->try_queue_mesg( msg ) )
        {
            // CK_FPRINTF_STDERR( "broadcasting %x from %x\n", (uint)_address_space[i]->SELF, (uint)_address_space[i] );
            // if the event has any shreds queued, fire them off..
            ((Chuck_Event *)_address_space[i]->SELF)->queue_broadcast( m_event_buffer );
        }
    }
    
    // release (added 1.3.1.1)
    _address_mutex->release();
}


// OSC SOURCE ( Parser ) 
// rename to OSC_address ? 

// this is an OSC_Address_Space - we register it to a particular receiver
// the receiver matches an incoming message against registered 
// address and sends the message to all that match. 

// OSCMesg may still have pointers to a mysterious void.
// we should copy values locally for string and blob.

// public for of inherited chuck_event mfunc.  

const char * osc_typename_strings[] = { 
	"untyped",
	"no arguments",
	"integer",
	"float",
	"string",
	"blob",
	"ERROR: num types exceeded"
};



OSC_Address_Space::OSC_Address_Space() { 
    init();
    setSpec( "/undefined/default,i" );
}


OSC_Address_Space::OSC_Address_Space( const char * spec ) { 
    init();
    setSpec( spec );
}

OSC_Address_Space::OSC_Address_Space( const char * addr, const char * types) { 
    init();
    setSpec( addr, types );
}

void
OSC_Address_Space::init() { 
    _receiver = NULL;
    SELF = NULL;
    _queueSize = 512; // start queue size at 512 // 64. 
    _dataSize = 0;
    _cur_mesg = NULL;
    _queue = NULL;
	_cur_value = 0;
    _current_data = NULL;
    _buffer_mutex = new XMutex();
}

OSC_Address_Space::~OSC_Address_Space()
{
    // clean up
    if( _queue ) free( _queue );
    // added 1.3.1.1
    SAFE_DELETE( _buffer_mutex );
}

void
OSC_Address_Space::setReceiver(OSC_Receiver * recv) { 
    _receiver = recv;
}

void
OSC_Address_Space::setSpec( const char *addr, const char * types ) { 
    if( snprintf( _spec, sizeof _spec, "%s,%s", addr, types ) >= sizeof _spec) {
        // TODO: handle the overflow more gracefully.
        EM_log(CK_LOG_SEVERE, "OSC_Address_Space::setSpec: Not enough space in _spec buffer, data was truncated.");
    }
    scanSpec();
    _needparse = true; 
    parseSpec(); 
}

void   
OSC_Address_Space::setSpec( const char *c ) { 
    strncpy ( _spec, c, 512); 
    scanSpec();
    _needparse = true; 
    parseSpec(); 
}
 
void
OSC_Address_Space::scanSpec() { //this allows for sloppy-ish definitions in type string
    //assumes that a potential spec is already in _spec;
    char * pread;
    char * pwrite;
    pread = pwrite = (char*)_spec;
    bool in_type = false;
    while ( *pread != '\0' ) { 
        if( *pread == ',' || *pread == ' ' ) { 
            if( !in_type ) {
                in_type = true;
                *pwrite = ',';
                pwrite++;
            } // otherwise, we ignore
			else if( *pread == ' ' ) { 
				EM_log(CK_LOG_INFO, "OSC-Address-Space: spaces in OSC type tag string! ignoring");
			}
        }
        else { 
            if( pwrite != pread ) { *pwrite = *pread; }
            pwrite++;
        }
        pread++;
    }
    if( !in_type ) {
        //no type args found, add a comma to indicate a zero-arg string. 
        *(pwrite++) = ',';
    }
    *pwrite = '\0'; //if pread terminates, so does pwrite. 
}

void
OSC_Address_Space::parseSpec() { 

   if( !_needparse ) return;

   char * type = strrchr(_spec, ',');
   if( !type ) return;

   *type = '\0'; //null the address
   type++; //point to type string

   strcpy ( _address, _spec );
   strcpy ( _type , type );

//   CK_FPRINTF_STDERR( " parsing spec- address %s :: type %s\n", _address, _type );

   int n = strlen ( type );
   _noArgs = ( n == 0 );
   resizeData( ( n < 1 ) ? 1 : n );
   _qread = 0;
   _qwrite = 1;

   _needparse = false;
}

void
OSC_Address_Space::resizeQueue( int n )
{
    if( n <= _queueSize )
        return; // only grows
    
    // lock
    _buffer_mutex->acquire();

    // it's possible that between the test above and now, 
    // qread has moved on and we actually DO have space..

    EM_log( CK_LOG_INFO, "OSC_Address (%s) -- buffer full ( r:%d , w:%d, of %d(x%d) ), resizing...",
            _address, _qread, _qwrite, _queueSize, _dataSize );
    // CK_FPRINTF_STDERR( "--- hasMesg ( %d ) nextMesg ( %d )\n", (int) has_mesg(), (int)next_mesg()  );

    // we're the server... all we know is that the contents of qread are in the current_buffer, 
    // so we can move the data, but we don't want to move ahead and lose it just yet.

    // compute chunk size
    size_t chunkSize = _dataSize * sizeof( opsc_data );

    // allocate
    int _newQSize = n;
    opsc_data * _new_queue = (opsc_data *)malloc( _newQSize * chunkSize );

    // clear
    memset( _new_queue, 0, _newQSize * chunkSize ); // out with the old...
    // copy
    if( _qread < _qwrite )
    {
        // we're doing this because qwrite is out of room.  
        // so if qwrite is already greater than qread,
        // just copy the whole thing to the start of the new buffer ( adding more space to the end ) 
        memcpy( (void*)_new_queue, (const void*)_queue, _queueSize * chunkSize);
        // _qwrite and _qread can stay right where they are. 
        // CK_FPRINTF_STDERR( "resize - case 1\n" );
    }
    else
    { 
        // _qread is in front of _qwrite, so we must unwrap, and place _qread at the
        // front of the queue, and qwrite at the end, preserving all elements that qread
        // is waiting to consume.
        int nread = _queueSize - _qread;
        memcpy( (void*)_new_queue, 
                (const void*)(_queue + _qread * _dataSize), 
                (nread) * chunkSize);
        memcpy( (void*)(_new_queue + nread * _dataSize),
                (const void*)_queue,
                (_qread) * _dataSize * sizeof( opsc_data ));
        _qread = 0;
        _qwrite += nread;
        // EM_log(CK_LOG_INFO, "resize - case 2\n");
    }

    _queueSize = _newQSize; 
    opsc_data * trash = _queue; 
    _queue = _new_queue;
    free ( (void*)trash );

    // don't move qread or qwrite until we're done. 
    _buffer_mutex->release();
}


void
OSC_Address_Space::resizeData( int n ) { 
    if( _dataSize == n ) return;
    _dataSize = n;
    int queueLen = _queueSize * _dataSize * sizeof( opsc_data );
    _queue = ( opsc_data * ) realloc ( _queue, queueLen );
    memset ( _queue, 0, queueLen );
    _current_data = (opsc_data* ) realloc ( _current_data, _dataSize * sizeof( opsc_data) );
}


bool
OSC_Address_Space::message_matches( OSCMesg * m ) {

    //this should test for type as well.
   
    bool addr_match;
    if( strcmp ( m->address, _address ) == 0 ) addr_match = true;
    else addr_match = PatternMatch( m->address, _address);
    if( !addr_match ) return false;

    //address AND type must match 
    //but there should be an option for the blank 'information about this' pattern

    char * type = m->types+1;
    if( m->types == NULL || strcmp( _type , type ) != 0 ) { 
//        if( type )
//            CK_FPRINTF_STDERR( "error, mismatched type string( %s ) vs ( %s ) \n", _type, type );
//        else 
//            CK_FPRINTF_STDERR( "error, missing type string (expecting %s) \n", _type );
        return false;
    }
    return true;
}


bool OSC_Address_Space::try_queue_mesg( OSCMesg * m ) 
{
    if( !message_matches( m ) ) return false;
    queue_mesg( m );
    return true;
}


bool OSC_Address_Space::has_mesg()
{
    // EM_log( CK_LOG_FINER, "OSC has mesg" );
    return ( ( _qread + 1 ) % _queueSize != _qwrite );
}


bool OSC_Address_Space::next_mesg()
{
    // 1.3.1.1 update
    if( !has_mesg() )
        return false;

    // lock
    _buffer_mutex->acquire();

    // TODO: ge uhhhh should release mutex?
    if( has_mesg() )
    {
        // move qread forward
        _qread = ( _qread + 1 ) % _queueSize;
        memcpy( _current_data, _queue + _qread * _dataSize, _dataSize * sizeof( opsc_data ) );
        _cur_mesg = _current_data;
        _cur_value = 0;

        // release
        _buffer_mutex->release();
        return true;
    }

    // release
    _buffer_mutex->release();
    return false;

//    if( has_mesg() )
//    {
//        _buffer_mutex->acquire();
//        
//        // TODO: ge uhhhh should release mutex?
//        if( !has_mesg() ) return false;
//        
//        // move qread forward
//        _qread = ( _qread + 1 ) % _queueSize;
//        memcpy( _current_data, _queue + _qread * _dataSize, _dataSize * sizeof( opsc_data ) );
//        _cur_mesg = _current_data;
//        _cur_value = 0;
//        
//        _buffer_mutex->release();
//        
//        return true;
//    }
//    
//    return false;
}

bool OSC_Address_Space::vcheck( osc_datatype tt )
{
	if( !_cur_mesg )
    {
        EM_log( CK_LOG_SEVERE, "OscEvent: getVal(): nextMsg() must be called before reading data..." );
        return false;
	}
    else if( _cur_value < 0 || _cur_value >= _dataSize )
    {
        EM_log( CK_LOG_SEVERE, "OscEvent: read position %d outside message ...", _cur_value );
        return false;
    }
    else if( _cur_mesg[_cur_value].t != tt )
    {
        EM_log( CK_LOG_SEVERE, "OscEvent: error -> message type %s != request type %s", osc_typename_strings[_cur_mesg[_cur_value].t], osc_typename_strings[tt]  );
        return false;
    }

    return true;
}

int4byte OSC_Address_Space::next_int()
{
    return (vcheck(OSC_INT)) ? _cur_mesg[_cur_value++].i : 0;
}

float OSC_Address_Space::next_float()
{
    return (vcheck(OSC_FLOAT)) ? _cur_mesg[_cur_value++].f : 0.0;
}

char * OSC_Address_Space::next_string()
{
    return (vcheck(OSC_STRING)) ? _cur_mesg[_cur_value++].s : NULL ;
}

char * 
OSC_Address_Space::next_string_dup()
{
    if( !vcheck(OSC_STRING) ) return NULL;
    char * alt_c = _cur_mesg[_cur_value++].s;
    char * dup_c = (char * ) malloc ( (strlen ( alt_c )+1 ) * sizeof(char) );
    strcpy ( dup_c, alt_c );
    return dup_c;
}

char *
OSC_Address_Space::next_blob() { 
    return ( vcheck(OSC_BLOB) )   ?  _cur_mesg[_cur_value++].s : NULL ;
}

void
OSC_Address_Space::queue_mesg( OSCMesg * m ) 
{
    // in the server thread. 
    int nqw = ( _qwrite + 1 ) % _queueSize;
    EM_log( CK_LOG_FINE, "OSC address(%s): read: %d write: %d size: %d", _address, _qread, _qwrite, _queueSize );

    if( nqw == _qread ) {
        if( _queueSize < OSC_ADDRESS_QUEUE_MAX ) { 
            resizeQueue( _queueSize * 2 );
            nqw = ( _qwrite + 1 ) % _queueSize; // _qwrite, _qread may have changed.
        }
        else { 
            EM_log( CK_LOG_INFO, "OSC_Address_Space(%s): message queue reached max size %d...", _address, _queueSize );
            EM_log( CK_LOG_INFO, "...(dropping oldest message %d)", _qread );

            // bump!
            _buffer_mutex->acquire();
            _qread = ( _qread + 1 ) % _queueSize; 
            _buffer_mutex->release();
        }
    }

    _vals = _queue + _qwrite * _dataSize;

    if( _noArgs ) { // if address takes no arguments, 
        _vals[0].t = OSC_NOARGS;
    }
    else { 
        char * type = m->types+1;
        char * data = m->data;
        
        unsigned int endy;
        int i=0;
        
        float *fp;
        int   *ip;
        int   clen;
        while ( *type != '\0' ) { 
            switch ( *type ) { 
            case 'f':
                endy = ntohl(*((unsigned long*)data));
                fp = (float*)(&endy);
                _vals[i].t = OSC_FLOAT;
                _vals[i].f = *fp; 
                data += 4;
                break;
            case 'i':
                endy = ntohl(*((unsigned long*)data));
                ip = (int4byte*)(&endy);
                _vals[i].t = OSC_INT;
                _vals[i].i = *ip; 
                data += 4;
                break;
            case 's':
                // string
                clen = strlen(data) + 1; // terminating!
                _vals[i].t = OSC_STRING;
                _vals[i].s = (char *) realloc ( _vals[i].s, clen * sizeof(char) );
                memcpy ( _vals[i].s, data, clen ); // make a copy of the data...
                // CK_FPRINTF_STDERR( "add string |%s| ( %d ) \n", _vals[i].s, clen );
                data += (((clen-1) >> 2) + 1) << 2;
                // data += clen + 4 - clen % 4;
            break;
            case 'b':
                // blobs
                endy = ntohl(*((unsigned long*)data));
                clen = *((int*)(&endy));
                _vals[i].t = OSC_BLOB;
                _vals[i].s = (char* ) realloc ( _vals[i].s, clen * sizeof(char) );
                memcpy ( _vals[i].s, data, clen );
                data += clen + 4 - clen % 4;
                break;
            }
            i++;
            type++;
        }
    }
    
    _qwrite = nqw;

    //review
    /*
      for( i=0; i < _nvals; i++ ) {
      CK_FPRINTF_STDERR( "%d %f : ", i, _val[i] );
      }
      CK_FPRINTF_STDERR( "\n" );
    */
}
