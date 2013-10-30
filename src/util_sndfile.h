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

/*
** libsndfile Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//-----------------------------------------------------------------------------
// name: util_sndfile.h
// desc: libsndfile for ChucK
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ari Lazier (alazier@alumni.princeton.edu)
// libsndfile: Erik de Castro Lopo (erikd@mega-nerd.com)
//-----------------------------------------------------------------------------
#ifndef __UTIL_SNDFILE_H__
#define __UTIL_SNDFILE_H__

#include <stdio.h>
#include <stdlib.h>


// XXX philipd this might break things?
// these defines were placed after the machine-dependent 
// checks below...i'm not sure why

#define COMPILER_IS_GCC 1
#define CPU_CLIPS_NEGATIVE 1
#define ENABLE_EXPERIMENTAL_CODE 0
#define GCC_MAJOR_VERSION 3
#define HAVE_CALLOC 1
#define HAVE_CEIL 1
#define HAVE_DECL_S_IRGRP 1
#define HAVE_DLFCN_H 1
#define HAVE_FLEXIBLE_ARRAY 1
#define HAVE_FLOOR 1
#define HAVE_FMOD 1
#define HAVE_FREE 1
#define HAVE_FSTAT 1
#define HAVE_FSYNC 1
#define HAVE_FTRUNCATE 1
#define HAVE_GETPAGESIZE 1
#define HAVE_GMTIME 1
#define HAVE_GMTIME_R 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIBM 1
#define HAVE_LSEEK 1
#define HAVE_MALLOC 1
#define HAVE_MEMORY_H 1
#define HAVE_MMAP 1
#define HAVE_OPEN 1
#define HAVE_READ 1
#define HAVE_REALLOC 1
#define HAVE_SNPRINTF 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_WAIT_H 1
#define HAVE_VSNPRINTF 1
#define HAVE_WRITE 1
#define PACKAGE "libsndfile"
#define PACKAGE_BUGREPORT "erikd@mega-nerd.com"
#define PACKAGE_NAME "libsndfile"
#define PACKAGE_STRING "libsndfile 1.0.10"
#define PACKAGE_TARNAME "libsndfile"
#define PACKAGE_VERSION "1.0.10"

#define SIZEOF_DOUBLE 8
#define SIZEOF_FLOAT 4
#define SIZEOF_INT 4
#define SIZEOF_LONG 4
#define SIZEOF_OFF_T 8
#define SIZEOF_SF_COUNT_T 8
#define SIZEOF_SHORT 2
#define SIZEOF_SIZE_T 4
#define SIZEOF_SSIZE_T 4
#define SIZEOF_VOIDP 4
#define STDC_HEADERS 1

#define VERSION "1.0.10"

#ifdef __MACOSX_CORE__
#define CPU_CLIPS_POSITIVE 1
#ifdef __LITTLE_ENDIAN__
#define CPU_IS_BIG_ENDIAN 0
#define CPU_IS_LITTLE_ENDIAN 1
#define HAVE_LRINTF 1
#define HAVE_LRINT 1
#else
#define CPU_IS_BIG_ENDIAN 1
#define CPU_IS_LITTLE_ENDIAN 0
#endif
#define HAVE_PREAD 1
#define HAVE_PWRITE 1
#define OS_IS_MACOSX 1
#define OS_IS_WIN32 0
#define TYPEOF_SF_COUNT_T off_t
#endif

#if     defined(__PLATFORM_WIN32__) //Dev Studio
#define CPU_CLIPS_POSITIVE 0
#define CPU_IS_BIG_ENDIAN 0
#define CPU_IS_LITTLE_ENDIAN 1
#define HAVE_LRINTF 0
#define HAVE_LRINT 0
#undef  HAVE_GMTIME_R
#undef  HAVE_GMTIME
#undef  HAVE_SNPRINTF
#undef  HAVE_FLEXIBLE_ARRAY
#define OS_IS_MACOSX 0
#define OS_IS_WIN32 1
#define TYPEOF_SF_COUNT_T off_t
#endif

#if     defined (__WINDOWS_PTHREAD__) //Cygwin
#define CPU_CLIPS_POSITIVE 0
#define CPU_IS_BIG_ENDIAN 0
#define CPU_IS_LITTLE_ENDIAN 1
#define HAVE_LRINTF 0
#define HAVE_LRINT 0
#define OS_IS_MACOSX 0
#define OS_IS_WIN32 0
#define TYPEOF_SF_COUNT_T off_t
#endif

#ifdef __LINUX_ALSA__
#define HAVE_ALSA_ASOUNDLIB_H 
#endif

#if defined(__PLATFORM_LINUX__)
#define CPU_CLIPS_POSITIVE 0
#define CPU_IS_BIG_ENDIAN 0
#define CPU_IS_LITTLE_ENDIAN 1
#define HAVE_PREAD 1
#define HAVE_PWRITE 1
#define OS_IS_MACOSX 0
#define OS_IS_WIN32 0
#define SIZEOF_OFF64_T 0
#define SIZEOF_LOFF_T 8
#define TYPEOF_SF_COUNT_T loff_t
#define _FILE_OFFSET_BITS 64
#define HAVE_FDATASYNC 1
#define HAVE_ENDIAN_H 1
#define HAVE_LRINTF 1
#define HAVE_LRINT 1
#endif



// XXX 'inline' is necessary for C compilation
// in the microsoft vc6 compiler...
// and other ms win32 specialteez. 

#ifdef __PLATFORM_WIN32__

#define C_INLINE __inline 
#define SF_COUNT_MAX 0x7FFFFFFFFFFFFFFF

#else

#define C_INLINE inline
#define SF_COUNT_MAX 0x7FFFFFFFFFFFFFFFLL
#define HAVE_UNISTD_H 1
#define HAVE_SSIZE_T 1

#endif

/*
** sndfile.h -- system-wide definitions
**
** API documentation is in the doc/ directory of the source code tarball
** and at http://www.mega-nerd.com/libsndfile/api.html.
*/

#ifndef SNDFILE_H
#define SNDFILE_H

/* This is the version 1.0.X header file. */
#define SNDFILE_1


/* For the Metrowerks CodeWarrior Pro Compiler (mainly MacOS) */

#if (defined (__MWERKS__))
#else
#include    <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* The following file types can be read and written.
** A file type would consist of a major type (ie SF_FORMAT_WAV) bitwise
** ORed with a minor type (ie SF_FORMAT_PCM). SF_FORMAT_TYPEMASK and
** SF_FORMAT_SUBMASK can be used to separate the major and minor file
** types.
*/

enum
{   /* Major formats. */
    SF_FORMAT_WAV           = 0x010000,     /* Microsoft WAV format (little endian). */
    SF_FORMAT_AIFF          = 0x020000,     /* Apple/SGI AIFF format (big endian). */
    SF_FORMAT_AU            = 0x030000,     /* Sun/NeXT AU format (big endian). */
    SF_FORMAT_RAW           = 0x040000,     /* RAW PCM data. */
    SF_FORMAT_PAF           = 0x050000,     /* Ensoniq PARIS file format. */
    SF_FORMAT_SVX           = 0x060000,     /* Amiga IFF / SVX8 / SV16 format. */
    SF_FORMAT_NIST          = 0x070000,     /* Sphere NIST format. */
    SF_FORMAT_VOC           = 0x080000,     /* VOC files. */
    SF_FORMAT_IRCAM         = 0x0A0000,     /* Berkeley/IRCAM/CARL */
    SF_FORMAT_W64           = 0x0B0000,     /* Sonic Foundry's 64 bit RIFF/WAV */
    SF_FORMAT_MAT4          = 0x0C0000,     /* Matlab (tm) V4.2 / GNU Octave 2.0 */
    SF_FORMAT_MAT5          = 0x0D0000,     /* Matlab (tm) V5.0 / GNU Octave 2.1 */
    SF_FORMAT_PVF           = 0x0E0000,     /* Portable Voice Format */
    SF_FORMAT_XI            = 0x0F0000,     /* Fasttracker 2 Extended Instrument */
    SF_FORMAT_HTK           = 0x100000,     /* HMM Tool Kit format */
    SF_FORMAT_SDS           = 0x110000,     /* Midi Sample Dump Standard */
    SF_FORMAT_AVR           = 0x120000,     /* Audio Visual Research */
    SF_FORMAT_WAVEX         = 0x130000,     /* MS WAVE with WAVEFORMATEX */

    /* Subtypes from here on. */

    SF_FORMAT_PCM_S8        = 0x0001,       /* Signed 8 bit data */
    SF_FORMAT_PCM_16        = 0x0002,       /* Signed 16 bit data */
    SF_FORMAT_PCM_24        = 0x0003,       /* Signed 24 bit data */
    SF_FORMAT_PCM_32        = 0x0004,       /* Signed 32 bit data */
    SF_FORMAT_PCM_U8        = 0x0005,       /* Unsigned 8 bit data (WAV and RAW only) */
    SF_FORMAT_FLOAT         = 0x0006,       /* 32 bit float data */
    SF_FORMAT_DOUBLE        = 0x0007,       /* 64 bit float data */
    SF_FORMAT_ULAW          = 0x0010,       /* U-Law encoded. */
    SF_FORMAT_ALAW          = 0x0011,       /* A-Law encoded. */
    SF_FORMAT_IMA_ADPCM     = 0x0012,       /* IMA ADPCM. */
    SF_FORMAT_MS_ADPCM      = 0x0013,       /* Microsoft ADPCM. */
    SF_FORMAT_GSM610        = 0x0020,       /* GSM 6.10 encoding. */
    SF_FORMAT_VOX_ADPCM     = 0x0021,       /* OKI / Dialogix ADPCM */
    SF_FORMAT_G721_32       = 0x0030,       /* 32kbs G721 ADPCM encoding. */
    SF_FORMAT_G723_24       = 0x0031,       /* 24kbs G723 ADPCM encoding. */
    SF_FORMAT_G723_40       = 0x0032,       /* 40kbs G723 ADPCM encoding. */
    SF_FORMAT_DWVW_12       = 0x0040,       /* 12 bit Delta Width Variable Word encoding. */
    SF_FORMAT_DWVW_16       = 0x0041,       /* 16 bit Delta Width Variable Word encoding. */
    SF_FORMAT_DWVW_24       = 0x0042,       /* 24 bit Delta Width Variable Word encoding. */
    SF_FORMAT_DWVW_N        = 0x0043,       /* N bit Delta Width Variable Word encoding. */
    SF_FORMAT_DPCM_8        = 0x0050,       /* 8 bit differential PCM (XI only) */
    SF_FORMAT_DPCM_16       = 0x0051,       /* 16 bit differential PCM (XI only) */

    /* Endian-ness options. */

    SF_ENDIAN_FILE          = 0x00000000,   /* Default file endian-ness. */
    SF_ENDIAN_LITTLE        = 0x10000000,   /* Force little endian-ness. */
    SF_ENDIAN_BIG           = 0x20000000,   /* Force big endian-ness. */
    SF_ENDIAN_CPU           = 0x30000000,   /* Force CPU endian-ness. */

    SF_FORMAT_SUBMASK       = 0x0000FFFF,
    SF_FORMAT_TYPEMASK      = 0x0FFF0000,
    SF_FORMAT_ENDMASK       = 0x30000000
} ;



/*
** The following are the valid command numbers for the sf_command()
** interface.  The use of these commands is documented in the file
** command.html in the doc directory of the source code distribution.
*/



enum
{   SFC_GET_LIB_VERSION             = 0x1000,
    SFC_GET_LOG_INFO                = 0x1001,
    SFC_GET_NORM_DOUBLE             = 0x1010,
    SFC_GET_NORM_FLOAT              = 0x1011,
    SFC_SET_NORM_DOUBLE             = 0x1012,
    SFC_SET_NORM_FLOAT              = 0x1013,
    SFC_GET_SIMPLE_FORMAT_COUNT     = 0x1020,
    SFC_GET_SIMPLE_FORMAT           = 0x1021,
    SFC_GET_FORMAT_INFO             = 0x1028,
    SFC_GET_FORMAT_MAJOR_COUNT      = 0x1030,
    SFC_GET_FORMAT_MAJOR            = 0x1031,
    SFC_GET_FORMAT_SUBTYPE_COUNT    = 0x1032,
    SFC_GET_FORMAT_SUBTYPE          = 0x1033,
    SFC_CALC_SIGNAL_MAX             = 0x1040,
    SFC_CALC_NORM_SIGNAL_MAX        = 0x1041,
    SFC_CALC_MAX_ALL_CHANNELS       = 0x1042,
    SFC_CALC_NORM_MAX_ALL_CHANNELS  = 0x1043,
    SFC_SET_ADD_PEAK_CHUNK          = 0x1050,
    SFC_UPDATE_HEADER_NOW           = 0x1060,
    SFC_SET_UPDATE_HEADER_AUTO      = 0x1061,
    SFC_FILE_TRUNCATE               = 0x1080,
    SFC_SET_RAW_START_OFFSET        = 0x1090,
    SFC_SET_DITHER_ON_WRITE         = 0x10A0,
    SFC_SET_DITHER_ON_READ          = 0x10A1,
    SFC_GET_DITHER_INFO_COUNT       = 0x10A2,
    SFC_GET_DITHER_INFO             = 0x10A3,
    SFC_GET_EMBED_FILE_INFO         = 0x10B0,
    SFC_SET_CLIPPING                = 0x10C0,
    SFC_GET_CLIPPING                = 0x10C1,
    SFC_GET_INSTRUMENT              = 0x10D0,
    SFC_SET_INSTRUMENT              = 0x10D1,

    /* Following commands for testing only. */
    SFC_TEST_IEEE_FLOAT_REPLACE     = 0x6001,

    /*
    ** SFC_SET_ADD_* values are deprecated and will disappear at some
    ** time in the future. They are guaranteed to be here up to and
    ** including version 1.0.8 to avoid breakage of existng software.
    ** They currently do nothing and will continue to do nothing.
    */

    SFC_SET_ADD_DITHER_ON_WRITE     = 0x1070,
    SFC_SET_ADD_DITHER_ON_READ      = 0x1071
} ;





/*
** String types that can be set and read from files. Not all file types
** support this and even the file types which support one, may not support
** all string types.
*/


enum
{   SF_STR_TITLE                    = 0x01,
    SF_STR_COPYRIGHT                = 0x02,
    SF_STR_SOFTWARE                 = 0x03,
    SF_STR_ARTIST                   = 0x04,
    SF_STR_COMMENT                  = 0x05,
    SF_STR_DATE                 = 0x06
} ;



enum
{   /* True and false */
    SF_FALSE    = 0,
    SF_TRUE     = 1,

    /* Modes for opening files. */
    SFM_READ    = 0x10,
    SFM_WRITE   = 0x20,
    SFM_RDWR    = 0x30
} ;



/* Pubic error values. These are guaranteed to remain unchanged for the duration
** of the library major version number.
** There are also a large number of private error numbers which are internal to
** the library which can change at any time.
*/



enum
{   SF_ERR_NO_ERROR             = 0,
    SF_ERR_UNRECOGNISED_FORMAT  = 1,
    SF_ERR_SYSTEM               = 2
} ;



/* A SNDFILE* pointer can be passed around much like stdio.h's FILE* pointer. */

typedef struct SNDFILE_tag  SNDFILE ;


/* The following typedef is system specific and is defined when libsndfile is.
** compiled. sf_count_t can be one of loff_t (Linux), off_t (*BSD), 
** off64_t (Solaris), __int64_t (Win32) etc.
*/

typedef TYPEOF_SF_COUNT_T sf_count_t ;

//#define SF_COUNT_MAX      0x7FFFFFFFFFFFFFFFLL

/* A pointer to a SF_INFO structure is passed to sf_open_read () and filled in.
** On write, the SF_INFO structure is filled in by the user and passed into
** sf_open_write ().
*/

struct SF_INFO
{   sf_count_t  frames ;        /* Used to be called samples.  Changed to avoid confusion. */
    int         samplerate ;
    int         channels ;
    int         format ;
    int         sections ;
    int         seekable ;
} ;

typedef struct SF_INFO SF_INFO ;

/* The SF_FORMAT_INFO struct is used to retrieve information about the sound
** file formats libsndfile supports using the sf_command () interface.
**
** Using this interface will allow applications to support new file formats
** and encoding types when libsndfile is upgraded, without requiring
** re-compilation of the application.
**
** Please consult the libsndfile documentation (particularly the information
** on the sf_command () interface) for examples of its use.
*/

typedef struct
{   int         format ;
    const char  *name ;
    const char  *extension ;
} SF_FORMAT_INFO ;

/*
** Enums and typedefs for adding dither on read and write.
** See the html documentation for sf_command(), SFC_SET_DITHER_ON_WRITE
** and SFC_SET_DITHER_ON_READ.
*/

enum
{   SFD_DEFAULT_LEVEL   = 0,
    SFD_CUSTOM_LEVEL    = 0x40000000,

    SFD_NO_DITHER       = 500,
    SFD_WHITE           = 501,
    SFD_TRIANGULAR_PDF  = 502
} ;

typedef struct
{   int         type ;
    double      level ;
    const char  *name ;
} SF_DITHER_INFO ;

/* Struct used to retrieve information about a file embedded within a
** larger file. See SFC_GET_EMBED_FILE_INFO.
*/

typedef struct
{   sf_count_t  offset ;
    sf_count_t  length ;
} SF_EMBED_FILE_INFO ;

/* Struct used to retrieve music sample information from a file.
*/

typedef struct
{   int basenote ;
    int gain ;
    int sustain_mode ;
    int sustain_start, sustain_end ;
    int release_mode ;
    int release_start, reslease_end ;
} SF_INSTRUMENT ;

/* sustain_mode and release_mode will be one of the following. */

enum
{   SF_LOOP_NONE = 800,
    SF_LOOP_FORWARD,
    SF_LOOP_BACKWARD
} ;

/* Open the specified file for read, write or both. On error, this will
** return a NULL pointer. To find the error number, pass a NULL SNDFILE
** to sf_perror () or sf_error_str ().
** All calls to sf_open() should be matched with a call to sf_close().
*/

SNDFILE*    sf_open     (const char *path, int mode, SF_INFO *sfinfo) ;

/* Use the existing file descriptor to create a SNDFILE object. If close_desc
** is TRUE, the file descriptor will be closed when sf_close() is called. If
** it is FALSE, the descritor will not be closed.
** When passed a descriptor like this, the library will assume that the start
** of file header is at the current file offset. This allows sound files within
** larger container files to be read and/or written.
** On error, this will return a NULL pointer. To find the error number, pass a
** NULL SNDFILE to sf_perror () or sf_error_str ().
** All calls to sf_open_fd() should be matched with a call to sf_close().

*/

SNDFILE*    sf_open_fd  (int fd, int mode, SF_INFO *sfinfo, int close_desc) ;

/* sf_error () returns a error number which can be translated to a text
** string using sf_error_number().
*/

int     sf_error        (SNDFILE *sndfile) ;

/* sf_strerror () returns to the caller a pointer to the current error message for
** the given SNDFILE.
*/

const char* sf_strerror (SNDFILE *sndfile) ;

/* sf_error_number () allows the retrieval of the error string for each internal
** error number.
**
*/

const char* sf_error_number (int errnum) ;

/* The following three error functions are deprecated but they will remain in the
** library for the forseeable future. The function sf_strerror() should be used
** in their place.
*/

int     sf_perror       (SNDFILE *sndfile) ;
int     sf_error_str    (SNDFILE *sndfile, char* str, size_t len) ;


/* Return TRUE if fields of the SF_INFO struct are a valid combination of values. */

int     sf_command  (SNDFILE *sndfile, int command, void *data, int datasize) ;

/* Return TRUE if fields of the SF_INFO struct are a valid combination of values. */

int     sf_format_check (const SF_INFO *info) ;

/* Seek within the waveform data chunk of the SNDFILE. sf_seek () uses
** the same values for whence (SEEK_SET, SEEK_CUR and SEEK_END) as
** stdio.h function fseek ().
** An offset of zero with whence set to SEEK_SET will position the
** read / write pointer to the first data sample.
** On success sf_seek returns the current position in (multi-channel)
** samples from the start of the file.
** Please see the libsndfile documentation for moving the read pointer
** separately from the write pointer on files open in mode SFM_RDWR.
** On error all of these functions return -1.
*/

sf_count_t  sf_seek         (SNDFILE *sndfile, sf_count_t frames, int whence) ;

/* Functions for retrieving and setting string data within sound files.
** Not all file types support this features; AIFF and WAV do. For both
** functions, the str_type parameter must be one of the SF_STR_* values
** defined above.
** On error, sf_set_string() returns non-zero while sf_get_string()
** returns NULL.
*/

int sf_set_string (SNDFILE *sndfile, int str_type, const char* str) ;

const char* sf_get_string (SNDFILE *sndfile, int str_type) ;

/* Functions for reading/writing the waveform data of a sound file.
*/

sf_count_t  sf_read_raw     (SNDFILE *sndfile, void *ptr, sf_count_t bytes) ;
sf_count_t  sf_write_raw    (SNDFILE *sndfile, void *ptr, sf_count_t bytes) ;

/* Functions for reading and writing the data chunk in terms of frames.
** The number of items actually read/written = frames * number of channels.
**     sf_xxxx_raw      read/writes the raw data bytes from/to the file
**     sf_xxxx_short    passes data in the native short format
**     sf_xxxx_int      passes data in the native int format
**     sf_xxxx_float    passes data in the native float format
**     sf_xxxx_double   passes data in the native double format
** All of these read/write function return number of frames read/written.
*/

sf_count_t  sf_readf_short  (SNDFILE *sndfile, short *ptr, sf_count_t frames) ;
sf_count_t  sf_writef_short (SNDFILE *sndfile, short *ptr, sf_count_t frames) ;

sf_count_t  sf_readf_int    (SNDFILE *sndfile, int *ptr, sf_count_t frames) ;
sf_count_t  sf_writef_int   (SNDFILE *sndfile, int *ptr, sf_count_t frames) ;

sf_count_t  sf_readf_float  (SNDFILE *sndfile, float *ptr, sf_count_t frames) ;
sf_count_t  sf_writef_float (SNDFILE *sndfile, float *ptr, sf_count_t frames) ;

sf_count_t  sf_readf_double     (SNDFILE *sndfile, double *ptr, sf_count_t frames) ;
sf_count_t  sf_writef_double    (SNDFILE *sndfile, double *ptr, sf_count_t frames) ;

/* Functions for reading and writing the data chunk in terms of items.
** Otherwise similar to above.
** All of these read/write function return number of items read/written.
*/

sf_count_t  sf_read_short   (SNDFILE *sndfile, short *ptr, sf_count_t items) ;
sf_count_t  sf_write_short  (SNDFILE *sndfile, short *ptr, sf_count_t items) ;

sf_count_t  sf_read_int     (SNDFILE *sndfile, int *ptr, sf_count_t items) ;
sf_count_t  sf_write_int    (SNDFILE *sndfile, int *ptr, sf_count_t items) ;

sf_count_t  sf_read_float   (SNDFILE *sndfile, float *ptr, sf_count_t items) ;
sf_count_t  sf_write_float  (SNDFILE *sndfile, float *ptr, sf_count_t items) ;

sf_count_t  sf_read_double  (SNDFILE *sndfile, double *ptr, sf_count_t items) ;
sf_count_t  sf_write_double (SNDFILE *sndfile, double *ptr, sf_count_t items) ;

/* Close the SNDFILE and clean up all memory allocations associated with this
** file.
** Returns 0 on success, or an error number.
*/

int     sf_close        (SNDFILE *sndfile) ;

#ifdef __cplusplus
}       /* extern "C" */
#endif  /* __cplusplus */

#endif  /* SNDFILE_H */
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


#ifndef SNDFILE_H
#endif

#define SF_BUFFER_LEN           (8192*2)
#define SF_FILENAME_LEN         (256)
#define SF_HEADER_LEN           (4096)
#define SF_TEXT_LEN             (1024)
#define SF_SYSERR_LEN           (256)
#define SF_MAX_STRINGS          (16)

#define SF_SEEK_ERROR           ((sf_count_t) -1)


#define BITWIDTH2BYTES(x)   (((x) + 7) / 8)

/*  For some reason sizeof returns an unsigned  value which causes
**  a warning when that value is added or subtracted from a signed
**  value. Use SIGNED_SIZEOF instead.
*/
#define     SIGNED_SIZEOF(x)    ((int) sizeof (x))

#define     ARRAY_LEN(x)    ((int) (sizeof (x) / sizeof ((x) [0])))

#define     SF_MAX(a,b)     ((a) > (b) ? (a) : (b))
#define     SF_MIN(a,b)     ((a) < (b) ? (a) : (b))

enum
{   /* PEAK chunk location. */
    SF_PEAK_START       = 42,
    SF_PEAK_END         = 43,

    /* PEAK chunk location. */
    SF_SCALE_MAX        = 52,
    SF_SCALE_MIN        = 53,

    /* str_flags values. */
    SF_STR_ALLOW_START  = 0x0100,
    SF_STR_ALLOW_END    = 0x0200,

    /* Location of strings. */
    SF_STR_LOCATE_START = 0x0400,
    SF_STR_LOCATE_END   = 0x0800,

    SFD_TYPEMASK        = 0x0FFFFFFF
} ;

#define     SFM_MASK    (SFM_READ | SFM_WRITE | SFM_RDWR)
#define     SFM_UNMASK  (~SFM_MASK)

/*---------------------------------------------------------------------------------------
** Formats that may be supported at some time in the future.
** When support is finalised, these values move to src/sndfile.h.
*/

enum
{   /* Work in progress. */

    /* Formats supported read only. */
    SF_FORMAT_WVE           = 0x4020000,        /* Psion ALaw Sound File */
    SF_FORMAT_TXW           = 0x4030000,        /* Yamaha TX16 sampler file */
    SF_FORMAT_DWD           = 0x4040000,        /* DiamondWare Digirized */

    /* Following are detected but not supported. */
    SF_FORMAT_OGG           = 0x4090000,

    SF_FORMAT_REX           = 0x40A0000,        /* Propellorheads Rex/Rcy */
    SF_FORMAT_SD2           = 0x40C0000,        /* Sound Designer 2 */
    SF_FORMAT_REX2          = 0x40D0000,        /* Propellorheads Rex2 */
    SF_FORMAT_KRZ           = 0x40E0000,        /* Kurzweil sampler file */
    SF_FORMAT_WMA           = 0x4100000,        /* Windows Media Audio. */
    SF_FORMAT_SHN           = 0x4110000,        /* Shorten. */
    SF_FORMAT_FLAC          = 0x4120000,

    /* Unsupported encodings. */
    SF_FORMAT_VORBIS        = 0x1001,

    SF_FORMAT_SVX_FIB       = 0x1020,       /* SVX Fibonacci Delta encoding. */
    SF_FORMAT_SVX_EXP       = 0x1021,       /* SVX Exponential Delta encoding. */

    SF_FORMAT_PCM_N         = 0x1030
} ;

/*---------------------------------------------------------------------------------------
**  PEAK_CHUNK_OLD - This chunk type is common to both AIFF and WAVE files although their
**  endian encodings are different.
*/

typedef struct
{   float           value ;     /* signed value of peak */
    unsigned int    position ;  /* the sample frame for the peak */
} PEAK_POS ;

typedef struct
{   unsigned int    version ;   /* version of the PEAK chunk */
    unsigned int    timestamp ; /* secs since 1/1/1970  */
#if HAVE_FLEXIBLE_ARRAY
    PEAK_POS        peaks [] ;  /* the per channel peak info */
#else
    PEAK_POS        peaks [1] ; /* the per channel peak info */
#endif
} PEAK_CHUNK ;

typedef struct
{   int     type ;
    int     flags ;
    char    *str ;
} STR_DATA ;

/*=======================================================================================
**  SF_PRIVATE stuct - a pointer to this struct is passed back to the caller of the
**  sf_open_XXXX functions. The caller however has no knowledge of the struct's
**  contents.
*/

typedef struct sf_private_tag
{   /* Force the compiler to double align the start of buffer. */
    double          buffer      [SF_BUFFER_LEN / sizeof (double)] ;
    char            filename    [SF_FILENAME_LEN] ;

    char            syserr      [SF_SYSERR_LEN] ;

    /* logbuffer and logindex should only be changed within the logging functions
    ** of common.c
    */
    char            logbuffer   [SF_BUFFER_LEN] ;
    unsigned char   header      [SF_HEADER_LEN] ; /* Must be unsigned */
    int             rwf_endian ;    /* Header endian-ness flag. */

    /* Storage and housekeeping data for adding/reading strings from
    ** sound files.
    */
    STR_DATA        strings [SF_MAX_STRINGS] ;
    char            str_storage [SF_BUFFER_LEN] ;
    char            *str_end ;
    int             str_flags ;

    /* Guard value. If this changes the buffers above have overflowed. */
    int             Magick ;

    /* Index variables for maintaining logbuffer and header above. */
    int             logindex ;
    int             headindex, headend ;
    int             has_text ;
    int             do_not_close_descriptor ;

    int             filedes ;

    int             end_of_file ;
    int             error ;

    int             mode ;          /* Open mode : SFM_READ, SFM_WRITE or SFM_RDWR. */
    int             endian ;        /* File endianness : SF_ENDIAN_LITTLE or SF_ENDIAN_BIG. */
    int             float_endswap ; /* Need to endswap float32s? */

    /* Vairables for handling pipes. */
    int             is_pipe ;       /* True if file is a pipe. */
    sf_count_t      pipeoffset ;    /* Number of bytes read from a pipe. */

    /* True if clipping must be performed on float->int conversions. */
    int             add_clipping ;

    SF_INFO         sf ;

    int             have_written ;  /* Has a single write been done to the file? */
    int             has_peak ;      /* Has a PEAK chunk (AIFF and WAVE) been read? */
    int             peak_loc ;      /* Write a PEAK chunk at the start or end of the file? */
    PEAK_CHUNK      *pchunk ;

    sf_count_t      filelength ;    /* Overall length of (embedded) file. */
    sf_count_t      fileoffset ;    /* Offset in number of bytes from beginning of file. */

    sf_count_t      dataoffset ;    /* Offset in number of bytes from beginning of file. */
    sf_count_t      datalength ;    /* Length in bytes of the audio data. */
    sf_count_t      dataend ;       /* Offset to file tailer. */

    int             blockwidth ;    /* Size in bytes of one set of interleaved samples. */
    int             bytewidth ;     /* Size in bytes of one sample (one channel). */

    void            *dither ;
    void            *interleave ;

    int             last_op ;       /* Last operation; either SFM_READ or SFM_WRITE */
    sf_count_t      read_current ;
    sf_count_t      write_current ;

    void            *fdata ;        /*  This is a pointer to dynamically allocated file format
                                    **  specific data.
                                    */

    SF_DITHER_INFO  write_dither ;
    SF_DITHER_INFO  read_dither ;

    int             norm_double ;
    int             norm_float ;

    int             auto_header ;

    int             ieee_replace ;
    /* A set of file specific function pointers */

    sf_count_t      (*read_short)   (struct sf_private_tag*, short *ptr, sf_count_t len) ;
    sf_count_t      (*read_int)     (struct sf_private_tag*, int *ptr, sf_count_t len) ;
    sf_count_t      (*read_float)   (struct sf_private_tag*, float *ptr, sf_count_t len) ;
    sf_count_t      (*read_double)  (struct sf_private_tag*, double *ptr, sf_count_t len) ;

    sf_count_t      (*write_short)  (struct sf_private_tag*, short *ptr, sf_count_t len) ;
    sf_count_t      (*write_int)    (struct sf_private_tag*, int *ptr, sf_count_t len) ;
    sf_count_t      (*write_float)  (struct sf_private_tag*, float *ptr, sf_count_t len) ;
    sf_count_t      (*write_double) (struct sf_private_tag*, double *ptr, sf_count_t len) ;

    sf_count_t      (*seek)         (struct sf_private_tag*, int mode, sf_count_t samples_from_start) ;
    int             (*write_header) (struct sf_private_tag*, int calc_length) ;
    int             (*command)      (struct sf_private_tag*, int command, void *data, int datasize) ;
    int             (*close)        (struct sf_private_tag*) ;

    char            *format_desc ;
} SF_PRIVATE ;



enum
{   SFE_NO_ERROR        = SF_ERR_NO_ERROR,
    SFE_BAD_OPEN_FORMAT = SF_ERR_UNRECOGNISED_FORMAT,
    SFE_SYSTEM          = SF_ERR_SYSTEM,

    SFE_BAD_FILE,
    SFE_BAD_FILE_READ,
    SFE_OPEN_FAILED,
    SFE_BAD_SNDFILE_PTR,
    SFE_BAD_SF_INFO_PTR,
    SFE_BAD_SF_INCOMPLETE,
    SFE_BAD_FILE_PTR,
    SFE_BAD_INT_PTR,
    SFE_BAD_STAT_SIZE,
    SFE_MALLOC_FAILED,
    SFE_UNIMPLEMENTED,
    SFE_BAD_READ_ALIGN,
    SFE_BAD_WRITE_ALIGN,
    SFE_UNKNOWN_FORMAT,
    SFE_NOT_READMODE,
    SFE_NOT_WRITEMODE,
    SFE_BAD_MODE_RW,
    SFE_BAD_SF_INFO,
    SFE_BAD_OFFSET,
    SFE_NO_EMBED_SUPPORT,
    SFE_NO_EMBEDDED_RDWR,
    SFE_NO_PIPE_WRITE,

    SFE_INTERNAL,
    SFE_LOG_OVERRUN,
    SFE_BAD_CONTROL_CMD,
    SFE_BAD_ENDIAN,
    SFE_CHANNEL_COUNT,
    SFE_BAD_RDWR_FORMAT,

    SFE_INTERLEAVE_MODE,
    SFE_INTERLEAVE_SEEK,
    SFE_INTERLEAVE_READ,

    SFE_BAD_SEEK,
    SFE_NOT_SEEKABLE,
    SFE_AMBIGUOUS_SEEK,
    SFE_WRONG_SEEK,
    SFE_SEEK_FAILED,

    SFE_BAD_OPEN_MODE,
    SFE_OPEN_PIPE_RDWR,
    SFE_RDWR_POSITION,

    SFE_STR_NO_SUPPORT,
    SFE_STR_MAX_DATA,
    SFE_STR_MAX_COUNT,
    SFE_STR_BAD_TYPE,
    SFE_STR_NO_ADD_END,
    SFE_STR_BAD_STRING,
    SFE_STR_WEIRD,
    SFE_RDWR_BAD_HEADER,

    SFE_WAV_NO_RIFF,
    SFE_WAV_NO_WAVE,
    SFE_WAV_NO_FMT,
    SFE_WAV_FMT_SHORT,
    SFE_WAV_FMT_TOO_BIG,
    SFE_WAV_BAD_FACT,
    SFE_WAV_BAD_PEAK,
    SFE_WAV_PEAK_B4_FMT,
    SFE_WAV_BAD_FORMAT,
    SFE_WAV_BAD_BLOCKALIGN,
    SFE_WAV_NO_DATA,
    SFE_WAV_ADPCM_NOT4BIT,
    SFE_WAV_ADPCM_CHANNELS,
    SFE_WAV_GSM610_FORMAT,
    SFE_WAV_UNKNOWN_CHUNK,
    SFE_WAV_WVPK_DATA,

    SFE_AIFF_NO_FORM,
    SFE_AIFF_AIFF_NO_FORM,
    SFE_AIFF_COMM_NO_FORM,
    SFE_AIFF_SSND_NO_COMM,
    SFE_AIFF_UNKNOWN_CHUNK,
    SFE_AIFF_COMM_CHUNK_SIZE,
    SFE_AIFF_BAD_COMM_CHUNK,
    SFE_AIFF_PEAK_B4_COMM,
    SFE_AIFF_BAD_PEAK,
    SFE_AIFF_NO_SSND,
    SFE_AIFF_NO_DATA,
    SFE_AIFF_RW_SSND_NOT_LAST,

    SFE_AU_UNKNOWN_FORMAT,
    SFE_AU_NO_DOTSND,
    SFE_AU_EMBED_BAD_LEN,

    SFE_RAW_READ_BAD_SPEC,
    SFE_RAW_BAD_BITWIDTH,
    SFE_RAW_BAD_FORMAT,

    SFE_PAF_NO_MARKER,
    SFE_PAF_VERSION,
    SFE_PAF_UNKNOWN_FORMAT,
    SFE_PAF_SHORT_HEADER,

    SFE_SVX_NO_FORM,
    SFE_SVX_NO_BODY,
    SFE_SVX_NO_DATA,
    SFE_SVX_BAD_COMP,
    SFE_SVX_BAD_NAME_LENGTH,

    SFE_NIST_BAD_HEADER,
    SFE_NIST_CRLF_CONVERISON,
    SFE_NIST_BAD_ENCODING,

    SFE_VOC_NO_CREATIVE,
    SFE_VOC_BAD_FORMAT,
    SFE_VOC_BAD_VERSION,
    SFE_VOC_BAD_MARKER,
    SFE_VOC_BAD_SECTIONS,
    SFE_VOC_MULTI_SAMPLERATE,
    SFE_VOC_MULTI_SECTION,
    SFE_VOC_MULTI_PARAM,
    SFE_VOC_SECTION_COUNT,
    SFE_VOC_NO_PIPE,

    SFE_IRCAM_NO_MARKER,
    SFE_IRCAM_BAD_CHANNELS,
    SFE_IRCAM_UNKNOWN_FORMAT,

    SFE_W64_64_BIT,
    SFE_W64_NO_RIFF,
    SFE_W64_NO_WAVE,
    SFE_W64_NO_FMT,
    SFE_W64_NO_DATA,
    SFE_W64_FMT_SHORT,
    SFE_W64_FMT_TOO_BIG,
    SFE_W64_ADPCM_NOT4BIT,
    SFE_W64_ADPCM_CHANNELS,
    SFE_W64_GSM610_FORMAT,

    SFE_MAT4_BAD_NAME,
    SFE_MAT4_NO_SAMPLERATE,
    SFE_MAT4_ZERO_CHANNELS,

    SFE_MAT5_BAD_ENDIAN,
    SFE_MAT5_NO_BLOCK,
    SFE_MAT5_SAMPLE_RATE,
    SFE_MAT5_ZERO_CHANNELS,

    SFE_PVF_NO_PVF1,
    SFE_PVF_BAD_HEADER,
    SFE_PVF_BAD_BITWIDTH,

    SFE_DWVW_BAD_BITWIDTH,
    SFE_G72X_NOT_MONO,

    SFE_XI_BAD_HEADER,
    SFE_XI_EXCESS_SAMPLES,
    SFE_XI_NO_PIPE,

    SFE_HTK_NO_PIPE,

    SFE_SDS_NOT_SDS,
    SFE_SDS_BAD_BIT_WIDTH,

    SFE_MAX_ERROR           /* This must be last in list. */
} ;

int subformat_to_bytewidth (int format) ;
int s_bitwidth_to_subformat (int bits) ;
int u_bitwidth_to_subformat (int bits) ;

/*  Functions for reading and writing floats and doubles on processors
**  with non-IEEE floats/doubles.
*/
float   float32_be_read     (unsigned char *cptr) ;
float   float32_le_read     (unsigned char *cptr) ;
void    float32_be_write    (float in, unsigned char *out) ;
void    float32_le_write    (float in, unsigned char *out) ;

double  double64_be_read    (unsigned char *cptr) ;
double  double64_le_read    (unsigned char *cptr) ;
void    double64_be_write   (double in, unsigned char *out) ;
void    double64_le_write   (double in, unsigned char *out) ;

/* Functions for writing to the internal logging buffer. */

void    psf_log_printf      (SF_PRIVATE *psf, const char *format, ...) ;
void    psf_log_SF_INFO     (SF_PRIVATE *psf) ;

void    psf_hexdump (void *ptr, int len) ;

/* Functions used when writing file headers. */

int     psf_binheader_writef    (SF_PRIVATE *psf, const char *format, ...) ;
void    psf_asciiheader_printf  (SF_PRIVATE *psf, const char *format, ...) ;

/* Functions used when reading file headers. */

int     psf_binheader_readf (SF_PRIVATE *psf, char const *format, ...) ;

/* Functions used in the write function for updating the peak chunk. */

void    peak_update_short   (SF_PRIVATE *psf, short *ptr, size_t items) ;
void    peak_update_int     (SF_PRIVATE *psf, int *ptr, size_t items) ;
void    peak_update_double  (SF_PRIVATE *psf, double *ptr, size_t items) ;

/* Functions defined in command.c. */

int     psf_get_format_simple_count (void) ;
int     psf_get_format_simple       (SF_FORMAT_INFO *data) ;

int     psf_get_format_info         (SF_FORMAT_INFO *data) ;

int     psf_get_format_major_count  (void) ;
int     psf_get_format_major        (SF_FORMAT_INFO *data) ;

int     psf_get_format_subtype_count    (void) ;
int     psf_get_format_subtype      (SF_FORMAT_INFO *data) ;

void    psf_generate_format_desc (SF_PRIVATE *psf) ;

double  psf_calc_signal_max         (SF_PRIVATE *psf, int normalize) ;
int     psf_calc_max_all_channels   (SF_PRIVATE *psf, double *peaks, int normalize) ;

/* Functions in strings.c. */

const char* psf_get_string (SF_PRIVATE *psf, int str_type) ;
int psf_store_string (SF_PRIVATE *psf, int str_type, const char *str) ;

/* Default seek function. Use for PCM and float encoded data. */
sf_count_t  psf_default_seek (SF_PRIVATE *psf, int mode, sf_count_t samples_from_start) ;

/* Generate the currebt date as a string. */
void    psf_get_date_str (char *str, int maxlen) ;

int macos_guess_file_type (SF_PRIVATE *psf, const char *filename) ;

/*------------------------------------------------------------------------------------
**  File I/O functions which will allow access to large files (> 2 Gig) on
**  some 32 bit OSes. Implementation in file_io.c.
*/

int psf_fopen (SF_PRIVATE *psf, const char *pathname, int flags) ;
int psf_set_stdio (SF_PRIVATE *psf, int mode) ;
int psf_filedes_valid (SF_PRIVATE *psf) ;
void psf_set_file (SF_PRIVATE *psf, int fd) ;

sf_count_t psf_fseek (SF_PRIVATE *psf, sf_count_t offset, int whence) ;
sf_count_t psf_fread (void *ptr, sf_count_t bytes, sf_count_t count, SF_PRIVATE *psf) ;
sf_count_t psf_fwrite (void *ptr, sf_count_t bytes, sf_count_t count, SF_PRIVATE *psf) ;
sf_count_t psf_fgets (char *buffer, sf_count_t bufsize, SF_PRIVATE *psf) ;
sf_count_t psf_ftell (SF_PRIVATE *psf) ;
sf_count_t psf_get_filelen (SF_PRIVATE *psf) ;

int psf_is_pipe (SF_PRIVATE *psf) ;

int psf_ftruncate (SF_PRIVATE *psf, sf_count_t len) ;
int psf_fclose (SF_PRIVATE *psf) ;

/*
void psf_fclearerr (SF_PRIVATE *psf) ;
int psf_ferror (SF_PRIVATE *psf) ;
*/

/*------------------------------------------------------------------------------------
** Functions for reading and writing different file formats.
*/

int     aiff_open   (SF_PRIVATE *psf) ;
int     au_open     (SF_PRIVATE *psf) ;
int     au_nh_open  (SF_PRIVATE *psf) ; /* Headerless version of AU. */
int     avr_open    (SF_PRIVATE *psf) ;
int     htk_open    (SF_PRIVATE *psf) ;
int     ircam_open  (SF_PRIVATE *psf) ;
int     mat4_open   (SF_PRIVATE *psf) ;
int     mat5_open   (SF_PRIVATE *psf) ;
int     nist_open   (SF_PRIVATE *psf) ;
int     paf_open    (SF_PRIVATE *psf) ;
int     pvf_open    (SF_PRIVATE *psf) ;
int     raw_open    (SF_PRIVATE *psf) ;
int     sds_open    (SF_PRIVATE *psf) ;
int     svx_open    (SF_PRIVATE *psf) ;
int     voc_open    (SF_PRIVATE *psf) ;
int     w64_open    (SF_PRIVATE *psf) ;
int     wav_open    (SF_PRIVATE *psf) ;
int     xi_open     (SF_PRIVATE *psf) ;

/* In progress. Do not currently work. */

int     ogg_open    (SF_PRIVATE *psf) ;
int     rx2_open    (SF_PRIVATE *psf) ;
int     sd2_open    (SF_PRIVATE *psf) ;
int     txw_open    (SF_PRIVATE *psf) ;
int     wve_open    (SF_PRIVATE *psf) ;
int     dwd_open    (SF_PRIVATE *psf) ;

int     macbinary3_open (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------------
**  Init functions for a number of common data encodings.
*/

int     pcm_init        (SF_PRIVATE *psf) ;
int     ulaw_init       (SF_PRIVATE *psf) ;
int     alaw_init       (SF_PRIVATE *psf) ;
int     float32_init    (SF_PRIVATE *psf) ;
int     double64_init   (SF_PRIVATE *psf) ;
int     dwvw_init       (SF_PRIVATE *psf, int bitwidth) ;
int     gsm610_init     (SF_PRIVATE *psf) ;
int     vox_adpcm_init  (SF_PRIVATE *psf) ;

int     dither_init     (SF_PRIVATE *psf, int mode) ;

int     wav_w64_ima_init (SF_PRIVATE *psf, int blockalign, int samplesperblock) ;
int     wav_w64_msadpcm_init (SF_PRIVATE *psf, int blockalign, int samplesperblock) ;

int     aiff_ima_init (SF_PRIVATE *psf, int blockalign, int samplesperblock) ;

int     interleave_init (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------------
** Other helper functions.
*/

void    *psf_memset (void *s, int c, sf_count_t n) ;

/*------------------------------------------------------------------------------------
** Here's how we fix systems which don't snprintf / vsnprintf.
** Systems without these functions should use the
*/

#if (defined (WIN32) || defined (_WIN32))
#define LSF_SNPRINTF    _snprintf

#define snprintf _snprintf //XXX really?
#elif       (HAVE_SNPRINTF && ! FORCE_MISSING_SNPRINTF)
#define LSF_SNPRINTF    snprintf
#else
int missing_snprintf (char *str, size_t n, char const *fmt, ...) ;
#define LSF_SNPRINTF    missing_snprintf
#endif

#if (defined (WIN32) || defined (_WIN32))
#define LSF_VSNPRINTF   _vsnprintf
#elif       (HAVE_VSNPRINTF && ! FORCE_MISSING_SNPRINTF)
#define LSF_VSNPRINTF   vsnprintf
#else
int missing_vsnprintf (char *str, size_t n, const char *fmt, ...) ;
#define LSF_VSNPRINTF   missing_vsnprintf
#endif

#endif /* COMMON_H_INCLUDED */

/*------------------------------------------------------------------------------------
** Extra commands for sf_command(). Not for public use yet.
*/

enum
{   SFC_TEST_AIFF_ADD_INST_CHUNK    = 0x2000,
    SFC_TEST_WAV_ADD_INFO_CHUNK     = 0x2010
} ;

/*
** Maybe, one day, make these functions or something like them, public.
**
** Buffer to buffer dithering. Pointer in and out are allowed to point
** to the same buffer for in-place dithering.
*/

#if 0
int sf_dither_short     (const SF_DITHER_INFO *dither, const short *in, short *out, int count) ;
int sf_dither_int       (const SF_DITHER_INFO *dither, const int *in, int *out, int count) ;
int sf_dither_float     (const SF_DITHER_INFO *dither, const float *in, float *out, int count) ;
int sf_dither_double    (const SF_DITHER_INFO *dither, const double *in, double *out, int count) ;
#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 7b45c0ee-5835-4a18-a4ef-994e4cd95b67
*/
/*
** Copyright (C) 1999-2001 Erik de Castro Lopo <erikd@mega-nerd.com>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
** This file is not the same as the original file from Sun Microsystems. Nearly
** all the original definitions and function prototypes that were in the file 
** of this name have been moved to private.h.
*/  

#ifndef G72X_HEADER_FILE
#define G72X_HEADER_FILE

/* 
** Number of samples per block to process. 
** Must be a common multiple of possible bits per sample : 2, 3, 4, 5 and 8.
*/
#define G72x_BLOCK_SIZE     (3*5*8)  

/*  
**  Identifiers for the differing kinds of G72x ADPCM codecs.
**  The identifiers also define the number of encoded bits per sample.
*/

enum
{   G723_16_BITS_PER_SAMPLE = 2,
    G723_24_BITS_PER_SAMPLE = 3, 
    G723_40_BITS_PER_SAMPLE = 5, 

    G721_32_BITS_PER_SAMPLE = 4,
    G721_40_BITS_PER_SAMPLE = 5,

    G723_16_SAMPLES_PER_BLOCK = G72x_BLOCK_SIZE,
    G723_24_SAMPLES_PER_BLOCK = G723_24_BITS_PER_SAMPLE * (G72x_BLOCK_SIZE / G723_24_BITS_PER_SAMPLE), 
    G723_40_SAMPLES_PER_BLOCK = G723_40_BITS_PER_SAMPLE * (G72x_BLOCK_SIZE / G723_40_BITS_PER_SAMPLE), 

    G721_32_SAMPLES_PER_BLOCK = G72x_BLOCK_SIZE,
    G721_40_SAMPLES_PER_BLOCK = G721_40_BITS_PER_SAMPLE * (G72x_BLOCK_SIZE / G721_40_BITS_PER_SAMPLE),

    G723_16_BYTES_PER_BLOCK = (G723_16_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,
    G723_24_BYTES_PER_BLOCK = (G723_24_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,
    G723_40_BYTES_PER_BLOCK = (G723_40_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,

    G721_32_BYTES_PER_BLOCK = (G721_32_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,
    G721_40_BYTES_PER_BLOCK = (G721_40_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8
} ; 

/* 
** This is the public structure for passing data between the caller and
** the G72x encoder and decoder. 
** The private array is used by the encoder and decoder for internal
** state information and should not be changed in any way by the caller.
** When decoding or encoding a stream, the same instance of this struct
** should be used for every call so that the decoder/encoder keeps the
** correct state data between calls.
*/

typedef struct 
{   /* Private data. Don't mess with it. */
    unsigned long   sprivateo [256 / sizeof (long)] ;  

    /* Public data. Read only. */
    int             blocksize, max_bytes, samplesperblock, bytesperblock ;

    /* Public data. Read and write. */
    int             blocks, blockcount, samplecount ;
    unsigned char   block   [G72x_BLOCK_SIZE] ;
    short           samples [G72x_BLOCK_SIZE] ;
} G72x_DATA ;

/* External function definitions. */

int g72x_reader_init (G72x_DATA *data, int codec) ;
int g72x_writer_init (G72x_DATA *data, int codec) ;
/*
**  Initialize the ADPCM state table for the given codec.
**  Return 0 on success, 1 on fail.
*/

int g72x_decode_block (G72x_DATA *data) ;
/*
**  The caller fills data->block with data->bytes bytes before calling the 
**  function. The value data->bytes must be an integer multiple of 
**  data->blocksize and be <= data->max_bytes.
**  When it returns, the caller can read out data->samples samples. 
*/  

int g72x_encode_block (G72x_DATA *data) ;
/*  
**  The caller fills state->samples some integer multiple data->samples_per_block
**  (up to G72x_BLOCK_SIZE) samples before calling the function.
**  When it returns, the caller can read out bytes encoded bytes. 
*/

#endif /* !G72X_HEADER_FILE */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 6ca84e5f-f932-4ba1-87ee-37056d921621
*/

/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#ifndef G72X_PRIVATE_H
#define G72X_PRIVATE_H


/*
** The following is the definition of the state structure used by the
** G.721/G.723 encoder and decoder to preserve their internal state
** between successive calls.  The meanings of the majority of the state 
** structure fields are explained in detail in the CCITT Recommendation 
** G.721.  The field names are essentially identical to variable names 
** in the bit level description of the coding algorithm included in this 
** Recommendation.
*/

typedef struct private_g72x
{   long  yl;   /* Locked or steady state step size multiplier. */
    short yu;   /* Unlocked or non-steady state step size multiplier. */
    short dms;  /* Short term energy estimate. */
    short dml;  /* Long term energy estimate. */
    short ap;   /* Linear weighting coefficient of 'yl' and 'yu'. */

    short a[2]; /* Coefficients of pole portion of prediction filter. */
    short b[6]; /* Coefficients of zero portion of prediction filter. */
    short pk[2];    /*
                    ** Signs of previous two samples of a partially
                    ** reconstructed signal.
                    **/
    short dq[6];    /*
                    ** Previous 6 samples of the quantized difference
                    ** signal represented in an internal floating point
                    ** format.
                    **/
    short sr[2];    /*
                    ** Previous 2 samples of the quantized difference
                    ** signal represented in an internal floating point
                    ** format.
                    */
    char td;    /* delayed tone detect, new in 1988 version */
    
    /*  The following struct members were added for libsndfile. The original 
    **  code worked by calling a set of functions on a sample by sample basis 
    **  which is slow on architectures like Intel x86. For libsndfile, this 
    **  was changed so that the encoding and decoding routines could work on
    **  a block of samples at a time to reduce the function call overhead.
    */
    int     (*encoder) (int, struct private_g72x* state) ;
    int     (*decoder) (int, struct private_g72x* state) ;
    
    int     codec_bits ;
    int     byte_index, sample_index ;
    
} G72x_STATE ;



int predictor_zero (G72x_STATE *state_ptr);

int predictor_pole (G72x_STATE *state_ptr);

int step_size (G72x_STATE *state_ptr);

int quantize (int d, int    y, short *table, int size);

int reconstruct (int sign, int dqln,    int y);

void update (int code_size, int y, int wi, int fi, int dq, int sr, int dqsez, G72x_STATE *state_ptr);

int g721_encoder    (int sample, G72x_STATE *state_ptr);
int g721_decoder    (int code, G72x_STATE *state_ptr);

int g723_16_encoder (int sample, G72x_STATE *state_ptr);
int g723_16_decoder (int code, G72x_STATE *state_ptr);

int g723_24_encoder (int sample, G72x_STATE *state_ptr);
int g723_24_decoder (int code, G72x_STATE *state_ptr);

int g723_40_encoder (int sample, G72x_STATE *state_ptr);
int g723_40_decoder (int code, G72x_STATE *state_ptr);

int unpack_bytes (G72x_DATA *data, int bits) ;
int pack_bytes (G72x_DATA *data, int bits) ;

void private_init_state (G72x_STATE *state_ptr) ;

#endif /* G72X_PRIVATE_H */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: d9ad4da7-0fa3-471d-8020-720b5cfb5e5b
*/

/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


/*
** Many file types (ie WAV, AIFF) use sets of four consecutive bytes as a
** marker indicating different sections of the file.
** The following MAKE_MARKER macro allows th creation of integer constants
** for these markers.
*/

#if (CPU_IS_LITTLE_ENDIAN == 1)
    #define MAKE_MARKER(a,b,c,d)    ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#elif (CPU_IS_BIG_ENDIAN == 1)
    #define MAKE_MARKER(a,b,c,d)    (((a)<<24)|((b)<<16)|((c)<<8)|(d))
#else
    #error "Target CPU endian-ness unknown. May need to hand edit src/config.h"
#endif

/* wo standard endswap macros. */

#define ENDSWAP_SHORT(x)    ((((x)>>8)&0xFF)+(((x)&0xFF)<<8))
#define ENDSWAP_INT(x)      ((((x)>>24)&0xFF)+(((x)>>8)&0xFF00)+(((x)&0xFF00)<<8)+(((x)&0xFF)<<24))
/*
** Macros to handle reading of data of a specific endian-ness into host endian
** shorts and ints. The single input is an unsigned char* pointer to the start
** of the object. There are two versions of each macro as we need to deal with
** both big and little endian CPUs.
*/

#if (CPU_IS_LITTLE_ENDIAN == 1)
    #define LES2H_SHORT(x)          (x)
    #define LEI2H_INT(x)            (x)

    #define BES2H_SHORT(x)          ENDSWAP_SHORT(x)
    #define BEI2H_INT(x)            ENDSWAP_INT(x)

#elif (CPU_IS_BIG_ENDIAN == 1)
    #define LES2H_SHORT(x)          ENDSWAP_SHORT(x)
    #define LEI2H_INT(x)            ENDSWAP_INT(x)

    #define BES2H_SHORT(x)          (x)
    #define BEI2H_INT(x)            (x)

#else
    #error "Target CPU endian-ness unknown. May need to hand edit src/config.h"
#endif

#define LES2H_SHORT_PTR(x)      ((x) [0] + ((x) [1] << 8))
#define LES2H_INT_PTR(x)        (((x) [0] << 16) + ((x) [1] << 24))

#define LET2H_SHORT_PTR(x)      ((x) [1] + ((x) [2] << 8))
#define LET2H_INT_PTR(x)        (((x) [0] << 8) + ((x) [1] << 16) + ((x) [2] << 24))

#define LEI2H_SHORT_PTR(x)      ((x) [2] + ((x) [3] << 8))
#define LEI2H_INT_PTR(x)        ((x) [0] + ((x) [1] << 8) + ((x) [2] << 16) + ((x) [3] << 24))

#define BES2H_SHORT_PTR(x)      (((x) [0] << 8) + (x) [1])
#define BES2H_INT_PTR(x)        (((x) [0] << 24) + ((x) [1] << 16))

#define BET2H_SHORT_PTR(x)      (((x) [0] << 8) + (x) [1])
#define BET2H_INT_PTR(x)        (((x) [0] << 24) + ((x) [1] << 16) + ((x) [2] << 8))

#define BEI2H_SHORT_PTR(x)      (((x) [0] << 8) + (x) [1])
#define BEI2H_INT_PTR(x)        (((x) [0] << 24) + ((x) [1] << 16) + ((x) [2] << 8) + (x) [3])

/* Endian swapping routines implemented in sfendian.c. */

void    endswap_short_array (short *ptr, int len) ;
void    endswap_int_array   (int *ptr, int len) ;

/* Always swaps 8 byte values whether sizeof (long) == 8 or not. */
void    endswap_long_array  (long *ptr, int len) ;

void    endswap_short_copy  (short *dest, short *src, int len) ;
void    endswap_int_copy    (int *dest, int *src, int len) ;

/* Always swaps 8 byte values whether sizeof (long) == 8 or not. */
void    endswap_long_copy   (long *dest, long *src, int len) ;

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: f0c5cd54-42d3-4237-90ec-11fe24995de7
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#ifndef GSM_H
#define GSM_H


/*
 *  Interface
 */

typedef struct gsm_state *  gsm;
typedef short               gsm_signal;     /* signed 16 bit */
typedef unsigned char       gsm_byte;
typedef gsm_byte            gsm_frame[33];      /* 33 * 8 bits   */

#define GSM_MAGIC       0xD         /* 13 kbit/s RPE-LTP */

#define GSM_PATCHLEVEL      10
#define GSM_MINOR           0
#define GSM_MAJOR           1

#define GSM_OPT_VERBOSE     1
#define GSM_OPT_FAST        2
#define GSM_OPT_LTP_CUT     3
#define GSM_OPT_WAV49       4
#define GSM_OPT_FRAME_INDEX 5
#define GSM_OPT_FRAME_CHAIN 6

gsm  gsm_create     (void);

/* Added for libsndfile : May 6, 2002 */
void gsm_init (gsm);    

void gsm_destroy (gsm); 

int  gsm_print   (FILE *, gsm, gsm_byte  *);
int  gsm_option  (gsm, int, int *);

void gsm_encode  (gsm, gsm_signal *, gsm_byte  *);
int  gsm_decode  (gsm, gsm_byte   *, gsm_signal *);

int  gsm_explode (gsm, gsm_byte   *, gsm_signal *);
void gsm_implode (gsm, gsm_signal *, gsm_byte   *);

#endif  /* GSM_H */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 8cfc7698-5433-4b6f-aeca-967c6fda4dec
*/

/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef AU_HEADER_FILE
#define AU_HEADER_FILE


enum
{   AU_H_G721_32    = 200,
    AU_H_G723_24    = 201,
    AU_H_G723_40    = 202
} ;

int au_g72x_reader_init (SF_PRIVATE *psf, int codec) ;
int au_g72x_writer_init (SF_PRIVATE *psf, int codec) ;

#endif /* AU_HEADER_FILE */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 63affc81-e204-4468-9705-60abe4d10689
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* This file contains definitions commong to WAV and W64 files. */


#ifndef WAV_W64_H_INCLUDED
#define WAV_W64_H_INCLUDED

/*------------------------------------------------------------------------------
** List of known WAV format tags
*/

enum
{
  /* keep sorted for wav_w64_format_str() */
    WAVE_FORMAT_UNKNOWN                 = 0x0000,       /* Microsoft Corporation */
    WAVE_FORMAT_PCM                     = 0x0001,       /* Microsoft PCM format */
    WAVE_FORMAT_MS_ADPCM                = 0x0002,       /* Microsoft ADPCM */
    WAVE_FORMAT_IEEE_FLOAT              = 0x0003,       /* Micrososft 32 bit float format */
    WAVE_FORMAT_VSELP                   = 0x0004,       /* Compaq Computer Corporation */
    WAVE_FORMAT_IBM_CVSD                = 0x0005,       /* IBM Corporation */
    WAVE_FORMAT_ALAW                    = 0x0006,       /* Microsoft Corporation */
    WAVE_FORMAT_MULAW                   = 0x0007,       /* Microsoft Corporation */
    WAVE_FORMAT_OKI_ADPCM               = 0x0010,       /* OKI */
    WAVE_FORMAT_IMA_ADPCM               = 0x0011,       /* Intel Corporation */
    WAVE_FORMAT_MEDIASPACE_ADPCM        = 0x0012,       /* Videologic */
    WAVE_FORMAT_SIERRA_ADPCM            = 0x0013,       /* Sierra Semiconductor Corp */
    WAVE_FORMAT_G723_ADPCM              = 0x0014,       /* Antex Electronics Corporation */
    WAVE_FORMAT_DIGISTD                 = 0x0015,       /* DSP Solutions, Inc. */
    WAVE_FORMAT_DIGIFIX                 = 0x0016,       /* DSP Solutions, Inc. */
    WAVE_FORMAT_DIALOGIC_OKI_ADPCM      = 0x0017,       /*  Dialogic Corporation  */
    WAVE_FORMAT_MEDIAVISION_ADPCM       = 0x0018,       /*  Media Vision, Inc. */
    WAVE_FORMAT_CU_CODEC                = 0x0019,       /* Hewlett-Packard Company */
    WAVE_FORMAT_YAMAHA_ADPCM            = 0x0020,       /* Yamaha Corporation of America */
    WAVE_FORMAT_SONARC                  = 0x0021,       /* Speech Compression */
    WAVE_FORMAT_DSPGROUP_TRUESPEECH     = 0x0022,       /* DSP Group, Inc */
    WAVE_FORMAT_ECHOSC1                 = 0x0023,       /* Echo Speech Corporation */
    WAVE_FORMAT_AUDIOFILE_AF36          = 0x0024,       /* Audiofile, Inc. */
    WAVE_FORMAT_APTX                    = 0x0025,       /* Audio Processing Technology */
    WAVE_FORMAT_AUDIOFILE_AF10          = 0x0026,       /* Audiofile, Inc. */
    WAVE_FORMAT_PROSODY_1612            = 0x0027,       /* Aculab plc */
    WAVE_FORMAT_LRC                     = 0x0028,       /* Merging Technologies S.A. */
    WAVE_FORMAT_DOLBY_AC2               = 0x0030,       /* Dolby Laboratories */
    WAVE_FORMAT_GSM610                  = 0x0031,       /* Microsoft Corporation */
    WAVE_FORMAT_MSNAUDIO                = 0x0032,       /* Microsoft Corporation */
    WAVE_FORMAT_ANTEX_ADPCME            = 0x0033,       /* Antex Electronics Corporation */
    WAVE_FORMAT_CONTROL_RES_VQLPC       = 0x0034,       /* Control Resources Limited */
    WAVE_FORMAT_DIGIREAL                = 0x0035,       /* DSP Solutions, Inc. */
    WAVE_FORMAT_DIGIADPCM               = 0x0036,       /* DSP Solutions, Inc. */
    WAVE_FORMAT_CONTROL_RES_CR10        = 0x0037,       /* Control Resources Limited */
    WAVE_FORMAT_NMS_VBXADPCM            = 0x0038,       /* Natural MicroSystems */
    WAVE_FORMAT_ROLAND_RDAC             = 0x0039,       /* Roland */
    WAVE_FORMAT_ECHOSC3                 = 0x003A,       /* Echo Speech Corporation */
    WAVE_FORMAT_ROCKWELL_ADPCM          = 0x003B,       /* Rockwell International */
    WAVE_FORMAT_ROCKWELL_DIGITALK       = 0x003C,       /* Rockwell International */
    WAVE_FORMAT_XEBEC                   = 0x003D,       /* Xebec Multimedia Solutions Limited */
    WAVE_FORMAT_G721_ADPCM              = 0x0040,       /* Antex Electronics Corporation */
    WAVE_FORMAT_G728_CELP               = 0x0041,       /* Antex Electronics Corporation */
    WAVE_FORMAT_MSG723                  = 0x0042,       /* Microsoft Corporation */
    WAVE_FORMAT_MPEG                    = 0x0050,       /* Microsoft Corporation */
    WAVE_FORMAT_RT24                    = 0x0052,       /* InSoft Inc. */
    WAVE_FORMAT_PAC                     = 0x0053,       /* InSoft Inc. */
    WAVE_FORMAT_MPEGLAYER3              = 0x0055,       /* MPEG 3 Layer 1 */
    WAVE_FORMAT_LUCENT_G723             = 0x0059,       /* Lucent Technologies */
    WAVE_FORMAT_CIRRUS                  = 0x0060,       /* Cirrus Logic */
    WAVE_FORMAT_ESPCM                   = 0x0061,       /* ESS Technology */
    WAVE_FORMAT_VOXWARE                 = 0x0062,       /* Voxware Inc */
    WAVE_FORMAT_CANOPUS_ATRAC           = 0x0063,       /* Canopus, Co., Ltd. */
    WAVE_FORMAT_G726_ADPCM              = 0x0064,       /* APICOM */
    WAVE_FORMAT_G722_ADPCM              = 0x0065,       /* APICOM */
    WAVE_FORMAT_DSAT                    = 0x0066,       /* Microsoft Corporation */
    WAVE_FORMAT_DSAT_DISPLAY            = 0x0067,       /* Microsoft Corporation */
    WAVE_FORMAT_VOXWARE_BYTE_ALIGNED    = 0x0069,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_AC8             = 0x0070,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_AC10            = 0x0071,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_AC16            = 0x0072,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_AC20            = 0x0073,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_RT24            = 0x0074,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_RT29            = 0x0075,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_RT29HW          = 0x0076,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_VR12            = 0x0077,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_VR18            = 0x0078,       /* Voxware Inc. */
    WAVE_FORMAT_VOXWARE_TQ40            = 0x0079,       /* Voxware Inc. */
    WAVE_FORMAT_SOFTSOUND               = 0x0080,       /* Softsound, Ltd. */
    WAVE_FORMAT_VOXARE_TQ60             = 0x0081,       /* Voxware Inc. */
    WAVE_FORMAT_MSRT24                  = 0x0082,       /* Microsoft Corporation */
    WAVE_FORMAT_G729A                   = 0x0083,       /* AT&T Laboratories */
    WAVE_FORMAT_MVI_MV12                = 0x0084,       /* Motion Pixels */
    WAVE_FORMAT_DF_G726                 = 0x0085,       /* DataFusion Systems (Pty) (Ltd) */
    WAVE_FORMAT_DF_GSM610               = 0x0086,       /* DataFusion Systems (Pty) (Ltd) */
    /* removed because duplicate */
    /* WAVE_FORMAT_ISIAUDIO             = 0x0088, */    /* Iterated Systems, Inc. */
    WAVE_FORMAT_ONLIVE                  = 0x0089,       /* OnLive! Technologies, Inc. */
    WAVE_FORMAT_SBC24                   = 0x0091,       /* Siemens Business Communications Systems */
    WAVE_FORMAT_DOLBY_AC3_SPDIF         = 0x0092,       /* Sonic Foundry */
    WAVE_FORMAT_ZYXEL_ADPCM             = 0x0097,       /* ZyXEL Communications, Inc. */
    WAVE_FORMAT_PHILIPS_LPCBB           = 0x0098,       /* Philips Speech Processing */
    WAVE_FORMAT_PACKED                  = 0x0099,       /* Studer Professional Audio AG */
    WAVE_FORMAT_RHETOREX_ADPCM          = 0x0100,       /* Rhetorex, Inc. */

    /* removed because of the following */
    /* WAVE_FORMAT_IRAT                 = 0x0101,*/     /* BeCubed Software Inc. */

    /* these three are unofficial */
    IBM_FORMAT_MULAW                    = 0x0101,       /* IBM mu-law format */
    IBM_FORMAT_ALAW                     = 0x0102,       /* IBM a-law format */
    IBM_FORMAT_ADPCM                    = 0x0103,       /* IBM AVC Adaptive Differential PCM format */

    WAVE_FORMAT_VIVO_G723               = 0x0111,       /* Vivo Software */
    WAVE_FORMAT_VIVO_SIREN              = 0x0112,       /* Vivo Software */
    WAVE_FORMAT_DIGITAL_G723            = 0x0123,       /* Digital Equipment Corporation */
    WAVE_FORMAT_CREATIVE_ADPCM          = 0x0200,       /* Creative Labs, Inc */
    WAVE_FORMAT_CREATIVE_FASTSPEECH8    = 0x0202,       /* Creative Labs, Inc */
    WAVE_FORMAT_CREATIVE_FASTSPEECH10   = 0x0203,       /* Creative Labs, Inc */
    WAVE_FORMAT_QUARTERDECK             = 0x0220,       /* Quarterdeck Corporation */
    WAVE_FORMAT_FM_TOWNS_SND            = 0x0300,       /* Fujitsu Corporation */
    WAVE_FORMAT_BZV_DIGITAL             = 0x0400,       /* Brooktree Corporation */
    WAVE_FORMAT_VME_VMPCM               = 0x0680,       /* AT&T Labs, Inc. */
    WAVE_FORMAT_OLIGSM                  = 0x1000,       /* Ing C. Olivetti & C., S.p.A. */
    WAVE_FORMAT_OLIADPCM                = 0x1001,       /* Ing C. Olivetti & C., S.p.A. */
    WAVE_FORMAT_OLICELP                 = 0x1002,       /* Ing C. Olivetti & C., S.p.A. */
    WAVE_FORMAT_OLISBC                  = 0x1003,       /* Ing C. Olivetti & C., S.p.A. */
    WAVE_FORMAT_OLIOPR                  = 0x1004,       /* Ing C. Olivetti & C., S.p.A. */
    WAVE_FORMAT_LH_CODEC                = 0x1100,       /* Lernout & Hauspie */
    WAVE_FORMAT_NORRIS                  = 0x1400,       /* Norris Communications, Inc. */
    /* removed because duplicate */
    /* WAVE_FORMAT_ISIAUDIO             = 0x1401, */    /* AT&T Labs, Inc. */
    WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS = 0x1500,       /* AT&T Labs, Inc. */
    WAVE_FORMAT_DVM                     = 0x2000,       /* FAST Multimedia AG */
    WAVE_FORMAT_INTERWAV_VSC112         = 0x7150,       /* ????? */
    WAVE_FORMAT_EXTENSIBLE              = 0xFFFE
} ;

typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
} MIN_WAV_FMT ;

typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
    unsigned short  extrabytes ;
    unsigned short  dummy ;
} WAV_FMT_SIZE20 ;

typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
    unsigned short  extrabytes ;
    unsigned short  samplesperblock ;
    unsigned short  numcoeffs ;
    struct
    {   short   coeff1 ;
        short   coeff2 ;
    }   coeffs [7] ;
} MS_ADPCM_WAV_FMT ;

typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
    unsigned short  extrabytes ;
    unsigned short  samplesperblock ;
} IMA_ADPCM_WAV_FMT ;

typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
    unsigned short  extrabytes ;
    unsigned short  auxblocksize ;
} G72x_ADPCM_WAV_FMT ;


typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
    unsigned short  extrabytes ;
    unsigned short  samplesperblock ;
} GSM610_WAV_FMT ;

typedef struct
{   unsigned int    esf_field1 ;
    unsigned short  esf_field2 ;
    unsigned short  esf_field3 ;
    char            esf_field4 [8] ;
} EXT_SUBFORMAT ;

typedef struct
{   unsigned short  format ;
    unsigned short  channels ;
    unsigned int    samplerate ;
    unsigned int    bytespersec ;
    unsigned short  blockalign ;
    unsigned short  bitwidth ;
    unsigned short  extrabytes ;
    unsigned short  validbits ;
    unsigned int    channelmask ;
    EXT_SUBFORMAT   esf ;
} EXTENSIBLE_WAV_FMT ;

typedef union
{   unsigned short      format ;
    MIN_WAV_FMT         min ;
    IMA_ADPCM_WAV_FMT   ima ;
    MS_ADPCM_WAV_FMT    msadpcm ;
    G72x_ADPCM_WAV_FMT  g72x ;
    EXTENSIBLE_WAV_FMT  ext ;
    GSM610_WAV_FMT      gsm610 ;
    WAV_FMT_SIZE20      size20 ;
    char                padding [512] ;
} WAV_FMT ;

typedef struct
{   int frames ;
} FACT_CHUNK ;

#define     WAV_W64_GSM610_BLOCKSIZE    65
#define     WAV_W64_GSM610_SAMPLES      320

/*------------------------------------------------------------------------------------
**  Functions defined in wav_ms_adpcm.c
*/

#define MSADPCM_ADAPT_COEFF_COUNT   7

void    msadpcm_write_adapt_coeffs (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------------
**  Functions defined in wav_gsm610.c
*/

int     wav_w64_srate2blocksize (int srate_chan_product) ;
char const* wav_w64_format_str (int k) ;
int     wav_w64_read_fmt_chunk (SF_PRIVATE *psf, WAV_FMT *wav_fmt, int structsize) ;

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 877fde12-9be3-4a31-8a5a-fdae39958613
*/
/*
** Copyright (C) 1999-2001 Erik de Castro Lopo <erikd@mega-nerd.com>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
** This file is not the same as the original file from Sun Microsystems. Nearly
** all the original definitions and function prototypes that were in the file 
** of this name have been moved to private.h.
*/  

#ifndef G72X_HEADER_FILE
#define G72X_HEADER_FILE

/* 
** Number of samples per block to process. 
** Must be a common multiple of possible bits per sample : 2, 3, 4, 5 and 8.
*/
#define G72x_BLOCK_SIZE     (3*5*8)  

/*  
**  Identifiers for the differing kinds of G72x ADPCM codecs.
**  The identifiers also define the number of encoded bits per sample.
*/

enum
{   G723_16_BITS_PER_SAMPLE = 2,
    G723_24_BITS_PER_SAMPLE = 3, 
    G723_40_BITS_PER_SAMPLE = 5, 

    G721_32_BITS_PER_SAMPLE = 4,
    G721_40_BITS_PER_SAMPLE = 5,

    G723_16_SAMPLES_PER_BLOCK = G72x_BLOCK_SIZE,
    G723_24_SAMPLES_PER_BLOCK = G723_24_BITS_PER_SAMPLE * (G72x_BLOCK_SIZE / G723_24_BITS_PER_SAMPLE), 
    G723_40_SAMPLES_PER_BLOCK = G723_40_BITS_PER_SAMPLE * (G72x_BLOCK_SIZE / G723_40_BITS_PER_SAMPLE), 

    G721_32_SAMPLES_PER_BLOCK = G72x_BLOCK_SIZE,
    G721_40_SAMPLES_PER_BLOCK = G721_40_BITS_PER_SAMPLE * (G72x_BLOCK_SIZE / G721_40_BITS_PER_SAMPLE),

    G723_16_BYTES_PER_BLOCK = (G723_16_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,
    G723_24_BYTES_PER_BLOCK = (G723_24_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,
    G723_40_BYTES_PER_BLOCK = (G723_40_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,

    G721_32_BYTES_PER_BLOCK = (G721_32_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8,
    G721_40_BYTES_PER_BLOCK = (G721_40_BITS_PER_SAMPLE * G72x_BLOCK_SIZE) / 8
} ; 

/* 
** This is the public structure for passing data between the caller and
** the G72x encoder and decoder. 
** The private array is used by the encoder and decoder for internal
** state information and should not be changed in any way by the caller.
** When decoding or encoding a stream, the same instance of this struct
** should be used for every call so that the decoder/encoder keeps the
** correct state data between calls.
*/

typedef struct 
{   /* Private data. Don't mess with it. */
    unsigned long   sprivateo [256 / sizeof (long)] ;  

    /* Public data. Read only. */
    int             blocksize, max_bytes, samplesperblock, bytesperblock ;

    /* Public data. Read and write. */
    int             blocks, blockcount, samplecount ;
    unsigned char   block   [G72x_BLOCK_SIZE] ;
    short           samples [G72x_BLOCK_SIZE] ;
} G72x_DATA ;

/* External function definitions. */

int g72x_reader_init (G72x_DATA *data, int codec) ;
int g72x_writer_init (G72x_DATA *data, int codec) ;
/*
**  Initialize the ADPCM state table for the given codec.
**  Return 0 on success, 1 on fail.
*/

int g72x_decode_block (G72x_DATA *data) ;
/*
**  The caller fills data->block with data->bytes bytes before calling the 
**  function. The value data->bytes must be an integer multiple of 
**  data->blocksize and be <= data->max_bytes.
**  When it returns, the caller can read out data->samples samples. 
*/  

int g72x_encode_block (G72x_DATA *data) ;
/*  
**  The caller fills state->samples some integer multiple data->samples_per_block
**  (up to G72x_BLOCK_SIZE) samples before calling the function.
**  When it returns, the caller can read out bytes encoded bytes. 
*/

#endif /* !G72X_HEADER_FILE */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 6ca84e5f-f932-4ba1-87ee-37056d921621
*/

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#ifndef PRIVATE_H
#define PRIVATE_H

/* Added by Erik de Castro Lopo */
#define USE_FLOAT_MUL
#define FAST
#define WAV49  
/* Added by Erik de Castro Lopo */



typedef short               word;       /* 16 bit signed int    */
typedef int                 longword;   /* 32 bit signed int    */

typedef unsigned short      uword;      /* unsigned word    */
typedef unsigned int        ulongword;  /* unsigned longword    */

struct gsm_state 
{   word            dp0[ 280 ] ;

    word            z1;         /* preprocessing.c, Offset_com. */
    longword        L_z2;       /*                  Offset_com. */
    int             mp;         /*                  Preemphasis */

    word            u[8] ;          /* short_term_aly_filter.c  */
    word            LARpp[2][8] ;   /*                              */
    word            j;              /*                              */

    word            ltp_cut;        /* long_term.c, LTP crosscorr.  */
    word            nrp;            /* 40 */    /* long_term.c, synthesis   */
    word            v[9] ;          /* short_term.c, synthesis  */
    word            msr;            /* decoder.c,   Postprocessing  */

    char            verbose;        /* only used if !NDEBUG     */
    char            fast;           /* only used if FAST        */

    char            wav_fmt;        /* only used if WAV49 defined   */
    unsigned char   frame_index;    /*            odd/even chaining */
    unsigned char   frame_chain;    /*   half-byte to carry forward */

    /* Moved here from code.c where it was defined as static */
    word e[50] ;
} ;

typedef struct gsm_state GSM_STATE ;

#define MIN_WORD    (-32767 - 1)
#define MAX_WORD      32767

#define MIN_LONGWORD    (-2147483647 - 1)
#define MAX_LONGWORD      2147483647

/* Signed arithmetic shift right. */
static C_INLINE word
SASR_W (word x, word by)
{   return (x >> by) ;
} /* SASR */

static C_INLINE longword
SASR_L (longword x, word by)
{   return (x >> by) ;
} /* SASR */

/*
 *  Prototypes from add.c
 */
word    gsm_mult        (word a, word b) ;
longword gsm_L_mult     (word a, word b) ;
word    gsm_mult_r      (word a, word b) ;

word    gsm_div         (word num, word denum) ;

word    gsm_add         (word a, word b ) ;
longword gsm_L_add  (longword a, longword b ) ;

word    gsm_sub         (word a, word b) ;
longword gsm_L_sub  (longword a, longword b) ;

word    gsm_abs         (word a) ;

word    gsm_norm        (longword a ) ;

longword gsm_L_asl      (longword a, int n) ;
word    gsm_asl         (word a, int n) ;

longword gsm_L_asr      (longword a, int n) ;
word    gsm_asr         (word a, int n) ;

/*
 *  Inlined functions from add.h 
 */

static C_INLINE longword
GSM_MULT_R (word a, word b)
{   return (((longword) (a)) * ((longword) (b)) + 16384) >> 15 ;
} /* GSM_MULT_R */

static C_INLINE longword
GSM_MULT (word a, word b)
{   return (((longword) (a)) * ((longword) (b))) >> 15 ;
} /* GSM_MULT */

static C_INLINE longword
GSM_L_MULT (word a, word b)
{   return ((longword) (a)) * ((longword) (b)) << 1 ;
} /* GSM_L_MULT */

static C_INLINE longword
GSM_L_ADD (longword a, longword b)
{   ulongword utmp ;
    
    if (a < 0 && b < 0)
    {   utmp = (ulongword)-((a) + 1) + (ulongword)-((b) + 1) ;
        return (utmp >= (ulongword) MAX_LONGWORD) ? MIN_LONGWORD : -(longword)utmp-2 ;
        } ;
    
    if (a > 0 && b > 0)
    {   utmp = (ulongword) a + (ulongword) b ;
        return (utmp >= (ulongword) MAX_LONGWORD) ? MAX_LONGWORD : utmp ;
        } ;
    
    return a + b ;
} /* GSM_L_ADD */

static C_INLINE longword
GSM_ADD (word a, word b)
{   longword ltmp ;

    ltmp = ((longword) a) + ((longword) b) ;

    if (ltmp >= MAX_WORD)
        return MAX_WORD ;
    if (ltmp <= MIN_WORD)
        return MIN_WORD ;

    return ltmp ;
} /* GSM_ADD */

static C_INLINE longword
GSM_SUB (word a, word b)
{   longword ltmp ;

    ltmp = ((longword) a) - ((longword) b) ;
    
    if (ltmp >= MAX_WORD)
        ltmp = MAX_WORD ;
    else if (ltmp <= MIN_WORD)
        ltmp = MIN_WORD ;
    
    return ltmp ;
} /* GSM_SUB */

static C_INLINE word
GSM_ABS (word a)
{
    if (a > 0)
        return a ;
    if (a == MIN_WORD)
        return MAX_WORD ;
    return -a ;
} /* GSM_ADD */


/*
 *  More prototypes from implementations..
 */
void Gsm_Coder (
        struct gsm_state    * S,
        word    * s,    /* [0..159] samples     IN  */
        word    * LARc, /* [0..7] LAR coefficients  OUT */
        word    * Nc,   /* [0..3] LTP lag       OUT     */
        word    * bc,   /* [0..3] coded LTP gain    OUT     */
        word    * Mc,   /* [0..3] RPE grid selection    OUT     */
        word    * xmaxc,/* [0..3] Coded maximum amplitude OUT   */
        word    * xMc) ;/* [13*4] normalized RPE samples OUT    */

void Gsm_Long_Term_Predictor (      /* 4x for 160 samples */
        struct gsm_state * S,
        word    * d,    /* [0..39]   residual signal    IN  */
        word    * dp,   /* [-120..-1] d'        IN  */
        word    * e,    /* [0..40]          OUT */
        word    * dpp,  /* [0..40]          OUT */
        word    * Nc,   /* correlation lag      OUT */
        word    * bc) ; /* gain factor          OUT */

void Gsm_LPC_Analysis (
        struct gsm_state * S,
        word * s,       /* 0..159 signals   IN/OUT  */
        word * LARc) ;   /* 0..7   LARc's   OUT */

void Gsm_Preprocess (
        struct gsm_state * S,
        word * s, word * so) ;

void Gsm_Encoding (
        struct gsm_state * S,
        word    * e,    
        word    * ep,   
        word    * xmaxc,
        word    * Mc,   
        word    * xMc) ;

void Gsm_Short_Term_Analysis_Filter (
        struct gsm_state * S,
        word    * LARc, /* coded log area ratio [0..7]  IN  */
        word    * d) ;  /* st res. signal [0..159]  IN/OUT  */

void Gsm_Decoder (
        struct gsm_state * S,
        word    * LARcr,    /* [0..7]       IN  */
        word    * Ncr,      /* [0..3]       IN  */
        word    * bcr,      /* [0..3]       IN  */
        word    * Mcr,      /* [0..3]       IN  */
        word    * xmaxcr,   /* [0..3]       IN  */
        word    * xMcr,     /* [0..13*4]        IN  */
        word    * s) ;      /* [0..159]     OUT     */

void Gsm_Decoding (
        struct gsm_state * S,
        word    xmaxcr,
        word    Mcr,
        word    * xMcr,     /* [0..12]      IN  */
        word    * erp) ;    /* [0..39]      OUT     */

void Gsm_Long_Term_Synthesis_Filtering (
        struct gsm_state* S,
        word    Ncr,
        word    bcr,
        word    * erp,      /* [0..39]        IN    */
        word    * drp) ;    /* [-120..-1] IN, [0..40] OUT   */

void Gsm_RPE_Decoding (
    /*-struct gsm_state *S,-*/
        word xmaxcr,
        word Mcr,
        word * xMcr,  /* [0..12], 3 bits             IN      */
        word * erp) ; /* [0..39]                     OUT     */

void Gsm_RPE_Encoding (
        /*-struct gsm_state * S,-*/
        word    * e,            /* -5..-1][0..39][40..44     IN/OUT  */
        word    * xmaxc,        /*                              OUT */
        word    * Mc,           /*                              OUT */
        word    * xMc) ;        /* [0..12]                      OUT */

void Gsm_Short_Term_Synthesis_Filter (
        struct gsm_state * S,
        word    * LARcr,    /* log area ratios [0..7]  IN   */
        word    * drp,      /* received d [0...39]     IN   */
        word    * s) ;      /* signal   s [0..159]    OUT   */

void Gsm_Update_of_reconstructed_short_time_residual_signal (
        word    * dpp,      /* [0...39] IN  */
        word    * ep,       /* [0...39] IN  */
        word    * dp) ;     /* [-120...-1]  IN/OUT  */

/*
 *  Tables from table.c
 */
#ifndef GSM_TABLE_C

extern word gsm_A [8], gsm_B [8], gsm_MIC [8], gsm_MAC [8] ;
extern word gsm_INVA [8] ;
extern word gsm_DLB [4], gsm_QLB [4] ;
extern word gsm_H [11] ;
extern word gsm_NRFAC [8] ;
extern word gsm_FAC [8] ;

#endif  /* GSM_TABLE_C */

/*
 *  Debugging
 */
#ifdef NDEBUG

#   define  gsm_debug_words(a, b, c, d)     /* nil */
#   define  gsm_debug_longwords(a, b, c, d)     /* nil */
#   define  gsm_debug_word(a, b)            /* nil */
#   define  gsm_debug_longword(a, b)        /* nil */

#else   /* !NDEBUG => DEBUG */

    void  gsm_debug_words     (char * name, int, int, word *) ;
    void  gsm_debug_longwords (char * name, int, int, longword *) ;
    void  gsm_debug_longword  (char * name, longword) ;
    void  gsm_debug_word      (char * name, word) ;

#endif /* !NDEBUG */

#endif  /* PRIVATE_H */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 8bc5fdf2-e8c8-4686-9bd7-a30b512bef0c
*/

/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* Version 1.3 */


/*============================================================================
**  On Intel Pentium processors (especially PIII and probably P4), converting
**  from float to int is very slow. To meet the C specs, the code produced by
**  most C compilers targeting Pentium needs to change the FPU rounding mode
**  before the float to int conversion is performed.
**
**  Changing the FPU rounding mode causes the FPU pipeline to be flushed. It
**  is this flushing of the pipeline which is so slow.
**
**  Fortunately the ISO C99 specifications define the functions lrint, lrintf,
**  llrint and llrintf which fix this problem as a side effect.
**
**  On Unix-like systems, the configure process should have detected the
**  presence of these functions. If they weren't found we have to replace them
**  here with a standard C cast.
*/

/*
**  The C99 prototypes for lrint and lrintf are as follows:
**
**      long int lrintf (float x) ;
**      long int lrint  (double x) ;
*/


/*
**  The presence of the required functions are detected during the configure
**  process and the values HAVE_LRINT and HAVE_LRINTF are set accordingly in
**  the config.h file.
*/

#define     HAVE_LRINT_REPLACEMENT  0

#if (HAVE_LRINT && HAVE_LRINTF)

    /*
    **  These defines enable functionality introduced with the 1999 ISO C
    **  standard. They must be defined before the inclusion of math.h to
    **  engage them. If optimisation is enabled, these functions will be
    **  inlined. With optimisation switched off, you have to link in the
    **  maths library using -lm.
    */

    #define _ISOC9X_SOURCE  1
    #define _ISOC99_SOURCE  1

    #define __USE_ISOC9X    1
    #define __USE_ISOC99    1


#elif ( defined (WIN32) || defined (_WIN32) || defined(__PLATFORM_WIN32__) || defined(__LITTLE_ENDIAN__) )

    #undef      HAVE_LRINT_REPLACEMENT
    #define     HAVE_LRINT_REPLACEMENT  1

    /*
    **  Win32 doesn't seem to have these functions.
    **  Therefore implement inline versions of these functions here.
    */

    __inline long int
    lrint (double flt)
    {   int intgr ;

        _asm
        {   fld flt
            fistp intgr
            } ;

        return intgr ;
    }

    __inline long int
    lrintf (float flt)
    {   int intgr ;

        _asm
        {   fld flt
            fistp intgr
            } ;

        return intgr ;
    }

#elif (defined (__MWERKS__) && defined (macintosh))

    /* This MacOS 9 solution was provided by Stephane Letz */

    #undef      HAVE_LRINT_REPLACEMENT
    #define     HAVE_LRINT_REPLACEMENT  1

    #undef  lrint
    #undef  lrintf

    #define lrint   double2int
    #define lrintf  float2int

    inline int
    float2int (register float in)
    {   long res [2] ;

        asm
        {   fctiw   in, in
            stfd     in, res
        }
        return res [1] ;
    } /* float2int */

    inline int
    double2int (register double in)
    {   long res [2] ;

        asm
        {   fctiw   in, in
            stfd     in, res
        }
        return res [1] ;
    } /* double2int */

#elif (defined (__MACH__) && defined (__APPLE__))

    /* For Apple MacOSX. */

    #undef      HAVE_LRINT_REPLACEMENT
    #define     HAVE_LRINT_REPLACEMENT  1

    #undef lrint
    #undef lrintf

    #define lrint   double2int
    #define lrintf  float2int

    inline static long int
    float2int (register float in)
    {   int res [2] ;

        __asm__ __volatile__
        (   "fctiw	%1, %1\n\t"
            "stfd	%1, %0"
            : "=m" (res)    /* Output */
            : "f" (in)      /* Input */
            : "memory"
            ) ;

        return res [1] ;
    } /* lrintf */

    inline static long int
    double2int (register double in)
    {   int res [2] ;

        __asm__ __volatile__
        (   "fctiw	%1, %1\n\t"
            "stfd	%1, %0"
            : "=m" (res)    /* Output */
            : "f" (in)      /* Input */
            : "memory"
            ) ;

        return res [1] ;
    } /* lrint */

#else
    #ifndef __sgi
    // #warning "Don't have the functions lrint() and lrintf()."
    // #warning "Replacing these functions with a standard C cast."
    #endif


    #define lrint(dbl)      ((long) (dbl))
    #define lrintf(flt)     ((long) (flt))

#endif



/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 42db1693-ff61-4051-bac1-e4d24c4e30b7
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* Some defines that microsoft 'forgot' to implement. */

#ifndef S_IRWXU
#define S_IRWXU     0000700 /* rwx, owner */
#endif

#ifndef     S_IRUSR
#define     S_IRUSR 0000400 /* read permission, owner */
#endif

#ifndef     S_IWUSR
#define     S_IWUSR 0000200 /* write permission, owner */
#endif

#ifndef     S_IXUSR
#define     S_IXUSR 0000100 /* execute/search permission, owner */
#endif

#ifndef     S_IRWXG
#define     S_IRWXG 0000070 /* rwx, group */
#endif

#ifndef     S_IRGRP
#define     S_IRGRP 0000040 /* read permission, group */
#endif

#ifndef     S_IWGRP
#define     S_IWGRP 0000020 /* write permission, grougroup */
#endif

#ifndef     S_IXGRP
#define     S_IXGRP 0000010 /* execute/search permission, group */
#endif

#ifndef     S_IRWXO
#define     S_IRWXO 0000007 /* rwx, other */
#endif

#ifndef     S_IROTH
#define     S_IROTH 0000004 /* read permission, other */
#endif

#ifndef     S_IWOTH
#define     S_IWOTH 0000002 /* write permission, other */
#endif

#ifndef     S_IXOTH
#define     S_IXOTH 0000001 /* execute/search permission, other */
#endif

#ifndef S_ISFIFO
#define S_ISFIFO(mode)  (((mode) & _S_IFMT) == _S_IFIFO)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)   (((mode) & _S_IFREG) == _S_IFREG)
#endif

/*
**  Don't know if these are still needed.
**
**  #define _IFMT       _S_IFMT
**  #define _IFREG      _S_IFREG
*/
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 253aea6d-6299-46fd-8d06-bc5f6224c8fe
*/

#endif
