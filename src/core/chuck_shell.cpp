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
// file: chuck_shell.cpp
// desc: chuck shell implementation
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "chuck_shell.h"
#include "chuck_otf.h"
#include "chuck_errmsg.h"
#include "util_network.h"
#include "util_string.h"

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#ifndef __PLATFORM_WIN32__
 #include <unistd.h>
 #include <sys/param.h>
 #include <sys/types.h>
 #include <dirent.h>
#endif

using namespace std;




//-----------------------------------------------------------------------------
// name: tokenize_string
// desc: divides a string into space separated tokens, respecting single and 
//      double quotes
//-----------------------------------------------------------------------------
void tokenize_string( string str, vector< string > & tokens)
{
    t_CKINT space = 1;
    t_CKINT end_space = 0;
    t_CKINT squote = 0;
    t_CKINT dquote = 0;
    t_CKINT i = 0, j = 0, len = str.size();
    
    for( i = 0; i < len; i++ )
    {
        if( isspace( str[i] ) && space )
        {
            j++;
            continue;
        }
        
        if( isspace( str[i] ) && end_space )
        {
            tokens.push_back( string( str, j, i - j ) );
            j = i + 1;
            space = 1;
            end_space = 0;
            continue;
        }
        
        if( str[i] == '\'' )
        {
            if( dquote )
                continue;
            if( !squote )
            {
                str.erase( i, 1 );
                len--;
                i--;
                space = 0;
                end_space = 0;
                squote = 1;
                continue;
            }
            
            else if( str[i - 1] == '\\' )
            {
                str.erase( i - 1, 1 );
                len--;
                i--;
                continue;
            }
            
            else
            {
                str.erase( i, 1 );
                len--;
                i--;
                squote = 0;
                end_space = 1;
                space = 0;
                continue;
            }
        }
        
        if( str[i] == '"' ) //"
        {
            if( squote )
                continue;
            if( !dquote )
            {
                str.erase( i, 1 );
                i--;
                len--;
                space = 0;
                end_space = 0;
                dquote = 1;
                continue;
            }
            
            else if( str[i - 1] == '\\' )
            {
                str.erase( i - 1, 1 );
                len--;
                i--;
                continue;
            }
            
            else
            {
                str.erase( i, 1 );
                i--;
                len--;
                dquote = 0;
                end_space = 1;
                space = 0;
                continue;
            }
        }
        
        if( !squote && !dquote )
        {       
            end_space = 1;
            space = 0;
        }
    }
    
    if( i > j && end_space )
    {
        tokens.push_back( string( str, j, i - j ) );
    }
}

//-----------------------------------------------------------------------------
// name: win32_tmpnam()
// desc: replacement for broken tmpnam() on Windows Vista + 7
// file_path should be at least MAX_PATH characters. 
//-----------------------------------------------------------------------------
#ifdef __PLATFORM_WIN32__

#include <windows.h>

int win32_tmpnam(char *file_path)
{
    char tmp_path[MAX_PATH];
    
    if(GetTempPath(256, tmp_path) == 0)
        return 0;
    
    if(GetTempFileName(tmp_path, "cksh", 0, file_path) == 0)
        return 0;

    return 1;
}

#endif




//-----------------------------------------------------------------------------
// name: Chuck_Shell()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Shell::Chuck_Shell()
{
    ui = NULL;
    process_vm = NULL;
    current_vm = NULL;
    initialized = FALSE;
    stop = FALSE;
    code_entry_active = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Shell()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Shell::~Chuck_Shell()
{
    vector<string>::size_type i, len = allocated_commands.size();    

    //iterate through commands, delete the associated heap data
    for( i = 0; i != len; i++ )
        SAFE_DELETE( allocated_commands[i] );

    // delete ui
    SAFE_DELETE( ui );

    // flag
    initialized = FALSE;
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell::init( Chuck_VM * vm, Chuck_Shell_UI * ui )
{
    // log
    EM_log( CK_LOG_SYSTEM, "initializing chuck shell..." );
    
    // check
    if( initialized == TRUE )
    {
        EM_log( CK_LOG_WARNING, "chuck shell already initialized" );
        return TRUE;
    }
    
    // ui
    if( ui == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck](via shell): NULL ui passed to Chuck_Shell::init..." );
        return FALSE;
    }
    
    this->ui = ui;
    
    process_vm = vm;

    Chuck_Shell_Network_VM * cspv = new Chuck_Shell_Network_VM();
    if( !cspv->init( "localhost", 8888 ) )
    {
        CK_FPRINTF_STDERR( "[chuck](via shell): error initializing process VM..." );
        SAFE_DELETE( cspv );
        return FALSE;
    }
    
    current_vm = cspv;
    vms.push_back( current_vm->copy() );

    process_vm = vm;
    
    code_entry_active = FALSE;
    code = "";    
    
    // init default variables
    variables["COMMAND_PROMPT"] = "chuck %> ";
    variables["DEFAULT_CONTEXT"] = "shell_global";

    // initialize prompt
    prompt = variables["COMMAND_PROMPT"];   
    
    Command * temp;

    // initialize the string -> Chuck_Shell_Command map
    temp = new Command_VM();
    temp->init( this );
    commands["vm"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_VMList();
    temp->init( this );
    commands["vms"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Add();
    temp->init( this );
    commands["+"] = temp;
    commands["add"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Remove();
    temp->init( this );
    commands["-"] = temp;
    commands["remove"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Status();
    temp->init( this );
    commands["status"] = temp;
    commands["^"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Removeall();
    temp->init( this );
    commands["removeall"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Removelast();
    temp->init( this );
    commands["--"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Replace();
    temp->init( this );
    commands["replace"] = temp;
    commands["="] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Close();
    temp->init( this );
    commands["close"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Kill();
    temp->init( this );
    commands["kill"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Exit();
    temp->init( this );
    commands["exit"] = temp;
    commands["quit"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Ls();
    temp->init( this );
    commands["ls"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Pwd();
    temp->init( this );
    commands["pwd"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Cd();
    temp->init( this );
    commands["cd"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Alias();
    temp->init( this );
    commands["alias"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Unalias();
    temp->init( this );
    commands["unalias"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Source();
    temp->init( this );
    commands["source"] = temp;
    commands["."] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_Code();
    temp->init( this );
    commands["code"] = temp;
    allocated_commands.push_back( temp );
    
    // flag
    initialized = TRUE;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: run()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::run()
{
    // log
    EM_log( CK_LOG_SYSTEM, "running chuck shell..." );

    // make sure
    if(initialized == FALSE)
    {
        CK_FPRINTF_STDERR( "[chuck](via shell): shell not initialized...\n" );
        return;
    }

    string command;
    string result;

    ui->next_result( "welcome to chuck shell!\n" );
    // ui->next_result( "(type \"help\" for more information)\n" );
    
    // loop
    for( ; !stop; )
    {
        // get command
        if( ui->next_command( prompt, command ) == TRUE )
        {
            // result.clear();
            result = "";
            
            //printf( "chuck_shell::run\n" );
            
            // execute the command
            execute( command, result );

            // command.clear();
            command = "";

            // pass the result to the shell ui
            ui->next_result( result );
        }
        else
            // done
            break;
    }
    
    // log
    EM_log( CK_LOG_SYSTEM, "exiting chuck shell..." );
}




//-----------------------------------------------------------------------------
// name: execute
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell::execute( string & in, string & out )
{
    if( !initialized )
    {
        return FALSE;
    }
    
    // vector of string tokens
    vector< string > vec;
        
    // clear the response
    out = "";

    // first find out if this is code to run
    if( !code_entry_active && in[in.find_first_not_of( " \t\v\n" )] == '{' )
        start_code();
    
    if( code_entry_active )
    {
        continue_code( in );
    
        if( code_entry_active == FALSE )
        {
            //strip opening and closing braces
            string::size_type k = code.find( "{" );
            string head = string( code, 0, k );
            code = string( code, k + 1, code.size() - k - 1 );
            
            k = code.rfind( "}" );
            string tail = string( code, k + 1, code.size() - k - 1 );
            code = string( code, 0, k );
            
            do_code( code, out );
            
            tokenize_string( tail, vec );
            
            if( vec.size() > 0 )
            {
                string temp = "code save " + vec[0];
                execute( temp, out );
            }
        }
        
        return TRUE;
    }
    
    // divide the string into white space separated substrings
    tokenize_string( in, vec );
    
    // if no tokens
    if( vec.size() == 0) 
        return TRUE;
    
    // first check if the first token matches an alias
    while( aliases.find( vec[0] ) != aliases.end() )
    // use a while loop to handle nested aliases
    {
        vector< string > vec2;
        tokenize_string( aliases[vec[0]], vec2 );
        vec.erase( vec.begin() );
        vec2.insert( vec2.end(), vec.begin(), vec.end() );
        vec = vec2;
    }
    
    // locate the command
    if( commands.find( vec[0] ) == commands.end() )
    // command doesn't exist!
    {
        out += vec[0] + ": command not found\n";
        return FALSE;
    }
    
    else
    // execute the command
    {
        Command * command = commands[vec[0]];
        vec.erase( vec.begin() );
        if( command->execute( vec, out ) == 0 )
            return TRUE;
        
        else
            return FALSE;
    }
        
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: start_code()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::start_code()
{
    code = "";
    code_entry_active = TRUE;
    scope = 0;
}




//-----------------------------------------------------------------------------
// name: continue_code()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::continue_code( string & in )
{
    if( in.find( "{" ) != string::npos )
    // increase the scope one level and change the prompt
    {
        char buf[16];
        scope++;
#ifndef __PLATFORM_WIN32__
        snprintf( buf, 16, "code %2d> ", (int)scope );
#else
        sprintf( buf, "code %2d> ", scope);
#endif
        prompt = buf;
    }
    
    if( in.find( "}" ) != string::npos )
    // decrease the scope one level and change the prompt
    {
        char buf[16];
        scope--;
#ifndef __PLATFORM_WIN32__
        snprintf( buf, 16, "code %2d> ", (int)scope );
#else
        sprintf( buf, "code %2d> ", scope);
#endif
        prompt = buf;
    }
    
    // collect this line as ChucK code
    code += in + "\n";

    if( scope == 0 )
    // the end of this code block -- lets execute it on the current_vm
        code_entry_active = FALSE;
}




//-----------------------------------------------------------------------------
// name: do_code()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::do_code( string & code, string & out, string command )
{
    // open a temporary file
#if defined(__PLATFORM_LINUX__) || defined(__MACOSX_CORE__)
    char tmp_dir[] = "/tmp";
    char * tmp_filepath = new char [32];
    strncpy( tmp_filepath, "/tmp/chuck_file.XXXXXX", 32 );
    int fd = mkstemp( tmp_filepath );
    if( fd == -1 )
    {
        out += string( "shell: error: unable to create tmpfile at " ) + tmp_dir + "\n";
        delete[] tmp_filepath;
        prompt = variables["COMMAND_PROMPT"];
        return;
    }
    
    FILE * tmp_file = fdopen( fd, "w+" );
    if( tmp_file == NULL )
    {
        out += string( "shell: error: unable to reopen tmpfile at " ) + tmp_filepath + "\n";
        delete[] tmp_filepath;
        prompt = variables["COMMAND_PROMPT"];
        return;
    }
#else
    char tmp_filepath1[MAX_PATH];
    win32_tmpnam(tmp_filepath1);

    if( tmp_filepath1 == NULL )
    {
        out += string( "shell: error: unable to generate tmpfile name\n" );
        prompt = variables["COMMAND_PROMPT"];
        return;
    }
    
    string tmp_filepath_stl = normalize_directory_separator(string(tmp_filepath1));
    const char *tmp_filepath = tmp_filepath_stl.c_str();
    FILE * tmp_file = fopen( tmp_filepath, "w" );
    if( tmp_file == NULL )
    {
        out += string( "shell: error: unable to open tmpfile '" ) + tmp_filepath + "'\n";
        prompt = variables["COMMAND_PROMPT"];
        return;
    }
#endif
        
    // write the code to the temp file
    fprintf( tmp_file, "%s", code.c_str() );
    fclose( tmp_file );

    string argv = string( command ) + tmp_filepath;

    this->execute( argv, out );
    // if( this->execute( argv, out ) )
    //    ;
    
    // delete the file
#ifndef __PLATFORM_WIN32__
    unlink( tmp_filepath );
#else
    DeleteFile( tmp_filepath );
#endif // __PLATFORM_WIN32__

#if defined(__PLATFORM_LINUX__)
    delete[] tmp_filepath;
#endif

    prompt = variables["COMMAND_PROMPT"];
}




//-----------------------------------------------------------------------------
// name: do_context()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::do_code_context( string & )
{
    
}




//-----------------------------------------------------------------------------
// name: close()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::close()
{
    stop = TRUE;
    // 
}




//-----------------------------------------------------------------------------
// name: kill()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Shell::exit()
{
    stop = TRUE;
    
    // REFACTOR 2017 TODO Ge: all_stop
/*    
    if( process_vm != NULL )
        all_stop(); // ge: 1.3.5.3
        // process_vm->stop();
*/
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::init( const string & hostname, t_CKINT port )
{
    this->hostname = hostname;
    this->port = port;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: copy()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Shell_VM * Chuck_Shell_Network_VM::copy()
{
    Chuck_Shell_Network_VM * net_vm = new Chuck_Shell_Network_VM();
    net_vm->init( hostname, port );
    return net_vm;
}




//-----------------------------------------------------------------------------
// name: add_shred()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::add_shred( const vector< string > & files, 
                                            string & out )
{
    t_CKINT i = 0;
    t_CKBOOL return_val;
    vector<string>::size_type j, str_len, vec_len = files.size() + 1;
    const char ** argv = new const char * [ vec_len ];
    
    /* prepare an argument vector to submit to otf_send_cmd */
    /* first, specify an add command */
    char * argv0 = new char [2];
    strncpy( argv0, "+", 2 );
    argv[0] = argv0;
    
    /* copy file paths into argv */
    for( j = 1; j < vec_len; j++ )
    {
        str_len = files[j - 1].size() + 1;
        char * argvj = new char [str_len];
        strncpy( argvj, files[j - 1].c_str(), str_len );
        argv[j] = argvj;
    }
    
    /* send the command */
    if( otf_send_cmd( vec_len, argv, i, hostname.c_str(), port ) )
        return_val = TRUE;
    else
    {
        out += "add: error: command failed\n";
        return_val = FALSE;
    }

    /* clean up heap data */
    for( j = 0; j < vec_len; j++ )
    {
        // TODO: verify this is alright
        char * arg = (char *)argv[j];
        delete[] arg;
    }
    delete[] argv;
    
    return return_val;
}




//-----------------------------------------------------------------------------
// name: remove_shred()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::remove_shred( const vector< string > & ids, 
                                               string & out )
{
    t_CKINT i = 0;
    t_CKBOOL return_val;
    vector<string>::size_type j, str_len, vec_len = ids.size() + 1;
    const char ** argv = new const char * [ vec_len ];
    
    /* prepare an argument vector to submit to otf_send_cmd */
    /* first, specify an add command */
    char * argv0 = new char [2];
    strncpy( argv0, "-", 2 );
    argv[0] = argv0;
    
    /* copy ids into argv */
    for( j = 1; j < vec_len; j++ )
    {
        str_len = ids[j - 1].size() + 1;
        char * argvj = new char [str_len];
        strncpy( argvj, ids[j - 1].c_str(), str_len );
        argv[j] = argvj;
    }
    
    /* send the command */
    if( otf_send_cmd( vec_len, argv, i, hostname.c_str(), port ) )
        return_val = TRUE;
    else
    {
        out += "remove: error: command failed\n";
        return_val = FALSE;
    }

    /* clean up heap data */
    for( j = 0; j < vec_len; j++ )
    {
        // TODO: verify this is ok
        char * arg = (char *)argv[j];
        delete[] arg;
    }
    delete[] argv;
    
    return return_val;
}




//-----------------------------------------------------------------------------
// name: remove_all()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::remove_all( string & out )
{
    t_CKBOOL return_val = TRUE;
    t_CKINT j = 0;
    const char * argv = "--removeall";
    if( !otf_send_cmd( 1, &argv, j, hostname.c_str(), port ) )
    {
        out += "removeall: error: command failed\n";
        return_val = FALSE;
    }

    return return_val;
}




//-----------------------------------------------------------------------------
// name: remove_last()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::remove_last( string & out )
{
    t_CKBOOL return_val = TRUE;
    t_CKINT j = 0;
    const char * argv = "--";
    if( !otf_send_cmd( 1, &argv, j, hostname.c_str(), port ) )
    {
        out += "removelast: error: command failed\n";
        return_val = FALSE;
    }
    
    return return_val;
}




//-----------------------------------------------------------------------------
// name: replace_shred()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::replace_shred( const vector< string > &vec,
                                                string & out )
{
    if( vec.size() < 2 )
    {
        out += "replace: error: insufficient arguments...\n";
        return FALSE;
    }
    
    t_CKINT i = 0;
    t_CKBOOL return_val;
    vector<string>::size_type j, str_len, vec_len = vec.size() + 1;
    const char ** argv = new const char * [ vec_len ];
    
    /* prepare an argument vector to submit to otf_send_cmd */
    /* first, specify an add command */
    argv[0] = "--replace";
    
    /* copy ids/files into argv */
    for( j = 1; j < vec_len; j++ )
    {
        str_len = vec[j - 1].size() + 1;
        char * argvj = new char [str_len];
        strncpy( argvj, vec[j - 1].c_str(), str_len );
        argv[j] = argvj;
    }
    
    /* send the command */
    if( otf_send_cmd( vec_len, argv, i, hostname.c_str(), port ) )
        return_val = TRUE;
    
    else
    {
        out += "replace: error: command failed\n";
        return_val = FALSE;
    }
    
    if( vec.size() % 2 != 0 )
    {
        out += "repalce: warning: ignoring excess arguments\n";
        return FALSE;
    }
    
    /* clean up heap data */
    for( j = 1; j < vec_len; j++ )
    {
        // TODO: verify this is ok
        char * arg = (char *)argv[j];
        delete[] arg;
    }
    delete[] argv;
    
    return return_val;
}




//-----------------------------------------------------------------------------
// name: status()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::status( string & out )
{
    const char * argv = "--status";
    t_CKBOOL return_val = FALSE;
    t_CKINT j = 0;
    
    if( otf_send_cmd( 1, &argv, j, hostname.c_str(), port ) )
        return_val = TRUE;
    else
    {
        return_val = FALSE;
        out += "status: error: command failed\n";
    }
    
    return return_val;
}




//-----------------------------------------------------------------------------
// name: kill()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_Network_VM::kill( string & out )
{
    const char * argv = "--kill";
    t_CKINT j = 0;
    t_CKBOOL return_val;
    
    if( otf_send_cmd( 1, &argv, j, hostname.c_str(), port ) )
        return_val = TRUE;
    
    else
    {
        return_val = FALSE;
        out += "kill: error: command failed";
    }
    
    return return_val;
}




//-----------------------------------------------------------------------------
// name: fullname()
// desc: returns a somewhat descriptive full name for this VM
//-----------------------------------------------------------------------------
string Chuck_Shell_Network_VM::fullname()
{
    char buf[16];
    sprintf( buf, ":%u", (int)port );
    
    return hostname + buf;
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell_UI::init()
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell::Command::init( Chuck_Shell * caller )
{
    this->caller = caller;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command::usage()
{
    return "no usage specified";
}




//-----------------------------------------------------------------------------
// name: long_usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command::long_usage()
{
    return "no usage specified";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Add::execute( vector< string > & argv,
                                           string & out )
{
    t_CKINT result = 0;
    
    if( caller->current_vm == NULL)
    {
        out += "add: error: not attached to a VM\n";
        result = -1;
    }
    
    else if( argv.size() < 1 )
    {
        out += "usage: " + usage() + "\n";
        result = -1;
    }
    
    else
    {
        result = caller->current_vm->add_shred( argv, out );
    }

    return result;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Add::usage()
{
    return "add file ...";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Remove::execute( vector< string > & argv,
                                              string & out )
{
    t_CKINT result = 0;
    
    if( caller->current_vm == NULL)
    {
        out += "remove: error: not attached to a VM\n";
        result = -1;
    }
    
    else if( argv.size() < 1 )
    {
        out += "usage: " + usage() + "\n";
        result = -1;
    }
    
    else
        result = caller->current_vm->remove_shred( argv, out );
    
    return result;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Remove::usage()
{
    return "remove shred_number ...";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Removeall::execute( vector< string > & argv, 
                                                 string & out )
{
    t_CKINT result = 0;
    
    if( caller->current_vm == NULL)
    {
        out += "removeall: error: not attached to a VM\n";
        result = -1;
    }
    
    else if( argv.size() > 0 )
    {
        out += "usage " + usage() + "\n";
        result = -1;
    }
    
    else
        result = caller->current_vm->remove_all( out );

    return result;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Removeall::usage()
{
    return "removeall";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Removelast::execute( vector< string > & argv,
                                                  string & out )
{
    t_CKINT result = -1;
    
    if( caller->current_vm == NULL)
        out += "removelast: error: not attached to a VM\n";
    else
        result = caller->current_vm->remove_last( out );
    
    if( argv.size() > 0 )
        out += "removelast: warning: ignoring excess arguments...\n";
    
    return result;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Removelast::usage()
{
    return "removelast";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Replace::execute( vector< string > & argv,
                                               string & out )
{
    if( caller->current_vm == NULL)
    {
        out += "replace: error: not attached to a VM\n";
        return -1;
    }
    
    caller->current_vm->replace_shred( argv, out );
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Replace::usage()
{
    return "replace shred_id file ...";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Status::execute( vector< string > & argv,
                                              string & out )
{
    t_CKINT result = 0;
    
    if( caller->current_vm == NULL)
    {
        out += "status: error: not attached to a VM\n";
        result = -1;
    }
    
    else if( argv.size() > 0 )
    {
        out += "usage: " + usage() + "\n";
        result = -1;
    }
    
    else
        result = caller->current_vm->status( out );
    
    return result;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Status::usage()
{
    return "status";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Kill::execute( vector< string > & argv,
                                            string & out )
{
    caller->current_vm->kill( out );
    if( argv.size() > 0 )
        out += "kill: warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Close::execute( vector< string > & argv,
                                             string & out )
{
    caller->close();
    
    out += "closing chuck shell...  bye!\n";
    
    // REFACTOR-2017: removed: this needs to be done outside
    // if( g_shell != NULL )
    //     out += "(note: in-process VM still running, hit ctrl-c to exit)\n";
    
    if( argv.size() > 0 )
        out += "close: warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Exit::execute( vector< string > & argv,
                                            string & out )
{
    caller->exit();
    if( argv.size() > 0 )
        out += "exit: warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Ls::execute( vector< string > & argv,
                                          string & out )
{
#ifndef __PLATFORM_WIN32__
    
    if( argv.size() == 0 )
    {
        DIR * dir_handle = opendir( "." );
        if( dir_handle == NULL )
        {
            out += "ls: error: unable to open directory .\n";
            return -1;
        }
        
        dirent * dir_entity = readdir( dir_handle );
        
        while( dir_entity != NULL )
        {
            out += string( dir_entity->d_name ) + "\n";
            dir_entity = readdir( dir_handle );
        }
        
        closedir( dir_handle );
        return 0;
    }
    
    int i, len = argv.size();
    t_CKBOOL print_parent_name = len > 1 ? TRUE : FALSE;
    
    for( i = 0; i < len; i++ )
    {
        DIR * dir_handle = opendir( argv[i].c_str() );
        if( dir_handle == NULL )
        {
            out += "ls: error: unable to open directory " + argv[i] + "\n";
            continue;
        }
        
        dirent * dir_entity = readdir( dir_handle );
        
        if( print_parent_name )
            out += argv[i] + ":\n";
        
        while( dir_entity != NULL )
        {
            out += string( dir_entity->d_name ) + "\n";
            dir_entity = readdir( dir_handle );
        }
        
        if( print_parent_name )
            out += "\n";
        
        closedir( dir_handle );
    }
    
#else
    
    if( argv.size() == 0 )
    {
        DWORD i, k = 64;
        LPTSTR cwd = new char [k];
        WIN32_FIND_DATA find_data;

        i = GetCurrentDirectory( k - 2, cwd );
        
        if( i >= k )
        {
            SAFE_DELETE_ARRAY( cwd );
            cwd = new char [i + 2];
            GetCurrentDirectory( i, cwd );
        }
    
        i = strlen( cwd );
        cwd[i] = '\\';
        cwd[i + 1] = '*';
        cwd[i + 2] = 0;

        HANDLE find_handle = FindFirstFile( cwd, &find_data );
        
        out += string( find_data.cFileName ) + "\n";

        while( FindNextFile( find_handle, &find_data ) )
            out += string( find_data.cFileName ) + "\n";
        
        FindClose( find_handle );
        return 0;
    }
    
    int i, len = argv.size();
    t_CKBOOL print_parent_name = len > 1 ? TRUE : FALSE;
    
    for( i = 0; i < len; i++ )
    {
        int j = argv[i].size() + 3;
        WIN32_FIND_DATA find_data;
        LPTSTR dir = new char [j];
        strncpy( dir, argv[i].c_str(), j );
        dir[j - 3] = '\\';
        dir[j - 2] = '*';
        dir[j - 1] = 0;

        if( print_parent_name )
            out += argv[i] + ":\n";
        
        HANDLE find_handle = FindFirstFile( dir, &find_data );
        
        out += string( find_data.cFileName ) + "\n";

        while( FindNextFile( find_handle, &find_data ) )
            out += string( find_data.cFileName ) + "\n";
        
        if( print_parent_name )
            out += "\n";

        FindClose( find_handle );
    }
    


#endif // __PLATFORM_WIN32__
    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Cd::execute( vector< string > & argv,
                                          string & out )
{
#ifndef __PLATFORM_WIN32__
    if( argv.size() < 1 )
    {
        if( chdir( getenv( "HOME" ) ) )
            out += "cd: error: command failed\n";
    }
    
    else
    {
        if( chdir( argv[0].c_str() ) )
            out += "cd: error: command failed\n";
    }
    
#else
    if( argv.size() < 1 )
        out += "usage: " + usage() + "\n";

    else
    {
        if( !SetCurrentDirectory( argv[0].c_str() ) )
            out += "cd: error: command failed\n";
    }

#endif //__PLATFORM_WIN32__
    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Pwd::execute( vector< string > & argv,
                                           string & out )
{
#ifndef __PLATFORM_WIN32__
    char * cwd = getcwd( NULL, 0 );
    out += string( cwd ) + "\n";
    free( cwd );
#else
    DWORD i, k = 256;
    LPTSTR cwd = new char [k];
    i = GetCurrentDirectory( k, cwd );
    
    if( i >= k )
    {
        SAFE_DELETE_ARRAY( cwd );
        cwd = new char [i];
        GetCurrentDirectory( i, cwd );
    }
    
    out += string( cwd ) + "\n";

    SAFE_DELETE_ARRAY( cwd );
#endif
    
    if( argv.size() > 0 )
        out += "pwd: warning: ignoring excess arguments...\n";
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Alias::execute( vector< string > & argv,
                                             string & out )
{
    vector<string>::size_type i, len = argv.size();
    string::size_type j;
    string a, b;
    
    for( i = 0; i < len; i++ )
    {
        j = argv[i].find( "=" );
        
        // no alias assignment specified; just print the alias value if exists
        if( j == string::npos )
        {
            // see if the alias exists in the map
            if( caller->aliases.find( argv[i] ) == caller->aliases.end() )
                out += "alias: error: alias " + argv[i] + " not found\n";
            else
                out += "alias " + argv[i] + "='" + 
                       caller->aliases[argv[i]] + "'\n";
        }
        // create the alias
        else
        {
            a = string( argv[i], 0, j );
            b = string( argv[i], j + 1, string::npos );
            caller->aliases[a] = b;
        }
    }
    
    if( len == 0 )
    // no arguments specified; print the entire alias map
    {   
        map< string, string>::iterator i = caller->aliases.begin(), 
                                       end = caller->aliases.end();
        for( ; i != end; i++ )
            out += "alias " + i->first + "='" + i->second + "'\n";
    }
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Unalias::execute( vector< string > & argv,
                                               string & out )
{
    vector<string>::size_type i, len = argv.size();
    
    for( i = 0; i < len; i++ )
    {
        if( caller->aliases.find( argv[i] ) == caller->aliases.end() )
            out += "error: alias " + argv[i] + " not found\n";
        
        else
            caller->aliases.erase( argv[i] );
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Source::execute( vector< string > & argv,
                                              string & out )
{
    vector<string>::size_type i, len = argv.size();
    char line_buf[255];
    string line, temp;

    for( i = 0; i < len; i++ )
    {
        FILE * source_file = fopen( argv[i].c_str(), "r" );
        
        if( source_file == NULL )
            out += "error: unable to open file " + argv[i];
        
        else
        {
            while( fgets( line_buf, 255, source_file ) != NULL )
            {
                line = string( line_buf );
                caller->execute( line, temp );
                out += temp;
            }
        }
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Help::execute( vector< string > & argv,
                                            string & out )
{
    out += "";
    return 0;
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell::Command_VM::init( Chuck_Shell * caller )
{
    Command * temp;
    
    Command::init( caller );
    
    temp = new Command_VMAttach();
    temp->init( caller );
    commands["attach"] = temp;
    commands["@"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_VMAdd();
    temp->init( caller );
    commands["add"] = temp;
    commands["+"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_VMRemove();
    temp->init( caller );
    commands["remove"] = temp;
    commands["-"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_VMList();
    temp->init( caller );
    commands["list"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_VMSwap();
    temp->init( caller );
    commands["swap"] = temp;
    commands["<"] = temp;
    commands["<-"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_VMAttachAdd();
    temp->init( caller );
    commands["@+"] = temp;  
    allocated_commands.push_back( temp );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ~Command_VM()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Shell::Command_VM::~Command_VM()
{
    vector<string>::size_type i, len = allocated_commands.size();
     
    //iterate through commands, delete the associated heap data
    for( i = 0; i != len; i++ )
        SAFE_DELETE( allocated_commands[i] );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VM::execute( vector< string > & argv,
                                          string & out )
{
    if( argv.size() < 1)
        out += "usage: " + usage() + "\n";

    else if( commands.find( argv[0] ) == commands.end() )
    // command doesn't exist
        out += "error: vm " + argv[0] + ": command not found\n";

    else
    // call the mapped command
    {
        Command * command = commands[argv[0]];
        argv.erase( argv.begin() );
        command->execute( argv, out );
    }
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_VM::usage()
{
    return "vm [command] [args] ...";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VMAttach::execute( vector < string > & argv,
                                                string & out )
{
    string hostname;
    t_CKINT port;
    t_CKINT result = 0;
    
    /* parse the hostname:port */
    if( argv.size() == 0 )
    /* no hostname:port specified, use default */
    {
        argv.push_back( "localhost:8888" );
        hostname = "localhost";
        port = 8888;
    }
    
    else
    {
        string::size_type i = argv[0].find( ":" );

        if( i == string::npos )
        // couldn't find a port number; use the default
        {
            hostname = argv[0];
            port = 8888;
            argv[0] += ":8888";
        }
        
        else
        {
            port = strtol( argv[0].c_str() + i + 1, NULL, 10 );
            if( port == 0 )
                out += string( "error: invalid port '" ) + 
                       string( argv[0].c_str() + i + 1 ) + "'\n";
            
            else
                hostname = string( argv[0], 0, i );
        }
    }
    
    if( port != 0 )
    {
        //string temp;
        
        Chuck_Shell_Network_VM * new_vm = new Chuck_Shell_Network_VM();
        new_vm->init( hostname, port );
        
        /*if( !new_vm->status( temp ) )
        {
            SAFE_DELETE( new_vm );
            result = -1;
        }*/
        
        SAFE_DELETE( caller->current_vm );
        caller->current_vm = new_vm;
        out += argv[0] + " is now current VM\n";
        result = 0;
    }
    
    else 
    {
        out += "error: unable to attach to " + argv[0] + "\n";
        result = -1;
    }
    
    if( argv.size() > 1 )
        out += "warning: ignoring excess arguments...\n";
    
    return result;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VMAdd::execute( vector< string > & argv,
                                             string & out )
{
    char buf[16];
    
    if( caller->current_vm == NULL )
    {
        out += "error: no VM to save\n";
        return -1;
    }
    
    caller->vms.push_back( caller->current_vm->copy() );
    
#ifndef __PLATFORM_WIN32__
    snprintf( buf, 16, "%lu", caller->vms.size() - 1 );
#else
    sprintf( buf, "%u", caller->vms.size() - 1 );   
#endif // __PLATFORM_WIN32__

    out += caller->current_vm->fullname() + " saved as VM " + buf + "\n";
    
    if( argv.size() > 0 )
        out += "warning: ignoring excess arguments...\n";
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VMRemove::execute( vector< string > & argv,
                                                string & out )
{
    vector<string>::size_type i = 0, vm_no, len = argv.size();
    
    for( ; i < len; i++ )
    {
        vm_no = strtoul( argv[i].c_str(), NULL, 10 );
        if( ( vm_no == 0 && errno == EINVAL ) || caller->vms.size() <= vm_no || 
            caller->vms[vm_no] == NULL )
        {
            out += "error: invalid VM id: " + argv[i] + "\n";
        }
        else
        {
            SAFE_DELETE( caller->vms[vm_no] );
        }
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VMSwap::execute( vector< string > & argv,
                                              string & out )
{
    vector<string>::size_type new_vm = 0;
    
    if( argv.size() < 1 )
    {
        out += string( "error: too few arguments...\n" );
        return -1;
    }
    
    new_vm = strtol( argv[0].c_str(), NULL, 10 );
    if( new_vm >= caller->vms.size() || caller->vms[new_vm] == NULL )
    {
        out += string( "error: invalid VM: " ) + argv[0] + "\n";
        return -1;
    }
    
    SAFE_DELETE( caller->current_vm );
    caller->current_vm = caller->vms[new_vm]->copy();
    out += "current VM is now " + caller->current_vm->fullname() + "\n";
    
    if( argv.size() > 1 )
        out += "warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VMList::execute( vector< string > & argv,
                                              string & out )
{
    char buf[16];
    vector<string>::size_type i, len = caller->vms.size();
    
    if( caller->current_vm != NULL )
        out += string("current VM: ") + caller->current_vm->fullname() + "\n";
    
    for( i = 0; i < len; i++ )
    {
        if( caller->vms[i] != NULL )
        {
#ifndef __PLATFORM_WIN32__
            snprintf( buf, 16, "%lu", i );
#else
            sprintf( buf, "%lu", i );
#endif // __PLATFORM_WIN32__
            out += string( "VM " ) + buf + ": " + 
                   caller->vms[i]->fullname() + "\n";
        }
    }
    
    if( argv.size() > 0 )
        out += "warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_VMAttachAdd::execute( vector< string > & argv,
                                                   string & out )
{
    t_CKINT result = 0;
    string exec = string( "vm @ " ) + ( argv.size() > 0 ? argv[0] : "" );
    
    if( caller->execute( exec, out ) )
    {
        exec = "vm + ";
        if( caller->execute( exec, out ) )
            result = 0;
        
        else
            result = -1;
    }
        
    else
        result = -1;
    
    return result;
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Shell::Command_Code::init( Chuck_Shell * caller )
{
    Command * temp;
    
    Command::init( caller );
    
    temp = new Command_CodeSave();
    temp->init( caller );
    commands["save"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_CodeList();
    temp->init( caller );
    commands["list"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_CodePrint();
    temp->init( caller );
    commands["print"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_CodeDelete();
    temp->init( caller );
    commands["delete"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_CodeWrite();
    temp->init( caller );
    commands["write"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_CodeRead();
    temp->init( caller );
    commands["read"] = temp;
    allocated_commands.push_back( temp );
    
    temp = new Command_CodeAdd();
    temp->init( caller );
    commands["add"] = temp;
    commands["+"] = temp;
    allocated_commands.push_back( temp );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ~Command_Code()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Shell::Command_Code::~Command_Code()
{
    vector<string>::size_type i, len = allocated_commands.size();
    
    //iterate through commands, delete the associated heap data
    for( i = 0; i != len; i++ )
        SAFE_DELETE( allocated_commands[i] );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_Code::execute( vector< string > & argv,
                                            string & out )
{
    if( argv.size() < 1)
        out += "usage: " + usage() + "\n";
    
    else if( commands.find( argv[0] ) == commands.end() )
        // command doesn't exist
        out += "error: code " + argv[0] + ": command not found\n";

    else
    {
        // call the mapped command
        Command * command = commands[argv[0]];
        argv.erase( argv.begin() );
        command->execute( argv, out );
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
string Chuck_Shell::Command_Code::usage()
{
    return "code [command] [args] ...";
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodeSave::execute( vector < string > & argv,
                                                string & out )
{
    if( argv.size() < 1 )
    {
        out += "error: please specify a name to identify the code\n";
    }    
    else
    {
        if( caller->code == "" )
            out += "error: no code to save\n";
        
        else
            caller->saved_code[argv[0]] = caller->code;
        
        if( argv.size() > 1 )
            out += "warning: ignoring excess arguments...\n";
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodeList::execute( vector < string > & argv,
                                                string & out )
{
    map< string, string >::iterator i = caller->saved_code.begin(), 
                                     end = caller->saved_code.end();
    for( ; i != end; i++ )
        out += i->first + "\n";
    
    if( argv.size() > 0 )
        out += "warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodeAdd::execute( vector < string > & argv,
                                               string & out )
{
    if( argv.size() == 0 )
    {
        if( caller->code != "" )
            caller->do_code( caller->code, out );
        
        else
            out += "error: no code to add\n";
    }
    
    else
    {
        vector<string>::size_type i, len;
        for( i = 0, len = argv.size(); i < len; i++ )
        {
            // iterate through all of the arguments, add the saved codes
            if( caller->saved_code.find( argv[i] ) == 
                caller->saved_code.end() )
                out += "error: code " + argv[i] + " not found\n";
            
            else
                caller->do_code( caller->code, out );
        }
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodePrint::execute( vector < string > & argv,
                                                 string & out )
{
    if( argv.size() == 0 )
    {
        if( caller->code != "" )
            out += caller->code + "\n";
        else
            out += "error: no code to print\n";
    }
    
    else
    {
        vector<string>::size_type i, len;
        for( i = 0, len = argv.size(); i < len; i++ )
        {
            if( caller->saved_code.find( argv[i] ) == 
                caller->saved_code.end() )
            {
                out += "error: code " + argv[i] + " not found\n";
            }
            else
            {
                string code = caller->saved_code[argv[i]];
                
                out += argv[i] + ":\n";
                out += code + "\n";
            }
        }
    }

    return 0;
}   




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodeDelete::execute( vector < string > & argv,
                                                  string & out )
{
    if( argv.size() == 0 )
    {
        out += "error: specify the code to delete\n";
    }
    else
    {
        vector<string>::size_type i, len;
        for( i = 0, len = argv.size(); i < len; i++ )
        {
            if( caller->saved_code.find( argv[i] ) == 
                caller->saved_code.end() )
                out += "error: code " + argv[i] + " not found\n";
            
            else
            {
                caller->saved_code.erase( argv[i] );
            }
        }
    }

    return 0;
}   




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodeWrite::execute( vector < string > & argv,
                                                 string & out )
{
    if( argv.size() < 1 )
    {
        out += "error: insufficient arguments...\n";
    }
    else
    {
        if( caller->saved_code.find( argv[0] ) == caller->saved_code.end() )
        {
            // this code doesnt exist!
            out += "error: code " + argv[0] + " not found\n";
        }
        else
        {
            string filename;
            string code = caller->saved_code[argv[0]];
            FILE * write_file;
            
            if( argv.size() >= 2 )
            {
                // use second argument as filename
                filename = argv[1];
            }
            else
            {
                // append .ck to the code name to get the file name
                filename = argv[0] + ".ck";
            }

            // open the file
            write_file = fopen( filename.c_str(), "w" );

            if( write_file == NULL )
            {
                out += "error: unable to open " + filename + " for writing\n";
            }
            else
            {
                // write code to the file
                if( fprintf( write_file, "%s", code.c_str() ) < 0 )
                    out += "error: unable to write to " + filename + "\n";
                
                fclose( write_file );
            }
        }
    }
    
    if( argv.size() > 2 )
        out += "warning: ignoring excess arguments...\n";

    return 0;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Shell::Command_CodeRead::execute( vector < string > & argv,
                                                string & out )
{
    if( argv.size() < 1 )
    {
        out += "error: insufficient arguments...\n";
    }
    else
    {
        string code_name;
        FILE * read_file;
        
        if( argv.size() >= 2 )
        {
            // use second argument as the code_name
            code_name = argv[1];
        }
        else
        {
            // remove the file extension to get the code_name
            // or use the whole filename if there is no extension
            string::size_type k = argv[0].rfind( "." );

            if( k == string::npos )
                code_name = argv[1];
            else
                code_name = string( argv[0], 0, k );
        }
        
        // open the file
        read_file = fopen( argv[0].c_str(), "r" );
        if( read_file == NULL )
            out += "error: unable to open " + argv[0] + " for reading\n";
        
        else
        {
            string code = "";
            char buffer[256], * result = fgets( buffer, 256, read_file );
            
            if( result == NULL )
                out += "error: unable to read " + argv[0] + 
                    ", or file is empty\n";

            else
            {
                while( result != NULL )
                {
                    code += buffer;
                    result = fgets( buffer, 256, read_file );
                }
                
                caller->saved_code[code_name] = code;
            }
            
            fclose( read_file );
        }
    }
    
    if( argv.size() > 2 )
        out += "warning: ignoring excess arguments...\n";

    return 0;
}   
