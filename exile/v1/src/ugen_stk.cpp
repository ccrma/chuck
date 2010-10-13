/*----------------------------------------------------------------------------
    ChucK Concurrent, On-the-fly Audio Programming Language
      Compiler and Virtual Machine

    Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

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
// file: ugen_stk.cpp
// desc: import library for Synthesis ToolKit (STK) - Perry Cook + Gary Scavone
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ugen_stk.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MY_FLOAT double
// see stk_query()...


/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate and rawwave path variables
    can be queried and modified via Stk.  In addition,
    this class provides error handling and
    byte-swapping functions.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__STK_H)
#define __STK_H

#include <string>
#include <map>

// Most data in STK is passed and calculated with the
// following user-definable floating-point type.  You
// can change this to "float" if you prefer or perhaps
// a "long double" in the future.

//XXX sample is already defined in chuck_def.h!!!
#if !defined(SAMPLE) 
   typedef SAMPLE MY_FLOAT;
#endif
// The "MY_FLOAT" type will be deprecated in STK
// versions higher than 4.1.2 and replaced with the variable
// "StkFloat".
//typedef double StkFloat;
//#if defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)
//  #pragma deprecated(MY_FLOAT)
//#else
//  typedef StkFloat MY_FLOAT __attribute__ ((deprecated));
//#endif

//! STK error handling class.
/*!
  This is a fairly abstract exception handling class.  There could
  be sub-classes to take care of more specific error conditions ... or
  not.
*/
class StkError
{
public:
  enum TYPE { 
    WARNING,
    DEBUG_WARNING,
    FUNCTION_ARGUMENT,
    FILE_NOT_FOUND,
    FILE_UNKNOWN_FORMAT,
    FILE_ERROR,
    PROCESS_THREAD,
    PROCESS_SOCKET,
    PROCESS_SOCKET_IPADDR,
    AUDIO_SYSTEM,
    MIDI_SYSTEM,
    UNSPECIFIED
  };

public: // SWAP formerly protected
  char message[256];
  TYPE type;

public:
  //! The constructor.
  StkError(const char *p, TYPE tipe = StkError::UNSPECIFIED);

  //! The destructor.
  virtual ~StkError(void);

  //! Prints "thrown" error message to stdout.
  virtual void printMessage(void);

  //! Returns the "thrown" error message TYPE.
  virtual const TYPE& getType(void) { return type; }

  //! Returns the "thrown" error message string.
  virtual const char *getMessage(void) const { return message; }
};


class Stk
{
public:

  typedef unsigned long STK_FORMAT;
  static const STK_FORMAT STK_SINT8;   /*!< -128 to +127 */
  static const STK_FORMAT STK_SINT16;  /*!< -32768 to +32767 */
  static const STK_FORMAT STK_SINT32;  /*!< -2147483648 to +2147483647. */
  static const STK_FORMAT MY_FLOAT32; /*!< Normalized between plus/minus 1.0. */
  static const STK_FORMAT MY_FLOAT64; /*!< Normalized between plus/minus 1.0. */

  //! Static method which returns the current STK sample rate.
  static MY_FLOAT sampleRate(void);

  //! Static method which sets the STK sample rate.
  /*!
    The sample rate set using this method is queried by all STK
    classes which depend on its value.  It is initialized to the
    default SRATE set in Stk.h.  Many STK classes use the sample rate
    during instantiation.  Therefore, if you wish to use a rate which
    is different from the default rate, it is imperative that it be
    set \e BEFORE STK objects are instantiated.
  */
  static void setSampleRate(MY_FLOAT newRate);

  //! Static method which returns the current rawwave path.
  static std::string rawwavePath(void);

  //! Static method which sets the STK rawwave path.
  static void setRawwavePath(std::string newPath);

  //! Static method which byte-swaps a 16-bit data type.
  static void swap16(unsigned char *ptr);

  //! Static method which byte-swaps a 32-bit data type.
  static void swap32(unsigned char *ptr);

  //! Static method which byte-swaps a 64-bit data type.
  static void swap64(unsigned char *ptr);

  //! Static cross-platform method to sleep for a number of milliseconds.
  static void sleep(unsigned long milliseconds);

public: // SWAP formerly private
  static MY_FLOAT srate;
  static std::string rawwavepath;

public: // SWAP formerly protected

  //! Default constructor.
  Stk(void);

  //! Class destructor.
  virtual ~Stk(void);

  //! Function for error reporting and handling.
  static void handleError( const char *message, StkError::TYPE type );

};

// Here are a few other useful typedefs.
typedef signed short SINT16;
typedef signed int SINT32;
typedef float FLOAT32;
typedef double FLOAT64;

// Boolean values
#define FALSE 0
#define TRUE 1

// The default sampling rate.
#define SRATE (MY_FLOAT) 44100.0

// The default real-time audio input and output buffer size.  If
// clicks are occuring in the input and/or output sound stream, a
// larger buffer size may help.  Larger buffer sizes, however, produce
// more latency.
#define RT_BUFFER_SIZE 512

// The default rawwave path value is set with the preprocessor
// definition RAWWAVE_PATH.  This can be specified as an argument to
// the configure script, in an integrated development environment, or
// below.  The global STK rawwave path variable can be dynamically set
// with the Stk::setRawwavePath() function.  This value is
// concatenated to the beginning of all references to rawwave files in
// the various STK core classes (ex. Clarinet.cpp).  If you wish to
// move the rawwaves directory to a different location in your file
// system, you will need to set this path definition appropriately.
#if !defined(RAWWAVE_PATH)
  #define RAWWAVE_PATH "../../rawwaves/"
#endif

#define PI (MY_FLOAT) 3.14159265359
#define TWO_PI (MY_FLOAT) (2 * PI)

#define ONE_OVER_128 (MY_FLOAT) 0.0078125

#if defined(__WINDOWS_PTHREAD__)
  #define __OS_WINDOWS_CYGWIN__
  #define __STK_REALTIME__
#elif defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)
  #define __OS_WINDOWS__
  #define __STK_REALTIME__
#elif defined(__LINUX_OSS__) || defined(__LINUX_ALSA__) || defined(__LINUX_JACK__)
  #define __OS_LINUX__
  #define __STK_REALTIME__
#elif defined(__IRIX_AL__)
  #define __OS_IRIX__
  #define __STK_REALTIME__
#elif defined(__MACOSX_CORE__)
  #define __OS_MACOSX__
  #define __STK_REALTIME__
#endif

//#define _STK_DEBUG_

#endif


//------------------------------------------------------------------------------
// name: union what
// desc: the litmus
//------------------------------------------------------------------------------
union what { long x; char y[sizeof(long)]; };
t_CKBOOL little_endian = FALSE;

//-----------------------------------------------------------------------------
// name: stk_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY stk_query( Chuck_DL_Query * QUERY )
{
    // set srate
    Stk::setSampleRate( QUERY->srate );
    // test for endian
    what w; w.x = 1;
    little_endian = (t_CKBOOL)w.y[0];

    //! \sectionMain STK

    //! \section stk - instruments

    // add BandedWG
    QUERY->ugen_add( QUERY, "BandedWG", NULL ); 
    QUERY->ugen_func( QUERY, BandedWG_ctor, BandedWG_dtor, BandedWG_tick, BandedWG_pmsg );
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_noteOn, NULL, "float", "noteOn" ); //! noteOn
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_noteOff, NULL, "float", "noteOff" ); //! noteOff
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_pluck, NULL, "float", "pluck" ); //! pluck waveguide
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_startBowing, NULL, "float", "startBowing" ); //! pluck waveguide
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_stopBowing, NULL, "float", "stopBowing" ); //! pluck waveguide
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_freq, BandedWG_cget_freq, "float", "freq" ); //! strike Position
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_bowRate, BandedWG_cget_bowRate, "float", "bowRate" ); //! strike Position
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_bowPressure, BandedWG_cget_bowPressure, "float", "bowPressure" ); //! strike Position
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_preset, BandedWG_cget_preset, "int", "preset" ); //! strike Position
    QUERY->ugen_ctrl( QUERY, BandedWG_ctrl_strikePosition, BandedWG_cget_strikePosition, "float", "strikePosition" ); //! strike Position

    // add BlowBotl
    QUERY->ugen_add( QUERY, "BlowBotl", NULL );
    QUERY->ugen_func( QUERY, BlowBotl_ctor, BlowBotl_dtor, BlowBotl_tick, BlowBotl_pmsg );
    QUERY->ugen_ctrl( QUERY, BlowBotl_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowBotl_ctrl_noteOff, NULL, "float", "noteOff" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowBotl_ctrl_startBlowing, NULL, "float", "startBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowBotl_ctrl_stopBlowing, NULL, "float", "stopBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowBotl_ctrl_freq, BlowBotl_cget_freq, "float", "freq" ); //! frequency
    QUERY->ugen_ctrl( QUERY, BlowBotl_ctrl_rate, BlowBotl_cget_rate, "float", "rate" ); //! frequency

        // add BlowHole
    QUERY->ugen_add( QUERY, "BlowHole", NULL );
    QUERY->ugen_func( QUERY, BlowHole_ctor, BlowHole_dtor, BlowHole_tick, BlowHole_pmsg );
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_noteOff, NULL, "float", "noteOff" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_startBlowing, NULL, "float", "startBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_stopBlowing, NULL, "float", "stopBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_freq, BlowHole_cget_freq, "float", "freq" ); //! frequency
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_vent, BlowHole_cget_vent, "float", "vent" ); //! vent frequency
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_tonehole, BlowHole_cget_tonehole, "float", "tonehole" ); //! tonehole size
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_reed, BlowHole_cget_reed, "float", "reed" ); //! reed stiffness
    QUERY->ugen_ctrl( QUERY, BlowHole_ctrl_rate, BlowHole_cget_rate, "float", "rate" ); //! rate of change

	// add Bowed
    QUERY->ugen_add( QUERY, "Bowed", NULL );
    QUERY->ugen_func( QUERY, Bowed_ctor, Bowed_dtor, Bowed_tick, Bowed_pmsg );
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_noteOff, NULL, "float", "noteOff" ); //! note off
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_startBowing, NULL, "float", "startBowing" ); //! begin bowing instrument
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_stopBowing, NULL, "float", "stopBowing" ); //! stop bowing
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_freq, Bowed_cget_freq, "float", "freq" ); //!
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_rate, Bowed_cget_rate, "float", "rate" ); //!
    QUERY->ugen_ctrl( QUERY, Bowed_ctrl_vibrato, Bowed_cget_vibrato, "float", "vibrato" ); //!    

	// add Brass
    QUERY->ugen_add( QUERY, "Brass", NULL );
    QUERY->ugen_func( QUERY, Brass_ctor, Brass_dtor, Brass_tick, Brass_pmsg );
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_noteOff, NULL, "float", "noteOff" ); //! note on
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_clear, NULL, "float", "clear" ); //! clear instrument
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_startBlowing, NULL, "float", "startBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_stopBlowing, NULL, "float", "stopBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_freq, Brass_cget_freq, "float", "freq" ); //! frequency
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_rate, Brass_cget_rate, "float", "rate" ); //! rate of change
    QUERY->ugen_ctrl( QUERY, Brass_ctrl_lip, Brass_cget_lip, "float", "lip" ); //! lip stiffness

	// add Clarinet
    QUERY->ugen_add( QUERY, "Clarinet", NULL );
    QUERY->ugen_func( QUERY, Clarinet_ctor, Clarinet_dtor, Clarinet_tick, Clarinet_pmsg );
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_noteOff, NULL, "float", "noteOff" ); //! note on
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_clear, NULL, "float", "clear" ); //! note on
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_startBlowing, NULL, "float", "startBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_stopBlowing, NULL, "float", "stopBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_freq, Clarinet_cget_freq, "float", "freq" ); //! frequency
    QUERY->ugen_ctrl( QUERY, Clarinet_ctrl_rate, Clarinet_cget_rate, "float", "rate" ); //! rate of change

	// add Flute
    QUERY->ugen_add( QUERY, "Flute", NULL );
    QUERY->ugen_func( QUERY, Flute_ctor, Flute_dtor, Flute_tick, Flute_pmsg );
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_noteOff, NULL, "float", "noteOff" ); //! note on
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_clear, NULL, "float", "clear" ); //! note on
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_startBlowing, NULL, "float", "startBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_stopBlowing, NULL, "float", "stopBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_freq, Flute_cget_freq, "float", "freq" ); //! frequency
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_rate, Flute_cget_rate, "float", "rate" ); //! frequency
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_jetReflection, Flute_cget_jetReflection, "float", "jetReflection" ); //! rate of change
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_jetDelay, Flute_cget_jetDelay, "float", "jetDelay" ); //! rate of change
    QUERY->ugen_ctrl( QUERY, Flute_ctrl_endReflection, Flute_cget_endReflection, "float", "endReflection" ); //! rate of change

    // add Mandolin
    //! see \example mand-o-matic.ck
    QUERY->ugen_add( QUERY, "Mandolin", NULL );
    QUERY->ugen_func( QUERY, Mandolin_ctor, Mandolin_dtor, Mandolin_tick, Mandolin_pmsg );
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_pluck, NULL, "float", "pluck" );  //! pluck string with given amplitude 
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_freq, Mandolin_cget_freq, "float", "freq" ); //! string frequency 
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_pluckPos, Mandolin_cget_pluckPos, "float", "pluckPos" ); //! set pluck position ( 0-1) along string
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_bodySize, Mandolin_cget_bodySize, "float", "bodySize" ); //! modify instrument size
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_stringDamping, Mandolin_cget_stringDamping, "float", "stringDamping" ); //! control string damping
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_stringDetune, Mandolin_cget_stringDetune, "float", "stringDetune" ); //! control detuning of string pair
    QUERY->ugen_ctrl( QUERY, Mandolin_ctrl_afterTouch, NULL, "float", "afterTouch" ); //!aftertouch

    // add ModalBar
    //! see \example modalbot.ck
    QUERY->ugen_add( QUERY, "ModalBar", NULL );
    QUERY->ugen_func( QUERY, ModalBar_ctor, ModalBar_dtor, ModalBar_tick, ModalBar_pmsg );
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_strike, NULL, "float", "strike" ); //! strike bar
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_damp, NULL, "float", "damp" ); //! damp bar
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_noteOn, NULL, "float", "noteOn" ); //! start note
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_noteOff, NULL, "float", "noteOff" ); //! stop note
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_clear, NULL, "float", "clear" ); //! reset

    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_preset, ModalBar_cget_preset, "int", "preset" ); //! choose preset
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_freq, ModalBar_cget_freq, "float", "freq" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_strikePosition, ModalBar_cget_strikePosition, "float", "strikePosition" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_stickHardness, ModalBar_cget_stickHardness, "float", "stickHardness" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_masterGain, ModalBar_cget_masterGain, "float", "masterGain" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_directGain, ModalBar_cget_directGain, "float", "directGain" ); //! set frequency

    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_mode, ModalBar_cget_mode, "int", "mode" ); //! choose mode
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_modeRatio, ModalBar_cget_modeRatio, "float", "modeRatio" ); //! mode edit
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_modeRadius, ModalBar_cget_modeRadius, "float", "modeRadius" ); //! mode dit
    QUERY->ugen_ctrl( QUERY, ModalBar_ctrl_modeGain, ModalBar_cget_modeGain, "float", "modeGain" ); //! mode edit

    // add Moog
    //! see \example moogie.ck
    QUERY->ugen_add( QUERY, "Moog", NULL );
    QUERY->ugen_func( QUERY, Moog_ctor, Moog_dtor, Moog_tick, Moog_pmsg );
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_noteOn, NULL, "float", "noteOn" ); //! start note
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_freq, Moog_cget_freq, "float", "freq" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_modSpeed, Moog_cget_modSpeed, "float", "modSpeed" ); //! modulation speed
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_modDepth, Moog_cget_modDepth, "float", "modDepth" ); //! modulation depth
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_filterQ, Moog_cget_filterQ, "float", "filterQ" ); //! filter Q value
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_filterSweepRate, Moog_cget_filterSweepRate, "float", "filterSweepRate" ); //! filter sweep rate
    QUERY->ugen_ctrl( QUERY, Moog_ctrl_afterTouch, NULL , "float", "afterTouch" ); // aftertouch
        
    // add Saxofony
    QUERY->ugen_add( QUERY, "Saxofony", NULL );
    QUERY->ugen_func( QUERY, Saxofony_ctor, Saxofony_dtor, Saxofony_tick, Saxofony_pmsg );
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_noteOn, NULL, "float", "noteOn" ); //! note on
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_noteOff, NULL, "float", "noteOff" ); //! note on
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_clear, NULL, "float", "clear" ); //! clear instrument
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_startBlowing, NULL, "float", "startBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_stopBlowing, NULL, "float", "stopBlowing" ); //! note on
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_freq, Saxofony_cget_freq, "float", "freq" ); //! frequency
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_rate, Saxofony_cget_rate, "float", "rate" ); //! frequency
    QUERY->ugen_ctrl( QUERY, Saxofony_ctrl_blowPosition, Saxofony_cget_blowPosition, "float", "blowPosition" ); //! lip stiffness

    // add Shakers
    //! see \example shake-o-matic.ck
    QUERY->ugen_add( QUERY, "Shakers", NULL );
    QUERY->ugen_func( QUERY, Shakers_ctor, Shakers_dtor, Shakers_tick, Shakers_pmsg );
    QUERY->ugen_ctrl( QUERY, Shakers_ctrl_freq, Shakers_cget_freq, "float", "freq" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, Shakers_ctrl_noteOn, NULL, "float", "noteOn" ); //! start shake with given amplitude
    QUERY->ugen_ctrl( QUERY, Shakers_ctrl_noteOff, NULL, "float", "noteOff" ); //! stop shake
    QUERY->ugen_ctrl( QUERY, Shakers_ctrl_which, Shakers_cget_which, "int", "which" ); //! select instrument
    
    // add Sitar
    QUERY->ugen_add( QUERY, "Sitar", NULL );
    QUERY->ugen_func( QUERY, Sitar_ctor, Sitar_dtor, Sitar_tick, Sitar_pmsg );
    QUERY->ugen_ctrl( QUERY, Sitar_ctrl_pluck, NULL, "float", "pluck" ); //! 
    QUERY->ugen_ctrl( QUERY, Sitar_ctrl_noteOn, NULL, "float", "noteOn" ); 
    QUERY->ugen_ctrl( QUERY, Sitar_ctrl_noteOff, NULL, "float", "noteOff" ); 
    QUERY->ugen_ctrl( QUERY, Sitar_ctrl_clear, NULL, "float", "clear" ); 
    QUERY->ugen_ctrl( QUERY, Sitar_ctrl_freq, Sitar_cget_freq, "float", "freq" ); 

    // add StifKarp
    //! see \example stifkarp.ck
    QUERY->ugen_add( QUERY, "StifKarp", NULL );
    QUERY->ugen_func( QUERY, StifKarp_ctor, StifKarp_dtor, StifKarp_tick, StifKarp_pmsg );
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_pluck, NULL, "float", "pluck" ); //! 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_noteOn, NULL, "float", "noteOn" ); 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_noteOff, NULL, "float", "noteOff" ); 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_clear, NULL, "float", "clear" ); 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_freq, StifKarp_cget_freq, "float", "freq" ); 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_pickupPosition, StifKarp_cget_pickupPosition, "float", "pickupPosition" ); 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_stretch, StifKarp_cget_stretch, "float", "stretch" ); 
    QUERY->ugen_ctrl( QUERY, StifKarp_ctrl_baseLoopGain, StifKarp_cget_baseLoopGain, "float", "baseLoopGain" ); 

    // add VoicForm
    //! see \example voic-o-form.ck
    QUERY->ugen_add( QUERY, "VoicForm", NULL );
    QUERY->ugen_func( QUERY, VoicForm_ctor, VoicForm_dtor, VoicForm_tick, VoicForm_pmsg );
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_freq, VoicForm_cget_freq, "float", "freq" ); //! frequency 
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_phoneme, VoicForm_cget_phoneme, "string", "phoneme" ); //! select phoneme  ( above ) 
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_noteOn, NULL, "float", "noteOn" ); //! start note
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_noteOff, NULL, "float", "noteOff" ); //! stop note
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_speak, NULL, "float", "speak" ); //! start singing
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_quiet, NULL, "float", "quiet" ); //! stop singing
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_voiced, VoicForm_cget_voiced, "float", "voiced" ); //! set mix for voiced component
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_unVoiced, VoicForm_cget_unVoiced, "float", "unVoiced" ); //! set mix for unvoiced component
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_pitchSweepRate, VoicForm_cget_pitchSweepRate, "float", "pitchSweepRate" ); //! pitch sweep
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_voiceMix, VoicForm_cget_voiceMix, "float", "voiceMix" ); //! voiced/unvoiced mix
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_selPhoneme, VoicForm_cget_selPhoneme, "int", "setPhoneme" ); //! select phoneme by number
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_vibratoFreq, VoicForm_cget_vibratoFreq, "float", "vibratoFreq" );//! vibrato
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_vibratoGain, VoicForm_cget_vibratoGain, "float", "vibratoGain" );//! vibrato depth
    QUERY->ugen_ctrl( QUERY, VoicForm_ctrl_loudness, VoicForm_cget_loudness, "float", "loudness" ); //! 'loudness' of voicee

    //! \section2 stk - fm synths

    // add FM
    QUERY->ugen_add( QUERY, "FM", NULL );
    QUERY->ugen_func( QUERY, FM_ctor, FM_dtor, FM_tick, FM_pmsg );
    QUERY->ugen_ctrl( QUERY, FM_ctrl_freq, FM_cget_freq, "float", "freq" ); //!set frequency
    QUERY->ugen_ctrl( QUERY, FM_ctrl_noteOn, NULL, "float", "noteOn" );  //! trigger note
    QUERY->ugen_ctrl( QUERY, FM_ctrl_noteOff, NULL, "float", "noteOff" ); //! end note
    QUERY->ugen_ctrl( QUERY, FM_ctrl_modDepth, NULL, "float", "modDepth" ); //!modulation Depth
    QUERY->ugen_ctrl( QUERY, FM_ctrl_modSpeed, NULL, "float", "modSpeed" ); //!modulation Speed
    QUERY->ugen_ctrl( QUERY, FM_ctrl_afterTouch, NULL, "float", "afterTouch" ); //!aftertouch
    QUERY->ugen_ctrl( QUERY, FM_ctrl_control1, NULL, "float", "control1" ); //! FM control 1
    QUERY->ugen_ctrl( QUERY, FM_ctrl_control2, NULL, "float", "control2" ); //! FM control 2
    
    // add FM's subclasses

    // add BeeThree
    QUERY->ugen_add( QUERY, "BeeThree", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, BeeThree_ctor, BeeThree_dtor, BeeThree_tick, BeeThree_pmsg );
    QUERY->ugen_ctrl( QUERY, BeeThree_ctrl_noteOn, NULL, "float", "noteOn" ); //!trigger note

    // add FMVoices
    QUERY->ugen_add( QUERY, "FMVoices", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, FMVoices_ctor, FMVoices_dtor, FMVoices_tick, FMVoices_pmsg );
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_noteOn, NULL, "float", "noteOn" );
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_freq, FMVoices_cget_freq, "float", "freq" ); //!voice frequency
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_vowel, NULL, "float", "vowel" ); //!select vowel
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_spectralTilt, NULL, "float", "spectralTilt" ); //! spectral tilt
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_lfoSpeed, NULL, "float", "lfoSpeed" ); //!speed of LFO
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_lfoDepth, NULL, "float", "lfoDepth" ); //!depth of LFO
    QUERY->ugen_ctrl( QUERY, FMVoices_ctrl_adsrTarget, NULL, "float", "adsrTarget" ); //!adsr targets

    // add HevyMetl
    QUERY->ugen_add( QUERY, "HevyMetl", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, HevyMetl_ctor, HevyMetl_dtor, HevyMetl_tick, HevyMetl_pmsg );
    QUERY->ugen_ctrl( QUERY, HevyMetl_ctrl_noteOn, NULL, "float", "noteOn" ); //! trigger note

    // add PercFlut
    QUERY->ugen_add( QUERY, "PercFlut", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, PercFlut_ctor, PercFlut_dtor, PercFlut_tick, PercFlut_pmsg );
    QUERY->ugen_ctrl( QUERY, PercFlut_ctrl_freq, PercFlut_cget_freq, "float", "freq" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, PercFlut_ctrl_noteOn, NULL, "float", "noteOn" ); //!trigger note

    // add Rhodey
    //! see \examples rhodey.ck
    QUERY->ugen_add( QUERY, "Rhodey", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, Rhodey_ctor, Rhodey_dtor, Rhodey_tick, Rhodey_pmsg );
    QUERY->ugen_ctrl( QUERY, Rhodey_ctrl_freq, Rhodey_cget_freq, "float", "freq" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, Rhodey_ctrl_noteOn, NULL, "float", "noteOn" ); //! trigger note

    // add TubeBell
    QUERY->ugen_add( QUERY, "TubeBell", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, TubeBell_ctor, TubeBell_dtor, TubeBell_tick, TubeBell_pmsg );
    QUERY->ugen_ctrl( QUERY, TubeBell_ctrl_freq, TubeBell_cget_freq, "float", "freq" );  //! set frequency
    QUERY->ugen_ctrl( QUERY, TubeBell_ctrl_noteOn, NULL, "float", "noteOn" ); //!  trigger note
    
    // add Wurley
    //! see \examples wurley.ck
    QUERY->ugen_add( QUERY, "Wurley", NULL );
    QUERY->ugen_extends (QUERY, "FM");
    QUERY->ugen_func( QUERY, Wurley_ctor, Wurley_dtor, Wurley_tick, Wurley_pmsg ); 
    QUERY->ugen_ctrl( QUERY, Wurley_ctrl_freq, Wurley_cget_freq, "float", "freq" ); //! set frequency
    QUERY->ugen_ctrl( QUERY, Wurley_ctrl_noteOn, NULL, "float", "noteOn" ); //! trigger note

    //end FM
	
    //! \section stk - delay

    // add Delay
	//! see \example net_relay.ck
    QUERY->ugen_add( QUERY, "Delay", NULL );
    QUERY->ugen_func( QUERY, Delay_ctor, Delay_dtor, Delay_tick, Delay_pmsg );
    QUERY->ugen_ctrl( QUERY, Delay_ctrl_delay, Delay_cget_delay, "dur", "delay" ); //! length of delay
    QUERY->ugen_ctrl( QUERY, Delay_ctrl_max, Delay_cget_max, "dur", "max" ); //! max delay (buffer size) 

    // delay 'subs' not actually extending . 
    
    // add DelayA
    QUERY->ugen_add( QUERY, "DelayA", NULL );
    QUERY->ugen_func( QUERY, DelayA_ctor, DelayA_dtor, DelayA_tick, DelayA_pmsg );
    QUERY->ugen_ctrl( QUERY, DelayA_ctrl_delay, DelayA_cget_delay, "dur", "delay" ); //! length of delay
    QUERY->ugen_ctrl( QUERY, DelayA_ctrl_max, DelayA_cget_max, "dur", "max" ); //! max delay ( buffer size ) 

    // add DelayL
	//! see \example i-robot.ck
    QUERY->ugen_add( QUERY, "DelayL", NULL );
    QUERY->ugen_func( QUERY, DelayL_ctor, DelayL_dtor, DelayL_tick, DelayL_pmsg );
    QUERY->ugen_ctrl( QUERY, DelayL_ctrl_delay, DelayL_cget_delay, "dur", "delay" ); //! length of delay
    QUERY->ugen_ctrl( QUERY, DelayL_ctrl_max, DelayL_cget_max, "dur", "max" ); //! max delay ( buffer size ) 

    // add Echo
    QUERY->ugen_add( QUERY, "Echo", NULL );
    QUERY->ugen_func( QUERY, Echo_ctor, Echo_dtor, Echo_tick, Echo_pmsg );
    QUERY->ugen_ctrl( QUERY, Echo_ctrl_delay, Echo_cget_delay, "dur", "delay" ); //! length of echo
    QUERY->ugen_ctrl( QUERY, Echo_ctrl_max, Echo_cget_max, "dur", "max" ); //! max delay
    QUERY->ugen_ctrl( QUERY, Echo_ctrl_mix, Echo_cget_mix, "float", "mix" ); //! mix level ( wet/dry ) 

    //! \section stk - envelopes

    // add Envelope
	//! see \example sixty.ck
    QUERY->ugen_add( QUERY, "Envelope", NULL );
    QUERY->ugen_func( QUERY, Envelope_ctor, Envelope_dtor, Envelope_tick, Envelope_pmsg );
    QUERY->ugen_ctrl( QUERY, Envelope_ctrl_keyOn, NULL, "int", "keyOn" ); //! ramp to 1.0
    QUERY->ugen_ctrl( QUERY, Envelope_ctrl_keyOff, NULL, "int", "keyOff" ); //! ramp to 0.0
    QUERY->ugen_ctrl( QUERY, Envelope_ctrl_target, Envelope_cget_target, "float", "target" ); //! ramp to arbitrary value.
    QUERY->ugen_ctrl( QUERY, Envelope_ctrl_time, Envelope_cget_time, "float", "time" ); //! time to reach target
    QUERY->ugen_ctrl( QUERY, Envelope_ctrl_rate, Envelope_cget_rate, "float", "rate"); //! rate of change 
    QUERY->ugen_ctrl( QUERY, Envelope_ctrl_value, Envelope_cget_value, "float", "value" ); //! set immediate value

    // add ADSR
	//! see \example adsr.ck
    QUERY->ugen_add( QUERY, "ADSR", NULL );
    QUERY->ugen_extends ( QUERY, "Envelope" );
    QUERY->ugen_func( QUERY, ADSR_ctor, ADSR_dtor, ADSR_tick, ADSR_pmsg );
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_keyOn, NULL, "int", "keyOn" ); //! start the attack for non-zero values
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_keyOff, NULL, "int", "keyOff" ); //! start release for non-zero values
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_attackTime, NULL, "float", "attackTime" ); //! attack time
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_attackRate, ADSR_cget_attackRate, "float", "attackRate" ); //! attack rate
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_decayTime, NULL, "float", "decayTime" ); //! decay time 
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_decayRate, ADSR_cget_decayRate, "float", "decayRate" ); //! decay rate
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_sustainLevel, ADSR_cget_sustainLevel, "float", "sustainLevel" ); //! sustain level
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_releaseTime, NULL, "float", "releaseTime" ); //! release time 
    QUERY->ugen_ctrl( QUERY, ADSR_ctrl_releaseRate, ADSR_cget_releaseRate, "float", "releaseRate" ); //! release rate
    QUERY->ugen_ctrl( QUERY, NULL, ADSR_cget_state, "int", "state" ); //! attack=0, decay=1 , sustain=2, release=3, done=4
    //uhhh we are supposed to have target and value here as well..  d'oh

    //! \section stk - filters
	
    // add BiQuad
    QUERY->ugen_add( QUERY, "BiQuad", NULL ); 
    QUERY->ugen_func( QUERY, BiQuad_ctor, BiQuad_dtor, BiQuad_tick, BiQuad_pmsg );
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_b2, BiQuad_cget_b2, "float", "b2" ); //! b2 coefficient
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_b1, BiQuad_cget_b1, "float", "b1" ); //! b1 coefficient
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_b0, BiQuad_cget_b0, "float", "b0" ); //! b0 coefficient
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_a2, BiQuad_cget_a2, "float", "a2" ); //! a2 coefficient
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_a1, BiQuad_cget_a1, "float", "a1" ); //! a1 coefficient
    QUERY->ugen_ctrl( QUERY, NULL, BiQuad_cget_a0, "float", "a0" ); //! a0 coefficient
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_pfreq, NULL, "float", "pfreq" );  //! set resonance frequency (poles)
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_prad, NULL, "float", "prad" ); //! pole radius (less than 1 to be stable)
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_zfreq, NULL, "float", "zfreq" ); //! notch frequency
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_zrad, NULL, "float", "zrad" ); //! zero radius
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_norm, NULL, "float", "norm" ); //! normalization
    QUERY->ugen_ctrl( QUERY, BiQuad_ctrl_eqzs, NULL, "float", "eqzs" ); //! equal gain zeroes

    // add Filter
    QUERY->ugen_add( QUERY, "Filter", NULL );
    QUERY->ugen_func( QUERY, Filter_ctor, Filter_dtor, Filter_tick, Filter_pmsg );
    QUERY->ugen_ctrl( QUERY, Filter_ctrl_coefs, NULL, "string", "coefs" );

    // Filter subclasses
    
    // add OnePole
    QUERY->ugen_add( QUERY, "OnePole", NULL );
    QUERY->ugen_func( QUERY, OnePole_ctor, OnePole_dtor, OnePole_tick, OnePole_pmsg ); 
    
    QUERY->ugen_ctrl( QUERY, OnePole_ctrl_a1, OnePole_cget_a1, "float", "a1" ); //! filter coefficient
    QUERY->ugen_ctrl( QUERY, OnePole_ctrl_b0, OnePole_cget_b0, "float", "b0" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, OnePole_ctrl_pole, OnePole_cget_pole, "float", "pole" ); //! set pole position along real axis of z-plane

    // add TwoPole
	//! see \example powerup.ck
    QUERY->ugen_add( QUERY, "TwoPole", NULL );
    QUERY->ugen_func( QUERY, TwoPole_ctor, TwoPole_dtor, TwoPole_tick, TwoPole_pmsg );
    QUERY->ugen_ctrl( QUERY, TwoPole_ctrl_a1, TwoPole_cget_a1, "float", "a1" ); //! filter coefficient
    QUERY->ugen_ctrl( QUERY, TwoPole_ctrl_a2, TwoPole_cget_a2, "float", "a2" ); //! filter coefficient
    QUERY->ugen_ctrl( QUERY, TwoPole_ctrl_b0, TwoPole_cget_b0, "float", "b0" ); //! filter coefficient
    QUERY->ugen_ctrl( QUERY, TwoPole_ctrl_freq, TwoPole_cget_freq,   "float", "freq" ); //! filter resonance frequency
    QUERY->ugen_ctrl( QUERY, TwoPole_ctrl_radius, TwoPole_cget_radius, "float", "radius" ); //! filter resonance radius
    QUERY->ugen_ctrl( QUERY, TwoPole_ctrl_norm, TwoPole_cget_norm, "int", "norm" ); //! toggle filter normalization 

    // add OneZero
    QUERY->ugen_add( QUERY, "OneZero", NULL );
    QUERY->ugen_func( QUERY, OneZero_ctor, OneZero_dtor, OneZero_tick, OneZero_pmsg );
    QUERY->ugen_ctrl( QUERY, OneZero_ctrl_zero, OneZero_cget_zero, "float", "zero" ); //! set zero position
    QUERY->ugen_ctrl( QUERY, OneZero_ctrl_b0, OneZero_cget_b0, "float", "b0" ); //! filter coefficient
    QUERY->ugen_ctrl( QUERY, OneZero_ctrl_b1, OneZero_cget_b1, "float", "b1" ); //! filter coefficient 

    // add TwoZero
    QUERY->ugen_add( QUERY, "TwoZero", NULL );
    QUERY->ugen_func( QUERY, TwoZero_ctor, TwoZero_dtor, TwoZero_tick, TwoZero_pmsg );
    QUERY->ugen_ctrl( QUERY, TwoZero_ctrl_b0, TwoZero_cget_b0, "float", "b0" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, TwoZero_ctrl_b1, TwoZero_cget_b1, "float", "b1" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, TwoZero_ctrl_b2, TwoZero_cget_b2, "float", "b2" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, TwoZero_ctrl_freq, TwoZero_cget_freq, "float", "freq" ); //! filter notch frequency
    QUERY->ugen_ctrl( QUERY, TwoZero_ctrl_radius, TwoZero_cget_radius, "float", "radius" ); //! filter notch radius

    // add PoleZero
    QUERY->ugen_add( QUERY, "PoleZero", NULL );
    QUERY->ugen_func( QUERY, PoleZero_ctor, PoleZero_dtor, PoleZero_tick, PoleZero_pmsg );
    QUERY->ugen_ctrl( QUERY, PoleZero_ctrl_a1, PoleZero_cget_a1, "float", "a1" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, PoleZero_ctrl_b0, PoleZero_cget_b0, "float", "b0" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, PoleZero_ctrl_b1, PoleZero_cget_b1, "float", "b1" ); //! filter coefficient 
    QUERY->ugen_ctrl( QUERY, PoleZero_ctrl_blockZero, PoleZero_cget_blockZero, "float", "blockZero" ); //! DC blocking filter with given pole position
    QUERY->ugen_ctrl( QUERY, PoleZero_ctrl_allpass, PoleZero_cget_allpass, "float", "allpass" ); //!allpass filter with given coefficient

    //end Filters

    //! \section stk-reverbs

    // add JCRev
    QUERY->ugen_add( QUERY, "JCRev", NULL );
    QUERY->ugen_func( QUERY, JCRev_ctor, JCRev_dtor, JCRev_tick, JCRev_pmsg );
    QUERY->ugen_ctrl( QUERY, JCRev_ctrl_mix, JCRev_cget_mix, "float", "mix" ); //! mix level

    // add NRev
    QUERY->ugen_add( QUERY, "NRev", NULL );
    QUERY->ugen_func( QUERY, NRev_ctor, NRev_dtor, NRev_tick, NRev_pmsg );
    QUERY->ugen_ctrl( QUERY, NRev_ctrl_mix, NRev_cget_mix, "float", "mix" ); // set effect mix

    // add PRCRev
    QUERY->ugen_add( QUERY, "PRCRev", NULL );
    QUERY->ugen_func( QUERY, PRCRev_ctor, PRCRev_dtor, PRCRev_tick, PRCRev_pmsg );
    QUERY->ugen_ctrl( QUERY, PRCRev_ctrl_mix, PRCRev_cget_mix, "float", "mix" ); //! mix level

    //! \section components

    // add Chorus
    QUERY->ugen_add( QUERY, "Chorus", NULL );
    QUERY->ugen_func( QUERY, Chorus_ctor, Chorus_dtor, Chorus_tick, Chorus_pmsg );
    QUERY->ugen_ctrl( QUERY, Chorus_ctrl_modFreq, Chorus_cget_modFreq, "float", "modFreq" ); //! modulation frequency
    QUERY->ugen_ctrl( QUERY, Chorus_ctrl_modDepth, Chorus_cget_modDepth, "float", "modDepth" ); //! modulation depth
    QUERY->ugen_ctrl( QUERY, Chorus_ctrl_mix, Chorus_cget_mix, "float", "mix" ); //! effect mix

    // add Modulate
    QUERY->ugen_add( QUERY, "Modulate", NULL );
    QUERY->ugen_func( QUERY, Modulate_ctor, Modulate_dtor, Modulate_tick, Modulate_pmsg );
    QUERY->ugen_ctrl( QUERY, Modulate_ctrl_vibratoRate, Modulate_cget_vibratoRate, "float", "vibratoRate" );  //! set rate of vibrato
    QUERY->ugen_ctrl( QUERY, Modulate_ctrl_vibratoGain, Modulate_cget_vibratoGain, "float", "vibratoGain" ); //! gain for vibrato
    QUERY->ugen_ctrl( QUERY, Modulate_ctrl_randomGain, Modulate_cget_randomGain, "float", "randomGain" ); //!  gain for random contribution

    // add PitShift
    QUERY->ugen_add( QUERY, "PitShift", NULL );
    QUERY->ugen_func( QUERY, PitShift_ctor, PitShift_dtor, PitShift_tick, PitShift_pmsg );
    QUERY->ugen_ctrl( QUERY, PitShift_ctrl_shift, PitShift_cget_shift, "float", "shift" ); //! degree of pitch shifting
    QUERY->ugen_ctrl( QUERY, PitShift_ctrl_effectMix, PitShift_cget_effectMix, "float", "effectMix" ); //! mix level

    // add SubNoise
    QUERY->ugen_add( QUERY, "SubNoise", NULL );
    QUERY->ugen_func( QUERY, SubNoise_ctor, SubNoise_dtor, SubNoise_tick, SubNoise_pmsg );
    QUERY->ugen_ctrl( QUERY, SubNoise_ctrl_rate, SubNoise_cget_rate, "int", "rate" ); //! subsampling rate

    //! \section stk - file i/o
    
    // add WvIn
    QUERY->ugen_add( QUERY, "WvIn", NULL );
    QUERY->ugen_func( QUERY, WvIn_ctor, WvIn_dtor, WvIn_tick, WvIn_pmsg );
    QUERY->ugen_ctrl( QUERY, WvIn_ctrl_rate, WvIn_cget_rate, "float", "rate" ); //! playback rate
    QUERY->ugen_ctrl( QUERY, WvIn_ctrl_path, WvIn_cget_path, "string", "path" ); //! specifies file to be played

    // add WaveLoop
    //! see \example dope.ck
    QUERY->ugen_add( QUERY, "WaveLoop", NULL );
    QUERY->ugen_extends ( QUERY, "WvIn" );
    QUERY->ugen_func( QUERY, WaveLoop_ctor, WaveLoop_dtor, WaveLoop_tick, WaveLoop_pmsg );
    QUERY->ugen_ctrl( QUERY, WaveLoop_ctrl_freq, WaveLoop_cget_freq, "float", "freq" ); //! set frequency of playback ( loops / second )
    QUERY->ugen_ctrl( QUERY, WaveLoop_ctrl_phase, WaveLoop_cget_phase, "float", "addPhase" ); //! offset by phase
    QUERY->ugen_ctrl( QUERY, WaveLoop_ctrl_phaseOffset, WaveLoop_cget_phaseOffset, "float", "addPhaseOffset" ); //! set phase offset

    //add WvOut
    QUERY->ugen_add( QUERY, "WvOut", NULL);
    QUERY->ugen_func( QUERY, WvOut_ctor, WvOut_dtor, WvOut_tick, WvOut_pmsg ); 
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_matFilename, NULL, "string", "matFilename"); //!open matlab file for writing
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_sndFilename, NULL, "string", "sndFilename"); //!open snd file for writing
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_wavFilename, NULL, "string", "wavFilename"); //!open WAVE file for writing
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_rawFilename, NULL, "string", "rawFilename"); //!open raw file for writing
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_aifFilename, NULL, "string", "aifFilename"); //!open AIFF file for writing
    QUERY->ugen_ctrl( QUERY, NULL, WvOut_cget_filename, "string", "filename" ); //!get filename
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_record, WvOut_cget_record, "int", "record" ); // !start/stop output
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_closeFile, NULL, "string", "closeFile"); //! close file properly
    QUERY->ugen_ctrl( QUERY, WvOut_ctrl_autoPrefix, WvOut_cget_autoPrefix, "string", "autoPrefix"); //! set/get auto prefix string
    
    return TRUE;
}




/***************************************************/
/*! \class Envelope
    \brief STK envelope base class.

    This class implements a simple envelope
    generator which is capable of ramping to
    a target value by a specified \e rate.
    It also responds to simple \e keyOn and
    \e keyOff messages, ramping to 1.0 on
    keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ENVELOPE_H)
#define __ENVELOPE_H


class Envelope : public Stk
{
public:

  //! Default constructor.
  Envelope(void);

  //! Class destructor.
  virtual ~Envelope(void);

  //! Set target = 1.
  virtual void keyOn(void);

  //! Set target = 0.
  virtual void keyOff(void);

  //! Set the \e rate.
  void setRate(MY_FLOAT aRate);

  //! Set the \e rate based on a time duration.
  void setTime(MY_FLOAT aTime);

  //! Set the target value.
  virtual void setTarget(MY_FLOAT aTarget);

  //! Set current and target values to \e aValue.
  virtual void setValue(MY_FLOAT aValue);

  //! Return the current envelope \e state (0 = at target, 1 otherwise).
  virtual int getState(void) const;

  //! Return one envelope output value.
  virtual MY_FLOAT tick(void);

  //! Return \e vectorSize envelope outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

public:
  MY_FLOAT value;
  MY_FLOAT target;
  MY_FLOAT rate;
  int state;
};

#endif




/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This Envelope subclass implements a
    traditional ADSR (Attack, Decay,
    Sustain, Release) envelope.  It
    responds to simple keyOn and keyOff
    messages, keeping track of its state.
    The \e state = ADSR::DONE after the
    envelope value reaches 0.0 in the
    ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ADSR_H)
#define __ADSR_H


class ADSR : public Envelope
{
 public:

  //! Envelope states.
  enum { ATTACK, DECAY, SUSTAIN, RELEASE, DONE };

  //! Default constructor.
  ADSR(void);

  //! Class destructor.
  ~ADSR(void);

  //! Set target = 1, state = \e ADSR::ATTACK.
  void keyOn(void);

  //! Set target = 0, state = \e ADSR::RELEASE.
  void keyOff(void);

  //! Set the attack rate.
  void setAttackRate(MY_FLOAT aRate);

  //! Set the decay rate.
  void setDecayRate(MY_FLOAT aRate);

  //! Set the sustain level.
  void setSustainLevel(MY_FLOAT aLevel);

  //! Set the release rate.
  void setReleaseRate(MY_FLOAT aRate);

  //! Set the attack rate based on a time duration.
  void setAttackTime(MY_FLOAT aTime);

  //! Set the decay rate based on a time duration.
  void setDecayTime(MY_FLOAT aTime);

  //! Set the release rate based on a time duration.
  void setReleaseTime(MY_FLOAT aTime);

  //! Set sustain level and attack, decay, and release state rates based on time durations.
  void setAllTimes(MY_FLOAT aTime, MY_FLOAT dTime, MY_FLOAT sLevel, MY_FLOAT rTime);

  //! Set the target value.
  void setTarget(MY_FLOAT aTarget);

  //! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, DONE).
  int getState(void) const;

  //! Set to state = ADSR::SUSTAIN with current and target values of \e aValue.
  void setValue(MY_FLOAT aValue);

  //! Return one envelope output value.
  MY_FLOAT tick(void);

  //! Return \e vectorSize envelope outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public:
  MY_FLOAT attackRate;
  MY_FLOAT decayRate;
  MY_FLOAT sustainLevel;
  MY_FLOAT releaseRate;
};

#endif




/***************************************************/
/*! \class Instrmnt
    \brief STK instrument abstract base class.

    This class provides a common interface for
    all STK instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__INSTRMNT_H)
#define __INSTRMNT_H

#include <iostream>

class Instrmnt : public Stk
{
 public:
  //! Default constructor.
  Instrmnt();

  //! Class destructor.
  virtual ~Instrmnt();

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude) = 0;

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude) = 0;

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

  //! Compute one output sample.
  virtual MY_FLOAT tick() = 0;

  //! Computer \e vectorSize outputs and return them in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
  
  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

  public: // SWAP formerly protected
    MY_FLOAT lastOutput;

};

#endif




/***************************************************/
/*! \class Filter
    \brief STK filter class.

    This class implements a generic structure which
    can be used to create a wide range of filters.
    It can function independently or be subclassed
    to provide more specific controls based on a
    particular filter type.

    In particular, this class implements the standard
    difference equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients
    are normalized by a[0].

    The \e gain parameter is applied at the filter
    input and does not affect the coefficient values.
    The default gain value is 1.0.  This structure
    results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FILTER_H)
#define __FILTER_H


class Filter : public Stk
{
public:
  //! Default constructor creates a zero-order pass-through "filter".
  Filter(void);

  //! Overloaded constructor which takes filter coefficients.
  /*!
    An StkError can be thrown if either \e nb or \e na is less than
    one, or if the a[0] coefficient is equal to zero.
  */
  Filter(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients);

  //! Class destructor.
  virtual ~Filter(void);

  //! Clears all internal states of the filter.
  void clear(void);

  //! Set filter coefficients.
  /*!
    An StkError can be thrown if either \e nb or \e na is less than
    one, or if the a[0] coefficient is equal to zero.  If a[0] is not
    equal to 1, the filter coeffcients are normalized by a[0].
  */
  void setCoefficients(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients);

  //! Set numerator coefficients.
  /*!
    An StkError can be thrown if \e nb is less than one.  Any
    previously set denominator coefficients are left unaffected.
    Note that the default constructor sets the single denominator
    coefficient a[0] to 1.0.
  */
  void setNumerator(int nb, MY_FLOAT *bCoefficients);

  //! Set denominator coefficients.
  /*!
    An StkError can be thrown if \e na is less than one or if the
    a[0] coefficient is equal to zero.  Previously set numerator
    coefficients are unaffected unless a[0] is not equal to 1, in
    which case all coeffcients are normalized by a[0].  Note that the
    default constructor sets the single numerator coefficient b[0]
    to 1.0.
  */
  void setDenominator(int na, MY_FLOAT *aCoefficients);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  virtual void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  virtual MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  virtual MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  virtual MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public:
  MY_FLOAT gain;
  int     nB;
  int     nA;
  MY_FLOAT *b;
  MY_FLOAT *a;
  MY_FLOAT *outputs;
  MY_FLOAT *inputs;

};

#endif




/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This protected Filter subclass implements
    a non-interpolating digital delay-line.
    A fixed maximum length of 4095 and a delay
    of zero is set using the default constructor.
    Alternatively, the delay and maximum length
    can be set during instantiation with an
    overloaded constructor.
    
    A non-interpolating delay line is typically
    used in fixed delay-length applications, such
    as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DELAY_H)
#define __DELAY_H


class Delay : public Filter
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  Delay();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  Delay(long theDelay, long maxDelay);

  //! Class destructor.
  virtual ~Delay();

  //! Clears the internal state of the delay line.
  void clear();

  //! Set the delay-line length.
  /*!
    The valid range for \e theDelay is from 0 to the maximum delay-line length.
  */
  void setDelay(long theDelay);
  void set( long delay, long max );

  //! Return the current delay-line length.
  MY_FLOAT getDelay(void) const;

  //! Calculate and return the signal energy in the delay-line.
  MY_FLOAT energy(void) const;

  //! Return the value at \e tapDelay samples from the delay-line input.
  /*!
    The tap point is determined modulo the delay-line length and is
    relative to the last input value (i.e., a tapDelay of zero returns
    the last input value).
  */
  MY_FLOAT contentsAt(unsigned long tapDelay) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  virtual MY_FLOAT nextOut(void) const;

  //! Input one sample to the delay-line and return one output.
  virtual MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the delay-line and return an equal number of outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public:
  long inPoint;
  long outPoint;
  long length;
  MY_FLOAT delay;
};

#endif




/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using first-order
    linear interpolation.  A fixed maximum length
    of 4095 and a delay of zero is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    Linear interpolation is an efficient technique
    for achieving fractional delay lengths, though
    it does introduce high-frequency signal
    attenuation to varying degrees depending on the
    fractional delay setting.  The use of higher
    order Lagrange interpolators can typically
    improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DELAYL_H)
#define __DELAYL_H


class DelayL : public Delay
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  DelayL();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  
  DelayL(MY_FLOAT theDelay, long maxDelay);

  //! Class destructor.
  ~DelayL();

  //! Set the delay-line length.
  /*!
    The valid range for \e theDelay is from 0 to the maximum delay-line length.
  */
  void setDelay(MY_FLOAT theDelay);
  void set( MY_FLOAT delay, long max );

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  MY_FLOAT nextOut(void);

  //! Input one sample to the delay-line and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

 public: // SWAP formerly protected  
  MY_FLOAT alpha;
  MY_FLOAT omAlpha;
  MY_FLOAT nextOutput;
  bool doNextOut;
};

#endif




/***************************************************/
/*! \class BowTabl
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith (1986).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BOWTABL_H)
#define __BOWTABL_H


class BowTabl : public Stk
{
public:
  //! Default constructor.
  BowTabl();

  //! Class destructor.
  ~BowTabl();

  //! Set the table offset value.
  /*!
    The table offset is a bias which controls the
    symmetry of the friction.  If you want the
    friction to vary with direction, use a non-zero
    value for the offset.  The default value is zero.
  */
  void setOffset(MY_FLOAT aValue);

  //! Set the table slope value.
  /*!
   The table slope controls the width of the friction
   pulse, which is related to bow force.
  */
  void setSlope(MY_FLOAT aValue);

  //! Return the last output value.
  MY_FLOAT lastOut(void) const;

  //! Return the function value for \e input.
  /*!
    The function input represents differential
    string-to-bow velocity.
  */
  MY_FLOAT tick(const MY_FLOAT input);

  //! Take \e vectorSize inputs and return the corresponding function values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public: // SWAP formerly protected  
  MY_FLOAT offSet;
  MY_FLOAT slope;
  MY_FLOAT lastOutput;

};

#endif




/***************************************************/
/*! \class BiQuad
    \brief STK biquad (two-pole, two-zero) filter class.

    This protected Filter subclass implements a
    two-pole, two-zero digital filter.  A method
    is provided for creating a resonance in the
    frequency response while maintaining a constant
    filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BIQUAD_H)
#define __BIQUAD_H


class BiQuad : public Filter
{
public:

  //! Default constructor creates a second-order pass-through filter.
  BiQuad();

  //! Class destructor.
  virtual ~BiQuad();

  //! Clears all internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the b[1] coefficient value.
  void setB1(MY_FLOAT b1);

  //! Set the b[2] coefficient value.
  void setB2(MY_FLOAT b2);

  //! Set the a[1] coefficient value.
  void setA1(MY_FLOAT a1);

  //! Set the a[2] coefficient value.
  void setA2(MY_FLOAT a2);

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  If \e normalize is true,
    the filter zeros are placed at z = 1, z = -1, and the coefficients
    are then normalized to produce a constant unity peak gain
    (independent of the filter \e gain parameter).  The resulting
    filter frequency response has a resonance at the given \e
    frequency.  The closer the poles are to the unit-circle (\e radius
    close to one), the narrower the resulting resonance width.
  */
  void setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize = FALSE);

  //! Set the filter coefficients for a notch at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate zeros with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  No filter normalization
    is attempted.
  */
  void setNotch(MY_FLOAT frequency, MY_FLOAT radius);

  //! Sets the filter zeroes for equal resonance gain.
  /*!
    When using the filter as a resonator, zeroes places at z = 1, z
    = -1 will result in a constant gain at resonance of 1 / (1 - R),
    where R is the pole radius setting.

  */
  void setEqualGainZeroes();

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif




/***************************************************/
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999 - 2002.
    Modified for Stk 4.0 by Gary Scavone.
*/
/***************************************************/

#if !defined(__BANDEDWG_H)
#define __BANDEDWG_H

#define MAX_BANDED_MODES 20


class BandedWG : public Instrmnt
{
 public:
  //! Class constructor.
  BandedWG();

  //! Class destructor.
  ~BandedWG();

  //! Reset and clear all internal state.
  void clear();

  //! Set strike position (0.0 - 1.0).
  void setStrikePosition(MY_FLOAT position);

  //! Select a preset.
  void setPreset(int preset);

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply bow velocity/pressure to instrument with given amplitude and rate of increase.
  void startBowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease bow velocity/breath pressure with given rate of decrease.
  void stopBowing(MY_FLOAT rate);

  //! Pluck the instrument with given amplitude.
  void pluck(MY_FLOAT amp);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT amplitude) { noteOn ( freakency, amplitude ); } 
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected

  double m_rate;
  int m_preset;
  double m_bowpressure;

  bool doPluck;
  bool trackVelocity;
  int nModes;
  int presetModes;
  BowTabl *bowTabl;
  ADSR *adsr;
  BiQuad *bandpass;
  DelayL *delay;
  MY_FLOAT maxVelocity;
  MY_FLOAT modes[MAX_BANDED_MODES];
  MY_FLOAT freakency;
  MY_FLOAT baseGain;
  MY_FLOAT gains[MAX_BANDED_MODES];
  MY_FLOAT basegains[MAX_BANDED_MODES];
  MY_FLOAT excitation[MAX_BANDED_MODES];
  MY_FLOAT integrationConstant;
  MY_FLOAT velocityInput;
  MY_FLOAT bowVelocity;
  MY_FLOAT bowTarget;
  MY_FLOAT bowPosition;
  MY_FLOAT strikeAmp;
  int strikePosition;

};

#endif




/***************************************************/
/*! \class WvIn
    \brief STK audio data input base class.

    This class provides input support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvIn loads the contents of an audio file for
    subsequent output.  Linear interpolation is
    used for fractional "read rates".

    WvIn supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which return samples produced
    by averaging across sample frames, from the 
    tickFrame() methods, which return pointers to
    multi-channel sample frames.  For single-channel
    data, these methods return equivalent values.

    Small files are completely read into local memory
    during instantiation.  Large files are read
    incrementally from disk.  The file size threshold
    and the increment size values are defined in
    WvIn.h.

    WvIn currently supports WAV, AIFF, SND (AU),
    MAT-file (Matlab), and STK RAW file formats.
    Signed integer (8-, 16-, and 32-bit) and floating-
    point (32- and 64-bit) data types are supported.
    Uncompressed data types are not supported.  If
    using MAT-files, data should be saved in an array
    with each data channel filling a matrix row.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__WVIN_H)
#define __WVIN_H

// Files larger than CHUNK_THRESHOLD will be copied into memory
// in CHUNK_SIZE increments, rather than completely loaded into
// a buffer at once.

#define CHUNK_THRESHOLD 5000000  // 5 Mb
#define CHUNK_SIZE 1024          // sample frames

#include <stdio.h>

class WvIn : public Stk
{
public:
  //! Default constructor.
  WvIn();

  //! Overloaded constructor for file input.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  WvIn( const char *fileName, bool raw = FALSE, bool doNormalize = TRUE, bool generate=true );

  //! Class destructor.
  virtual ~WvIn();

  //! Open the specified file and load its data.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  virtual void openFile( const char *fileName, bool raw = FALSE, bool doNormalize = TRUE, bool generate = true );

  //! If a file is open, close it.
  void closeFile(void);

  //! Clear outputs and reset time (file pointer) to zero.
  void reset(void);

  //! Normalize data to a maximum of +-1.0.
  /*!
    For large, incrementally loaded files with integer data types,
    normalization is computed relative to the data type maximum.
    No normalization is performed for incrementally loaded files
    with floating-point data types.
  */
  void normalize(void);

  //! Normalize data to a maximum of \e +-peak.
  /*!
    For large, incrementally loaded files with integer data types,
    normalization is computed relative to the data type maximum 
    (\e peak/maximum).  For incrementally loaded files with floating-
    point data types, direct scaling by \e peak is performed.
  */
  void normalize(MY_FLOAT peak);

  //! Return the file size in sample frames.
  unsigned long getSize(void) const;

  //! Return the number of audio channels in the file.
  unsigned int getChannels(void) const;

  //! Return the input file sample rate in Hz (not the data read rate).
  /*!
    WAV, SND, and AIF formatted files specify a sample rate in
    their headers.  STK RAW files have a sample rate of 22050 Hz
    by definition.  MAT-files are assumed to have a rate of 44100 Hz.
  */
  MY_FLOAT getFileRate(void) const;

  //! Query whether reading is complete.
  bool isFinished(void) const;

  //! Set the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  void setRate(MY_FLOAT aRate);

  //! Increment the read pointer by \e aTime samples.
  virtual void addTime(MY_FLOAT aTime);

  //! Turn linear interpolation on/off.
  /*!
    Interpolation is automatically off when the read rate is
    an integer value.  If interpolation is turned off for a
    fractional rate, the time index is truncated to an integer
    value.
  */
  void setInterpolate(bool doInterpolate);

  //! Return the average across the last output sample frame.
  virtual MY_FLOAT lastOut(void) const;

  //! Read out the average across one sample frame of data.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual MY_FLOAT tick(void);

  //! Read out vectorSize averaged sample frames of data in \e vector.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return a pointer to the last output sample frame.
  virtual const MY_FLOAT *lastFrame(void) const;

  //! Return a pointer to the next sample frame of data.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual const MY_FLOAT *tickFrame(void);

  //! Read out sample \e frames of data to \e frameVector.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual MY_FLOAT *tickFrame(MY_FLOAT *frameVector, unsigned int frames);

public: // SWAP formerly protected

  // Initialize class variables.
  void init( void );

  // Read file data.
  virtual void readData(unsigned long index);

  // Get STK RAW file information.
  bool getRawInfo( const char *fileName );

  // Get WAV file header information.
  bool getWavInfo( const char *fileName );

  // Get SND (AU) file header information.
  bool getSndInfo( const char *fileName );

  // Get AIFF file header information.
  bool getAifInfo( const char *fileName );

  // Get MAT-file header information.
  bool getMatInfo( const char *fileName );

  char msg[256];
  char m_filename[256]; // chuck data
  FILE *fd;
  MY_FLOAT *data;
  MY_FLOAT *lastOutput;
  bool chunking;
  bool finished;
  bool interpolate;
  bool byteswap;
  unsigned long fileSize;
  unsigned long bufferSize;
  unsigned long dataOffset;
  unsigned int channels;
  long chunkPointer;
  STK_FORMAT dataType;
  MY_FLOAT fileRate;
  MY_FLOAT gain;
  MY_FLOAT time;
  MY_FLOAT rate;
public:
  bool m_loaded;
};

#endif // defined(__WVIN_H)




/***************************************************/
/*! \class WaveLoop
    \brief STK waveform oscillator class.

    This class inherits from WvIn and provides
    audio file looping functionality.

    WaveLoop supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__WAVELOOP_H)
#define __WAVELOOP_H

#include <stdio.h>

class WaveLoop : public WvIn
{
public:
  WaveLoop( );
  //! Class constructor.
  WaveLoop( const char *fileName, bool raw = FALSE, bool generate = true );
  
  virtual void openFile( const char * fileName, bool raw = FALSE, bool n = TRUE );

  //! Class destructor.
  virtual ~WaveLoop();

  //! Set the data interpolation rate based on a looping frequency.
  /*!
    This function determines the interpolation rate based on the file
    size and the current Stk::sampleRate.  The \e aFrequency value
    corresponds to file cycles per second.  The frequency can be
    negative, in which case the loop is read in reverse order.
   */
  void setFrequency(MY_FLOAT aFrequency);

  //! Increment the read pointer by \e aTime samples, modulo file size.
  void addTime(MY_FLOAT aTime);

  //! Increment current read pointer by \e anAngle, relative to a looping frequency.
  /*!
    This function increments the read pointer based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
   */
  void addPhase(MY_FLOAT anAngle);

  //! Add a phase offset to the current read pointer.
  /*!
    This function determines a time offset based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
   */
  void addPhaseOffset(MY_FLOAT anAngle);

  //! Return a pointer to the next sample frame of data.
  const MY_FLOAT *tickFrame(void);

public:

  // Read file data.
  void readData(unsigned long index);
  MY_FLOAT phaseOffset;
  MY_FLOAT m_freq ; // chuck data;
};

#endif // defined(__WAVELOOP_H)




/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This protected Filter subclass implements
    a two-zero digital filter.  A method is
    provided for creating a "notch" in the
    frequency response while maintaining a
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TWOZERO_H)
#define __TWOZERO_H


class TwoZero : public Filter // formerly protected Filter
{
 public:
  //! Default constructor creates a second-order pass-through filter.
  TwoZero();

  //! Class destructor.
  ~TwoZero();

  //! Clears the internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the b[1] coefficient value.
  void setB1(MY_FLOAT b1);

  //! Set the b[2] coefficient value.
  void setB2(MY_FLOAT b2);

  //! Sets the filter coefficients for a "notch" at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate zeros with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  The coefficients are then
    normalized to produce a maximum filter gain of one (independent of
    the filter \e gain parameter).  The resulting filter frequency
    response has a "notch" or anti-resonance at the given \e
    frequency.  The closer the zeros are to the unit-circle (\e radius
    close to or equal to one), the narrower the resulting notch width.
  */
  void setNotch(MY_FLOAT frequency, MY_FLOAT radius);

  void ck_setNotchFreq ( MY_FLOAT freq ) { m_notchFreq = freq; setNotch (m_notchFreq, m_notchRad); }
  void ck_setNotchRad  ( MY_FLOAT rad  ) { m_notchRad = rad;   setNotch (m_notchFreq, m_notchRad); }


  //chuck helper functions
  MY_FLOAT m_notchFreq;
  MY_FLOAT m_notchRad;

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif




/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FM_H)
#define __FM_H


class FM : public Instrmnt
{
 public:
  //! Class constructor, taking the number of wave/envelope operators to control.
  FM( int operators = 4 );

  //! Class destructor.
  virtual ~FM();

  //! Reset and clear all wave and envelope states.
  void clear();

  //! Load the rawwave filenames in waves.
  void loadWaves(const char **filenames);

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Set the frequency ratio for the specified wave.
  void setRatio(int waveIndex, MY_FLOAT ratio);

  //! Set the gain for the specified wave.
  void setGain(int waveIndex, MY_FLOAT gain);

  //! Set the modulation speed in Hz.
  void setModulationSpeed(MY_FLOAT mSpeed);

  //! Set the modulation depth.
  void setModulationDepth(MY_FLOAT mDepth);

  //! Set the value of control1.
  void setControl1(MY_FLOAT cVal);

  //! Set the value of control1.
  void setControl2(MY_FLOAT cVal);

  //! Start envelopes toward "on" targets.
  void keyOn();

  //! Start envelopes toward "off" targets.
  void keyOff();

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Pure virtual function ... must be defined in subclasses.
  virtual MY_FLOAT tick() = 0;

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  ADSR     **adsr; 
  WaveLoop **waves;
  WaveLoop *vibrato;
  TwoZero  *twozero;
  int nOperators;
  MY_FLOAT baseFrequency;
  MY_FLOAT *ratios;
  MY_FLOAT *gains;
  MY_FLOAT modDepth;
  MY_FLOAT control1;
  MY_FLOAT control2;
  MY_FLOAT __FM_gains[100];
  MY_FLOAT __FM_susLevels[16];
  MY_FLOAT __FM_attTimes[32];

};

#endif




/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BEETHREE_H)
#define __BEETHREE_H


class BeeThree : public FM
{
 public:
  //! Class constructor.
  BeeThree();

  //! Class destructor.
  ~BeeThree();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn( MY_FLOAT amplitude) { noteOn( baseFrequency, amplitude ); }
  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/***************************************************/
/*! \class JetTabl
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__JETTABL_H)
#define __JETTABL_H


class JetTabl : public Stk
{
public:
  //! Default constructor.
  JetTabl();

  //! Class destructor.
  ~JetTabl();

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the function value for \e input.
  MY_FLOAT tick(MY_FLOAT input);

  //! Take \e vectorSize inputs and return the corresponding function values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public: // SWAP formerly protected  
  MY_FLOAT lastOutput;

};

#endif




/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This protected Filter subclass implements
    a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass
    filter with a given coefficient.  Another
    method is provided to create a DC blocking filter.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__POLEZERO_H)
#define __POLEZERO_H


class PoleZero : public Filter // formerly protected Filter
{
 public:

  //! Default constructor creates a first-order pass-through filter.
  PoleZero();

  //! Class destructor.
  ~PoleZero();

  //! Clears the internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the b[1] coefficient value.
  void setB1(MY_FLOAT b1);

  //! Set the a[1] coefficient value.
  void setA1(MY_FLOAT a1);

  //! Set the filter for allpass behavior using \e coefficient.
  /*!
    This method uses \e coefficient to create an allpass filter,
    which has unity gain at all frequencies.  Note that the \e
    coefficient magnitude must be less than one to maintain stability.
  */
  void setAllpass(MY_FLOAT coefficient);

  //! Create a DC blocking filter with the given pole position in the z-plane.
  /*!
    This method sets the given pole position, together with a zero
    at z=1, to create a DC blocking filter.  \e thePole should be
    close to one to minimize low-frequency attenuation.

  */
  void setBlockZero(MY_FLOAT thePole = 0.99);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif




/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__NOISE_H)
#define __NOISE_H


class Noise : public Stk
{
public:

  //! Default constructor which seeds the random number generator with the system time.
  Noise();

  //! Constructor which seeds the random number generator with a given seed.
  /*!
    If the seed value is zero, the random number generator is
    seeded with the system time.
  */
  Noise( unsigned int seed );

  //! Class destructor.
  virtual ~Noise();

  //! Seed the random number generator with a specific seed value.
  /*!
    If no seed is provided or the seed value is zero, the random
    number generator is seeded with the current system time.
  */
  void setSeed( unsigned int seed = 0 );

  //! Return a random number between -1.0 and 1.0 using rand().
  virtual MY_FLOAT tick();

  //! Return \e vectorSize random numbers between -1.0 and 1.0 in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last computed value.
  MY_FLOAT lastOut() const;

public: // SWAP formerly protected  

  MY_FLOAT lastOutput;

};

#endif




/***************************************************/
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BOTTLE_H)
#define __BOTTLE_H


class BlowBotl : public Instrmnt
{
 public:
  //! Class constructor.
  BlowBotl();

  //! Class destructor.
  ~BlowBotl();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply breath velocity to instrument
  void startBlowing(MY_FLOAT amplitude) { startBlowing ( amplitude, m_rate ); } //chuck

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note current frequency, given amplitude.
  void noteOn(MY_FLOAT amplitude) { noteOn ( baseFrequency, amplitude ); } //chuck

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
 
  double m_rate;
  JetTabl *jetTable;
  BiQuad *resonator;
  PoleZero *dcBlock;
  Noise *noise;
  ADSR *adsr;
  WaveLoop *vibrato;
  MY_FLOAT maxPressure;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;
  MY_FLOAT outputGain;

  MY_FLOAT baseFrequency; // chuck data
};

#endif




/***************************************************/
/*! \class ReedTabl
    \brief STK reed table class.

    This class implements a simple one breakpoint,
    non-linear reed function, as described by
    Smith (1986).  This function is based on a
    memoryless non-linear spring model of the reed
    (the reed mass is ignored) which saturates when
    the reed collides with the mouthpiece facing.

    See McIntyre, Schumacher, & Woodhouse (1983),
    Smith (1986), Hirschman, Cook, Scavone, and
    others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__REEDTABL_H)
#define __REEDTABL_H


class ReedTabl : public Stk
{
public:
  //! Default constructor.
  ReedTabl();

  //! Class destructor.
  ~ReedTabl();

  //! Set the table offset value.
  /*!
    The table offset roughly corresponds to the size
    of the initial reed tip opening (a greater offset
    represents a smaller opening).
  */
  void setOffset(MY_FLOAT aValue);

  //! Set the table slope value.
  /*!
   The table slope roughly corresponds to the reed
   stiffness (a greater slope represents a harder
   reed).
  */
  void setSlope(MY_FLOAT aValue);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the function value for \e input.
  /*!
    The function input represents the differential
    pressure across the reeds.
  */
  MY_FLOAT tick(MY_FLOAT input);

  //! Take \e vectorSize inputs and return the corresponding function values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public: // SWAP formerly protected  
  MY_FLOAT offSet;
  MY_FLOAT slope;
  MY_FLOAT lastOutput;

};

#endif




/***************************************************/
/*! \class OneZero
    \brief STK one-zero filter class.

    This protected Filter subclass implements
    a one-zero digital filter.  A method is
    provided for setting the zero position
    along the real axis of the z-plane while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ONEZERO_H)
#define __ONEZERO_H


class OneZero : public Filter // formerly protected Filter
{
 public:

  //! Default constructor creates a first-order low-pass filter.
  OneZero();

  //! Overloaded constructor which sets the zero position during instantiation.
  OneZero(MY_FLOAT theZero);

  //! Class destructor.
  ~OneZero();

  //! Clears the internal state of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the b[1] coefficient value.
  void setB1(MY_FLOAT b1);

  //! Set the zero position in the z-plane.
  /*!
    This method sets the zero position along the real-axis of the
    z-plane and normalizes the coefficients for a maximum gain of one.
    A positive zero value produces a high-pass filter, while a
    negative zero value produces a low-pass filter.  This method does
    not affect the filter \e gain value.
  */
  void setZero(MY_FLOAT theZero);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif




/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BLOWHOLE_H)
#define __BLOWHOLE_H


class BlowHole : public Instrmnt
{
 public:
  //! Class constructor.
  BlowHole(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~BlowHole();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the tonehole state (0.0 = closed, 1.0 = fully open).
  void setTonehole(MY_FLOAT newValue);

  //! Set the register hole state (0.0 = closed, 1.0 = fully open).
  void setVent(MY_FLOAT newValue);

  //! Apply breath velocity to instrument
  void startBlowing(MY_FLOAT amplitude) { startBlowing ( amplitude, m_rate ); } //chuck

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn( MY_FLOAT amplitude ) { noteOn ( m_frequency, amplitude ); }
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  

  double m_frequency;
  double m_rate;
  double m_tonehole;
  double m_vent;
  double m_reed;

  DelayL *delays[3];
  ReedTabl *reedTable;
  OneZero *filter;
  PoleZero *tonehole;
  PoleZero *vent;
  Envelope *envelope;
  Noise *noise;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT scatter;
  MY_FLOAT th_coeff;
  MY_FLOAT r_th;
  MY_FLOAT rh_coeff;
  MY_FLOAT rh_gain;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;

};

#endif




/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This protected Filter subclass implements
    a one-pole digital filter.  A method is
    provided for setting the pole position along
    the real axis of the z-plane while maintaining
    a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ONEPOLE_H)
#define __ONEPOLE_H

class OnePole : public Filter // formerly protected Filter
{
public:

  //! Default constructor creates a first-order low-pass filter.
  OnePole();

  //! Overloaded constructor which sets the pole position during instantiation.
  OnePole(MY_FLOAT thePole);

  //! Class destructor.
  ~OnePole();

  //! Clears the internal state of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the a[1] coefficient value.
  void setA1(MY_FLOAT a1);

  //! Set the pole position in the z-plane.
  /*!
    This method sets the pole position along the real-axis of the
    z-plane and normalizes the coefficients for a maximum gain of one.
    A positive pole value produces a low-pass filter, while a negative
    pole value produces a high-pass filter.  This method does not
    affect the filter \e gain value.
  */
  void setPole(MY_FLOAT thePole);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif




/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BOWED_H)
#define __BOWED_H


class Bowed : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Bowed(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Bowed();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set vibrato gain.
  void setVibrato(MY_FLOAT gain);

  void startBowing(MY_FLOAT amplitude) { startBowing(amplitude, m_rate ); }
  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBowing(MY_FLOAT rate);

  void noteOn(MY_FLOAT amplitude) { noteOn ( m_frequency, amplitude ); } 
  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  double m_frequency; 
  double m_rate; 
  DelayL *neckDelay;
  DelayL *bridgeDelay;
  BowTabl *bowTable;
  OnePole *stringFilter;
  BiQuad *bodyFilter;
  WaveLoop *vibrato;
  ADSR *adsr;
  MY_FLOAT maxVelocity;
  MY_FLOAT baseDelay;
  MY_FLOAT vibratoGain;
  MY_FLOAT betaRatio;

};

#endif




/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using a first-order
    allpass filter.  A fixed maximum length
    of 4095 and a delay of 0.5 is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    An allpass filter has unity magnitude gain but
    variable phase delay properties, making it useful
    in achieving fractional delays without affecting
    a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay
    response, the minimum delay possible in this
    implementation is limited to a value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DelayA_h)
#define __DelayA_h


class DelayA : public Delay
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  DelayA();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  
  DelayA(MY_FLOAT theDelay, long maxDelay);

  //! Class destructor.
  ~DelayA();

  //! Clears the internal state of the delay line.
  void clear();

  //! Set the delay-line length
  /*!
    The valid range for \e theDelay is from 0.5 to the maximum delay-line length.
  */
  void setDelay(MY_FLOAT theDelay);
  void set( MY_FLOAT delay, long max );

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  MY_FLOAT nextOut(void);

  //! Input one sample to the delay-line and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

public: // SWAP formerly protected  
  MY_FLOAT alpha;
  MY_FLOAT coeff;
  MY_FLOAT apInput;
  MY_FLOAT nextOutput;
  bool doNextOut;
};

#endif




/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BRASS_H)
#define __BRASS_H


class Brass: public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Brass(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Brass();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the lips frequency.
  void setLip(MY_FLOAT frequency);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude,MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  DelayA *delayLine;
  BiQuad *lipFilter;
  PoleZero *dcBlock;
  ADSR *adsr;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT lipTarget;
  MY_FLOAT slideTarget;
  MY_FLOAT vibratoGain;
  MY_FLOAT maxPressure;

};

#endif




/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__CHORUS_H)
#define __CHORUS_H


class Chorus : public Stk
{
 public:
  //! Class constructor, taking the longest desired delay length.
  Chorus(MY_FLOAT baseDelay);

  //! Class destructor.
  ~Chorus();

  //! Reset and clear all internal state.
  void clear();

  //! Set modulation depth.
  void setModDepth(MY_FLOAT depth);

  //! Set modulation frequency.
  void setModFrequency(MY_FLOAT frequency);

  //! Set the mixture of input and processed levels in the output (0.0 = input only, 1.0 = processed only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 public: // SWAP formerly protected  
  DelayL *delayLine[2];
  WaveLoop *mods[2];
  MY_FLOAT baseLength;
  MY_FLOAT modDepth;
  MY_FLOAT lastOutput[2];
  MY_FLOAT effectMix;

};

#endif





/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__CLARINET_H)
#define __CLARINET_H


class Clarinet : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Clarinet(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Clarinet();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected
  DelayL *delayLine;
  ReedTabl *reedTable;
  OneZero *filter;
  Envelope *envelope;
  Noise *noise;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;

};

#endif




/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DRUMMER_H)
#define __DRUMMER_H


#define DRUM_NUMWAVES 11
#define DRUM_POLYPHONY 4

class Drummer : public Instrmnt
{
 public:
  //! Class constructor.
  Drummer();

  //! Class destructor.
  ~Drummer();

  //! Start a note with the given drum type and amplitude.
  /*!
    Use general MIDI drum instrument numbers, converted to
    frequency values as if MIDI note numbers, to select a
    particular instrument.
  */
  void noteOn(MY_FLOAT instrument, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

 public: // SWAP formerly protected  
  WvIn    *waves[DRUM_POLYPHONY];
  OnePole *filters[DRUM_POLYPHONY];
  int      sounding[DRUM_POLYPHONY];
  int      nSounding;

};

#endif




/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements a echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ECHO_H)
#define __ECHO_H


class Echo : public Stk
{
public:
  //! Class constructor, taking the longest desired delay length.
  Echo(MY_FLOAT longestDelay);

  //! Class destructor.
  ~Echo();

  //! Reset and clear all internal state.
  void clear();

  //! Set the delay line length in samples.
  void setDelay(MY_FLOAT delay);
  void set(MY_FLOAT max);
  MY_FLOAT getDelay();

  //! Set the mixture of input and processed levels in the output (0.0 = input only, 1.0 = processed only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public:
  Delay *delayLine;
  long length;
  MY_FLOAT lastOutput;
  MY_FLOAT effectMix;

};

#endif





/***************************************************/
/*! \class FMVoices
    \brief STK singing FM synthesis instrument.

    This class implements 3 carriers and a common
    modulator, also referred to as algorithm 6 of
    the TX81Z.

    \code
    Algorithm 6 is :
                        /->1 -\
                     4-|-->2 - +-> Out
                        \->3 -/
    \endcode

    Control Change Numbers: 
       - Vowel = 2
       - Spectral Tilt = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FMVOICES_H)
#define __FMVOICES_H


class FMVoices : public FM
{
 public:
  //! Class constructor.
  FMVoices();

  //! Class destructor.
  ~FMVoices();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn( MY_FLOAT amplitude) { noteOn(baseFrequency, amplitude); }

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected
  int currentVowel;
  MY_FLOAT tilt[3];
  MY_FLOAT mods[3];
};

#endif




/***************************************************/
/*! \class Flute
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FLUTE_H)
#define __FLUTE_H


class Flute : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Flute(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Flute();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the reflection coefficient for the jet delay (-1.0 - 1.0).
  void setJetReflection(MY_FLOAT coefficient);

  //! Set the reflection coefficient for the air column delay (-1.0 - 1.0).
  void setEndReflection(MY_FLOAT coefficient);

  //! Set the length of the jet delay in terms of a ratio of jet delay to air column delay lengths.
  void setJetDelay(MY_FLOAT aRatio);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  DelayL *jetDelay;
  DelayL *boreDelay;
  JetTabl *jetTable;
  OnePole *filter;
  PoleZero *dcBlock;
  Noise *noise;
  ADSR *adsr;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT lastFrequency;
  MY_FLOAT maxPressure;
  MY_FLOAT jetReflection;
  MY_FLOAT endReflection;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;
  MY_FLOAT outputGain;
  MY_FLOAT jetRatio;

};

#endif




/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FORMSWEP_H)
#define __FORMSWEP_H


class FormSwep : public BiQuad
{
 public:

  //! Default constructor creates a second-order pass-through filter.
  FormSwep();

  //! Class destructor.
  ~FormSwep();

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  The filter zeros are
    placed at z = 1, z = -1, and the coefficients are then normalized to
    produce a constant unity gain (independent of the filter \e gain
    parameter).  The resulting filter frequency response has a
    resonance at the given \e frequency.  The closer the poles are to
    the unit-circle (\e radius close to one), the narrower the
    resulting resonance width.
  */
  void setResonance(MY_FLOAT aFrequency, MY_FLOAT aRadius);

  //! Set both the current and target resonance parameters.
  void setStates(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain = 1.0);

  //! Set target resonance parameters.
  void setTargets(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain = 1.0);

  //! Set the sweep rate (between 0.0 - 1.0).
  /*!
    The formant parameters are varied in increments of the
    sweep rate between their current and target values.
    A sweep rate of 1.0 will produce an immediate change in
    resonance parameters from their current values to the
    target values.  A sweep rate of 0.0 will produce no
    change in resonance parameters.  
  */
  void setSweepRate(MY_FLOAT aRate);    

  //! Set the sweep rate in terms of a time value in seconds.
  /*!
    This method adjusts the sweep rate based on a
    given time for the formant parameters to reach
    their target values.
  */
  void setSweepTime(MY_FLOAT aTime);    

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 public: // SWAP formerly protected  
  bool dirty;
  MY_FLOAT frequency;
  MY_FLOAT radius;
  MY_FLOAT startFrequency;
  MY_FLOAT startRadius;
  MY_FLOAT startGain;
  MY_FLOAT targetFrequency;
  MY_FLOAT targetRadius;
  MY_FLOAT targetGain;
  MY_FLOAT deltaFrequency;
  MY_FLOAT deltaRadius;
  MY_FLOAT deltaGain;
  MY_FLOAT sweepState;
  MY_FLOAT sweepRate;

};

#endif




/***************************************************/
/*! \class HevyMetl
    \brief STK heavy metal FM synthesis instrument.

    This class implements 3 cascade operators with
    feedback modulation, also referred to as
    algorithm 3 of the TX81Z.

    \code
    Algorithm 3 is :    4--\
                    3-->2-- + -->1-->Out
    \endcode

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__HEVYMETL_H)
#define __HEVYMETL_H


class HevyMetl : public FM
{
 public:
  //! Class constructor.
  HevyMetl();

  //! Class destructor.
  ~HevyMetl();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn( MY_FLOAT amplitude) { noteOn(baseFrequency, amplitude); }

  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/***************************************************/
/*! \class Reverb
    \brief STK abstract reverberator parent class.

    This class provides common functionality for
    STK reverberator subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


#if !defined(__REVERB_H)
#define __REVERB_H

class Reverb : public Stk
{
 public:
  //! Class constructor.
  Reverb();

  //! Class destructor.
  virtual ~Reverb();

  //! Reset and clear all internal state.
  virtual void clear() = 0;

  //! Set the mixture of input and "reverberated" levels in the output (0.0 = input only, 1.0 = reverb only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

  //! Abstract tick function ... must be implemented in subclasses.
  virtual MY_FLOAT tick(MY_FLOAT input) = 0;

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 public: // SWAP formerly protected

  // Returns true if argument value is prime.
  bool isPrime(int number);

  MY_FLOAT lastOutput[2];
  MY_FLOAT effectMix;

};

#endif // defined(__REVERB_H)





/***************************************************/
/*! \class JCRev
    \brief John Chowning's reverberator class.

    This class is derived from the CLM JCRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This class implements three series
    allpass units, followed by four parallel comb
    filters, and two decorrelation delay lines in
    parallel at the output.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__JCREV_H)
#define __JCREV_H


class JCRev : public Reverb
{
 public:
  //! Class constructor taking a T60 decay time argument.
  JCRev(MY_FLOAT T60);

  //! Class destructor.
  ~JCRev();

  //! Reset and clear all internal state.
  void clear();

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

 public: // SWAP formerly protected
  Delay *allpassDelays[3];
  Delay *combDelays[4];
  Delay *outLeftDelay;
  Delay *outRightDelay;
  MY_FLOAT allpassCoefficient;
  MY_FLOAT combCoefficient[4];

};

#endif





/***************************************************/
/*! \class PluckTwo
    \brief STK enhanced plucked string model class.

    This class implements an enhanced two-string,
    plucked physical model, a la Jaffe-Smith,
    Smith, and others.

    PluckTwo is an abstract class, with no excitation
    specified.  Therefore, it can't be directly
    instantiated.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PLUCKTWO_H)
#define __PLUCKTWO_H


class PluckTwo : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  PluckTwo(MY_FLOAT lowestFrequency);

  //! Class destructor.
  virtual ~PluckTwo();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Detune the two strings by the given factor.  A value of 1.0 produces unison strings.
  void setDetune(MY_FLOAT detune);

  //! Efficient combined setting of frequency and detuning.
  void setFreqAndDetune(MY_FLOAT frequency, MY_FLOAT detune);

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPluckPosition(MY_FLOAT position);

  //! Set the base loop gain.
  /*!
    The actual loop gain is set according to the frequency.
    Because of high-frequency loop filter roll-off, higher
    frequency settings have greater loop gains.
  */
  void setBaseLoopGain(MY_FLOAT aGain);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Virtual (abstract) tick function is implemented by subclasses.
  virtual MY_FLOAT tick() = 0;

  public: // SWAP formerly protected  
    DelayA *delayLine;
    DelayA *delayLine2;
    DelayL *combDelay;
    OneZero *filter;
    OneZero *filter2;
    long length;
    MY_FLOAT loopGain;
    MY_FLOAT baseLoopGain;
    MY_FLOAT lastFrequency;
    MY_FLOAT lastLength;
    MY_FLOAT detuning;
    MY_FLOAT pluckAmplitude;
    MY_FLOAT pluckPosition;

};

#endif




/***************************************************/
/*! \class Mandolin
    \brief STK mandolin instrument model class.

    This class inherits from PluckTwo and uses
    "commuted synthesis" techniques to model a
    mandolin instrument.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    Commuted Synthesis, in particular, is covered
    by patents, granted, pending, and/or
    applied-for.  All are assigned to the Board of
    Trustees, Stanford University.  For
    information, contact the Office of Technology
    Licensing, Stanford University.

    Control Change Numbers: 
       - Body Size = 2
       - Pluck Position = 4
       - String Sustain = 11
       - String Detuning = 1
       - Microphone Position = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MANDOLIN_H)
#define __MANDOLIN_H


class Mandolin : public PluckTwo
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Mandolin(MY_FLOAT lowestFrequency);

  //! Class destructor.
  virtual ~Mandolin();

  //! Pluck the strings with the given amplitude (0.0 - 1.0) using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Pluck the strings with the given amplitude (0.0 - 1.0) and position (0.0 - 1.0).
  void pluck(MY_FLOAT amplitude,MY_FLOAT position);

  //! Start a note with the given frequency and amplitude (0.0 - 1.0).
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Set the body size (a value of 1.0 produces the "default" size).
  void setBodySize(MY_FLOAT size);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

  public: // SWAP formerly protected  
    WvIn *soundfile[12];
    MY_FLOAT directBody;
    int mic;
    long dampTime;
    bool waveDone;
  MY_FLOAT m_bodySize;
  MY_FLOAT m_stringDamping;
  MY_FLOAT m_stringDetune;
};

#endif




/***************************************************/
/*! \class Mesh2D
    \brief Two-dimensional rectilinear waveguide mesh class.

    This class implements a rectilinear,
    two-dimensional digital waveguide mesh
    structure.  For details, see Van Duyne and
    Smith, "Physical Modeling with the 2-D Digital
    Waveguide Mesh", Proceedings of the 1993
    International Computer Music Conference.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - X Dimension = 2
       - Y Dimension = 4
       - Mesh Decay = 11
       - X-Y Input Position = 1

    by Julius Smith, 2000 - 2002.
    Revised by Gary Scavone for STK, 2002.
*/
/***************************************************/

#if !defined(__MESH2D_H)
#define __MESH2D_H


#define NXMAX ((short)(12))
#define NYMAX ((short)(12))

class Mesh2D : public Instrmnt
{
 public:
  //! Class constructor, taking the x and y dimensions in samples.
  Mesh2D(short nX, short nY);

  //! Class destructor.
  ~Mesh2D();

  //! Reset and clear all internal state.
  void clear(); 

  //! Set the x dimension size in samples.
  void setNX(short lenX);

  //! Set the y dimension size in samples.
  void setNY(short lenY);

  //! Set the x, y input position on a 0.0 - 1.0 scale.
  void setInputPosition(MY_FLOAT xFactor, MY_FLOAT yFactor);

  //! Set the loss filters gains (0.0 - 1.0).
  void setDecay(MY_FLOAT decayFactor);

  //! Impulse the mesh with the given amplitude (frequency ignored).
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay) ... currently ignored.
  void noteOff(MY_FLOAT amplitude);

  //! Calculate and return the signal energy stored in the mesh.
  MY_FLOAT energy();

  //! Compute one output sample, without adding energy to the mesh.
  MY_FLOAT tick();

  //! Input a sample to the mesh and compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected

  MY_FLOAT tick0();
  MY_FLOAT tick1();
  void clearMesh();

  short NX, NY;
  short xInput, yInput;
  OnePole *filterX[NXMAX];
  OnePole *filterY[NYMAX];
  MY_FLOAT v[NXMAX-1][NYMAX-1]; // junction velocities
  MY_FLOAT vxp[NXMAX][NYMAX]; // positive-x velocity wave
  MY_FLOAT vxm[NXMAX][NYMAX]; // negative-x velocity wave
  MY_FLOAT vyp[NXMAX][NYMAX]; // positive-y velocity wave
  MY_FLOAT vym[NXMAX][NYMAX]; // negative-y velocity wave

  // Alternate buffers
  MY_FLOAT vxp1[NXMAX][NYMAX]; // positive-x velocity wave
  MY_FLOAT vxm1[NXMAX][NYMAX]; // negative-x velocity wave
  MY_FLOAT vyp1[NXMAX][NYMAX]; // positive-y velocity wave
  MY_FLOAT vym1[NXMAX][NYMAX]; // negative-y velocity wave

  int counter; // time in samples


};

#endif




/***************************************************/
/*! \class Modal
    \brief STK resonance model instrument.

    This class contains an excitation wavetable,
    an envelope, an oscillator, and N resonances
    (non-sweeping BiQuad filters), where N is set
    during instantiation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MODAL_H)
#define __MODAL_H


class Modal : public Instrmnt
{
public:
  //! Class constructor, taking the desired number of modes to create.
  Modal( int modes = 4 );

  //! Class destructor.
  virtual ~Modal();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Set the ratio and radius for a specified mode filter.
  void setRatioAndRadius(int modeIndex, MY_FLOAT ratio, MY_FLOAT radius);

  //! Set the master gain.
  void setMasterGain(MY_FLOAT aGain);

  //! Set the direct gain.
  void setDirectGain(MY_FLOAT aGain);

  //! Set the gain for a specified mode filter.
  void setModeGain(int modeIndex, MY_FLOAT gain);

  //! Initiate a strike with the given amplitude (0.0 - 1.0).
  virtual void strike(MY_FLOAT amplitude);

  //! Damp modes with a given decay factor (0.0 - 1.0).
  void damp(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value) = 0;

public: // SWAP formerly protected  
  Envelope *envelope; 
  WvIn     *wave;
  BiQuad   **filters;
  OnePole  *onepole;
  WaveLoop *vibrato;
  int nModes;
  MY_FLOAT vibratoGain;
  MY_FLOAT masterGain;
  MY_FLOAT directGain;
  MY_FLOAT stickHardness;
  MY_FLOAT strikePosition;
  MY_FLOAT baseFrequency;
  MY_FLOAT *ratios;
  MY_FLOAT *radii;

};

#endif




/***************************************************/
/*! \class ModalBar
    \brief STK resonant bar instrument class.

    This class implements a number of different
    struck bar instruments.  It inherits from the
    Modal class.

    Control Change Numbers: 
       - Stick Hardness = 2
       - Stick Position = 4
       - Vibrato Gain = 11
       - Vibrato Frequency = 7
       - Volume = 128
       - Modal Presets = 16
         - Marimba = 0
         - Vibraphone = 1
         - Agogo = 2
         - Wood1 = 3
         - Reso = 4
         - Wood2 = 5
         - Beats = 6
         - Two Fixed = 7
         - Clump = 8

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MODALBAR_H)
#define __MODALBAR_H


class ModalBar : public Modal
{
public:
  //! Class constructor.
  ModalBar();

  //! Class destructor.
  ~ModalBar();

  //! Set stick hardness (0.0 - 1.0).
  void setStickHardness(MY_FLOAT hardness);

  //! Set stick position (0.0 - 1.0).
  void setStrikePosition(MY_FLOAT position);

  //! Select a bar preset (currently modulo 9).
  void setPreset(int preset);

  //! Set the modulation (vibrato) depth.
  void setModulationDepth(MY_FLOAT mDepth);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);
};

#endif




/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SUBNOISE_H)
#define __SUBNOISE_H


class SubNoise : public Noise
{
 public:

  //! Default constructor sets sub-sample rate to 16.
  SubNoise(int subRate = 16);

  //! Class destructor.
  ~SubNoise();

  //! Return the current sub-sampling rate.
  int subRate(void) const;

  //! Set the sub-sampling rate.
  void setRate(int subRate);

  //! Return a sub-sampled random number between -1.0 and 1.0.
  MY_FLOAT tick();

 public: // SWAP formerly protected  
  int counter;
  int rate;

};

#endif




/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MODULATE_H)
#define __MODULATE_H


class Modulate : public Stk
{
 public:
  //! Class constructor.
  Modulate();

  //! Class destructor.
  ~Modulate();

  //! Reset internal state.
  void reset();

  //! Set the periodic (vibrato) rate or frequency in Hz.
  void setVibratoRate(MY_FLOAT aRate);

  //! Set the periodic (vibrato) gain.
  void setVibratoGain(MY_FLOAT aGain);

  //! Set the random modulation gain.
  void setRandomGain(MY_FLOAT aGain);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Return \e vectorSize outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last computed output value.
  MY_FLOAT lastOut() const;

 public: // SWAP formerly protected
  WaveLoop *vibrato;
  SubNoise *noise;
  OnePole  *filter;
  MY_FLOAT vibratoGain;
  MY_FLOAT randomGain;
  MY_FLOAT lastOutput;

};

#endif




/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument contains up to 5 attack waves,
    5 looped waves, and an ADSR envelope.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SAMPLER_H)
#define __SAMPLER_H


class Sampler : public Instrmnt
{
 public:
  //! Default constructor.
  Sampler();

  //! Class destructor.
  virtual ~Sampler();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency) = 0;

  //! Initiate the envelopes with a key-on event and reset the attack waves.
  void keyOn();

  //! Signal a key-off event to the envelopes.
  void keyOff();

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value) = 0;

 public: // SWAP formerly protected  
  ADSR     *adsr; 
  WvIn     *attacks[5];
  WaveLoop *loops[5];
  OnePole  *filter;
  MY_FLOAT baseFrequency;
  MY_FLOAT attackRatios[5];
  MY_FLOAT loopRatios[5];
  MY_FLOAT attackGain;
  MY_FLOAT loopGain;
  int whichOne;

};

#endif




/***************************************************/
/*! \class Moog
    \brief STK moog-like swept filter sampling synthesis class.

    This instrument uses one attack wave, one
    looped wave, and an ADSR envelope (inherited
    from the Sampler class) and adds two sweepable
    formant (FormSwep) filters.

    Control Change Numbers: 
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MOOG_H)
#define __MOOG_H


class Moog : public Sampler
{
 public:
  //! Class constructor.
  Moog();

  //! Class destructor.
  ~Moog();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  virtual void noteOn(MY_FLOAT amplitude ); //CHUCK!  note start note with current frequency

  //! Set the modulation (vibrato) speed in Hz.
  void setModulationSpeed(MY_FLOAT mSpeed);

  //! Set the modulation (vibrato) depth.
  void setModulationDepth(MY_FLOAT mDepth);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected
  FormSwep *filters[2];
  MY_FLOAT modDepth;
  MY_FLOAT filterQ;
  MY_FLOAT filterRate;

};

#endif




/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class is derived from the CLM NRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This particular arrangement consists
    of 6 comb filters in parallel, followed by 3
    allpass filters, a lowpass filter, and another
    allpass in series, followed by two allpass
    filters in parallel with corresponding right
    and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__NREV_H)
#define __NREV_H


class NRev : public Reverb
{
 public:
  //! Class constructor taking a T60 decay time argument.
  NRev(MY_FLOAT T60);

  //! Class destructor.
  ~NRev();

  //! Reset and clear all internal state.
  void clear();

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

 public: // SWAP formerly protected  
  Delay *allpassDelays[8];
  Delay *combDelays[6];
  MY_FLOAT allpassCoefficient;
  MY_FLOAT combCoefficient[6];
	MY_FLOAT lowpassState;

};

#endif





/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class is based on some of the famous
    Stanford/CCRMA reverbs (NRev, KipRev), which
    were based on the Chowning/Moorer/Schroeder
    reverberators using networks of simple allpass
    and comb delay filters.  This class implements
    two series allpass units and two parallel comb
    filters.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PRCREV_H)
#define __PRCREV_H


class PRCRev : public Reverb
{
public:
  //! Class constructor taking a T60 decay time argument.
  PRCRev(MY_FLOAT T60);

  //! Class destructor.
  ~PRCRev();

  //! Reset and clear all internal state.
  void clear();

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

public: // SWAP formerly protected  
  Delay *allpassDelays[2];
  Delay *combDelays[2];
  MY_FLOAT allpassCoefficient;
  MY_FLOAT combCoefficient[2];

};

#endif





/***************************************************/
/*! \class PercFlut
    \brief STK percussive flute FM synthesis instrument.

    This class implements algorithm 4 of the TX81Z.

    \code
    Algorithm 4 is :   4->3--\
                          2-- + -->1-->Out
    \endcode

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PERCFLUT_H)
#define __PERCFLUT_H


class PercFlut : public FM
{
 public:
  //! Class constructor.
  PercFlut();

  //! Class destructor.
  ~PercFlut();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn( MY_FLOAT amplitude) { noteOn(baseFrequency, amplitude); }

  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/***************************************************/
/*! \class Phonemes
    \brief STK phonemes table.

    This class does nothing other than declare a
    set of 32 static phoneme formant parameters
    and provide access to those values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PHONEMES_H)
#define __PHONEMES_H


class Phonemes
{
public:

  Phonemes(void);
  ~Phonemes(void);

  //! Returns the phoneme name for the given index (0-31).
  static const char *name( unsigned int index );

  //! Returns the voiced component gain for the given phoneme index (0-31).
  static MY_FLOAT voiceGain( unsigned int index );

  //! Returns the unvoiced component gain for the given phoneme index (0-31).
  static MY_FLOAT noiseGain( unsigned int index );

  //! Returns the formant frequency for the given phoneme index (0-31) and partial (0-3).
  static MY_FLOAT formantFrequency( unsigned int index, unsigned int partial );

  //! Returns the formant radius for the given phoneme index (0-31) and partial (0-3).
  static MY_FLOAT formantRadius( unsigned int index, unsigned int partial );

  //! Returns the formant gain for the given phoneme index (0-31) and partial (0-3).
  static MY_FLOAT formantGain( unsigned int index, unsigned int partial );

public: // SWAP formerly private

  static const char phonemeNames[][4];
  static const MY_FLOAT phonemeGains[][2];
  static const MY_FLOAT phonemeParameters[][4][3];

};

#endif




/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PITSHIFT_H)
#define __PITSHIFT_H


class PitShift : public Stk
{
 public:
  //! Class constructor.
  PitShift();

  //! Class destructor.
  ~PitShift();

  //! Reset and clear all internal state.
  void clear();

  //! Set the pitch shift factor (1.0 produces no shift).
  void setShift(MY_FLOAT shift);

  //! Set the mixture of input and processed levels in the output (0.0 = input only, 1.0 = processed only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 public: // SWAP formerly protected  
  DelayL *delayLine[2];
  MY_FLOAT lastOutput;
  MY_FLOAT delay[2];
  MY_FLOAT env[2];
  MY_FLOAT effectMix;
  MY_FLOAT rate;

};

#endif





/***************************************************/
/*! \class Plucked
    \brief STK plucked string model class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PLUCKED_H)
#define __PLUCKED_H


class Plucked : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Plucked(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Plucked();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

 public: // SWAP formerly protected  
  DelayA *delayLine;
  OneZero *loopFilter;
  OnePole *pickFilter;
  Noise *noise;
  long length;
  MY_FLOAT loopGain;

};

#endif





/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__RESONATE_H)
#define __RESONATE_H


class Resonate : public Instrmnt
{
 public:
  //! Class constructor.
  Resonate();

  //! Class destructor.
  ~Resonate();

  //! Reset and clear all internal state.
  void clear();

  //! Set the filter for a resonance at the given frequency (Hz) and radius.
  void setResonance(MY_FLOAT frequency, MY_FLOAT radius);

  //! Set the filter for a notch at the given frequency (Hz) and radius.
  void setNotch(MY_FLOAT frequency, MY_FLOAT radius);

  //! Set the filter zero coefficients for contant resonance gain.
  void setEqualGainZeroes();

  //! Initiate the envelope with a key-on event.
  void keyOn();

  //! Signal a key-off event to the envelope.
  void keyOff();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  ADSR     *adsr;
  BiQuad   *filter;
  Noise    *noise;
  MY_FLOAT poleFrequency;
  MY_FLOAT poleRadius;
  MY_FLOAT zeroFrequency;
  MY_FLOAT zeroRadius;

};

#endif




/***************************************************/
/*! \class Rhodey
    \brief STK Fender Rhodes electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__RHODEY_H)
#define __RHODEY_H


class Rhodey : public FM
{
 public:
  //! Class constructor.
  Rhodey();

  //! Class destructor.
  ~Rhodey();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn(MY_FLOAT amplitude) { noteOn(baseFrequency * 0.5, amplitude ); } 

  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/***************************************************/
/*! \class SKINI
    \brief STK SKINI parsing class

    This class parses SKINI formatted text
    messages.  It can be used to parse individual
    messages or it can be passed an entire file.
    The file specification is Perry's and his
    alone, but it's all text so it shouldn't be to
    hard to figure out.

    SKINI (Synthesis toolKit Instrument Network
    Interface) is like MIDI, but allows for
    floating-point control changes, note numbers,
    etc.  The following example causes a sharp
    middle C to be played with a velocity of 111.132:

    \code
    noteOn  60.01  111.13
    \endcode

    \sa \ref skini

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SKINI_H)
#define __SKINI_H

#include <stdio.h>

class SKINI : public Stk
{
 public:
  //! Default constructor used for parsing messages received externally.
  SKINI();

  //! Overloaded constructor taking a SKINI formatted scorefile.
  SKINI(char *fileName);

  //! Class destructor
  ~SKINI();

  //! Attempt to parse the given string, returning the message type.
  /*!
    A type value equal to zero indicates an invalid message.
  */
  long parseThis(char* aString);

  //! Parse the next message (if a file is loaded) and return the message type.
  /*!
    A negative value is returned when the file end is reached.
  */
  long nextMessage();

  //! Return the current message type.
  long getType() const;

  //! Return the current message channel value.
  long getChannel() const;

  //! Return the current message delta time value (in seconds).
  MY_FLOAT getDelta() const;

  //! Return the current message byte two value.
  MY_FLOAT getByteTwo() const;

  //! Return the current message byte three value.
  MY_FLOAT getByteThree() const;

  //! Return the current message byte two value (integer).
  long getByteTwoInt() const;

  //! Return the current message byte three value (integer).
  long getByteThreeInt() const;

  //! Return remainder string after parsing.
  const char* getRemainderString();

  //! Return the message type as a string.
  const char* getMessageTypeString();

  //! Return the SKINI type string for the given type value.
  const char* whatsThisType(long type);

  //! Return the SKINI controller string for the given controller number.
  const char* whatsThisController(long number);

 public: // SWAP formerly protected

  FILE *myFile;
  long messageType;
  char msgTypeString[64];
  long channel;
  MY_FLOAT deltaTime;
  MY_FLOAT byteTwo;
  MY_FLOAT byteThree;
  long byteTwoInt;
  long byteThreeInt;
  char remainderString[1024];
  char whatString[1024];
};

static const double Midi2Pitch[129] = {
8.18,8.66,9.18,9.72,10.30,10.91,11.56,12.25,
12.98,13.75,14.57,15.43,16.35,17.32,18.35,19.45,
20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87,
32.70,34.65,36.71,38.89,41.20,43.65,46.25,49.00,
51.91,55.00,58.27,61.74,65.41,69.30,73.42,77.78,
82.41,87.31,92.50,98.00,103.83,110.00,116.54,123.47,
130.81,138.59,146.83,155.56,164.81,174.61,185.00,196.00,
207.65,220.00,233.08,246.94,261.63,277.18,293.66,311.13,
329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88,
523.25,554.37,587.33,622.25,659.26,698.46,739.99,783.99,
830.61,880.00,932.33,987.77,1046.50,1108.73,1174.66,1244.51,
1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53,
2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,
3322.44,3520.00,3729.31,3951.07,4186.01,4434.92,4698.64,4978.03,
5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13,
8372.02,8869.84,9397.27,9956.06,10548.08,11175.30,11839.82,12543.85,
13289.75};

#endif






/***************************************************/
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SAXOFONY_H)
#define __SAXOFONY_H


class Saxofony : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Saxofony(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Saxofony();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the "blowing" position between the air column terminations (0.0 - 1.0).
  void setBlowPosition(MY_FLOAT aPosition);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected
  DelayL *delays[2];
  ReedTabl *reedTable;
  OneZero *filter;
  Envelope *envelope;
  Noise *noise;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;
  MY_FLOAT position;

};

#endif




/***************************************************/
/*! \class Shakers
    \brief PhISEM and PhOLIES class.

    PhISEM (Physically Informed Stochastic Event
    Modeling) is an algorithmic approach for
    simulating collisions of multiple independent
    sound producing objects.  This class is a
    meta-model that can simulate a Maraca, Sekere,
    Cabasa, Bamboo Wind Chimes, Water Drops,
    Tambourine, Sleighbells, and a Guiro.

    PhOLIES (Physically-Oriented Library of
    Imitated Environmental Sounds) is a similar
    approach for the synthesis of environmental
    sounds.  This class implements simulations of
    breaking sticks, crunchy snow (or not), a
    wrench, sandpaper, and more.

    Control Change Numbers: 
      - Shake Energy = 2
      - System Decay = 4
      - Number Of Objects = 11
      - Resonance Frequency = 1
      - Shake Energy = 128
      - Instrument Selection = 1071
        - Maraca = 0
        - Cabasa = 1
        - Sekere = 2
        - Guiro = 3
        - Water Drops = 4
        - Bamboo Chimes = 5
        - Tambourine = 6
        - Sleigh Bells = 7
        - Sticks = 8
        - Crunch = 9
        - Wrench = 10
        - Sand Paper = 11
        - Coke Can = 12
        - Next Mug = 13
        - Penny + Mug = 14
        - Nickle + Mug = 15
        - Dime + Mug = 16
        - Quarter + Mug = 17
        - Franc + Mug = 18
        - Peso + Mug = 19
        - Big Rocks = 20
        - Little Rocks = 21
        - Tuned Bamboo Chimes = 22

    by Perry R. Cook, 1996 - 1999.
*/
/***************************************************/

#if !defined(__SHAKERS_H)
#define __SHAKERS_H


#define MAX_FREQS 8
#define NUM_INSTR 24

class Shakers : public Instrmnt
{
 public:
  //! Class constructor.
  Shakers();

  //! Class destructor.
  ~Shakers();

  //! Start a note with the given instrument and amplitude.
  /*!
    Use the instrument numbers above, converted to frequency values
    as if MIDI note numbers, to select a particular instrument.
  */
  virtual void ck_noteOn(MY_FLOAT amplitude ); // chuck single arg call 
  virtual void noteOn(MY_FLOAT instrument, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

  int m_noteNum; // chuck data
 MY_FLOAT freq;
 int setupName(char* instr);
 int setupNum(int inst);

 public: // SWAP formerly protected

  int setFreqAndReson(int which, MY_FLOAT freq, MY_FLOAT reson);
  void setDecays(MY_FLOAT sndDecay, MY_FLOAT sysDecay);
  void setFinalZs(MY_FLOAT z0, MY_FLOAT z1, MY_FLOAT z2);
  MY_FLOAT wuter_tick();
  MY_FLOAT tbamb_tick();
  MY_FLOAT ratchet_tick();

  int instType;
  int ratchetPos, lastRatchetPos;
  MY_FLOAT shakeEnergy;
  MY_FLOAT inputs[MAX_FREQS];
  MY_FLOAT outputs[MAX_FREQS][2];
  MY_FLOAT coeffs[MAX_FREQS][2];
  MY_FLOAT sndLevel;
  MY_FLOAT baseGain;
  MY_FLOAT gains[MAX_FREQS];
  int nFreqs;
  MY_FLOAT t_center_freqs[MAX_FREQS];
  MY_FLOAT center_freqs[MAX_FREQS];
  MY_FLOAT resons[MAX_FREQS];
  MY_FLOAT freq_rand[MAX_FREQS];
  int freqalloc[MAX_FREQS];
  MY_FLOAT soundDecay;
  MY_FLOAT systemDecay;
  MY_FLOAT nObjects;
  MY_FLOAT collLikely;
  MY_FLOAT totalEnergy;
  MY_FLOAT ratchet,ratchetDelta;
  MY_FLOAT finalZ[3];
  MY_FLOAT finalZCoeffs[3];
  MY_FLOAT defObjs[NUM_INSTR];
  MY_FLOAT defDecays[NUM_INSTR];
  MY_FLOAT decayScale[NUM_INSTR];

};

#endif




/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SIMPLE_H)
#define __SIMPLE_H


class Simple : public Instrmnt
{
 public:
  //! Class constructor.
  Simple();

  //! Class destructor.
  virtual ~Simple();

  //! Clear internal states.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Start envelope toward "on" target.
  void keyOn();

  //! Start envelope toward "off" target.
  void keyOff();

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  ADSR     *adsr; 
  WaveLoop  *loop;
  OnePole  *filter;
  BiQuad   *biquad;
  Noise    *noise;
  MY_FLOAT baseFrequency;
  MY_FLOAT loopGain;

};

#endif




/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SINGWAVE_H)
#define __SINGWAVE_H


class SingWave : public Stk
{
 public:
  //! Class constructor taking filename argument.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  SingWave(const char *fileName, bool raw=FALSE);

  //! Class destructor.
  ~SingWave();

  //! Reset file to beginning.
  void reset();

  //! Normalize the file to a maximum of +-1.0.
  void normalize();

  //! Normalize the file to a maximum of \e +- peak.
  void normalize(MY_FLOAT peak);

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the vibrato frequency in Hz.
  void setVibratoRate(MY_FLOAT aRate);

  //! Set the vibrato gain.
  void setVibratoGain(MY_FLOAT gain);

  //! Set the random-ness amount.
  void setRandomGain(MY_FLOAT gain);

  //! Set the sweep rate.
  void setSweepRate(MY_FLOAT aRate);

  //! Set the gain rate.
  void setGainRate(MY_FLOAT aRate);    

  //! Set the gain target value.
  void setGainTarget(MY_FLOAT target);

  //! Start a note.
  void noteOn();

  //! Stop a note.
  void noteOff();

  //! Return the last output value.
  MY_FLOAT lastOut();

  //! Compute one output sample.
  MY_FLOAT tick();

 public: // SWAP formerly protected

  WaveLoop *wave;
  Modulate *modulator;
  Envelope *envelope;
  Envelope *pitchEnvelope;
  MY_FLOAT rate;
  MY_FLOAT sweepRate;
  MY_FLOAT lastOutput;
  MY_FLOAT m_freq; //chuck data

};

#endif




/***************************************************/
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SITAR_H)
#define __SITAR_H


class Sitar : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Sitar(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Sitar();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

 public: // SWAP formerly protected  
  DelayA *delayLine;
  OneZero *loopFilter;
  Noise *noise;
  ADSR *envelope;
  long length;
  MY_FLOAT loopGain;
  MY_FLOAT amGain;
  MY_FLOAT delay;
  MY_FLOAT targetDelay;

};

#endif





/***************************************************/
/*! \class Socket
    \brief STK TCP socket client/server class.

    This class provides a uniform cross-platform
    TCP socket client or socket server interface.
    Methods are provided for reading or writing
    data buffers to/from connections.  This class
    also provides a number of static functions for
    use with external socket descriptors.

    The user is responsible for checking the values
    returned by the read/write methods.  Values
    less than or equal to zero indicate a closed
    or lost connection or the occurence of an error.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SOCKET_H)
#define __SOCKET_H


class Socket : public Stk
{
 public:
  //! Default constructor which creates a local socket server on port 2006 (or the specified port number).
  /*!
    An StkError will be thrown if a socket error occurs during instantiation.
  */
  Socket( int port = 2006 );

  //! Class constructor which creates a socket client connection to the specified host and port.
  /*!
    An StkError will be thrown if a socket error occurs during instantiation.
  */
  Socket( int port, const char *hostname );

  //! The class destructor closes the socket instance, breaking any existing connections.
  ~Socket();

  //! Connect a socket client to the specified host and port and returns the resulting socket descriptor.
  /*!
    This method is valid for socket clients only.  If it is called for
    a socket server, -1 is returned.  If the socket client is already
    connected, that connection is terminated and a new connection is
    attempted.  Server connections are made using the accept() method.
    An StkError will be thrown if a socket error occurs during
    instantiation. \sa accept
  */
  int connect( int port, const char *hostname = "localhost" );

  //! Close this socket.
  void close( void );

  //! Return the server/client socket descriptor.
  int socket( void ) const;

  //! Return the server/client port number.
  int port( void ) const;

  //! If this is a socket server, extract the first pending connection request from the queue and create a new connection, returning the descriptor for the accepted socket.
  /*!
    If no connection requests are pending and the socket has not
    been set non-blocking, this function will block until a connection
    is present.  If an error occurs or this is a socket client, -1 is
    returned.
  */
  int accept( void );

  //! If enable = false, the socket is set to non-blocking mode.  When first created, sockets are by default in blocking mode.
  static void setBlocking( int socket, bool enable );

  //! Close the socket with the given descriptor.
  static void close( int socket );

  //! Returns TRUE is the socket descriptor is valid.
  static bool isValid( int socket );

  //! Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
  int writeBuffer(const void *buffer, long bufferSize, int flags = 0);

  //! Write a buffer via the specified socket.  Returns the number of bytes written or -1 if an error occurs.
  static int writeBuffer(int socket, const void *buffer, long bufferSize, int flags );

  //! Read a buffer from the socket connection, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
  int readBuffer(void *buffer, long bufferSize, int flags = 0);

  //! Read a buffer via the specified socket.  Returns the number of bytes read or -1 if an error occurs.
  static int readBuffer(int socket, void *buffer, long bufferSize, int flags );

 public: // SWAP formerly protected

  char msg[256];
  int soket;
  int poort;
  bool server;

};

#endif // defined(__SOCKET_H)




/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#if !defined(__VECTOR3D_H)
#define __VECTOR3D_H

class Vector3D {

public:
  //! Default constructor taking optional initial X, Y, and Z values.
  Vector3D(double initX=0.0, double initY=0.0, double initZ=0.0);

  //! Class destructor.
  ~Vector3D();

  //! Get the current X value.
  double getX();

  //! Get the current Y value.
  double getY();

  //! Get the current Z value.
  double getZ();

  //! Calculate the vector length.
  double getLength();

  //! Set the X, Y, and Z values simultaniously.
  void setXYZ(double anX, double aY, double aZ);

  //! Set the X value.
  void setX(double aval);

  //! Set the Y value.
  void setY(double aval);

  //! Set the Z value.
  void setZ(double aval);

public: // SWAP formerly protected
  double myX;
  double myY;
  double myZ;
};

#endif




/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SPHERE_H)
#define __SPHERE_H


class Sphere
{
public:
  //! Constructor taking an initial radius value.
  Sphere(double initRadius);

  //! Class destructor.
  ~Sphere();

  //! Set the 3D center position of the sphere.
  void setPosition(double anX, double aY, double aZ);

  //! Set the 3D velocity of the sphere.
  void setVelocity(double anX, double aY, double aZ);

  //! Set the radius of the sphere.
  void setRadius(double aRadius);

  //! Set the mass of the sphere.
  void setMass(double aMass);

  //! Get the current position of the sphere as a 3D vector.
  Vector3D* getPosition();

  //! Get the relative position of the given point to the sphere as a 3D vector.
  Vector3D* getRelativePosition(Vector3D *aPosition);

  //! Set the velcoity of the sphere as a 3D vector.
  double getVelocity(Vector3D* aVelocity);

  //! Returns the distance from the sphere boundary to the given position (< 0 if inside).
  double isInside(Vector3D *aPosition);

  //! Get the current sphere radius.
  double getRadius();

  //! Get the current sphere mass.
  double getMass();

  //! Increase the current sphere velocity by the given 3D components.
  void addVelocity(double anX, double aY, double aZ);

  //! Move the sphere for the given time increment.
  void tick(double timeIncrement);
   
public: // SWAP formerly private
  Vector3D *myPosition;
  Vector3D *myVelocity;
  Vector3D workingVector;
  double myRadius;
  double myMass;
};

#endif




/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__StifKarp_h)
#define __StifKarp_h


class StifKarp : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  StifKarp(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~StifKarp();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the stretch "factor" of the string (0.0 - 1.0).
  void setStretch(MY_FLOAT stretch);

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPickupPosition(MY_FLOAT position);

  //! Set the base loop gain.
  /*!
    The actual loop gain is set according to the frequency.
    Because of high-frequency loop filter roll-off, higher
    frequency settings have greater loop gains.
  */
  void setBaseLoopGain(MY_FLOAT aGain);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
    DelayA *delayLine;
    DelayL *combDelay;
    OneZero *filter;
    Noise *noise;
    BiQuad *biQuad[4];
    long length;
    MY_FLOAT loopGain;
    MY_FLOAT baseLoopGain;
    MY_FLOAT lastFrequency;
    MY_FLOAT lastLength;
    MY_FLOAT stretching;
    MY_FLOAT pluckAmplitude;
    MY_FLOAT pickupPosition;

};

#endif




/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TABLE_H)
#define __TABLE_H


class Table : public Stk
{
public:
  //! Constructor loads the data from \e fileName.
  Table(char *fileName);

  //! Class destructor.
  ~Table();

  //! Return the number of elements in the table.
  long getLength() const;

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the table value at position \e index.
  /*!
    Linear interpolation is performed if \e index is
    fractional.
   */
  MY_FLOAT tick(MY_FLOAT index);

  //! Take \e vectorSize index positions and return the corresponding table values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

public: // SWAP formerly protected  
  long length;
  MY_FLOAT *data;
  MY_FLOAT lastOutput;

};

#endif // defined(__TABLE_H)




/***************************************************/
/*! \class WvOut
    \brief STK audio data output base class.

    This class provides output support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvOut writes samples to an audio file.  It
    supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which output single samples
    to all channels in a sample frame, from the
    tickFrame() method, which takes a pointer
    to multi-channel sample frame data.

    WvOut currently supports WAV, AIFF, AIFC, SND
    (AU), MAT-file (Matlab), and STK RAW file
    formats.  Signed integer (8-, 16-, and 32-bit)
    and floating- point (32- and 64-bit) data types
    are supported.  STK RAW files use 16-bit
    integers by definition.  MAT-files will always
    be written as 64-bit floats.  If a data type
    specification does not match the specified file
    type, the data type will automatically be
    modified.  Uncompressed data types are not
    supported.

    Currently, WvOut is non-interpolating and the
    output rate is always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__WVOUT_H)
#define __WVOUT_H

#include <stdio.h>

#define BUFFER_SIZE 1024  // sample frames

class WvOut : public Stk
{
 public:

  typedef unsigned long FILE_TYPE;

  static const FILE_TYPE WVOUT_RAW; /*!< STK RAW file type. */
  static const FILE_TYPE WVOUT_WAV; /*!< WAV file type. */
  static const FILE_TYPE WVOUT_SND; /*!< SND (AU) file type. */
  static const FILE_TYPE WVOUT_AIF; /*!< AIFF file type. */
  static const FILE_TYPE WVOUT_MAT; /*!< Matlab MAT-file type. */

  //! Default constructor.
  WvOut();

  //! Overloaded constructor used to specify a file name, type, and data format with this object.
  /*!
    An StkError is thrown for invalid argument values or if an error occurs when initializing the output file.
  */
  WvOut( const char *fileName, unsigned int nChannels = 1, FILE_TYPE type = WVOUT_WAV, Stk::STK_FORMAT format = STK_SINT16 );

  //! Class destructor.
  virtual ~WvOut();

  //! Create a file of the specified type and name and output samples to it in the given data format.
  /*!
    An StkError is thrown for invalid argument values or if an error occurs when initializing the output file.
  */
  void openFile( const char *fileName, unsigned int nChannels = 1,  \
		 WvOut::FILE_TYPE type = WVOUT_WAV, Stk::STK_FORMAT = STK_SINT16 );
  //! If a file is open, write out samples in the queue and then close it.
  void closeFile( void );

  //! Return the number of sample frames output.
  unsigned long getFrames( void ) const;

  //! Return the number of seconds of data output.
  MY_FLOAT getTime( void ) const;

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tick(const MY_FLOAT sample);

  //! Output each sample in \e vector to all channels in \e vectorSize sample frames.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tick(const MY_FLOAT *vector, unsigned int vectorSize);

  //! Output the \e frameVector of sample frames of the given length.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tickFrame(const MY_FLOAT *frameVector, unsigned int frames = 1);

 public: // SWAP formerly protected

  // Initialize class variables.
  void init( void );

  // Write data to output file;
  virtual void writeData( unsigned long frames );

  // Write STK RAW file header.
  bool setRawFile( const char *fileName );

  // Write WAV file header.
  bool setWavFile( const char *fileName );

  // Close WAV file, updating the header.
  void closeWavFile( void );

  // Write SND (AU) file header.
  bool setSndFile( const char *fileName );

  // Close SND file, updating the header.
  void closeSndFile( void );

  // Write AIFF file header.
  bool setAifFile( const char *fileName );

  // Close AIFF file, updating the header.
  void closeAifFile( void );

  // Write MAT-file header.
  bool setMatFile( const char *fileName );

  // Close MAT-file, updating the header.
  void closeMatFile( void );

  char msg[256];
  FILE *fd;
  MY_FLOAT *data;
  FILE_TYPE fileType;
  STK_FORMAT dataType;
  bool byteswap;
  unsigned int channels;
  unsigned long counter;
  unsigned long totalCount;
  char m_filename[1024];
  t_CKUINT start;
  char autoPrefix[1024];
  t_CKUINT flush;
};

#endif // defined(__WVOUT_H)




/***************************************************/
/*! \class TubeBell
    \brief STK tubular bell (orchestral chime) FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TUBEBELL_H)
#define __TUBEBELL_H


class TubeBell : public FM
{
 public:
  //! Class constructor.
  TubeBell();

  //! Class destructor.
  ~TubeBell();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn( MY_FLOAT amplitude) { noteOn(baseFrequency, amplitude); }

  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This protected Filter subclass implements
    a two-pole digital filter.  A method is
    provided for creating a resonance in the
    frequency response while maintaining a nearly
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TWOPOLE_H)
#define __TWOPOLE_H


class TwoPole : public Filter // formerly protected Filter
{
 public:

  //! Default constructor creates a second-order pass-through filter.
  TwoPole();

  //! Class destructor.
  ~TwoPole();

  //! Clears the internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the a[1] coefficient value.
  void setA1(MY_FLOAT a1);

  //! Set the a[2] coefficient value.
  void setA2(MY_FLOAT a2);

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  If \e normalize is true,
    the coefficients are then normalized to produce unity gain at \e
    frequency (the actual maximum filter gain tends to be slightly
    greater than unity when \e radius is not close to one).  The
    resulting filter frequency response has a resonance at the given
    \e frequency.  The closer the poles are to the unit-circle (\e
    radius close to one), the narrower the resulting resonance width.
    An unstable filter will result for \e radius >= 1.0.  For a better
    resonance filter, use a BiQuad filter. \sa BiQuad filter class
  */
  void setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize = FALSE);

  bool m_resNorm;
  MY_FLOAT m_resFreq;
  MY_FLOAT m_resRad;

  void ck_setResNorm( bool n ) { m_resNorm = n; setResonance(m_resFreq, m_resRad, m_resNorm); } 
  void ck_setResFreq( MY_FLOAT n ) { m_resFreq = n; setResonance(m_resFreq, m_resRad, m_resNorm); } 
  void ck_setResRad( MY_FLOAT n ) { m_resRad = n; setResonance(m_resFreq, m_resRad, m_resNorm); } 
  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif




/***************************************************/
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/
#if !defined(__VOICFORM_H)
#define __VOICFORM_H


class VoicForm : public Instrmnt
{
  public:
  //! Class constructor, taking the lowest desired playing frequency.
  VoicForm();

  //! Class destructor.
  ~VoicForm();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set instrument parameters for the given phoneme.  Returns FALSE if phoneme not found.
  bool setPhoneme(const char* phoneme);

  //! Set the voiced component gain.
  void setVoiced(MY_FLOAT vGain);

  //! Set the unvoiced component gain.
  void setUnVoiced(MY_FLOAT nGain);

  //! Set the sweep rate for a particular formant filter (0-3).
  void setFilterSweepRate(int whichOne, MY_FLOAT rate);

  //! Set voiced component pitch sweep rate.
  void setPitchSweepRate(MY_FLOAT rate);

  //! Start the voice.
  void speak();

  //! Stop the voice.
  void quiet();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  //! start at current frequency..
  void noteOn( MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

public: // SWAP formerly protected
  int       m_phonemeNum;
  SingWave *voiced;
  Noise    *noise;
  Envelope *noiseEnv;
  FormSwep  *filters[4];
  OnePole  *onepole;
  OneZero  *onezero;

};

#endif




/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of
    STK instrument classes.  Individual voices can
    be controlled via unique note tags.
    Instrument groups can be controlled by channel
    number.

    A previously constructed STK instrument class
    is linked with a voice manager using the
    addInstrument() function.  An optional channel
    number argument can be specified to the
    addInstrument() function as well (default
    channel = 0).  The voice manager does not
    delete any instrument instances ... it is the
    responsibility of the user to allocate and
    deallocate all instruments.

    The tick() function returns the mix of all
    sounding voices.  Each noteOn returns a unique
    tag (credits to the NeXT MusicKit), so you can
    send control changes to specific voices within
    an ensemble.  Alternately, control changes can
    be sent to all voices on a given channel.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__VOICER_H)
#define __VOICER_H


class Voicer : public Stk
{
public:
  //! Class constructor taking the maximum number of instruments to control and an optional note decay time (in seconds).
  Voicer( int maxInstruments, MY_FLOAT decayTime=0.2 );

  //! Class destructor.
  ~Voicer();

  //! Add an instrument with an optional channel number to the voice manager.
  /*!
    A set of instruments can be grouped by channel number and
    controlled via the functions which take a channel number argument.
  */
  void addInstrument( Instrmnt *instrument, int channel=0 );

  //! Remove the given instrument pointer from the voice manager's control.
  /*!
    It is important that any instruments which are to be deleted by
    the user while the voice manager is running be first removed from
    the manager's control via this function!!
   */
  void removeInstrument( Instrmnt *instrument );

  //! Initiate a noteOn event with the given note number and amplitude and return a unique note tag.
  /*!
    Send the noteOn message to the first available unused voice.
    If all voices are sounding, the oldest voice is interrupted and
    sent the noteOn message.  If the optional channel argument is
    non-zero, only voices on that channel are used.  If no voices are
    found for a specified non-zero channel value, the function returns
    -1.  The amplitude value should be in the range 0.0 - 128.0.
  */
  long noteOn( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel=0 );

  //! Send a noteOff to all voices having the given noteNumber and optional channel (default channel = 0).
  /*!
    The amplitude value should be in the range 0.0 - 128.0.
  */
  void noteOff( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel=0 );

  //! Send a noteOff to the voice with the given note tag.
  /*!
    The amplitude value should be in the range 0.0 - 128.0.
  */
  void noteOff( long tag, MY_FLOAT amplitude );

  //! Send a frequency update message to all voices assigned to the optional channel argument (default channel = 0).
  /*!
    The \e noteNumber argument corresponds to a MIDI note number, though it is a floating-point value and can range beyond the normal 0-127 range.
   */
  void setFrequency( MY_FLOAT noteNumber, int channel=0 );

  //! Send a frequency update message to the voice with the given note tag.
  /*!
    The \e noteNumber argument corresponds to a MIDI note number, though it is a floating-point value and can range beyond the normal 0-127 range.
   */
  void setFrequency( long tag, MY_FLOAT noteNumber );

  //! Send a pitchBend message to all voices assigned to the optional channel argument (default channel = 0).
  void pitchBend( MY_FLOAT value, int channel=0 );

  //! Send a pitchBend message to the voice with the given note tag.
  void pitchBend( long tag, MY_FLOAT value );

  //! Send a controlChange to all instruments assigned to the optional channel argument (default channel = 0).
  void controlChange( int number, MY_FLOAT value, int channel=0 );

  //! Send a controlChange to the voice with the given note tag.
  void controlChange( long tag, int number, MY_FLOAT value );

  //! Send a noteOff message to all existing voices.
  void silence( void );

  //! Mix the output for all sounding voices.
  MY_FLOAT tick();

  //! Compute \e vectorSize output mixes and return them in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

public: // SWAP formerly protected

  typedef struct {
    Instrmnt *instrument;
    long tag;
    MY_FLOAT noteNumber;
    MY_FLOAT frequency;
    int sounding;
    int channel;
  } Voice;

  int  nVoices;
  int maxVoices;
  Voice *voices;
  long tags;
  int muteTime;
  MY_FLOAT lastOutput;
  MY_FLOAT lastOutputLeft;
  MY_FLOAT lastOutputRight;
};

#endif




/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1996 - 2002.
*/
/***************************************************/

#if !defined(__WHISTLE_H)
#define __WHISTLE_H


class Whistle : public Instrmnt
{
public:
  //! Class constructor.
  Whistle();

  //! Class destructor.
  ~Whistle();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

public: // SWAP formerly protected  
	Vector3D *tempVectorP;
  Vector3D *tempVector;
  OnePole onepole;
  Noise noise;
	Envelope envelope;
  Sphere *can;           // Declare a Spherical "can".
  Sphere *pea, *bumper;  // One spherical "pea", and a spherical "bumper".

  WaveLoop *sine;

  MY_FLOAT baseFrequency;
	MY_FLOAT maxPressure;
  MY_FLOAT noiseGain;
  MY_FLOAT fippleFreqMod;
	MY_FLOAT fippleGainMod;
	MY_FLOAT blowFreqMod;
	MY_FLOAT tickSize;
	MY_FLOAT canLoss;
	int subSample, subSampCount;
};

#endif




/***************************************************/
/*! \class Wurley
    \brief STK Wurlitzer electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__WURLEY_H)
#define __WURLEY_H


class Wurley : public FM
{
 public:
  //! Class constructor.
  Wurley();

  //! Class destructor.
  ~Wurley();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);
  void noteOn(MY_FLOAT amplitude) { noteOn ( baseFrequency, amplitude ); }

  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/*********************************************************/
/*
  Definition of SKINI Message Types and Special Symbols
     Synthesis toolKit Instrument Network Interface

  These symbols should have the form __SK_<name>_

  Where <name> is the string used in the SKINI stream.

  by Perry R. Cook, 1995 - 2002.
*/
/*********************************************************/

/***** MIDI COMPATIBLE MESSAGES *****/
/***** Status Bytes Have Channel=0 **/

#define NOPE    -32767
#define YEP     1
#define SK_DBL  -32766
#define SK_INT  -32765
#define SK_STR  -32764

#define __SK_NoteOff_                128
#define __SK_NoteOn_                 144
#define __SK_PolyPressure_           160
#define __SK_ControlChange_          176
#define __SK_ProgramChange_          192
#define __SK_AfterTouch_             208
#define __SK_ChannelPressure_        __SK_AfterTouch_
#define __SK_PitchWheel_             224
#define __SK_PitchBend_              __SK_PitchWheel_
#define __SK_PitchChange_            49

#define __SK_Clock_                  248
#define __SK_SongStart_              250
#define __SK_Continue_               251
#define __SK_SongStop_               252
#define __SK_ActiveSensing_          254
#define __SK_SystemReset_            255

#define __SK_Volume_                 7
#define __SK_ModWheel_               1
#define __SK_Modulation_             __SK_ModWheel_
#define __SK_Breath_                 2
#define __SK_FootControl_            4
#define __SK_Portamento_             65
#define __SK_Balance_                8
#define __SK_Pan_                    10
#define __SK_Sustain_                64
#define __SK_Damper_                 __SK_Sustain_
#define __SK_Expression_             11 

#define __SK_AfterTouch_Cont_        128
#define __SK_ModFrequency_           __SK_Expression_

#define __SK_ProphesyRibbon_         16
#define __SK_ProphesyWheelUp_        2
#define __SK_ProphesyWheelDown_      3
#define __SK_ProphesyPedal_          18
#define __SK_ProphesyKnob1_          21
#define __SK_ProphesyKnob2_          22

/***  Instrument Family Specific ***/

#define __SK_NoiseLevel_             __SK_FootControl_

#define __SK_PickPosition_           __SK_FootControl_
#define __SK_StringDamping_          __SK_Expression_
#define __SK_StringDetune_           __SK_ModWheel_
#define __SK_BodySize_               __SK_Breath_
#define __SK_BowPressure_            __SK_Breath_
#define __SK_BowPosition_            __SK_PickPosition_
#define __SK_BowBeta_                __SK_BowPosition_

#define __SK_ReedStiffness_          __SK_Breath_
#define __SK_ReedRestPos_            __SK_FootControl_

#define __SK_FluteEmbouchure_        __SK_Breath_
#define __SK_JetDelay_               __SK_FluteEmbouchure_

#define __SK_LipTension_             __SK_Breath_
#define __SK_SlideLength_            __SK_FootControl_

#define __SK_StrikePosition_         __SK_PickPosition_
#define __SK_StickHardness_          __SK_Breath_

#define __SK_TrillDepth_             1051
#define __SK_TrillSpeed_             1052
#define __SK_StrumSpeed_             __SK_TrillSpeed_
#define __SK_RollSpeed_              __SK_TrillSpeed_

#define __SK_FilterQ_                __SK_Breath_
#define __SK_FilterFreq_             1062
#define __SK_FilterSweepRate_        __SK_FootControl_

#define __SK_ShakerInst_             1071 
#define __SK_ShakerEnergy_           __SK_Breath_
#define __SK_ShakerDamping_          __SK_ModFrequency_
#define __SK_ShakerNumObjects_       __SK_FootControl_

#define __SK_Strumming_              1090
#define __SK_NotStrumming_           1091
#define __SK_Trilling_               1092
#define __SK_NotTrilling_            1093
#define __SK_Rolling_                __SK_Strumming_
#define __SK_NotRolling_             __SK_NotStrumming_

#define __SK_PlayerSkill_            2001
#define __SK_Chord_                  2002
#define __SK_ChordOff_               2003

#define __SK_SINGER_FilePath_        3000
#define __SK_SINGER_Frequency_       3001
#define __SK_SINGER_NoteName_        3002
#define __SK_SINGER_Shape_           3003
#define __SK_SINGER_Glot_            3004
#define __SK_SINGER_VoicedUnVoiced_  3005
#define __SK_SINGER_Synthesize_      3006
#define __SK_SINGER_Silence_         3007
#define __SK_SINGER_VibratoAmt_      __SK_ModWheel_
#define __SK_SINGER_RndVibAmt_       3008
#define __SK_SINGER_VibFreq_         __SK_Expression_




#define __SK_MaxMsgTypes_ 128

struct SKINISpec { char messageString[32];
                   long  type;
                   long data2;
                   long data3;
                 };

/*   SEE COMMENT BLOCK AT BOTTOM FOR FIELDS AND USES   */
/* MessageString     ,type,  ch?,        data2        ,             data3 */ 

struct SKINISpec skini_msgs[__SK_MaxMsgTypes_] = 
{
 {"NoteOff"          ,        __SK_NoteOff_,               SK_DBL,  SK_DBL},
 {"NoteOn"           ,         __SK_NoteOn_,               SK_DBL,  SK_DBL},
 {"PolyPressure"     ,   __SK_PolyPressure_,               SK_DBL,  SK_DBL},
 {"ControlChange"    ,  __SK_ControlChange_,               SK_INT,  SK_DBL},
 {"ProgramChange"    ,  __SK_ProgramChange_,               SK_DBL,  SK_DBL},
 {"AfterTouch"       ,     __SK_AfterTouch_,               SK_DBL,    NOPE},
 {"ChannelPressure"  ,__SK_ChannelPressure_,               SK_DBL,    NOPE},
 {"PitchWheel"       ,     __SK_PitchWheel_,               SK_DBL,    NOPE},
 {"PitchBend"        ,      __SK_PitchBend_,               SK_DBL,    NOPE},
 {"PitchChange"      ,    __SK_PitchChange_,               SK_DBL,    NOPE},
                                                                 
 {"Clock"            ,          __SK_Clock_,                 NOPE,    NOPE},
 {"Undefined"        ,                  249,                 NOPE,    NOPE},
 {"SongStart"        ,      __SK_SongStart_,                 NOPE,    NOPE},
 {"Continue"         ,       __SK_Continue_,                 NOPE,    NOPE},
 {"SongStop"         ,       __SK_SongStop_,                 NOPE,    NOPE},
 {"Undefined"        ,                  253,                 NOPE,    NOPE},
 {"ActiveSensing"    ,  __SK_ActiveSensing_,                 NOPE,    NOPE},
 {"SystemReset"      ,    __SK_SystemReset_,                 NOPE,    NOPE},
    
 {"Volume"           ,  __SK_ControlChange_, __SK_Volume_        ,  SK_DBL},
 {"ModWheel"         ,  __SK_ControlChange_, __SK_ModWheel_      ,  SK_DBL},
 {"Modulation"       ,  __SK_ControlChange_, __SK_Modulation_    ,  SK_DBL},
 {"Breath"           ,  __SK_ControlChange_, __SK_Breath_        ,  SK_DBL},
 {"FootControl"      ,  __SK_ControlChange_, __SK_FootControl_   ,  SK_DBL},
 {"Portamento"       ,  __SK_ControlChange_, __SK_Portamento_    ,  SK_DBL},
 {"Balance"          ,  __SK_ControlChange_, __SK_Balance_       ,  SK_DBL},
 {"Pan"              ,  __SK_ControlChange_, __SK_Pan_           ,  SK_DBL},
 {"Sustain"          ,  __SK_ControlChange_, __SK_Sustain_       ,  SK_DBL},
 {"Damper"           ,  __SK_ControlChange_, __SK_Damper_        ,  SK_DBL},
 {"Expression"       ,  __SK_ControlChange_, __SK_Expression_    ,  SK_DBL},
                                  
 {"NoiseLevel"       ,  __SK_ControlChange_, __SK_NoiseLevel_    ,  SK_DBL},
 {"PickPosition"     ,  __SK_ControlChange_, __SK_PickPosition_  ,  SK_DBL},
 {"StringDamping"    ,  __SK_ControlChange_, __SK_StringDamping_ ,  SK_DBL},
 {"StringDetune"     ,  __SK_ControlChange_, __SK_StringDetune_  ,  SK_DBL},
 {"BodySize"         ,  __SK_ControlChange_, __SK_BodySize_      ,  SK_DBL},
 {"BowPressure"      ,  __SK_ControlChange_, __SK_BowPressure_   ,  SK_DBL},
 {"BowPosition"      ,  __SK_ControlChange_, __SK_BowPosition_   ,  SK_DBL},
 {"BowBeta"          ,  __SK_ControlChange_, __SK_BowBeta_       ,  SK_DBL},
 
 {"ReedStiffness"    ,  __SK_ControlChange_, __SK_ReedStiffness_ ,  SK_DBL},
 {"ReedRestPos"      ,  __SK_ControlChange_, __SK_ReedRestPos_   ,  SK_DBL},
 {"FluteEmbouchure"  ,  __SK_ControlChange_, __SK_FluteEmbouchure_, SK_DBL},
 {"LipTension"       ,  __SK_ControlChange_, __SK_LipTension_    ,  SK_DBL},
 {"StrikePosition"   ,  __SK_ControlChange_, __SK_StrikePosition_,  SK_DBL},
 {"StickHardness"    ,  __SK_ControlChange_, __SK_StickHardness_ ,  SK_DBL},

 {"TrillDepth"       ,  __SK_ControlChange_, __SK_TrillDepth_    ,  SK_DBL}, 
 {"TrillSpeed"       ,  __SK_ControlChange_, __SK_TrillSpeed_    ,  SK_DBL},
                                             
 {"Strumming"        ,  __SK_ControlChange_, __SK_Strumming_     ,  127   }, 
 {"NotStrumming"     ,  __SK_ControlChange_, __SK_Strumming_     ,  0     },
                                             
 {"PlayerSkill"      ,  __SK_ControlChange_, __SK_PlayerSkill_   ,  SK_DBL}, 

 {"Chord"            ,  __SK_Chord_	   , 		SK_DBL   , SK_STR }, 
 {"ChordOff"         ,  __SK_ChordOff_     ,  		SK_DBL   ,  NOPE  }, 

 {"ShakerInst"       ,  __SK_ControlChange_, __SK_ShakerInst_   ,  SK_DBL},
 {"Maraca"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   0    },
 {"Sekere"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   1    },
 {"Cabasa"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   2    },
 {"Bamboo"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   3    },
 {"Waterdrp"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   4    },
 {"Tambourn"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   5    },
 {"Sleighbl"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   6    },
 {"Guiro"	     ,  __SK_ControlChange_, __SK_ShakerInst_	,   7    },	

 {"OpenFile"         ,                  256,         SK_STR      ,    NOPE},
 {"SetPath"          ,                  257,         SK_STR      ,    NOPE},

 {"FilePath"         ,  __SK_SINGER_FilePath_,           SK_STR   ,    NOPE},
 {"Frequency"        ,  __SK_SINGER_Frequency_,          SK_STR   ,    NOPE},
 {"NoteName"         ,  __SK_SINGER_NoteName_,           SK_STR   ,    NOPE},
 {"VocalShape"       ,  __SK_SINGER_Shape_   ,           SK_STR   ,    NOPE},
 {"Glottis"          ,  __SK_SINGER_Glot_    ,           SK_STR   ,    NOPE},
 {"VoicedUnVoiced"   ,  __SK_SINGER_VoicedUnVoiced_,     SK_DBL   ,  SK_STR},
 {"Synthesize"       ,  __SK_SINGER_Synthesize_,         SK_STR   ,    NOPE},
 {"Silence"          ,  __SK_SINGER_Silence_,            SK_STR   ,    NOPE},
 {"VibratoAmt"       ,  __SK_ControlChange_  ,__SK_SINGER_VibratoAmt_,SK_DBL},
 {"RndVibAmt"        ,  __SK_SINGER_RndVibAmt_          ,SK_STR,       NOPE},
 {"VibFreq"          ,  __SK_ControlChange_  ,__SK_SINGER_VibFreq_   ,SK_DBL}
};


/**  FORMAT: *************************************************************/
/*                                                                       */
/* MessageStr$      ,type, data2, data3,                                 */
/*                                                                       */
/*     type is the message type sent back from the SKINI line parser.    */
/*     data<n> is either                                                 */
/*          NOPE    : field not used, specifically, there aren't going   */                                           
/*                    to be any more fields on this line.  So if there   */
/*                    is is NOPE in data2, data3 won't even be checked   */
/*          SK_INT  : byte (actually scanned as 32 bit signed integer)   */
/*                      If it's a MIDI data field which is required to   */
/*                      be an integer, like a controller number, it's    */
/*                      0-127.  Otherwise) get creative with SK_INTs     */
/*          SK_DBL  : double precision floating point.  SKINI uses these */
/*                    in the MIDI context for note numbers with micro    */
/*                    tuning, velocities, controller values, etc.        */
/*          SK_STR  : only valid in final field.  This allows (nearly)   */
/*                    arbitrary message types to be supported by simply  */
/*                    scanning the string to EndOfLine and then passing  */
/*                    it to a more intellegent handler.  For example,    */
/*                    MIDI SYSEX (system exclusive) messages of up to    */
/*                    256bytes can be read as space-delimited integers   */
/*                    into the 1K SK_STR buffer.  Longer bulk dumps,     */
/*                    soundfiles, etc. should be handled as a new        */
/*                    message type pointing to a FileName stored in the  */
/*                    SK_STR field, or as a new type of multi-line       */                                      
/*                    message.                                           */
/*                                                                       */
/*************************************************************************/


/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This Envelope subclass implements a
    traditional ADSR (Attack, Decay,
    Sustain, Release) envelope.  It
    responds to simple keyOn and keyOff
    messages, keeping track of its state.
    The \e state = ADSR::DONE after the
    envelope value reaches 0.0 in the
    ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/
#include <stdio.h>
#include "ugen_stk.h"

ADSR :: ADSR() : Envelope()
{
  target = (MY_FLOAT) 0.0;
  value = (MY_FLOAT) 0.0;
  attackRate = (MY_FLOAT) 0.001;
  decayRate = (MY_FLOAT) 0.001;
  sustainLevel = (MY_FLOAT) 0.5;
  releaseRate = (MY_FLOAT) 0.01;
  state = ATTACK;
}

ADSR :: ~ADSR()
{
}

void ADSR :: keyOn()
{
  target = (MY_FLOAT) 1.0;
  rate = attackRate;
  state = ATTACK;
}

void ADSR :: keyOff()
{
  target = (MY_FLOAT) 0.0;
  rate = releaseRate;
  state = RELEASE;
}

void ADSR :: setAttackRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    attackRate = -aRate;
  }
  else attackRate = aRate;
}

void ADSR :: setDecayRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    decayRate = -aRate;
  }
  else decayRate = aRate;
}

void ADSR :: setSustainLevel(MY_FLOAT aLevel)
{
  if (aLevel < 0.0 ) {
    printf("[chuck](via ADSR): sustain level out of range ... correcting!\n");
    sustainLevel = (MY_FLOAT)  0.0;
  }
  else sustainLevel = aLevel;
}

void ADSR :: setReleaseRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    releaseRate = -aRate;
  }
  else releaseRate = aRate;
}

void ADSR :: setAttackTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    attackRate = 1.0 / ( -aTime * Stk::sampleRate() );
  }
  else attackRate = 1.0 / ( aTime * Stk::sampleRate() );
}

void ADSR :: setDecayTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via ADSR): negative times not allowed ... correcting!\n");
    decayRate = 1.0 / ( -aTime * Stk::sampleRate() );
  }
  else decayRate = 1.0 / ( aTime * Stk::sampleRate() );
}

void ADSR :: setReleaseTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via ADSR): negative times not allowed ... correcting!\n");
    releaseRate = sustainLevel / ( -aTime * Stk::sampleRate() );
  }
  else releaseRate = sustainLevel / ( aTime * Stk::sampleRate() );
}

void ADSR :: setAllTimes(MY_FLOAT aTime, MY_FLOAT dTime, MY_FLOAT sLevel, MY_FLOAT rTime)
{
  this->setAttackTime(aTime);
  this->setDecayTime(dTime);
  this->setSustainLevel(sLevel);
  this->setReleaseTime(rTime);
}

void ADSR :: setTarget(MY_FLOAT aTarget)
{
  target = aTarget;
  if (value < target) {
    state = ATTACK;
    this->setSustainLevel(target);
    rate = attackRate;
  }
  if (value > target) {
    this->setSustainLevel(target);
    state = DECAY;
    rate = decayRate;
  }
}

void ADSR :: setValue(MY_FLOAT aValue)
{
  state = SUSTAIN;
  target = aValue;
  value = aValue;
  this->setSustainLevel(aValue);
  rate = (MY_FLOAT)  0.0;
}

int ADSR :: getState(void) const
{
  return state;
}

MY_FLOAT ADSR :: tick()
{
  switch (state) {

  case ATTACK:
    value += rate;
    if (value >= target) {
      value = target;
      rate = decayRate;
      target = sustainLevel;
	    state = DECAY;
    }
    break;

  case DECAY:
    value -= decayRate;
    if (value <= sustainLevel) {
      value = sustainLevel;
      rate = (MY_FLOAT) 0.0;
      state = SUSTAIN;
    }
    break;

  case RELEASE:
    value -= releaseRate;
    if (value <= 0.0)       {
      value = (MY_FLOAT) 0.0;
      state = DONE;
    }
  }

  return value;
}

MY_FLOAT *ADSR :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}
/***************************************************/
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999 - 2002.
    Modified for Stk 4.0 by Gary Scavone.
*/
/***************************************************/

#include <math.h>

BandedWG :: BandedWG()
{
  doPluck = true;

  delay = new DelayL[MAX_BANDED_MODES];
  bandpass = new BiQuad[MAX_BANDED_MODES];
  
  bowTabl = new BowTabl;
  bowTabl->setSlope( 3.0 );

  adsr = new ADSR;
  adsr->setAllTimes( 0.02, 0.005, 0.9, 0.01);

  freakency = 220.0;
  setPreset(0);

  bowPosition = 0;
  baseGain = (MY_FLOAT) 0.999;
  
  integrationConstant = 0.0;
  trackVelocity = false;

  bowVelocity = 0.0;
  bowTarget = 0.0;

  strikeAmp = 0.0;
}

BandedWG :: ~BandedWG()
{
  delete bowTabl;
  delete adsr;
  delete [] bandpass;
  delete [] delay;
}

void BandedWG :: clear()
{
  for (int i=0; i<nModes; i++) {
    delay[i].clear();
    bandpass[i].clear();
  }
}

void BandedWG :: setPreset(int preset)
{
  int i;
  m_preset = preset;
  switch (preset){

  case 1: // Tuned Bar
    presetModes = 4;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 4.0198391420;
    modes[2] = (MY_FLOAT) 10.7184986595;
    modes[3] = (MY_FLOAT) 18.0697050938;

    for (i=0; i<presetModes; i++) {
      basegains[i] = (MY_FLOAT) pow(0.999,(double) i+1);
      excitation[i] = 1.0;
    }

    break;

  case 2: // Glass Harmonica
    presetModes = 5;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 2.32;
    modes[2] = (MY_FLOAT) 4.25;
    modes[3] = (MY_FLOAT) 6.63;
    modes[4] = (MY_FLOAT) 9.38;
    // modes[5] = (MY_FLOAT) 12.22;

    for (i=0; i<presetModes; i++) {
      basegains[i] = (MY_FLOAT) pow(0.999,(double) i+1);
      excitation[i] = 1.0;
    }
    /*
      baseGain = (MY_FLOAT) 0.99999;
      for (i=0; i<presetModes; i++) 
      gains[i]= (MY_FLOAT) pow(baseGain, delay[i].getDelay()+i);
    */

    break;
   
  case 3: // Tibetan Prayer Bowl (ICMC'02)
    presetModes = 12;
    modes[0]=0.996108344;
    basegains[0]=0.999925960128219;
    excitation[0]=11.900357/10.0;
    modes[1]=1.0038916562;
    basegains[1]=0.999925960128219;
    excitation[1]=11.900357/10.;
    modes[2]=2.979178;
    basegains[2]=0.999982774366897;
    excitation[2]=10.914886/10.;
    modes[3]=2.99329767;
    basegains[3]=0.999982774366897;
    excitation[3]=10.914886/10.;
    modes[4]=5.704452;
    basegains[4]=1.0; //0.999999999999999999987356406352;
    excitation[4]=42.995041/10.;
    modes[5]=5.704452;
    basegains[5]=1.0; //0.999999999999999999987356406352;
    excitation[5]=42.995041/10.;
    modes[6]=8.9982;
    basegains[6]=1.0; //0.999999999999999999996995497558225;
    excitation[6]=40.063034/10.;
    modes[7]=9.01549726;
    basegains[7]=1.0; //0.999999999999999999996995497558225;
    excitation[7]=40.063034/10.;
    modes[8]=12.83303;
    basegains[8]=0.999965497558225;
    excitation[8]=7.063034/10.;
    modes[9]=12.807382;
    basegains[9]=0.999965497558225;
    excitation[9]=7.063034/10.;
    modes[10]=17.2808219;
    basegains[10]=0.9999999999999999999965497558225;
    excitation[10]=57.063034/10.;
    modes[11]=21.97602739726;
    basegains[11]=0.999999999999999965497558225;
    excitation[11]=57.063034/10.;

    break;	

  default: // Uniform Bar
    presetModes = 4;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 2.756;
    modes[2] = (MY_FLOAT) 5.404;
    modes[3] = (MY_FLOAT) 8.933;

    for (i=0; i<presetModes; i++) {
      basegains[i] = (MY_FLOAT) pow(0.9,(double) i+1);
      excitation[i] = 1.0;
    }

    break;
  }

  nModes = presetModes;
  setFrequency( freakency );
}

void BandedWG :: setFrequency(MY_FLOAT frequency)
{
  freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): BandedWG: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }
  if (freakency > 1568.0) freakency = 1568.0;

  MY_FLOAT radius;
  MY_FLOAT base = Stk::sampleRate() / freakency;
  MY_FLOAT length;
  for (int i=0; i<presetModes; i++) {
    // Calculate the delay line lengths for each mode.
    length = (int)(base / modes[i]);
    if ( length > 2.0) {
      delay[i].setDelay( length );
      gains[i]=basegains[i];
      //	  gains[i]=(MY_FLOAT) pow(basegains[i], 1/((MY_FLOAT)delay[i].getDelay()));
      //	  std::cerr << gains[i];
    }
    else	{
      nModes = i;
      break;
    }
    //	std::cerr << std::endl;

    // Set the bandpass filter resonances
    radius = 1.0 - PI * 32 / Stk::sampleRate(); //freakency * modes[i] / Stk::sampleRate()/32;
    if ( radius < 0.0 ) radius = 0.0;
    bandpass[i].setResonance(freakency * modes[i], radius, true);

    delay[i].clear();
    bandpass[i].clear();
  }

  //int olen = (int)(delay[0].getDelay());
  //strikePosition = (int)(strikePosition*(length/modes[0])/olen);
}

void BandedWG :: setStrikePosition(MY_FLOAT position)
{
  strikePosition = (int)(delay[0].getDelay() * position / 2.0);
}

void BandedWG :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = 0.03 + (0.1 * amplitude); 
}

void BandedWG :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void BandedWG :: pluck(MY_FLOAT amplitude)
{
  int j;
  MY_FLOAT min_len = delay[nModes-1].getDelay();
  for (int i=0; i<nModes; i++)
    for(j=0; j<(int)(delay[i].getDelay()/min_len); j++)
      delay[i].tick( excitation[i]*amplitude / nModes /*/ (delay[i].getDelay()/min_len)*/);

  /*	strikeAmp += amplitude;*/
}

void BandedWG :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);

  if ( doPluck )
    this->pluck(amplitude);
  else
    this->startBowing(amplitude, amplitude * 0.001);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BandedWG: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void BandedWG :: noteOff(MY_FLOAT amplitude)
{
  if ( !doPluck )
    this->stopBowing((1.0 - amplitude) * 0.005);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BandedWG: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT BandedWG :: tick()
{
  int k;

  MY_FLOAT input = 0.0;
  if ( doPluck ) {
    input = 0.0;
    //  input = strikeAmp/nModes;
    //  strikeAmp = 0.0;
  }
  else {
    if (integrationConstant == 0.0)
      velocityInput = 0.0;
    else
      velocityInput = integrationConstant * velocityInput;

    for (k=0; k<nModes; k++)
      velocityInput += baseGain * delay[k].lastOut();
      
    if ( trackVelocity )  {
      bowVelocity *= 0.9995;
      bowVelocity += bowTarget;
      bowTarget *= 0.995;
    }
    else
      bowVelocity = adsr->tick() * maxVelocity;

    input = bowVelocity - velocityInput;
    input = input * bowTabl->tick(input);
    input = input/(MY_FLOAT)nModes;
  }

  MY_FLOAT data = 0.0;  
  for (k=0; k<nModes; k++) {
    bandpass[k].tick(input + gains[k] * delay[k].lastOut());
    delay[k].tick(bandpass[k].lastOut());
    data += bandpass[k].lastOut();
  }
  
  //lastOutput = data * nModes;
  lastOutput = data * 4;
  return lastOutput;
}

void BandedWG :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): BandedWG: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): BandedWG: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_BowPressure_) { // 2
    m_bowpressure = norm;
    if ( norm == 0.0 )
      doPluck = true;
    else {
      doPluck = false;
      bowTabl->setSlope( 10.0 - (9.0 * norm));
    }
  }
  else if (number == 4)	{ // 4
    if ( !trackVelocity ) trackVelocity = true;
    bowTarget += 0.005 * (norm - bowPosition);
    bowPosition = norm;
    //adsr->setTarget(bowPosition);
  }
  else if (number == 8) // 8
    this->setStrikePosition( norm );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    //bowTarget += 0.02 * (norm - bowPosition);
    //bowPosition = norm;
    if ( trackVelocity ) trackVelocity = false;
    maxVelocity = 0.13 * norm; 
    adsr->setTarget(norm);
  }      
  else if (number == __SK_ModWheel_) { // 1
    //    baseGain = 0.9989999999 + (0.001 * norm );
	  baseGain = 0.8999999999999999 + (0.1 * norm);
    //	std::cerr << "[chuck](via STK): Yuck!" << std::endl;
    for (int i=0; i<nModes; i++)
      gains[i]=(MY_FLOAT) basegains[i]*baseGain;
    //      gains[i]=(MY_FLOAT) pow(baseGain, (int)((MY_FLOAT)delay[i].getDelay()+i));
  }
  else if (number == __SK_ModFrequency_) // 11
    integrationConstant = norm;
  else if (number == __SK_Sustain_)	{ // 64
    if (value < 65) doPluck = true;
    else doPluck = false;
  }
  else if (number == __SK_Portamento_)	{ // 65
    if (value < 65) trackVelocity = false;
    else trackVelocity = true;
  }
  else if (number == __SK_ProphesyRibbon_) // 16
    this->setPreset((int) value);  
  else
    std::cerr << "[chuck](via STK): BandedWG: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BandedWG: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}


/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


BeeThree :: BeeThree()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( (Stk::rawwavePath() + "special:sinewave").c_str(), TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 0.999);
  this->setRatio(1, 1.997);
  this->setRatio(2, 3.006);
  this->setRatio(3, 6.009);

  gains[0] = __FM_gains[95];
  gains[1] = __FM_gains[95];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[95];

  adsr[0]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[1]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[2]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[3]->setAllTimes( 0.005, 0.001, 0.4, 0.03);

  twozero->setGain( 0.1 );
}  

BeeThree :: ~BeeThree()
{
}

void BeeThree :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[95];
  gains[1] = amplitude * __FM_gains[95];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[95];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "BeeThree: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT BeeThree :: tick()
{
  register MY_FLOAT temp;

  if (modDepth > 0.0)	{
    temp = 1.0 + (modDepth * vibrato->tick() * 0.1);
    waves[0]->setFrequency(baseFrequency * temp * ratios[0]);
    waves[1]->setFrequency(baseFrequency * temp * ratios[1]);
    waves[2]->setFrequency(baseFrequency * temp * ratios[2]);
    waves[3]->setFrequency(baseFrequency * temp * ratios[3]);
  }

  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = control1 * 2.0 * gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  temp += control2 * 2.0 * gains[2] * adsr[2]->tick() * waves[2]->tick();
  temp += gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp += gains[0] * adsr[0]->tick() * waves[0]->tick();

  lastOutput = temp * 0.125;
  return lastOutput;
}
/***************************************************/
/*! \class BiQuad
    \brief STK biquad (two-pole, two-zero) filter class.

    This protected Filter subclass implements a
    two-pole, two-zero digital filter.  A method
    is provided for creating a resonance in the
    frequency response while maintaining a constant
    filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

BiQuad :: BiQuad() : Filter()
{
  MY_FLOAT B[3] = {1.0, 0.0, 0.0};
  MY_FLOAT A[3] = {1.0, 0.0, 0.0};
  Filter::setCoefficients( 3, B, 3, A );
}

BiQuad :: ~BiQuad()
{
}

void BiQuad :: clear(void)
{
  Filter::clear();
}

void BiQuad :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void BiQuad :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void BiQuad :: setB2(MY_FLOAT b2)
{
  b[2] = b2;
}

void BiQuad :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void BiQuad :: setA2(MY_FLOAT a2)
{
  a[2] = a2;
}

void BiQuad :: setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize)
{
  a[2] = radius * radius;
  a[1] = -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Use zeros at +- 1 and normalize the filter peak gain.
    b[0] = 0.5 - 0.5 * a[2];
    b[1] = 0.0;
    b[2] = -b[0];
  }
}

void BiQuad :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  // This method does not attempt to normalize the filter gain.
  b[2] = radius * radius;
  b[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());
}

void BiQuad :: setEqualGainZeroes()
{
  b[0] = 1.0;
  b[1] = 0.0;
  b[2] = -1.0;
}

void BiQuad :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT BiQuad :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT BiQuad :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT BiQuad :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] + b[1] * inputs[1] + b[2] * inputs[2];
  outputs[0] -= a[2] * outputs[2] + a[1] * outputs[1];
  inputs[2] = inputs[1];
  inputs[1] = inputs[0];
  outputs[2] = outputs[1];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *BiQuad :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


#define __BOTTLE_RADIUS_ 0.999

BlowBotl :: BlowBotl()
{

  m_rate = 1.0;
  jetTable = new JetTabl();

  dcBlock = new PoleZero();
  dcBlock->setBlockZero();

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 5.925 );
  vibratoGain = 0.0;

  resonator = new BiQuad();
  resonator->setResonance(500.0, __BOTTLE_RADIUS_, true);

  adsr = new ADSR();
  adsr->setAllTimes( 0.005, 0.01, 0.8, 0.010);

  noise = new Noise();
  noiseGain = 20.0;

	maxPressure = (MY_FLOAT) 0.0;
	baseFrequency = 0.0; // chuck data
}

BlowBotl :: ~BlowBotl()
{
  delete jetTable;
  delete resonator;
  delete dcBlock;
  delete noise;
  delete adsr;
  delete vibrato;
}

void BlowBotl :: clear()
{
  resonator->clear();
}

void BlowBotl :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): BlowBotl: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  resonator->setResonance( freakency, __BOTTLE_RADIUS_, true );
  baseFrequency = freakency ; // chuck data
}

void BlowBotl :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude;
  adsr->keyOn();
}

void BlowBotl :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void BlowBotl :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BlowBotl: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void BlowBotl :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BlowBotl: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT BlowBotl :: tick()
{
  MY_FLOAT breathPressure;
  MY_FLOAT randPressure;
  MY_FLOAT pressureDiff;

  // Calculate the breath pressure (envelope + vibrato)
  breathPressure = maxPressure * adsr->tick();
  breathPressure += vibratoGain * vibrato->tick();

  pressureDiff = breathPressure - resonator->lastOut();

  randPressure = noiseGain * noise->tick();
  randPressure *= breathPressure;
  randPressure *= (1.0 + pressureDiff);

  resonator->tick( breathPressure + randPressure - ( jetTable->tick( pressureDiff ) * pressureDiff ) );
  lastOutput = 0.2 * outputGain * dcBlock->tick( pressureDiff );

  return lastOutput;
}

void BlowBotl :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): BlowBotl: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): BlowBotl: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_NoiseLevel_) // 4
    noiseGain = norm * 30.0;
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = norm * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): BlowBotl: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BlowBotl: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.
    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

BlowHole :: BlowHole(MY_FLOAT lowestFrequency)
{

   m_rate = 1.0;
   m_frequency = 220.0;
   m_reed = 0.5;
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  // delays[0] is the delay line between the reed and the register vent.
  delays[0] = (DelayL *) new DelayL( 5.0 * Stk::sampleRate() / 22050.0, 100 );
  // delays[1] is the delay line between the register vent and the tonehole.
  delays[1] = (DelayL *) new DelayL( length >> 1, length );
  // delays[2] is the delay line between the tonehole and the end of the bore.
  delays[2] = (DelayL *) new DelayL( 4.0 * Stk::sampleRate() / 22050.0, 100 );
  reedTable = new ReedTabl();
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Calculate the initial tonehole three-port scattering coefficient
  double r_b = 0.0075;    // main bore radius
  r_th = 0.003;          // tonehole radius
  scatter = -pow(r_th,2) / ( pow(r_th,2) + 2*pow(r_b,2) );

  // Calculate tonehole coefficients
  MY_FLOAT te = 1.4 * r_th;    // effective length of the open hole
  th_coeff = (te*2*Stk::sampleRate() - 347.23) / (te*2*Stk::sampleRate() + 347.23);
  tonehole = new PoleZero;
  // Start with tonehole open
  tonehole->setA1(-th_coeff);
  tonehole->setB0(th_coeff);
  tonehole->setB1(-1.0);

  // Calculate register hole filter coefficients
  double r_rh = 0.0015;    // register vent radius
  te = 1.4 * r_rh;       // effective length of the open hole
  double xi = 0.0;         // series resistance term
  double zeta = 347.23 + 2*PI*pow(r_b,2)*xi/1.1769;
  double psi = 2*PI*pow(r_b,2)*te / (PI*pow(r_rh,2));
  rh_coeff = (zeta - 2 * Stk::sampleRate() * psi) / (zeta + 2 * Stk::sampleRate() * psi);
  rh_gain = -347.23 / (zeta + 2 * Stk::sampleRate() * psi);
  vent = new PoleZero;
  vent->setA1(rh_coeff);
  vent->setB0(1.0);
  vent->setB1(1.0);
  // Start with register vent closed
  vent->setGain(0.0);

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.01;
}

BlowHole :: ~BlowHole()
{
  delete delays[0];
  delete delays[1];
  delete delays[2];
  delete reedTable;
  delete filter;
  delete tonehole;
  delete vent;
  delete envelope;
  delete noise;
  delete vibrato;
}

void BlowHole :: clear()
{
  delays[0]->clear();
  delays[1]->clear();
  delays[2]->clear();
  filter->tick((MY_FLOAT) 0.0);
  tonehole->tick((MY_FLOAT) 0.0);
  vent->tick((MY_FLOAT) 0.0);
}

void BlowHole :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): BlowHole: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) * 0.5 - 3.5;
  delay -= delays[0]->getDelay() + delays[2]->getDelay();

  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delays[1]->setDelay(delay);
}

void BlowHole :: setVent(MY_FLOAT newValue)
{
  // This method allows setting of the register vent "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).

  MY_FLOAT gain;
  if (newValue <= 0.0) gain = 0.0;
  else if (newValue >= 1.0) gain = rh_gain;
  else gain = newValue * rh_gain;
  m_vent = newValue;  
  vent->setGain(gain);
}

void BlowHole :: setTonehole(MY_FLOAT newValue)
{
  // This method allows setting of the tonehole "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).
  MY_FLOAT new_coeff;

  if (newValue <= 0.0) new_coeff = 0.9995;
  else if (newValue >= 1.0) new_coeff = th_coeff;
  else new_coeff = (newValue * (th_coeff - 0.9995)) + 0.9995;
  m_tonehole = newValue;
  tonehole->setA1(-new_coeff);
  tonehole->setB0(new_coeff);
}

void BlowHole :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void BlowHole :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void BlowHole :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing((MY_FLOAT) 0.55 + (amplitude * 0.30), amplitude * 0.005);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BlowHole: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void BlowHole :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.01);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BlowHole: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT BlowHole :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  // Calculate the differential pressure = reflected - mouthpiece pressures
  pressureDiff = delays[0]->lastOut() - breathPressure;

  // Do two-port junction scattering for register vent
  MY_FLOAT pa = breathPressure + pressureDiff * reedTable->tick(pressureDiff);
  MY_FLOAT pb = delays[1]->lastOut();
  vent->tick(pa+pb);

  lastOutput = delays[0]->tick(vent->lastOut()+pb);
  lastOutput *= outputGain;

  // Do three-port junction scattering (under tonehole)
  pa += vent->lastOut();
  pb = delays[2]->lastOut();
  MY_FLOAT pth = tonehole->lastOut();
  temp = scatter * (pa + pb - 2 * pth);

  delays[2]->tick(filter->tick(pa + temp) * -0.95);
  delays[1]->tick(pb + temp);
  tonehole->tick(pa + pb - pth + temp);

  return lastOutput;
}

void BlowHole :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): BlowHole: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): BlowHole: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_ReedStiffness_) { // 2 
    m_reed = norm;
    reedTable->setSlope( -0.44 + (0.26 * norm) ); 
   }
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = ( norm * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    this->setTonehole( norm );
  else if (number == __SK_ModWheel_) // 1
    this->setVent( norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope->setValue( norm );
  else
    std::cerr << "[chuck](via STK): BlowHole: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): BlowHole: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class BowTabl
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith (1986).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

BowTabl :: BowTabl()
{
  offSet = (MY_FLOAT) 0.0;
  slope = (MY_FLOAT) 0.1;
}

BowTabl :: ~BowTabl()
{
}

void BowTabl :: setOffset(MY_FLOAT aValue)
{
  offSet = aValue;
}

void BowTabl :: setSlope(MY_FLOAT aValue)
{
  slope = aValue;
}

MY_FLOAT BowTabl :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT BowTabl :: tick(MY_FLOAT input)
{
  // The input represents differential string vs. bow velocity.
  MY_FLOAT sample;
  sample = input + offSet;  // add bias to input
  sample *= slope;          // then scale it
  lastOutput = (MY_FLOAT)  fabs((double) sample) + (MY_FLOAT) 0.75;
  lastOutput = (MY_FLOAT)  pow( lastOutput,(MY_FLOAT) -4.0 );

  // Set minimum friction to 0.0
  //if (lastOutput < 0.0 ) lastOutput = 0.0;
  // Set maximum friction to 1.0.
  if (lastOutput > 1.0 ) lastOutput = (MY_FLOAT) 1.0;

  return lastOutput;
}

MY_FLOAT *BowTabl :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Bowed :: Bowed(MY_FLOAT lowestFrequency)
{
  long length;
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  neckDelay = new DelayL(100.0, length);
  length >>= 1;
  bridgeDelay = new DelayL(29.0, length);

  bowTable = new BowTabl;
  bowTable->setSlope((MY_FLOAT) 3.0);

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 6.12723);
  vibratoGain = (MY_FLOAT) 0.0;

  stringFilter = new OnePole;
  stringFilter->setPole((MY_FLOAT) (0.6 - (0.1 * 22050.0 / Stk::sampleRate() ) ) );
  stringFilter->setGain((MY_FLOAT) 0.95);

  bodyFilter = new BiQuad;
  bodyFilter->setResonance( 500.0, 0.85, TRUE );
  bodyFilter->setGain((MY_FLOAT) 0.2);

  adsr = new ADSR;
  adsr->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.005,(MY_FLOAT) 0.9,(MY_FLOAT) 0.01);
    
  betaRatio = (MY_FLOAT) 0.127236;

  // Necessary to initialize internal variables.
  setFrequency( 220.0 );
}

Bowed :: ~Bowed()
{
  delete neckDelay;
  delete bridgeDelay;
  delete bowTable;
  delete stringFilter;
  delete bodyFilter;
  delete vibrato;
  delete adsr;
}

void Bowed :: clear()
{
  neckDelay->clear();
  bridgeDelay->clear();
}

void Bowed :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Bowed: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }
  m_frequency = freakency;

  // Delay = length - approximate filter delay.
  baseDelay = Stk::sampleRate() / freakency - (MY_FLOAT) 4.0;
  if ( baseDelay <= 0.0 ) baseDelay = 0.3;
  bridgeDelay->setDelay(baseDelay * betaRatio); 	               // bow to bridge length
  neckDelay->setDelay(baseDelay * ((MY_FLOAT) 1.0 - betaRatio)); // bow to nut (finger) length
}

void Bowed :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = (MY_FLOAT) 0.03 + ((MY_FLOAT) 0.2 * amplitude); 
}

void Bowed :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void Bowed :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->startBowing(amplitude, amplitude * 0.001);
  this->setFrequency(frequency);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Bowed: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Bowed :: noteOff(MY_FLOAT amplitude)
{
  this->stopBowing(((MY_FLOAT) 1.0 - amplitude) * (MY_FLOAT) 0.005);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Bowed: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void Bowed :: setVibrato(MY_FLOAT gain)
{
  vibratoGain = gain;
}

MY_FLOAT Bowed :: tick()
{
  MY_FLOAT bowVelocity;
  MY_FLOAT bridgeRefl;
  MY_FLOAT nutRefl;
  MY_FLOAT newVel;
  MY_FLOAT velDiff;
  MY_FLOAT stringVel;
    
  bowVelocity = maxVelocity * adsr->tick();

  bridgeRefl = -stringFilter->tick( bridgeDelay->lastOut() );
  nutRefl = -neckDelay->lastOut();
  stringVel = bridgeRefl + nutRefl;               // Sum is String Velocity
  velDiff = bowVelocity - stringVel;              // Differential Velocity
  newVel = velDiff * bowTable->tick( velDiff );   // Non-Linear Bow Function
  neckDelay->tick(bridgeRefl + newVel);           // Do string propagations
  bridgeDelay->tick(nutRefl + newVel);
    
  if (vibratoGain > 0.0)  {
    neckDelay->setDelay((baseDelay * ((MY_FLOAT) 1.0 - betaRatio)) + 
                        (baseDelay * vibratoGain * vibrato->tick()));
  }

  lastOutput = bodyFilter->tick(bridgeDelay->lastOut());                 

  return lastOutput;
}

void Bowed :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Bowed: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Bowed: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_BowPressure_) // 2
		bowTable->setSlope( 5.0 - (4.0 * norm) );
  else if (number == __SK_BowPosition_) { // 4
		betaRatio = 0.027236 + (0.2 * norm);
    bridgeDelay->setDelay(baseDelay * betaRatio);
    neckDelay->setDelay(baseDelay * ((MY_FLOAT) 1.0 - betaRatio));
  }
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = ( norm * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget(norm);
  else
    std::cerr << "[chuck](via STK): Bowed: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Bowed: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Brass :: Brass(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayA( 0.5 * length, length );

  lipFilter = new BiQuad();
  lipFilter->setGain( 0.03 );
  dcBlock = new PoleZero();
  dcBlock->setBlockZero();

  adsr = new ADSR;
  adsr->setAllTimes( 0.005, 0.001, 1.0, 0.010);

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 6.137 );
  vibratoGain = 0.0;

  this->clear();
	maxPressure = (MY_FLOAT) 0.0;
  lipTarget = 0.0;

  // Necessary to initialize variables.
  setFrequency( 220.0 );
}

Brass :: ~Brass()
{
  delete delayLine;
  delete lipFilter;
  delete dcBlock;
  delete adsr;
  delete vibrato;
}

void Brass :: clear()
{
  delayLine->clear();
  lipFilter->clear();
  dcBlock->clear();
}

void Brass :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Brass: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  // Fudge correction for filter delays.
  slideTarget = (Stk::sampleRate() / freakency * 2.0) + 3.0;
  delayLine->setDelay(slideTarget); // play a harmonic

  lipTarget = freakency;
  lipFilter->setResonance( freakency, 0.997 );
}

void Brass :: setLip(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Brass: setLip parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  lipFilter->setResonance( freakency, 0.997 );
}

void Brass :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude;
  adsr->keyOn();
}

void Brass :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void Brass :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  this->startBlowing(amplitude, amplitude * 0.001);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Brass: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Brass :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.005);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Brass: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Brass :: tick()
{
  MY_FLOAT breathPressure = maxPressure * adsr->tick();
  breathPressure += vibratoGain * vibrato->tick();

  MY_FLOAT mouthPressure = 0.3 * breathPressure;
  MY_FLOAT borePressure = 0.85 * delayLine->lastOut();
  MY_FLOAT deltaPressure = mouthPressure - borePressure; // Differential pressure.
  deltaPressure = lipFilter->tick( deltaPressure );      // Force - > position.
  deltaPressure *= deltaPressure;                        // Basic position to area mapping.
  if ( deltaPressure > 1.0 ) deltaPressure = 1.0;         // Non-linear saturation.
  // The following input scattering assumes the mouthPressure = area.
  lastOutput = deltaPressure * mouthPressure + ( 1.0 - deltaPressure) * borePressure;
  lastOutput = delayLine->tick( dcBlock->tick( lastOutput ) );

  return lastOutput;
}

void Brass :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Brass: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Brass: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_LipTension_)	{ // 2
    MY_FLOAT temp = lipTarget * pow( 4.0, (2.0 * norm) - 1.0 );
    this->setLip(temp);
  }
  else if (number == __SK_SlideLength_) // 4
    delayLine->setDelay( slideTarget * (0.5 + norm) );
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_ ) // 1
    vibratoGain = norm * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): Brass: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Brass: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

Chorus :: Chorus(MY_FLOAT baseDelay)
{
  delayLine[0] = new DelayL((long) baseDelay, (long) (baseDelay * 1.414) + 2);
  delayLine[1] = new DelayL((long) (baseDelay), (long) baseDelay + 2);
  baseLength = baseDelay;

  // Concatenate the STK rawwave path to the rawwave file
  mods[0] = new WaveLoop( "special:sinewave", TRUE );
  mods[1] = new WaveLoop( "special:sinewave", TRUE );
  mods[0]->setFrequency(0.2);
  mods[1]->setFrequency(0.222222);
  modDepth = 0.05;
  effectMix = 0.5;
  this->clear();
}

Chorus :: ~Chorus()
{
  delete delayLine[0];
  delete delayLine[1];
  delete mods[0];
  delete mods[1];
}

void Chorus :: clear()
{
  delayLine[0]->clear();
  delayLine[1]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

void Chorus :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    std::cerr << "[chuck](via STK): Chorus: setEffectMix parameter is less than zero!" << std::endl;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    std::cerr << "[chuck](via STK): Chorus: setEffectMix parameter is greater than 1.0!" << std::endl;
    effectMix = 1.0;
  }
}

void Chorus :: setModDepth(MY_FLOAT depth)
{
  modDepth = depth;
}

void Chorus :: setModFrequency(MY_FLOAT frequency)
{
  mods[0]->setFrequency(frequency);
  mods[1]->setFrequency(frequency * 1.1111);
}

MY_FLOAT Chorus :: lastOut() const
{
  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
}

MY_FLOAT Chorus :: lastOutLeft() const
{
  return lastOutput[0];
}

MY_FLOAT Chorus :: lastOutRight() const
{
  return lastOutput[1];
}

MY_FLOAT Chorus :: tick(MY_FLOAT input)
{
  delayLine[0]->setDelay(baseLength * 0.707 * (1.0 + mods[0]->tick()));
  delayLine[1]->setDelay(baseLength  * 0.5 *  (1.0 - mods[1]->tick()));
  lastOutput[0] = input * (1.0 - effectMix);
  lastOutput[0] += effectMix * delayLine[0]->tick(input);
  lastOutput[1] = input * (1.0 - effectMix);
  lastOutput[1] += effectMix * delayLine[1]->tick(input);
  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
}

MY_FLOAT *Chorus :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Clarinet :: Clarinet(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayL( (MY_FLOAT)(length / 2.0), length);
  reedTable = new ReedTabl();
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.1;
}

Clarinet :: ~Clarinet()
{
  delete delayLine;
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibrato;
}

void Clarinet :: clear()
{
  delayLine->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Clarinet :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Clarinet: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) * 0.5 - 1.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);
}

void Clarinet :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Clarinet :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void Clarinet :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->startBlowing((MY_FLOAT) 0.55 + (amplitude * (MY_FLOAT) 0.30), amplitude * (MY_FLOAT) 0.005);
  outputGain = amplitude + (MY_FLOAT) 0.001;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Clarinet: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Clarinet :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * (MY_FLOAT) 0.01);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Clarinet: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Clarinet :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  // Perform commuted loss filtering.
  pressureDiff = -0.95 * filter->tick(delayLine->lastOut());

  // Calculate pressure difference of reflected and mouthpiece pressures.
  pressureDiff = pressureDiff - breathPressure;

  // Perform non-linear scattering using pressure difference in reed function.
  lastOutput = delayLine->tick(breathPressure + pressureDiff * reedTable->tick(pressureDiff));

  // Apply output gain.
  lastOutput *= outputGain;

  return lastOutput;
}

void Clarinet :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Clarinet: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Clarinet: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_ReedStiffness_) // 2
    reedTable->setSlope((MY_FLOAT) -0.44 + ( (MY_FLOAT) 0.26 * norm ));
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = (norm * (MY_FLOAT) 0.4);
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency((norm * (MY_FLOAT) 12.0));
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = (norm * (MY_FLOAT) 0.5);
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope->setValue(norm);
  else
    std::cerr << "[chuck](via STK): Clarinet: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Clarinet: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This protected Filter subclass implements
    a non-interpolating digital delay-line.
    A fixed maximum length of 4095 and a delay
    of zero is set using the default constructor.
    Alternatively, the delay and maximum length
    can be set during instantiation with an
    overloaded constructor.
    
    A non-interpolating delay line is typically
    used in fixed delay-length applications, such
    as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

Delay :: Delay()
{
    this->set( 0, 4096 );
}

Delay :: Delay(long theDelay, long maxDelay)
{
    this->set( theDelay, maxDelay );
}

void Delay :: set( long delay, long max )
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    length = max+1;

    // We need to delete the previously allocated inputs.
    if( inputs ) delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();

    inPoint = 0;
    this->setDelay(delay);
}

Delay :: ~Delay()
{
}

void Delay :: clear(void)
{
  long i;
  for (i=0;i<length;i++) inputs[i] = 0.0;
  outputs[0] = 0.0;
}

void Delay :: setDelay(long theDelay)
{
  if (theDelay > length-1) { // The value is too big.
    std::cerr << "[chuck](via STK): Delay: setDelay(" << theDelay << ") too big!" << std::endl;
    // Force delay to maxLength.
    outPoint = inPoint + 1;
    delay = length - 1;
  }
  else if (theDelay < 0 ) {
    std::cerr << "[chuck](via STK): Delay: setDelay(" << theDelay << ") less than zero!" << std::endl;
    outPoint = inPoint;
    delay = 0;
  }
  else {
    outPoint = inPoint - (long) theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPoint < 0) outPoint += length;  // modulo maximum length
}

MY_FLOAT Delay :: getDelay(void) const
{
  return delay;
}

MY_FLOAT Delay :: energy(void) const
{
  int i;
  register MY_FLOAT e = 0;
  if (inPoint >= outPoint) {
    for (i=outPoint; i<inPoint; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  } else {
    for (i=outPoint; i<length; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
    for (i=0; i<inPoint; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  }
  return e;
}

MY_FLOAT Delay :: contentsAt(unsigned long tapDelay) const
{
  long i = tapDelay;
  if (i < 1) {
    std::cerr << "[chuck](via STK): Delay: contentsAt(" << tapDelay << ") too small!" << std::endl;
    i = 1;
  }
  else if (i > delay) {
    std::cerr << "[chuck](via STK): Delay: contentsAt(" << tapDelay << ") too big!" << std::endl;
    i = (long) delay;
  }

  long tap = inPoint - i;
  if (tap < 0) // Check for wraparound.
    tap += length;

  return inputs[tap];
}

MY_FLOAT Delay :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT Delay :: nextOut(void) const
{
  return inputs[outPoint];
}

MY_FLOAT Delay :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Check for end condition
  if (inPoint == length)
    inPoint -= length;

  // Read out next value
  outputs[0] = inputs[outPoint++];

  if (outPoint>=length)
    outPoint -= length;

  return outputs[0];
}

MY_FLOAT *Delay :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using a first-order
    allpass filter.  A fixed maximum length
    of 4095 and a delay of 0.5 is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    An allpass filter has unity magnitude gain but
    variable phase delay properties, making it useful
    in achieving fractional delays without affecting
    a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay
    response, the minimum delay possible in this
    implementation is limited to a value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

DelayA :: DelayA()
{
  this->setDelay( 0.5 );
  apInput = 0.0;
  doNextOut = true;
}

DelayA :: DelayA(MY_FLOAT theDelay, long maxDelay)
{
  // Writing before reading allows delays from 0 to length-1. 
  length = maxDelay+1;

  if ( length > 4096 ) {
    // We need to delete the previously allocated inputs.
    delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();
  }

  inPoint = 0;
  this->setDelay(theDelay);
  apInput = 0.0;
  doNextOut = true;
}

void DelayA :: set( MY_FLOAT delay, long max )
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    length = max+1;

    // We need to delete the previously allocated inputs.
    if( inputs ) delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();

    inPoint = 0;
    this->setDelay(delay);
    apInput = 0.0;
    doNextOut = true;
}

DelayA :: ~DelayA()
{
}

void DelayA :: clear()
{
  Delay::clear();
  apInput = 0.0;
}

void DelayA :: setDelay(MY_FLOAT theDelay)  
{
  MY_FLOAT outPointer;

  if (theDelay > length-1) {
    std::cerr << "[chuck](via STK): DelayA: setDelay(" << theDelay << ") too big!" << std::endl;
    // Force delay to maxLength
    outPointer = inPoint + 1.0;
    delay = length - 1;
  }
  else if (theDelay < 0.5) {
    std::cerr << "[chuck](via STK): DelayA: setDelay(" << theDelay << ") less than 0.5 not possible!" << std::endl;
    outPointer = inPoint + 0.4999999999;
    delay = 0.5;
  }
  else {
    outPointer = inPoint - theDelay + 1.0;     // outPoint chases inpoint
    delay = theDelay;
  }

  if (outPointer < 0)
    outPointer += length;  // modulo maximum length

  outPoint = (long) outPointer;        // integer part
  alpha = 1.0 + outPoint - outPointer; // fractional part

  if (alpha < 0.5) {
    // The optimal range for alpha is about 0.5 - 1.5 in order to
    // achieve the flattest phase delay response.
    outPoint += 1;
    if (outPoint >= length) outPoint -= length;
    alpha += (MY_FLOAT) 1.0;
  }

  coeff = ((MY_FLOAT) 1.0 - alpha) / 
    ((MY_FLOAT) 1.0 + alpha);         // coefficient for all pass
}

MY_FLOAT DelayA :: nextOut(void)
{
  if ( doNextOut ) {
    // Do allpass interpolation delay.
    nextOutput = -coeff * outputs[0];
    nextOutput += apInput + (coeff * inputs[outPoint]);
    doNextOut = false;
  }

  return nextOutput;
}

MY_FLOAT DelayA :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Increment input pointer modulo length.
  if (inPoint == length)
    inPoint -= length;

  outputs[0] = nextOut();
  doNextOut = true;

  // Save the allpass input and increment modulo length.
  apInput = inputs[outPoint++];
  if (outPoint == length)
    outPoint -= length;

  return outputs[0];
}
/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using first-order
    linear interpolation.  A fixed maximum length
    of 4095 and a delay of zero is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    Linear interpolation is an efficient technique
    for achieving fractional delay lengths, though
    it does introduce high-frequency signal
    attenuation to varying degrees depending on the
    fractional delay setting.  The use of higher
    order Lagrange interpolators can typically
    improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

DelayL :: DelayL()
{
    doNextOut = true;
}

DelayL :: DelayL(MY_FLOAT theDelay, long maxDelay)
{
  // Writing before reading allows delays from 0 to length-1. 
  length = maxDelay+1;

  if ( length > 4096 ) {
    // We need to delete the previously allocated inputs.
    delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();
  }

  inPoint = 0;
  this->setDelay(theDelay);
  doNextOut = true;
}

DelayL :: ~DelayL()
{
}

void DelayL :: set( MY_FLOAT delay, long max )
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    length = max+1;

    // We need to delete the previously allocated inputs.
    if( inputs ) delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();

    inPoint = 0;
    this->setDelay(delay);
    doNextOut = true;
}

void DelayL :: setDelay(MY_FLOAT theDelay)
{
  MY_FLOAT outPointer;

  if (theDelay > length-1) {
    std::cerr << "[chuck](via STK): DelayL: setDelay(" << theDelay << ") too big!" << std::endl;
    // Force delay to maxLength
    outPointer = inPoint + 1.0;
    delay = length - 1;
  }
  else if (theDelay < 0 ) {
    std::cerr << "[chuck](via STK): DelayL: setDelay(" << theDelay << ") less than zero!" << std::endl;
    outPointer = inPoint;
    delay = 0;
  }
  else {
    outPointer = inPoint - theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPointer < 0)
    outPointer += length; // modulo maximum length

  outPoint = (long) outPointer;  // integer part
  alpha = outPointer - outPoint; // fractional part
  omAlpha = (MY_FLOAT) 1.0 - alpha;
}

MY_FLOAT DelayL :: nextOut(void)
{
  if ( doNextOut ) {
    // First 1/2 of interpolation
    nextOutput = inputs[outPoint] * omAlpha;
    // Second 1/2 of interpolation
    if (outPoint+1 < length)
      nextOutput += inputs[outPoint+1] * alpha;
    else
      nextOutput += inputs[0] * alpha;
    doNextOut = false;
  }

  return nextOutput;
}

MY_FLOAT DelayL :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Increment input pointer modulo length.
  if (inPoint == length)
    inPoint -= length;

  outputs[0] = nextOut();
  doNextOut = true;

  // Increment output pointer modulo length.
  if (++outPoint >= length)
    outPoint -= length;

  return outputs[0];
}
/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

// Not really General MIDI yet.  Coming soon.
unsigned char genMIDIMap[128] =
  { 0,0,0,0,0,0,0,0,		// 0-7
    0,0,0,0,0,0,0,0,		// 8-15
    0,0,0,0,0,0,0,0,		// 16-23
    0,0,0,0,0,0,0,0,		// 24-31
    0,0,0,0,1,0,2,0,		// 32-39
    2,3,6,3,6,4,7,4,		// 40-47
    5,8,5,0,0,0,10,0,		// 48-55
    9,0,0,0,0,0,0,0,		// 56-63
    0,0,0,0,0,0,0,0,		// 64-71
    0,0,0,0,0,0,0,0,		// 72-79
    0,0,0,0,0,0,0,0,		// 80-87
    0,0,0,0,0,0,0,0,		// 88-95
    0,0,0,0,0,0,0,0,		// 96-103
    0,0,0,0,0,0,0,0,		// 104-111
    0,0,0,0,0,0,0,0,		// 112-119
    0,0,0,0,0,0,0,0     // 120-127
  };
 
//XXX changed this from 16 to 32 for the 'special' convention..also, we do not have these linked
//in the headers			  
char waveNames[DRUM_NUMWAVES][32] =
  { 
    "special:dope",
    "special:bassdrum",
    "special:snardrum",
    "special:tomlowdr",
    "special:tommiddr",
    "special:tomhidrm",
    "special:hihatcym",
    "special:ridecymb",
    "special:crashcym", 
    "special:cowbell1", 
    "special:tambourn"
  };

Drummer :: Drummer() : Instrmnt()
{
  for (int i=0; i<DRUM_POLYPHONY; i++)   {
    filters[i] = new OnePole;
    sounding[i] = -1;
  }

  // This counts the number of sounding voices.
  nSounding = 0;
}

Drummer :: ~Drummer()
{
  int i;
  for ( i=0; i<nSounding-1; i++ ) delete waves[i];
  for ( i=0; i<DRUM_POLYPHONY; i++ ) delete filters[i];
}

void Drummer :: noteOn(MY_FLOAT instrument, MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Drummer: NoteOn instrument = " << instrument << ", amplitude = " << amplitude << std::endl;
#endif

  MY_FLOAT gain = amplitude;
  if ( amplitude > 1.0 ) {
    std::cerr << "[chuck](via STK): Drummer: noteOn amplitude parameter is greater than 1.0!" << std::endl;
    gain = 1.0;
  }
  else if ( amplitude < 0.0 ) {
    std::cerr << "[chuck](via STK): Drummer: noteOn amplitude parameter is less than 0.0!" << std::endl;
    return;
  }

  // Yes, this is tres kludgey.
  int noteNum = (int) ((12*log(instrument/220.0)/log(2.0)) + 57.01);

  // Check first to see if there's already one like this sounding.
  int i, waveIndex = -1;
  for (i=0; i<DRUM_POLYPHONY; i++) {
    if (sounding[i] == noteNum) waveIndex = i;
  }

  if ( waveIndex >= 0 ) {
    // Reset this sound.
    waves[waveIndex]->reset();
    filters[waveIndex]->setPole((MY_FLOAT) 0.999 - (gain * 0.6));
    filters[waveIndex]->setGain(gain);
  }
  else {
    if (nSounding == DRUM_POLYPHONY) {
      // If we're already at maximum polyphony, then preempt the oldest voice.
      delete waves[0];
      filters[0]->clear();
      WvIn *tempWv = waves[0];
      OnePole *tempFilt = filters[0];
      // Re-order the list.
      for (i=0; i<DRUM_POLYPHONY-1; i++) {
        waves[i] = waves[i+1];
        filters[i] = filters[i+1];
      }
      waves[DRUM_POLYPHONY-1] = tempWv;
      filters[DRUM_POLYPHONY-1] = tempFilt;
    }
    else
      nSounding += 1;

    sounding[nSounding-1] = noteNum;
    // Concatenate the STK rawwave path to the rawwave file
    waves[nSounding-1] = new WvIn( (Stk::rawwavePath() + waveNames[genMIDIMap[noteNum]]).c_str(), TRUE );
    if (Stk::sampleRate() != 22050.0)
      waves[nSounding-1]->setRate( 22050.0 / Stk::sampleRate() );
    filters[nSounding-1]->setPole((MY_FLOAT) 0.999 - (gain * 0.6) );
    filters[nSounding-1]->setGain( gain );
  }

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Number Sounding = " << nSounding << std::endl;
  for (i=0; i<nSounding; i++) std::cerr << sounding[i] << "  ";
  std::cerr << "[chuck](via STK): \n";
#endif
}

void Drummer :: noteOff(MY_FLOAT amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while(i < nSounding) {
    filters[i++]->setGain( amplitude * 0.01 );
  }
}

MY_FLOAT Drummer :: tick()
{
  MY_FLOAT output = 0.0;
  OnePole *tempFilt;

  int j, i = 0;
  while (i < nSounding) {
    if ( waves[i]->isFinished() ) {
      delete waves[i];
	    tempFilt = filters[i];
      // Re-order the list.
      for (j=i; j<nSounding-1; j++) {
        sounding[j] = sounding[j+1];
        waves[j] = waves[j+1];
        filters[j] = filters[j+1];
      }
      filters[j] = tempFilt;
      filters[j]->clear();
      sounding[j] = -1;
      nSounding -= 1;
      i -= 1;
    }
    else
      output += filters[i]->tick( waves[i]->tick() );
    i++;
  }

  return output;
}
/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements a echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

Echo :: Echo(MY_FLOAT longestDelay)
{
    delayLine = NULL;
    this->set( longestDelay );
    effectMix = 0.5;
}

Echo :: ~Echo()
{
    delete delayLine;
}

void Echo :: set( MY_FLOAT max )
{
    length = (long)max + 2;
    MY_FLOAT delay = delayLine ? delayLine->getDelay() : length>>1;
    if( delayLine ) delete delayLine;
    delayLine = new Delay(length>>1, length);
    this->clear();
    this->setDelay(delay+.5);
}

MY_FLOAT Echo :: getDelay()
{
    return delayLine->getDelay();
}

void Echo :: clear()
{
  delayLine->clear();
  lastOutput = 0.0;
}

void Echo :: setDelay(MY_FLOAT delay)
{
  MY_FLOAT size = delay;
  if ( delay < 0.0 ) {
    std::cerr << "[chuck](via STK): Echo: setDelay parameter is less than zero!" << std::endl;
    size = 0.0;
  }
  else if ( delay > length ) {
    std::cerr << "[chuck](via STK): Echo: setDelay parameter is greater than delay length!" << std::endl;
    size = length;
  }

  delayLine->setDelay((long)size);
}

void Echo :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    std::cerr << "[chuck](via STK): Echo: setEffectMix parameter is less than zero!" << std::endl;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    std::cerr << "[chuck](via STK): Echo: setEffectMix parameter is greater than 1.0!" << std::endl;
    effectMix = 1.0;
  }
}

MY_FLOAT Echo :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Echo :: tick(MY_FLOAT input)
{
  lastOutput = effectMix * delayLine->tick(input);
  lastOutput += input * (1.0 - effectMix);
  return lastOutput;
}

MY_FLOAT *Echo :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class Envelope
    \brief STK envelope base class.

    This class implements a simple envelope
    generator which is capable of ramping to
    a target value by a specified \e rate.
    It also responds to simple \e keyOn and
    \e keyOff messages, ramping to 1.0 on
    keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>

Envelope :: Envelope(void) : Stk()
{    
  target = (MY_FLOAT) 0.0;
  value = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 0.001;
  state = 0;
}

Envelope :: ~Envelope(void)
{    
}

void Envelope :: keyOn(void)
{
  target = (MY_FLOAT) 1.0;
  if (value != target) state = 1;
}

void Envelope :: keyOff(void)
{
  target = (MY_FLOAT) 0.0;
  if (value != target) state = 1;
}

void Envelope :: setRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via Envelope): negative rates not allowed ... correcting!\n");
    rate = -aRate;
  }
  else
    rate = aRate;
}

void Envelope :: setTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via Envelope): negative times not allowed ... correcting!\n");
    rate = 1.0 / (-aTime * Stk::sampleRate());
  }
  else
    rate = 1.0 / (aTime * Stk::sampleRate());
}

void Envelope :: setTarget(MY_FLOAT aTarget)
{
  target = aTarget;
  if (value != target) state = 1;
}

void Envelope :: setValue(MY_FLOAT aValue)
{
  state = 0;
  target = aValue;
  value = aValue;
}

int Envelope :: getState(void) const
{
  return state;
}

MY_FLOAT Envelope :: tick(void)
{
  if (state) {
    if (target > value) {
      value += rate;
      if (value >= target) {
        value = target;
        state = 0;
      }
    }
    else {
      value -= rate;
      if (value <= target) {
        value = target;
        state = 0;
      }
    }
  }
  return value;
}

MY_FLOAT *Envelope :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Envelope :: lastOut(void) const
{
  return value;
}

/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>

FM :: FM(int operators)
  : nOperators(operators)
{
  if ( nOperators <= 0 ) {
    char msg[256];
    sprintf(msg, "[chuck](via FM): Invalid number of operators (%d) argument to constructor!", operators);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  twozero = new TwoZero();
  twozero->setB2( -1.0 );
  twozero->setGain( 0.0 );

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency(6.0);

  int i;
  ratios = (MY_FLOAT *) new MY_FLOAT[nOperators];
  gains = (MY_FLOAT *) new MY_FLOAT[nOperators];
  adsr = (ADSR **) calloc( nOperators, sizeof(ADSR *) );
  waves = (WaveLoop **) calloc( nOperators, sizeof(WaveLoop *) );
  for (i=0; i<nOperators; i++ ) {
    ratios[i] = 1.0;
    gains[i] = 1.0;
    adsr[i] = new ADSR();
  }

  modDepth = (MY_FLOAT) 0.0;
  control1 = (MY_FLOAT) 1.0;
  control2 = (MY_FLOAT) 1.0;
  baseFrequency = (MY_FLOAT) 440.0;

  MY_FLOAT temp = 1.0;
  for (i=99; i>=0; i--) {
    __FM_gains[i] = temp;
    temp *= 0.933033;
  }

  temp = 1.0;
  for (i=15; i>=0; i--) {
    __FM_susLevels[i] = temp;
    temp *= 0.707101;
  }

  temp = 8.498186;
  for (i=0; i<32; i++) {
    __FM_attTimes[i] = temp;
    temp *= 0.707101;
  }
}

FM :: ~FM()
{
  delete vibrato;
  delete twozero;

  delete [] ratios;
  delete [] gains;
  for (int i=0; i<nOperators; i++ ) {
    delete adsr[i];
    delete waves[i];
  }

  free(adsr);
  free(waves);
}

void FM :: loadWaves(const char **filenames )
{
  for (int i=0; i<nOperators; i++ )
    waves[i] = new WaveLoop( filenames[i], TRUE );
}

void FM :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;

  for (int i=0; i<nOperators; i++ )
    waves[i]->setFrequency( baseFrequency * ratios[i] );
}

void FM :: setRatio(int waveIndex, MY_FLOAT ratio)
{
  if ( waveIndex < 0 ) {
    std::cerr << "[chuck](via STK): FM: setRatio waveIndex parameter is less than zero!" << std::endl;
    return;
  }
  else if ( waveIndex >= nOperators ) {
    std::cerr << "[chuck](via STK): FM: setRatio waveIndex parameter is greater than the number of operators!" << std::endl;
    return;
  }

  ratios[waveIndex] = ratio;
  if (ratio > 0.0) 
    waves[waveIndex]->setFrequency(baseFrequency * ratio);
  else
    waves[waveIndex]->setFrequency(ratio);
}

void FM :: setGain(int waveIndex, MY_FLOAT gain)
{
  if ( waveIndex < 0 ) {
    std::cerr << "[chuck](via STK): FM: setGain waveIndex parameter is less than zero!" << std::endl;
    return;
  }
  else if ( waveIndex >= nOperators ) {
    std::cerr << "[chuck](via STK): FM: setGain waveIndex parameter is greater than the number of operators!" << std::endl;
    return;
  }

  gains[waveIndex] = gain;
}

void FM :: setModulationSpeed(MY_FLOAT mSpeed)
{
  vibrato->setFrequency(mSpeed);
}

void FM :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth;
}

void FM :: setControl1(MY_FLOAT cVal)
{
  control1 = cVal * (MY_FLOAT) 2.0;
}

void FM :: setControl2(MY_FLOAT cVal)
{
  control2 = cVal * (MY_FLOAT) 2.0;
}

void FM :: keyOn()
{
  for (int i=0; i<nOperators; i++ )
    adsr[i]->keyOn();
}

void FM :: keyOff()
{
  for (int i=0; i<nOperators; i++ )
    adsr[i]->keyOff();
}

void FM :: noteOff(MY_FLOAT amplitude)
{
  keyOff();

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): FM: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void FM :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): FM: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): FM: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_Breath_) // 2
    setControl1( norm );
  else if (number == __SK_FootControl_) // 4
    setControl2( norm );
  else if (number == __SK_ModFrequency_) // 11
    setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    //adsr[0]->setTarget( norm );
    adsr[1]->setTarget( norm );
    //adsr[2]->setTarget( norm );
    adsr[3]->setTarget( norm );
  }
  else
    std::cerr << "[chuck](via STK): FM: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): FM: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}

/***************************************************/
/*! \class FMVoices
    \brief STK singing FM synthesis instrument.

    This class implements 3 carriers and a common
    modulator, also referred to as algorithm 6 of
    the TX81Z.

    \code
    Algorithm 6 is :
                        /->1 -\
                     4-|-->2 - +-> Out
                        \->3 -/
    \endcode

    Control Change Numbers: 
       - Vowel = 2
       - Spectral Tilt = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


FMVoices :: FMVoices()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 2.00);
  this->setRatio(1, 4.00);
  this->setRatio(2, 12.0);
  this->setRatio(3, 1.00);

  gains[3] = __FM_gains[80];

  adsr[0]->setAllTimes( 0.05, 0.05, __FM_susLevels[15], 0.05);
  adsr[1]->setAllTimes( 0.05, 0.05, __FM_susLevels[15], 0.05);
  adsr[2]->setAllTimes( 0.05, 0.05, __FM_susLevels[15], 0.05);
  adsr[3]->setAllTimes( 0.01, 0.01, __FM_susLevels[15], 0.5);

  twozero->setGain( 0.0 );
  modDepth = (MY_FLOAT) 0.005;
  currentVowel = 0;
  tilt[0] = 1.0;
  tilt[1] = 0.5;
  tilt[2] = 0.2;    
  mods[0] = 1.0;
  mods[1] = 1.1;
  mods[2] = 1.1;
  baseFrequency = 110.0;
  setFrequency( 110.0 );    
}  

FMVoices :: ~FMVoices()
{
}

void FMVoices :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT temp, temp2 = 0.0;
  int tempi = 0;
  unsigned int i = 0;

  if (currentVowel < 32)	{
    i = currentVowel;
    temp2 = (MY_FLOAT) 0.9;
  }
  else if (currentVowel < 64)	{
    i = currentVowel - 32;
    temp2 = (MY_FLOAT) 1.0;
  }
  else if (currentVowel < 96)	{
    i = currentVowel - 64;
    temp2 = (MY_FLOAT) 1.1;
  }
  else if (currentVowel <= 128)	{
    i = currentVowel - 96;
    temp2 = (MY_FLOAT) 1.2;
  }

  baseFrequency = frequency;
  temp = (temp2 * Phonemes::formantFrequency(i, 0) / baseFrequency) + 0.5;
  tempi = (int) temp;
  this->setRatio(0,(MY_FLOAT) tempi);
  temp = (temp2 * Phonemes::formantFrequency(i, 1) / baseFrequency) + 0.5;
  tempi = (int) temp;
  this->setRatio(1,(MY_FLOAT) tempi);
  temp = (temp2 * Phonemes::formantFrequency(i, 2) / baseFrequency) + 0.5;
  tempi = (int) temp;
  this->setRatio(2, (MY_FLOAT) tempi);    
  gains[0] = 1.0;
  gains[1] = 1.0;
  gains[2] = 1.0;
}

void FMVoices :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  tilt[0] = amplitude;
  tilt[1] = amplitude * amplitude;
  tilt[2] = tilt[1] * amplitude;
  this->keyOn();

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): FMVoices: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT FMVoices :: tick()
{
  register MY_FLOAT temp, temp2;

  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  temp2 = vibrato->tick() * modDepth * (MY_FLOAT) 0.1;

  waves[0]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[3]);

  waves[0]->addPhaseOffset(temp * mods[0]);
  waves[1]->addPhaseOffset(temp * mods[1]);
  waves[2]->addPhaseOffset(temp * mods[2]);
  waves[3]->addPhaseOffset(twozero->lastOut());
  twozero->tick(temp);
  temp =  gains[0] * tilt[0] * adsr[0]->tick() * waves[0]->tick();
  temp += gains[1] * tilt[1] * adsr[1]->tick() * waves[1]->tick();
  temp += gains[2] * tilt[2] * adsr[2]->tick() * waves[2]->tick();
    
  return temp * 0.33;
}

void FMVoices :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): FMVoices: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): FMVoices: Control value greater than 128.0!" << std::endl;
  }


  if (number == __SK_Breath_) // 2
    gains[3] = __FM_gains[(int) ( norm * 99.9 )];
  else if (number == __SK_FootControl_)	{ // 4
    currentVowel = (int) (norm * 128.0);
    this->setFrequency(baseFrequency);
  }
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    tilt[0] = norm;
    tilt[1] = norm * norm;
    tilt[2] = tilt[1] * norm;
  }
  else
    std::cerr << "[chuck](via STK): FMVoices: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): FMVoices: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Filter
    \brief STK filter class.

    This class implements a generic structure which
    can be used to create a wide range of filters.
    It can function independently or be subclassed
    to provide more specific controls based on a
    particular filter type.

    In particular, this class implements the standard
    difference equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients
    are normalized by a[0].

    The \e gain parameter is applied at the filter
    input and does not affect the coefficient values.
    The default gain value is 1.0.  This structure
    results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>

Filter :: Filter()
{
  // The default constructor should setup for pass-through.
  gain = 1.0;
  nB = 1;
  nA = 1;
  b = new MY_FLOAT[nB];
  b[0] = 1.0;
  a = new MY_FLOAT[nA];
  a[0] = 1.0;

  inputs = new MY_FLOAT[nB];
  outputs = new MY_FLOAT[nA];
  this->clear();
}

Filter :: Filter(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients)
{
  char message[256];

  // Check the arguments.
  if ( nb < 1 || na < 1 ) {
    sprintf(message, "[chuck](via Filter): nb (%d) and na (%d) must be >= 1!", nb, na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "[chuck](via Filter): a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  gain = 1.0;
  nB = nb;
  nA = na;
  b = new MY_FLOAT[nB];
  a = new MY_FLOAT[nA];

  inputs = new MY_FLOAT[nB];
  outputs = new MY_FLOAT[nA];
  this->clear();

  this->setCoefficients(nB, bCoefficients, nA, aCoefficients);
}

Filter :: ~Filter()
{
  delete [] b;
  delete [] a;
  delete [] inputs;
  delete [] outputs;
}

void Filter :: clear(void)
{
  int i;
  for (i=0; i<nB; i++)
    inputs[i] = 0.0;
  for (i=0; i<nA; i++)
    outputs[i] = 0.0;
}

void Filter :: setCoefficients(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( nb < 1 || na < 1 ) {
    sprintf(message, "[chuck](via Filter): nb (%d) and na (%d) must be >= 1!", nb, na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "[chuck](via Filter): a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (nb != nB) {
    delete [] b;
    delete [] inputs;
    nB = nb;
    b = new MY_FLOAT[nB];
    inputs = new MY_FLOAT[nB];
    for (i=0; i<nB; i++) inputs[i] = 0.0;
  }

  if (na != nA) {
    delete [] a;
    delete [] outputs;
    nA = na;
    a = new MY_FLOAT[nA];
    outputs = new MY_FLOAT[nA];
    for (i=0; i<nA; i++) outputs[i] = 0.0;
  }

  for (i=0; i<nB; i++)
    b[i] = bCoefficients[i];
  for (i=0; i<nA; i++)
    a[i] = aCoefficients[i];

  // scale coefficients by a[0] if necessary
  if (a[0] != 1.0) {
    for (i=0; i<nB; i++)
      b[i] /= a[0];
    for (i=0; i<nA; i++)
      a[i] /= a[0];
  }
}

void Filter :: setNumerator(int nb, MY_FLOAT *bCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( nb < 1 ) {
    sprintf(message, "[chuck](via Filter): nb (%d) must be >= 1!", nb);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (nb != nB) {
    delete [] b;
    delete [] inputs;
    nB = nb;
    b = new MY_FLOAT[nB];
    inputs = new MY_FLOAT[nB];
    for (i=0; i<nB; i++) inputs[i] = 0.0;
  }

  for (i=0; i<nB; i++)
    b[i] = bCoefficients[i];
}

void Filter :: setDenominator(int na, MY_FLOAT *aCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( na < 1 ) {
    sprintf(message, "[chuck](via Filter): na (%d) must be >= 1!", na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "[chuck](via Filter): a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (na != nA) {
    delete [] a;
    delete [] outputs;
    nA = na;
    a = new MY_FLOAT[nA];
    outputs = new MY_FLOAT[nA];
    for (i=0; i<nA; i++) outputs[i] = 0.0;
  }

  for (i=0; i<nA; i++)
    a[i] = aCoefficients[i];

  // scale coefficients by a[0] if necessary

  if (a[0] != 1.0) {
    for (i=0; i<nB; i++)
      b[i] /= a[0];
    for (i=0; i<nA; i++)
      a[i] /= a[0];
  }
}

void Filter :: setGain(MY_FLOAT theGain)
{
  gain = theGain;
}

MY_FLOAT Filter :: getGain(void) const
{
  return gain;
}

MY_FLOAT Filter :: lastOut(void) const
{
  return outputs[0];
}

MY_FLOAT Filter :: tick(MY_FLOAT sample)
{
  int i;

  outputs[0] = 0.0;
  inputs[0] = gain * sample;
  for (i=nB-1; i>0; i--) {
    outputs[0] += b[i] * inputs[i];
    inputs[i] = inputs[i-1];
  }
  outputs[0] += b[0] * inputs[0];

  for (i=nA-1; i>0; i--) {
    outputs[0] += -a[i] * outputs[i];
    outputs[i] = outputs[i-1];
  }

  return outputs[0];
}

MY_FLOAT *Filter :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class Flute
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Flute :: Flute(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  boreDelay = new DelayL( 100.0, length );
  length >>= 1;
  jetDelay = new DelayL( 49.0, length );
  jetTable = new JetTabl();
  filter = new OnePole();
  dcBlock = new PoleZero();
  dcBlock->setBlockZero();
  noise = new Noise();
  adsr = new ADSR();

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 5.925 );

  this->clear();

  filter->setPole( 0.7 - ((MY_FLOAT) 0.1 * 22050.0 / Stk::sampleRate() ) );
  filter->setGain( -1.0 );
  adsr->setAllTimes( 0.005, 0.01, 0.8, 0.010);
  endReflection = (MY_FLOAT) 0.5;
  jetReflection = (MY_FLOAT) 0.5;
  noiseGain =  0.15;             // Breath pressure random component.
  vibratoGain = (MY_FLOAT) 0.05; // Breath periodic vibrato component.
  jetRatio = (MY_FLOAT) 0.32;

	maxPressure = (MY_FLOAT) 0.0;
  lastFrequency = 220.0;
}

Flute :: ~Flute()
{
  delete jetDelay;
  delete boreDelay;
  delete jetTable;
  delete filter;
  delete dcBlock;
  delete noise;
  delete adsr;
  delete vibrato;
}

void Flute :: clear()
{
  jetDelay->clear();
  boreDelay->clear();
  filter->clear();
  dcBlock->clear();
}

void Flute :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Flute: setFrequency parameter is less than or equal to zero!" << std::endl;
    lastFrequency = 220.0;
  }

  // We're overblowing here.
  lastFrequency *= 0.66666;
  // Delay = length - approximate filter delay.
  MY_FLOAT delay = Stk::sampleRate() / lastFrequency - (MY_FLOAT) 2.0;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;

  boreDelay->setDelay(delay);
  jetDelay->setDelay(delay * jetRatio);
}

void Flute :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
	fprintf (stderr,"flute::startblowing %f %f \n", amplitude, rate);
  adsr->setAttackRate(rate);
  maxPressure = amplitude / (MY_FLOAT) 0.8;
  adsr->keyOn();
}

void Flute :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void Flute :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Flute: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Flute :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Flute: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void Flute :: setJetReflection(MY_FLOAT coefficient)
{
  jetReflection = coefficient;
}

void Flute :: setEndReflection(MY_FLOAT coefficient)
{         
  endReflection = coefficient;
}               

void Flute :: setJetDelay(MY_FLOAT aRatio)
{
  // Delay = length - approximate filter delay.
  MY_FLOAT temp = Stk::sampleRate() / lastFrequency - (MY_FLOAT) 2.0;
  jetRatio = aRatio;
  jetDelay->setDelay(temp * aRatio); // Scaled by ratio.
}

MY_FLOAT Flute :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = maxPressure * adsr->tick();
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  MY_FLOAT temp = filter->tick( boreDelay->lastOut() );
  temp = dcBlock->tick(temp); // Block DC on reflection.

  pressureDiff = breathPressure - (jetReflection * temp);
  pressureDiff = jetDelay->tick( pressureDiff );
  pressureDiff = jetTable->tick( pressureDiff ) + (endReflection * temp);
  lastOutput = (MY_FLOAT) 0.3 * boreDelay->tick( pressureDiff );

  lastOutput *= outputGain;
  return lastOutput;
}

void Flute :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Flute: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Flute: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_JetDelay_) // 2
    this->setJetDelay( (MY_FLOAT) (0.08 + (0.48 * norm)) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = ( norm * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = ( norm * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): Flute: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Flute: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


FormSwep :: FormSwep() : BiQuad()
{
  frequency = (MY_FLOAT) 0.0;
  radius = (MY_FLOAT) 0.0;
  targetGain = (MY_FLOAT) 1.0;
  targetFrequency = (MY_FLOAT) 0.0;
  targetRadius = (MY_FLOAT) 0.0;
  deltaGain = (MY_FLOAT) 0.0;
  deltaFrequency = (MY_FLOAT) 0.0;
  deltaRadius = (MY_FLOAT) 0.0;
  sweepState = (MY_FLOAT)  0.0;
  sweepRate = (MY_FLOAT) 0.002;
  dirty = false;
  this->clear();
}

FormSwep :: ~FormSwep()
{
}

void FormSwep :: setResonance(MY_FLOAT aFrequency, MY_FLOAT aRadius)
{
  dirty = false;
  radius = aRadius;
  frequency = aFrequency;

  BiQuad::setResonance( frequency, radius, true );
}

void FormSwep :: setStates(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain)
{
  dirty = false;

  if ( frequency != aFrequency || radius != aRadius )
    BiQuad::setResonance( aFrequency, aRadius, true );

  frequency = aFrequency;
  radius = aRadius;
  gain = aGain;
  targetFrequency = aFrequency;
  targetRadius = aRadius;
  targetGain = aGain;
}

void FormSwep :: setTargets(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain)
{
  dirty = true;
  startFrequency = frequency;
  startRadius = radius;
  startGain = gain;
  targetFrequency = aFrequency;
  targetRadius = aRadius;
  targetGain = aGain;
  deltaFrequency = aFrequency - frequency;
  deltaRadius = aRadius - radius;
  deltaGain = aGain - gain;
  sweepState = (MY_FLOAT) 0.0;
}

void FormSwep :: setSweepRate(MY_FLOAT aRate)
{
  sweepRate = aRate;
  if ( sweepRate > 1.0 ) sweepRate = 1.0;
  if ( sweepRate < 0.0 ) sweepRate = 0.0;
}

void FormSwep :: setSweepTime(MY_FLOAT aTime)
{
  sweepRate = 1.0 / ( aTime * Stk::sampleRate() );
  if ( sweepRate > 1.0 ) sweepRate = 1.0;
  if ( sweepRate < 0.0 ) sweepRate = 0.0;
}

MY_FLOAT FormSwep :: tick(MY_FLOAT sample)
{                                     
  if (dirty)  {
    sweepState += sweepRate;
    if ( sweepState >= 1.0 )   {
      sweepState = (MY_FLOAT) 1.0;
      dirty = false;
      radius = targetRadius;
      frequency = targetFrequency;
      gain = targetGain;
    }
    else  {
      radius = startRadius + (deltaRadius * sweepState);
      frequency = startFrequency + (deltaFrequency * sweepState);
      gain = startGain + (deltaGain * sweepState);
    }
    BiQuad::setResonance( frequency, radius, true );
  }

  return BiQuad::tick( sample );
}

MY_FLOAT *FormSwep :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class HevyMetl
    \brief STK heavy metal FM synthesis instrument.

    This class implements 3 cascade operators with
    feedback modulation, also referred to as
    algorithm 3 of the TX81Z.

    Algorithm 3 is :     4--\
                    3-->2-- + -->1-->Out

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


HevyMetl :: HevyMetl()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0 * 1.000);
  this->setRatio(1, 4.0 * 0.999);
  this->setRatio(2, 3.0 * 1.001);
  this->setRatio(3, 0.5 * 1.002);

  gains[0] = __FM_gains[92];
  gains[1] = __FM_gains[76];
  gains[2] = __FM_gains[91];
  gains[3] = __FM_gains[68];

  adsr[0]->setAllTimes( 0.001, 0.001, 1.0, 0.01);
  adsr[1]->setAllTimes( 0.001, 0.010, 1.0, 0.50);
  adsr[2]->setAllTimes( 0.010, 0.005, 1.0, 0.20);
  adsr[3]->setAllTimes( 0.030, 0.010, 0.2, 0.20);

  twozero->setGain( 2.0 );
  vibrato->setFrequency( 5.5 );
  modDepth = 0.0;
}  

HevyMetl :: ~HevyMetl()
{
}

void HevyMetl :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[92];
  gains[1] = amplitude * __FM_gains[76];
  gains[2] = amplitude * __FM_gains[91];
  gains[3] = amplitude * __FM_gains[68];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "HevyMetl: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT HevyMetl :: tick()
{
  register MY_FLOAT temp;

  temp = vibrato->tick() * modDepth * 0.2;    
  waves[0]->setFrequency(baseFrequency * (1.0 + temp) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * (1.0 + temp) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * (1.0 + temp) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * (1.0 + temp) * ratios[3]);
    
  temp = gains[2] * adsr[2]->tick() * waves[2]->tick();
  waves[1]->addPhaseOffset(temp);
    
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = (1.0 - (control2 * 0.5)) * gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);
    
  temp += control2 * (MY_FLOAT) 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;
    
  waves[0]->addPhaseOffset(temp);
  temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class Instrmnt
    \brief STK instrument abstract base class.

    This class provides a common interface for
    all STK instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Instrmnt :: Instrmnt()
{
}

Instrmnt :: ~Instrmnt()
{
}

void Instrmnt :: setFrequency(MY_FLOAT frequency)
{
  std::cerr << "[chuck](via STK): Instrmnt: virtual setFrequency function call!" << std::endl;
}

MY_FLOAT Instrmnt :: lastOut() const
{
  return lastOutput;
}

// Support for stereo output:
MY_FLOAT Instrmnt :: lastOutLeft(void) const
{
  return 0.5 * lastOutput;
}
                                                                                
MY_FLOAT Instrmnt :: lastOutRight(void) const
{
  return 0.5 * lastOutput;
}

MY_FLOAT *Instrmnt :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

void Instrmnt :: controlChange(int number, MY_FLOAT value)
{
}
/***************************************************/
/*! \class JCRev
    \brief John Chowning's reverberator class.

    This class is derived from the CLM JCRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This class implements three series
    allpass units, followed by four parallel comb
    filters, and two decorrelation delay lines in
    parallel at the output.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

JCRev :: JCRev(MY_FLOAT T60)
{
  // Delay lengths for 44100 Hz sample rate.
  int lengths[9] = {1777, 1847, 1993, 2137, 389, 127, 43, 211, 179};
  double scaler = Stk::sampleRate() / 44100.0;

  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<9; i++) {
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for (i=0; i<3; i++)
	  allpassDelays[i] = new Delay(lengths[i+4], lengths[i+4]);

  for (i=0; i<4; i++)	{
    combDelays[i] = new Delay(lengths[i], lengths[i]);
    combCoefficient[i] = pow(10.0,(-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  outLeftDelay = new Delay(lengths[7], lengths[7]);
  outRightDelay = new Delay(lengths[8], lengths[8]);
  allpassCoefficient = 0.7;
  effectMix = 0.3;
  this->clear();
}

JCRev :: ~JCRev()
{
  delete allpassDelays[0];
  delete allpassDelays[1];
  delete allpassDelays[2];
  delete combDelays[0];
  delete combDelays[1];
  delete combDelays[2];
  delete combDelays[3];
  delete outLeftDelay;
  delete outRightDelay;
}

void JCRev :: clear()
{
  allpassDelays[0]->clear();
  allpassDelays[1]->clear();
  allpassDelays[2]->clear();
  combDelays[0]->clear();
  combDelays[1]->clear();
  combDelays[2]->clear();
  combDelays[3]->clear();
  outRightDelay->clear();
  outLeftDelay->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

MY_FLOAT JCRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp, temp0, temp1, temp2, temp3, temp4, temp5, temp6;
  MY_FLOAT filtout;

  temp = allpassDelays[0]->lastOut();
  temp0 = allpassCoefficient * temp;
  temp0 += input;
  allpassDelays[0]->tick(temp0);
  temp0 = -(allpassCoefficient * temp0) + temp;
    
  temp = allpassDelays[1]->lastOut();
  temp1 = allpassCoefficient * temp;
  temp1 += temp0;
  allpassDelays[1]->tick(temp1);
  temp1 = -(allpassCoefficient * temp1) + temp;
    
  temp = allpassDelays[2]->lastOut();
  temp2 = allpassCoefficient * temp;
  temp2 += temp1;
  allpassDelays[2]->tick(temp2);
  temp2 = -(allpassCoefficient * temp2) + temp;
    
  temp3 = temp2 + (combCoefficient[0] * combDelays[0]->lastOut());
  temp4 = temp2 + (combCoefficient[1] * combDelays[1]->lastOut());
  temp5 = temp2 + (combCoefficient[2] * combDelays[2]->lastOut());
  temp6 = temp2 + (combCoefficient[3] * combDelays[3]->lastOut());

  combDelays[0]->tick(temp3);
  combDelays[1]->tick(temp4);
  combDelays[2]->tick(temp5);
  combDelays[3]->tick(temp6);

  filtout = temp3 + temp4 + temp5 + temp6;

  lastOutput[0] = effectMix * (outLeftDelay->tick(filtout));
  lastOutput[1] = effectMix * (outRightDelay->tick(filtout));
  temp = (1.0 - effectMix) * input;
  lastOutput[0] += temp;
  lastOutput[1] += temp;
    
  return (lastOutput[0] + lastOutput[1]) * 0.5;
}
/***************************************************/
/*! \class JetTabl
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


JetTabl :: JetTabl()
{
  lastOutput = (MY_FLOAT) 0.0;
}

JetTabl :: ~JetTabl()
{
}

MY_FLOAT JetTabl :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT JetTabl :: tick( MY_FLOAT input )
{
  // Perform "table lookup" using a polynomial
  // calculation (x^3 - x), which approximates
  // the jet sigmoid behavior.
  lastOutput = input * (input * input - (MY_FLOAT)  1.0);

  // Saturate at +/- 1.0.
  if (lastOutput > 1.0) 
    lastOutput = (MY_FLOAT) 1.0;
  if (lastOutput < -1.0)
    lastOutput = (MY_FLOAT) -1.0; 
  return lastOutput;
}

MY_FLOAT *JetTabl :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class Mandolin
    \brief STK mandolin instrument model class.

    This class inherits from PluckTwo and uses
    "commuted synthesis" techniques to model a
    mandolin instrument.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    Commuted Synthesis, in particular, is covered
    by patents, granted, pending, and/or
    applied-for.  All are assigned to the Board of
    Trustees, Stanford University.  For
    information, contact the Office of Technology
    Licensing, Stanford University.

    Control Change Numbers: 
       - Body Size = 2
       - Pluck Position = 4
       - String Sustain = 11
       - String Detuning = 1
       - Microphone Position = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Mandolin :: Mandolin(MY_FLOAT lowestFrequency)
  : PluckTwo(lowestFrequency)
{
  // Concatenate the STK rawwave path to the rawwave files
  soundfile[0] = new WvIn( "special:mand1", TRUE );
  soundfile[1] = new WvIn( "special:mand1", TRUE );
  soundfile[2] = new WvIn( "special:mand1", TRUE );
  soundfile[3] = new WvIn( "special:mand1", TRUE );
  soundfile[4] = new WvIn( "special:mand1", TRUE );
  soundfile[5] = new WvIn( "special:mand1", TRUE );
  soundfile[6] = new WvIn( "special:mand1", TRUE );
  soundfile[7] = new WvIn( "special:mand1", TRUE );
  soundfile[8] = new WvIn( "special:mand1", TRUE );
  soundfile[9] = new WvIn( "special:mand1", TRUE );
  soundfile[10] = new WvIn( "special:mand1", TRUE );
  soundfile[11] = new WvIn( "special:mand1", TRUE );

  directBody = 1.0;
  mic = 0;
  dampTime = 0;
  waveDone = soundfile[mic]->isFinished();
}

Mandolin :: ~Mandolin()
{
  for ( int i=0; i<12; i++ )
    delete soundfile[i];
}

void Mandolin :: pluck(MY_FLOAT amplitude)
{
  // This function gets interesting, because pluck
  // may be longer than string length, so we just
  // reset the soundfile and add in the pluck in
  // the tick method.
  soundfile[mic]->reset();
  waveDone = false;
  pluckAmplitude = amplitude;
  if ( amplitude < 0.0 ) {
    std::cerr << "[chuck](via STK): Mandolin: pluck amplitude parameter less than zero!" << std::endl;
    pluckAmplitude = 0.0;
  }
  else if ( amplitude > 1.0 ) {
    std::cerr << "[chuck](via STK): Mandolin: pluck amplitude parameter greater than 1.0!" << std::endl;
    pluckAmplitude = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay->setDelay((MY_FLOAT) 0.5 * pluckPosition * lastLength); 
  dampTime = (long) lastLength;   // See tick method below.
}

void Mandolin :: pluck(MY_FLOAT amplitude, MY_FLOAT position)
{
  // Pluck position puts zeroes at position * length.
  pluckPosition = position;
  if ( position < 0.0 ) {
    std::cerr << "[chuck](via STK): Mandolin: pluck position parameter less than zero!" << std::endl;
    pluckPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    std::cerr << "[chuck](via STK): Mandolin: pluck position parameter greater than 1.0!" << std::endl;
    pluckPosition = 1.0;
  }

  this->pluck(amplitude);
}

void Mandolin :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Mandolin: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Mandolin :: setBodySize(MY_FLOAT size)
{
  m_bodySize = size;
  // Scale the commuted body response by its sample rate (22050).
  MY_FLOAT rate = size * 22050.0 / Stk::sampleRate();
  for ( int i=0; i<12; i++ )
    soundfile[i]->setRate(rate);
}

MY_FLOAT Mandolin :: tick()
{
  MY_FLOAT temp = 0.0;
  if ( !waveDone ) {
    // Scale the pluck excitation with comb
    // filtering for the duration of the file.
    temp = soundfile[mic]->tick() * pluckAmplitude;
    temp = temp - combDelay->tick(temp);
    waveDone = soundfile[mic]->isFinished();
  }

  // Damping hack to help avoid overflow on re-plucking.
  if ( dampTime >=0 ) {
    dampTime -= 1;
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastOutput = delayLine->tick( filter->tick( temp + (delayLine->lastOut() * (MY_FLOAT) 0.7) ) );
    // Calculate 2nd delay just like the 1st.
    lastOutput += delayLine2->tick( filter2->tick( temp + (delayLine2->lastOut() * (MY_FLOAT) 0.7) ) );
  }
  else { // No damping hack after 1 period.
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastOutput = delayLine->tick( filter->tick( temp + (delayLine->lastOut() * loopGain) ) );
    // Calculate 2nd delay just like the 1st.
    lastOutput += delayLine2->tick( filter2->tick( temp + (delayLine2->lastOut() * loopGain) ) );
  }

  lastOutput *= (MY_FLOAT) 0.3;
  return lastOutput;
}

void Mandolin :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Mandolin: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Mandolin: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_BodySize_) // 2
    this->setBodySize( norm * 2.0 );
  else if (number == __SK_PickPosition_) // 4
    this->setPluckPosition( norm );
  else if (number == __SK_StringDamping_) // 11
    this->setBaseLoopGain((MY_FLOAT) 0.97 + (norm * (MY_FLOAT) 0.03));
  else if (number == __SK_StringDetune_) // 1
    this->setDetune((MY_FLOAT) 1.0 - (norm * (MY_FLOAT) 0.1));
  else if (number == __SK_AfterTouch_Cont_) // 128
    mic = (int) (norm * 11.0);
  else
    std::cerr << "[chuck](via STK): Mandolin: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Mandolin: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Mesh2D
    \brief Two-dimensional rectilinear waveguide mesh class.

    This class implements a rectilinear,
    two-dimensional digital waveguide mesh
    structure.  For details, see Van Duyne and
    Smith, "Physical Modeling with the 2-D Digital
    Waveguide Mesh", Proceedings of the 1993
    International Computer Music Conference.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - X Dimension = 2
       - Y Dimension = 4
       - Mesh Decay = 11
       - X-Y Input Position = 1

    by Julius Smith, 2000 - 2002.
    Revised by Gary Scavone for STK, 2002.
*/
/***************************************************/

#include <stdlib.h>

Mesh2D :: Mesh2D(short nX, short nY)
{
  this->setNX(nX);
  this->setNY(nY);

  MY_FLOAT pole = 0.05;

  short i;
  for (i=0; i<NYMAX; i++) {
    filterY[i] = new OnePole(pole);
    filterY[i]->setGain(0.99);
  }

  for (i=0; i<NXMAX; i++) {
    filterX[i] = new OnePole(pole);
    filterX[i]->setGain(0.99);
  }

  this->clearMesh();

  counter=0;
  xInput = 0;
  yInput = 0;
}

Mesh2D :: ~Mesh2D()
{
  short i;
  for (i=0; i<NYMAX; i++)
    delete filterY[i];

  for (i=0; i<NXMAX; i++)
    delete filterX[i];
}

void Mesh2D :: clear()
{
  this->clearMesh();

  short i;
  for (i=0; i<NY; i++)
    filterY[i]->clear();

  for (i=0; i<NX; i++)
    filterX[i]->clear();

  counter=0;
}

void Mesh2D :: clearMesh()
{
  int x, y;
  for (x=0; x<NXMAX-1; x++) {
    for (y=0; y<NYMAX-1; y++) {
      v[x][y] = 0;
    }
  }
  for (x=0; x<NXMAX; x++) {
    for (y=0; y<NYMAX; y++) {

      vxp[x][y] = 0;
      vxm[x][y] = 0;
      vyp[x][y] = 0;
      vym[x][y] = 0;

      vxp1[x][y] = 0;
      vxm1[x][y] = 0;
      vyp1[x][y] = 0;
      vym1[x][y] = 0;
    }
  }
}

MY_FLOAT Mesh2D :: energy()
{
  // Return total energy contained in wave variables Note that some
  // energy is also contained in any filter delay elements.

  int x, y;
  MY_FLOAT t;
  MY_FLOAT e = 0;
  if ( counter & 1 ) { // Ready for Mesh2D::tick1() to be called.
    for (x=0; x<NX; x++) {
      for (y=0; y<NY; y++) {
        t = vxp1[x][y];
        e += t*t;
        t = vxm1[x][y];
        e += t*t;
        t = vyp1[x][y];
        e += t*t;
        t = vym1[x][y];
        e += t*t;
      }
    }
  }
  else { // Ready for Mesh2D::tick0() to be called.
    for (x=0; x<NX; x++) {
      for (y=0; y<NY; y++) {
        t = vxp[x][y];
        e += t*t;
        t = vxm[x][y];
        e += t*t;
        t = vyp[x][y];
        e += t*t;
        t = vym[x][y];
        e += t*t;
      }
    }
  }

  return(e);
}

void Mesh2D :: setNX(short lenX)
{
  NX = lenX;
  if ( lenX < 2 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setNX(" << lenX << "): Minimum length is 2!" << std::endl;
    NX = 2;
  }
  else if ( lenX > NXMAX ) {
    std::cerr << "[chuck](via STK): Mesh2D::setNX(" << lenX << "): Maximum length is " << NXMAX << "!" << std::endl;
    NX = NXMAX;
  }
}

void Mesh2D :: setNY(short lenY)
{
  NY = lenY;
  if ( lenY < 2 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setNY(" << lenY << "): Minimum length is 2!" << std::endl;
    NY = 2;
  }
  else if ( lenY > NYMAX ) {
    std::cerr << "[chuck](via STK): Mesh2D::setNY(" << lenY << "): Maximum length is " << NYMAX << "!" << std::endl;
    NY = NYMAX;
  }
}

void Mesh2D :: setDecay(MY_FLOAT decayFactor)
{
  MY_FLOAT gain = decayFactor;
  if ( decayFactor < 0.0 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setDecay decayFactor value is less than 0.0!" << std::endl;
    gain = 0.0;
  }
  else if ( decayFactor > 1.0 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setDecay decayFactor value is greater than 1.0!" << std::endl;
    gain = 1.0;
  }

  int i;
  for (i=0; i<NYMAX; i++)
    filterY[i]->setGain(gain);

  for (i=0; i<NXMAX; i++)
    filterX[i]->setGain(gain);
}

void Mesh2D :: setInputPosition(MY_FLOAT xFactor, MY_FLOAT yFactor)
{
  if ( xFactor < 0.0 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setInputPosition xFactor value is less than 0.0!" << std::endl;
    xInput = 0;
  }
  else if ( xFactor > 1.0 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setInputPosition xFactor value is greater than 1.0!" << std::endl;
    xInput = NX - 1;
  }
  else
    xInput = (short) (xFactor * (NX - 1));

  if ( yFactor < 0.0 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setInputPosition yFactor value is less than 0.0!" << std::endl;
    yInput = 0;
  }
  else if ( yFactor > 1.0 ) {
    std::cerr << "[chuck](via STK): Mesh2D::setInputPosition yFactor value is greater than 1.0!" << std::endl;
    yInput = NY - 1;
  }
  else
    yInput = (short) (yFactor * (NY - 1));
}

void Mesh2D :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  // Input at corner.
  if ( counter & 1 ) {
    vxp1[xInput][yInput] += amplitude;
    vyp1[xInput][yInput] += amplitude;
  }
  else {
    vxp[xInput][yInput] += amplitude;
    vyp[xInput][yInput] += amplitude;
  }

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Mesh2D: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Mesh2D :: noteOff(MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Mesh2D: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Mesh2D :: tick(MY_FLOAT input)
{
  if ( counter & 1 ) {
    vxp1[xInput][yInput] += input;
    vyp1[xInput][yInput] += input;
    lastOutput = tick1();
  }
  else {
    vxp[xInput][yInput] += input;
    vyp[xInput][yInput] += input;
    lastOutput = tick0();
  }

  counter++;
  return lastOutput;
}

MY_FLOAT Mesh2D :: tick()
{
  lastOutput = ((counter & 1) ? this->tick1() : this->tick0());
  counter++;
  return lastOutput;
}

#define VSCALE ((MY_FLOAT) (0.5))

MY_FLOAT Mesh2D :: tick0()
{
  int x, y;
  MY_FLOAT outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      v[x][y] = ( vxp[x][y] + vxm[x+1][y] + 
		  vyp[x][y] + vym[x][y+1] ) * VSCALE;
    }
  }    

  // Update junction outgoing waves, using alternate wave-variable buffers.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      MY_FLOAT vxy = v[x][y];
      // Update positive-going waves.
      vxp1[x+1][y] = vxy - vxm[x+1][y];
      vyp1[x][y+1] = vxy - vym[x][y+1];
      // Update minus-going waves.
      vxm1[x][y] = vxy - vxp[x][y];
      vym1[x][y] = vxy - vyp[x][y];
    }
  }    

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY-1; y++) {
    vxp1[0][y] = filterY[y]->tick(vxm[0][y]);
    vxm1[NX-1][y] = vxp[NX-1][y];
  }
  for (x=0; x<NX-1; x++) {
    vyp1[x][0] = filterX[x]->tick(vym[x][0]);
    vym1[x][NY-1] = vyp[x][NY-1];
  }

  // Output = sum of outgoing waves at far corner.  Note that the last
  // index in each coordinate direction is used only with the other
  // coordinate indices at their next-to-last values.  This is because
  // the "unit strings" attached to each velocity node to terminate
  // the mesh are not themselves connected together.
  outsamp = vxp[NX-1][NY-2] + vyp[NX-2][NY-1];

  return outsamp;
}

MY_FLOAT Mesh2D :: tick1()
{
  int x, y;
  MY_FLOAT outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      v[x][y] = ( vxp1[x][y] + vxm1[x+1][y] + 
		  vyp1[x][y] + vym1[x][y+1] ) * VSCALE;
    }
  }

  // Update junction outgoing waves, 
  // using alternate wave-variable buffers.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      MY_FLOAT vxy = v[x][y];

      // Update positive-going waves.
      vxp[x+1][y] = vxy - vxm1[x+1][y];
      vyp[x][y+1] = vxy - vym1[x][y+1];

      // Update minus-going waves.
      vxm[x][y] = vxy - vxp1[x][y];
      vym[x][y] = vxy - vyp1[x][y];
    }
  }

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY-1; y++) {
    vxp[0][y] = filterY[y]->tick(vxm1[0][y]);
    vxm[NX-1][y] = vxp1[NX-1][y];
  }
  for (x=0; x<NX-1; x++) {
    vyp[x][0] = filterX[x]->tick(vym1[x][0]);
    vym[x][NY-1] = vyp1[x][NY-1];
  }

  // Output = sum of outgoing waves at far corner.
  outsamp = vxp1[NX-1][NY-2] + vyp1[NX-2][NY-1];

  return outsamp;
}

void Mesh2D :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Mesh2D: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Mesh2D: Control value greater than 128.0!" << std::endl;
  }

  if (number == 2) // 2
    setNX( (short) (norm * (NXMAX-2) + 2) );
  else if (number == 4) // 4
    setNY( (short) (norm * (NYMAX-2) + 2) );
  else if (number == 11) // 11
    setDecay( 0.9 + (norm * 0.1) );
  else if (number == __SK_ModWheel_) // 1
    setInputPosition(norm, norm);
  else if (number == __SK_AfterTouch_Cont_) // 128
    ;
  else
    std::cerr << "[chuck](via STK): Mesh2D: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Mesh2D: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Modal
    \brief STK resonance model instrument.

    This class contains an excitation wavetable,
    an envelope, an oscillator, and N resonances
    (non-sweeping BiQuad filters), where N is set
    during instantiation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>

Modal :: Modal(int modes)
  : nModes(modes)
{
  if ( nModes <= 0 ) {
    char msg[256];
    sprintf(msg, "[chuck](via Modal): Invalid number of modes (%d) argument to constructor!", modes);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  // We don't make the excitation wave here yet, because we don't know
  // what it's going to be.

  ratios = (MY_FLOAT *) new MY_FLOAT[nModes];
  radii = (MY_FLOAT *) new MY_FLOAT[nModes];
  filters = (BiQuad **) calloc( nModes, sizeof(BiQuad *) );
  for (int i=0; i<nModes; i++ ) {
    filters[i] = new BiQuad;
    filters[i]->setEqualGainZeroes();
  }

  envelope = new Envelope;
  onepole = new OnePole;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );

  // Set some default values.
  vibrato->setFrequency( 6.0 );
  vibratoGain = 0.0;
  directGain = 0.0;
  masterGain = 1.0;
  baseFrequency = 440.0;

  this->clear();

  stickHardness =  0.5;
  strikePosition = 0.561;
}  

Modal :: ~Modal()
{
  delete envelope; 
  delete onepole;
  delete vibrato;

  delete [] ratios;
  delete [] radii;
  for (int i=0; i<nModes; i++ ) {
    delete filters[i];
  }
  free(filters);
}

void Modal :: clear()
{    
  onepole->clear();
  for (int i=0; i<nModes; i++ )
    filters[i]->clear();
}

void Modal :: setFrequency(MY_FLOAT frequency)
{
  baseFrequency = frequency;
  for (int i=0; i<nModes; i++ )
    this->setRatioAndRadius(i, ratios[i], radii[i]);
}

void Modal :: setRatioAndRadius(int modeIndex, MY_FLOAT ratio, MY_FLOAT radius)
{
  if ( modeIndex < 0 ) {
    std::cerr << "[chuck](via STK): Modal: setRatioAndRadius modeIndex parameter is less than zero!" << std::endl;
    return;
  }
  else if ( modeIndex >= nModes ) {
    std::cerr << "[chuck](via STK): Modal: setRatioAndRadius modeIndex parameter is greater than the number of operators!" << std::endl;
    return;
  }

  MY_FLOAT nyquist = Stk::sampleRate() / 2.0;
  MY_FLOAT temp;

  if (ratio * baseFrequency < nyquist) {
    ratios[modeIndex] = ratio;
  }
  else {
    temp = ratio;
    while (temp * baseFrequency > nyquist) temp *= (MY_FLOAT) 0.5;
    ratios[modeIndex] = temp;
#if defined(_STK_DEBUG_)
    std::cerr << "[chuck](via STK): Modal : Aliasing would occur here ... correcting." << std::endl;
#endif
  }
  radii[modeIndex] = radius;
  if (ratio < 0) 
    temp = -ratio;
  else
    temp = ratio*baseFrequency;

  filters[modeIndex]->setResonance(temp, radius);
}

void Modal :: setMasterGain(MY_FLOAT aGain)
{
  masterGain = aGain;
}

void Modal :: setDirectGain(MY_FLOAT aGain)
{
  directGain = aGain;
}

void Modal :: setModeGain(int modeIndex, MY_FLOAT gain)
{
  if ( modeIndex < 0 ) {
    std::cerr << "[chuck](via STK): Modal: setModeGain modeIndex parameter is less than zero!" << std::endl;
    return;
  }
  else if ( modeIndex >= nModes ) {
    std::cerr << "[chuck](via STK): Modal: setModeGain modeIndex parameter is greater than the number of operators!" << std::endl;
    return;
  }

  filters[modeIndex]->setGain(gain);
}

void Modal :: strike(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( amplitude < 0.0 ) {
    std::cerr << "[chuck](via STK): Modal: strike amplitude is less than zero!" << std::endl;
    gain = 0.0;
  }
  else if ( amplitude > 1.0 ) {
    std::cerr << "[chuck](via STK): Modal: strike amplitude is greater than 1.0!" << std::endl;
    gain = 1.0;
  }

  envelope->setRate(1.0);
  envelope->setTarget(gain);
  onepole->setPole(1.0 - gain);
  envelope->tick();
  wave->reset();

  MY_FLOAT temp;
  for (int i=0; i<nModes; i++) {
    if (ratios[i] < 0)
      temp = -ratios[i];
    else
      temp = ratios[i] * baseFrequency;
    filters[i]->setResonance(temp, radii[i]);
  }
}

void Modal :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->strike(amplitude);
  this->setFrequency(frequency);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Modal: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Modal :: noteOff(MY_FLOAT amplitude)
{
  // This calls damp, but inverts the meaning of amplitude (high
  // amplitude means fast damping).
  this->damp(1.0 - (amplitude * 0.03));

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Modal: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void Modal :: damp(MY_FLOAT amplitude)
{
  MY_FLOAT temp;
  for (int i=0; i<nModes; i++) {
    if (ratios[i] < 0)
      temp = -ratios[i];
    else
      temp = ratios[i] * baseFrequency;
    filters[i]->setResonance(temp, radii[i]*amplitude);
  }
}

MY_FLOAT Modal :: tick()
{
  MY_FLOAT temp = masterGain * onepole->tick(wave->tick() * envelope->tick());

  MY_FLOAT temp2 = 0.0;
  for (int i=0; i<nModes; i++)
    temp2 += filters[i]->tick(temp);

  temp2  -= temp2 * directGain;
  temp2 += directGain * temp;

  if (vibratoGain != 0.0)	{
    // Calculate AM and apply to master out
    temp = 1.0 + (vibrato->tick() * vibratoGain);
    temp2 = temp * temp2;
  }
    
  lastOutput = temp2;
  return lastOutput;
}
/***************************************************/
/*! \class ModalBar
    \brief STK resonant bar instrument class.

    This class implements a number of different
    struck bar instruments.  It inherits from the
    Modal class.

    Control Change Numbers: 
       - Stick Hardness = 2
       - Stick Position = 4
       - Vibrato Gain = 11
       - Vibrato Frequency = 7
       - Direct Stick Mix = 1
       - Volume = 128
       - Modal Presets = 16
         - Marimba = 0
         - Vibraphone = 1
         - Agogo = 2
         - Wood1 = 3
         - Reso = 4
         - Wood2 = 5
         - Beats = 6
         - Two Fixed = 7
         - Clump = 8

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

ModalBar :: ModalBar()
  : Modal()
{
  // Concatenate the STK rawwave path to the rawwave file
  wave = new WvIn( "special:marmstk1", TRUE );
  wave->setRate((MY_FLOAT) 0.5 * 22050.0 / Stk::sampleRate() );

  // Set the resonances for preset 0 (marimba).
  setPreset( 0 );
}

ModalBar :: ~ModalBar()
{
  delete wave;
}

void ModalBar :: setStickHardness(MY_FLOAT hardness)
{
  stickHardness = hardness;
  if ( hardness < 0.0 ) {
    std::cerr << "[chuck](via STK): ModalBar: setStickHardness parameter is less than zero!" << std::endl;
    stickHardness = 0.0;
  }
  else if ( hardness > 1.0 ) {
    std::cerr << "[chuck](via STK): ModalBar: setStickHarness parameter is greater than 1.0!" << std::endl;
    stickHardness = 1.0;
  }

  wave->setRate( (0.25 * (MY_FLOAT)pow(4.0, stickHardness)) );
  masterGain = 0.1 + (1.8 * stickHardness);
}

void ModalBar :: setStrikePosition(MY_FLOAT position)
{
  strikePosition = position;
  if ( position < 0.0 ) {
    std::cerr << "[chuck](via STK): ModalBar: setStrikePositions parameter is less than zero!" << std::endl;
    strikePosition = 0.0;
  }
  else if ( position > 1.0 ) {
    std::cerr << "[chuck](via STK): ModalBar: setStrikePosition parameter is greater than 1.0!" << std::endl;
    strikePosition = 1.0;
  }

  // Hack only first three modes.
  MY_FLOAT temp2 = position * PI;
  MY_FLOAT temp = sin(temp2);                                       
  this->setModeGain(0, 0.12 * temp);

  temp = sin(0.05 + (3.9 * temp2));
  this->setModeGain(1,(MY_FLOAT) -0.03 * temp);

  temp = (MY_FLOAT)  sin(-0.05 + (11 * temp2));
  this->setModeGain(2,(MY_FLOAT) 0.11 * temp);
}

void ModalBar :: setPreset(int preset)
{
  // Presets:
  //     First line:  relative modal frequencies (negative number is
  //                  a fixed mode that doesn't scale with frequency
  //     Second line: resonances of the modes
  //     Third line:  mode volumes
  //     Fourth line: stickHardness, strikePosition, and direct stick
  //                  gain (mixed directly into the output
  static MY_FLOAT presets[9][4][4] = { 
    {{1.0, 3.99, 10.65, -2443},		// Marimba
     {0.9996, 0.9994, 0.9994, 0.999},
     {0.04, 0.01, 0.01, 0.008},
     {0.429688, 0.445312, 0.093750}},
    {{1.0, 2.01, 3.9, 14.37}, 		// Vibraphone
     {0.99995, 0.99991, 0.99992, 0.9999},	
     {0.025, 0.015, 0.015, 0.015 },
     {0.390625,0.570312,0.078125}},
    {{1.0, 4.08, 6.669, -3725.0},		// Agogo 
     {0.999, 0.999, 0.999, 0.999},	
     {0.06, 0.05, 0.03, 0.02},
     {0.609375,0.359375,0.140625}},
    {{1.0, 2.777, 7.378, 15.377},		// Wood1
     {0.996, 0.994, 0.994, 0.99},	
     {0.04, 0.01, 0.01, 0.008},
     {0.460938,0.375000,0.046875}},
    {{1.0, 2.777, 7.378, 15.377},		// Reso
     {0.99996, 0.99994, 0.99994, 0.9999},	
     {0.02, 0.005, 0.005, 0.004},
     {0.453125,0.250000,0.101562}},
    {{1.0, 1.777, 2.378, 3.377},		// Wood2
     {0.996, 0.994, 0.994, 0.99},	
     {0.04, 0.01, 0.01, 0.008},
     {0.312500,0.445312,0.109375}},
    {{1.0, 1.004, 1.013, 2.377},		// Beats
     {0.9999, 0.9999, 0.9999, 0.999},	
     {0.02, 0.005, 0.005, 0.004},
     {0.398438,0.296875,0.070312}},
    {{1.0, 4.0, -1320.0, -3960.0},		// 2Fix
     {0.9996, 0.999, 0.9994, 0.999},	
     {0.04, 0.01, 0.01, 0.008},
     {0.453125,0.453125,0.070312}},
    {{1.0, 1.217, 1.475, 1.729},		// Clump
     {0.999, 0.999, 0.999, 0.999},	
     {0.03, 0.03, 0.03, 0.03 },
     {0.390625,0.570312,0.078125}},
  };

  int temp = (preset % 9);
  for (int i=0; i<nModes; i++) {
    this->setRatioAndRadius(i, presets[temp][0][i], presets[temp][1][i]);
    this->setModeGain(i, presets[temp][2][i]);
  }

  this->setStickHardness(presets[temp][3][0]);
  this->setStrikePosition(presets[temp][3][1]);
  directGain = presets[temp][3][2];

  if (temp == 1) // vibraphone
    vibratoGain = 0.2;
  else
    vibratoGain = 0.0;
}

void ModalBar :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): ModalBar: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): ModalBar: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_StickHardness_) // 2
    this->setStickHardness( norm );
  else if (number == __SK_StrikePosition_) // 4
    this->setStrikePosition( norm );
  else if (number == __SK_ProphesyRibbon_) // 16
		this->setPreset((int) value);
  else if (number == __SK_ModWheel_) // 1
    directGain = norm;
  else if (number == 11) // 11
    vibratoGain = norm * 0.3;
  else if (number == __SK_ModFrequency_) // 7
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_AfterTouch_Cont_)	// 128
    envelope->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): ModalBar: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): ModalBar: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Modulate :: Modulate()
{
  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 6.0 );
  vibratoGain = 0.04;

  noise = new SubNoise(330);
  randomGain = 0.05;

  filter = new OnePole( 0.999 );
  filter->setGain( randomGain );
}

Modulate :: ~Modulate()
{
  delete vibrato;
  delete noise;
  delete filter;
}

void Modulate :: reset()
{
  lastOutput = (MY_FLOAT)  0.0;
}

void Modulate :: setVibratoRate(MY_FLOAT aRate)
{
  vibrato->setFrequency( aRate );
}

void Modulate :: setVibratoGain(MY_FLOAT aGain)
{
  vibratoGain = aGain;
}

void Modulate :: setRandomGain(MY_FLOAT aGain)
{
  randomGain = aGain;
  filter->setGain( randomGain );
}

MY_FLOAT Modulate :: tick()
{
  // Compute periodic and random modulations.
  lastOutput = vibratoGain * vibrato->tick();
  lastOutput += filter->tick( noise->tick() );
  return lastOutput;                        
}

MY_FLOAT *Modulate :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Modulate :: lastOut() const
{
  return lastOutput;
}
/***************************************************/
/*! \class Moog
    \brief STK moog-like swept filter sampling synthesis class.

    This instrument uses one attack wave, one
    looped wave, and an ADSR envelope (inherited
    from the Sampler class) and adds two sweepable
    formant (FormSwep) filters.

    Control Change Numbers: 
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Moog :: Moog()
{
  // Concatenate the STK rawwave path to the rawwave file
  attacks[0] = new WvIn( "special:mandpluk", TRUE );
  loops[0] = new WaveLoop( "special:impuls20", TRUE );
  loops[1] = new WaveLoop( "special:sinewave", TRUE ); // vibrato
  loops[1]->setFrequency((MY_FLOAT) 6.122);

  filters[0] = new FormSwep();
  filters[0]->setTargets( 0.0, 0.7 );

  filters[1] = new FormSwep();
  filters[1]->setTargets( 0.0, 0.7 );

  adsr->setAllTimes((MY_FLOAT) 0.001,(MY_FLOAT) 1.5,(MY_FLOAT) 0.6,(MY_FLOAT) 0.250);
  filterQ = (MY_FLOAT) 0.85;
  filterRate = (MY_FLOAT) 0.0001;
  modDepth = (MY_FLOAT) 0.0;
}  

Moog :: ~Moog()
{
  delete attacks[0];
  delete loops[0];
  delete loops[1];
  delete filters[0];
  delete filters[1];
}

void Moog :: setFrequency(MY_FLOAT frequency)
{
  baseFrequency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Moog: setFrequency parameter is less than or equal to zero!" << std::endl;
    baseFrequency = 220.0;
  }

  MY_FLOAT rate = attacks[0]->getSize() * 0.01 * baseFrequency / sampleRate();
  attacks[0]->setRate( rate );
  loops[0]->setFrequency(baseFrequency);
}

//CHUCK wrapper
void Moog :: noteOn(MY_FLOAT amplitude ) { 
  noteOn ( baseFrequency, amplitude );
}

void Moog :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  MY_FLOAT temp;
    
  this->setFrequency( frequency );
  this->keyOn();
  attackGain = amplitude * (MY_FLOAT) 0.5;
  loopGain = amplitude;

  temp = filterQ + (MY_FLOAT) 0.05;
  filters[0]->setStates( 2000.0, temp );
  filters[1]->setStates( 2000.0, temp );

  temp = filterQ + (MY_FLOAT) 0.099;
  filters[0]->setTargets( frequency, temp );
  filters[1]->setTargets( frequency, temp );

  filters[0]->setSweepRate( filterRate * 22050.0 / Stk::sampleRate() );
  filters[1]->setSweepRate( filterRate * 22050.0 / Stk::sampleRate() );

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Moog: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Moog :: setModulationSpeed(MY_FLOAT mSpeed)
{
  loops[1]->setFrequency(mSpeed);
}

void Moog :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth * (MY_FLOAT) 0.5;
}

MY_FLOAT Moog :: tick()
{
  MY_FLOAT temp;

  if ( modDepth != 0.0 ) {
    temp = loops[1]->tick() * modDepth;    
    loops[0]->setFrequency( baseFrequency * (1.0 + temp) );
  }
  
  temp = Sampler::tick();
  temp = filters[0]->tick( temp );
  lastOutput = filters[1]->tick( temp );
  return lastOutput * 3.0;
}

void Moog :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Moog: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Moog: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_FilterQ_) // 2
    filterQ = 0.80 + ( 0.1 * norm );
  else if (number == __SK_FilterSweepRate_) // 4
    filterRate = norm * 0.0002;
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0 );
  else if (number == __SK_ModWheel_)  // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): Moog: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Moog: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}



/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class is derived from the CLM NRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This particular arrangement consists
    of 6 comb filters in parallel, followed by 3
    allpass filters, a lowpass filter, and another
    allpass in series, followed by two allpass
    filters in parallel with corresponding right
    and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

NRev :: NRev(MY_FLOAT T60)
{
  int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19};
  double scaler = Stk::sampleRate() / 25641.0;

  int delay, i;
  for (i=0; i<15; i++) {
    delay = (int) floor(scaler * lengths[i]);
    if ( (delay & 1) == 0) delay++;
    while ( !this->isPrime(delay) ) delay += 2;
    lengths[i] = delay;
  }

  for (i=0; i<6; i++) {
    combDelays[i] = new Delay( lengths[i], lengths[i]);
    combCoefficient[i] = pow(10.0, (-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  for (i=0; i<8; i++)
    allpassDelays[i] = new Delay(lengths[i+6], lengths[i+6]);

  allpassCoefficient = 0.7;
  effectMix = 0.3;
  this->clear();
}

NRev :: ~NRev()
{
  int i;
  for (i=0; i<6; i++)  delete combDelays[i];
  for (i=0; i<8; i++)  delete allpassDelays[i];
}

void NRev :: clear()
{
  int i;
  for (i=0; i<6; i++) combDelays[i]->clear();
  for (i=0; i<8; i++) allpassDelays[i]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
  lowpassState = 0.0;
}

MY_FLOAT NRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp, temp0, temp1, temp2, temp3;
  int i;

  temp0 = 0.0;
  for (i=0; i<6; i++) {
    temp = input + (combCoefficient[i] * combDelays[i]->lastOut());
    temp0 += combDelays[i]->tick(temp);
  }
  for (i=0; i<3; i++)	{
    temp = allpassDelays[i]->lastOut();
    temp1 = allpassCoefficient * temp;
    temp1 += temp0;
    allpassDelays[i]->tick(temp1);
    temp0 = -(allpassCoefficient * temp1) + temp;
  }

	// One-pole lowpass filter.
  lowpassState = 0.7*lowpassState + 0.3*temp0;
  temp = allpassDelays[3]->lastOut();
  temp1 = allpassCoefficient * temp;
  temp1 += lowpassState;
  allpassDelays[3]->tick(temp1);
  temp1 = -(allpassCoefficient * temp1) + temp;
    
  temp = allpassDelays[4]->lastOut();
  temp2 = allpassCoefficient * temp;
  temp2 += temp1;
  allpassDelays[4]->tick(temp2);
  lastOutput[0] = effectMix*(-(allpassCoefficient * temp2) + temp);
    
  temp = allpassDelays[5]->lastOut();
  temp3 = allpassCoefficient * temp;
  temp3 += temp1;
  allpassDelays[5]->tick(temp3);
  lastOutput[1] = effectMix*(-(allpassCoefficient * temp3) + temp);

  temp = (1.0 - effectMix) * input;
  lastOutput[0] += temp;
  lastOutput[1] += temp;
    
  return (lastOutput[0] + lastOutput[1]) * 0.5;

}
/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>
#include <time.h>

Noise :: Noise() : Stk()
{
  // Seed the random number generator with system time.
  this->setSeed( 0 );
  lastOutput = (MY_FLOAT) 0.0;
}

Noise :: Noise( unsigned int seed ) : Stk()
{
  // Seed the random number generator
  this->setSeed( seed );
  lastOutput = (MY_FLOAT) 0.0;
}

Noise :: ~Noise()
{
}

void Noise :: setSeed( unsigned int seed )
{
  if ( seed == 0 )
    srand( (unsigned int) time(NULL) );
  else
    srand( seed );
}

MY_FLOAT Noise :: tick()
{
  lastOutput = (MY_FLOAT) (2.0 * rand() / (RAND_MAX + 1.0) );
  lastOutput -= 1.0;
  return lastOutput;
}

MY_FLOAT *Noise :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Noise :: lastOut() const
{
  return lastOutput;
}

/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This protected Filter subclass implements
    a one-pole digital filter.  A method is
    provided for setting the pole position along
    the real axis of the z-plane while maintaining
    a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


OnePole :: OnePole() : Filter()
{
  MY_FLOAT B = 0.1;
  MY_FLOAT A[2] = {1.0, -0.9};
  Filter::setCoefficients( 1, &B, 2, A );
}

OnePole :: OnePole(MY_FLOAT thePole) : Filter()
{
  MY_FLOAT B;
  MY_FLOAT A[2] = {1.0, -0.9};

  // Normalize coefficients for peak unity gain.
  if (thePole > 0.0)
    B = (MY_FLOAT) (1.0 - thePole);
  else
    B = (MY_FLOAT) (1.0 + thePole);

  A[1] = -thePole;
  Filter::setCoefficients( 1, &B, 2,  A );
}

OnePole :: ~OnePole()    
{
}

void OnePole :: clear(void)
{
  Filter::clear();
}

void OnePole :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void OnePole :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void OnePole :: setPole(MY_FLOAT thePole)
{
  // Normalize coefficients for peak unity gain.
  if (thePole > 0.0)
    b[0] = (MY_FLOAT) (1.0 - thePole);
  else
    b[0] = (MY_FLOAT) (1.0 + thePole);

  a[1] = -thePole;
}

void OnePole :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT OnePole :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT OnePole :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT OnePole :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] - a[1] * outputs[1];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *OnePole :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class OneZero
    \brief STK one-zero filter class.

    This protected Filter subclass implements
    a one-zero digital filter.  A method is
    provided for setting the zero position
    along the real axis of the z-plane while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


OneZero :: OneZero() : Filter()
{
  MY_FLOAT B[2] = {0.5, 0.5};
  MY_FLOAT A = 1.0;
  Filter::setCoefficients( 2, B, 1, &A );
}

OneZero :: OneZero(MY_FLOAT theZero) : Filter()
{
  MY_FLOAT B[2];
  MY_FLOAT A = 1.0;

  // Normalize coefficients for unity gain.
  if (theZero > 0.0)
    B[0] = 1.0 / ((MY_FLOAT) 1.0 + theZero);
  else
    B[0] = 1.0 / ((MY_FLOAT) 1.0 - theZero);

  B[1] = -theZero * B[0];
  Filter::setCoefficients( 2, B, 1,  &A );
}

OneZero :: ~OneZero(void)
{
}

void OneZero :: clear(void)
{
  Filter::clear();
}

void OneZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void OneZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void OneZero :: setZero(MY_FLOAT theZero)
{
  // Normalize coefficients for unity gain.
  if (theZero > 0.0)
    b[0] = 1.0 / ((MY_FLOAT) 1.0 + theZero);
  else
    b[0] = 1.0 / ((MY_FLOAT) 1.0 - theZero);

  b[1] = -theZero * b[0];
}

void OneZero :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT OneZero :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT OneZero :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT OneZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[1] * inputs[1] + b[0] * inputs[0];
  inputs[1] = inputs[0];

  return outputs[0];
}

MY_FLOAT *OneZero :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class is based on some of the famous
    Stanford/CCRMA reverbs (NRev, KipRev), which
    were based on the Chowning/Moorer/Schroeder
    reverberators using networks of simple allpass
    and comb delay filters.  This class implements
    two series allpass units and two parallel comb
    filters.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

PRCRev :: PRCRev(MY_FLOAT T60)
{
  // Delay lengths for 44100 Hz sample rate.
  int lengths[4]= {353, 1097, 1777, 2137};
  double scaler = Stk::sampleRate() / 44100.0;

  // Scale the delay lengths if necessary.
  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<4; i++)	{
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for (i=0; i<2; i++)	{
    allpassDelays[i] = new Delay( lengths[i], lengths[i] );
    combDelays[i] = new Delay( lengths[i+2], lengths[i+2] );
    combCoefficient[i] = pow(10.0,(-3 * lengths[i+2] / (T60 * Stk::sampleRate())));
  }

  allpassCoefficient = 0.7;
  effectMix = 0.5;
  this->clear();
}

PRCRev :: ~PRCRev()
{
  delete allpassDelays[0];
  delete allpassDelays[1];
  delete combDelays[0];
  delete combDelays[1];
}

void PRCRev :: clear()
{
  allpassDelays[0]->clear();
  allpassDelays[1]->clear();
  combDelays[0]->clear();
  combDelays[1]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

MY_FLOAT PRCRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp, temp0, temp1, temp2, temp3;

  temp = allpassDelays[0]->lastOut();
  temp0 = allpassCoefficient * temp;
  temp0 += input;
  allpassDelays[0]->tick(temp0);
  temp0 = -(allpassCoefficient * temp0) + temp;
    
  temp = allpassDelays[1]->lastOut();
  temp1 = allpassCoefficient * temp;
  temp1 += temp0;
  allpassDelays[1]->tick(temp1);
  temp1 = -(allpassCoefficient * temp1) + temp;
    
  temp2 = temp1 + (combCoefficient[0] * combDelays[0]->lastOut());
  temp3 = temp1 + (combCoefficient[1] * combDelays[1]->lastOut());

  lastOutput[0] = effectMix * (combDelays[0]->tick(temp2));
  lastOutput[1] = effectMix * (combDelays[1]->tick(temp3));
  temp = (MY_FLOAT) (1.0 - effectMix) * input;
  lastOutput[0] += temp;
  lastOutput[1] += temp;
    
  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;

}
/***************************************************/
/*! \class PercFlut
    \brief STK percussive flute FM synthesis instrument.

    This class implements algorithm 4 of the TX81Z.

    \code
    Algorithm 4 is :   4->3--\
                          2-- + -->1-->Out
    \endcode

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


PercFlut :: PercFlut()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.50 * 1.000);
  this->setRatio(1, 3.00 * 0.995);
  this->setRatio(2, 2.99 * 1.005);
  this->setRatio(3, 6.00 * 0.997);
  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[71];
  gains[2] = __FM_gains[93];
  gains[3] = __FM_gains[85];

  adsr[0]->setAllTimes( 0.05, 0.05, __FM_susLevels[14], 0.05);
  adsr[1]->setAllTimes( 0.02, 0.50, __FM_susLevels[13], 0.5);
  adsr[2]->setAllTimes( 0.02, 0.30, __FM_susLevels[11], 0.05);
  adsr[3]->setAllTimes( 0.02, 0.05, __FM_susLevels[13], 0.01);

  twozero->setGain( 0.0 );
  modDepth = 0.005;
}  

PercFlut :: ~PercFlut()
{
}

void PercFlut :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;
}

void PercFlut :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99] * 0.5;
  gains[1] = amplitude * __FM_gains[71] * 0.5;
  gains[2] = amplitude * __FM_gains[93] * 0.5;
  gains[3] = amplitude * __FM_gains[85] * 0.5;
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "PercFlut: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT PercFlut :: tick()
{
  register MY_FLOAT temp;

  temp = vibrato->tick() * modDepth * (MY_FLOAT) 0.2;    
  waves[0]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[3]);
    
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();

  twozero->tick(temp);
  waves[2]->addPhaseOffset(temp);
  temp = (1.0 - (control2 * 0.5)) * gains[2] * adsr[2]->tick() * waves[2]->tick();

  temp += control2 * 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
  lastOutput = temp * (MY_FLOAT) 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class Phonemes
    \brief STK phonemes table.

    This class does nothing other than declare a
    set of 32 static phoneme formant parameters
    and provide access to those values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

const char Phonemes :: phonemeNames[32][4] = 
  {"eee", "ihh", "ehh", "aaa",
   "ahh", "aww", "ohh", "uhh",
   "uuu", "ooo", "rrr", "lll",
   "mmm", "nnn", "nng", "ngg",
   "fff", "sss", "thh", "shh",
   "xxx", "hee", "hoo", "hah",
   "bbb", "ddd", "jjj", "ggg",
   "vvv", "zzz", "thz", "zhh"
  };

const MY_FLOAT Phonemes :: phonemeGains[32][2] =
  {{1.0, 0.0},    // eee
   {1.0, 0.0},    // ihh
   {1.0, 0.0},    // ehh
   {1.0, 0.0},    // aaa

   {1.0, 0.0},    // ahh
   {1.0, 0.0},    // aww
   {1.0, 0.0},    // ohh
   {1.0, 0.0},    // uhh

   {1.0, 0.0},    // uuu
   {1.0, 0.0},    // ooo
   {1.0, 0.0},    // rrr
   {1.0, 0.0},    // lll

   {1.0, 0.0},    // mmm
   {1.0, 0.0},    // nnn
   {1.0, 0.0},    // nng
   {1.0, 0.0},    // ngg

   {0.0, 0.7},    // fff
   {0.0, 0.7},    // sss
   {0.0, 0.7},    // thh
   {0.0, 0.7},    // shh

   {0.0, 0.7},    // xxx
   {0.0, 0.1},    // hee
   {0.0, 0.1},    // hoo
   {0.0, 0.1},    // hah

   {1.0, 0.1},    // bbb
   {1.0, 0.1},    // ddd
   {1.0, 0.1},    // jjj
   {1.0, 0.1},    // ggg

   {1.0, 1.0},    // vvv
   {1.0, 1.0},    // zzz
   {1.0, 1.0},    // thz
   {1.0, 1.0}     // zhh
  };

const MY_FLOAT Phonemes :: phonemeParameters[32][4][3] =
  {{  { 273, 0.996,  10},       // eee (beet)
      {2086, 0.945, -16}, 
      {2754, 0.979, -12}, 
      {3270, 0.440, -17}},
   {  { 385, 0.987,  10},       // ihh (bit)
      {2056, 0.930, -20},
      {2587, 0.890, -20}, 
      {3150, 0.400, -20}},
   {  { 515, 0.977,  10},       // ehh (bet)
      {1805, 0.810, -10}, 
      {2526, 0.875, -10}, 
      {3103, 0.400, -13}},
   {  { 773, 0.950,  10},       // aaa (bat)
      {1676, 0.830,  -6},
      {2380, 0.880, -20}, 
      {3027, 0.600, -20}},
     
   {  { 770, 0.950,   0},       // ahh (father)
      {1153, 0.970,  -9},
      {2450, 0.780, -29},
      {3140, 0.800, -39}},
   {  { 637, 0.910,   0},       // aww (bought)
      { 895, 0.900,  -3},
      {2556, 0.950, -17},
      {3070, 0.910, -20}},
   {  { 637, 0.910,   0},       // ohh (bone)  NOTE::  same as aww (bought)
      { 895, 0.900,  -3},
      {2556, 0.950, -17},
      {3070, 0.910, -20}},
   {  { 561, 0.965,   0},       // uhh (but)
      {1084, 0.930, -10}, 
      {2541, 0.930, -15}, 
      {3345, 0.900, -20}},
    
   {  { 515, 0.976,   0},       // uuu (foot)
      {1031, 0.950,  -3},
      {2572, 0.960, -11},
      {3345, 0.960, -20}},
   {  { 349, 0.986, -10},       // ooo (boot)
      { 918, 0.940, -20},
      {2350, 0.960, -27},
      {2731, 0.950, -33}},
   {  { 394, 0.959, -10},       // rrr (bird)
      {1297, 0.780, -16},
      {1441, 0.980, -16},
      {2754, 0.950, -40}},
   {  { 462, 0.990,  +5},       // lll (lull)
      {1200, 0.640, -10},
      {2500, 0.200, -20},
      {3000, 0.100, -30}},
     
   {  { 265, 0.987, -10},       // mmm (mom)
      {1176, 0.940, -22},
      {2352, 0.970, -20},
      {3277, 0.940, -31}},
   {  { 204, 0.980, -10},       // nnn (nun)
      {1570, 0.940, -15},
      {2481, 0.980, -12},
      {3133, 0.800, -30}},
   {  { 204, 0.980, -10},       // nng (sang)    NOTE:: same as nnn
      {1570, 0.940, -15},
      {2481, 0.980, -12},
      {3133, 0.800, -30}},
   {  { 204, 0.980, -10},       // ngg (bong)    NOTE:: same as nnn
      {1570, 0.940, -15},
      {2481, 0.980, -12},
      {3133, 0.800, -30}},
     
   {  {1000, 0.300,   0},       // fff
      {2800, 0.860, -10},
      {7425, 0.740,   0},
      {8140, 0.860,   0}},
   {  {0,    0.000,   0},       // sss
      {2000, 0.700, -15},
      {5257, 0.750,  -3}, 
      {7171, 0.840,   0}},
   {  { 100, 0.900,   0},       // thh
      {4000, 0.500, -20},
      {5500, 0.500, -15},
      {8000, 0.400, -20}},
   {  {2693, 0.940,   0},       // shh
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},

   {  {1000, 0.300, -10},       // xxx           NOTE:: Not Really Done Yet
      {2800, 0.860, -10},
      {7425, 0.740,   0},
      {8140, 0.860,   0}},
   {  { 273, 0.996, -40},       // hee (beet)    (noisy eee)
      {2086, 0.945, -16}, 
      {2754, 0.979, -12}, 
      {3270, 0.440, -17}},
   {  { 349, 0.986, -40},       // hoo (boot)    (noisy ooo)
      { 918, 0.940, -10},
      {2350, 0.960, -17},
      {2731, 0.950, -23}},
   {  { 770, 0.950, -40},       // hah (father)  (noisy ahh)
      {1153, 0.970,  -3},
      {2450, 0.780, -20},
      {3140, 0.800, -32}},
     
   {  {2000, 0.700, -20},       // bbb           NOTE:: Not Really Done Yet
      {5257, 0.750, -15},
      {7171, 0.840,  -3}, 
      {9000, 0.900,   0}},
   {  { 100, 0.900,   0},       // ddd           NOTE:: Not Really Done Yet
      {4000, 0.500, -20},
      {5500, 0.500, -15},
      {8000, 0.400, -20}},
   {  {2693, 0.940,   0},       // jjj           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},
   {  {2693, 0.940,   0},       // ggg           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},
     
   {  {2000, 0.700, -20},       // vvv           NOTE:: Not Really Done Yet
      {5257, 0.750, -15},
      {7171, 0.840,  -3}, 
      {9000, 0.900,   0}},
   {  { 100, 0.900,   0},       // zzz           NOTE:: Not Really Done Yet
      {4000, 0.500, -20},
      {5500, 0.500, -15},
      {8000, 0.400, -20}},
   {  {2693, 0.940,   0},       // thz           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},
   {  {2693, 0.940,   0},       // zhh           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}}
  };

Phonemes :: Phonemes(void)
{
}

Phonemes :: ~Phonemes(void)
{
}

const char *Phonemes :: name( unsigned int index )
{
  if ( index > 31 ) {
    std::cerr << "[chuck](via STK): Phonemes: name index is greater than 31!" << std::endl;
    return 0;
  }
  return phonemeNames[index];
}

MY_FLOAT Phonemes :: voiceGain( unsigned int index )
{
  if ( index > 31 ) {
    std::cerr << "[chuck](via STK): Phonemes: voiceGain index is greater than 31!" << std::endl;
    return 0.0;
  }
  return phonemeGains[index][0];
}

MY_FLOAT Phonemes :: noiseGain( unsigned int index )
{
  if ( index > 31 ) {
    std::cerr << "[chuck](via STK): Phonemes: noiseGain index is greater than 31!" << std::endl;
    return 0.0;
  }
  return phonemeGains[index][1];
}

MY_FLOAT Phonemes :: formantFrequency( unsigned int index, unsigned int partial )
{
  if ( index > 31 ) {
    std::cerr << "[chuck](via STK): Phonemes: formantFrequency index is greater than 31!" << std::endl;
    return 0.0;
  }
  if ( partial > 3 ) {
    std::cerr << "[chuck](via STK): Phonemes: formantFrequency partial is greater than 3!" << std::endl;
    return 0.0;
  }
  return phonemeParameters[index][partial][0];
}

MY_FLOAT Phonemes :: formantRadius( unsigned int index, unsigned int partial )
{
  if ( index > 31 ) {
    std::cerr << "[chuck](via STK): Phonemes: formantRadius index is greater than 31!" << std::endl;
    return 0.0;
  }
  if ( partial > 3 ) {
    std::cerr << "[chuck](via STK): Phonemes: formantRadius partial is greater than 3!" << std::endl;
    return 0.0;
  }
  return phonemeParameters[index][partial][1];
}

MY_FLOAT Phonemes :: formantGain( unsigned int index, unsigned int partial )
{
  if ( index > 31 ) {
    std::cerr << "[chuck](via STK): Phonemes: formantGain index is greater than 31!" << std::endl;
    return 0.0;
  }
  if ( partial > 3 ) {
    std::cerr << "[chuck](via STK): Phonemes: formantGain partial is greater than 3!" << std::endl;
    return 0.0;
  }
  return phonemeParameters[index][partial][2];
}
/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>
#include <math.h>

PitShift :: PitShift()
{
  delay[0] = 12;
  delay[1] = 512;
  delayLine[0] = new DelayL(delay[0], (long) 1024);
  delayLine[1] = new DelayL(delay[1], (long) 1024);
  effectMix = (MY_FLOAT) 0.5;
  rate = 1.0;
}

PitShift :: ~PitShift()
{
  delete delayLine[0];
  delete delayLine[1];
}

void PitShift :: clear()
{
  delayLine[0]->clear();
  delayLine[1]->clear();
  lastOutput = 0.0;
}

void PitShift :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    std::cerr << "[chuck](via STK): PitShift: setEffectMix parameter is less than zero!" << std::endl;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    std::cerr << "[chuck](via STK): PitShift: setEffectMix parameter is greater than 1.0!" << std::endl;
    effectMix = 1.0;
  }
}

void PitShift :: setShift(MY_FLOAT shift)
{
  if (shift < 1.0)    {
    rate = 1.0 - shift; 
  }
  else if (shift > 1.0)       {
    rate = 1.0 - shift;
  }
  else {
    rate = 0.0;
    delay[0] = 512;
  }
}

MY_FLOAT PitShift :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT PitShift :: tick(MY_FLOAT input)
{
  delay[0] = delay[0] + rate;
  while (delay[0] > 1012) delay[0] -= 1000;
  while (delay[0] < 12) delay[0] += 1000;
  delay[1] = delay[0] + 500;
  while (delay[1] > 1012) delay[1] -= 1000;
  while (delay[1] < 12) delay[1] += 1000;
  delayLine[0]->setDelay((long)delay[0]);
  delayLine[1]->setDelay((long)delay[1]);
  env[1] = fabs(delay[0] - 512) * 0.002;
  env[0] = 1.0 - env[1];
  lastOutput =  env[0] * delayLine[0]->tick(input);
  lastOutput += env[1] * delayLine[1]->tick(input);
  lastOutput *= effectMix;
  lastOutput += (1.0 - effectMix) * input;
  return lastOutput;
}

MY_FLOAT *PitShift :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

/***************************************************/
/*! \class PluckTwo
    \brief STK enhanced plucked string model class.

    This class implements an enhanced two-string,
    plucked physical model, a la Jaffe-Smith,
    Smith, and others.

    PluckTwo is an abstract class, with no excitation
    specified.  Therefore, it can't be directly
    instantiated.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


PluckTwo :: PluckTwo(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  baseLoopGain = (MY_FLOAT) 0.995;
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA((MY_FLOAT)(length / 2.0), length);
  delayLine2 = new DelayA((MY_FLOAT)(length / 2.0), length);
  combDelay = new DelayL((MY_FLOAT)(length / 2.0), length);
  filter = new OneZero;
  filter2 = new OneZero;
  pluckAmplitude = (MY_FLOAT) 0.3;
  pluckPosition = (MY_FLOAT) 0.4;
  detuning = (MY_FLOAT) 0.995;
  lastFrequency = lowestFrequency * (MY_FLOAT) 2.0;
  lastLength = length * (MY_FLOAT) 0.5;
}

PluckTwo :: ~PluckTwo()
{
  delete delayLine;
  delete delayLine2;
  delete combDelay;
  delete filter;
  delete filter2;
}

void PluckTwo :: clear()
{
  delayLine->clear();
  delayLine2->clear();
  combDelay->clear();
  filter->clear();
  filter2->clear();
}

void PluckTwo :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency;
  if ( lastFrequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): PluckTwo: setFrequency parameter less than or equal to zero!" << std::endl;
    lastFrequency = 220.0;
  }

  // Delay = length - approximate filter delay.
  lastLength = ( Stk::sampleRate() / lastFrequency);
  MY_FLOAT delay = (lastLength / detuning) - (MY_FLOAT) 0.5;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length ) delay = length;
  delayLine->setDelay( delay );

  delay = (lastLength * detuning) - (MY_FLOAT) 0.5;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length ) delay = length;
  delayLine2->setDelay( delay );

  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if ( loopGain > 1.0 ) loopGain = (MY_FLOAT) 0.99999;
}

void PluckTwo :: setDetune(MY_FLOAT detune)
{
  detuning = detune;
  if ( detuning <= 0.0 ) {
    std::cerr << "[chuck](via STK): PluckTwo: setDetune parameter less than or equal to zero!" << std::endl;
    detuning = 0.1;
  }
  delayLine->setDelay(( lastLength / detuning) - (MY_FLOAT) 0.5);
  delayLine2->setDelay( (lastLength * detuning) - (MY_FLOAT) 0.5);
}

void PluckTwo :: setFreqAndDetune(MY_FLOAT frequency, MY_FLOAT detune)
{
  detuning = detune;
  this->setFrequency(frequency);
}

void PluckTwo :: setPluckPosition(MY_FLOAT position)
{
  pluckPosition = position;
  if ( position < 0.0 ) {
    std::cerr << "[chuck](via STK): PluckTwo: setPluckPosition parameter is less than zero!" << std::endl;
    pluckPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    std::cerr << "[chuck](via STK): PluckTwo: setPluckPosition parameter is greater than 1.0!" << std::endl;
    pluckPosition = 1.0;
  }
}

void PluckTwo :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFrequency * (MY_FLOAT) 0.000005);
  if ( loopGain > 0.99999 ) loopGain = (MY_FLOAT) 0.99999;
}

void PluckTwo :: noteOff(MY_FLOAT amplitude)
{
  loopGain =  ((MY_FLOAT) 1.0 - amplitude) * (MY_FLOAT) 0.5;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): PluckTwo: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

/***************************************************/
/*! \class Plucked
    \brief STK plucked string model class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Plucked :: Plucked(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  loopFilter = new OneZero;
  pickFilter = new OnePole;
  noise = new Noise;
  this->clear();
}

Plucked :: ~Plucked()
{
  delete delayLine;
  delete loopFilter;
  delete pickFilter;
  delete noise;
}

void Plucked :: clear()
{
  delayLine->clear();
  loopFilter->clear();
  pickFilter->clear();
}

void Plucked :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Plucked: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);
  loopGain = 0.995 + (freakency * 0.000005);
  if ( loopGain >= 1.0 ) loopGain = (MY_FLOAT) 0.99999;
}

void Plucked :: pluck(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    std::cerr << "[chuck](via STK): Plucked: pluck amplitude greater than 1.0!" << std::endl;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    std::cerr << "[chuck](via STK): Plucked: pluck amplitude less than zero!" << std::endl;
    gain = 0.0;
  }

  pickFilter->setPole((MY_FLOAT) 0.999 - (gain * (MY_FLOAT) 0.15));
  pickFilter->setGain(gain * (MY_FLOAT) 0.5);
  for (long i=0; i<length; i++)
    // Fill delay with noise additively with current contents.
    delayLine->tick( 0.6 * delayLine->lastOut() + pickFilter->tick( noise->tick() ) );
}

void Plucked :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Plucked: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Plucked :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    std::cerr << "[chuck](via STK): Plucked: noteOff amplitude greater than 1.0!" << std::endl;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    std::cerr << "[chuck](via STK): Plucked: noteOff amplitude less than or zero!" << std::endl;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Plucked: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Plucked :: tick()
{
  // Here's the whole inner loop of the instrument!!
  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) ); 
  lastOutput *= (MY_FLOAT) 3.0;
  return lastOutput;
}
/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This protected Filter subclass implements
    a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass
    filter with a given coefficient.  Another
    method is provided to create a DC blocking filter.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


PoleZero :: PoleZero() : Filter()
{
  // Default setting for pass-through.
  MY_FLOAT B[2] = {1.0, 0.0};
  MY_FLOAT A[2] = {1.0, 0.0};
  Filter::setCoefficients( 2, B, 2, A );
}

PoleZero :: ~PoleZero()
{
}

void PoleZero :: clear(void)
{
  Filter::clear();
}

void PoleZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void PoleZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void PoleZero :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void PoleZero :: setAllpass(MY_FLOAT coefficient)
{
  b[0] = coefficient;
  b[1] = 1.0;
  a[0] = 1.0; // just in case
  a[1] = coefficient;
}

void PoleZero :: setBlockZero(MY_FLOAT thePole)
{
  b[0] = 1.0;
  b[1] = -1.0;
  a[0] = 1.0; // just in case
  a[1] = -thePole;
}

void PoleZero :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT PoleZero :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT PoleZero :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT PoleZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] + b[1] * inputs[1] - a[1] * outputs[1];
  inputs[1] = inputs[0];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *PoleZero :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

/***************************************************/
/*! \class ReedTabl
    \brief STK reed table class.

    This class implements a simple one breakpoint,
    non-linear reed function, as described by
    Smith (1986).  This function is based on a
    memoryless non-linear spring model of the reed
    (the reed mass is ignored) which saturates when
    the reed collides with the mouthpiece facing.

    See McIntyre, Schumacher, & Woodhouse (1983),
    Smith (1986), Hirschman, Cook, Scavone, and
    others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


ReedTabl :: ReedTabl()
{
  offSet = (MY_FLOAT) 0.6;  // Offset is a bias, related to reed rest position.
  slope = (MY_FLOAT) -0.8;  // Slope corresponds loosely to reed stiffness.
}

ReedTabl :: ~ReedTabl()
{

}

void ReedTabl :: setOffset(MY_FLOAT aValue)
{
  offSet = aValue;
}

void ReedTabl :: setSlope(MY_FLOAT aValue)
{
  slope = aValue;
}

MY_FLOAT ReedTabl :: lastOut() const
{
    return lastOutput;
}

MY_FLOAT ReedTabl :: tick(MY_FLOAT input)    
{
  // The input is differential pressure across the reed.
  lastOutput = offSet + (slope * input);

  // If output is > 1, the reed has slammed shut and the
  // reflection function value saturates at 1.0.
  if (lastOutput > 1.0) lastOutput = (MY_FLOAT) 1.0;

  // This is nearly impossible in a physical system, but
  // a reflection function value of -1.0 corresponds to
  // an open end (and no discontinuity in bore profile).
  if (lastOutput < -1.0) lastOutput = (MY_FLOAT) -1.0;
  return lastOutput;
}

MY_FLOAT *ReedTabl :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Resonate :: Resonate()
{
  adsr = new ADSR;
  noise = new Noise;

  filter = new BiQuad;
  poleFrequency = 4000.0;
  poleRadius = 0.95;
  // Set the filter parameters.
  filter->setResonance( poleFrequency, poleRadius, TRUE );
  zeroFrequency = 0.0;
  zeroRadius = 0.0;
}  

Resonate :: ~Resonate()
{
  delete adsr;
  delete filter;
  delete noise;
}

void Resonate :: keyOn()
{
  adsr->keyOn();
}

void Resonate :: keyOff()
{
  adsr->keyOff();
}

void Resonate :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  adsr->setTarget( amplitude );
  this->keyOn();
  this->setResonance(frequency, poleRadius);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Resonate: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}
void Resonate :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Resonate: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void Resonate :: setResonance(MY_FLOAT frequency, MY_FLOAT radius)
{
  poleFrequency = frequency;
  if ( frequency < 0.0 ) {
    std::cerr << "[chuck](via STK): Resonate: setResonance frequency parameter is less than zero!" << std::endl;
    poleFrequency = 0.0;
  }

  poleRadius = radius;
  if ( radius < 0.0 ) {
    std::cerr << "[chuck](via STK): Resonate: setResonance radius parameter is less than 0.0!" << std::endl;
    poleRadius = 0.0;
  }
  else if ( radius >= 1.0 ) {
    std::cerr << "[chuck](via STK): Resonate: setResonance radius parameter is greater than or equal to 1.0, which is unstable!" << std::endl;
    poleRadius = 0.9999;
  }
  filter->setResonance( poleFrequency, poleRadius, TRUE );
}

void Resonate :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  zeroFrequency = frequency;
  if ( frequency < 0.0 ) {
    std::cerr << "[chuck](via STK): Resonate: setNotch frequency parameter is less than zero!" << std::endl;
    zeroFrequency = 0.0;
  }

  zeroRadius = radius;
  if ( radius < 0.0 ) {
    std::cerr << "[chuck](via STK): Resonate: setNotch radius parameter is less than 0.0!" << std::endl;
    zeroRadius = 0.0;
  }
  
  filter->setNotch( zeroFrequency, zeroRadius );
}

void Resonate :: setEqualGainZeroes()
{
  filter->setEqualGainZeroes();
}

MY_FLOAT Resonate :: tick()
{
  lastOutput = filter->tick(noise->tick());
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Resonate :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Resonate: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Resonate: Control value greater than 128.0!" << std::endl;
  }

  if (number == 2) // 2
    setResonance( norm * Stk::sampleRate() * 0.5, poleRadius );
  else if (number == 4) // 4
    setResonance( poleFrequency, norm*0.9999 );
  else if (number == 11) // 11
    this->setNotch( norm * Stk::sampleRate() * 0.5, zeroRadius );
  else if (number == 1)
    this->setNotch( zeroFrequency, norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): Resonate: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Resonate: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Reverb
    \brief STK abstract reverberator parent class.

    This class provides common functionality for
    STK reverberator subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Reverb :: Reverb()
{
}

Reverb :: ~Reverb()
{
}

void Reverb :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

MY_FLOAT Reverb :: lastOut() const
{
  return (lastOutput[0] + lastOutput[1]) * 0.5;
}

MY_FLOAT Reverb :: lastOutLeft() const
{
  return lastOutput[0];
}

MY_FLOAT Reverb :: lastOutRight() const
{
  return lastOutput[1];
}

MY_FLOAT *Reverb :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

bool Reverb :: isPrime(int number)
{
  if (number == 2) return true;
  if (number & 1)	{
	  for (int i=3; i<(int)sqrt((double)number)+1; i+=2)
		  if ( (number % i) == 0) return false;
	  return true; /* prime */
	}
  else return false; /* even */
}
/***************************************************/
/*! \class Rhodey
    \brief STK Fender Rhodes-like electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Rhodey :: Rhodey()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0);
  this->setRatio(1, 0.5);
  this->setRatio(2, 1.0);
  this->setRatio(3, 15.0);

  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[90];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[67];

  adsr[0]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 1.00, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.001, 0.25, 0.0, 0.04);

  twozero->setGain((MY_FLOAT) 1.0);
}  

Rhodey :: ~Rhodey()
{
}

void Rhodey :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency * (MY_FLOAT) 2.0;

  for (int i=0; i<nOperators; i++ )
    waves[i]->setFrequency( baseFrequency * ratios[i] );
}

void Rhodey :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99];
  gains[1] = amplitude * __FM_gains[90];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[67];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "Rhodey: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Rhodey :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class SKINI
    \brief STK SKINI parsing class

    This class parses SKINI formatted text
    messages.  It can be used to parse individual
    messages or it can be passed an entire file.
    The file specification is Perry's and his
    alone, but it's all text so it shouldn't be to
    hard to figure out.

    SKINI (Synthesis toolKit Instrument Network
    Interface) is like MIDI, but allows for
    floating-point control changes, note numbers,
    etc.  The following example causes a sharp
    middle C to be played with a velocity of 111.132:

    noteOn  60.01  111.13

    See also SKINI.txt.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <string.h>
#include <stdlib.h>

// Constructor for use when parsing SKINI strings (coming over socket
// for example.  Use parseThis() method with string pointer.
SKINI :: SKINI()
{
}

//  Constructor for reading SKINI files ... use nextMessage() method.
SKINI :: SKINI(char *fileName)
{
  char msg[256];

  myFile = fopen(fileName,"r");
  if ((long) myFile < 0) {
    sprintf(msg, "[chuck](via SKINI): Could not open or find file (%s).", fileName);
    handleError(msg, StkError::FILE_NOT_FOUND);
  }

  this->nextMessage();
}

SKINI :: ~SKINI()
{
}

/*****************  SOME HANDY ROUTINES   *******************/


#define __SK_MAX_FIELDS_ 5
#define __SK_MAX_SIZE_ 32

short ignore(char aChar)
{
  short ignoreIt = 0;
  if (aChar == 0)   ignoreIt = 1;        //  Null String Termination
  if (aChar == '\n')  ignoreIt = 1;      //  Carraige Return???
  if (aChar == '/') ignoreIt = 2;        //  Comment Line
  return ignoreIt;
}

short delimit(char aChar)
{
  if (aChar == ' ' ||           // Space
      aChar == ','  ||          // Or Comma
      aChar == '\t')            // Or Tab
    return 1;
  else
    return 0;
}

short nextChar(char* aString)
{
  int i;

  for (i=0;i<__SK_MAX_SIZE_;i++)  {
    if (        aString[i] != ' ' &&             // Space
                aString[i] != ','  &&            // Or Comma
                aString[i] != '\t'     )         // Or Tab
	    return i;
  }
  return 1024;
}

int subStrings(char *aString, 
	   char someStrings[__SK_MAX_FIELDS_][__SK_MAX_SIZE_], 
	   int  somePointrs[__SK_MAX_FIELDS_],
	   char *remainderString)
{
  int notDone,howMany,point,temp;
  notDone = 1;
  howMany = 0;
  point = 0;
  temp = nextChar(aString);
  if (temp >= __SK_MAX_SIZE_) {
    notDone = 0;
    // printf("Confusion here: Ignoring this line\n");
    // printf("%s\n",aString);
    return howMany;
  }
  point = temp;
  somePointrs[howMany] = point;
  temp = 0;
  while (notDone)    {
    if (aString[point] == '\n') {
      notDone = 0;
    }
    else        {
      someStrings[howMany][temp++] = aString[point++];
      if (temp >= __SK_MAX_SIZE_)      {
        howMany = 0;
        return howMany;
      }
      if (delimit(aString[point]) || aString[point] == '\n') {
        someStrings[howMany][temp] = 0;
        howMany += 1;
        if (howMany < __SK_MAX_FIELDS_)       {
          temp = nextChar(&aString[point]);
          point += temp;
          somePointrs[howMany-1] = point;
          temp = 0;
        }
        else   {
          temp = 0;
          somePointrs[howMany-1] = point;
          while(aString[point] != '\n')
            remainderString[temp++] = aString[point++];
          remainderString[temp] = aString[point];
        }
      }
    }   
  }
  //     printf("Got: %i Strings:\n",howMany);
  //     for (temp=0;temp<howMany;temp++) 
  //         printf("%s\n",someStrings[temp]);
  return howMany;
	 
}

/****************  THE ENCHILLADA !!!!  **********************/
/***   This function parses a single string (if it can)   ****/
/***   of SKINI message, setting the appropriate variables ***/
/*************************************************************/

long SKINI :: parseThis(char* aString)
{
  int which,aField;
  int temp,temp2;
  char someStrings[__SK_MAX_FIELDS_][__SK_MAX_SIZE_];
  int  somePointrs[__SK_MAX_FIELDS_];
     
  temp = nextChar(aString);
  if ((which = ignore(aString[temp]))) {
    if (which == 2) printf("// CommentLine: %s\n",aString);
    messageType = 0;
    return messageType;
  }
  else        {
    temp = subStrings(aString,someStrings,somePointrs,remainderString);
    if (temp > 0)    
      which = 0;
    aField = 0;
    strcpy(msgTypeString,someStrings[aField]);
    while ((which < __SK_MaxMsgTypes_) && 
           (strcmp(msgTypeString,
                   skini_msgs[which].messageString)))  {
	    which += 1;  
    }
    if (which >= __SK_MaxMsgTypes_)  {
	    messageType = 0;
	    printf("Couldn't parse this message field: =%s\n %s\n",
             msgTypeString,aString);
	    return messageType;
    }
    else  {
	    messageType = skini_msgs[which].type;
      // printf("Message Token = %s type = %i\n", msgTypeString,messageType);
    }
    aField += 1;

    if (someStrings[aField][0] == '=') {
	    deltaTime = (MY_FLOAT) atof(&someStrings[aField][1]);
	    deltaTime = -deltaTime;
    }
    else {
	    deltaTime = (MY_FLOAT) atof(someStrings[aField]);
    }
    // printf("DeltaTime = %f\n",deltaTime);
    aField += 1;
	
    channel = atoi(someStrings[aField]);    
    // printf("Channel = %i\n",channel);
    aField += 1;
	
    if (skini_msgs[which].data2 != NOPE)    {
	    if (skini_msgs[which].data2 == SK_INT)       {
        byteTwoInt = atoi(someStrings[aField]);    
        byteTwo = (MY_FLOAT) byteTwoInt;
	    }
	    else if (skini_msgs[which].data2 == SK_DBL)       {
        byteTwo = (MY_FLOAT) atof(someStrings[aField]);    
        byteTwoInt = (long) byteTwo;
	    }
	    else if (skini_msgs[which].data2 == SK_STR)       {
        temp = somePointrs[aField-1];    /*  Hack Danger Here, Why -1??? */
        temp2 = 0;
        while (aString[temp] != '\n')   { 
          remainderString[temp2++] = aString[temp++];
        }
        remainderString[temp2] = 0;
      }
	    else {
        byteTwoInt = skini_msgs[which].data2;
        byteTwo = (MY_FLOAT) byteTwoInt;
        aField -= 1;
	    }
	    
	    aField += 1;
	    if (skini_msgs[which].data3 != NOPE)    {
        if (skini_msgs[which].data3 == SK_INT)        {
          byteThreeInt = atoi(someStrings[aField]);    
          byteThree = (MY_FLOAT) byteThreeInt;
        }
        else if (skini_msgs[which].data3 == SK_DBL)   {
          byteThree = (MY_FLOAT) atof(someStrings[aField]);    
          byteThreeInt = (long) byteThree;
        }
        else if (skini_msgs[which].data3 == SK_STR)   {
          temp = somePointrs[aField-1];	/*  Hack Danger Here, Why -1??? */
          temp2 = 0;
          while (aString[temp] != '\n')   { 
            remainderString[temp2++] = aString[temp++];
          }
          remainderString[temp2] = 0;
        }
        else {
          byteThreeInt = skini_msgs[which].data3;
          byteThree = (MY_FLOAT) byteThreeInt;
        }
	    }
	    else {
        byteThreeInt = byteTwoInt;
        byteThree = byteTwo;
	    }
    }
  }
  return messageType;
}

long SKINI ::  nextMessage()
{
  int notDone;
  char inputString[1024];

  notDone = 1;
  while (notDone)     {
    notDone = 0;
    if (!fgets(inputString,1024,myFile)) {    
	    printf("// End of Score. Thanks for using SKINI!!\n");
	    messageType = -1;
	    return messageType;
    }
    else if (parseThis(inputString) == 0)   {
	    notDone = 1;
    }
  }
  return messageType;
}

long SKINI ::  getType() const
{
  return messageType;
}
 
long SKINI ::  getChannel() const
{
  return channel;
}

MY_FLOAT SKINI :: getDelta() const
{
  return deltaTime;
}

MY_FLOAT SKINI :: getByteTwo() const
{
  return byteTwo;
}

long SKINI :: getByteTwoInt() const
{
  return byteTwoInt;
}

MY_FLOAT SKINI :: getByteThree() const
{
  return byteThree;
}

long SKINI :: getByteThreeInt() const
{
  return byteThreeInt;
}

const char* SKINI :: getRemainderString()
{
  return remainderString;
}

const char* SKINI :: getMessageTypeString()
{
  return msgTypeString;
}

const char* SKINI :: whatsThisType(long type)
{
  int i = 0;
  whatString[0] = 0;
  for ( i=0; i<__SK_MaxMsgTypes_; i++ ) {
    if ( type == skini_msgs[i].type ) {
	    strcat(whatString, skini_msgs[i].messageString);
	    strcat(whatString, ",");
    }
  }
  return whatString;            
}

const char* SKINI :: whatsThisController(long contNum)
{
  int i = 0;
  whatString[0] = 0;
  for ( i=0; i<__SK_MaxMsgTypes_; i++) {
    if ( skini_msgs[i].type == __SK_ControlChange_
        && contNum == skini_msgs[i].data2) {
	    strcat(whatString, skini_msgs[i].messageString);
	    strcat(whatString, ",");
    }
  }
  return whatString;
}


/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument contains up to 5 attack waves,
    5 looped waves, and an ADSR envelope.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Sampler :: Sampler()
{
  // We don't make the waves here yet, because
  // we don't know what they will be.
  adsr = new ADSR;
  baseFrequency = 440.0;
  filter = new OnePole;
  attackGain = 0.25;
  loopGain = 0.25;
  whichOne = 0;
}  

Sampler :: ~Sampler()
{
  delete adsr;
  delete filter;
}

void Sampler :: keyOn()
{
  adsr->keyOn();
  attacks[0]->reset();
}

void Sampler :: keyOff()
{
  adsr->keyOff();
}

void Sampler :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  cerr << "Sampler: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Sampler :: tick()
{
  lastOutput = attackGain * attacks[whichOne]->tick();
  lastOutput += loopGain * loops[whichOne]->tick();
  lastOutput = filter->tick(lastOutput);
  lastOutput *= adsr->tick();
  return lastOutput;
}
/***************************************************/
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Saxofony :: Saxofony(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  // Initialize blowing position to 0.2 of length / 2.
  position = 0.2;
  delays[0] = (DelayL *) new DelayL( (1.0-position) * (length >> 1), length );
  delays[1] = (DelayL *) new DelayL( position * (length >> 1), length );

  reedTable = new ReedTabl;
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) 0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);

  outputGain = (MY_FLOAT) 0.3;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.1;
}

Saxofony :: ~Saxofony()
{
  delete delays[0];
  delete delays[1];
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibrato;
}

void Saxofony :: clear()
{
  delays[0]->clear();
  delays[1]->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Saxofony :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Saxofony: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 3.0;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;

  delays[0]->setDelay((1.0-position) * delay);
  delays[1]->setDelay(position * delay);
}

void Saxofony :: setBlowPosition(MY_FLOAT aPosition)
{
  if (position == aPosition) return;

  if (aPosition < 0.0) position = 0.0;
  else if (aPosition > 1.0) position = 1.0;
  else position = aPosition;

  MY_FLOAT total_delay = delays[0]->getDelay();
  total_delay += delays[1]->getDelay();

  delays[0]->setDelay((1.0-position) * total_delay);
  delays[1]->setDelay(position * total_delay);
}

void Saxofony :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Saxofony :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0);
}

void Saxofony :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing((MY_FLOAT) 0.55 + (amplitude * 0.30), amplitude * 0.005);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Saxofony: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Saxofony :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.01);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Saxofony: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Saxofony :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  temp = -0.95 * filter->tick( delays[0]->lastOut() );
  lastOutput = temp - delays[1]->lastOut();
  pressureDiff = breathPressure - lastOutput;
  delays[1]->tick(temp);
  delays[0]->tick(breathPressure - (pressureDiff * reedTable->tick(pressureDiff)) - temp);

  lastOutput *= outputGain;
  return lastOutput;
}

void Saxofony :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Saxofony: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Saxofony: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_ReedStiffness_) // 2
    reedTable->setSlope( 0.1 + (0.4 * norm) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = ( norm * 0.4 );
  else if (number == 29) // 29
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = ( norm * 0.5 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope->setValue( norm );
  else if (number == 11) // 11
    this->setBlowPosition( norm );
  else if (number == 26) // reed table offset
    reedTable->setOffset(0.4 + ( norm * 0.6));
  else
    std::cerr << "[chuck](via STK): Saxofony: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Saxofony: controlChange number = " << number << ", value = " << value << std::endl;
#endif

}
/***************************************************/
/*! \class Shakers
    \brief PhISEM and PhOLIES class.

    PhISEM (Physically Informed Stochastic Event
    Modeling) is an algorithmic approach for
    simulating collisions of multiple independent
    sound producing objects.  This class is a
    meta-model that can simulate a Maraca, Sekere,
    Cabasa, Bamboo Wind Chimes, Water Drops,
    Tambourine, Sleighbells, and a Guiro.

    PhOLIES (Physically-Oriented Library of
    Imitated Environmental Sounds) is a similar
    approach for the synthesis of environmental
    sounds.  This class implements simulations of
    breaking sticks, crunchy snow (or not), a
    wrench, sandpaper, and more.

    Control Change Numbers: 
       - Shake Energy = 2
       - System Decay = 4
       - Number Of Objects = 11
       - Resonance Frequency = 1
       - Shake Energy = 128
       - Instrument Selection = 1071
        - Maraca = 0
        - Cabasa = 1
        - Sekere = 2
        - Guiro = 3
        - Water Drops = 4
        - Bamboo Chimes = 5
        - Tambourine = 6
        - Sleigh Bells = 7
        - Sticks = 8
        - Crunch = 9
        - Wrench = 10
        - Sand Paper = 11
        - Coke Can = 12
        - Next Mug = 13
        - Penny + Mug = 14
        - Nickle + Mug = 15
        - Dime + Mug = 16
        - Quarter + Mug = 17
        - Franc + Mug = 18
        - Peso + Mug = 19
        - Big Rocks = 20
        - Little Rocks = 21
        - Tuned Bamboo Chimes = 22

    by Perry R. Cook, 1996 - 1999.
*/
/***************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>

int my_random(int max) //  Return Random Int Between 0 and max
{
  int temp = (int) ((float)max * rand() / (RAND_MAX + 1.0) );
  return temp;
}

MY_FLOAT float_random(MY_FLOAT max) // Return random float between 0.0 and max
{	
  MY_FLOAT temp = (MY_FLOAT) (max * rand() / (RAND_MAX + 1.0) );
  return temp;	
}

MY_FLOAT noise_tick() //  Return random MY_FLOAT float between -1.0 and 1.0
{
  MY_FLOAT temp = (MY_FLOAT) (2.0 * rand() / (RAND_MAX + 1.0) );
  temp -= 1.0;
  return temp;
}

// Maraca
#define MARA_SOUND_DECAY 0.95
#define MARA_SYSTEM_DECAY 0.999
#define MARA_GAIN 20.0
#define MARA_NUM_BEANS 25
#define MARA_CENTER_FREQ 3200.0
#define MARA_RESON 0.96

// Sekere
#define SEKE_SOUND_DECAY 0.96
#define SEKE_SYSTEM_DECAY 0.999
#define SEKE_GAIN 20.0
#define SEKE_NUM_BEANS 64
#define SEKE_CENTER_FREQ 5500.0
#define SEKE_RESON 0.6

// Sandpaper
#define SANDPAPR_SOUND_DECAY 0.999
#define SANDPAPR_SYSTEM_DECAY 0.999
#define SANDPAPR_GAIN 0.5
#define SANDPAPR_NUM_GRAINS 128
#define SANDPAPR_CENTER_FREQ 4500.0
#define SANDPAPR_RESON 0.6

// Cabasa
#define CABA_SOUND_DECAY 0.96
#define CABA_SYSTEM_DECAY 0.997
#define CABA_GAIN 40.0
#define CABA_NUM_BEADS 512
#define CABA_CENTER_FREQ 3000.0
#define CABA_RESON 0.7

// Bamboo Wind Chimes
#define BAMB_SOUND_DECAY 0.95
#define BAMB_SYSTEM_DECAY 0.9999
#define BAMB_GAIN 2.0
#define BAMB_NUM_TUBES 1.25
#define BAMB_CENTER_FREQ0  2800.0
#define BAMB_CENTER_FREQ1  0.8 * 2800.0
#define BAMB_CENTER_FREQ2  1.2 * 2800.0
#define BAMB_RESON	   0.995

// Tuned Bamboo Wind Chimes (Anklung)
#define TBAMB_SOUND_DECAY 0.95
#define TBAMB_SYSTEM_DECAY 0.9999
#define TBAMB_GAIN 1.0
#define TBAMB_NUM_TUBES 1.25
#define TBAMB_CENTER_FREQ0 1046.6
#define TBAMB_CENTER_FREQ1  1174.8
#define TBAMB_CENTER_FREQ2  1397.0
#define TBAMB_CENTER_FREQ3  1568.0
#define TBAMB_CENTER_FREQ4  1760.0
#define TBAMB_CENTER_FREQ5  2093.3
#define TBAMB_CENTER_FREQ6  2350.0
#define TBAMB_RESON	   0.996

// Water Drops
#define WUTR_SOUND_DECAY 0.95
#define WUTR_SYSTEM_DECAY 0.996
#define WUTR_GAIN 1.0
#define WUTR_NUM_SOURCES 10
#define WUTR_CENTER_FREQ0  450.0
#define WUTR_CENTER_FREQ1  600.0
#define WUTR_CENTER_FREQ2  750.0 
#define WUTR_RESON   0.9985
#define WUTR_FREQ_SWEEP  1.0001

// Tambourine
#define TAMB_SOUND_DECAY 0.95
#define TAMB_SYSTEM_DECAY 0.9985
#define TAMB_GAIN 5.0
#define TAMB_NUM_TIMBRELS 32
#define TAMB_SHELL_FREQ 2300
#define TAMB_SHELL_GAIN 0.1
#define TAMB_SHELL_RESON 0.96
#define TAMB_CYMB_FREQ1  5600
#define TAMB_CYMB_FREQ2 8100
#define TAMB_CYMB_RESON 0.99

// Sleighbells
#define SLEI_SOUND_DECAY 0.97
#define SLEI_SYSTEM_DECAY 0.9994
#define SLEI_GAIN 1.0
#define SLEI_NUM_BELLS 32
#define SLEI_CYMB_FREQ0 2500
#define SLEI_CYMB_FREQ1 5300
#define SLEI_CYMB_FREQ2 6500
#define SLEI_CYMB_FREQ3 8300
#define SLEI_CYMB_FREQ4 9800
#define SLEI_CYMB_RESON 0.99 

// Guiro
#define GUIR_SOUND_DECAY 0.95
#define GUIR_GAIN 10.0
#define GUIR_NUM_PARTS 128
#define GUIR_GOURD_FREQ  2500.0
#define GUIR_GOURD_RESON 0.97
#define GUIR_GOURD_FREQ2  4000.0
#define GUIR_GOURD_RESON2 0.97

// Wrench
#define WRENCH_SOUND_DECAY 0.95
#define WRENCH_GAIN 5
#define WRENCH_NUM_PARTS 128
#define WRENCH_FREQ  3200.0
#define WRENCH_RESON 0.99
#define WRENCH_FREQ2  8000.0
#define WRENCH_RESON2 0.992

// Cokecan
#define COKECAN_SOUND_DECAY 0.97
#define COKECAN_SYSTEM_DECAY 0.999
#define COKECAN_GAIN 0.8
#define COKECAN_NUM_PARTS 48
#define COKECAN_HELMFREQ 370
#define COKECAN_HELM_RES  0.99
#define COKECAN_METLFREQ0 1025
#define COKECAN_METLFREQ1 1424
#define COKECAN_METLFREQ2 2149
#define COKECAN_METLFREQ3 3596
#define COKECAN_METL_RES 0.992 

// PhOLIES (Physically-Oriented Library of Imitated Environmental
// Sounds), Perry Cook, 1997-8

// Stix1
#define STIX1_SOUND_DECAY 0.96
#define STIX1_SYSTEM_DECAY 0.998
#define STIX1_GAIN 30.0
#define STIX1_NUM_BEANS 2
#define STIX1_CENTER_FREQ 5500.0
#define STIX1_RESON 0.6

// Crunch1
#define CRUNCH1_SOUND_DECAY 0.95
#define CRUNCH1_SYSTEM_DECAY 0.99806
#define CRUNCH1_GAIN 20.0
#define CRUNCH1_NUM_BEADS 7
#define CRUNCH1_CENTER_FREQ 800.0
#define CRUNCH1_RESON 0.95

// Nextmug
#define NEXTMUG_SOUND_DECAY 0.97
#define NEXTMUG_SYSTEM_DECAY 0.9995
#define NEXTMUG_GAIN 0.8
#define NEXTMUG_NUM_PARTS 3
#define NEXTMUG_FREQ0 2123
#define NEXTMUG_FREQ1 4518
#define NEXTMUG_FREQ2 8856
#define NEXTMUG_FREQ3 10753
#define NEXTMUG_RES 0.997 

#define PENNY_FREQ0 11000
#define PENNY_FREQ1 5200
#define PENNY_FREQ2 3835
#define PENNY_RES   0.999

#define NICKEL_FREQ0 5583
#define NICKEL_FREQ1 9255
#define NICKEL_FREQ2 9805
#define NICKEL_RES   0.9992

#define DIME_FREQ0 4450
#define DIME_FREQ1 4974
#define DIME_FREQ2 9945
#define DIME_RES   0.9993

#define QUARTER_FREQ0 1708
#define QUARTER_FREQ1 8863
#define QUARTER_FREQ2 9045
#define QUARTER_RES   0.9995

#define FRANC_FREQ0 5583
#define FRANC_FREQ1 11010
#define FRANC_FREQ2 1917
#define FRANC_RES   0.9995

#define PESO_FREQ0 7250
#define PESO_FREQ1 8150
#define PESO_FREQ2 10060
#define PESO_RES   0.9996

// Big Gravel
#define BIGROCKS_SOUND_DECAY 0.98
#define BIGROCKS_SYSTEM_DECAY 0.9965
#define BIGROCKS_GAIN 20.0
#define BIGROCKS_NUM_PARTS 23
#define BIGROCKS_FREQ 6460
#define BIGROCKS_RES 0.932 

// Little Gravel
#define LITLROCKS_SOUND_DECAY 0.98
#define LITLROCKS_SYSTEM_DECAY 0.99586
#define LITLROCKS_GAIN 20.0
#define LITLROCKS_NUM_PARTS 1600
#define LITLROCKS_FREQ 9000
#define LITLROCKS_RES 0.843 

// Finally ... the class code!


Shakers :: Shakers()
{
  int i;

  instType = 0;
  shakeEnergy = 0.0;
  nFreqs = 0;
  sndLevel = 0.0;

  for ( i=0; i<MAX_FREQS; i++ )	{
    inputs[i] = 0.0;
    outputs[i][0] = 0.0;
    outputs[i][1] = 0.0;
    coeffs[i][0] = 0.0;
    coeffs[i][1] = 0.0;
    gains[i] = 0.0;
    center_freqs[i] = 0.0;
    resons[i] =  0.0;
    freq_rand[i] = 0.0;
    freqalloc[i] = 0;
  }

  soundDecay = 0.0;
  systemDecay = 0.0;
  nObjects = 0.0;
  collLikely = 0.0;
  totalEnergy = 0.0;
  ratchet = 0.0;
  ratchetDelta = 0.0005;
  lastRatchetPos = 0;
  finalZ[0] = 0.0;
  finalZ[1] = 0.0;
  finalZ[2] = 0.0;
  finalZCoeffs[0] = 1.0;
  finalZCoeffs[1] = 0.0;
  finalZCoeffs[2] = 0.0;
  freq = 220.0;

  this->setupNum(instType);
}

Shakers :: ~Shakers()
{
}

#define MAX_SHAKE 2000.0

char instrs[NUM_INSTR][10] = {
  "Maraca", "Cabasa", "Sekere", "Guiro",
  "Waterdrp", "Bamboo", "Tambourn", "Sleighbl", 
  "Stix1", "Crunch1", "Wrench", "SandPapr",
  "CokeCan", "NextMug", "PennyMug", "NicklMug",
  "DimeMug", "QuartMug", "FrancMug", "PesoMug",
  "BigRocks", "LitlRoks", "TBamboo"
};

int Shakers :: setupName(char* instr)
{
  int which = 0;

  for (int i=0;i<NUM_INSTR;i++)	{
    if ( !strcmp(instr,instrs[i]) )
	    which = i;
  }

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Shakers: Setting instrument to " << instrs[which] << std::endl;
#endif

  return this->setupNum(which);
}

void Shakers :: setFinalZs(MY_FLOAT z0, MY_FLOAT z1, MY_FLOAT z2)	 {
  finalZCoeffs[0] = z0;
  finalZCoeffs[1] = z1;
  finalZCoeffs[2] = z2;
}

void Shakers :: setDecays(MY_FLOAT sndDecay, MY_FLOAT sysDecay) {
  soundDecay = sndDecay;
  systemDecay = sysDecay;
}

int Shakers :: setFreqAndReson(int which, MY_FLOAT freq, MY_FLOAT reson) {
  if (which < MAX_FREQS)	{
    resons[which] = reson;
    center_freqs[which] = freq;
    t_center_freqs[which] = freq;
    coeffs[which][1] = reson * reson;
    coeffs[which][0] = -reson * 2.0 * cos(freq * TWO_PI / Stk::sampleRate());
    return 1;
  }
  else return 0;
}

int Shakers :: setupNum(int inst)
{
  int i, rv = 0;
  MY_FLOAT temp;

  inst %= 23; // chuck hack
  if (inst == 1) { // Cabasa
    rv = inst;
    nObjects = CABA_NUM_BEADS;
    defObjs[inst] = CABA_NUM_BEADS;
    setDecays(CABA_SOUND_DECAY, CABA_SYSTEM_DECAY);
    defDecays[inst] = CABA_SYSTEM_DECAY;
    decayScale[inst] = 0.97;
    nFreqs = 1;
    baseGain = CABA_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CABA_CENTER_FREQ,CABA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst == 2) { // Sekere
    rv = inst;
    nObjects = SEKE_NUM_BEANS;
    defObjs[inst] = SEKE_NUM_BEANS;
    this->setDecays(SEKE_SOUND_DECAY,SEKE_SYSTEM_DECAY);
    defDecays[inst] = SEKE_SYSTEM_DECAY;
    decayScale[inst] = 0.94;
    nFreqs = 1;
    baseGain = SEKE_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SEKE_CENTER_FREQ,SEKE_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst == 3) { //  Guiro
    rv = inst;
    nObjects = GUIR_NUM_PARTS;
    defObjs[inst] = GUIR_NUM_PARTS;
    setDecays(GUIR_SOUND_DECAY,1.0);
    defDecays[inst] = 0.9999;
    decayScale[inst] = 1.0;
    nFreqs = 2;
    baseGain = GUIR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    setFreqAndReson(0,GUIR_GOURD_FREQ,GUIR_GOURD_RESON);
    setFreqAndReson(1,GUIR_GOURD_FREQ2,GUIR_GOURD_RESON2);
    ratchet = 0;
    ratchetPos = 10;
  }
  else if (inst == 4) { //  Water Drops
    rv = inst;
    nObjects = WUTR_NUM_SOURCES;
    defObjs[inst] = WUTR_NUM_SOURCES;
    setDecays(WUTR_SOUND_DECAY,WUTR_SYSTEM_DECAY);
    defDecays[inst] = WUTR_SYSTEM_DECAY;
    decayScale[inst] = 0.8;
    nFreqs = 3;
    baseGain = WUTR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    freqalloc[0] = 1;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.2;
    freq_rand[1] = 0.2;
    freq_rand[2] = 0.2;
    setFreqAndReson(0,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFreqAndReson(1,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFreqAndReson(2,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFinalZs(1.0,0.0,0.0);
  }
  else if (inst == 5) { // Bamboo
    rv = inst;
    nObjects = BAMB_NUM_TUBES;
    defObjs[inst] = BAMB_NUM_TUBES;
    setDecays(BAMB_SOUND_DECAY, BAMB_SYSTEM_DECAY);
    defDecays[inst] = BAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.7;
    nFreqs = 3;
    baseGain = BAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    freqalloc[0] = 1;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.2;
    freq_rand[1] = 0.2;
    freq_rand[2] = 0.2;
    setFreqAndReson(0,BAMB_CENTER_FREQ0,BAMB_RESON);
    setFreqAndReson(1,BAMB_CENTER_FREQ1,BAMB_RESON);
    setFreqAndReson(2,BAMB_CENTER_FREQ2,BAMB_RESON);
    setFinalZs(1.0,0.0,0.0);
  }
  else if (inst == 6) { // Tambourine
    rv = inst;
    nObjects = TAMB_NUM_TIMBRELS;
    defObjs[inst] = TAMB_NUM_TIMBRELS;
    setDecays(TAMB_SOUND_DECAY,TAMB_SYSTEM_DECAY);
    defDecays[inst] = TAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 3;
    baseGain = TAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp*TAMB_SHELL_GAIN;
    gains[1]=temp*0.8;
    gains[2]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.05;
    freq_rand[2] = 0.05;
    setFreqAndReson(0,TAMB_SHELL_FREQ,TAMB_SHELL_RESON);
    setFreqAndReson(1,TAMB_CYMB_FREQ1,TAMB_CYMB_RESON);
    setFreqAndReson(2,TAMB_CYMB_FREQ2,TAMB_CYMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 7) { // Sleighbell
    rv = inst;
    nObjects = SLEI_NUM_BELLS;
    defObjs[inst] = SLEI_NUM_BELLS;
    setDecays(SLEI_SOUND_DECAY,SLEI_SYSTEM_DECAY);
    defDecays[inst] = SLEI_SYSTEM_DECAY;
    decayScale[inst] = 0.9;
    nFreqs = 5;
    baseGain = SLEI_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp*0.5;
    gains[4]=temp*0.3;
    for (i=0;i<nFreqs;i++)	{
	    freqalloc[i] = 1;
	    freq_rand[i] = 0.03;
    }
    setFreqAndReson(0,SLEI_CYMB_FREQ0,SLEI_CYMB_RESON);
    setFreqAndReson(1,SLEI_CYMB_FREQ1,SLEI_CYMB_RESON);
    setFreqAndReson(2,SLEI_CYMB_FREQ2,SLEI_CYMB_RESON);
    setFreqAndReson(3,SLEI_CYMB_FREQ3,SLEI_CYMB_RESON);
    setFreqAndReson(4,SLEI_CYMB_FREQ4,SLEI_CYMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 8) { // Stix1
    rv = inst;
    nObjects = STIX1_NUM_BEANS;
    defObjs[inst] = STIX1_NUM_BEANS;
    setDecays(STIX1_SOUND_DECAY,STIX1_SYSTEM_DECAY);
    defDecays[inst] = STIX1_SYSTEM_DECAY;

    decayScale[inst] = 0.96;
    nFreqs = 1;
    baseGain = STIX1_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,STIX1_CENTER_FREQ,STIX1_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 9) { // Crunch1
    rv = inst;
    nObjects = CRUNCH1_NUM_BEADS;
    defObjs[inst] = CRUNCH1_NUM_BEADS;
    setDecays(CRUNCH1_SOUND_DECAY,CRUNCH1_SYSTEM_DECAY);
    defDecays[inst] = CRUNCH1_SYSTEM_DECAY;
    decayScale[inst] = 0.96;
    nFreqs = 1;
    baseGain = CRUNCH1_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CRUNCH1_CENTER_FREQ,CRUNCH1_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst == 10) { // Wrench
    rv = inst;
    nObjects = WRENCH_NUM_PARTS;
    defObjs[inst] = WRENCH_NUM_PARTS;
    setDecays(WRENCH_SOUND_DECAY,1.0);
    defDecays[inst] = 0.9999;
    decayScale[inst] = 0.98;
    nFreqs = 2;
    baseGain = WRENCH_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    setFreqAndReson(0,WRENCH_FREQ,WRENCH_RESON);
    setFreqAndReson(1,WRENCH_FREQ2,WRENCH_RESON2);
    ratchet = 0;
    ratchetPos = 10;
  }
  else if (inst == 11) { // Sandpapr
    rv = inst;
    nObjects = SANDPAPR_NUM_GRAINS;
    defObjs[inst] = SANDPAPR_NUM_GRAINS;
    this->setDecays(SANDPAPR_SOUND_DECAY,SANDPAPR_SYSTEM_DECAY);
    defDecays[inst] = SANDPAPR_SYSTEM_DECAY;
    decayScale[inst] = 0.97;
    nFreqs = 1;
    baseGain = SANDPAPR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SANDPAPR_CENTER_FREQ,SANDPAPR_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst == 12) { // Cokecan
    rv = inst;
    nObjects = COKECAN_NUM_PARTS;
    defObjs[inst] = COKECAN_NUM_PARTS;
    setDecays(COKECAN_SOUND_DECAY,COKECAN_SYSTEM_DECAY);
    defDecays[inst] = COKECAN_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 5;
    baseGain = COKECAN_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp*1.8;
    gains[2]=temp*1.8;
    gains[3]=temp*1.8;
    gains[4]=temp*1.8;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    setFreqAndReson(0,COKECAN_HELMFREQ,COKECAN_HELM_RES);
    setFreqAndReson(1,COKECAN_METLFREQ0,COKECAN_METL_RES);
    setFreqAndReson(2,COKECAN_METLFREQ1,COKECAN_METL_RES);
    setFreqAndReson(3,COKECAN_METLFREQ2,COKECAN_METL_RES);
    setFreqAndReson(4,COKECAN_METLFREQ3,COKECAN_METL_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst>12 && inst<20) { // Nextmug
    rv = inst;
    nObjects = NEXTMUG_NUM_PARTS;
    defObjs[inst] = NEXTMUG_NUM_PARTS;
    setDecays(NEXTMUG_SOUND_DECAY,NEXTMUG_SYSTEM_DECAY);
    defDecays[inst] = NEXTMUG_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 4;
    baseGain = NEXTMUG_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp*0.8;
    gains[2]=temp*0.6;
    gains[3]=temp*0.4;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    freqalloc[5] = 0;
    setFreqAndReson(0,NEXTMUG_FREQ0,NEXTMUG_RES);
    setFreqAndReson(1,NEXTMUG_FREQ1,NEXTMUG_RES);
    setFreqAndReson(2,NEXTMUG_FREQ2,NEXTMUG_RES);
    setFreqAndReson(3,NEXTMUG_FREQ3,NEXTMUG_RES);
    setFinalZs(1.0,0.0,-1.0);

    if (inst == 14) { // Mug + Penny
      nFreqs = 7;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,PENNY_FREQ0,PENNY_RES);
      setFreqAndReson(5,PENNY_FREQ1,PENNY_RES);
      setFreqAndReson(6,PENNY_FREQ2,PENNY_RES);
    }
    else if (inst == 15) { // Mug + Nickel
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,NICKEL_FREQ0,NICKEL_RES);
      setFreqAndReson(5,NICKEL_FREQ1,NICKEL_RES);
      setFreqAndReson(6,NICKEL_FREQ2,NICKEL_RES);
    }
    else if (inst == 16) { // Mug + Dime
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,DIME_FREQ0,DIME_RES);
      setFreqAndReson(5,DIME_FREQ1,DIME_RES);
      setFreqAndReson(6,DIME_FREQ2,DIME_RES);
    }
    else if (inst == 17) { // Mug + Quarter
      nFreqs = 6;
      gains[4] = temp*1.3;
      gains[5] = temp*1.0;
      gains[6] = temp*0.8;
      setFreqAndReson(4,QUARTER_FREQ0,QUARTER_RES);
      setFreqAndReson(5,QUARTER_FREQ1,QUARTER_RES);
      setFreqAndReson(6,QUARTER_FREQ2,QUARTER_RES);
    }
    else if (inst == 18) { // Mug + Franc
      nFreqs = 6;
      gains[4] = temp*0.7;
      gains[5] = temp*0.4;
      gains[6] = temp*0.3;
      setFreqAndReson(4,FRANC_FREQ0,FRANC_RES);
      setFreqAndReson(5,FRANC_FREQ1,FRANC_RES);
      setFreqAndReson(6,FRANC_FREQ2,FRANC_RES);
    }
    else if (inst == 19) { // Mug + Peso
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*1.2;
      gains[6] = temp*0.7;
      setFreqAndReson(4,PESO_FREQ0,PESO_RES);
      setFreqAndReson(5,PESO_FREQ1,PESO_RES);
      setFreqAndReson(6,PESO_FREQ2,PESO_RES);
    }
  }
  else if (inst == 20) { // Big Rocks
    nFreqs = 1;
    rv = inst;
    nObjects = BIGROCKS_NUM_PARTS;
    defObjs[inst] = BIGROCKS_NUM_PARTS;
    setDecays(BIGROCKS_SOUND_DECAY,BIGROCKS_SYSTEM_DECAY);
    defDecays[inst] = BIGROCKS_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    baseGain = BIGROCKS_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 1;
    freq_rand[0] = 0.11;
    setFreqAndReson(0,BIGROCKS_FREQ,BIGROCKS_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 21) { // Little Rocks
    nFreqs = 1;
    rv = inst;
    nObjects = LITLROCKS_NUM_PARTS;
    defObjs[inst] = LITLROCKS_NUM_PARTS;
    setDecays(LITLROCKS_SOUND_DECAY,LITLROCKS_SYSTEM_DECAY);
    defDecays[inst] = LITLROCKS_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    baseGain = LITLROCKS_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 1;
    freq_rand[0] = 0.18;
    setFreqAndReson(0,LITLROCKS_FREQ,LITLROCKS_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 22) { // Tuned Bamboo
    rv = inst;
    nObjects = TBAMB_NUM_TUBES;
    defObjs[inst] = TBAMB_NUM_TUBES;
    setDecays(TBAMB_SOUND_DECAY, TBAMB_SYSTEM_DECAY);
    defDecays[inst] = TBAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.7;
    nFreqs = 7;
    baseGain = TBAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp;
    gains[4]=temp;
    gains[5]=temp;
    gains[6]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    freqalloc[5] = 0;
    freqalloc[6] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    freq_rand[2] = 0.0;
    freq_rand[3] = 0.0;
    freq_rand[4] = 0.0;
    freq_rand[5] = 0.0;
    freq_rand[6] = 0.0;
    setFreqAndReson(0,TBAMB_CENTER_FREQ0,TBAMB_RESON);
    setFreqAndReson(1,TBAMB_CENTER_FREQ1,TBAMB_RESON);
    setFreqAndReson(2,TBAMB_CENTER_FREQ2,TBAMB_RESON);
    setFreqAndReson(3,TBAMB_CENTER_FREQ3,TBAMB_RESON);
    setFreqAndReson(4,TBAMB_CENTER_FREQ4,TBAMB_RESON);
    setFreqAndReson(5,TBAMB_CENTER_FREQ5,TBAMB_RESON);
    setFreqAndReson(6,TBAMB_CENTER_FREQ6,TBAMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else { // Maraca (inst == 0) or default
    rv = 0;
    nObjects = MARA_NUM_BEANS;
    defObjs[0] = MARA_NUM_BEANS;
    setDecays(MARA_SOUND_DECAY,MARA_SYSTEM_DECAY);
    defDecays[0] = MARA_SYSTEM_DECAY;
    decayScale[inst] = 0.9;
    nFreqs = 1;
    baseGain = MARA_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,MARA_CENTER_FREQ,MARA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  m_noteNum = inst; // chuck data
  return rv;
}

// chuck function!

void Shakers :: ck_noteOn(MY_FLOAT amplitude ) { 
  if (instType !=  m_noteNum) instType = this->setupNum(m_noteNum);
  shakeEnergy += amplitude * MAX_SHAKE * 0.1;
  if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  if (instType==10 || instType==3) ratchetPos += 1;
}

void Shakers :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  // Yep ... pretty kludgey, but it works!
  int noteNum = (int) ((12*log(frequency/220.0)/log(2.0)) + 57.01) % 32;
  m_noteNum = noteNum;
  if (instType !=  noteNum) instType = this->setupNum(noteNum);
  shakeEnergy += amplitude * MAX_SHAKE * 0.1;
  if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  if (instType==10 || instType==3) ratchetPos += 1;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Shakers: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Shakers :: noteOff(MY_FLOAT amplitude)
{
  shakeEnergy = 0.0;
  if (instType==10 || instType==3) ratchetPos = 0;
}

#define MIN_ENERGY 0.3

MY_FLOAT Shakers :: tick()
{
  MY_FLOAT data;
  MY_FLOAT temp_rand;
  int i;

  if (instType == 4) {
  	if (shakeEnergy > MIN_ENERGY)	{
      lastOutput = wuter_tick();
      lastOutput *= 0.0001;
    }
    else {
      lastOutput = 0.0;
    }
  }
  else if (instType == 22) {
    lastOutput = tbamb_tick();
  }
  else if (instType == 10 || instType == 3) {
    if (ratchetPos > 0) {
      ratchet -= (ratchetDelta + (0.002*totalEnergy));
      if (ratchet < 0.0) {
        ratchet = 1.0;
        ratchetPos -= 1;
	    }
      totalEnergy = ratchet;
      lastOutput = ratchet_tick();
      lastOutput *= 0.0001;
    }
    else lastOutput = 0.0;
  }
  else  {
    //  MY_FLOAT generic_tick()	{
    if (shakeEnergy > MIN_ENERGY) {
      shakeEnergy *= systemDecay;               // Exponential system decay
      if (float_random(1024.0) < nObjects) {
        sndLevel += shakeEnergy;   
        for (i=0;i<nFreqs;i++) {
          if (freqalloc[i])	{
            temp_rand = t_center_freqs[i] * (1.0 + (freq_rand[i] * noise_tick()));
            coeffs[i][0] = -resons[i] * 2.0 * cos(temp_rand * TWO_PI / Stk::sampleRate());
          }
        }
    	}
      inputs[0] = sndLevel * noise_tick();      // Actual Sound is Random
      for (i=1; i<nFreqs; i++)	{
        inputs[i] = inputs[0];
      }
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<nFreqs;i++)	{
        inputs[i] -= outputs[i][0]*coeffs[i][0];  // Do
        inputs[i] -= outputs[i][1]*coeffs[i][1];  // resonant
        outputs[i][1] = outputs[i][0];            // filter
        outputs[i][0] = inputs[i];                // calculations
        finalZ[0] += gains[i] * outputs[i][1];
      }
      data = finalZCoeffs[0] * finalZ[0];     // Extra zero(s) for shape
      data += finalZCoeffs[1] * finalZ[1];    // Extra zero(s) for shape
      data += finalZCoeffs[2] * finalZ[2];    // Extra zero(s) for shape
      if (data > 10000.0)	data = 10000.0;
      if (data < -10000.0) data = -10000.0;
      lastOutput = data * 0.0001;
    }
    else lastOutput = 0.0;
  }

  return lastOutput;
}

void Shakers :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Shakers: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Shakers: Control value greater than 128.0!" << std::endl;
  }

  MY_FLOAT temp;
  int i;

  if (number == __SK_Breath_) { // 2 ... energy
    shakeEnergy += norm * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3) {
	    ratchetPos = (int) fabs(value - lastRatchetPos);
	    ratchetDelta = 0.0002 * ratchetPos;
	    lastRatchetPos = (int) value;
    }
  }
  else if (number == __SK_ModFrequency_) { // 4 ... decay
    if (instType != 3 && instType != 10) {
      systemDecay = defDecays[instType] + ((value - 64.0) *
                                           decayScale[instType] *
                                           (1.0 - defDecays[instType]) / 64.0 );
      gains[0] = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
      for (i=1;i<nFreqs;i++) gains[i] = gains[0];
      if (instType == 6) { // tambourine
        gains[0] *= TAMB_SHELL_GAIN;
        gains[1] *= 0.8;
      }
      else if (instType == 7) { // sleighbell
        gains[3] *= 0.5;
        gains[4] *= 0.3;
      }
      else if (instType == 12) { // cokecan
        for (i=1;i<nFreqs;i++) gains[i] *= 1.8;
      }
      for (i=0;i<nFreqs;i++) gains[i] *= ((128-value)/100.0 + 0.36);
    }
  }
  else if (number == __SK_FootControl_) { // 11 ... number of objects
    if (instType == 5) // bamboo
      nObjects = (MY_FLOAT) (value * defObjs[instType] / 64.0) + 0.3;
    else
      nObjects = (MY_FLOAT) (value * defObjs[instType] / 64.0) + 1.1;
    gains[0] = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    for (i=1;i<nFreqs;i++) gains[i] = gains[0];
    if (instType == 6) { // tambourine
      gains[0] *= TAMB_SHELL_GAIN;
      gains[1] *= 0.8;
    }
    else if (instType == 7) { // sleighbell
      gains[3] *= 0.5;
      gains[4] *= 0.3;
    }
    else if (instType == 12) { // cokecan
      for (i=1;i<nFreqs;i++) gains[i] *= 1.8;
    }
    if (instType != 3 && instType != 10) {
    // reverse calculate decay setting
    double temp = (double) (64.0 * (systemDecay-defDecays[instType])/(decayScale[instType]*(1-defDecays[instType])) + 64.0);
    // scale gains by decay setting
    for (i=0;i<nFreqs;i++) gains[i] *= ((128-temp)/100.0 + 0.36);
    }
  }
  else if (number == __SK_ModWheel_) { // 1 ... resonance frequency
    for (i=0; i<nFreqs; i++)	{
      if (instType == 6 || instType == 2 || instType == 7) // limit range a bit for tambourine
        temp = center_freqs[i] * pow (1.008,value-64);
      else
        temp = center_freqs[i] * pow (1.015,value-64);
      t_center_freqs[i] = temp;

      coeffs[i][0] = -resons[i] * 2.0 * cos(temp * TWO_PI / Stk::sampleRate());
      coeffs[i][1] = resons[i]*resons[i];
    }
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    shakeEnergy += norm * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3)	{
	    ratchetPos = (int) fabs(value - lastRatchetPos);
	    ratchetDelta = 0.0002 * ratchetPos;
	    lastRatchetPos = (int) value;
    }
  }
  else  if (number == __SK_ShakerInst_) { // 1071
    instType = (int) (value + 0.5);	//  Just to be safe
    this->setupNum(instType);
  }                                       
  else
    std::cerr << "[chuck](via STK): Shakers: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Shakers: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}

// KLUDGE-O-MATIC-O-RAMA

MY_FLOAT Shakers :: wuter_tick() {
  MY_FLOAT data;
  int j;
  shakeEnergy *= systemDecay;               // Exponential system decay
  if (my_random(32767) < nObjects) {     
    sndLevel = shakeEnergy;   
    j = my_random(3);
	  if (j == 0)   {
      center_freqs[0] = WUTR_CENTER_FREQ1 * (0.75 + (0.25 * noise_tick()));
	    gains[0] = fabs(noise_tick());
	  }
	  else if (j == 1)      {
      center_freqs[1] = WUTR_CENTER_FREQ1 * (1.0 + (0.25 * noise_tick()));
	    gains[1] = fabs(noise_tick());
	  }
	  else  {
      center_freqs[2] = WUTR_CENTER_FREQ1 * (1.25 + (0.25 * noise_tick()));
	    gains[2] = fabs(noise_tick());
	  }
	}
	
  gains[0] *= resons[0];
  if (gains[0] >  0.001) {
    center_freqs[0]  *= WUTR_FREQ_SWEEP;
    coeffs[0][0] = -resons[0] * 2.0 * 
      cos(center_freqs[0] * TWO_PI / Stk::sampleRate());
  }
  gains[1] *= resons[1];
  if (gains[1] > 0.001) {
    center_freqs[1] *= WUTR_FREQ_SWEEP;
    coeffs[1][0] = -resons[1] * 2.0 * 
      cos(center_freqs[1] * TWO_PI / Stk::sampleRate());
  }
  gains[2] *= resons[2];
  if (gains[2] > 0.001) {
    center_freqs[2] *= WUTR_FREQ_SWEEP;
    coeffs[2][0] = -resons[2] * 2.0 * 
      cos(center_freqs[2] * TWO_PI / Stk::sampleRate());
  }
	
  sndLevel *= soundDecay;        // Each (all) event(s) 
                                 // decay(s) exponentially 
  inputs[0] = sndLevel;
  inputs[0] *= noise_tick();     // Actual Sound is Random
  inputs[1] = inputs[0] * gains[1];
  inputs[2] = inputs[0] * gains[2];
  inputs[0] *= gains[0];
  inputs[0] -= outputs[0][0]*coeffs[0][0];
  inputs[0] -= outputs[0][1]*coeffs[0][1];
  outputs[0][1] = outputs[0][0];
  outputs[0][0] = inputs[0];
  data = gains[0]*outputs[0][0];
  inputs[1] -= outputs[1][0]*coeffs[1][0];
  inputs[1] -= outputs[1][1]*coeffs[1][1];
  outputs[1][1] = outputs[1][0];
  outputs[1][0] = inputs[1];
  data += gains[1]*outputs[1][0];
  inputs[2] -= outputs[2][0]*coeffs[2][0];
  inputs[2] -= outputs[2][1]*coeffs[2][1];
  outputs[2][1] = outputs[2][0];
  outputs[2][0] = inputs[2];
  data += gains[2]*outputs[2][0];
 
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = data * 4;

  data = finalZ[2] - finalZ[0];
  return data;
}

MY_FLOAT Shakers :: ratchet_tick() {
  MY_FLOAT data;
  if (my_random(1024) < nObjects) {
    sndLevel += 512 * ratchet * totalEnergy;
  }
  inputs[0] = sndLevel;
  inputs[0] *= noise_tick() * ratchet;
  sndLevel *= soundDecay;
		 
  inputs[1] = inputs[0];
  inputs[0] -= outputs[0][0]*coeffs[0][0];
  inputs[0] -= outputs[0][1]*coeffs[0][1];
  outputs[0][1] = outputs[0][0];
  outputs[0][0] = inputs[0];
  inputs[1] -= outputs[1][0]*coeffs[1][0];
  inputs[1] -= outputs[1][1]*coeffs[1][1];
  outputs[1][1] = outputs[1][0];
  outputs[1][0] = inputs[1];
     
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = gains[0]*outputs[0][1] + gains[1]*outputs[1][1];
  data = finalZ[0] - finalZ[2];
  return data;
}

MY_FLOAT Shakers :: tbamb_tick() {
  MY_FLOAT data, temp;
  static int which = 0;
  int i;

  if (shakeEnergy > MIN_ENERGY)	{
      shakeEnergy *= systemDecay;    // Exponential system decay
      if (float_random(1024.0) < nObjects) {
	    sndLevel += shakeEnergy;
	    which = my_random(7);
	  }  
      temp = sndLevel * noise_tick();      // Actual Sound is Random
	  for (i=0;i<nFreqs;i++)	inputs[i] = 0;
	  inputs[which] = temp;
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<nFreqs;i++)	{
        inputs[i] -= outputs[i][0]*coeffs[i][0];  // Do
        inputs[i] -= outputs[i][1]*coeffs[i][1];  // resonant
        outputs[i][1] = outputs[i][0];            // filter
        outputs[i][0] = inputs[i];                // calculations
        finalZ[0] += gains[i] * outputs[i][1];
      }
      data = finalZCoeffs[0] * finalZ[0];     // Extra zero(s) for shape
      data += finalZCoeffs[1] * finalZ[1];    // Extra zero(s) for shape
      data += finalZCoeffs[2] * finalZ[2];    // Extra zero(s) for shape
      if (data > 10000.0)	data = 10000.0;
      if (data < -10000.0) data = -10000.0;
      data = data * 0.0001;
  }
  else data = 0.0;
  return data;
}
/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Simple :: Simple()
{
  adsr = new ADSR;
  baseFrequency = (MY_FLOAT) 440.0;

  // Concatenate the STK rawwave path to the rawwave file
  loop = new WaveLoop( "special:impuls10", TRUE );

  filter = new OnePole(0.5);
  noise = new Noise;
  biquad = new BiQuad();

  setFrequency(baseFrequency);
  loopGain = 0.5;
}  

Simple :: ~Simple()
{
  delete adsr;
  delete loop;
  delete filter;
  delete biquad;
}

void Simple :: keyOn()
{
  adsr->keyOn();
}

void Simple :: keyOff()
{
  adsr->keyOff();
}

void Simple :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  keyOn();
  setFrequency(frequency);
  filter->setGain(amplitude); 

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Simple: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}
void Simple :: noteOff(MY_FLOAT amplitude)
{
  keyOff();

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Simple: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void Simple :: setFrequency(MY_FLOAT frequency)
{
  biquad->setResonance( frequency, 0.98, true );
  loop->setFrequency(frequency);
}

MY_FLOAT Simple :: tick()
{
  lastOutput = loopGain * loop->tick();
  biquad->tick( noise->tick() );
  lastOutput += (1.0 - loopGain) * biquad->lastOut();
  lastOutput = filter->tick( lastOutput );
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Simple :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Clarinet: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Clarinet: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_Breath_) // 2
    filter->setPole( 0.99 * (1.0 - (norm * 2.0)) );
  else if (number == __SK_NoiseLevel_) // 4
    loopGain = norm;
  else if (number == __SK_ModFrequency_) { // 11
    norm /= 0.2 * Stk::sampleRate();
    adsr->setAttackRate( norm );
    adsr->setDecayRate( norm );
    adsr->setReleaseRate( norm );
  }
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "[chuck](via STK): Simple: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Simple: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

 
SingWave :: SingWave(const char *fileName, bool raw)
{
  // An exception could be thrown here.
  wave = new WaveLoop( fileName, raw );

	rate = 1.0;
	sweepRate = 0.001;
	modulator = new Modulate();
	modulator->setVibratoRate( 6.0 );
	modulator->setVibratoGain( 0.04 );
	modulator->setRandomGain( 0.005 );
	envelope = new Envelope;
	pitchEnvelope = new Envelope;
	setFrequency( 75.0 );
	pitchEnvelope->setRate( 1.0 );
	this->tick();
	this->tick();
	pitchEnvelope->setRate( sweepRate * rate );
}

SingWave :: ~SingWave()
{
  delete wave;
	delete modulator;
	delete envelope;
	delete pitchEnvelope;
}

void SingWave :: reset()
{
  wave->reset();
	lastOutput = 0.0;
}

void SingWave :: normalize()
{
  wave->normalize();
}

void SingWave :: normalize(MY_FLOAT newPeak)
{
  wave->normalize( newPeak );
}

void SingWave :: setFrequency(MY_FLOAT frequency)
{
  m_freq = frequency;
	MY_FLOAT temp = rate;
	rate = wave->getSize() * frequency / Stk::sampleRate();
	temp -= rate;
	if ( temp < 0) temp = -temp;
	pitchEnvelope->setTarget( rate );
	pitchEnvelope->setRate( sweepRate * temp );
}

void SingWave :: setVibratoRate(MY_FLOAT aRate)
{
	modulator->setVibratoRate( aRate );
}

void SingWave :: setVibratoGain(MY_FLOAT gain)
{
	modulator->setVibratoGain(gain);
}

void SingWave :: setRandomGain(MY_FLOAT gain)
{
	modulator->setRandomGain(gain);
}

void SingWave :: setSweepRate(MY_FLOAT aRate)
{
	sweepRate = aRate;
}

void SingWave :: setGainRate(MY_FLOAT aRate)
{
	envelope->setRate(aRate);
}

void SingWave :: setGainTarget(MY_FLOAT target)
{
	envelope->setTarget(target);
}

void SingWave :: noteOn()
{
	envelope->keyOn();
}

void SingWave :: noteOff()
{
	envelope->keyOff();
}

MY_FLOAT SingWave :: tick()
{
  // Set the wave rate.
  MY_FLOAT newRate = pitchEnvelope->tick();
  newRate += newRate * modulator->tick();
  wave->setRate( newRate );

  lastOutput = wave->tick();
	lastOutput *= envelope->tick();
    
	return lastOutput;             
}

MY_FLOAT SingWave :: lastOut()
{
	return lastOutput;
}
/***************************************************/
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Sitar :: Sitar(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  delay = length / 2.0;
  targetDelay = delay;

  loopFilter = new OneZero;
  loopFilter->setZero(0.01);

  envelope = new ADSR();
  envelope->setAllTimes(0.001, 0.04, 0.0, 0.5);

  noise = new Noise;
  this->clear();
}

Sitar :: ~Sitar()
{
  delete delayLine;
  delete loopFilter;
  delete noise;
  delete envelope;
}

void Sitar :: clear()
{
  delayLine->clear();
  loopFilter->clear();
}

void Sitar :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Sitar: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  targetDelay = (Stk::sampleRate() / freakency);
  delay = targetDelay * (1.0 + (0.05 * noise->tick()));
  delayLine->setDelay(delay);
  loopGain = 0.995 + (freakency * 0.0000005);
  if (loopGain > 0.9995) loopGain = 0.9995;
}

void Sitar :: pluck(MY_FLOAT amplitude)
{
  envelope->keyOn();
}

void Sitar :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  pluck(amplitude);
  amGain = 0.1 * amplitude;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Sitar: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Sitar :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    std::cerr << "[chuck](via STK): Plucked: noteOff amplitude greater than 1.0!" << std::endl;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    std::cerr << "[chuck](via STK): Plucked: noteOff amplitude less than or zero!" << std::endl;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Plucked: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Sitar :: tick()
{
  if ( fabs(targetDelay - delay) > 0.001 ) {
    if (targetDelay < delay)
      delay *= 0.99999;
    else
      delay *= 1.00001;
    delayLine->setDelay(delay);
  }

  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) + 
                                (amGain * envelope->tick() * noise->tick()));
  
  return lastOutput;
}
/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>
#include <math.h>

Sphere::Sphere(double initRadius)
{
  myRadius = initRadius;
  myMass = 1.0;
  myPosition = new Vector3D(0, 0, 0);
  myVelocity = new Vector3D(0, 0, 0);
};

Sphere::~Sphere()
{
  delete myPosition;
  delete myVelocity;
}

void Sphere::setPosition(double anX, double aY, double aZ)
{
  myPosition->setXYZ(anX, aY, aZ);
};

void Sphere::setVelocity(double anX, double aY, double aZ)
{
  myVelocity->setXYZ(anX, aY, aZ);
};

void Sphere::setRadius(double aRadius)
{
  myRadius = aRadius;
};

void Sphere::setMass(double aMass)
{
  myMass = aMass;
};

Vector3D* Sphere::getPosition()
{
  return myPosition;
};

Vector3D* Sphere::getRelativePosition(Vector3D* aPosition)
{
  workingVector.setXYZ(aPosition->getX() - myPosition->getX(),
                       aPosition->getY() - myPosition->getY(),  
                       aPosition->getZ() - myPosition->getZ());
  return &workingVector;
};

double Sphere::getVelocity(Vector3D* aVelocity)
{
  aVelocity->setXYZ(myVelocity->getX(), myVelocity->getY(), myVelocity->getZ());
  return myVelocity->getLength();
};

double Sphere::isInside(Vector3D *aPosition)
{
  // Return directed distance from aPosition to spherical boundary ( <
  // 0 if inside).
  double distance;
  Vector3D *tempVector;

  tempVector = this->getRelativePosition(aPosition);
  distance = tempVector->getLength();
  return distance - myRadius;
};

double Sphere::getRadius()
{
  return myRadius;
};

double Sphere::getMass()
{
  return myMass;
};

void Sphere::addVelocity(double anX, double aY, double aZ)
{
  myVelocity->setX(myVelocity->getX() + anX);
  myVelocity->setY(myVelocity->getY() + aY);
  myVelocity->setZ(myVelocity->getZ() + aZ);
}

void Sphere::tick(double timeIncrement)
{
  myPosition->setX(myPosition->getX() + (timeIncrement * myVelocity->getX()));
  myPosition->setY(myPosition->getY() + (timeIncrement * myVelocity->getY()));
  myPosition->setZ(myPosition->getZ() + (timeIncrement * myVelocity->getZ()));
};

/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <string.h>
#include <math.h>

StifKarp :: StifKarp(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayA(0.5 * length, length);
  combDelay = new DelayL( 0.2 * length, length);

  filter = new OneZero();
  noise = new Noise();
  biQuad[0] = new BiQuad();
  biQuad[1] = new BiQuad();
  biQuad[2] = new BiQuad();
  biQuad[3] = new BiQuad();

  pluckAmplitude = 0.3;
  pickupPosition = (MY_FLOAT) 0.4;
  lastFrequency = lowestFrequency * 2.0;
  lastLength = length * 0.5;
  stretching = 0.9999;
  baseLoopGain = 0.995;
  loopGain = 0.999;

  clear();
}

StifKarp :: ~StifKarp()
{
  delete delayLine;
  delete combDelay;
  delete filter;
  delete noise;
  delete biQuad[0];
  delete biQuad[1];
  delete biQuad[2];
  delete biQuad[3];
}

void StifKarp :: clear()
{
  delayLine->clear();
  combDelay->clear();
  filter->clear();
}

void StifKarp :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency; 
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: setFrequency parameter is less than or equal to zero!" << std::endl;
    lastFrequency = 220.0;
  }

  lastLength = Stk::sampleRate() / lastFrequency;
  MY_FLOAT delay = lastLength - 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay( delay );

  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if (loopGain >= 1.0) loopGain = (MY_FLOAT) 0.99999;

  setStretch(stretching);

  combDelay->setDelay((MY_FLOAT) 0.5 * pickupPosition * lastLength); 
}

void StifKarp :: setStretch(MY_FLOAT stretch)
{
  stretching = stretch;
  MY_FLOAT coefficient;
  MY_FLOAT freq = lastFrequency * 2.0;
  MY_FLOAT dFreq = ( (0.5 * Stk::sampleRate()) - freq ) * 0.25;
  MY_FLOAT temp = 0.5 + (stretch * 0.5);
  if (temp > 0.9999) temp = 0.9999;
  for (int i=0; i<4; i++)	{
    coefficient = temp * temp;
    biQuad[i]->setA2( coefficient );
    biQuad[i]->setB0( coefficient );
    biQuad[i]->setB2( 1.0 );

    coefficient = -2.0 * temp * cos(TWO_PI * freq / Stk::sampleRate());
    biQuad[i]->setA1( coefficient );
    biQuad[i]->setB1( coefficient );

    freq += dFreq;
  }
}

void StifKarp :: setPickupPosition(MY_FLOAT position) {
  pickupPosition = position;
  if ( position < 0.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: setPickupPosition parameter is less than zero!" << std::endl;
    pickupPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: setPickupPosition parameter is greater than 1.0!" << std::endl;
    pickupPosition = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay->setDelay(0.5 * pickupPosition * lastLength); 
}

void StifKarp :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFrequency * 0.000005);
  if ( loopGain > 0.99999 ) loopGain = (MY_FLOAT) 0.99999;
}

void StifKarp :: pluck(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: pluck amplitude greater than 1.0!" << std::endl;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: pluck amplitude less than zero!" << std::endl;
    gain = 0.0;
  }

  pluckAmplitude = amplitude;
  for (long i=0; i<length; i++)  {
    // Fill delay with noise additively with current contents.
    delayLine->tick((delayLine->lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude);
    //delayLine->tick( combDelay->tick((delayLine->lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude));
  }
}

void StifKarp :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): StifKarp: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void StifKarp :: noteOff(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: noteOff amplitude greater than 1.0!" << std::endl;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    std::cerr << "[chuck](via STK): StifKarp: noteOff amplitude less than zero!" << std::endl;
    gain = 0.0;
  }
  loopGain =  (1.0 - gain) * 0.5;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): StifPluck: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT StifKarp :: tick()
{
  MY_FLOAT temp = delayLine->lastOut() * loopGain;

  // Calculate allpass stretching.
  for (int i=0; i<4; i++)
    temp = biQuad[i]->tick(temp);

  // Moving average filter.
  temp = filter->tick(temp);

  lastOutput = delayLine->tick(temp);
  lastOutput = lastOutput - combDelay->tick(lastOutput);
  return lastOutput;
}

void StifKarp :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): StifKarp: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): StifKarp: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_PickPosition_) // 4
    setPickupPosition( norm );
  else if (number == __SK_StringDamping_) // 11
    setBaseLoopGain( 0.97 + (norm * 0.03) );
  else if (number == __SK_StringDetune_) // 1
    setStretch( 0.9 + (0.1 * (1.0 - norm)) );
  else
    std::cerr << "[chuck](via STK): StifKarp: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): StifKarp: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}

/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate can be queried and
    modified via Stk.  In addition, this class
    provides error handling and byte-swapping
    functions.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>
#include <string.h>

MY_FLOAT Stk :: srate = (MY_FLOAT) SRATE;
std::string Stk :: rawwavepath = RAWWAVE_PATH;
const Stk::STK_FORMAT Stk :: STK_SINT8 = 1;
const Stk::STK_FORMAT Stk :: STK_SINT16 = 2;
const Stk::STK_FORMAT Stk :: STK_SINT32 = 8;
const Stk::STK_FORMAT Stk :: MY_FLOAT32 = 16;
const Stk::STK_FORMAT Stk :: MY_FLOAT64 = 32;

Stk :: Stk(void)
{
}

Stk :: ~Stk(void)
{
}

MY_FLOAT Stk :: sampleRate(void)
{
  return srate;
}

void Stk :: setSampleRate(MY_FLOAT newRate)
{
  if (newRate > 0)
    srate = newRate;
}

std::string Stk :: rawwavePath(void)
{
  return rawwavepath;
}

void Stk :: setRawwavePath(std::string newPath)
{
  if ( !newPath.empty() )
    rawwavepath = newPath;

  // Make sure the path includes a "/"
  if ( rawwavepath[rawwavepath.length()-1] != '/' )
    rawwavepath += "/";
}

void Stk :: swap16(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 2nd bytes
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

void Stk :: swap32(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 4th bytes
  val = *(ptr);
  *(ptr) = *(ptr+3);
  *(ptr+3) = val;

  //Swap 2nd and 3rd bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

void Stk :: swap64(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 8th bytes
  val = *(ptr);
  *(ptr) = *(ptr+7);
  *(ptr+7) = val;

  // Swap 2nd and 7th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+5);
  *(ptr+5) = val;

  // Swap 3rd and 6th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+3);
  *(ptr+3) = val;

  // Swap 4th and 5th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__) || defined(__OS_WINDOWS_CYGWIN__))
  #include <unistd.h>
#elif defined(__OS_WINDOWS__)
  #include <windows.h>
#endif

void Stk :: sleep(unsigned long milliseconds)
{
#if defined(__OS_WINDOWS__)
  Sleep((DWORD) milliseconds);
#elif (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__) || defined(__OS_WINDOWS_CYGWIN__))
  usleep( (unsigned long) (milliseconds * 1000.0) );
#endif
}

void Stk :: handleError( const char *message, StkError::TYPE type )
{
  if (type == StkError::WARNING)
    fprintf(stderr, "%s\n", message);
  else if (type == StkError::DEBUG_WARNING) {
#if defined(_STK_DEBUG_)
    fprintf(stderr, "%s\n", message);
#endif
  }
  else {
    // Print error message before throwing.
    fprintf(stderr, "%s\n", message);
    throw StkError(message, type);
  }
}

StkError :: StkError(const char *p, TYPE tipe)
  : type(tipe)
{
  strncpy(message, p, 256);
}

StkError :: ~StkError(void)
{
}

void StkError :: printMessage(void)
{
  printf("%s\n", message);
}
/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


SubNoise :: SubNoise(int subRate) : Noise()
{    
  rate = subRate;
  counter = rate;
}
SubNoise :: ~SubNoise()
{
}

int SubNoise :: subRate(void) const
{
  return rate;
}

void SubNoise :: setRate(int subRate)
{
  if (subRate > 0)
    rate = subRate;
}

MY_FLOAT SubNoise :: tick()
{
  if ( ++counter > rate ) {
    Noise::tick();
    counter = 1;
  }

  return lastOutput;
}
/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

Table :: Table(char *fileName)
{
  char message[256];

  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    sprintf(message, "[chuck](via Table): Couldn't stat or find file (%s).", fileName);
    handleError( message, StkError::FILE_NOT_FOUND );
  }
  length = (long) filestat.st_size / 8;  // length in 8-byte samples

  // Open the file and read samples into data[]
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd) {
    sprintf(message, "[chuck](via Table): Couldn't open or find file (%s).", fileName);
    handleError( message, StkError::FILE_NOT_FOUND );
  }

  data = (MY_FLOAT *) new MY_FLOAT[length];

  // Read samples into data[]
  long i = 0;
  double temp;
  while ( fread(&temp, 8, 1, fd) ) {
if( little_endian )
    swap64((unsigned char *)&temp);

    data[i++] = (MY_FLOAT) temp;
  }
  fclose(fd);

  lastOutput = 0.0;
}

Table :: ~Table()
{
  delete [ ] data;
}

long Table :: getLength() const
{
  return length;
}

MY_FLOAT Table :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Table :: tick(MY_FLOAT index)
{
  MY_FLOAT alpha;
  long temp;

  if (index > length-1) {
    std::cerr << "[chuck](via STK): Table: Index (" << index << ") exceeds table length ... sticking at end!" << std::endl;
    index = length-1;
  }
  else if (index < 0.0) {
    std::cerr << "[chuck](via STK): Table: Index (" << index << ") is less than zero ... setting to zero!" << std::endl;
    index = 0.0;
  }

  // Index in range 0 to length-1
  temp = (long) index;                   // Integer part of index
  alpha = index - (MY_FLOAT) temp;      // Fractional part of index
  if (alpha > 0.0) {                    // Do linear interpolation
    lastOutput = data[temp];
    lastOutput += (alpha*(data[temp+1] - lastOutput));
  }
  else lastOutput = data[temp];

  return lastOutput;
}

MY_FLOAT *Table :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class TubeBell
    \brief STK tubular bell (orchestral chime) FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


TubeBell :: TubeBell()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0   * 0.995);
  this->setRatio(1, 1.414 * 0.995);
  this->setRatio(2, 1.0   * 1.005);
  this->setRatio(3, 1.414 * 1.000);

  gains[0] = __FM_gains[94];
  gains[1] = __FM_gains[76];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[71];

  adsr[0]->setAllTimes( 0.005, 4.0, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.005, 4.0, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 2.0, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.004, 4.0, 0.0, 0.04);

  twozero->setGain( 0.5 );
  vibrato->setFrequency( 2.0 );
}  

TubeBell :: ~TubeBell()
{
}

void TubeBell :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[94];
  gains[1] = amplitude * __FM_gains[76];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[71];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "TubeBell: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT TubeBell :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This protected Filter subclass implements
    a two-pole digital filter.  A method is
    provided for creating a resonance in the
    frequency response while maintaining a nearly
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

TwoPole :: TwoPole() : Filter()
{
  MY_FLOAT B = 1.0;
  MY_FLOAT A[3] = {1.0, 0.0, 0.0};
  m_resFreq = 440.0;
  m_resRad = 0.0;
  m_resNorm = false;
  Filter::setCoefficients( 1, &B, 3, A );
}

TwoPole :: ~TwoPole()
{
}

void TwoPole :: clear(void)
{
  Filter::clear();
}

void TwoPole :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void TwoPole :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void TwoPole :: setA2(MY_FLOAT a2)
{
  a[2] = a2;
}

void TwoPole :: setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize)
{
  a[2] = radius * radius;
  a[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Normalize the filter gain ... not terribly efficient.
    MY_FLOAT real = 1 - radius + (a[2] - radius) * cos(TWO_PI * 2 * frequency / Stk::sampleRate());
    MY_FLOAT imag = (a[2] - radius) * sin(TWO_PI * 2 * frequency / Stk::sampleRate());
    b[0] = sqrt( pow(real, 2) + pow(imag, 2) );
  }
}

void TwoPole :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT TwoPole :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT TwoPole :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT TwoPole :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] - a[2] * outputs[2] - a[1] * outputs[1];
  outputs[2] = outputs[1];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *TwoPole :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This protected Filter subclass implements
    a two-zero digital filter.  A method is
    provided for creating a "notch" in the
    frequency response while maintaining a
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

TwoZero :: TwoZero() : Filter()
{
  MY_FLOAT B[3] = {1.0, 0.0, 0.0};
  MY_FLOAT A = 1.0;
  m_notchFreq = 440.0;
  m_notchRad = 0.0;
  Filter::setCoefficients( 3, B, 1, &A );
}

TwoZero :: ~TwoZero()
{
}

void TwoZero :: clear(void)
{
  Filter::clear();
}

void TwoZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void TwoZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void TwoZero :: setB2(MY_FLOAT b2)
{
  b[2] = b2;
}

void TwoZero :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  b[2] = radius * radius;
  b[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());

  // Normalize the filter gain.
  if (b[1] > 0.0) // Maximum at z = 0.
    b[0] = 1.0 / (1.0+b[1]+b[2]);
  else            // Maximum at z = -1.
    b[0] = 1.0 / (1.0-b[1]+b[2]);
  b[1] *= b[0];
  b[2] *= b[0];
}

void TwoZero :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT TwoZero :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT TwoZero :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT TwoZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[2] * inputs[2] + b[1] * inputs[1] + b[0] * inputs[0];
  inputs[2] = inputs[1];
  inputs[1] = inputs[0];

  return outputs[0];
}

MY_FLOAT *TwoZero :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Vector3D :: Vector3D(double initX, double initY, double initZ)
{
  myX = initX;
  myY = initY;
  myZ = initZ;
}

Vector3D :: ~Vector3D()
{
}

double Vector3D :: getX()
{
  return myX;
}

double Vector3D :: getY()
{
  return myY;
}

double Vector3D :: getZ()
{
  return myZ;
}

double Vector3D :: getLength()
{
  double temp;
  temp = myX * myX;
  temp += myY * myY;
  temp += myZ * myZ;
  temp = sqrt(temp);
  return temp;
}

void Vector3D :: setXYZ(double anX, double aY, double aZ)
{
  myX = anX;
  myY = aY;
  myZ = aZ;
};

void Vector3D :: setX(double aval)
{
  myX = aval;
}

void Vector3D :: setY(double aval)
{
  myY = aval;
}

void Vector3D :: setZ(double aval)
{
  myZ = aval;
}


/***************************************************/
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


#include <math.h>

VoicForm :: VoicForm() : Instrmnt()
{
  // Concatenate the STK rawwave path to the rawwave file
	voiced = new SingWave( "special:impuls20", TRUE );
	voiced->setGainRate( 0.001 );
	voiced->setGainTarget( 0.0 );
	noise = new Noise;

  for ( int i=0; i<4; i++ ) {
    filters[i] = new FormSwep;
    filters[i]->setSweepRate( 0.001 );
  }
    
	onezero = new OneZero;
	onezero->setZero( -0.9 );
	onepole = new OnePole;
	onepole->setPole( 0.9 );
    
	noiseEnv = new Envelope;
	noiseEnv->setRate( 0.001 );
	noiseEnv->setTarget( 0.0 );
    
	m_phonemeNum = 0;
	this->setPhoneme( "eee" );
	this->clear();
}  

VoicForm :: ~VoicForm()
{
	delete voiced;
	delete noise;
	delete onezero;
	delete onepole;
	delete noiseEnv;
  for ( int i=0; i<4; i++ ) {
    delete filters[i];
  }
}

void VoicForm :: clear()
{
	onezero->clear();
	onepole->clear();
  for ( int i=0; i<4; i++ ) {
    filters[i]->clear();
  }
}

void VoicForm :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): VoicForm: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

	voiced->setFrequency( freakency );
}

bool VoicForm :: setPhoneme(const char *phoneme )
{
	bool found = false;
    unsigned int i = 0;
	while( i < 32 && !found ) {
		if( !strcmp( Phonemes::name(i), phoneme ) ) {
			found = true;
      filters[0]->setTargets( Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
      filters[1]->setTargets( Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
      filters[2]->setTargets( Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
      filters[3]->setTargets( Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
      setVoiced( Phonemes::voiceGain( i ) );
      setUnVoiced( Phonemes::noiseGain( i ) );
      m_phonemeNum = i;
#if defined(_STK_DEBUG_)
      std::cerr << "[chuck](via STK): VoicForm: found formant " << phoneme << " (number " << i << ")" << std::endl;
#endif
		}
		i++;
	}

	if( !found )
        std::cerr << "[chuck](via STK): VoicForm: phoneme " << phoneme << " not found!" << std::endl;

	return found;
}

void VoicForm :: setVoiced(MY_FLOAT vGain)
{
	voiced->setGainTarget(vGain);
}

void VoicForm :: setUnVoiced(MY_FLOAT nGain)
{
	noiseEnv->setTarget(nGain * 0.01);
}

void VoicForm :: setFilterSweepRate(int whichOne, MY_FLOAT rate)
{
  if ( whichOne < 0 || whichOne > 3 ) {
    std::cerr << "[chuck](via STK): VoicForm: setFilterSweepRate filter argument outside range 0-3!" << std::endl;
    return;
  }

	filters[whichOne]->setSweepRate(rate);
}

void VoicForm :: setPitchSweepRate(MY_FLOAT rate)
{
	voiced->setSweepRate(rate);
}

void VoicForm :: speak()
{
	voiced->noteOn();
}

void VoicForm :: quiet()
{
	voiced->noteOff();
	noiseEnv->setTarget( 0.0 );
}

void VoicForm :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
	setFrequency(frequency);
	voiced->setGainTarget(amplitude);
	onepole->setPole( 0.97 - (amplitude * 0.2) );
}

void VoicForm :: noteOn( MY_FLOAT amplitude )
{
	voiced->setGainTarget(amplitude);
	onepole->setPole( 0.97 - (amplitude * 0.2) );
}

void VoicForm :: noteOff(MY_FLOAT amplitude)
{
	this->quiet();
}

MY_FLOAT VoicForm :: tick()
{
	MY_FLOAT temp;
	temp = onepole->tick( onezero->tick( voiced->tick() ) );
	temp += noiseEnv->tick() * noise->tick();
	lastOutput = filters[0]->tick(temp);
	lastOutput += filters[1]->tick(temp);
	lastOutput += filters[2]->tick(temp);
	lastOutput += filters[3]->tick(temp);
  /*
	temp  += noiseEnv->tick() * noise->tick();
	lastOutput  = filters[0]->tick(temp);
	lastOutput  = filters[1]->tick(lastOutput);
	lastOutput  = filters[2]->tick(lastOutput);
	lastOutput  = filters[3]->tick(lastOutput);
  */
	return lastOutput;
}
 
void VoicForm :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): VoicForm: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): VoicForm: Control value greater than 128.0!" << std::endl;
  }

	if (number == __SK_Breath_)	{ // 2
		this->setVoiced( 1.0 - norm );
		this->setUnVoiced( norm );
	}
	else if (number == __SK_FootControl_)	{ // 4
    MY_FLOAT temp = 0.0;
		unsigned int i = (int) value;
		if (i < 32)	{
      temp = 0.9;
		}
		else if (i < 64)	{
      i -= 32;
      temp = 1.0;
		}
		else if (i < 96)	{
      i -= 64;
      temp = 1.1;
		}
		else if (i < 128)	{
      i -= 96;
      temp = 1.2;
		}
		else if (i == 128)	{
      i = 0;
      temp = 1.4;
		}
    filters[0]->setTargets( temp * Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
    filters[1]->setTargets( temp * Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
    filters[2]->setTargets( temp * Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
    filters[3]->setTargets( temp * Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
    setVoiced( Phonemes::voiceGain( i ) );
    setUnVoiced( Phonemes::noiseGain( i ) );
	}
	else if (number == __SK_ModFrequency_) // 11
		voiced->setVibratoRate( norm * 12.0);  // 0 to 12 Hz
	else if (number == __SK_ModWheel_) // 1
		voiced->setVibratoGain( norm * 0.2);
	else if (number == __SK_AfterTouch_Cont_)	{ // 128
		setVoiced( norm );
		onepole->setPole( 0.97 - ( norm * 0.2) );
	}
  else
    std::cerr << "[chuck](via STK): VoicForm: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): VoicForm: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of
    STK instrument classes.  Individual voices can
    be controlled via unique note tags.
    Instrument groups can be controlled by channel
    number.

    A previously constructed STK instrument class
    is linked with a voice manager using the
    addInstrument() function.  An optional channel
    number argument can be specified to the
    addInstrument() function as well (default
    channel = 0).  The voice manager does not
    delete any instrument instances ... it is the
    responsibility of the user to allocate and
    deallocate all instruments.

    The tick() function returns the mix of all
    sounding voices.  Each noteOn returns a unique
    tag (credits to the NeXT MusicKit), so you can
    send control changes to specific voices within
    an ensemble.  Alternately, control changes can
    be sent to all voices on a given channel.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>
#include <math.h>

Voicer :: Voicer( int maxInstruments, MY_FLOAT decayTime )
{
  nVoices = 0;
  maxVoices = maxInstruments;
  voices = (Voice *) new Voice[maxVoices];
  tags = 23456;
  muteTime = (int) ( decayTime * Stk::sampleRate() );
}

Voicer :: ~Voicer()
{
  delete [] voices;
}

void Voicer :: addInstrument( Instrmnt *instrument, int channel )
{
  //voices = (Voice *) realloc( (void *) voices, nVoices+1 * sizeof( Voice ) );
  if ( nVoices == maxVoices ) {
    std::cerr << "[chuck](via STK): Voicer: Maximum number of voices already added!!" << std::endl;
    return;
  }

  voices[nVoices].instrument = instrument;
  voices[nVoices].tag = 0;
  voices[nVoices].channel = channel;
  voices[nVoices].noteNumber = -1;
  voices[nVoices].frequency = 0.0;
  voices[nVoices].sounding = 0;
  nVoices++;
}

void Voicer :: removeInstrument( Instrmnt *instrument )
{
  bool found = false;
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].instrument == instrument ) found = true;
    if ( found && i+1 < nVoices ) {
      voices[i].instrument = voices[i+1].instrument;
      voices[i].tag = voices[i+1].tag;
      voices[i].noteNumber = voices[i+1].noteNumber;
      voices[i].frequency = voices[i+1].frequency;
      voices[i].sounding = voices[i+1].sounding;
      voices[i].channel = voices[i+1].channel;
    }
  }

  if ( found )
    nVoices--;
    //voices = (Voice *) realloc( voices, --nVoices * sizeof( Voice ) );

}

long Voicer :: noteOn(MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel )
{
  int i;
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( i=0; i<nVoices; i++ ) {
    if (voices[i].noteNumber < 0 && voices[i].channel == channel) {
	    voices[i].tag = tags++;
      voices[i].channel = channel;
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
	    voices[i].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
      voices[i].sounding = 1;
      return voices[i].tag;
    }
  }

  // All voices are sounding, so interrupt the oldest voice.
  int voice = -1;
  for ( i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel ) {
      if ( voice == -1 ) voice = i;
      else if ( voices[i].tag < voices[voice].tag ) voice = i;
    }
  }

  if ( voice >= 0 ) {
    voices[voice].tag = tags++;
    voices[voice].channel = channel;
    voices[voice].noteNumber = noteNumber;
    voices[voice].frequency = frequency;
    voices[voice].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
    voices[voice].sounding = 1;
    return voices[voice].tag;
  }

  return -1;
}

void Voicer :: noteOff( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].noteNumber == noteNumber && voices[i].channel == channel ) {
      voices[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices[i].sounding = -muteTime;
    }
  }
}

void Voicer :: noteOff( long tag, MY_FLOAT amplitude )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices[i].sounding = -muteTime;
      break;
    }
  }
}

void Voicer :: setFrequency( MY_FLOAT noteNumber, int channel )
{
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel ) {
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
      voices[i].instrument->setFrequency( frequency );
    }
  }
}

void Voicer :: setFrequency( long tag, MY_FLOAT noteNumber )
{
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
      voices[i].instrument->setFrequency( frequency );
      break;
    }
  }
}

void Voicer :: pitchBend( MY_FLOAT value, int channel )
{
  MY_FLOAT pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel )
      voices[i].instrument->setFrequency( (MY_FLOAT) (voices[i].frequency * pitchScaler) );
  }
}

void Voicer :: pitchBend( long tag, MY_FLOAT value )
{
  MY_FLOAT pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->setFrequency( (MY_FLOAT) (voices[i].frequency * pitchScaler) );
      break;
    }
  }
}

void Voicer :: controlChange( int number, MY_FLOAT value, int channel )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel )
      voices[i].instrument->controlChange( number, value );
  }
}

void Voicer :: controlChange( long tag, int number, MY_FLOAT value )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->controlChange( number, value );
      break;
    }
  }
}

void Voicer :: silence( void )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].sounding > 0 )
      voices[i].instrument->noteOff( 0.5 );
  }
}

MY_FLOAT Voicer :: tick()
{
  lastOutput = lastOutputLeft = lastOutputRight = 0.0;
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].sounding != 0 ) {
      lastOutput += voices[i].instrument->tick();
      lastOutputLeft += voices[i].instrument->lastOutLeft();
      lastOutputRight += voices[i].instrument->lastOutRight();
    }
    if ( voices[i].sounding < 0 ) {
      voices[i].sounding++;
      if ( voices[i].sounding == 0 )
        voices[i].noteNumber = -1;
    }
  }
  return lastOutput / nVoices;
}

MY_FLOAT *Voicer :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Voicer :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Voicer :: lastOutLeft() const
{
  return lastOutputLeft;
}

MY_FLOAT Voicer :: lastOutRight() const
{
  return lastOutputRight;
}

/***************************************************/
/*! \class WaveLoop
    \brief STK waveform oscillator class.

    This class inherits from WvIn and provides
    audio file looping functionality.

    WaveLoop supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

WaveLoop :: WaveLoop( const char *fileName, bool raw, bool generate )
  : WvIn( fileName, raw ), phaseOffset(0.0)
{
  m_freq = 0;
  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer+bufferSize == fileSize) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[j];
  }

}

WaveLoop :: WaveLoop( )
  : WvIn( ), phaseOffset(0.0)
{ m_freq = 0; } 

void
WaveLoop :: openFile( const char * fileName, bool raw, bool norm )
{
    m_loaded = FALSE;
    WvIn::openFile( fileName, raw, norm );
    // If at end of file, redo extra sample frame for looping.
    if (chunkPointer+bufferSize == fileSize) {
      for (unsigned int j=0; j<channels; j++)
        data[bufferSize*channels+j] = data[j];
    }
    m_loaded = TRUE;
}

WaveLoop :: ~WaveLoop()
{
    m_loaded = FALSE;
}

void WaveLoop :: readData( unsigned long index )
{
  WvIn::readData( index );

  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer+bufferSize == fileSize) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[j];
  }
}

void WaveLoop :: setFrequency(MY_FLOAT aFrequency)
{
  // This is a looping frequency.
  m_freq = aFrequency; // chuck data

  rate = fileSize * aFrequency / sampleRate();
}

void WaveLoop :: addTime(MY_FLOAT aTime)
{
  // Add an absolute time in samples 
  time += aTime;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WaveLoop :: addPhase(MY_FLOAT anAngle)
{
  // Add a time in cycles (one cycle = fileSize).
  time += fileSize * anAngle;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WaveLoop :: addPhaseOffset(MY_FLOAT anAngle)
{
  // Add a phase offset in cycles, where 1.0 = fileSize.
  phaseOffset = fileSize * anAngle;
}

const MY_FLOAT *WaveLoop :: tickFrame(void)
{
  register MY_FLOAT tyme, alpha;
  register unsigned long i, index;

  // Check limits of time address ... if necessary, recalculate modulo fileSize.
  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;

  if (phaseOffset) {
    tyme = time + phaseOffset;
    while (tyme < 0.0)
      tyme += fileSize;
    while (tyme >= fileSize)
      tyme -= fileSize;
  }
  else {
    tyme = time;
  }

  if (chunking) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer) || (tyme >= chunkPointer+bufferSize) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer;
  }

  // Always do linear interpolation here ... integer part of time address.
  index = (unsigned long) tyme;

  // Fractional part of time address.
  alpha = tyme - (MY_FLOAT) index;
  index *= channels;
  for (i=0; i<channels; i++) {
    lastOutput[i] = data[index];
    lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
    index++;
  }

  if (chunking) {
    // Scale outputs by gain.
    for (i=0; i<channels; i++)  lastOutput[i] *= gain;
  }

  // Increment time, which can be negative.
  time += rate;

  return lastOutput;
}

/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1996 - 2002.
*/
/***************************************************/

#include <stdlib.h>
#include <math.h>

#define CAN_RADIUS 100
#define PEA_RADIUS 30
#define BUMP_RADIUS 5

#define NORM_CAN_LOSS 0.97
#define SLOW_CAN_LOSS 0.90
#define GRAVITY 20.0
//  GRAVITY WAS 6.0

#define NORM_TICK_SIZE 0.004
#define SLOW_TICK_SIZE 0.0001

#define ENV_RATE 0.001 

Whistle :: Whistle()
{
	tempVector = new Vector3D(0,0,0);
  can = new Sphere(CAN_RADIUS);
  pea = new Sphere(PEA_RADIUS);
  bumper = new Sphere(BUMP_RADIUS);

  // Concatenate the STK rawwave path to the rawwave file
  sine = new WaveLoop( "special:sinewave", TRUE );
  sine->setFrequency(2800.0);

  can->setPosition(0, 0, 0); // set can location
  can->setVelocity(0, 0, 0); // and the velocity

  onepole.setPole(0.95);  // 0.99

  bumper->setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  bumper->setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  pea->setPosition(0, CAN_RADIUS/2, 0);
  pea->setVelocity(35, 15, 0);

  envelope.setRate(ENV_RATE);
  envelope.keyOn();

	fippleFreqMod = 0.5;
	fippleGainMod = 0.5;
	blowFreqMod = 0.25;
	noiseGain = 0.125;
	maxPressure = (MY_FLOAT) 0.0;
	baseFrequency = 2000;

	tickSize = NORM_TICK_SIZE;
	canLoss = NORM_CAN_LOSS;

	subSample = 1;
	subSampCount = subSample;
}

Whistle :: ~Whistle()
{
  delete tempVector;
  delete can;
  delete pea;
  delete bumper;
  delete sine;
}

void Whistle :: clear()
{
}

void Whistle :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency * 4;  // the whistle is a transposing instrument
  if ( frequency <= 0.0 ) {
    std::cerr << "[chuck](via STK): Whistle: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  baseFrequency = freakency;
}

void Whistle :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
	envelope.setRate(ENV_RATE);
	envelope.setTarget(amplitude);
}

void Whistle :: stopBlowing(MY_FLOAT rate)
{
  envelope.setRate(rate);
  envelope.keyOff();
}

void Whistle :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing(amplitude*2.0 ,amplitude * 0.2);
#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Whistle: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Whistle :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Whistle: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

int frameCount = 0;

MY_FLOAT Whistle :: tick()
{
  MY_FLOAT soundMix, tempFreq;
  double envOut = 0, temp, temp1, temp2, tempX, tempY;
  double phi, cosphi, sinphi;
  double gain = 0.5, mod = 0.0;

	if (--subSampCount <= 0)	{
		tempVectorP = pea->getPosition();
		subSampCount = subSample;
		temp = bumper->isInside(tempVectorP);
#ifdef WHISTLE_ANIMATION
    frameCount += 1;
    if (frameCount >= (1470 / subSample))	{
      frameCount = 0;
      // printf("%f %f %f\n",tempVectorP->getX(),tempVectorP->getY(),envOut);
      fflush(stdout);
    }
#endif
    envOut = envelope.tick();

    if (temp < (BUMP_RADIUS + PEA_RADIUS)) {
      tempX = envOut * tickSize * 2000 * noise.tick();
      tempY = -envOut * tickSize * 1000 * (1.0 + noise.tick());
      pea->addVelocity(tempX,tempY,0); 
      pea->tick(tickSize);
    }
        
    mod  = exp(-temp * 0.01);	// exp. distance falloff of fipple/pea effect
    temp = onepole.tick(mod);	// smooth it a little
    gain = (1.0 - (fippleGainMod*0.5)) + (2.0 * fippleGainMod * temp);
    gain *= gain;				// squared distance/gain
    //    tempFreq = 1.0				//  Normalized Base Freq
    //			+ (fippleFreqMod * 0.25) - (fippleFreqMod * temp) // fippleModulation 
    //			- (blowFreqMod) + (blowFreqMod * envOut); // blowingModulation
    // short form of above
    tempFreq = 1.0 + fippleFreqMod*(0.25-temp) + blowFreqMod*(envOut-1.0);
    tempFreq *= baseFrequency;

    sine->setFrequency(tempFreq);
    
    tempVectorP = pea->getPosition();
    temp = can->isInside(tempVectorP);
    temp  = -temp;       // We know (hope) it's inside, just how much??
    if (temp < (PEA_RADIUS * 1.25))        {            
      pea->getVelocity(tempVector);	//  This is the can/pea collision
      tempX = tempVectorP->getX();	// calculation.  Could probably
      tempY = tempVectorP->getY();	// simplify using tables, etc.
      phi = -atan2(tempY,tempX);
      cosphi = cos(phi);
      sinphi = sin(phi);
      temp1 = (cosphi*tempVector->getX()) - (sinphi*tempVector->getY());
      temp2 = (sinphi*tempVector->getX()) + (cosphi*tempVector->getY());
      temp1 = -temp1;
      tempX = (cosphi*temp1) + (sinphi*temp2);
      tempY = (-sinphi*temp1) + (cosphi*temp2);
      pea->setVelocity(tempX, tempY, 0);
      pea->tick(tickSize);
      pea->setVelocity(tempX*canLoss, tempY*canLoss, 0);
      pea->tick(tickSize);
    }
        
    temp = tempVectorP->getLength();	
    if (temp > 0.01)        {
      tempX = tempVectorP->getX();
      tempY = tempVectorP->getY();
      phi = atan2(tempY,tempX);
      phi += 0.3 * temp / CAN_RADIUS;
      cosphi = cos(phi);
      sinphi = sin(phi);
      tempX = 3.0 * temp * cosphi;
      tempY = 3.0 * temp * sinphi;
    }
    else {
      tempX = 0.0;
      tempY = 0.0;
    }
    
    temp = (0.9 + 0.1*subSample*noise.tick()) * envOut * 0.6 * tickSize;
    pea->addVelocity(temp * tempX,
                     (temp*tempY) - (GRAVITY*tickSize),0);
    pea->tick(tickSize);

    //    bumper->tick(0.0);
	}

	temp = envOut * envOut * gain / 2;
	soundMix = temp * (sine->tick() + (noiseGain*noise.tick()));
	lastOutput = 0.25 * soundMix; // should probably do one-zero filter here

	return lastOutput;
}

void Whistle :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "[chuck](via STK): Whistle: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "[chuck](via STK): Whistle: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_NoiseLevel_) // 4
    noiseGain = 0.25 * norm;
  else if (number == __SK_ModFrequency_) // 11
    fippleFreqMod = norm;
  else if (number == __SK_ModWheel_) // 1
    fippleGainMod = norm;
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope.setTarget( norm * 2.0 );
  else if (number == __SK_Breath_) // 2
    blowFreqMod = norm * 0.5;
  else if (number == __SK_Sustain_)	 // 64
	  if (value < 1.0) subSample = 1;
  else
    std::cerr << "[chuck](via STK): Whistle: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "[chuck](via STK): Whistle: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}

/***************************************************/
/*! \class Wurley
    \brief STK Wurlitzer electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Wurley :: Wurley()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
  waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0);
  this->setRatio(1, 4.0);
  this->setRatio(2, -510.0);
  this->setRatio(3, -510.0);

  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[82];
  gains[2] = __FM_gains[92];
  gains[3] = __FM_gains[68];

  adsr[0]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 0.25, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.001, 0.15, 0.0, 0.04);

  twozero->setGain( 2.0 );
  vibrato->setFrequency( 8.0 );
}  

Wurley :: ~Wurley()
{
}

void Wurley :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;
  waves[0]->setFrequency(baseFrequency * ratios[0]);
  waves[1]->setFrequency(baseFrequency * ratios[1]);
  waves[2]->setFrequency(ratios[2]);	// Note here a 'fixed resonance'.
  waves[3]->setFrequency(ratios[3]);
}

void Wurley :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99];
  gains[1] = amplitude * __FM_gains[82];
  gains[2] = amplitude * __FM_gains[82];
  gains[3] = amplitude * __FM_gains[68];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "Wurley: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Wurley :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class WvIn
    \brief STK audio data input base class.

    This class provides input support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvIn loads the contents of an audio file for
    subsequent output.  Linear interpolation is
    used for fractional "read rates".

    WvIn supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which return samples produced
    by averaging across sample frames, from the 
    tickFrame() methods, which return pointers to
    multi-channel sample frames.  For single-channel
    data, these methods return equivalent values.

    Small files are completely read into local memory
    during instantiation.  Large files are read
    incrementally from disk.  The file size threshold
    and the increment size values are defined in
    WvIn.h.

    WvIn currently supports WAV, AIFF, SND (AU),
    MAT-file (Matlab), and STK RAW file formats.
    Signed integer (8-, 16-, and 32-bit) and floating-
    point (32- and 64-bit) data types are supported.
    Uncompressed data types are not supported.  If
    using MAT-files, data should be saved in an array
    with each data channel filling a matrix row.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

#include <iostream>

#include "util_raw.h"

WvIn :: WvIn()
{
    init();
}

WvIn :: WvIn( const char *fileName, bool raw, bool doNormalize, bool generate )
{
    init();
    openFile( fileName, raw, generate );
}

WvIn :: ~WvIn()
{
    if (fd)
        fclose(fd);

    if (data)
        delete [] data;

    if (lastOutput)
        delete [] lastOutput;

    m_loaded = false;
}

void WvIn :: init( void )
{
    fd = 0;
    m_loaded = false;
    strcpy ( m_filename, "" );
    data = 0;
    lastOutput = 0;
    chunking = false;
    finished = true;
    interpolate = false;
    bufferSize = 0;
    channels = 0;
    time = 0.0;
}

void WvIn :: closeFile( void )
{
    if ( fd ) fclose( fd );
    finished = true;
}

void WvIn :: openFile( const char *fileName, bool raw, bool doNormalize, bool generate )
{
    unsigned long lastChannels = channels;
    unsigned long samples, lastSamples = (bufferSize+1)*channels;
    strncpy ( m_filename, fileName, 255 );
    m_filename[0] = '0';
    if(!generate || !strstr(fileName, "special:"))
    {
        closeFile();

        // Try to open the file.
        fd = fopen(fileName, "rb");
        if (!fd) {
            sprintf(msg, "[chuck](via WvIn): Could not open or find file (%s).", fileName);
            handleError(msg, StkError::FILE_NOT_FOUND);
        }

        bool result = false;
        if ( raw )
            result = getRawInfo( fileName );
        else {
            char header[12];
            if ( fread(&header, 4, 3, fd) != 3 ) goto error;
            if ( !strncmp(header, "RIFF", 4) &&
                !strncmp(&header[8], "WAVE", 4) )
                result = getWavInfo( fileName );
            else if ( !strncmp(header, ".snd", 4) )
                result = getSndInfo( fileName );
            else if ( !strncmp(header, "FORM", 4) &&
                    (!strncmp(&header[8], "AIFF", 4) || !strncmp(&header[8], "AIFC", 4) ) )
                result = getAifInfo( fileName );
            else {
                if ( fseek(fd, 126, SEEK_SET) == -1 ) goto error;
                if ( fread(&header, 2, 1, fd) != 1 ) goto error;
                if (!strncmp(header, "MI", 2) ||
                    !strncmp(header, "IM", 2) )
                    result = getMatInfo( fileName );
                else {
                    raw = TRUE;
                    result = getRawInfo( fileName );
                    // sprintf(msg, "WvIn: File (%s) format unknown.", fileName);
                    // handleError(msg, StkError::FILE_UNKNOWN_FORMAT);
                }
            }
        }

        if ( result == false )
            handleError(msg, StkError::FILE_ERROR);

        if ( fileSize == 0 ) {
            sprintf(msg, "[chuck](via WvIn): File (%s) data size is zero!", fileName);
            handleError(msg, StkError::FILE_ERROR);
        }
    }
    else
    {
        bufferSize = 256;
        channels = 1;
    }

    // Allocate new memory if necessary.
    samples = (bufferSize+1)*channels;
    if ( lastSamples < samples ) {
        if ( data ) delete [] data;
        data = (MY_FLOAT *) new MY_FLOAT[samples];
    }
    if ( lastChannels < channels ) {
        if ( lastOutput ) delete [] lastOutput;
        lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
    }

    if ( fmod(rate, 1.0) != 0.0 ) interpolate = true;
    chunkPointer = 0;

    reset();
    if(generate && strstr(fileName, "special:"))
    {
        // STK rawwave files have no header and are assumed to contain a
        // monophonic stream of 16-bit signed integers in big-endian byte
        // order with a sample rate of 22050 Hz.
        fileSize = bufferSize;
        dataOffset = 0;
        interpolate = true;
        chunking = false;
        dataType = STK_SINT16;
        byteswap = false;
        fileRate = 22050.0;
        rate = (MY_FLOAT)fileRate / Stk::sampleRate();
        

        // which
        if( strstr(fileName, "special:sinewave") )
        {
            for (unsigned int j=0; j<bufferSize; j++)
                data[j] = (SHRT_MAX) * sin(2*PI*j/256);
        }
        else
        {
            SAMPLE * rawdata = NULL;
            int rawsize = 0;

            if( strstr(fileName, "special:aah") ) {
                rawsize = ahh_size; rawdata = ahh_data;
            }
            else if( strstr(fileName, "special:britestk") ) {
                rawsize = britestk_size; rawdata = britestk_data;
            }
            else if( strstr(fileName, "special:dope") ) {
                rawsize = dope_size; rawdata = dope_data;
            }
            else if( strstr(fileName, "special:eee") ) {
                rawsize = eee_size; rawdata = eee_data;
            }
            else if( strstr(fileName, "special:fwavblnk") ) {
                rawsize = fwavblnk_size; rawdata = fwavblnk_data;
            }
            else if( strstr(fileName, "special:halfwave") ) {
                rawsize = halfwave_size; rawdata = halfwave_data;
            }
            else if( strstr(fileName, "special:impuls10") ) {
                rawsize = impuls10_size; rawdata = impuls10_data;
            }
            else if( strstr(fileName, "special:impuls20") ) {
                rawsize = impuls20_size; rawdata = impuls20_data;
            }
            else if( strstr(fileName, "special:impuls40") ) {
                rawsize = impuls40_size; rawdata = impuls40_data;
            }
            else if( strstr(fileName, "special:mand1") ) {
                rawsize = mand1_size; rawdata = mand1_data;
            }
            else if( strstr(fileName, "special:mandpluk") ) {
                rawsize = mandpluk_size; rawdata = mandpluk_data;
            }
            else if( strstr(fileName, "special:marmstk1") ) {
                rawsize = marmstk1_size; rawdata = marmstk1_data;
            }
            else if( strstr(fileName, "special:ooo") ) {
                rawsize = ooo_size; rawdata = ooo_data;
            }
            else if( strstr(fileName, "special:peksblnk") ) {
                rawsize = peksblnk_size; rawdata = peksblnk_data;
            }
            else if( strstr(fileName, "special:ppksblnk") ) {
                rawsize = ppksblnk_size; rawdata = ppksblnk_data;
            }
            else if( strstr(fileName, "special:silence") ) {
                rawsize = silence_size; rawdata = silence_data;
            }
            else if( strstr(fileName, "special:sineblnk") ) {
                rawsize = sineblnk_size; rawdata = sineblnk_data;
            }
            else if( strstr(fileName, "special:sinewave") ) {
                rawsize = sinewave_size; rawdata = sinewave_data;
            }
            else if( strstr(fileName, "special:snglpeak") ) {
                rawsize = snglpeak_size; rawdata = snglpeak_data;
            }
            else if( strstr(fileName, "special:twopeaks") ) {
                rawsize = twopeaks_size; rawdata = twopeaks_data;
            }
            else if( strstr(fileName, "special:glot_pop") ) {
                rawsize = glot_pop_size; rawdata = glot_pop_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }
            else if( strstr(fileName, "special:glot_ahh") ) {
                rawsize = glot_ahh_size; rawdata = glot_ahh_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }
            else if( strstr(fileName, "special:glot_eee" ) ) {
                rawsize = glot_eee_size; rawdata = glot_eee_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }
            else if( strstr(fileName, "special:glot_ooo" ) ) {
                rawsize = glot_ooo_size; rawdata = glot_ooo_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }

            if ( rawdata ) {
                if ( data ) delete [] data;
                data = (MY_FLOAT *) new MY_FLOAT[rawsize+1];
                bufferSize = rawsize;
                fileSize = bufferSize;
                for ( int j=0; j < rawsize; j++ ) {
                    data[j] = (MY_FLOAT) rawdata[j];
                }
            }
            else
                goto error;
        }
        data[bufferSize] = data[0];
    }
    else readData( 0 );  // Load file data.

    if ( doNormalize ) normalize();
    m_loaded = true;
    finished = false;
    interpolate = ( fmod( rate, 1.0 ) != 0.0 );
    return;

error:
    sprintf(msg, "[chuck](via WvIn): Error reading file (%s).", fileName);
    handleError(msg, StkError::FILE_ERROR);
}

bool WvIn :: getRawInfo( const char *fileName )
{
  // Use the system call "stat" to determine the file length.
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    sprintf(msg, "[chuck](via WvIn): Could not stat RAW file (%s).", fileName);
    return false;
  }

  fileSize = (long) filestat.st_size / 2;  // length in 2-byte samples
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
    gain = 1.0 / 32768.0;
  }

  // STK rawwave files have no header and are assumed to contain a
  // monophonic stream of 16-bit signed integers in big-endian byte
  // order with a sample rate of 22050 Hz.
  channels = 1;
  dataOffset = 0;
  rate = (MY_FLOAT) 22050.0 / Stk::sampleRate();
  fileRate = 22050.0;
  interpolate = false;
  dataType = STK_SINT16;
  byteswap = false;
if( little_endian )
  byteswap = true;

  return true;
}

bool WvIn :: getWavInfo( const char *fileName )
{
  // Find "format" chunk ... it must come before the "data" chunk.
  char id[4];
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  while ( strncmp(id, "fmt ", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Check that the data is not compressed.
  SINT16 format_tag;
  if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error; // Read fmt chunk size.
  if ( fread(&format_tag, 2, 1, fd) != 1 ) goto error;
if( !little_endian )
{
  swap16((unsigned char *)&format_tag);
  swap32((unsigned char *)&chunkSize);
}
  if (format_tag != 1 && format_tag != 3 ) { // PCM = 1, FLOAT = 3
    sprintf(msg, "[chuck](via WvIn): %s contains an unsupported data format type (%d).", fileName, format_tag);
    return false;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap16((unsigned char *)&temp);

  channels = (unsigned int ) temp;

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap32((unsigned char *)&srate);

  fileRate = (MY_FLOAT) srate;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( srate / Stk::sampleRate() );

  // Determine the data type.
  dataType = 0;
  if ( fseek(fd, 6, SEEK_CUR) == -1 ) goto error;   // Locate bits_per_sample info.
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap16((unsigned char *)&temp);

  if ( format_tag == 1 ) {
    if (temp == 8)
      dataType = STK_SINT8;
    else if (temp == 16)
      dataType = STK_SINT16;
    else if (temp == 32)
      dataType = STK_SINT32;
  }
  else if ( format_tag == 3 ) {
    if (temp == 32)
      dataType = MY_FLOAT32;
    else if (temp == 64)
      dataType = MY_FLOAT64;
  }
  if ( dataType == 0 ) {
    sprintf(msg, "[chuck](via WvIn): %d bits per sample with data format %d are not supported (%s).", temp, format_tag, fileName);
    return false;
  }

  // Jump over any remaining part of the "fmt" chunk.
  if ( fseek(fd, chunkSize-16, SEEK_CUR) == -1 ) goto error;

  // Find "data" chunk ... it must come after the "fmt" chunk.
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;

  while ( strncmp(id, "data", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Get length of data from the header.
  SINT32 bytes;
  if ( fread(&bytes, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap32((unsigned char *)&bytes);

  fileSize = 8 * bytes / temp / channels;  // sample frames
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  dataOffset = ftell(fd);
  byteswap = false;
if( !little_endian )
  byteswap = true;

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading WAV file (%s).", fileName);
  return false;
}

bool WvIn :: getSndInfo( const char *fileName )
{
  // Determine the data type.
  SINT32 format;
  if ( fseek(fd, 12, SEEK_SET) == -1 ) goto error;   // Locate format
  if ( fread(&format, 4, 1, fd) != 1 ) goto error;
if( little_endian )
    swap32((unsigned char *)&format);

  if (format == 2) dataType = STK_SINT8;
  else if (format == 3) dataType = STK_SINT16;
  else if (format == 5) dataType = STK_SINT32;
  else if (format == 6) dataType = MY_FLOAT32;
  else if (format == 7) dataType = MY_FLOAT64;
  else {
    sprintf(msg, "[chuck](via WvIn): data format in file %s is not supported.", fileName);
    return false;
  }

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&srate);

  fileRate = (MY_FLOAT) srate;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( srate / sampleRate() );

  // Get number of channels from the header.
  SINT32 chans;
  if ( fread(&chans, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&chans);

  channels = chans;

  if ( fseek(fd, 4, SEEK_SET) == -1 ) goto error;
  if ( fread(&dataOffset, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&dataOffset);

  // Get length of data from the header.
  if ( fread(&fileSize, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&fileSize);

  fileSize /= 2 * channels;  // Convert to sample frames.
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  byteswap = false;
if( little_endian )
  byteswap = true;

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading SND file (%s).", fileName);
  return false;
}

bool WvIn :: getAifInfo( const char *fileName )
{
  bool aifc = false;
  char id[4];

  // Determine whether this is AIFF or AIFC.
  if ( fseek(fd, 8, SEEK_SET) == -1 ) goto error;
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  if ( !strncmp(id, "AIFC", 4) ) aifc = true;

  // Find "common" chunk
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd) != 1) goto error;
  while ( strncmp(id, "COMM", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fseek(fd, 4, SEEK_CUR) == -1 ) goto error; // Jump over chunk size
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( little_endian )
  swap16((unsigned char *)&temp);

  channels = temp;

  // Get length of data from the header.
  SINT32 frames;
  if ( fread(&frames, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&frames);

  fileSize = frames; // sample frames
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  // Read the number of bits per sample.
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( little_endian )
  swap16((unsigned char *)&temp);

  // Get file sample rate from the header.  For AIFF files, this value
  // is stored in a 10-byte, IEEE Standard 754 floating point number,
  // so we need to convert it first.
  unsigned char srate[10];
  unsigned char exp;
  unsigned long mantissa;
  unsigned long last;
  if ( fread(&srate, 10, 1, fd) != 1 ) goto error;
  mantissa = (unsigned long) *(unsigned long *)(srate+2);
if( little_endian )
  swap32((unsigned char *)&mantissa);

  exp = 30 - *(srate+1);
  last = 0;
  while (exp--) {
    last = mantissa;
    mantissa >>= 1;
  }
  if (last & 0x00000001) mantissa++;
  fileRate = (MY_FLOAT) mantissa;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( fileRate / sampleRate() );

  // Determine the data format.
  dataType = 0;
  if ( aifc == false ) {
    if ( temp == 8 ) dataType = STK_SINT8;
    else if ( temp == 16 ) dataType = STK_SINT16;
    else if ( temp == 32 ) dataType = STK_SINT32;
  }
  else {
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
    if ( (!strncmp(id, "fl32", 4) || !strncmp(id, "FL32", 4)) && temp == 32 ) dataType = MY_FLOAT32;
    else if ( (!strncmp(id, "fl64", 4) || !strncmp(id, "FL64", 4)) && temp == 64 ) dataType = MY_FLOAT64;
  }
  if ( dataType == 0 ) {
    sprintf(msg, "[chuck](via WvIn): %d bits per sample in file %s are not supported.", temp, fileName);
    return false;
  }

  // Start at top to find data (SSND) chunk ... chunk order is undefined.
  if ( fseek(fd, 12, SEEK_SET) == -1 ) goto error;

  // Find data (SSND) chunk
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  while ( strncmp(id, "SSND", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Skip over chunk size, offset, and blocksize fields
  if ( fseek(fd, 12, SEEK_CUR) == -1 ) goto error;

  dataOffset = ftell(fd);
  byteswap = false;
if( little_endian )
  byteswap = true;

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading AIFF file (%s).", fileName);
  return false;
}

bool WvIn :: getMatInfo( const char *fileName )
{
  // Verify this is a version 5 MAT-file format.
  char head[4];
  if ( fseek(fd, 0, SEEK_SET) == -1 ) goto error;
  if ( fread(&head, 4, 1, fd) != 1 ) goto error;
  // If any of the first 4 characters of the header = 0, then this is
  // a Version 4 MAT-file.
  if ( strstr(head, "0") ) {
    sprintf(msg, "[chuck](via WvIn): %s appears to be a Version 4 MAT-file, which is not currently supported.",
            fileName);
    return false;
  }

  // Determine the endian-ness of the file.
  char mi[2];
  byteswap = false;
  // Locate "M" and "I" characters in header.
  if ( fseek(fd, 126, SEEK_SET) == -1 ) goto error;
  if ( fread(&mi, 2, 1, fd) != 1) goto error;
if( little_endian )
{
  if ( !strncmp(mi, "MI", 2) )
    byteswap = true;
  else if ( strncmp(mi, "IM", 2) ) goto error;
}
else
{
  if ( !strncmp(mi, "IM", 2))
    byteswap = true;
  else if ( strncmp(mi, "MI", 2) ) goto error;
}

  // Check the data element type
  SINT32 datatype;
  if ( fread(&datatype, 4, 1, fd) != 1 ) goto error;
  if ( byteswap ) swap32((unsigned char *)&datatype);
  if (datatype != 14) {
    sprintf(msg, "[chuck](via WvIn): The file does not contain a single Matlab array (or matrix) data element.");
    return false;
  }

  // Determine the array data type.
  SINT32 tmp;
  SINT32 size;
  if ( fseek(fd, 168, SEEK_SET) == -1 ) goto error;
  if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&tmp);
  if (tmp == 1) {  // array name > 4 characters
    if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;  // get array name length
    if (byteswap) swap32((unsigned char *)&tmp);
    size = (SINT32) ceil((float)tmp / 8);
    if ( fseek(fd, size*8, SEEK_CUR) == -1 ) goto error;  // jump over array name
  }
  else { // array name <= 4 characters, compressed data element
    if ( fseek(fd, 4, SEEK_CUR) == -1 ) goto error;
  }
  if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&tmp);
  if ( tmp == 1 ) dataType = STK_SINT8;
  else if ( tmp == 3 ) dataType = STK_SINT16;
  else if ( tmp == 5 ) dataType = STK_SINT32;
  else if ( tmp == 7 ) dataType = MY_FLOAT32;
  else if ( tmp == 9 ) dataType = MY_FLOAT64;
  else {
    sprintf(msg, "[chuck](via WvIn): The MAT-file array data format (%d) is not supported.", tmp);
    return false;
  }

  // Get number of rows from the header.
  SINT32 rows;
  if ( fseek(fd, 160, SEEK_SET) == -1 ) goto error;
  if ( fread(&rows, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&rows);

  // Get number of columns from the header.
  SINT32 columns;
  if ( fread(&columns,4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&columns);

  // Assume channels = smaller of rows or columns.
  if (rows < columns) {
    channels = rows;
    fileSize = columns;
  }
  else {
    sprintf(msg, "[chuck](via WvIn): Transpose the MAT-file array so that audio channels fill matrix rows (not columns).");
    return false;
  }
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  // Move read pointer to the data in the file.
  SINT32 headsize;
  if ( fseek(fd, 132, SEEK_SET) == -1 ) goto error;
  if ( fread(&headsize, 4, 1, fd) != 1 ) goto error; // file size from 132nd byte
  if (byteswap) swap32((unsigned char *)&headsize);
  headsize -= fileSize * 8 * channels;
  if ( fseek(fd, headsize, SEEK_CUR) == -1 ) goto error;
  dataOffset = ftell(fd);

  // Assume MAT-files have 44100 Hz sample rate.
  fileRate = 44100.0;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( fileRate / sampleRate() );

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading MAT-file (%s).", fileName);
  return false;
}





void WvIn :: readData( unsigned long index )
{
  while (index < (unsigned long)chunkPointer) {
    // Negative rate.
    chunkPointer -= CHUNK_SIZE;
    bufferSize = CHUNK_SIZE;
    if (chunkPointer < 0) {
      bufferSize += chunkPointer;
      chunkPointer = 0;
    }
  }
  while (index >= chunkPointer+bufferSize) {
    // Positive rate.
    chunkPointer += CHUNK_SIZE;
    bufferSize = CHUNK_SIZE;
    if ( (unsigned long)chunkPointer+CHUNK_SIZE >= fileSize) {
      bufferSize = fileSize - chunkPointer;
    }
  }

  long i, length = bufferSize;
  bool endfile = (chunkPointer+bufferSize == fileSize);
  if ( !endfile ) length += 1;

  // Read samples into data[].  Use MY_FLOAT data structure
  // to store samples.
  if ( dataType == STK_SINT16 ) {
    SINT16 *buf = (SINT16 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*2), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 2, fd) != 2 ) goto error;
    if ( byteswap ) {
      SINT16 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap16((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == STK_SINT32 ) {
    SINT32 *buf = (SINT32 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 4, fd) != 4 ) goto error;
    if ( byteswap ) {
      SINT32 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == MY_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 4, fd) != 4 ) goto error;
    if ( byteswap ) {
      FLOAT32 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == MY_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*8), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 8, fd) != 8 ) goto error;
    if ( byteswap ) {
      FLOAT64 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap64((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == STK_SINT8 ) {
    unsigned char *buf = (unsigned char *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 1, fd) != 1 ) goto error;
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i] - 128.0;  // 8-bit WAV data is unsigned!
  }

  // If at end of file, repeat last sample frame for interpolation.
  if ( endfile ) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[(bufferSize-1)*channels+j];
  }

  if (!chunking) {
    fclose(fd);
    fd = 0;
  }

  return;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading file data.");
  handleError(msg, StkError::FILE_ERROR);
}

void WvIn :: reset(void)
{
  time = (MY_FLOAT) 0.0;
  for (unsigned int i=0; i<channels; i++)
    lastOutput[i] = (MY_FLOAT) 0.0;
  finished = false;
}

void WvIn :: normalize(void)
{
  this->normalize((MY_FLOAT) 1.0);
}

// Normalize all channels equally by the greatest magnitude in all of the data.
void WvIn :: normalize(MY_FLOAT peak)
{
  if (chunking) {
    if ( dataType == STK_SINT8 ) gain = peak / 128.0;
    else if ( dataType == STK_SINT16 ) gain = peak / 32768.0;
    else if ( dataType == STK_SINT32 ) gain = peak / 2147483648.0;
    else if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) gain = peak;

    return;
  }

  unsigned long i;
  MY_FLOAT max = (MY_FLOAT) 0.0;

  for (i=0; i<channels*bufferSize; i++) {
    if (fabs(data[i]) > max)
      max = (MY_FLOAT) fabs((double) data[i]);
  }

  if (max > 0.0) {
    max = (MY_FLOAT) 1.0 / max;
    max *= peak;
    for (i=0;i<=channels*bufferSize;i++)
	    data[i] *= max;
  }
}

unsigned long WvIn :: getSize(void) const
{
  return fileSize;
}

unsigned int WvIn :: getChannels(void) const
{
  return channels;
}

MY_FLOAT WvIn :: getFileRate(void) const
{
  return fileRate;
}

bool WvIn :: isFinished(void) const
{
  return finished;
}

void WvIn :: setRate(MY_FLOAT aRate)
{
  rate = aRate;

  // If negative rate and at beginning of sound, move pointer to end
  // of sound.
  if ( (rate < 0) && (time == 0.0) ) time += rate + fileSize;

  if (fmod(rate, 1.0) != 0.0) interpolate = true;
  else interpolate = false;
}

void WvIn :: addTime(MY_FLOAT aTime)   
{
  // Add an absolute time in samples 
  time += aTime;

  if (time < 0.0) time = 0.0;
  if (time >= fileSize) {
    time = fileSize;
    finished = true;
  }
}

void WvIn :: setInterpolate(bool doInterpolate)
{
  interpolate = doInterpolate;
}

const MY_FLOAT *WvIn :: lastFrame(void) const
{
  return lastOutput;
}

MY_FLOAT WvIn :: lastOut(void) const
{
  if ( channels == 1 )
    return *lastOutput;

  MY_FLOAT output = 0.0;
  for (unsigned int i=0; i<channels; i++ ) {
    output += lastOutput[i];
  }
  return output / channels;
}

MY_FLOAT WvIn :: tick(void)
{
  tickFrame();
  return lastOut();
}

MY_FLOAT *WvIn :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vector[i] = tick();

  return vector;
}

const MY_FLOAT *WvIn :: tickFrame(void)
{
  register MY_FLOAT tyme, alpha;
  register unsigned long i, index;

  if (finished) return lastOutput;

  tyme = time;
  if (chunking) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer) || (tyme >= chunkPointer+bufferSize) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer;
  }

  // Integer part of time address.
  index = (long) tyme;

  if (interpolate) {
    // Linear interpolation ... fractional part of time address.
    alpha = tyme - (MY_FLOAT) index;
    index *= channels;
    for (i=0; i<channels; i++) {
      lastOutput[i] = data[index];
      lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
      index++;
    }
  }
  else {
    index *= channels;
    for (i=0; i<channels; i++)
      lastOutput[i] = data[index++];
  }

  if (chunking) {
    // Scale outputs by gain.
    for (i=0; i<channels; i++)  lastOutput[i] *= gain;
  }

  // Increment time, which can be negative.
  time += rate;
  if ( time < 0.0 || time >= fileSize ) finished = true;

  return lastOutput;
}

MY_FLOAT *WvIn :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    tickFrame();
    for ( j=0; j<channels; j++ )
      frameVector[i*channels+j] = lastOutput[j];
  }

  return frameVector;
}
/***************************************************/
/*! \class WvOut
    \brief STK audio data output base class.

    This class provides output support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvOut writes samples to an audio file.  It
    supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which output single samples
    to all channels in a sample frame, from the
    tickFrame() method, which takes a pointer
    to multi-channel sample frame data.

    WvOut currently supports WAV, AIFF, AIFC, SND
    (AU), MAT-file (Matlab), and STK RAW file
    formats.  Signed integer (8-, 16-, and 32-bit)
    and floating- point (32- and 64-bit) data types
    are supported.  STK RAW files use 16-bit
    integers by definition.  MAT-files will always
    be written as 64-bit floats.  If a data type
    specification does not match the specified file
    type, the data type will automatically be
    modified.  Uncompressed data types are not
    supported.

    Currently, WvOut is non-interpolating and the
    output rate is always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


const WvOut::FILE_TYPE WvOut :: WVOUT_RAW = 1;
const WvOut::FILE_TYPE WvOut :: WVOUT_WAV = 2;
const WvOut::FILE_TYPE WvOut :: WVOUT_SND = 3;
const WvOut::FILE_TYPE WvOut :: WVOUT_AIF = 4;
const WvOut::FILE_TYPE WvOut :: WVOUT_MAT = 5;

// WAV header structure. See ftp://ftp.isi.edu/in-notes/rfc2361.txt
// for information regarding format codes.
struct wavhdr {
  char riff[4];           // "RIFF"
  SINT32 file_size;      // in bytes
  char wave[4];           // "WAVE"
  char fmt[4];            // "fmt "
  SINT32 chunk_size;     // in bytes (16 for PCM)
  SINT16 format_tag;     // 1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
  SINT16 num_chans;      // 1=mono, 2=stereo
  SINT32 sample_rate;
  SINT32 bytes_per_sec;
  SINT16 bytes_per_samp; // 2=16-bit mono, 4=16-bit stereo
  SINT16 bits_per_samp;
  char data[4];           // "data"
  SINT32 data_length;    // in bytes
};

// SND (AU) header structure (NeXT and Sun).
struct sndhdr {
  char pref[4];
  SINT32 hdr_length;
  SINT32 data_length;
  SINT32 format;
  SINT32 sample_rate;
  SINT32 num_channels;
  char comment[16];
};

// AIFF/AIFC header structure ... only the part common to both
// formats.
struct aifhdr {
  char form[4];               // "FORM"
  SINT32 form_size;          // in bytes
  char aiff[4];               // "AIFF" or "AIFC"
  char comm[4];               // "COMM"
  SINT32 comm_size;          // "COMM" chunk size (18 for AIFF, 24 for AIFC)
  SINT16 num_chans;          // number of channels
  unsigned long sample_frames; // sample frames of audio data
  SINT16 sample_size;        // in bits
  unsigned char srate[10];      // IEEE 754 floating point format
};

struct aifssnd {
  char ssnd[4];               // "SSND"
  SINT32 ssnd_size;          // "SSND" chunk size
  unsigned long offset;         // data offset in data block (should be 0)
  unsigned long block_size;     // not used by STK (should be 0)
};

// MAT-file 5 header structure.
struct mathdr {
  char heading[124];   // Header text field
  SINT16 hff[2];      // Header flag fields
  SINT32 adf[11];     // Array data format fields
  // There's more, but it's of variable length
};

WvOut :: WvOut()
{
  init();
}

WvOut::WvOut( const char *fileName, unsigned int nChannels, FILE_TYPE type, Stk::STK_FORMAT format )
{
  init();
  openFile( fileName, nChannels, type, format );
}

WvOut :: ~WvOut()
{
  closeFile();

  if (data)
    delete [] data;
}

void WvOut :: init()
{
  fd = 0;
  data = 0;
  fileType = 0;
  dataType = 0;
  channels = 0;
  counter = 0;
  totalCount = 0;
  m_filename[0] = '\0';
  start = TRUE;
  flush = 0;
}

void WvOut :: closeFile( void )
{
  if ( fd ) {
    // If there's an existing file, close it first.
    writeData( counter );

    if ( fileType == WVOUT_RAW )
      fclose( fd );
    else if ( fileType == WVOUT_WAV )
      closeWavFile();
    else if ( fileType == WVOUT_SND )
      closeSndFile();
    else if ( fileType == WVOUT_AIF )
      closeAifFile();
    else if ( fileType == WVOUT_MAT )
      closeMatFile();
    fd = 0;

    // printf("%f Seconds Computed\n\n", getTime() );
    totalCount = 0;
  }

  m_filename[0] = '\0';

}

void WvOut :: openFile( const char *fileName, unsigned int nChannels, WvOut::FILE_TYPE type, Stk::STK_FORMAT format )
{
  closeFile();
  strncpy( m_filename, fileName, 255);
  if ( strlen( fileName ) > 255 ) 
    m_filename[255] = '\0';

  if ( nChannels < 1 ) {
    sprintf(msg, "[chuck](via WvOut): the channels argument must be greater than zero!");
    handleError( msg, StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels;
  channels = nChannels;
  fileType = type;

  if ( format != STK_SINT8 && format != STK_SINT16 &&
       format != STK_SINT32 && format != MY_FLOAT32 && 
       format != MY_FLOAT64 ) {
    sprintf( msg, "[chuck](via WvOut): Unknown data type specified (%ld).", format );
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  } 
  dataType = format;

  bool result = false;
  if ( fileType == WVOUT_RAW ) {
    if ( channels != 1 ) {
      sprintf(msg, "[chuck](via WvOut): STK RAW files are, by definition, always monaural (channels = %d not supported)!", nChannels);
      handleError( msg, StkError::FUNCTION_ARGUMENT );
    }
    result = setRawFile( fileName );
  }
  else if ( fileType == WVOUT_WAV )
    result = setWavFile( fileName );
  else if ( fileType == WVOUT_SND )
    result = setSndFile( fileName );
  else if ( fileType == WVOUT_AIF )
    result = setAifFile( fileName );
  else if ( fileType == WVOUT_MAT )
    result = setMatFile( fileName );
  else {
    sprintf(msg, "[chuck](via WvOut): Unknown file type specified (%ld).", fileType);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  if ( result == false )
    handleError(msg, StkError::FILE_ERROR);

  // Allocate new memory if necessary.
  if ( lastChannels < channels ) {
    if ( data ) delete [] data;
    data = (MY_FLOAT *) new MY_FLOAT[BUFFER_SIZE*channels];
  }
  counter = 0;
}

bool WvOut :: setRawFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".raw") == NULL) strcat(name, ".raw");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create RAW file: %s", name);
    return false;
  }

  if ( dataType != STK_SINT16 ) {
    dataType = STK_SINT16;
    sprintf(msg, "[chuck](via WvOut): Using 16-bit signed integer data format for file %s.", name);
    handleError(msg, StkError::WARNING);
  }

  byteswap = false;
if( little_endian )
  byteswap = true;

  // printf("\nCreating RAW file: %s\n", name);
  return true;
}

bool WvOut :: setWavFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".wav") == NULL) strcat(name, ".wav");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create WAV file: %s", name);
    return false;
  }

  struct wavhdr hdr = {"RIF", 44, "WAV", "fmt", 16, 1, 1,
                        (SINT32) Stk::sampleRate(), 0, 2, 16, "dat", 0};
  hdr.riff[3] = 'F';
  hdr.wave[3] = 'E';
  hdr.fmt[3]  = ' ';
  hdr.data[3] = 'a';
  hdr.num_chans = (SINT16) channels;
  if ( dataType == STK_SINT8 )
    hdr.bits_per_samp = 8;
  else if ( dataType == STK_SINT16 )
    hdr.bits_per_samp = 16;
  else if ( dataType == STK_SINT32 )
    hdr.bits_per_samp = 32;
  else if ( dataType == MY_FLOAT32 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 32;
  }
  else if ( dataType == MY_FLOAT64 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 64;
  }
  hdr.bytes_per_samp = (SINT16) (channels * hdr.bits_per_samp / 8);
  hdr.bytes_per_sec = (SINT32) (hdr.sample_rate * hdr.bytes_per_samp);

  byteswap = false;
if( !little_endian )
{
  byteswap = true;
  swap32((unsigned char *)&hdr.file_size);
  swap32((unsigned char *)&hdr.chunk_size);
  swap16((unsigned char *)&hdr.format_tag);
  swap16((unsigned char *)&hdr.num_chans);
  swap32((unsigned char *)&hdr.sample_rate);
  swap32((unsigned char *)&hdr.bytes_per_sec);
  swap16((unsigned char *)&hdr.bytes_per_samp);
  swap16((unsigned char *)&hdr.bits_per_samp);
}

  if ( fwrite(&hdr, 4, 11, fd) != 11 ) {
    sprintf(msg, "[chuck](via WvOut): Could not write WAV header for file %s", name);
    return false;
  }

  // printf("\nCreating WAV file: %s\n", name);
  return true;
}

void WvOut :: closeWavFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 || dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = totalCount * channels * bytes_per_sample;
if( !little_endian )
  swap32((unsigned char *)&bytes);

  fseek(fd, 40, SEEK_SET); // jump to data length
  fwrite(&bytes, 4, 1, fd);

  bytes = totalCount * channels * bytes_per_sample + 44;
if( !little_endian )
  swap32((unsigned char *)&bytes);

  fseek(fd, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd);
  fclose( fd );
}

bool WvOut :: setSndFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".snd") == NULL) strcat(name, ".snd");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create SND file: %s", name);
    return false;
  }

  struct sndhdr hdr = {".sn", 40, 0, 3, (SINT32) Stk::sampleRate(), 1, "Created by STK"};
  hdr.pref[3] = 'd';
  hdr.num_channels = channels;
  if ( dataType == STK_SINT8 )
    hdr.format = 2;
  else if ( dataType == STK_SINT16 )
    hdr.format = 3;
  else if ( dataType == STK_SINT32 )
    hdr.format = 5;
  else if ( dataType == MY_FLOAT32 )
    hdr.format = 6;
  else if ( dataType == MY_FLOAT64 )
    hdr.format = 7;

  byteswap = false;
if( little_endian )
{
  byteswap = true;
  swap32 ((unsigned char *)&hdr.hdr_length);
  swap32 ((unsigned char *)&hdr.format);
  swap32 ((unsigned char *)&hdr.sample_rate);
  swap32 ((unsigned char *)&hdr.num_channels);
}

  if ( fwrite(&hdr, 4, 10, fd) != 10 ) {
    sprintf(msg, "[chuck](via WvOut): Could not write SND header for file %s", name);
    return false;
  }

  // printf("\nCreating SND file: %s\n", name);
  return true;
}

void WvOut :: closeSndFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = totalCount * bytes_per_sample * channels;
if( little_endian )
  swap32 ((unsigned char *)&bytes);

  fseek(fd, 8, SEEK_SET); // jump to data size
  fwrite(&bytes, 4, 1, fd);
  fclose(fd);
}

bool WvOut :: setAifFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".aif") == NULL) strcat(name, ".aif");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create AIF file: %s", name);
    return false;
  }

  // Common parts of AIFF/AIFC header.
  struct aifhdr hdr = {"FOR", 46, "AIF", "COM", 18, 0, 0, 16, "0"};
  struct aifssnd ssnd = {"SSN", 8, 0, 0};
  hdr.form[3] = 'M';
  hdr.aiff[3] = 'F';
  hdr.comm[3] = 'M';
  ssnd.ssnd[3] = 'D';
  hdr.num_chans = channels;
  if ( dataType == STK_SINT8 )
    hdr.sample_size = 8;
  else if ( dataType == STK_SINT16 )
    hdr.sample_size = 16;
  else if ( dataType == STK_SINT32 )
    hdr.sample_size = 32;
  else if ( dataType == MY_FLOAT32 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 32;
    hdr.comm_size = 24;
  }
  else if ( dataType == MY_FLOAT64 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 64;
    hdr.comm_size = 24;
  }

  // For AIFF files, the sample rate is stored in a 10-byte,
  // IEEE Standard 754 floating point number, so we need to
  // convert to that.
  SINT16 i;
  unsigned long exp;
  unsigned long rate = (unsigned long) Stk::sampleRate();
  memset(hdr.srate, 0, 10);
  exp = rate;
  for (i=0; i<32; i++) {
    exp >>= 1;
    if (!exp) break;
  }
  i += 16383;
if( little_endian )
  swap16((unsigned char *)&i);

  *(SINT16 *)(hdr.srate) = (SINT16) i;

  for (i=32; i; i--) {
    if (rate & 0x80000000) break;
    rate <<= 1;
  }

if( little_endian )
  swap32((unsigned char *)&rate);

  *(unsigned long *)(hdr.srate+2) = (unsigned long) rate;

  byteswap = false;  
if( little_endian )
{
  byteswap = true;
  swap32((unsigned char *)&hdr.form_size);
  swap32((unsigned char *)&hdr.comm_size);
  swap16((unsigned char *)&hdr.num_chans);
  swap16((unsigned char *)&hdr.sample_size);
  swap32((unsigned char *)&ssnd.ssnd_size);
  swap32((unsigned char *)&ssnd.offset);
  swap32((unsigned char *)&ssnd.block_size);
}

  // The structure boundaries don't allow a single write of 54 bytes.
  if ( fwrite(&hdr, 4, 5, fd) != 5 ) goto error;
  if ( fwrite(&hdr.num_chans, 2, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.sample_frames, 4, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.sample_size, 2, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.srate, 10, 1, fd) != 1 ) goto error;

  if ( dataType == MY_FLOAT32 ) {
    char type[4] = {'f','l','3','2'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd) != 1 ) goto error;
  }
  else if ( dataType == MY_FLOAT64 ) {
    char type[4] = {'f','l','6','4'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd) != 1 ) goto error;
  }
  
  if ( fwrite(&ssnd, 4, 4, fd) != 4 ) goto error;

  // printf("\nCreating AIF file: %s\n", name);
  return true;

 error:
  sprintf(msg, "[chuck](via WvOut): Could not write AIF header for file %s", name);
  return false;
}

void WvOut :: closeAifFile( void )
{
  unsigned long frames = (unsigned long) totalCount;
if( little_endian )
  swap32((unsigned char *)&frames);

  fseek(fd, 22, SEEK_SET); // jump to "COMM" sample_frames
  fwrite(&frames, 4, 1, fd);

  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 || dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  unsigned long bytes = totalCount * bytes_per_sample * channels + 46;
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) bytes += 6;
if( little_endian )
  swap32((unsigned char *)&bytes);

  fseek(fd, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd);

  bytes = totalCount * bytes_per_sample * channels + 8;
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) bytes += 6;
if( little_endian )
  swap32((unsigned char *)&bytes);

  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 )
    fseek(fd, 48, SEEK_SET); // jump to "SSND" chunk size
  else
    fseek(fd, 42, SEEK_SET); // jump to "SSND" chunk size
  fwrite(&bytes, 4, 1, fd);

  fclose( fd );
}

bool WvOut :: setMatFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".mat") == NULL) strcat(name, ".mat");
  fd = fopen(name, "w+b");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create MAT file: %s", name);
    return false;
  }

  if ( dataType != MY_FLOAT64 ) {
    dataType = MY_FLOAT64;
    sprintf(msg, "[chuck](via WvOut): Using 64-bit floating-point data format for file %s", name);
    handleError(msg, StkError::WARNING);
  }

  struct mathdr hdr;
  strcpy(hdr.heading,"MATLAB 5.0 MAT-file, Generated using the Synthesis ToolKit in C++ (STK). By Perry R. Cook and Gary P. Scavone, 1995-2002.");

  int i;
  for (i=strlen(hdr.heading);i<124;i++) hdr.heading[i] = ' ';

  // Header Flag Fields
  hdr.hff[0] = (SINT16) 0x0100;   // Version field
  hdr.hff[1] = (SINT16) 'M';      // Endian indicator field ("MI")
  hdr.hff[1] <<= 8;
  hdr.hff[1] += 'I';

  hdr.adf[0] = (SINT32) 14;       // Matlab array data type value
  hdr.adf[1] = (SINT32) 0;        // Size of file after this point to end (in bytes)
                                 // Don't know size yet.

  // Numeric Array Subelements (4):
  // 1. Array Flags
  hdr.adf[2] = (SINT32) 6;        // Matlab 32-bit unsigned integer data type value
  hdr.adf[3] = (SINT32) 8;        // 8 bytes of data to follow
  hdr.adf[4] = (SINT32) 6;        // Double-precision array, no array flags set
  hdr.adf[5] = (SINT32) 0;        // 4 bytes undefined
  // 2. Array Dimensions
  hdr.adf[6] = (SINT32) 5;        // Matlab 32-bit signed integer data type value
  hdr.adf[7] = (SINT32) 8;        // 8 bytes of data to follow (2D array)
  hdr.adf[8] = (SINT32) channels; // This is the number of rows
  hdr.adf[9] = (SINT32) 0;        // This is the number of columns

  // 3. Array Name
  // We'll use fileName for the matlab array name (as well as the file name).
  // If fileName is 4 characters or less, we have to use a compressed data element
  // format for the array name data element.  Otherwise, the array name must
  // be formatted in 8-byte increments (up to 31 characters + NULL).
  SINT32 namelength = (SINT32) strlen(fileName);
  if (strstr(fileName, ".mat")) namelength -= 4;
  if (namelength > 31) namelength = 31; // Truncate name to 31 characters.
  char arrayName[64];
  strncpy(arrayName, fileName, namelength);
  arrayName[namelength] = '\0';
  if (namelength > 4) {
    hdr.adf[10] = (SINT32) 1;        // Matlab 8-bit signed integer data type value
  }
  else { // Compressed data element format
    hdr.adf[10] = namelength;
    hdr.adf[10] <<= 16;
    hdr.adf[10] += 1;
  }
  SINT32 headsize = 40;        // Number of bytes in data element so far.

  // Write the fixed portion of the header
  if ( fwrite(&hdr, 172, 1, fd) != 1 ) goto error;

  // Write MATLAB array name
  SINT32 tmp;
  if (namelength > 4) {
    if ( fwrite(&namelength, 4, 1, fd) != 1) goto error;
    if ( fwrite(arrayName, namelength, 1, fd) != 1 ) goto error;
    tmp = (SINT32) ceil((float)namelength / 8);
    if ( fseek(fd, tmp*8-namelength, SEEK_CUR) == -1 ) goto error;
    headsize += tmp * 8;
  }
  else { // Compressed data element format
    if ( fwrite(arrayName, namelength, 1, fd) != 1 ) goto error;
    tmp = 4 - namelength;
    if ( fseek(fd, tmp, SEEK_CUR) == -1 ) goto error;
  }

  // Finish writing known header information
  tmp = 9;        // Matlab IEEE 754 double data type
  if ( fwrite(&tmp, 4, 1, fd) != 1 ) goto error;
  tmp = 0;        // Size of real part subelement in bytes (8 per sample)
  if ( fwrite(&tmp, 4, 1, fd) != 1 ) goto error;
  headsize += 8;  // Total number of bytes in data element so far

  if ( fseek(fd, 132, SEEK_SET) == -1 ) goto error;
  if ( fwrite(&headsize, 4, 1, fd) != 1 ) goto error; // Write header size ... will update at end
  if ( fseek(fd, 0, SEEK_END) == -1 ) goto error;

  byteswap = false;
  fprintf( stderr, "[chuck]:(via STK): creating MAT-file (%s) containing MATLAB array: %s\n", name, arrayName);
  return true;

 error:
  sprintf(msg, "[chuck](via WvOut): Could not write MAT-file header for file %s", name);
  return false;
}

void WvOut :: closeMatFile( void )
{
  fseek(fd, 164, SEEK_SET); // jump to number of columns
  fwrite(&totalCount, 4, 1, fd);

  SINT32 headsize, temp;
  fseek(fd, 132, SEEK_SET);  // jump to header size
  fread(&headsize, 4, 1, fd);
  temp = headsize;
  headsize += (SINT32) (totalCount * 8 * channels);
  fseek(fd, 132, SEEK_SET);
  // Write file size (minus some header info)
  fwrite(&headsize, 4, 1, fd);

  fseek(fd, temp+132, SEEK_SET); // jumpt to data size (in bytes)
  temp = totalCount * 8 * channels;
  fwrite(&temp, 4, 1, fd);

  fclose(fd);
}

unsigned long WvOut :: getFrames( void ) const
{
  return totalCount;
}

MY_FLOAT WvOut :: getTime( void ) const
{
  return (MY_FLOAT) totalCount / Stk::sampleRate();
}

void WvOut :: writeData( unsigned long frames )
{
  if ( dataType == STK_SINT8 ) {
    if ( fileType == WVOUT_WAV ) { // 8-bit WAV data is unsigned!
      unsigned char sample;
      for ( unsigned long k=0; k<frames*channels; k++ ) {
        sample = (unsigned char) (data[k] * 127.0 + 128.0);
        if ( fwrite(&sample, 1, 1, fd) != 1 ) goto error;
      }
    }
    else {
      signed char sample;
      for ( unsigned long k=0; k<frames*channels; k++ ) {
        sample = (signed char) (data[k] * 127.0);
        //sample = ((signed char) (( data[k] + 1.0 ) * 127.5 + 0.5)) - 128;
        if ( fwrite(&sample, 1, 1, fd) != 1 ) goto error;
      }
    }
  }
  else if ( dataType == STK_SINT16 ) {
    SINT16 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (SINT16) (data[k] * 32767.0);
      //sample = ((SINT16) (( data[k] + 1.0 ) * 32767.5 + 0.5)) - 32768;
      if ( byteswap ) swap16( (unsigned char *)&sample );
      if ( fwrite(&sample, 2, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == STK_SINT32 ) {
    SINT32 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (SINT32) (data[k] * 2147483647.0);
      //sample = ((SINT32) (( data[k] + 1.0 ) * 2147483647.5 + 0.5)) - 2147483648;
      if ( byteswap ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == MY_FLOAT32 ) {
    FLOAT32 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (FLOAT32) (data[k]);
      if ( byteswap ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == MY_FLOAT64 ) {
    FLOAT64 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (FLOAT64) (data[k]);
      if ( byteswap ) swap64( (unsigned char *)&sample );
      if ( fwrite(&sample, 8, 1, fd) != 1 ) goto error;
    }
  }

  flush += frames;
  if( flush >= 8192 )
  {
      flush = 0;
      fflush( fd );
  }

  return;

 error:
  sprintf(msg, "[chuck](via WvOut): Error writing data to file.");
  handleError(msg, StkError::FILE_ERROR);
}

void WvOut :: tick(const MY_FLOAT sample)
{
  if ( !fd ) return;

  for ( unsigned int j=0; j<channels; j++ )
    data[counter*channels+j] = sample;

  counter++;
  totalCount++;

  if ( counter == BUFFER_SIZE ) {
    writeData( BUFFER_SIZE );
    counter = 0;
  }
}

void WvOut :: tick(const MY_FLOAT *vector, unsigned int vectorSize)
{
  if ( !fd ) return;

  for (unsigned int i=0; i<vectorSize; i++)
    tick( vector[i] );
}

void WvOut :: tickFrame(const MY_FLOAT *frameVector, unsigned int frames)
{
  if ( !fd ) return;

  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    for ( j=0; j<channels; j++ ) {
      data[counter*channels+j] = frameVector[i*channels+j];
    }
    counter++;
    totalCount++;

    if ( counter == BUFFER_SIZE ) {
      writeData( BUFFER_SIZE );
      counter = 0;
    }
  }
}



// chuck - import
// wrapper functions

//convenience functions 
/* void 
ck_domidi ( Instrmnt * inst, unsigned int i ) { 
    unsigned char status = (i>>16)&&0xff;
    unsigned char data1  = (i>>8)&&0xff;
    unsigned char data2  = (i)&&0xff;

}

void 
ck_domidi ( Instrmnt * inst, unsigned char status, unsigned char data1, unsigned char data2) { 
    unsigned char type = status && 0xf0; 
    switch ( type ) { 
    case __SK_NoteOn_: 
      inst->noteOn( mtof ( (float)data1 ), ((float)data2) / 128.0 );
      break;
    case __SK_NoteOff_:
      inst->noteOff( ((float)data2) / 128.0 );
      break;
    case __SK_ControlChange_:
      inst->controlChange( data1, data2 );
      break;
    case __SK_AfterTouch_:
      inst->controlChange( __SK_AfterTouch_, ((float)data1) / 128.0 );
    case __SK_PitchBend_:
      unsigned int mnum = ((unsigned int)data2) << 7 || data1 );
      inst->controlChange( __SK_PitchBend_, ((float)mnum) / 16384.0 );
      break;
    }
}

*/

UGEN_CTOR BandedWG_ctor ( t_CKTIME now ) { 
   return new BandedWG();
}

UGEN_DTOR BandedWG_dtor ( t_CKTIME now, void * data ) { 
    delete (BandedWG *)data;
}

UGEN_TICK BandedWG_tick ( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out ) { 
   *out = ((BandedWG *)data)->tick();
   return TRUE;
}

UGEN_PMSG BandedWG_pmsg( t_CKTIME now, void * data, const char * msg, void * value ) { 
   return FALSE;
}



UGEN_CTRL BandedWG_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->noteOn( GET_NEXT_FLOAT ( value ));
}

UGEN_CTRL BandedWG_ctrl_pluck( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->pluck( GET_NEXT_FLOAT ( value ));
}

UGEN_CTRL BandedWG_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->noteOff( GET_NEXT_FLOAT ( value ));
}

UGEN_CTRL BandedWG_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->setFrequency( GET_NEXT_FLOAT ( value ));
}


UGEN_CGET BandedWG_cget_freq( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    SET_NEXT_FLOAT ( value, f->freakency);
}

UGEN_CTRL BandedWG_ctrl_strikePosition( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->setStrikePosition( GET_NEXT_FLOAT ( value ) );
}
UGEN_CGET BandedWG_cget_strikePosition( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    SET_NEXT_FLOAT ( value, f->strikePosition );
}

UGEN_CTRL BandedWG_ctrl_bowRate( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->m_rate =  GET_NEXT_FLOAT ( value );
}
UGEN_CGET BandedWG_cget_bowRate( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    SET_NEXT_FLOAT ( value, f->m_rate );
}

UGEN_CTRL BandedWG_ctrl_bowPressure( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->controlChange( __SK_BowPressure_, GET_NEXT_FLOAT(value) * 128.0 );

}
UGEN_CGET BandedWG_cget_bowPressure( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    SET_NEXT_FLOAT ( value, f->m_bowpressure );
}


UGEN_CTRL BandedWG_ctrl_preset( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->setPreset ( GET_NEXT_INT ( value ) );
}

UGEN_CGET BandedWG_cget_preset( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    SET_NEXT_INT ( value, f->m_preset );
}

UGEN_CTRL BandedWG_ctrl_startBowing( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->startBowing( GET_NEXT_FLOAT (value), f->m_rate );
}
UGEN_CTRL BandedWG_ctrl_stopBowing( t_CKTIME now, void * data, void * value )
{
    BandedWG * f = (BandedWG *)data;
    f->stopBowing( GET_NEXT_FLOAT (value) );
}



// BiQuad
struct BiQuad_
{
    BiQuad biquad;
    t_CKFLOAT pfreq;
    t_CKFLOAT prad;
    t_CKFLOAT zfreq;
    t_CKFLOAT zrad;
    t_CKBOOL norm;
};

UGEN_CTOR BiQuad_ctor( t_CKTIME now )
{
    BiQuad_ * d = new BiQuad_;
    d->pfreq = 0.0;
    d->prad = 0.0;
    d->zfreq = 0.0;
    d->zrad = 0.0;
    d->norm = FALSE;

    return d;
}

UGEN_DTOR BiQuad_dtor( t_CKTIME now, void * data )
{
    delete (BiQuad_ *)data;
}

UGEN_TICK BiQuad_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    BiQuad_ * f = (BiQuad_ *)data;
    *out = (SAMPLE)f->biquad.tick( in );
    return TRUE;
}

UGEN_PMSG BiQuad_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL BiQuad_ctrl_b2( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->biquad.setB2( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL BiQuad_ctrl_b1( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->biquad.setB1( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL BiQuad_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->biquad.setB0( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL BiQuad_ctrl_a2( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->biquad.setA2( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL BiQuad_ctrl_a1( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->biquad.setA1( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL BiQuad_ctrl_pfreq( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->pfreq = GET_NEXT_FLOAT(value);
    f->biquad.setResonance( f->pfreq, f->prad, f->norm != 0 );
}

UGEN_CTRL BiQuad_ctrl_prad( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->prad = GET_NEXT_FLOAT(value);
    f->biquad.setResonance( f->pfreq, f->prad, f->norm != 0 );
}

UGEN_CTRL BiQuad_ctrl_zfreq( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->zfreq = GET_NEXT_FLOAT(value);
    f->biquad.setNotch( f->zfreq, f->zrad );
}

UGEN_CTRL BiQuad_ctrl_zrad( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->zrad = GET_NEXT_FLOAT(value);
    f->biquad.setNotch( f->zfreq, f->zrad );
}

UGEN_CTRL BiQuad_ctrl_norm( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->norm = GET_NEXT_UINT(value) != 0;
    f->biquad.setResonance( f->pfreq, f->prad, f->norm != 0 );
}

UGEN_CTRL BiQuad_ctrl_eqzs( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    f->biquad.setEqualGainZeroes();
}

UGEN_CGET BiQuad_cget_b2( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    SET_NEXT_FLOAT( value, f->biquad.b[2] );
}

UGEN_CGET BiQuad_cget_b1( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    SET_NEXT_FLOAT( value, f->biquad.b[1] );
}

UGEN_CGET BiQuad_cget_b0( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    SET_NEXT_FLOAT( value, f->biquad.b[0] );
}

UGEN_CGET BiQuad_cget_a2( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    SET_NEXT_FLOAT( value, f->biquad.a[2] );
}

UGEN_CGET BiQuad_cget_a1( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    SET_NEXT_FLOAT( value, f->biquad.a[1] );
}

UGEN_CGET BiQuad_cget_a0( t_CKTIME now, void * data, void * value )
{
    BiQuad_ * f = (BiQuad_ *)data;
    SET_NEXT_FLOAT( value, f->biquad.a[0] );
}

// BlowBotl
UGEN_CTOR BlowBotl_ctor( t_CKTIME now )
{
    return new BlowBotl();
}

UGEN_DTOR BlowBotl_dtor( t_CKTIME now, void * data )
{
    delete (BlowBotl *)data;
}

UGEN_TICK BlowBotl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    BlowBotl * p = (BlowBotl *)data;
    *out = p->tick();
    return TRUE;
}

UGEN_PMSG BlowBotl_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL BlowBotl_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->noteOn ( f );
}


UGEN_CTRL BlowBotl_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->noteOff ( f );
}

UGEN_CTRL BlowBotl_ctrl_startBlowing( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->startBlowing ( f );
}

UGEN_CTRL BlowBotl_ctrl_stopBlowing( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->stopBlowing ( f );
}

UGEN_CTRL BlowBotl_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setFrequency( f );
}

UGEN_CGET BlowBotl_cget_freq ( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    SET_NEXT_FLOAT( value, p->baseFrequency );
}


UGEN_CTRL BlowBotl_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->m_rate = f;
}

UGEN_CGET BlowBotl_cget_rate ( t_CKTIME now, void * data, void * value )
{
    BlowBotl * p = (BlowBotl *)data;
    SET_NEXT_FLOAT( value, p->m_rate );
}

// BlowHole
UGEN_CTOR BlowHole_ctor( t_CKTIME now )
{
    return new BlowHole( 44100 );
}

UGEN_DTOR BlowHole_dtor( t_CKTIME now, void * data )
{
    delete (BlowHole *)data;
}

UGEN_TICK BlowHole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    BlowHole * p = (BlowHole *)data;
    *out = p->tick();
    return TRUE;
}

UGEN_PMSG BlowHole_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL BlowHole_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->noteOn ( f );
}


UGEN_CTRL BlowHole_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->noteOff ( f );
}

UGEN_CTRL BlowHole_ctrl_startBlowing( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->startBlowing ( f );
}

UGEN_CTRL BlowHole_ctrl_stopBlowing( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->stopBlowing ( f );
}

UGEN_CTRL BlowHole_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setFrequency( f );
}

UGEN_CGET BlowHole_cget_freq ( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    SET_NEXT_FLOAT( value, p->m_frequency );
}


UGEN_CTRL BlowHole_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->m_rate = f;
}

UGEN_CGET BlowHole_cget_rate ( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    SET_NEXT_FLOAT( value, p->m_rate );
}

UGEN_CTRL BlowHole_ctrl_tonehole( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setTonehole( f );
}

UGEN_CGET BlowHole_cget_tonehole ( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    SET_NEXT_FLOAT( value, p->m_tonehole );
}

UGEN_CTRL BlowHole_ctrl_vent( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setVent( f );
}

UGEN_CGET BlowHole_cget_vent ( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    SET_NEXT_FLOAT( value, p->m_vent );
}

UGEN_CTRL BlowHole_ctrl_reed( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->controlChange(__SK_ReedStiffness_, f * 128.0);
}

UGEN_CGET BlowHole_cget_reed ( t_CKTIME now, void * data, void * value )
{
    BlowHole * p = (BlowHole *)data;
    SET_NEXT_FLOAT( value, p->m_reed );
}


// Bowed
UGEN_CTOR Bowed_ctor( t_CKTIME now )
{
    return new Bowed(40.0);
}

UGEN_DTOR Bowed_dtor( t_CKTIME now, void * data )
{
    delete (Bowed *)data;
}

UGEN_TICK Bowed_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Bowed * p = (Bowed *)data;
    *out = p->tick();
    return TRUE;
}

UGEN_PMSG Bowed_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Bowed_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->noteOn ( f );
}

UGEN_CTRL Bowed_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->noteOff ( f );
}

UGEN_CTRL Bowed_ctrl_startBowing( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->startBowing ( f );
}

UGEN_CTRL Bowed_ctrl_stopBowing( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->stopBowing ( f );
}

UGEN_CTRL Bowed_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setFrequency( f );
}

UGEN_CGET Bowed_cget_freq ( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    SET_NEXT_FLOAT( value, p->m_frequency );
}

UGEN_CTRL Bowed_ctrl_vibrato( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setVibrato( f );
}

UGEN_CGET Bowed_cget_vibrato( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    SET_NEXT_FLOAT( value, p->vibratoGain );
}

UGEN_CTRL Bowed_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->m_rate = f;
}

UGEN_CGET Bowed_cget_rate ( t_CKTIME now, void * data, void * value )
{
    Bowed * p = (Bowed *)data;
    SET_NEXT_FLOAT( value, p->m_rate );
}




// Chorus
UGEN_CTOR Chorus_ctor( t_CKTIME now )
{
    return new Chorus( 44100 );
}

UGEN_DTOR Chorus_dtor( t_CKTIME now, void * data )
{
    delete (Chorus *)data;
}

UGEN_TICK Chorus_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Chorus * p = (Chorus *)data;
    *out = p->tick(in);
    return TRUE;
}

UGEN_PMSG Chorus_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}


UGEN_CTRL Chorus_ctrl_mix( t_CKTIME now, void * data, void * value )
{
    Chorus * p = (Chorus *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setEffectMix( f );
}

UGEN_CTRL Chorus_ctrl_modDepth( t_CKTIME now, void * data, void * value )
{
    Chorus * p = (Chorus *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);

    p->setModDepth( f );
}

UGEN_CTRL Chorus_ctrl_modFreq( t_CKTIME now, void * data, void * value )
{
    Chorus * p = (Chorus *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setModFrequency( f );
}

UGEN_CGET Chorus_cget_mix( t_CKTIME now, void * data, void * value )
{
    Chorus * p = (Chorus *)data;
    SET_NEXT_FLOAT( value, p->effectMix );
}

UGEN_CGET Chorus_cget_modDepth( t_CKTIME now, void * data, void * value )
{
    Chorus * p = (Chorus *)data;
    SET_NEXT_FLOAT ( value, p->modDepth );
}

UGEN_CGET Chorus_cget_modFreq( t_CKTIME now, void * data, void * value )
{
    Chorus * p = (Chorus *)data;
    SET_NEXT_FLOAT ( value, p->mods[0]->m_freq );
}

//Brass
struct Brass_ { 
   Brass * imp;

   double m_frequency;
   double m_rate;
   double m_lip;

   Brass_ ( double d ) { 
      imp = new Brass(d);
      m_frequency = 100.0;
      m_rate = 0.5;
      m_lip = 0.1;        
   }
};

UGEN_CTOR Brass_ctor( t_CKTIME now )
{
    return new Brass_( 30.0 );
}

UGEN_DTOR Brass_dtor( t_CKTIME now, void * data )
{
    delete ((Brass_ *)data)->imp;
    delete (Brass_ *)data;
}

UGEN_TICK Brass_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Brass_ * b = (Brass_ *)data;
    *out = b->imp->tick();
    return TRUE;
}

UGEN_PMSG Brass_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Brass_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOn ( b->m_frequency, f );
}


UGEN_CTRL Brass_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOff ( f );
}

UGEN_CTRL Brass_ctrl_startBlowing( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->startBlowing ( f, b->m_rate );
}

UGEN_CTRL Brass_ctrl_stopBlowing( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->stopBlowing ( f );
}

UGEN_CTRL Brass_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    b->imp->clear();
}

UGEN_CTRL Brass_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_frequency = f;
    b->imp->setFrequency( f );
}

UGEN_CGET Brass_cget_freq ( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    SET_NEXT_FLOAT( value, b->m_frequency );
}

UGEN_CTRL Brass_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_rate = f;
}

UGEN_CGET Brass_cget_rate ( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    SET_NEXT_FLOAT( value, b->m_rate );
}

UGEN_CTRL Brass_ctrl_lip( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_lip = f;
    b->imp->setLip(f);
}

UGEN_CGET Brass_cget_lip ( t_CKTIME now, void * data, void * value )
{
    Brass_ * b = (Brass_ *)data;
    SET_NEXT_FLOAT( value, b->m_lip );
}


struct Clarinet_ { 
   Clarinet * imp;
   double m_frequency;
   double m_rate;
   double m_reed;
   Clarinet_ ( double d ) { 
      imp = new Clarinet(d);
      m_frequency = 100.0;
      m_rate = 0.5;
      m_reed = 0.5;
   }
};


// Clarinet
UGEN_CTOR Clarinet_ctor( t_CKTIME now )
{
    return new Clarinet_( 40.0 );
}

UGEN_DTOR Clarinet_dtor( t_CKTIME now, void * data )
{
    delete ((Clarinet_ *)data)->imp;
    delete (Clarinet_ *)data;
}

UGEN_TICK Clarinet_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Clarinet_ * b = (Clarinet_ *)data;
    *out = b->imp->tick();
    return TRUE;
}

UGEN_PMSG Clarinet_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Clarinet_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOn ( b->m_frequency, f );
}


UGEN_CTRL Clarinet_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOff ( f );
}

UGEN_CTRL Clarinet_ctrl_startBlowing( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->startBlowing ( f, b->m_rate );
}

UGEN_CTRL Clarinet_ctrl_stopBlowing( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->stopBlowing ( f );
}

UGEN_CTRL Clarinet_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    b->imp->clear();
}

UGEN_CTRL Clarinet_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_frequency = f;
    b->imp->setFrequency( f );
}

UGEN_CGET Clarinet_cget_freq ( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    SET_NEXT_FLOAT( value, b->m_frequency );
}

UGEN_CTRL Clarinet_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_rate = f;
}

UGEN_CGET Clarinet_cget_rate ( t_CKTIME now, void * data, void * value )
{
    Clarinet_ * b = (Clarinet_ *)data;
    SET_NEXT_FLOAT( value, b->m_rate );
}

// Flute
struct Flute_ { 
   Flute * imp;

   double m_frequency;
   double m_rate;
   double m_jetDelay;
   double m_jetReflection;
   double m_endReflection;

   Flute_ ( double d ) { 
      imp = new Flute(d);
      m_frequency = 100.0;
      m_rate = 0.5;
      m_jetDelay = 0.0;
      m_jetReflection = 0.0;
      m_endReflection = 0.0;
   }
};

UGEN_CTOR Flute_ctor( t_CKTIME now )
{
    return new Flute_( 40.0 );
}

UGEN_DTOR Flute_dtor( t_CKTIME now, void * data )
{
    delete ((Flute_ *)data)->imp;
    delete (Flute_ *)data;
}

UGEN_TICK Flute_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Flute_ * b = (Flute_ *)data;
    *out = b->imp->tick();
    return TRUE;
}

UGEN_PMSG Flute_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Flute_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOn ( b->m_frequency, f );
}


UGEN_CTRL Flute_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOff ( f );
}

UGEN_CTRL Flute_ctrl_startBlowing( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->startBlowing ( f, b->m_rate );
}

UGEN_CTRL Flute_ctrl_stopBlowing( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->stopBlowing ( f );
}

UGEN_CTRL Flute_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    b->imp->clear();
}

UGEN_CTRL Flute_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_frequency = f;
    b->imp->setFrequency( f );
}

UGEN_CGET Flute_cget_freq ( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    SET_NEXT_FLOAT( value, b->m_frequency );
}

UGEN_CTRL Flute_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_rate = f;
}

UGEN_CGET Flute_cget_rate ( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    SET_NEXT_FLOAT( value, b->m_rate );
}

UGEN_CTRL Flute_ctrl_jetDelay( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_jetDelay = f;
    b->imp->setJetDelay( f);
}

UGEN_CGET Flute_cget_jetDelay ( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    SET_NEXT_FLOAT( value, b->m_jetDelay );
}

UGEN_CTRL Flute_ctrl_jetReflection( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_jetReflection = f;
    b->imp->setJetReflection( f);
}

UGEN_CGET Flute_cget_jetReflection ( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    SET_NEXT_FLOAT( value, b->m_jetReflection );
}

UGEN_CTRL Flute_ctrl_endReflection( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_endReflection = f;
    b->imp->setEndReflection(f);
}

UGEN_CGET Flute_cget_endReflection ( t_CKTIME now, void * data, void * value )
{
    Flute_ * b = (Flute_ *)data;
    SET_NEXT_FLOAT( value, b->m_endReflection );
}


// ModalBar
struct ModalBar_ { 
   ModalBar modalbar;

   int    m_preset;
   int    m_modeIndex;
   double m_modeRatio;
   double m_modeRadius;
   
   ModalBar_ ( ) { 
      m_preset = 0;
      m_modeIndex = 0;
   }
};

UGEN_CTOR ModalBar_ctor( t_CKTIME now )
{
    return new ModalBar_();
}

UGEN_DTOR ModalBar_dtor( t_CKTIME now, void * data )
{
    delete (ModalBar *)data;
}

UGEN_TICK ModalBar_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    ModalBar_ * b = (ModalBar_ *)data;
    *out = b->modalbar.tick();
    return TRUE;
}

UGEN_PMSG ModalBar_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL ModalBar_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.noteOn ( b->modalbar.baseFrequency , f );
}

UGEN_CTRL ModalBar_ctrl_strike( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.strike ( f );
}

UGEN_CTRL ModalBar_ctrl_damp( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.damp ( f );
}

UGEN_CTRL ModalBar_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.clear ();
}

UGEN_CTRL ModalBar_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.noteOn ( b->modalbar.baseFrequency, f);
}

UGEN_CTRL ModalBar_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.setFrequency ( f );
}

UGEN_CGET ModalBar_cget_freq( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->modalbar.baseFrequency );
}


UGEN_CTRL ModalBar_ctrl_preset( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    int f = GET_CK_INT(value);
    b->m_preset = f;
    b->modalbar.setPreset ( f );
}

UGEN_CGET ModalBar_cget_preset( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_INT( value, b->m_preset );
}

UGEN_CTRL ModalBar_ctrl_strikePosition( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.setStrikePosition ( f );
}

UGEN_CGET ModalBar_cget_strikePosition( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->modalbar.strikePosition );
}

UGEN_CTRL ModalBar_ctrl_stickHardness( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.setStickHardness ( f );
}

UGEN_CGET ModalBar_cget_stickHardness( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->modalbar.stickHardness );
}
UGEN_CTRL ModalBar_ctrl_masterGain( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.setMasterGain ( f );
}

UGEN_CGET ModalBar_cget_masterGain( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->modalbar.masterGain );
}

UGEN_CTRL ModalBar_ctrl_directGain( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.setDirectGain ( f );
}

UGEN_CGET ModalBar_cget_directGain( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->modalbar.directGain );
}


UGEN_CTRL ModalBar_ctrl_mode( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    int i = GET_CK_INT(value);
    if ( i >= 0 && i < b->modalbar.nModes ) { 
        b->m_modeIndex = i;
        b->m_modeRatio = b->modalbar.ratios[i];
        b->m_modeRadius = b->modalbar.radii[i];
    }
}

UGEN_CGET ModalBar_cget_mode( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->m_modeIndex );
}

UGEN_CTRL ModalBar_ctrl_modeGain( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->modalbar.setModeGain ( b->m_modeIndex, f );
}

UGEN_CGET ModalBar_cget_modeGain( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->modalbar.filters[b->m_modeIndex]->getGain() );
}

UGEN_CTRL ModalBar_ctrl_modeRatio( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    if ( b->m_modeIndex >= 0 && b->m_modeIndex < b->modalbar.nModes ) { 
      b->modalbar.setRatioAndRadius ( b->m_modeIndex, f , b->m_modeRadius );
      b->m_modeRatio = b->modalbar.ratios[b->m_modeIndex];
    }
}

UGEN_CGET ModalBar_cget_modeRatio( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->m_modeRatio );
}

UGEN_CTRL ModalBar_ctrl_modeRadius( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    if ( b->m_modeIndex >= 0 && b->m_modeIndex < b->modalbar.nModes ) { 
      b->modalbar.setRatioAndRadius ( b->m_modeIndex, b->m_modeRatio, f );
      b->m_modeRadius = b->modalbar.radii[b->m_modeIndex];
    }
}

UGEN_CGET ModalBar_cget_modeRadius( t_CKTIME now, void * data, void * value )
{
    ModalBar_ * b = (ModalBar_ *)data;
    SET_NEXT_FLOAT( value, b->m_modeRadius );
}


//Sitar
struct Sitar_ { 
   Sitar * imp;
   double m_frequency;

   Sitar_ ( double d ) { 
      imp = new Sitar(d);
      m_frequency = 100.0;
   }
};

UGEN_CTOR Sitar_ctor( t_CKTIME now )
{
    return new Sitar_( 30.0 );
}

UGEN_DTOR Sitar_dtor( t_CKTIME now, void * data )
{
    delete ((Sitar_ *)data)->imp;
    delete (Sitar_ *)data;
}

UGEN_TICK Sitar_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Sitar_ * b = (Sitar_ *)data;
    *out = b->imp->tick();
    return TRUE;
}

UGEN_PMSG Sitar_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Sitar_ctrl_pluck( t_CKTIME now, void * data, void * value )
{
    Sitar_ * b = (Sitar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->pluck ( f );
}

UGEN_CTRL Sitar_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Sitar_ * b = (Sitar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOn ( b->m_frequency, f );
}

UGEN_CTRL Sitar_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Sitar_ * b = (Sitar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOff ( f );
}

UGEN_CTRL Sitar_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    Sitar_ * b = (Sitar_ *)data;
    b->imp->clear();
}

UGEN_CTRL Sitar_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Sitar_ * b = (Sitar_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_frequency = f;
    b->imp->setFrequency( f );
}

UGEN_CGET Sitar_cget_freq ( t_CKTIME now, void * data, void * value )
{
    Sitar_ * b = (Sitar_ *)data;
    SET_NEXT_FLOAT( value, b->m_frequency );
}



//Saxofony
struct Saxofony_ { 
   Saxofony * imp;

   double m_frequency;
   double m_rate;

   Saxofony_ ( double d ) { 
      imp = new Saxofony(d);
      m_frequency = 100.0;
      m_rate = 0.5;
   }
};

UGEN_CTOR Saxofony_ctor( t_CKTIME now )
{
    return new Saxofony_( 30.0 );
}

UGEN_DTOR Saxofony_dtor( t_CKTIME now, void * data )
{
    delete ((Saxofony_ *)data)->imp;
    delete (Saxofony_ *)data;
}

UGEN_TICK Saxofony_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Saxofony_ * b = (Saxofony_ *)data;
    *out = b->imp->tick();
    return TRUE;
}

UGEN_PMSG Saxofony_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Saxofony_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOn ( b->m_frequency, f );
}


UGEN_CTRL Saxofony_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->noteOff ( f );
}

UGEN_CTRL Saxofony_ctrl_startBlowing( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->startBlowing ( f, b->m_rate );
}

UGEN_CTRL Saxofony_ctrl_stopBlowing( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->stopBlowing ( f );
}

UGEN_CTRL Saxofony_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    b->imp->clear();
}

UGEN_CTRL Saxofony_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_frequency = f;
    b->imp->setFrequency( f );
}

UGEN_CGET Saxofony_cget_freq ( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    SET_NEXT_FLOAT( value, b->m_frequency );
}

UGEN_CTRL Saxofony_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->m_rate = f;
}

UGEN_CGET Saxofony_cget_rate ( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    SET_NEXT_FLOAT( value, b->m_rate );
}

UGEN_CTRL Saxofony_ctrl_blowPosition( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->imp->setBlowPosition(f);
}

UGEN_CGET Saxofony_cget_blowPosition ( t_CKTIME now, void * data, void * value )
{
    Saxofony_ * b = (Saxofony_ *)data;
    SET_NEXT_FLOAT( value, b->imp->position );
}



//StifKarp

UGEN_CTOR StifKarp_ctor( t_CKTIME now )
{
    return new StifKarp( 30.0 );
}

UGEN_DTOR StifKarp_dtor( t_CKTIME now, void * data )
{
    delete (StifKarp *)data;
}

UGEN_TICK StifKarp_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    StifKarp * b = (StifKarp *)data;
    *out = b->tick();
    return TRUE;
}

UGEN_PMSG StifKarp_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL StifKarp_ctrl_pluck( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->pluck ( f );
}

UGEN_CTRL StifKarp_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->noteOn ( b->lastFrequency, f );
}

UGEN_CTRL StifKarp_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->noteOff ( f );
}

UGEN_CTRL StifKarp_ctrl_clear( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    b->clear();
}

UGEN_CTRL StifKarp_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->setFrequency( f );
}

UGEN_CGET StifKarp_cget_freq ( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    SET_NEXT_FLOAT( value, b->lastFrequency );
}

UGEN_CTRL StifKarp_ctrl_pickupPosition( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->setPickupPosition( f );
}

UGEN_CGET StifKarp_cget_pickupPosition( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    SET_NEXT_FLOAT( value, b->pickupPosition );
}

UGEN_CTRL StifKarp_ctrl_stretch( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->setStretch( f );
}

UGEN_CGET StifKarp_cget_stretch( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    SET_NEXT_FLOAT( value, b->stretching );
}

UGEN_CTRL StifKarp_ctrl_baseLoopGain( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    b->setBaseLoopGain( f );
}

UGEN_CGET StifKarp_cget_baseLoopGain ( t_CKTIME now, void * data, void * value )
{
    StifKarp * b = (StifKarp *)data;
    SET_NEXT_FLOAT( value, b->baseLoopGain );
}


// Delay
UGEN_CTOR Delay_ctor( t_CKTIME now )
{
    return new Delay;
}

UGEN_DTOR Delay_dtor( t_CKTIME now, void * data )
{
    delete (Delay *)data;
}

UGEN_TICK Delay_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    *out = (SAMPLE)((Delay *)data)->tick( in );
    return TRUE;
}

UGEN_PMSG Delay_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL Delay_ctrl_delay( t_CKTIME now, void * data, void * value )
{
    ((Delay *)data)->setDelay( (long)(GET_NEXT_DUR(value)+.5) );
}

UGEN_CTRL Delay_ctrl_max( t_CKTIME now, void * data, void * value )
{
    Delay * delay = (Delay *)data;
    t_CKFLOAT val = (t_CKFLOAT)delay->getDelay();
    t_CKDUR max = GET_NEXT_DUR(value);
    delay->set( (long)(val+.5), (long)(max+1.5) );
}

UGEN_CGET Delay_cget_delay( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((Delay *)data)->getDelay() );
}

UGEN_CGET Delay_cget_max( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((Delay *)data)->length-1.0 );
}


// DelayA
UGEN_CTOR DelayA_ctor( t_CKTIME now )
{
    return new DelayA;
}

UGEN_DTOR DelayA_dtor( t_CKTIME now, void * data )
{
    delete (DelayA *)data;
}

UGEN_TICK DelayA_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    *out = (SAMPLE)((DelayA *)data)->tick( in );
    return TRUE;
}

UGEN_PMSG DelayA_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL DelayA_ctrl_delay( t_CKTIME now, void * data, void * value )
{
    ((DelayA *)data)->setDelay( GET_NEXT_DUR(value) );
}

UGEN_CTRL DelayA_ctrl_max( t_CKTIME now, void * data, void * value )
{
    DelayA * delay = (DelayA *)data;
    t_CKDUR val = (t_CKDUR)delay->getDelay();
    t_CKDUR max = GET_NEXT_DUR(value);
    delay->set( val, (long)(max+1.5) );
}

UGEN_CGET DelayA_cget_delay( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((DelayA *)data)->getDelay() );
}

UGEN_CGET DelayA_cget_max( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((DelayA *)data)->length-1.0 );
}


// DelayL
UGEN_CTOR DelayL_ctor( t_CKTIME now )
{
    return new DelayL;
}

UGEN_DTOR DelayL_dtor( t_CKTIME now, void * data )
{
    delete (DelayL *)data;
}

UGEN_TICK DelayL_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    *out = (SAMPLE)((DelayL *)data)->tick( in );
    return TRUE;
}

UGEN_PMSG DelayL_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL DelayL_ctrl_delay( t_CKTIME now, void * data, void * value )
{
    ((DelayL *)data)->setDelay( GET_NEXT_DUR(value) );
}

UGEN_CTRL DelayL_ctrl_max( t_CKTIME now, void * data, void * value )
{
    DelayL * delay = (DelayL *)data;
    t_CKDUR val = (t_CKDUR)delay->getDelay();
    t_CKDUR max = GET_NEXT_DUR(value);
    delay->set( val, (long)(max+1.5) );
}

UGEN_CGET DelayL_cget_delay( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((DelayL *)data)->getDelay() );
}

UGEN_CGET DelayL_cget_max( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((DelayL *)data)->length-1.0 );
}


// Echo
UGEN_CTOR Echo_ctor( t_CKTIME now )
{
    return new Echo( Stk::sampleRate() / 2.0 );
}

UGEN_DTOR Echo_dtor( t_CKTIME now, void * data )
{
    delete (Echo *)data;
}

UGEN_TICK Echo_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    *out = (SAMPLE)((Echo *)data)->tick( in );
    return TRUE;
}

UGEN_PMSG Echo_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL Echo_ctrl_delay( t_CKTIME now, void * data, void * value )
{
    ((Echo *)data)->setDelay( GET_NEXT_DUR(value) );
}

UGEN_CTRL Echo_ctrl_max( t_CKTIME now, void * data, void * value )
{
    ((Echo *)data)->set( GET_NEXT_DUR(value) );
}

UGEN_CTRL Echo_ctrl_mix( t_CKTIME now, void * data, void * value )
{
    ((Echo *)data)->setEffectMix( GET_NEXT_FLOAT(value) );
}

UGEN_CGET Echo_cget_delay( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((Echo *)data)->getDelay() ); 
}

UGEN_CGET Echo_cget_max( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_DUR( value, (t_CKDUR)((Echo *)data)->length );
}

UGEN_CGET Echo_cget_mix( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_FLOAT( value, (t_CKFLOAT)((Echo *)data)->effectMix );
}



//-----------------------------------------------------------------------------
// name: Envelope - import
// desc: ..
//-----------------------------------------------------------------------------
UGEN_CTOR Envelope_ctor( t_CKTIME now )
{
    return new Envelope;
}

UGEN_DTOR Envelope_dtor( t_CKTIME now, void * data )
{
    delete (Envelope *)data;
}

UGEN_TICK Envelope_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Envelope * d = (Envelope *)data;
    *out = in * d->tick();
    return TRUE;
}

UGEN_PMSG Envelope_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL Envelope_ctrl_time( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    d->setTime( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL Envelope_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    d->setRate( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL Envelope_ctrl_target( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    d->setTarget( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL Envelope_ctrl_value( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    d->setValue( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL Envelope_ctrl_keyOn( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    if( GET_NEXT_INT(value) )
        d->keyOn();
    else
        d->keyOff();
}

UGEN_CTRL Envelope_ctrl_keyOff( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    if( !GET_NEXT_INT(value) )
        d->keyOn();
    else
        d->keyOff();
}

UGEN_CTRL Envelope_cget_target( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    SET_NEXT_FLOAT( value, d->target );
}

UGEN_CTRL Envelope_cget_value( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    SET_NEXT_FLOAT( value, d->value );
}

UGEN_CTRL Envelope_cget_rate( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
    SET_NEXT_FLOAT( value, d->rate );
}

UGEN_CTRL Envelope_cget_time( t_CKTIME now, void * data, void * value )
{
    Envelope * d = (Envelope *)data;
	SET_NEXT_FLOAT( value, 1.0 / ( d->rate * Stk::sampleRate() ) );
}

//-----------------------------------------------------------------------------
// name: ADSR - import
// desc: ..
//-----------------------------------------------------------------------------
UGEN_CTOR ADSR_ctor( t_CKTIME now )
{
    return new ADSR;
}

UGEN_DTOR ADSR_dtor( t_CKTIME now, void * data )
{
    delete (ADSR *)data;
}

UGEN_TICK ADSR_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    ADSR * d = (ADSR *)data;
    *out = in * d->tick();
    return TRUE;
}

UGEN_PMSG ADSR_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}

UGEN_CTRL ADSR_ctrl_attackTime( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setAttackTime( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_attackRate( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setAttackRate( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_decayTime( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setDecayTime( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_decayRate( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setDecayRate( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_sustainLevel( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setSustainLevel( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_releaseTime( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setReleaseTime( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_releaseRate( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setReleaseRate( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_target( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setTarget( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_value( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    d->setValue( GET_NEXT_FLOAT(value) );
}

UGEN_CTRL ADSR_ctrl_keyOn( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    if( GET_NEXT_INT(value) )
        d->keyOn();
    else
        d->keyOff();
}

UGEN_CTRL ADSR_ctrl_keyOff( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    if( !GET_NEXT_INT(value) )
        d->keyOn();
    else
        d->keyOff();
}

UGEN_CTRL ADSR_cget_attackRate( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_FLOAT( value, d->attackRate );
}

UGEN_CTRL ADSR_cget_decayRate( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_FLOAT( value, d->decayRate );
}

UGEN_CTRL ADSR_cget_sustainLevel( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_FLOAT( value, d->sustainLevel );
}

UGEN_CTRL ADSR_cget_releaseRate( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_FLOAT( value, d->releaseRate );
}

UGEN_CTRL ADSR_cget_target( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_FLOAT( value, d->target );
}

UGEN_CTRL ADSR_cget_value( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_FLOAT( value, d->value );
}

UGEN_CTRL ADSR_cget_state( t_CKTIME now, void * data, void * value )
{
    ADSR * d = (ADSR *)data;
    SET_NEXT_INT( value, d->state );
}


//Filter
UGEN_CTOR Filter_ctor( t_CKTIME now )
{
    return new Filter;
}

UGEN_DTOR Filter_dtor( t_CKTIME now, void * data )
{
    delete (Filter *)data;
}

UGEN_TICK Filter_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Filter * d = (Filter *)data;
    *out = d->tick( in );
    return TRUE;
}

UGEN_PMSG Filter_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return FALSE;
}


UGEN_CTRL Filter_ctrl_coefs( t_CKTIME now, void * data, void * value )
{
    Filter * d = (Filter *)data;
    fprintf(stderr,"Filter.coefs :: not implemented\n");
}



UGEN_CTOR OnePole_ctor ( t_CKTIME now ) 
{
  return new OnePole();
}

UGEN_DTOR OnePole_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (OnePole *)data;
}

UGEN_TICK OnePole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    OnePole * m = (OnePole *)data;
    *out = m->tick( in );
    return TRUE;
}

UGEN_PMSG OnePole_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL OnePole_ctrl_a1( t_CKTIME now, void * data, void * value )
{
    OnePole * filter = (OnePole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setA1( f );
}

UGEN_CTRL OnePole_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    OnePole * filter = (OnePole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB0( f );
}

UGEN_CTRL OnePole_ctrl_pole( t_CKTIME now, void * data, void * value )
{
    OnePole * filter = (OnePole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setPole( f );
}


UGEN_CGET OnePole_cget_a1( t_CKTIME now, void * data, void * value )
{
    OnePole * filter = (OnePole *)data;
    SET_NEXT_FLOAT( value, filter->a[1] );
}

UGEN_CGET OnePole_cget_b0( t_CKTIME now, void * data, void * value )
{
    OnePole * filter = (OnePole *)data;
    SET_NEXT_FLOAT( value, filter->b[0] );
}

UGEN_CGET OnePole_cget_pole( t_CKTIME now, void * data, void * value )
{
    OnePole * filter = (OnePole *)data;
    SET_NEXT_FLOAT( value, -filter->a[1] );
}


//TwoPole functions

UGEN_CTOR TwoPole_ctor ( t_CKTIME now ) 
{
  return new TwoPole();
}

UGEN_DTOR TwoPole_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (TwoPole *)data;
}

UGEN_TICK TwoPole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    TwoPole * m = (TwoPole *)data;
    *out = m->tick( in );
    return TRUE;
}

UGEN_PMSG TwoPole_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL TwoPole_ctrl_a1( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setA1( f );
}

UGEN_CTRL TwoPole_ctrl_a2( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setA2( f );
}

UGEN_CTRL TwoPole_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB0( f );
}

UGEN_CTRL TwoPole_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->ck_setResFreq( f );
}

UGEN_CTRL TwoPole_ctrl_radius( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->ck_setResRad( f );
}

UGEN_CTRL TwoPole_ctrl_norm( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    bool b = ( GET_CK_INT(value) != 0 ); 
    filter->ck_setResNorm( b );
}


UGEN_CGET TwoPole_cget_a1( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    SET_NEXT_FLOAT( value, filter->a[1] );
}

UGEN_CGET TwoPole_cget_a2( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    SET_NEXT_FLOAT( value, filter->a[2] );
}

UGEN_CGET TwoPole_cget_b0( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    SET_NEXT_FLOAT( value, filter->b[0] );
}

UGEN_CGET TwoPole_cget_freq( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    SET_NEXT_FLOAT( value, filter->m_resFreq );
}

UGEN_CGET TwoPole_cget_radius( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    SET_NEXT_FLOAT( value, filter->m_resRad );
}

UGEN_CGET TwoPole_cget_norm( t_CKTIME now, void * data, void * value )
{
    TwoPole * filter = (TwoPole *)data;
    SET_NEXT_INT( value, filter->m_resNorm );

}



//OneZero functions

UGEN_CTOR OneZero_ctor ( t_CKTIME now ) 
{
  return new OneZero();
}

UGEN_DTOR OneZero_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (OneZero *)data;
}

UGEN_TICK OneZero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    OneZero * m = (OneZero *)data;
    *out = m->tick( in );
    return TRUE;
}

UGEN_PMSG OneZero_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL OneZero_ctrl_zero( t_CKTIME now, void * data, void * value )
{
    OneZero * filter = (OneZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setZero( f );
}

UGEN_CTRL OneZero_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    OneZero * filter = (OneZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB0( f );
}

UGEN_CTRL OneZero_ctrl_b1( t_CKTIME now, void * data, void * value )
{
    OneZero * filter = (OneZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB1( f );
}

UGEN_CGET OneZero_cget_zero( t_CKTIME now, void * data, void * value )
{
    OneZero * filter = (OneZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    double zeeroo = ( filter->b[0] == 0 ) ? 0 : -filter->b[1] / filter->b[0]; 
    SET_NEXT_FLOAT( value, zeeroo); 
}

UGEN_CGET OneZero_cget_b0( t_CKTIME now, void * data, void * value )
{
    OneZero * filter = (OneZero *)data;
    SET_NEXT_FLOAT( value, filter->b[0] );
}

UGEN_CGET OneZero_cget_b1( t_CKTIME now, void * data, void * value )
{

    OneZero * filter = (OneZero *)data;
    SET_NEXT_FLOAT( value, filter->b[1] );
}



//TwoZero functions

UGEN_CTOR TwoZero_ctor ( t_CKTIME now ) 
{
  return new TwoZero();
}

UGEN_DTOR TwoZero_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (TwoZero *)data;
}

UGEN_TICK TwoZero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    TwoZero * m = (TwoZero *)data;
    *out = m->tick( in );
    return TRUE;
}

UGEN_PMSG TwoZero_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL TwoZero_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB0( f );
}

UGEN_CTRL TwoZero_ctrl_b1( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB1( f );
}

UGEN_CTRL TwoZero_ctrl_b2( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB2( f );
}


UGEN_CTRL TwoZero_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->ck_setNotchFreq( f );
}


UGEN_CTRL TwoZero_ctrl_radius( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->ck_setNotchRad( f );
}


UGEN_CGET TwoZero_cget_b0( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    SET_NEXT_FLOAT( value, filter->b[0] );
}

UGEN_CGET TwoZero_cget_b1( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    SET_NEXT_FLOAT( value, filter->b[1] );
}

UGEN_CGET TwoZero_cget_b2( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    SET_NEXT_FLOAT( value, filter->b[2] );
}


UGEN_CGET TwoZero_cget_freq( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    SET_NEXT_FLOAT( value, filter->m_notchFreq );
}


UGEN_CGET TwoZero_cget_radius( t_CKTIME now, void * data, void * value )
{
    TwoZero * filter = (TwoZero *)data;
    SET_NEXT_FLOAT( value, filter->m_notchRad );
}



//PoleZero functions

UGEN_CTOR PoleZero_ctor ( t_CKTIME now ) 
{
  return new PoleZero();
}

UGEN_DTOR PoleZero_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (PoleZero *)data;
}

UGEN_TICK PoleZero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    PoleZero * m = (PoleZero *)data;
    *out = m->tick( in );
    return TRUE;
}

UGEN_PMSG PoleZero_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}


UGEN_CTRL PoleZero_ctrl_a1( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setA1( f );
}

UGEN_CTRL PoleZero_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB0( f );
}

UGEN_CTRL PoleZero_ctrl_b1( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setB1( f );
}

UGEN_CTRL PoleZero_ctrl_allpass( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setAllpass( f );
}

UGEN_CTRL PoleZero_ctrl_blockZero( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    filter->setBlockZero( f );
}

UGEN_CGET PoleZero_cget_a1( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    SET_NEXT_FLOAT ( value, filter->a[1] );
}

UGEN_CGET PoleZero_cget_b0( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    SET_NEXT_FLOAT ( value, filter->b[0] );
}

UGEN_CGET PoleZero_cget_b1( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    SET_NEXT_FLOAT ( value, filter->b[1] );
}

UGEN_CGET PoleZero_cget_allpass( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    SET_NEXT_FLOAT ( value, filter->b[0] );
}

UGEN_CGET PoleZero_cget_blockZero( t_CKTIME now, void * data, void * value )
{
    PoleZero * filter = (PoleZero *)data;
    SET_NEXT_FLOAT ( value, -filter->a[1] );
}



//FM functions

UGEN_CTOR FM_ctor ( t_CKTIME now ) 
{
  //  return new FM(4);
  fprintf(stderr,"error : FM is virtual - not for use! \n");
  return 0;
}

UGEN_DTOR FM_dtor ( t_CKTIME now, void * data ) 
{ 
  //  delete (FM *)data;
    fprintf(stderr,"error : FM is virtual!\n");
}

UGEN_TICK FM_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    FM * m = (FM *)data;
    //    *out = m->tick();
    fprintf(stderr,"error : FM tick is virtual\n");
    return TRUE;
}

UGEN_PMSG FM_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL FM_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->setFrequency( f );
}



UGEN_CTRL FM_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->keyOn();
}

UGEN_CTRL FM_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->noteOff( f );
}

UGEN_CTRL FM_ctrl_modDepth( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->setModulationDepth( f );
}

UGEN_CTRL FM_ctrl_modSpeed( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->setModulationSpeed( f );
}

UGEN_CTRL FM_ctrl_control1( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->setControl1( f );
}

UGEN_CTRL FM_ctrl_control2( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->setControl2( f );
}

UGEN_CTRL FM_ctrl_afterTouch( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    fm->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
}

UGEN_CTRL FM_cget_freq( t_CKTIME now, void * data, void * value )
{
    FM * fm= (FM *)data;
    SET_NEXT_FLOAT ( value, fm->baseFrequency );
}

//let's skip the controlchange members for now...


//BeeThree functions

UGEN_CTOR BeeThree_ctor ( t_CKTIME now ) 
{
  return new BeeThree();
}

UGEN_DTOR BeeThree_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (BeeThree *)data;
}

UGEN_TICK BeeThree_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    BeeThree * m = (BeeThree *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG BeeThree_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL BeeThree_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    BeeThree * bee= (BeeThree *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    bee->noteOn( f );
}


//FMVoices functions

UGEN_CTOR FMVoices_ctor ( t_CKTIME now ) 
{
  return new FMVoices();
}

UGEN_DTOR FMVoices_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (FMVoices *)data;
}

UGEN_TICK FMVoices_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    FMVoices * m = (FMVoices *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG FMVoices_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL FMVoices_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    FMVoices * voc = (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->noteOn( f );
}

UGEN_CTRL FMVoices_ctrl_freq( t_CKTIME now, void * data, void * value )
{ 
    FMVoices * voc = (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->setFrequency( f );
}

UGEN_CTRL FMVoices_ctrl_vowel( t_CKTIME now, void * data, void * value )
{
    FMVoices * voc= (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->controlChange( __SK_Breath_, f * 128.0 );
}

UGEN_CTRL FMVoices_ctrl_spectralTilt( t_CKTIME now, void * data, void * value )
{
    FMVoices * voc= (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->controlChange( __SK_FootControl_, f * 128.0);
}

UGEN_CTRL FMVoices_ctrl_lfoSpeed( t_CKTIME now, void * data, void * value )
{
    FMVoices * voc= (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->controlChange( __SK_ModFrequency_, f * 128.0);
}

UGEN_CTRL FMVoices_ctrl_lfoDepth( t_CKTIME now, void * data, void * value )
{
    FMVoices * voc= (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->controlChange( __SK_ModWheel_, f * 128.0);
}

UGEN_CTRL FMVoices_ctrl_adsrTarget( t_CKTIME now, void * data, void * value )
{
    FMVoices * voc= (FMVoices *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    voc->controlChange( __SK_AfterTouch_Cont_, f * 128.0);
}


UGEN_CGET FMVoices_cget_freq( t_CKTIME now, void * data, void * value )
{ 
    FMVoices * voc = (FMVoices *)data;
    SET_NEXT_FLOAT ( value, voc->baseFrequency ) ;
}


//HevyMetl functions

UGEN_CTOR HevyMetl_ctor ( t_CKTIME now ) 
{
  return new HevyMetl();
}

UGEN_DTOR HevyMetl_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (HevyMetl *)data;
}

UGEN_TICK HevyMetl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    HevyMetl * m = (HevyMetl *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG HevyMetl_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL HevyMetl_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    HevyMetl * hevy= (HevyMetl *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    hevy->noteOn( f );
}


//PercFlut functions

UGEN_CTOR PercFlut_ctor ( t_CKTIME now ) 
{
  return new PercFlut();
}

UGEN_DTOR PercFlut_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (PercFlut *)data;
}

UGEN_TICK PercFlut_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    PercFlut * m = (PercFlut *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG PercFlut_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL PercFlut_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    PercFlut * perc= (PercFlut *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    perc->noteOn( f );
}

UGEN_CTRL PercFlut_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    PercFlut * perc= (PercFlut *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    perc->setFrequency( f );
}

UGEN_CGET PercFlut_cget_freq( t_CKTIME now, void * data, void * value )
{
    PercFlut * perc= (PercFlut *)data;
    SET_NEXT_FLOAT ( value, perc->baseFrequency ) ;
}


//Rhodey functions

UGEN_CTOR Rhodey_ctor ( t_CKTIME now ) 
{
  return new Rhodey();
}

UGEN_DTOR Rhodey_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (Rhodey *)data;
}

UGEN_TICK Rhodey_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Rhodey * m = (Rhodey *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG Rhodey_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Rhodey_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Rhodey * rhod= (Rhodey *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    rhod->setFrequency( f );
}

UGEN_CGET Rhodey_cget_freq( t_CKTIME now, void * data, void * value )
{
    Rhodey * rhod= (Rhodey *)data;
    SET_NEXT_FLOAT ( value, rhod->baseFrequency * 0.5 ) ;

}

UGEN_CTRL Rhodey_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Rhodey * rhod= (Rhodey *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    rhod->noteOn( f );
}

UGEN_CTRL Rhodey_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Rhodey * rhod= (Rhodey *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    rhod->noteOff( f );
}

//TubeBell functions

UGEN_CTOR TubeBell_ctor ( t_CKTIME now ) 
{
  return new TubeBell();
}

UGEN_DTOR TubeBell_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (TubeBell *)data;
}

UGEN_TICK TubeBell_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    TubeBell * m = (TubeBell *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG TubeBell_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL TubeBell_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    TubeBell * tube = (TubeBell *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    tube->noteOn( f );
}

UGEN_CTRL TubeBell_ctrl_freq( t_CKTIME now, void * data, void * value )
{ 
    TubeBell * tube= (TubeBell *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    tube->setFrequency( f );
}


UGEN_CGET TubeBell_cget_freq( t_CKTIME now, void * data, void * value )
{ 
    TubeBell * tube= (TubeBell *)data;
    SET_NEXT_FLOAT( value, tube->baseFrequency );
}



//Wurley functions

UGEN_CTOR Wurley_ctor ( t_CKTIME now ) 
{
  return new Wurley();
}

UGEN_DTOR Wurley_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (Wurley *)data;
}

UGEN_TICK Wurley_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Wurley * m = (Wurley *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG Wurley_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Wurley_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Wurley * wurl= (Wurley *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    wurl->setFrequency( f );
}


UGEN_CTRL Wurley_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Wurley * wurl= (Wurley *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    wurl->noteOn( f );
}

UGEN_CTRL Wurley_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Wurley * wurl= (Wurley *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    wurl->noteOff( f );
}

UGEN_CGET Wurley_cget_freq( t_CKTIME now, void * data, void * value )
{
    Wurley * wurl= (Wurley *)data;
    SET_NEXT_FLOAT( value, wurl->baseFrequency );
}

//FormSwep functions

UGEN_CTOR FormSwep_ctor ( t_CKTIME now ) 
{
  return new FormSwep();
}

UGEN_DTOR FormSwep_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (FormSwep *)data;
}

UGEN_TICK FormSwep_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    FormSwep * m = (FormSwep *)data;
    *out = m->tick(in);
    return TRUE;
}

UGEN_PMSG FormSwep_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

//FormSwep requires multiple arguments
//to most of its parameters. 



UGEN_CTOR JCRev_ctor( t_CKTIME now )
{
    return new JCRev( 4.0f );
}

UGEN_DTOR JCRev_dtor( t_CKTIME now, void * data )
{
    delete (JCRev *)data;
}

UGEN_TICK JCRev_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    JCRev * j = (JCRev *)data;
    *out = j->tick( in );
    return TRUE;
}

UGEN_PMSG JCRev_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL JCRev_ctrl_mix( t_CKTIME now, void * data, void * value )
{
    JCRev * j = (JCRev *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setEffectMix( f );
}

UGEN_CGET JCRev_cget_mix( t_CKTIME now, void * data, void * value )
{
    JCRev * j = (JCRev *)data;
    SET_NEXT_FLOAT ( value, j->effectMix );
}


UGEN_CTOR Mandolin_ctor ( t_CKTIME now ) 
{
  return new Mandolin( 50.0f );
}

UGEN_DTOR Mandolin_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (Mandolin *)data;
}

UGEN_TICK Mandolin_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Mandolin * m = (Mandolin *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG Mandolin_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Mandolin_ctrl_pluck( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    m->pluck( f );
}

UGEN_CTRL Mandolin_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    m->setFrequency( f );

}

UGEN_CTRL Mandolin_ctrl_pluckPos( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    m->setPluckPosition( f );
}

UGEN_CTRL Mandolin_ctrl_bodySize( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    m->setBodySize( f * 2.0 );
}

UGEN_CTRL Mandolin_ctrl_stringDamping( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    m->setBaseLoopGain( 0.97f + f * 0.03f );
}

UGEN_CTRL Mandolin_ctrl_stringDetune( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    m->setDetune( 1.0f - 0.1f * f );
}

UGEN_CTRL Mandolin_ctrl_afterTouch( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    //not sure what this does in stk version so we'll just call controlChange
    m->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
}


// cgets
UGEN_CGET Mandolin_cget_freq( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    SET_NEXT_FLOAT ( value, m->lastFrequency );
}

UGEN_CGET Mandolin_cget_pluckPos( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    SET_NEXT_FLOAT ( value, m->pluckPosition );
}

UGEN_CGET Mandolin_cget_bodySize( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    SET_NEXT_FLOAT ( value, m->m_bodySize );
}

UGEN_CGET Mandolin_cget_stringDamping( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    SET_NEXT_FLOAT ( value, m->m_stringDamping );
}

UGEN_CGET Mandolin_cget_stringDetune( t_CKTIME now, void * data, void * value )
{
    Mandolin * m = (Mandolin *)data;
    SET_NEXT_FLOAT ( value, m->m_stringDetune );
}

// Modulate
UGEN_CTOR Modulate_ctor( t_CKTIME now )
{
    return new Modulate( );
}

UGEN_DTOR Modulate_dtor( t_CKTIME now, void * data )
{
    delete (Modulate *)data;
}

UGEN_TICK Modulate_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Modulate * j = (Modulate *)data;
    *out = j->tick();
    return TRUE;
}

UGEN_PMSG Modulate_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Modulate_ctrl_vibratoRate( t_CKTIME now, void * data, void * value )
{
    Modulate * j = (Modulate *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setVibratoRate( f );
}

UGEN_CTRL Modulate_ctrl_vibratoGain( t_CKTIME now, void * data, void * value )
{
    Modulate * j = (Modulate *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setVibratoGain( f );
}

UGEN_CTRL Modulate_ctrl_randomGain( t_CKTIME now, void * data, void * value )
{
    Modulate * j = (Modulate *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setRandomGain(f );
}

UGEN_CGET Modulate_cget_vibratoRate( t_CKTIME now, void * data, void * value )
{
    Modulate * j = (Modulate *)data;
    SET_NEXT_FLOAT ( value, j->vibrato->m_freq );
}

UGEN_CGET Modulate_cget_vibratoGain( t_CKTIME now, void * data, void * value )
{
    Modulate * j = (Modulate *)data;
    SET_NEXT_FLOAT ( value, j->vibratoGain );
}

UGEN_CGET Modulate_cget_randomGain( t_CKTIME now, void * data, void * value )
{
    Modulate * j = (Modulate *)data;
    SET_NEXT_FLOAT ( value, j->randomGain );
}



UGEN_CTOR Moog_ctor ( t_CKTIME now ) 
{
  return new Moog();
}

UGEN_DTOR Moog_dtor ( t_CKTIME now, void * data ) 
{ 
  delete (Moog *)data;
}

UGEN_TICK Moog_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Moog * m = (Moog *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG Moog_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}


UGEN_CTRL Moog_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->noteOn( f );
}


UGEN_CTRL Moog_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->setFrequency ( f );
}

UGEN_CTRL Moog_ctrl_modSpeed( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->setModulationSpeed(f);
} 

UGEN_CTRL Moog_ctrl_modDepth( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->setModulationDepth(f);
}

UGEN_CTRL Moog_ctrl_filterQ( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->controlChange( __SK_FilterQ_, f * 128.0 );
}

UGEN_CTRL Moog_ctrl_filterSweepRate( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->controlChange( __SK_FilterSweepRate_, f * 128.0 );

}

UGEN_CTRL Moog_ctrl_afterTouch( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    m->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
}


UGEN_CGET Moog_cget_freq( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    SET_NEXT_FLOAT ( value , m->baseFrequency );
}

UGEN_CGET Moog_cget_modSpeed( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    SET_NEXT_FLOAT ( value , m->loops[1]->m_freq );
} 

UGEN_CGET Moog_cget_modDepth( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    SET_NEXT_FLOAT ( value, m->modDepth * 2.0 );
}

UGEN_CGET Moog_cget_filterQ( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    SET_NEXT_FLOAT ( value,  10.0 * ( m->filterQ - 0.80 ) );
}

UGEN_CGET Moog_cget_filterSweepRate( t_CKTIME now, void * data, void * value )
{
    Moog * m = (Moog *)data;
    SET_NEXT_FLOAT ( value,  m->filterRate * 5000 );
}


// NRev
UGEN_CTOR NRev_ctor( t_CKTIME now )
{
    return new NRev( 4.0f );
}

UGEN_DTOR NRev_dtor( t_CKTIME now, void * data )
{
    delete (NRev *)data;
}

UGEN_TICK NRev_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    NRev * j = (NRev *)data;
    *out = j->tick( in );
    return TRUE;
}

UGEN_PMSG NRev_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL NRev_ctrl_mix( t_CKTIME now, void * data, void * value )
{
    NRev * j = (NRev *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setEffectMix( f );
}

UGEN_CGET NRev_cget_mix( t_CKTIME now, void * data, void * value )
{
    NRev * j = (NRev *)data;
    SET_NEXT_FLOAT ( value, j->effectMix );
}



// PitShift
UGEN_CTOR PitShift_ctor( t_CKTIME now )
{
    return new PitShift( );
}

UGEN_DTOR PitShift_dtor( t_CKTIME now, void * data )
{
    delete (PitShift *)data;
}

UGEN_TICK PitShift_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    PitShift * p = (PitShift *)data;
    *out = p->tick( in );
    return TRUE;
}

UGEN_PMSG PitShift_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL PitShift_ctrl_shift( t_CKTIME now, void * data, void * value )
{
    PitShift * p = (PitShift *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setShift( f );
}

UGEN_CTRL PitShift_ctrl_effectMix( t_CKTIME now, void * data, void * value )
{
    PitShift * p = (PitShift *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    p->setEffectMix( f );
}


UGEN_CGET PitShift_cget_shift( t_CKTIME now, void * data, void * value )
{
    PitShift * p = (PitShift *)data;
    SET_NEXT_FLOAT (value,  1.0 - p->rate );
}

UGEN_CGET PitShift_cget_effectMix( t_CKTIME now, void * data, void * value )
{
    PitShift * p = (PitShift *)data;
    SET_NEXT_FLOAT (value,  p->effectMix );
}


// PRCRev
UGEN_CTOR PRCRev_ctor( t_CKTIME now )
{
    return new PRCRev( 4.0f );
}

UGEN_DTOR PRCRev_dtor( t_CKTIME now, void * data )
{
    delete (PRCRev *)data;
}

UGEN_TICK PRCRev_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    PRCRev * j = (PRCRev *)data;
    *out = j->tick( in );
    return TRUE;
}

UGEN_PMSG PRCRev_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL PRCRev_ctrl_mix( t_CKTIME now, void * data, void * value )
{
    PRCRev * j = (PRCRev *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setEffectMix( f );
}

UGEN_CGET PRCRev_cget_mix( t_CKTIME now, void * data, void * value )
{
    PRCRev * j = (PRCRev *)data;
    SET_NEXT_FLOAT ( value, j->effectMix );
}


UGEN_CTOR Shakers_ctor( t_CKTIME now )
{
    return new Shakers;
}

UGEN_DTOR Shakers_dtor( t_CKTIME now, void * data )
{
    delete (Shakers *)data;
}

UGEN_TICK Shakers_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Shakers * s = (Shakers *)data;
    *out = s->tick();
    return TRUE;
}

UGEN_PMSG Shakers_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL Shakers_ctrl_which( t_CKTIME now, void * data, void * value )
{
    Shakers * s = (Shakers *)data;
    t_CKINT c = GET_CK_INT(value);
    s->setupNum( c );
}

UGEN_CTRL Shakers_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    Shakers * s = (Shakers *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    s->ck_noteOn( f );
}

UGEN_CTRL Shakers_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    Shakers * s = (Shakers *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    s->noteOff( f );
}

UGEN_CTRL Shakers_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Shakers * s = (Shakers *)data;
    s->freq = GET_CK_FLOAT(value);
    s->controlChange( __SK_ModWheel_, s->freq );
}

UGEN_CGET Shakers_cget_which( t_CKTIME now, void * data, void * value )
{
    Shakers * s = (Shakers *)data;
    SET_NEXT_INT( value, s->m_noteNum );
}

UGEN_CGET Shakers_cget_freq( t_CKTIME now, void * data, void * value )
{
    Shakers * s = (Shakers *)data;
    SET_NEXT_FLOAT( value, s->freq);
}

// SubNoise
UGEN_CTOR SubNoise_ctor( t_CKTIME now )
{
    return new SubNoise( );
}

UGEN_DTOR SubNoise_dtor( t_CKTIME now, void * data )
{
    delete (SubNoise *)data;
}

UGEN_TICK SubNoise_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    SubNoise * p = (SubNoise *)data;
    *out = p->tick();
    return TRUE;
}

UGEN_PMSG SubNoise_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    NRev * j = (NRev *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    j->setEffectMix( f );
	return TRUE;
}

UGEN_CTRL SubNoise_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    SubNoise * p = (SubNoise *)data;
    int i = GET_CK_INT(value);
    p->setRate( i );
}

UGEN_CTRL SubNoise_cget_rate( t_CKTIME now, void * data, void * value )
{
    SET_NEXT_INT( value, (int)((SubNoise *)data)->subRate() );
}




UGEN_CTOR VoicForm_ctor( t_CKTIME now ) 
{
  return new VoicForm();
}

UGEN_DTOR VoicForm_dtor( t_CKTIME now, void * data ) 
{ 
  delete (VoicForm *)data;
}

UGEN_TICK VoicForm_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    VoicForm * m = (VoicForm *)data;
    *out = m->tick();
    return TRUE;
}

UGEN_PMSG VoicForm_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL VoicForm_ctrl_phoneme( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    char *c = GET_CK_STRING(value); 
    v->setPhoneme( c );
}

UGEN_CTRL VoicForm_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->setFrequency( f );
}

UGEN_CTRL VoicForm_ctrl_noteOn( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->noteOn( f );
}

UGEN_CTRL VoicForm_ctrl_noteOff( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->noteOff( f );
}

UGEN_CTRL VoicForm_ctrl_speak( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    v->speak();
}

UGEN_CTRL VoicForm_ctrl_quiet( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    v->quiet();
}

UGEN_CTRL VoicForm_ctrl_voiced( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->setVoiced( f );
}

UGEN_CTRL VoicForm_ctrl_unVoiced( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->setUnVoiced( f ); //not sure if this should be multiplied
}

UGEN_CTRL VoicForm_ctrl_voiceMix( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->controlChange(__SK_Breath_, f * 128.0 );
}

UGEN_CTRL VoicForm_ctrl_selPhoneme( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    int i = GET_CK_INT(value); 
    v->controlChange(__SK_FootControl_, i );
}

UGEN_CTRL VoicForm_ctrl_vibratoFreq( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->controlChange( __SK_ModFrequency_, f * 128.0 );
}

UGEN_CTRL VoicForm_ctrl_vibratoGain( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->controlChange(__SK_ModWheel_, f * 128.0 );
}

UGEN_CTRL VoicForm_ctrl_loudness( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->controlChange(__SK_AfterTouch_Cont_, f * 128.0 );
}

UGEN_CTRL VoicForm_ctrl_pitchSweepRate( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->setPitchSweepRate( f );
}


UGEN_CGET VoicForm_cget_phoneme( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_STRING ( value, (char*)Phonemes::name(v->m_phonemeNum)); //cast away const!
}

UGEN_CGET VoicForm_cget_freq( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT( value, v->voiced->m_freq );
}

UGEN_CGET VoicForm_cget_voiced( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;

    SET_NEXT_FLOAT( value, v->voiced->envelope->value);
}

UGEN_CGET VoicForm_cget_unVoiced( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT( value, v->noiseEnv->value);
}

UGEN_CGET VoicForm_cget_voiceMix( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT( value, v->voiced->envelope->value);
}

UGEN_CGET VoicForm_cget_selPhoneme( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT ( value, v->m_phonemeNum );
}

UGEN_CGET VoicForm_cget_vibratoFreq( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT ( value, v->voiced->modulator->vibrato->m_freq );
}

UGEN_CGET VoicForm_cget_vibratoGain( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT ( value, v->voiced->modulator->vibratoGain );
}

UGEN_CGET VoicForm_cget_loudness( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT( value, v->voiced->envelope->value);
}

UGEN_CGET VoicForm_cget_pitchSweepRate( t_CKTIME now, void * data, void * value )
{
    VoicForm * v = (VoicForm *)data;
    SET_NEXT_FLOAT( value, v->voiced->m_freq );
}


// WvIn
UGEN_CTOR WvIn_ctor( t_CKTIME now )
{
    return new WvIn;
}

UGEN_DTOR WvIn_dtor( t_CKTIME now, void * data )
{
    delete (WvIn *)data;
}

UGEN_TICK WvIn_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    WvIn * w = (WvIn *)data;
    *out = ( w->m_loaded ? (t_CKFLOAT)w->tick() / SHRT_MAX : (SAMPLE)0.0 );
    return TRUE;
}

UGEN_PMSG WvIn_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    VoicForm * v = (VoicForm *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value); 
    v->controlChange(__SK_ModWheel_, f * 128.0 );
	return TRUE;
}

UGEN_CTRL WvIn_ctrl_rate( t_CKTIME now, void * data, void * value )
{
    WvIn * w = (WvIn *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    w->setRate( f );
}

UGEN_CTRL WvIn_ctrl_path( t_CKTIME now, void * data, void * value )
{
    WvIn * w = (WvIn *)data;
    char * c = *(char **)value;
    try { w->openFile( c, FALSE, FALSE ); }
    catch( StkError & e )
    {

        const char * s = e.getMessage();
        // fprintf( stderr, "[chuck](via STK): WvIn cannot load file '%s'\n", c );
    }
}


UGEN_CGET WvIn_cget_rate( t_CKTIME now, void * data, void * value )
{
    WvIn * w = (WvIn *)data;
    SET_NEXT_FLOAT ( value, w->rate );
}

UGEN_CGET WvIn_cget_path( t_CKTIME now, void * data, void * value )
{
    WvIn * w = (WvIn *)data;
    SET_NEXT_STRING ( value, w->m_filename );
}

// WaveLoop
UGEN_CTOR WaveLoop_ctor( t_CKTIME now )
{
    return new WaveLoop;
}

UGEN_DTOR WaveLoop_dtor( t_CKTIME now, void * data )
{
    delete (WaveLoop *)data;
}

UGEN_TICK WaveLoop_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    WaveLoop * w = (WaveLoop *)data;
    *out = ( w->m_loaded ? (t_CKFLOAT)w->tick() / SHRT_MAX : (SAMPLE)0.0 );
    return TRUE;
}

UGEN_PMSG WaveLoop_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

UGEN_CTRL WaveLoop_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    WaveLoop * w = (WaveLoop *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    w->setFrequency( f );
}

UGEN_CTRL WaveLoop_ctrl_phase( t_CKTIME now, void * data, void * value )
{
    WaveLoop * w = (WaveLoop *)data;
    float f = (float)GET_CK_FLOAT(value);
    w->addPhase( f );
}

UGEN_CTRL WaveLoop_ctrl_phaseOffset( t_CKTIME now, void * data, void * value )
{
    WaveLoop * w = (WaveLoop *)data;
    t_CKFLOAT f = GET_CK_FLOAT(value);
    w->addPhaseOffset( f );
}



UGEN_CGET WaveLoop_cget_freq( t_CKTIME now, void * data, void * value )
{
    WaveLoop * w = (WaveLoop *)data;
    SET_NEXT_FLOAT ( value, w->m_freq );
}

UGEN_CGET WaveLoop_cget_phase( t_CKTIME now, void * data, void * value )
{
    WaveLoop * w = (WaveLoop *)data;
    SET_NEXT_FLOAT ( value, w->time / w->fileSize );
}

UGEN_CGET WaveLoop_cget_phaseOffset( t_CKTIME now, void * data, void * value )
{
    WaveLoop * w = (WaveLoop *)data;
    SET_NEXT_FLOAT ( value, w->phaseOffset );
}

std::map<WvOut *, WvOut *> g_wv;

// WvOut
UGEN_CTOR WvOut_ctor( t_CKTIME now )
{
    WvOut * yo = new WvOut;
    strcpy( yo->autoPrefix, "chuck-session" );
    return yo;
}

UGEN_DTOR WvOut_dtor( t_CKTIME now, void * data )
{
    WvOut * w = (WvOut *)data;
    w->closeFile();
    std::map<WvOut *, WvOut *>::iterator iter;
    iter = g_wv.find( w );
    g_wv.erase( iter, iter );
    delete (WvOut *)data;
}

UGEN_TICK WvOut_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    WvOut * w = (WvOut *)data;
    if( w->start ) w->tick( in );
    *out = in; // pass samples downstream
    return TRUE;
}

UGEN_PMSG WvOut_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}

// XXX chuck got mono, so we have one channel. fix later.
UGEN_CTRL WvOut_ctrl_matFilename( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    char *filename = * (char**) value;
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix)+14] = 'h';
        buffer[strlen(w->autoPrefix)+17] = 'm';
        strcat( buffer, ").mat" );
        filename = buffer;
    }
    w->openFile( filename, 1, WvOut::WVOUT_MAT, Stk::STK_SINT16 );
    g_wv[w] = w;
}

UGEN_CTRL WvOut_ctrl_sndFilename( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    char *filename = * (char**) value;
    char buffer[1024];

    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix)+14] = 'h';
        buffer[strlen(w->autoPrefix)+17] = 'm';
        strcat( buffer, ").snd" );
        filename = buffer;
    }
    w->openFile( filename, 1, WvOut::WVOUT_SND, Stk::STK_SINT16 );
    g_wv[w] = w;
}

UGEN_CTRL WvOut_ctrl_wavFilename( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    char *filename = * (char**) value;
    char buffer[1024];

    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix)+14] = 'h';
        buffer[strlen(w->autoPrefix)+17] = 'm';
        strcat( buffer, ").wav" );
        filename = buffer;
    }
    w->openFile( filename, 1, WvOut::WVOUT_WAV, Stk::STK_SINT16 );
    g_wv[w] = w;
}

UGEN_CTRL WvOut_ctrl_rawFilename( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    char *filename = * (char**) value;
    char buffer[1024];

    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix)+14] = 'h';
        buffer[strlen(w->autoPrefix)+17] = 'm';
        strcat( buffer, ").raw" );
        filename = buffer;
    }
    w->openFile( filename, 1, WvOut::WVOUT_RAW, Stk::STK_SINT16 );
    g_wv[w] = w;
}

UGEN_CTRL WvOut_ctrl_aifFilename( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    char *filename = * (char**) value;
    char buffer[1024];

    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix)+14] = 'h';
        buffer[strlen(w->autoPrefix)+17] = 'm';
        strcat( buffer, ").aiff" );
        filename = buffer;
    }
    w->openFile( filename, 1, WvOut::WVOUT_AIF, Stk::STK_SINT16 );
    g_wv[w] = w;
}

UGEN_CTRL WvOut_ctrl_closeFile( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    w->closeFile();
    
    std::map<WvOut *, WvOut *>::iterator iter;
    iter = g_wv.find( w );
    g_wv.erase( iter, iter );
}

UGEN_CTRL WvOut_ctrl_record( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    t_CKINT i = GET_NEXT_INT(value);
    w->start = i ? 1 : 0;
}

UGEN_CTRL WvOut_ctrl_autoPrefix( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    strcpy( w->autoPrefix, GET_NEXT_STRING(value) );
}

UGEN_CGET WvOut_cget_filename( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    SET_NEXT_STRING( value, w->m_filename );
}

UGEN_CGET WvOut_cget_record( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    SET_NEXT_INT( value, w->start );
}

UGEN_CGET WvOut_cget_autoPrefix( t_CKTIME now, void * data, void * value )
{
    WvOut * w = (WvOut *)data;
    SET_NEXT_STRING( value, w->autoPrefix );
}

//-----------------------------------------------------------------------------
// name: ck_detach()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL stk_detach( t_CKUINT type, void * data )
{
    std::map<WvOut *, WvOut *>::iterator iter;
    
    for( iter = g_wv.begin(); iter != g_wv.end(); iter++ )
    {
        (*iter).second->closeFile();
    }
    
    return TRUE;
}
