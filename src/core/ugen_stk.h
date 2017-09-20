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
// file: ugen_stk.h
// desc: ChucK import for Synthesis ToolKit (STK)
//                        by Perry Cook and Gary Scavone
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Ari Lazier (alazier@cs.princeton.edu)
//         Philip L. Davidson (philipd@cs.princeton.edu)
//         Mark Daly (mdaly@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __UGEN_STK_H__
#define __UGEN_STK_H__

#include "chuck_dl.h"


// query
DLL_QUERY stk_query( Chuck_DL_Query * QUERY );
t_CKBOOL  stk_detach( t_CKUINT type, void * data );


// this determines STK float type and de-denormal method
#ifndef __STK_USE_SINGLE_PRECISION__
#define MY_FLOAT double
#define CK_STK_DDN CK_DDN_DOUBLE
#else
#define MY_FLOAT float
#define CK_STK_DDN CK_DDN_SINGLE
#endif


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
#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

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

//#define PI (MY_FLOAT) 3.14159265359
//#define TWO_PI (MY_FLOAT) (2 * PI)

#define ONE_OVER_128 (MY_FLOAT) 0.0078125

#if defined(__WINDOWS_PTHREAD__)
  #define __OS_WINDOWS_CYGWIN__
  #define __STK_REALTIME__
#elif defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)
  #define __OS_WINDOWS__
  #define __STK_REALTIME__
#elif defined(__PLATFORM_LINUX__)
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
  MY_FLOAT m_target; // chuck
  MY_FLOAT m_time; // chuck
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
  virtual ~ADSR(void);

  //! Set target = 1, state = \e ADSR::ATTACK.
  virtual void keyOn(void);

  //! Set target = 0, state = \e ADSR::RELEASE.
  virtual void keyOff(void);

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

  // chuck
  MY_FLOAT getAttackTime();
  MY_FLOAT getDecayTime();
  MY_FLOAT getReleaseTime();

public:
  MY_FLOAT attackRate;
  MY_FLOAT decayRate;
  MY_FLOAT sustainLevel;
  MY_FLOAT releaseRate;
  // chuck
  MY_FLOAT m_decayTime;
  MY_FLOAT m_releaseTime;
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
  // chuck
  t_CKFLOAT m_frequency;

  MY_FLOAT lastOutput;
};


#endif




/***************************************************/
/*! \class Filter (renamed to FilterStk)
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


class FilterStk : public Stk
{
public:
  //! Default constructor creates a zero-order pass-through "filter".
  FilterStk(void);

  //! Overloaded constructor which takes filter coefficients.
  /*!
    An StkError can be thrown if either \e nb or \e na is less than
    one, or if the a[0] coefficient is equal to zero.
  */
  FilterStk(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients);

  //! Class destructor.
  virtual ~FilterStk(void);

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


class Delay : public FilterStk
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


class BiQuad : public FilterStk
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
  //chuck
  t_CKFLOAT m_rate;
  t_CKINT m_preset;
  t_CKFLOAT m_bowPressure;
  t_CKFLOAT m_bowMotion;
  t_CKFLOAT m_modesGain;
  t_CKFLOAT m_strikePosition;

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
  // char m_filename[256]; // chuck data
  Chuck_String str_filename; // chuck data
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
  MY_FLOAT m_freq; // chuck data;
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


class TwoZero : public FilterStk // formerly protected Filter
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


class PoleZero : public FilterStk // formerly protected Filter
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

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 public: // SWAP formerly protected  
  // chuck
  t_CKFLOAT m_rate;
  t_CKFLOAT m_noiseGain;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_volume;
  
  void setVibratoFreq(MY_FLOAT freq)
  { vibrato->setFrequency( freq ); m_vibratoFreq = vibrato->m_freq; }
  void setVibratoGain(MY_FLOAT gain)
  { vibratoGain = gain; m_vibratoGain = vibratoGain; }
  void setNoiseGain(MY_FLOAT gain)
  { noiseGain = gain; m_noiseGain = gain; }

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


class OneZero : public FilterStk // formerly protected Filter
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

  // CHUCK
  t_CKFLOAT m_reed;
  t_CKFLOAT m_noiseGain;
  t_CKFLOAT m_tonehole;
  t_CKFLOAT m_vent;
  t_CKFLOAT m_pressure;
  t_CKFLOAT m_rate;

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

class OnePole : public FilterStk // formerly protected Filter
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

  // CHUCK
  t_CKFLOAT m_bowPressure;
  t_CKFLOAT m_bowPosition;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_volume;
  t_CKFLOAT m_rate;

  void setVibratoFreq(MY_FLOAT freq)
  { vibrato->setFrequency( freq ); m_vibratoFreq = vibrato->m_freq; }
  void setVibratoGain(MY_FLOAT gain)
  { vibratoGain = gain; m_vibratoGain = vibratoGain; }

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

  // CHUCK
  t_CKFLOAT m_rate;
  t_CKFLOAT m_lip;
  t_CKFLOAT m_slide;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_volume;

  void setVibratoFreq(MY_FLOAT freq)
  { vibrato->setFrequency( freq ); m_vibratoFreq = vibrato->m_freq; }
  void setVibratoGain(MY_FLOAT gain)
  { vibratoGain = gain; m_vibratoGain = vibratoGain; }

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

  //! Set baseDelay and modDepth
  void set(MY_FLOAT baseDelay, MY_FLOAT depth);

  //! Reset and clear all internal state.
  void clear();

  //! Set base delay
  void setDelay(MY_FLOAT baseDelay); // chuck

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

  // CHUCK
  t_CKFLOAT m_reed;
  t_CKFLOAT m_noiseGain;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_volume;
  t_CKFLOAT m_rate;

  void setVibratoFreq(MY_FLOAT freq)
  { vibrato->setFrequency( freq ); m_vibratoFreq = vibrato->m_freq; }
  void setVibratoGain(MY_FLOAT gain)
  { vibratoGain = gain; m_vibratoGain = vibratoGain; }
  void setNoiseGain(MY_FLOAT gain)
  { noiseGain = gain; m_noiseGain = gain; }

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

  // CHUCK
  t_CKFLOAT m_jetDelay;
  t_CKFLOAT m_jetReflection;
  t_CKFLOAT m_endReflection;
  t_CKFLOAT m_noiseGain;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_pressure;
  t_CKFLOAT m_rate;
 
  void setVibratoFreq(MY_FLOAT freq)
  { vibrato->setFrequency( freq ); m_vibratoFreq = vibrato->m_freq; }
  void setVibratoGain(MY_FLOAT gain)
  { vibratoGain = gain; m_vibratoGain = vibratoGain; }
  void setNoiseGain(MY_FLOAT gain)
  { noiseGain = gain; m_noiseGain = gain; }
 
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

  //! Set the body impulse response
  bool setBodyIR( const char * path, bool isRaw );

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
    // MY_FLOAT m_stringDamping;
    // MY_FLOAT m_stringDetune;
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
  // chuck
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_volume;

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
  //chuck
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_volume;

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
  //chuck
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_vibratoFreq;
  t_CKFLOAT m_volume;

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
  // chuck
  t_CKFLOAT m_rate;
  t_CKFLOAT m_stiffness;
  t_CKFLOAT m_aperture;
  t_CKFLOAT m_noiseGain;
  t_CKFLOAT m_vibratoGain;
  t_CKFLOAT m_pressure;

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
  // chuck
  t_CKFLOAT m_energy;
  t_CKFLOAT m_decay;
  t_CKFLOAT m_objects;

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
    // chuck
    t_CKFLOAT m_sustain;

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
#include "util_thread.h"

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

    // chuck: asynchronous data writer thread
    static XWriteThread * s_writeThread;
    
    // chuck: override stdio fwrite/etc. functions
    size_t fwrite(const void * ptr, size_t size, size_t nitems, FILE * stream);
    int fseek(FILE *stream, long offset, int whence);
    int fflush(FILE *stream);
    int fclose(FILE *stream);
    size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
    
    static void shutdown();
    
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
  // char m_filename[1024];
  Chuck_String str_filename;
  t_CKUINT start;
  // char autoPrefix[1024];
  Chuck_String autoPrefix;
  t_CKUINT flush;
  t_CKFLOAT fileGain;
    
    t_CKBOOL asyncIO;
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


class TwoPole : public FilterStk // formerly protected Filter
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
  Chuck_String str_phoneme; // chuck data
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
  
  // CHUCK HACK:
  virtual void controlChange( int which, MY_FLOAT value );

  //! Compute one output sample.
  MY_FLOAT tick();
};

#endif




/***************************************************/
/*! \class Blit
    \brief STK band-limited impulse train class.

    This class generates a band-limited impulse train using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the impulse train
    and the number of harmonics contained in the resulting signal.

    The signal is normalized so that the peak value is +/-1.0.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Original code by Robin Davies, 2005.
    Revisions by Gary Scavone for STK, 2005.
*/
/***************************************************/

class BLT
{
public:
    virtual ~BLT() {}

    virtual void setPhase( MY_FLOAT phase ) = 0;
    virtual void setFrequency( MY_FLOAT freq ) = 0;
    virtual void setHarmonics( unsigned int harmonics ) = 0;

    t_CKFLOAT getValuePhase() { return m_phase; }
    t_CKFLOAT getValueFreq() { return m_freq; }
    t_CKINT getValueHarmonics() { return m_harmonics; }

public:
    t_CKFLOAT m_phase;
    t_CKFLOAT m_freq;
    t_CKINT m_harmonics;
};

#ifndef STK_BLIT_H
#define STK_BLIT_H


class Blit : public BLT
{
 public:
  //! Default constructor that initializes BLIT frequency to 220 Hz.
  Blit( MY_FLOAT frequency = 220.0 );

  //! Class destructor.
  virtual ~Blit();

  //! Resets the oscillator state and phase to 0.
  void reset();

  //! Set the phase of the signal.
  /*!
    Set the phase of the signal, in the range 0 to 1.
  */
  virtual void setPhase( MY_FLOAT phase ) { phase_ = ONE_PI * phase; }

  //! Get the current phase of the signal.
  /*!
    Get the phase of the signal, in the range [0 to 1.0).
  */
  MY_FLOAT getPhase() const { return phase_ / ONE_PI; }

  //! Set the impulse train rate in terms of a frequency in Hz.
  virtual void setFrequency( MY_FLOAT frequency );

  //! Set the number of harmonics generated in the signal.
  /*!
    This function sets the number of harmonics contained in the
    resulting signal.  It is equivalent to (2 * M) + 1 in the BLIT
    algorithm.  The default value of 0 sets the algorithm for maximum
    harmonic content (harmonics up to half the sample rate).  This
    parameter is not checked against the current sample rate and
    fundamental frequency.  Thus, aliasing can result if one or more
    harmonics for a given fundamental frequency exceeds fs / 2.  This
    behavior was chosen over the potentially more problematic solution
    of automatically modifying the M parameter, which can produce
    audible clicks in the signal.
  */
  virtual void setHarmonics( unsigned int nHarmonics = 0 );

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:

  void updateHarmonics( void );
  MY_FLOAT computeSample( void );

  unsigned int nHarmonics_;
  unsigned int m_;
  MY_FLOAT rate_;
  MY_FLOAT phase_;
  MY_FLOAT p_;

};

#endif




/***************************************************/
/*! \class BlitSaw
    \brief STK band-limited sawtooth wave class.

    This class generates a band-limited sawtooth waveform using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the sawtooth and the
    number of harmonics contained in the resulting signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005.
*/
/***************************************************/

#ifndef STK_BLITSAW_H
#define STK_BLITSAW_H

class BlitSaw : public BLT
{
 public:
  //! Class constructor.
  BlitSaw( MY_FLOAT frequency = 220.0 );

  //! Class destructor.
  virtual ~BlitSaw();

  //! Resets the oscillator state and phase to 0.
  void reset();

  //! Set the phase of the signal.
  /*!
    Set the phase of the signal, in the range 0 to 1.
  */
  virtual void setPhase( MY_FLOAT phase ) { phase_ = ONE_PI * phase; }
  
  //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
  virtual void setFrequency( MY_FLOAT frequency );

  //! Set the number of harmonics generated in the signal.
  /*!
    This function sets the number of harmonics contained in the
    resulting signal.  It is equivalent to (2 * M) + 1 in the BLIT
    algorithm.  The default value of 0 sets the algorithm for maximum
    harmonic content (harmonics up to half the sample rate).  This
    parameter is not checked against the current sample rate and
    fundamental frequency.  Thus, aliasing can result if one or more
    harmonics for a given fundamental frequency exceeds fs / 2.  This
    behavior was chosen over the potentially more problematic solution
    of automatically modifying the M parameter, which can produce
    audible clicks in the signal.
  */
  virtual void setHarmonics( unsigned int nHarmonics = 0 );

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:

  void updateHarmonics( void );
  MY_FLOAT computeSample( void );

  unsigned int nHarmonics_;
  unsigned int m_;
  MY_FLOAT rate_;
  MY_FLOAT phase_;
  MY_FLOAT p_;
  MY_FLOAT C2_;
  MY_FLOAT a_;
  MY_FLOAT state_;

};

#endif




/***************************************************/
/*! \class BlitSquare
    \brief STK band-limited square wave class.

    This class generates a band-limited square wave signal.  It is
    derived in part from the approach reported by Stilson and Smith in
    "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996.
    The algorithm implemented in this class uses a SincM function with
    an even M value to achieve a bipolar bandlimited impulse train.
    This signal is then integrated to achieve a square waveform.  The
    integration process has an associated DC offset but that is
    subtracted off the output signal.

    The user can specify both the fundamental frequency of the
    waveform and the number of harmonics contained in the resulting
    signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005.
*/
/***************************************************/

#ifndef STK_BLITSQUARE_H
#define STK_BLITSQUARE_H

class BlitSquare : public BLT
{
 public:
  //! Default constructor that initializes BLIT frequency to 220 Hz.
  BlitSquare( MY_FLOAT frequency = 220.0 );

  //! Class destructor.
  virtual ~BlitSquare();

  //! Resets the oscillator state and phase to 0.
  void reset();

  //! Set the phase of the signal.
  /*!
    Set the phase of the signal, in the range 0 to 1.
  */
  virtual void setPhase( MY_FLOAT phase ) { phase_ = ONE_PI * phase; }

  //! Get the current phase of the signal.
  /*!
    Get the phase of the signal, in the range [0 to 1.0).
  */
  MY_FLOAT getPhase() const { return phase_ / ONE_PI; }

  //! Set the impulse train rate in terms of a frequency in Hz.
  virtual void setFrequency( MY_FLOAT frequency );

  //! Set the number of harmonics generated in the signal.
  /*!
    This function sets the number of harmonics contained in the
    resulting signal.  It is equivalent to (2 * M) + 1 in the BLIT
    algorithm.  The default value of 0 sets the algorithm for maximum
    harmonic content (harmonics up to half the sample rate).  This
    parameter is not checked against the current sample rate and
    fundamental frequency.  Thus, aliasing can result if one or more
    harmonics for a given fundamental frequency exceeds fs / 2.  This
    behavior was chosen over the potentially more problematic solution
    of automatically modifying the M parameter, which can produce
    audible clicks in the signal.
  */
  virtual void setHarmonics( unsigned int nHarmonics = 0 );

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:

  void updateHarmonics( void );
  MY_FLOAT computeSample( void );

  unsigned int nHarmonics_;
  unsigned int m_;
  MY_FLOAT rate_;
  MY_FLOAT phase_;
  MY_FLOAT a_;
  MY_FLOAT p_;
  MY_FLOAT offset_;
  MY_FLOAT m_boutput;
  MY_FLOAT m_output;
  MY_FLOAT dcbState_;
};

#endif


#ifndef STK_MIDIFILEIN_H
#define STK_MIDIFILEIN_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace stk {
    
/**********************************************************************/
/*! \class MidiFileIn
 \brief A standard MIDI file reading/parsing class.
 
 This class can be used to read events from a standard MIDI file.
 Event bytes are copied to a C++ vector and must be subsequently
 interpreted by the user.  The function getNextMidiEvent() skips
 meta and sysex events, returning only MIDI channel messages.
 Event delta-times are returned in the form of "ticks" and a
 function is provided to determine the current "seconds per tick".
 Tempo changes are internally tracked by the class and reflected in
 the values returned by the function getTickSeconds().
 
 by Gary P. Scavone, 2003 - 2010.
 */
/**********************************************************************/

class MidiFileIn : public Stk
{
public:
    //! Default constructor.
    /*!
     If an error occurs while opening or parsing the file header, an
     StkError exception will be thrown.
     */
    MidiFileIn( std::string fileName );
    
    //! Class destructor.
    ~MidiFileIn();
    
    //! Return the MIDI file format (0, 1, or 2).
    int getFileFormat() const { return format_; };
    
    //! Return the number of tracks in the MIDI file.
    unsigned int getNumberOfTracks() const { return nTracks_; };
    
    //! Return the MIDI file division value from the file header.
    /*!
     Note that this value must be "parsed" in accordance with the
     MIDI File Specification.  In particular, if the MSB is set, the
     file uses time-code representations for delta-time values.
     */
    int getDivision() const { return division_; };
    
    //! Move the specified track event reader to the beginning of its track.
    /*!
     The relevant track tempo value is reset as well.  If an invalid
     track number is specified, an StkError exception will be thrown.
     */
    void rewindTrack( unsigned int track = 0 );
    
    //! Get the current value, in seconds, of delta-time ticks for the specified track.
    /*!
     This value can change as events are read (via "Set Tempo"
     Meta-Events).  Therefore, one should call this function after
     every call to getNextEvent() or getNextMidiEvent().  If an
     invalid track number is specified, an StkError exception will be
     thrown.
     */
    double getTickSeconds( unsigned int track = 0 );
    
    //! Fill the user-provided vector with the next event in the specified track and return the event delta-time in ticks.
    /*!
     MIDI File events consist of a delta time and a sequence of event
     bytes.  This function returns the delta-time value and writes
     the subsequent event bytes directly to the event vector.  The
     user must parse the event bytes in accordance with the MIDI File
     Specification.  All returned MIDI channel events are complete
     ... a status byte is provided even when running status is used
     in the file.  If the track has reached its end, no bytes will be
     written and the event vector size will be zero.  If an invalid
     track number is specified or an error occurs while reading the
     file, an StkError exception will be thrown.
     */
    unsigned long getNextEvent( std::vector<unsigned char> *event, unsigned int track = 0 );
    
    //! Fill the user-provided vector with the next MIDI channel event in the specified track and return the event delta time in ticks.
    /*!
     All returned MIDI events are complete ... a status byte is
     provided even when running status is used in the file.  Meta and
     sysex events in the track are skipped though "Set Tempo" events
     are properly parsed for use by the getTickSeconds() function.
     If the track has reached its end, no bytes will be written and
     the event vector size will be zero.  If an invalid track number
     is specified or an error occurs while reading the file, an
     StkError exception will be thrown.
     */
    unsigned long getNextMidiEvent( std::vector<unsigned char> *midiEvent, unsigned int track = 0 );
    
protected:
    
    // This protected class function is used for reading variable-length
    // MIDI file values. It is assumed that this function is called with
    // the file read pointer positioned at the start of a
    // variable-length value.  The function returns true if the value is
    // successfully parsed.  Otherwise, it returns false.
    bool readVariableLength( unsigned long *value );
    
    std::ifstream file_;
    unsigned int nTracks_;
    int format_;
    int division_;
    bool usingTimeCode_;
    std::vector<double> tickSeconds_;
    std::vector<long> trackPointers_;
    std::vector<long> trackOffsets_;
    std::vector<long> trackLengths_;
    std::vector<char> trackStatus_;
    
    // This structure and the following variables are used to save and
    // keep track of a format 1 tempo map (and the initial tickSeconds
    // parameter for formats 0 and 2).
    struct TempoChange { 
        unsigned long count;
        double tickSeconds;
    };
    std::vector<TempoChange> tempoEvents_;
    std::vector<unsigned long> trackCounters_;
    std::vector<unsigned int> trackTempoIndex_;
};
    
} // stk namespace

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




#endif
