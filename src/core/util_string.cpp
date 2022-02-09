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
// name: util_string.cpp
// desc: string functions utility
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#include "util_string.h"
#include "chuck_errmsg.h"

#ifdef __PLATFORM_WIN32__
#include <Windows.h>
#endif // __PLATFORM_WIN32__

#ifdef __PLATFORM_LINUX__
#include <linux/limits.h>
#endif // __PLATFORM_LINUX__

#include <stdio.h>
using namespace std;



//-----------------------------------------------------------------------------
// name: itoa()
// desc: ...
//-----------------------------------------------------------------------------
string itoa( t_CKINT val )
{
    char buffer[128];
#ifdef _WIN64
    sprintf( buffer, "%lld", val );
#else
    sprintf( buffer, "%ld", val );
#endif
    return string(buffer);
}



//-----------------------------------------------------------------------------
// name: ftoa()
// desc: ...
//-----------------------------------------------------------------------------
string ftoa( t_CKFLOAT val, t_CKUINT precision )
{
    char str[32];
    char buffer[128];
    if( precision > 32 ) precision = 32;
    sprintf( str, "%%.%lif", (long)precision );
    sprintf( buffer, str, val );
    return string(buffer);
}




//-----------------------------------------------------------------------------
// name: tolower()
// desc: ...
//-----------------------------------------------------------------------------
string tolower( const string & str )
{
    string s = str;
    t_CKUINT len = s.length();

    // loop
    for( t_CKUINT i = 0; i < len; i++ )
        if( s[i] >= 'A' && s[i] <= 'Z' )
            s[i] += 32;

    return s;
}




//-----------------------------------------------------------------------------
// name: toupper()
// desc: ...
//-----------------------------------------------------------------------------
string toupper( const string & str )
{
    string s = str;
    t_CKUINT len = s.length();

    // loop
    for( t_CKUINT i = 0; i < len; i++ )
        if( s[i] >= 'a' && s[i] <= 'z' )
            s[i] -= 32;

    return s;
}




//-----------------------------------------------------------------------------
// name: trim()
// desc: ...
//-----------------------------------------------------------------------------
string trim( const string & val )
{
    // two ends
    t_CKINT start = 0;
    t_CKINT end = val.length() - 1;

    // left trim
    for( start = 0; start < end; start++ )
    {
        // non-white space
        if( val[start] != ' ' && val[start] != '\t' )
            break;
    }

    // if start > end, then all white space
    if( start > end ) return "";

    // right trim
    for( ; end >= start; end-- )
    {
        // non-white space
        if( val[end] != ' ' && val[end] != '\t' )
            break;
    }

    // cannot be
    assert( end >= start );

    // return
    return val.substr( start, end - start + 1 );
}



//-----------------------------------------------------------------------------
// name: ltrim()
// desc: ...
//-----------------------------------------------------------------------------
string ltrim( const string & val )
{
    // two ends
    t_CKINT start = 0;
    t_CKINT end = val.length() - 1;

    // left trim
    for( start = 0; start < end; start++ )
    {
        // non-white space
        if( val[start] != ' ' && val[start] != '\t' )
            break;
    }

    // if start > end, then all white space
    if( start > end ) return "";

    // return
    return val.substr( start, end - start + 1 );
}




//-----------------------------------------------------------------------------
// name: rtrim()
// desc: ...
//-----------------------------------------------------------------------------
string rtrim( const string & val )
{
    // two ends
    t_CKINT start = 0;
    t_CKINT end = val.length() - 1;

    // right trim
    for( ; end >= start; end-- )
    {
        // non-white space
        if( val[end] != ' ' && val[end] != '\t' )
            break;
    }

    // if end < start, then all white space
    if( end < start ) return "";

    // return
    return val.substr( start, end - start + 1 );
}




//-----------------------------------------------------------------------------
// name: extract_args()
// desc: extract argument from format filename:arg1:arg2:etc
//-----------------------------------------------------------------------------
t_CKBOOL extract_args( const string & token, 
                       string & filename, vector<string> & args )
{
    // clear vector
    args.clear();
    // clear filename
    filename = "";
    
    // last : found pos
    t_CKINT prev_pos = 0;
    // curr : pos
    t_CKINT i = 0;
    
    string tmp;
    
    // copy and trim
    string s = trim( token );
    
    // ignore second character as arg separator if its : on Windows
    t_CKBOOL ignoreSecond = FALSE;
#ifdef __PLATFORM_WIN32__
	ignoreSecond = TRUE;
#endif // __PLATFORM_WIN32__
    
    // detect
    t_CKBOOL scan = FALSE;
    t_CKBOOL ret = TRUE;
    t_CKBOOL ignoreNext = FALSE;
    char * mask = NULL;
    for( i = 0; i < s.length(); i++ )
        if( s[i] == '\\' )
        {
            scan = TRUE;
            break;
        }
    
    // mad...
    if( scan )
    {
        mask = new char[s.length()];
        // zero
        memset(mask, 0, s.length()*sizeof(char));
        
        // loop through
        for( i = 0; i < s.length(); i++ )
        {
            // escape (tom and ge fixed this, which used to look for \\ then : and randomly incremented i)
            // added '/' 1.3.1.1
            // if( s[i] == ':' && (i+1) < s.length() && ( s[i+1] == '\\' || s[i+1] == '/' ) )
            // {
            //     mask[i] = 1;
            //     break; // added 1.3.1.1
            // }
            
            // 1.3.2.0: spencer and ge fixed this, requiring \ to escape :
            if( s[i] == '\\' && (i+1) < s.length() )
            {
                // check next char
                if( s[i+1] == ':' || s[i+1] == '\\' )
                {
                    // add
                    mask[i] = 1;
                    // skip next character
                    i++;
                }
                else
                {
                    // error: recognized escape target
                    CK_FPRINTF_STDERR( "[chuck]: unrecognized escape sequence: \\%c", s[i+1] );
                    // return false
                    ret = FALSE;
                    // go clean
                    goto done;
                }
            }
        }
    }

    // loop through
    for( i = 0; i < s.length(); i++ )
    {
        // check mask
        if( mask && mask[i] )
        {
            //CK_FPRINTF_STDERR( "skipping %c\n", s[i] );
            // mark the next as false
            ignoreNext = TRUE;
            // skip this one
            continue;
        }

        // look for :
        if( !ignoreNext && s[i] == ':' && !(ignoreSecond && i == 1))
        {
            // sanity
            if( i == 0 )
            {
                ret = FALSE;
                goto done;
            }
            
            // copy
            if( filename == "" )
                filename = tmp;
            else
                args.push_back( tmp );
            
            tmp = "";
            
            // update
            prev_pos = i + 1;
        }
        else
        {
            tmp.append(1, s[i]);
        }
        
        // reset
        ignoreNext = FALSE;
    }
    
    // get the remainder, if any
    if( tmp.length() )
    {
        // copy
        if( filename == "" )
            filename = tmp;
        else
            args.push_back( tmp );
    }
    
    // testing code - spencer 1.3.2.0
//    CK_FPRINTF_STDERR( "INPUT: %s\n", token.c_str() );
//    CK_FPRINTF_STDERR( "FILENAME: %s\n", filename.c_str() );
//    for(i = 0; i < args.size(); i++)
//        CK_FPRINTF_STDERR( "ARG: %s\n", args[i].c_str() );

done:
    // reclaim
    SAFE_DELETE_ARRAY( mask );

    return ret;
}




/* from http://developer.apple.com/library/mac/#qa/qa1549/_index.html */
#if !defined(__PLATFORM_WIN32__) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !defined(__CHIP_MODE__)

#include <wordexp.h>
//-----------------------------------------------------------------------------
// name: expandTildePath()
// desc: expand ~ path, does not care whether path is valid or not
//-----------------------------------------------------------------------------
std::string expandTildePath( const std::string & path )
{
    wordexp_t we;
    // default is the original path
    std::string result = path;

    // search for pathnames matching pattern
    if( wordexp(path.c_str(), &we, 0 ) == 0 ) // success
    {
        // check results number
        if( we.we_wordc > 0 )
        {
            // number of matched pathnames
            result = we.we_wordv[0];
        }
        // free up
        wordfree( &we );
    }

    // return result
    return result;
}

#include <glob.h>
//-----------------------------------------------------------------------------
// name: globTildePath()
// desc: expand ~ path, making sure the path actually exists
//       adapted from CreatePathByExpandingTildePath below
//-----------------------------------------------------------------------------
std::string globTildePath( const std::string & path )
{
    glob_t globbuf;
    // default is the original path
    std::string result = path;

    // search for pathnames matching pattern
    if( glob(path.c_str(), GLOB_TILDE, NULL, &globbuf) == 0 ) // success
    {
        // number of matched pathnames
        if( globbuf.gl_pathc > 0 )
        {
            // copy
            result = globbuf.gl_pathv[0];
        }
        // free up
        globfree(&globbuf);
    }

    // return result
    return result;
}

// original
//char* CreatePathByExpandingTildePath(const char* path)
//{
//    glob_t globbuf;
//    char **v;
//    char *expandedPath = NULL, *result = NULL;
//
//    assert(path != NULL);
//
//    if (glob(path, GLOB_TILDE, NULL, &globbuf) == 0) //success
//    {
//        v = globbuf.gl_pathv; //list of matched pathnames
//        expandedPath = v[0]; //number of matched pathnames, gl_pathc == 1
//
//        size_t toCopy = strlen(expandedPath) + 1; //the extra char is for the null-termination
//        result = (char*)calloc(1, toCopy);
//        if(result)
//        {
//            //copy the null-termination as well
//            memcpy(result, expandedPath, toCopy);
//            // was: strncpy(result, expandedPath,strlen(expandedPath) + 1);
//            // which was generating a warning on linux/gcc
//            // warning: ‘char* strncpy(char*, const char*, size_t)’ specified
//            // bound depends on the length of the source argument [-Wstringop-overflow=]
//        }
//        globfree(&globbuf);
//    }
//
//    return result;
//}
#endif // not __PLATFORM_WIN32__ && __EMSCRIPTEN__ && __ANDROID__ && __CHIP_MODE__




//-----------------------------------------------------------------------------
// name: get_full_path()
// desc: get full path to file
//-----------------------------------------------------------------------------
std::string get_full_path( const std::string & fp )
{
#ifndef __PLATFORM_WIN32__
    
    char buf[PATH_MAX];
    char * result = realpath(fp.c_str(), buf);
    
    // try with .ck extension
    if(result == NULL && !str_endsin(fp.c_str(), ".ck"))
        result = realpath((fp + ".ck").c_str(), buf);
    
    if(result == NULL)
        return fp;
    else
        return buf;
    
#else //
    
	char buf[MAX_PATH];
	DWORD result = GetFullPathName(fp.c_str(), MAX_PATH, buf, NULL);

    // try with .ck extension
    if(result == 0 && !str_endsin(fp.c_str(), ".ck"))
        result = GetFullPathName((fp + ".ck").c_str(), MAX_PATH, buf, NULL);

	if(result == 0)
		return fp;
	else
		return normalize_directory_separator(buf);

#endif // __PLATFORM_WIN32__
}




//-----------------------------------------------------------------------------
// name: expand_filepath()
// desc: if possible return expand path (e.g., from the unix ~)
//-----------------------------------------------------------------------------
std::string expand_filepath( std::string & fp, t_CKBOOL ensurePathExists )
{
#if defined(__PLATFORM_WIN32__) || defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(__CHIP_MODE__)
    // no expansion in Windows systems or Emscripten or Android or iOS
    return fp;
#else
    // expand ~ to full path
    if( ensurePathExists )
        return globTildePath( fp );
    else
        return expandTildePath( fp );
#endif
}




//-----------------------------------------------------------------------------
// name: extract_filepath_dir()
// desc: return the directory portion of a file path, excluding the filename
//-----------------------------------------------------------------------------
std::string extract_filepath_dir(std::string &filepath)
{
    char path_separator = '/';
    
//#ifdef __WINDOWS_DS__
//    path_separator = '\\';
//#else
//    path_separator = '/';
//#endif
    
    // if the last character is a slash, skip it
    t_CKINT i = filepath.rfind(path_separator);
    // if not separator found, return empty string
    if(i == std::string::npos)
        return "";
    // skip any/all extra trailing slashes
    while( i > 0 && filepath[i-1] == path_separator )
        i--;
    
    // change spencer 2014-7-17: include trailing slash
    return std::string(filepath, 0, i+1);
}




//-----------------------------------------------------------------------------
// file: dir_go_up()
// desc: return directory path 'numUp' levels up the chain
// added: ge 1.3.2.0
//-----------------------------------------------------------------------------
string dir_go_up( const string & dir, t_CKINT numUp )
{
    // separator
    char path_separator = '/';

    // sanity check
    if( numUp < 0 ) numUp = -numUp;

    // pos
    size_t pos = dir.length();

    // skip any trailing slashes
    while( pos > 0 && dir[pos-1] == path_separator )
        pos--;
    
    // loop
    while( numUp > 0 )
    {
        // find the last slash
        pos = dir.rfind( path_separator, pos-1 );
        // if no separator found, return empty string
        if (pos == string::npos)
            return "";
        // skip any extra slashes
        while( pos > 0 && dir[pos-1] == path_separator )
            pos--;
        if( pos == 0 )
            return "/";

        // TODO: what about windows?
        // TODO: what about windows network: with two backslashes? "\\\\"

        // decrement
        numUp--;
    }

    // otherwise
    // change spencer 2014-7-17: include trailing slash
    string str = string( dir, 0, pos );
    if( str[str.length()-1] != '/' ) str = str + "/";
    return str;
}




//-----------------------------------------------------------------------------
// name: parse_path_list()
// desc: split "x:y:z"-style path list into {"x","y","z"}
//-----------------------------------------------------------------------------
void parse_path_list( std::string & str, std::list<std::string> & lst )
{
#if defined(__PLATFORM_WIN32__)
    const char separator = ';';
#else
    const char separator = ':';
#endif
    std::string::size_type i = 0, last = 0;
    while( last < str.size() && 
          ( i = str.find( separator, last ) ) != std::string::npos )
    {
        lst.push_back( str.substr( last, i - last ) );
        last = i + 1;
    }
    
    lst.push_back( str.substr( last, str.size() - last ) );
}




//-----------------------------------------------------------------------------
// name: normalize_directory_separator()
// desc: unify directory separator to be consistent across platforms;
//       inside chuck, we are going with the single forward slash '/'
//       as the generic directory separator; other separators will
//       be converted to it
//-----------------------------------------------------------------------------
std::string normalize_directory_separator( const std::string & filepath )
{
#ifdef __PLATFORM_WIN32__
    std::string new_filepath = filepath;
    t_CKINT len = new_filepath.size();
    for(int i = 0; i < len; i++)
    {
        if( new_filepath[i] == '\\' ) new_filepath[i] = '/';
    }
    return new_filepath;
#else
    return filepath;
#endif // __PLATFORM_WIN32__
}




//-----------------------------------------------------------------------------
// name: str_endsin()
// desc: return true if the first string ends with the second
//-----------------------------------------------------------------------------
t_CKBOOL str_endsin( const char * str, const char * end )
{
    size_t len = strlen(str);
    size_t endlen = strlen(end);
    
    return strncmp(str+(len-endlen), end, endlen) == 0;
}




//-----------------------------------------------------------------------------
// name: is_absolute_path()
// desc: return true if path names an absolute path on the underlying platform
// added: 1.4.1.1 (ge)
//-----------------------------------------------------------------------------
t_CKBOOL is_absolute_path( const std::string & path )
{
#ifdef __PLATFORM_WIN32__
    // a little more involved in windows: [letter]:[/ or \]
    if( path.length() >= 3 && path[1] == ':' && ( path[2] == '\\' || path[2] == '/' ) )
    {
        // check drive letter (apparently windows limited to 26 drive letters)
        if( (path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z') )
            return true;
    }
    // if we got here, then not absolute path
    return false;
#else
    // does the path begin with '/'?
    return( path.length() != 0 && path[0] == '/' );
#endif
}
