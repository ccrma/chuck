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
// file: ulib_regex.cpp
// desc: regex library
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2013
//-----------------------------------------------------------------------------

#include "ulib_regex.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "chuck_type.h"

#ifdef WIN32
#include "regex/regex.h"
#else
#include <regex.h>
#endif

#define CK_REGEX_MAX_MATCHES (10)

CK_DLL_SFUN( regex_match );
CK_DLL_SFUN( regex_match2 );
CK_DLL_SFUN( regex_replace );

DLL_QUERY regex_query( Chuck_DL_Query * QUERY )
{
    // set name
    QUERY->setname( QUERY, "RegEx" );
    
    // begin class
    QUERY->begin_class( QUERY, "RegEx", "Object" );
    
    // add match
    QUERY->add_sfun( QUERY, regex_match, "int", "match" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "str");
    
    // add match2
    QUERY->add_sfun( QUERY, regex_match2, "int", "match" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "str");
    QUERY->add_arg( QUERY, "string[]", "matches");
    
    // add replace
    QUERY->add_sfun( QUERY, regex_replace, "string", "replace" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "str");
    QUERY->add_arg( QUERY, "string", "replacement");
    
    QUERY->end_class( QUERY );
    
    return TRUE;
    
error:
    
    return FALSE;
}


CK_DLL_SFUN( regex_match )
{
    Chuck_String * pattern = GET_NEXT_STRING(ARGS);
    Chuck_String * str = GET_NEXT_STRING(ARGS);
    
    regex_t regex;
    t_CKBOOL r_free = FALSE;
    
    int result = regcomp(&regex, pattern->str.c_str(), REG_EXTENDED | REG_NOSUB);
    if(result != 0)
        goto error;
    
    r_free = TRUE;
    
    result = regexec(&regex, str->str.c_str(), 0, NULL, 0);
    
    RETURN->v_int = (result == 0 ? 1 : 0);
    
    regfree(&regex);

    return;
    
error:
    {
        char buf[256];
        
        regerror(result, &regex, buf, 256);
        EM_error2( 0, "(RegEx.match): regex reported error: %s", buf);
    }
    
    if(r_free)
        regfree(&regex);
    
    RETURN->v_int = 0;
}


CK_DLL_SFUN( regex_match2 )
{
    Chuck_String * pattern = GET_NEXT_STRING(ARGS);
    Chuck_String * str = GET_NEXT_STRING(ARGS);
    Chuck_Array4 * matches = (Chuck_Array4 *) GET_NEXT_OBJECT(ARGS);
    
    regex_t regex;
    t_CKBOOL r_free = FALSE;
    size_t i;
    regmatch_t *matcharray = NULL;
    
    matches->clear();
    
    int result = regcomp(&regex, pattern->str.c_str(), REG_EXTENDED);
    if(result != 0)
        goto error;
    
    r_free = TRUE;
    
    matcharray = new regmatch_t[regex.re_nsub+1];
    
    result = regexec(&regex, str->str.c_str(), regex.re_nsub+1, matcharray, 0);
    
    RETURN->v_int = (result == 0 ? 1 : 0);
    
    regfree(&regex);
    
    if(result == 0)
    {
        for(i = 0; i < regex.re_nsub+1; i++)
        {
            Chuck_String * match = (Chuck_String *) instantiate_and_initialize_object(&t_string, NULL);
            
            if(matcharray[i].rm_so >= 0 && matcharray[i].rm_eo > 0)
                match->str = std::string(str->str, matcharray[i].rm_so,
                                         matcharray[i].rm_eo-matcharray[i].rm_so);
                                         
            matches->push_back((t_CKUINT) match);
        }
    }
    
    return;
    
error:
    {
        char errbuf[256];
        
        regerror(result, &regex, errbuf, 256);
        EM_error2( 0, "(RegEx.match): regex reported error: %s", errbuf);
    }
    
    if(r_free)
        regfree(&regex);
    
    RETURN->v_int = 0;
}


CK_DLL_SFUN( regex_replace )
{
    
}

