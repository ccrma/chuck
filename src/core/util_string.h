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


// itoa
std::string itoa( t_CKINT val );
// ftoa
std::string ftoa( t_CKFLOAT val, t_CKUINT precision );
// tolower
std::string tolower( const std::string & val );
// toupper
std::string toupper( const std::string & val );
// trim
std::string trim( const std::string & val );
// ltrim
std::string ltrim( const std::string & val );
// rtrim
std::string rtrim( const std::string & val );
// return capitalized copy of a string
std::string capitalize( const std::string & s );

// argument extraction
t_CKBOOL extract_args( const std::string & token,
                       std::string & filename,
                       std::vector<std::string> & args );

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




#endif
