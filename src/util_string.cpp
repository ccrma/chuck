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
// name: util_string.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#include "util_string.h"
#include <stdio.h>
using namespace std;




//-----------------------------------------------------------------------------
// name: itoa()
// desc: ...
//-----------------------------------------------------------------------------
string itoa( t_CKINT val )
{
    char buffer[128];
    sprintf( buffer, "%li", val );
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
    sprintf( str, "%%.%lif", precision );
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
    
    // copy and trim
    string s = trim( token );
    
    // detect
    t_CKBOOL scan = FALSE;
    t_CKBOOL ret = TRUE;
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
        
        // loop through
        for( i = 0; i < s.length(); i++ )
        {
            // zero
            mask[i] = 0;
            
            // escape (tom and ge fixed this, which used to look for \\ then : and randomly incremented i)
            if( s[i] == ':' && (i+1) < s.length() && s[i+1] == '\\' )
            {
                mask[i] = 1;
            }
        }
    }
    
    // loop through
    for( i = 0; i < s.length(); i++ )
    {
        // look for :
        if( s[i] == ':' && ( !mask || !mask[i] ) )
        {
            // sanity
            if( i == 0 )
            {
                ret = FALSE;
                goto done;
            }
            
            // copy
            if( filename == "" )
                filename = s.substr( prev_pos, i - prev_pos );
            else
                args.push_back( s.substr( prev_pos, i - prev_pos ) );
            
            // update
            prev_pos = i + 1;
        }
    }
    
    // get the remainder, if any
    if( prev_pos < s.length() )
    {
        // copy
        if( filename == "" )
            filename = s.substr( prev_pos, s.length() - prev_pos );
        else
            args.push_back( s.substr( prev_pos, s.length() - prev_pos ) );
    }

done:
    // reclaim
    SAFE_DELETE_ARRAY( mask );

    return ret;
}


/* from http://developer.apple.com/library/mac/#qa/qa1549/_index.html */

#ifndef __PLATFORM_WIN32__

#include <glob.h>

char* CreatePathByExpandingTildePath(const char* path)
{
    glob_t globbuf;
    char **v;
    char *expandedPath = NULL, *result = NULL;
    
    assert(path != NULL);
    
    if (glob(path, GLOB_TILDE, NULL, &globbuf) == 0) //success
    {
        v = globbuf.gl_pathv; //list of matched pathnames
        expandedPath = v[0]; //number of matched pathnames, gl_pathc == 1
        
        result = (char*)calloc(1, strlen(expandedPath) + 1); //the extra char is for the null-termination
        if(result)
            strncpy(result, expandedPath, strlen(expandedPath) + 1); //copy the null-termination as well
        
        globfree(&globbuf);
    }
    
    return result;
}

#endif // __PLATFORM_WIN32__


std::string expand_filepath( std::string & fp )
{
#if defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)
    // no expansion in Windows systems
    return fp;
#else
    
    char * expanded_cstr = CreatePathByExpandingTildePath( fp.c_str() );
    
    if(expanded_cstr == NULL)
        return fp;
    
    std::string expanded_stdstr = expanded_cstr;
    
    free(expanded_cstr);
    
    return expanded_stdstr;
    
#endif
}


std::string extract_filepath_dir(std::string &filepath)
{
    char path_separator = '/';
    
//#ifdef __WINDOWS_DS__
//    path_separator = '\\';
//#else
//    path_separator = '/';
//#endif
    
    // if the last character is a slash, skip it
    int i = filepath.rfind(path_separator);
    
    if(i == std::string::npos)
        return std::string();
    
    return std::string(filepath, 0, i);
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



std::string normalize_directory_separator(std::string &filepath)
{
#ifdef __PLATFORM_WIN32__
    std::string new_filepath = filepath;
    int len = new_filepath.size();
    for(int i = 0; i < len; i++)
    {
        if(new_filepath[i] == '\\') new_filepath[i] = '/';
    }

    return new_filepath;
#else
    return filepath;
#endif // __PLATFORM_WIN32__
}
