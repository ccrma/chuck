/***************************************************/
/*! \class Skini
    \brief STK SKINI parsing class

    This class parses SKINI formatted text
    messages.  It can be used to parse individual
    messages or it can be passed an entire file.
    The SKINI specification is Perry's and his
    alone, but it's all text so it shouldn't be too
    hard to figure out.

    SKINI (Synthesis toolKit Instrument Network
    Interface) is like MIDI, but allows for
    floating-point control changes, note numbers,
    etc.  The following example causes a sharp
    middle C to be played with a velocity of 111.132:

    \code
    noteOn  60.01  111.132
    \endcode

    \sa \ref skini

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef SKINI_H
#define SKINI_H

//#include "Stk.h"
#include "ugen_stk.h"
#include <vector>
#include <string>
#include <fstream>

// from skini.msg

/*********************************************************/
/*
  Definition of SKINI Message Types and Special Symbols
     Synthesis toolKit Instrument Network Interface

  These symbols should have the form:

   \c __SK_<name>_

  where <name> is the string used in the SKINI stream.

  by Perry R. Cook, 1995 - 2004.
*/
/*********************************************************/

#define NOPE        -32767
#define YEP         1
#define SK_DBL      -32766
#define SK_INT      -32765
#define SK_STR      -32764
#define __SK_Exit_  999

/***** MIDI COMPATIBLE MESSAGES *****/
/*** (Status bytes for channel=0) ***/

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

// end of skini.msg


// from skini.tbl
#define __SK_MaxMsgTypes_ 80

struct SkiniSpec { char messageString[32];
                   long  type;
                   long data2;
                   long data3;
                 };

/* SEE COMMENT BLOCK AT BOTTOM FOR FIELDS AND USES   */
/* MessageString     , type,     data2,     data3    */ 
//break;
//return;
//goto later;
extern struct SkiniSpec skini_msgs[__SK_MaxMsgTypes_];


class Skini //: public Stk
{
 public:

  //! A message structure to store and pass parsed SKINI messages.
  struct Message { 
    long type;                         /*!< The message type, as defined in SKINI.msg. */
    long channel;                      /*!< The message channel (not limited to 16!). */
    float time;                        /*!< The message time stamp in seconds (delta or absolute). */
    std::vector<float> floatValues;    /*!< The message values read as floats (values are type-specific). */
    std::vector<long> intValues;       /*!< The message values read as ints (number and values are type-specific). */
    std::string remainder;             /*!< Any remaining message data, read as ascii text. */

    // Default constructor.
    Message()
      :type(0), channel(0), time(0.0), floatValues(2), intValues(2) {}
  };

  //! Default constructor.
  Skini();

  //! Class destructor
  ~Skini();

  //! Set a SKINI formatted file for reading.
  /*!
    If the file is successfully opened, this function returns \e
    true.  Otherwise, \e false is returned.
   */
  bool setFile( std::string fileName );

  //! Parse the next file message (if a file is loaded) and return the message type.
  /*!
    This function skips over lines in a file which cannot be
    parsed.  A type value equal to zero in the referenced message
    structure (and the returned value) indicates the file end is
    reached or no file is open for reading.
  */
  long nextMessage( Skini::Message& message );

  //! Attempt to parse the given string and returning the message type.
  /*!
    A type value equal to zero in the referenced message structure
    indicates an invalid message.
  */
  long parseString( std::string& line, Skini::Message& message );

  //! Attempt to write to file.
  /*!
	Given limited information instead of a whole message
  */
  bool write_message( long type, float time, long channel, float data1, float data2 );

  //! Close file_
  void close_file();

  //! Return the SKINI type string for the given type value.
  static std::string whatsThisType(long type);

  //! Return the SKINI controller string for the given controller number.
  static std::string whatsThisController(long number);

 protected:

  void tokenize( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters );

  std::fstream file_;
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


