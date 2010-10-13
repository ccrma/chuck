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

    noteOn  60.01  111.132

    See also SKINI.txt.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Skini.h"
//#include "SKINI.tbl"

Skini :: Skini()
{
}

Skini :: ~Skini()
{
}

bool Skini :: setFile( std::string fileName )
{
  if ( file_.is_open() ) {
    //errorString_ << "Skini::setFile: already reaading a file!";
    //handleError( StkError::WARNING );
    return false;
  }

  file_.open( fileName.c_str() );
  if ( !file_ ) {
//    errorString_ << "Skini::setFile: unable to open file (" << fileName << ")";
//    handleError( StkError::WARNING );
    return false;
  }

  return true;
}

long Skini :: nextMessage( Message& message )
{
  if ( !file_.is_open() ) return 0;

  std::string line;
  bool done = false;
  while ( !done ) {
    // Read a line from the file and skip over invalid messages.
    if ( std::getline( file_, line ).eof() ) {
//      errorString_ << "// End of Score.  Thanks for using SKINI!!";
//      handleError( StkError::STATUS );
      file_.close();
      message.type = 0;
      done = true;
    }
    else if ( parseString( line, message ) > 0 ) done = true;
  }

  return message.type;  
}


bool Skini :: write_message( long type, float time, long channel, float data1, float data2 )
{
	if( !file_.is_open() ) return 0;

	char messageString[32];
	char * message = new char[64];

	// find message string from type
	int iSkini = 0;
	while ( iSkini < __SK_MaxMsgTypes_ ) {
		if ( type == skini_msgs[iSkini].type ) 
		{
			strcpy( messageString, skini_msgs[iSkini].messageString );
			break;
		}
		iSkini++;
	}

	// make up some code that makes sense to you if not to the compiler
	printf( message, "%s %f %i %f %f\n", messageString, time, channel, data1, data2 );
	file_.write( message, 64 * sizeof(char) );
	return true;
}	


void Skini :: close_file()
{
	if( file_.is_open() )
		file_.close();
}


void Skini :: tokenize( const std::string&        str,
                        std::vector<std::string>& tokens,
                        const std::string&        delimiters )
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
} 

long Skini :: parseString( std::string& line, Message& message )
{
  message.type = 0;
  if ( line.empty() ) return message.type;

  // Check for comment lines.
  std::string::size_type lastPos = line.find_first_not_of(" ,\t", 0);
  std::string::size_type pos     = line.find_first_of("/", lastPos);
  if ( std::string::npos != pos ) {
//    errorString_ << "// Comment Line: " << line;
//    handleError( StkError::STATUS );
    return message.type;
  }

  // Tokenize the string.
  std::vector<std::string> tokens; 
  this->tokenize( line, tokens, " ,\t");

  // Valid SKINI messages must have at least three fields (type, time,
  // and channel).
  if ( tokens.size() < 3 ) return message.type;

  // Determine message type.
  int iSkini = 0;
  while ( iSkini < __SK_MaxMsgTypes_ ) {
    if ( tokens[0] == skini_msgs[iSkini].messageString ) break;
    iSkini++;
  }

  if ( iSkini >= __SK_MaxMsgTypes_ )  {
//    errorString_ << "Skini::parseString: couldn't parse this line:\n   " << line;
//    handleError( StkError::WARNING );
    return message.type;
  }
  
  // Found the type.
  message.type = skini_msgs[iSkini].type;

  // Parse time field.
  if ( tokens[1][0] == '=' ) {
    tokens[1].erase( tokens[1].begin() );
    if ( tokens[1].empty() ) {
//      errorString_ << "Skini::parseString: couldn't parse time field in line:\n   " << line;
//      handleError( StkError::WARNING );
      return message.type = 0;
    }
    message.time = (float) -atof( tokens[1].c_str() );
  }
  else
    message.time = (float) atof( tokens[1].c_str() );

  // Parse the channel field.
  message.channel = atoi( tokens[2].c_str() );

  // Parse the remaining fields (maximum of 2 more).
  int iValue = 0;
  long dataType = skini_msgs[iSkini].data2;
  while ( dataType != NOPE ) {

    if ( tokens.size() <= (unsigned int) (iValue+3) ) {
//      errorString_ <<  "Skini::parseString: inconsistency between type table and parsed line:\n   " << line;
//      handleError( StkError::WARNING );
      return message.type = 0;
    }

    switch ( dataType ) {

    case SK_INT:
      message.intValues[iValue] = atoi( tokens[iValue+3].c_str() );
      message.floatValues[iValue] = (float) message.intValues[iValue];
      break;

    case SK_DBL:
      message.floatValues[iValue] = atof( tokens[iValue+3].c_str() );
      message.intValues[iValue] = (long) message.floatValues[iValue];
      break;

    case SK_STR: // Must be the last field.
      message.remainder = tokens[iValue+3];
      return message.type;
    }

    if ( ++iValue == 1 )
      dataType = skini_msgs[iSkini].data3;
    else
      dataType = NOPE;
  }

  return message.type;
}

std::string Skini :: whatsThisType(long type)
{
  std::string typeString;

  for ( int i=0; i<__SK_MaxMsgTypes_; i++ ) {
    if ( type == skini_msgs[i].type ) {
      typeString = skini_msgs[i].messageString;
      break;
    }
  }
  return typeString;
}

std::string Skini :: whatsThisController( long number )
{
  std::string controller;

  for ( int i=0; i<__SK_MaxMsgTypes_; i++) {
    if ( skini_msgs[i].type == __SK_ControlChange_ &&
         number == skini_msgs[i].data2) {
      controller = skini_msgs[i].messageString;
      break;
    }
  }
  return controller;
}


struct SkiniSpec skini_msgs[__SK_MaxMsgTypes_] = 
{
 {"NoteOff"          ,        __SK_NoteOff_,                   SK_DBL,  SK_DBL},
 {"NoteOn"           ,         __SK_NoteOn_,                   SK_DBL,  SK_DBL},
 {"PolyPressure"     ,   __SK_PolyPressure_,                   SK_DBL,  SK_DBL},
 {"ControlChange"    ,  __SK_ControlChange_,                   SK_INT,  SK_DBL},
 {"ProgramChange"    ,  __SK_ProgramChange_,                   SK_DBL,    NOPE},
 {"AfterTouch"       ,     __SK_AfterTouch_,                   SK_DBL,    NOPE},
 {"ChannelPressure"  ,__SK_ChannelPressure_,                   SK_DBL,    NOPE},
 {"PitchWheel"       ,     __SK_PitchWheel_,                   SK_DBL,    NOPE},
 {"PitchBend"        ,      __SK_PitchBend_,                   SK_DBL,    NOPE},
 {"PitchChange"      ,    __SK_PitchChange_,                   SK_DBL,    NOPE},

 {"Clock"            ,          __SK_Clock_,                     NOPE,    NOPE},
 {"Undefined"        ,                  249,                     NOPE,    NOPE},
 {"SongStart"        ,      __SK_SongStart_,                     NOPE,    NOPE},
 {"Continue"         ,       __SK_Continue_,                     NOPE,    NOPE},
 {"SongStop"         ,       __SK_SongStop_,                     NOPE,    NOPE},
 {"Undefined"        ,                  253,                     NOPE,    NOPE},
 {"ActiveSensing"    ,  __SK_ActiveSensing_,                     NOPE,    NOPE},
 {"SystemReset"      ,    __SK_SystemReset_,                     NOPE,    NOPE},

 {"Volume"           ,  __SK_ControlChange_, __SK_Volume_            ,  SK_DBL},
 {"ModWheel"         ,  __SK_ControlChange_, __SK_ModWheel_          ,  SK_DBL},
 {"Modulation"       ,  __SK_ControlChange_, __SK_Modulation_        ,  SK_DBL},
 {"Breath"           ,  __SK_ControlChange_, __SK_Breath_            ,  SK_DBL},
 {"FootControl"      ,  __SK_ControlChange_, __SK_FootControl_       ,  SK_DBL},
 {"Portamento"       ,  __SK_ControlChange_, __SK_Portamento_        ,  SK_DBL},
 {"Balance"          ,  __SK_ControlChange_, __SK_Balance_           ,  SK_DBL},
 {"Pan"              ,  __SK_ControlChange_, __SK_Pan_               ,  SK_DBL},
 {"Sustain"          ,  __SK_ControlChange_, __SK_Sustain_           ,  SK_DBL},
 {"Damper"           ,  __SK_ControlChange_, __SK_Damper_            ,  SK_DBL},
 {"Expression"       ,  __SK_ControlChange_, __SK_Expression_        ,  SK_DBL},

 {"NoiseLevel"       ,  __SK_ControlChange_, __SK_NoiseLevel_        ,  SK_DBL},
 {"PickPosition"     ,  __SK_ControlChange_, __SK_PickPosition_      ,  SK_DBL},
 {"StringDamping"    ,  __SK_ControlChange_, __SK_StringDamping_     ,  SK_DBL},
 {"StringDetune"     ,  __SK_ControlChange_, __SK_StringDetune_      ,  SK_DBL},
 {"BodySize"         ,  __SK_ControlChange_, __SK_BodySize_          ,  SK_DBL},
 {"BowPressure"      ,  __SK_ControlChange_, __SK_BowPressure_       ,  SK_DBL},
 {"BowPosition"      ,  __SK_ControlChange_, __SK_BowPosition_       ,  SK_DBL},
 {"BowBeta"          ,  __SK_ControlChange_, __SK_BowBeta_           ,  SK_DBL},
 
 {"ReedStiffness"    ,  __SK_ControlChange_, __SK_ReedStiffness_     ,  SK_DBL},
 {"ReedRestPos"      ,  __SK_ControlChange_, __SK_ReedRestPos_       ,  SK_DBL},
 {"FluteEmbouchure"  ,  __SK_ControlChange_, __SK_FluteEmbouchure_   ,  SK_DBL},
 {"LipTension"       ,  __SK_ControlChange_, __SK_LipTension_        ,  SK_DBL},
 {"StrikePosition"   ,  __SK_ControlChange_, __SK_StrikePosition_    ,  SK_DBL},
 {"StickHardness"    ,  __SK_ControlChange_, __SK_StickHardness_     ,  SK_DBL},

 {"TrillDepth"       ,  __SK_ControlChange_, __SK_TrillDepth_        ,  SK_DBL}, 
 {"TrillSpeed"       ,  __SK_ControlChange_, __SK_TrillSpeed_        ,  SK_DBL},
                                             
 {"Strumming"        ,  __SK_ControlChange_, __SK_Strumming_         ,  127   }, 
 {"NotStrumming"     ,  __SK_ControlChange_, __SK_Strumming_         ,  0     },
                                             
 {"PlayerSkill"      ,  __SK_ControlChange_, __SK_PlayerSkill_       ,  SK_DBL}, 

 {"Chord"            ,  __SK_Chord_	       ,                   SK_DBL,  SK_STR}, 
 {"ChordOff"         ,  __SK_ChordOff_     ,                   SK_DBL,    NOPE}, 

 {"ShakerInst"       ,  __SK_ControlChange_, __SK_ShakerInst_        ,  SK_DBL},
 {"Maraca"           ,  __SK_ControlChange_, __SK_ShakerInst_	       ,   0    },
 {"Sekere"           ,  __SK_ControlChange_, __SK_ShakerInst_	       ,   1    },
 {"Cabasa"           ,  __SK_ControlChange_, __SK_ShakerInst_        ,   2    },
 {"Bamboo"           ,  __SK_ControlChange_, __SK_ShakerInst_        ,   3    },
 {"Waterdrp"         ,  __SK_ControlChange_, __SK_ShakerInst_        ,   4    },
 {"Tambourn"         ,  __SK_ControlChange_, __SK_ShakerInst_        ,   5    },
 {"Sleighbl"         ,  __SK_ControlChange_, __SK_ShakerInst_        ,   6    },
 {"Guiro"            ,  __SK_ControlChange_, __SK_ShakerInst_        ,   7    },	

 {"OpenFile"         ,                  256,                   SK_STR,    NOPE},
 {"SetPath"          ,                  257,                   SK_STR,    NOPE},

 {"FilePath"         ,  __SK_SINGER_FilePath_      ,           SK_STR,    NOPE},
 {"Frequency"        ,  __SK_SINGER_Frequency_     ,           SK_STR,    NOPE},
 {"NoteName"         ,  __SK_SINGER_NoteName_      ,           SK_STR,    NOPE},
 {"VocalShape"       ,  __SK_SINGER_Shape_         ,           SK_STR,    NOPE},
 {"Glottis"          ,  __SK_SINGER_Glot_          ,           SK_STR,    NOPE},
 {"VoicedUnVoiced"   ,  __SK_SINGER_VoicedUnVoiced_,           SK_DBL,  SK_STR},
 {"Synthesize"       ,  __SK_SINGER_Synthesize_    ,           SK_STR,    NOPE},
 {"Silence"          ,  __SK_SINGER_Silence_       ,           SK_STR,    NOPE},
 {"RndVibAmt"        ,  __SK_SINGER_RndVibAmt_     ,           SK_STR,    NOPE},
 {"VibratoAmt"       ,  __SK_ControlChange_        ,__SK_SINGER_VibratoAmt_,SK_DBL},
 {"VibFreq"          ,  __SK_ControlChange_        ,__SK_SINGER_VibFreq_   ,SK_DBL}
};
// end of skini.tbl
