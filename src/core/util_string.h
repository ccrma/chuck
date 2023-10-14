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
// name: util_string.h
// desc: string functions utility
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#ifndef __UTIL_STRING_H__
#define __UTIL_STRING_H__

#include "chuck_def.h"
#include <string>
#include <vector>
#include <list>


// int to ascii
std::string ck_itoa( t_CKINT val );
// float to ascii
std::string ck_ftoa( t_CKFLOAT val, t_CKUINT precision );
// ascii to unsigned long
unsigned long ck_atoul( const std::string & s, int base = 10 );
// ascii to signed long
long ck_atol( const std::string & s, int base = 10 );

// pointer to ascii
std::string ptoa( void * val );
// lower-case string
std::string tolower( const std::string & val );
// upper-case string
std::string toupper( const std::string & val );
// trim a string
std::string trim( const std::string & val );
// left trim
std::string ltrim( const std::string & val );
// right trim
std::string rtrim( const std::string & val );

// return capitalized copy of string
std::string capitalize( const std::string & s );

// return capiitalized copy of string with trailing punc, adding . as needed
std::string capitalize_and_periodize( const std::string & s );

// replace tabs
std::string replace_tabs( const std::string & s,
                          char replaceEachTabWithThis );

// snippet a string around an offset; useful for displaying
// long line of test with caret ^ offset; returns result and
// potentially modifies targetPositionMutable argument
std::string snippet( const std::string & str, t_CKINT desiredLength,
                     t_CKINT desiredLeftPadding, t_CKINT & targetPositionMutable,
                     t_CKINT * optionalLeftTrimmed = NULL,
                     t_CKINT * optionalRightTrimmed = NULL );

// argument extraction
t_CKBOOL extract_args( const std::string & token,
                       std::string & filename,
                       std::vector<std::string> & args );
// argument extraction (instead of vector, get args as one string)
t_CKBOOL extract_args( const std::string & token,
                       std::string & filename,
                       std::string & args );

// take existing path, and attempt to dir up
std::string dir_go_up( const std::string & dir, t_CKINT numUp );

// get full path to file
std::string get_full_path( const std::string & fp );

// perform filepath expansion (e.g., with ~ on unix systems and some windows)
std::string expand_filepath( const std::string & fp, t_CKBOOL ensurePathExists = FALSE );

// get directory portion of a filepath (minus the file itself)
std::string extract_filepath_dir( std::string & filepath );

// convert \ to / (on Windows)
std::string normalize_directory_separator(const std::string &filepath);

// check if path is absolute on the underlying platform
t_CKBOOL is_absolute_path( const std::string & path );

// split "x:y:z"-style path list into {"x","y","z"}
void parse_path_list( std::string & str, std::list<std::string> & lst );

// generate auto filename (usually with date-time) with file prefix and extension
std::string autoFilename( const std::string & prefix, const std::string & extension );

// test whether a filename ends in a particular extension
t_CKBOOL extension_matches( const std::string & filename,
                            const std::string & extension,
                            t_CKBOOL ignoreCase = TRUE );

// return filename without the extension
std::string extension_removed( const std::string & filename,
                               const std::string & extension,
                               t_CKBOOL ignoreCase = TRUE );

// test if a directory name is OK to recurse, e.g., in search for chugins
// the reason we have this function is for macOS bundles that are actually
// directories, e.g., if Faust.chug is directory and the actual chugin
// is located in side in /Contents/MacOS -- in which case we would not
// search for more chugins within the Faust.chug directory;
// HOWEVER, something like .chug (a directory name starting with '.') is
// okay, as that could be hidden directory
t_CKBOOL subdir_ok2recurse( const std::string & dirName,
                            const std::string & extension,
                            t_CKBOOL ignoreCase = TRUE );

// get formatted timestamp of current system time; no new line
std::string timestamp_formatted(); // e.g., "Sat Jun 24 04:18:42 2023"

// tokenize a string into a vector of strings, by delimiters
void tokenize( const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters );


//-----------------------------------------------------------------------------
// name: class TC | 1.5.0.5 (ge) added
// desc: terminal code for enhanced output, like color text
//
// NOTE: by default TC's globalDisableOverride is ENGAGED, meaning for TC to
//       actually process terminal codes, it must be explicitly enabled;
//       not all output supports terminal escape sequences and some may even
//       print the escape sequence
//
//       when chuck is integrated, the recommended way to enable this:
//           `chuck->toggleGlobalColorTextoutput( true );`
//       where `chuck` is a ChucK instance; this will also set the
//       corresponding setting value inside the ChucK instance
//
//       calling TC::globalDisableOverride( false ) is not recommended!
//
// (still working on making TC more flexible!) more resources:
// https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
// https://cplusplus.com/forum/general/18200/
// FYI nifty tool: https://michurin.github.io/xterm256-color-picker/
//-----------------------------------------------------------------------------
class TC
{
public:
    // reset all attributes
    static std::string reset() { return "\033[0m"; }
    // enable bold
    static std::string bold() { return std::string("\033[1m"); }
    // enable TC
    static void on();
    // disable TC (bypass)
    static void off();

public:
    // a more global switch, if engaged, will override on() for all TC:: methods
    // tranform text methods will bypass and return the input without modification;
    // TC::set*() will return empty strings;
    // if this switch is NOT engaged, will defer to on() and off()
    // does not affect sequences constructed manually outside of TC
    // FYI this option is typically used on systems with no color terminal capabilities
    // also see command line flags --color --no-color
    static void globalDisableOverride( t_CKBOOL setTrueToEngage );

public:
    // transform text; make bold
    static std::string bold( const std::string & text );
    // transform text; make color, with optional bold attribute
    static std::string green( const std::string & text, t_CKBOOL bold = FALSE );
    static std::string orange( const std::string & text, t_CKBOOL bold = FALSE );
    static std::string blue( const std::string & text, t_CKBOOL bold = FALSE );
    static std::string red( const std::string & text, t_CKBOOL bold = FALSE );
    static std::string yellow( const std::string & text, t_CKBOOL bold = FALSE );
    static std::string magenta( const std::string & text, t_CKBOOL bold = FALSE );

public:
    // terminal codes; use with TC::set()
    enum TerminalCode {
        RESET_ALL   = 0,
        BOLD        = 1,
        DIM         = 2,
        BLINK       = 5,
        FG_BLACK    = 30,
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_MAGENTA  = 35,
        FG_CYAN     = 36,
        FG_WHITE    = 37,
        FG_DEFAULT  = 39,

        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_YELLOW   = 43,
        BG_BLUE     = 44,
        BG_MAGENTA  = 45,
        BG_CYAN     = 46,
        BG_WHITE    = 47,
        BG_DEFAULT  = 49,

        FG_DARK_GRAY = 90,
        FG_LIGHT_RED = 91,
        FG_LIGHT_GREEN = 92,
        FG_LIGHT_YELLOW = 93,
        FG_LIGHT_BLUE = 94,
        FG_LIGHT_MAGENTA = 95,
        FG_LIGHT_CYAN = 96,
    };

    // set custom color
    static std::string color( TerminalCode code, const std::string & text, t_CKBOOL bold = FALSE );
    // set a terminal code
    static std::string set( TerminalCode code );
    // set using an integer
    static std::string seti( t_CKUINT code );
    // set foreground default color
    static std::string set_fg_default();
    // set background default color
    static std::string set_bg_default();
    // set specific colors
    static std::string set_green( t_CKBOOL bold = FALSE );
    static std::string set_orange( t_CKBOOL bold = FALSE );
    static std::string set_blue( t_CKBOOL bold = FALSE );


protected:
    // whether TC is enabled
    static t_CKBOOL isEnabled;
    // global bypass
    static t_CKBOOL globalBypass;
};


#endif
