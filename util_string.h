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
// name: util_string.h
// desc: string functions.
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
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

// argument extraction
t_CKBOOL extract_args( const std::string & token, 
                       std::string & filename, std::vector<std::string> & args );

// currently just expands ~ to HOME and ~user to user's home directory
std::string expand_filepath( std::string & fp );

std::string extract_filepath_dir(std::string &filepath);

// convert \ to / (on Windows)
std::string normalize_directory_separator(std::string &filepath);

//-----------------------------------------------------------------------------
// name: parse_path_list()
// desc: split "x:y:z"-style path list into {"x","y","z"}
//-----------------------------------------------------------------------------
void parse_path_list( std::string & str, std::list<std::string> & lst );


#endif
