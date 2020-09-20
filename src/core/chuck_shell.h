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
// file: chuck_shell.h
// desc: chuck shell implementation
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __CHUCK_SHELL_H__
#define __CHUCK_SHELL_H__

#include "chuck_def.h"
#include "chuck_errmsg.h"
#include "chuck_vm.h"

#include <string>
#include <map>
#include <vector>

using namespace std;

// forward references
class Chuck_Shell_VM;
class Chuck_Shell_UI;

typedef string Chuck_Shell_Request;
typedef string Chuck_Shell_Response;

//-----------------------------------------------------------------------------
// name: class Chuck_Shell
// desc: controller class for facilitating interaction between a shell UI and a 
//      shell mode.
//-----------------------------------------------------------------------------
class Chuck_Shell
{
public:
    Chuck_Shell();
    virtual ~Chuck_Shell();
    
    t_CKBOOL init( Chuck_VM * process_vm, Chuck_Shell_UI * );
    void run();
    t_CKBOOL execute( string &, string & );
    void close();
    void exit();

public: // HACK-GE: these were moved from protected for win32
    vector < Chuck_Shell_VM * > vms;
    Chuck_VM * process_vm;
    Chuck_Shell_VM * current_vm;

    //forward declaration
    class Command;

    // alias, variable, and command maps
    map < string, string > aliases;
    map < string, string > variables;
    map < string, Command * > commands;
    vector < Command * > allocated_commands;
	
	map < string, string > saved_code;

    string code;

	// helper functions
	t_CKBOOL do_glob( const string &, string &, vector < string > & );
    void do_code( string &, string &, string command = "+ " );
    void do_code_context( string & );

protected:
	// helper functions
	void do_aliases();
	void do_variables();
	
	// code helper functions
	void start_code();
    void continue_code( string & );
    void string_hash( string &, string & );
    
    Chuck_Shell_UI * ui;
    
    t_CKBOOL initialized;
    t_CKBOOL stop;
    
    string prompt;
    
    string in;
        
    // code entry variables
    t_CKBOOL code_entry_active;
    t_CKUINT scope;

public: // HACK-GE: moved from protected for win32

//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command
// desc: superclass to Chuck_Shell_Commands
//-----------------------------------------------------------------------------
    class Command
    {
    public:
        // virtual destructor (gewang)
        virtual ~Command() { }

    public:
        virtual t_CKBOOL init( Chuck_Shell * );
        virtual t_CKINT execute( vector< string > &, string & ) = 0;
		virtual string usage();
		virtual string long_usage();
    
    protected:
        Chuck_Shell * caller;
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VM : public Command
    {
    public:
        ~Command_VM();
        t_CKBOOL init( Chuck_Shell * );
        t_CKINT execute( vector< string > &, string & );
		string usage();
    
    protected:
        map < string, Command * > commands;
        vector < Command * > allocated_commands;
    };

//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM_Add
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VMAdd : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM_Remove
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VMRemove : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };

//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM_Attach
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VMAttach : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM_List
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VMList : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM_Swap
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VMSwap : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_VM_AttachAdd
// desc: ...
//-----------------------------------------------------------------------------
    class Command_VMAttachAdd : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Add
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Add : public Command
    {
    public:
        t_CKINT execute( vector < string > &, string & );
		string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Remove
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Remove : public Command
    {
    public:
        t_CKINT execute( vector < string > &, string & );
		string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Status
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Status : public Command
    {
    public:
        t_CKINT execute( vector < string > &, string & );
		string usage();
    };
    
	//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Removeall
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Removeall : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Removelast
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Removelast : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Replace
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Replace : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Kill
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Kill : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		//string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Close
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Close : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		//string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Exit
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Exit : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		//string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Ls
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Ls : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		//string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Cd
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Cd : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
		//string usage();
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Pwd
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Pwd : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Alias
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Alias : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Unalias
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Unalias : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Source
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Source : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Code
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Code : public Command
    {
    public:
        ~Command_Code();
        t_CKBOOL init( Chuck_Shell * );
        t_CKINT execute( vector< string > &, string & );
		string usage();
        
    private:
        map < string, Command * > commands;
        vector < Command * > allocated_commands;
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeContext
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodeContext : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
    
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeSave
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodeSave : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };

//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeDelete
// desc: ...
//-----------------------------------------------------------------------------
	class Command_CodeDelete : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
	
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeAdd
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodeAdd : public Command
    {
	public:
        t_CKINT execute( vector< string > &, string & );
    };
	
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeList
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodeList : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };

//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodePrint
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodePrint : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
	
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeWrite
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodeWrite : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };

//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_CodeRead
// desc: ...
//-----------------------------------------------------------------------------
    class Command_CodeRead : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
	
//-----------------------------------------------------------------------------
// name: class Chuck_Shell::Command_Help
// desc: ...
//-----------------------------------------------------------------------------
    class Command_Help : public Command
    {
    public:
        t_CKINT execute( vector< string > &, string & );
    };
	
};


//-----------------------------------------------------------------------------
// name: class Chuck_Shell_VM
// desc: encapsulates local and network VMs into a single class
//-----------------------------------------------------------------------------
class Chuck_Shell_VM
{
public:
    virtual ~Chuck_Shell_VM() {}

public:
	virtual Chuck_Shell_VM * copy()=0;
    virtual t_CKBOOL add_shred( const vector< string > &, string & )=0;
    virtual t_CKBOOL remove_shred( const vector< string > &, string & )=0;
    virtual t_CKBOOL remove_all( string & )=0;
    virtual t_CKBOOL remove_last( string & )=0;
    virtual t_CKBOOL replace_shred( const vector< string > &, string & )=0;
    virtual t_CKBOOL status( string & )=0;
    virtual t_CKBOOL kill( string & )=0;
    virtual string fullname()=0;
};

//-----------------------------------------------------------------------------
// name: class Chuck_Shell_Network_VM
// desc: VM across the network
//-----------------------------------------------------------------------------
class Chuck_Shell_Network_VM : public Chuck_Shell_VM
{
public:
    virtual ~Chuck_Shell_Network_VM() {}
	
public:
	Chuck_Shell_VM * copy();
    t_CKBOOL init( const string &, t_CKINT );
    t_CKBOOL add_shred( const vector< string > &, string & );
    t_CKBOOL remove_shred( const vector< string > &, string & );
    t_CKBOOL remove_all( string & );
    t_CKBOOL remove_last( string & );
    t_CKBOOL replace_shred( const vector< string > &, string & );
    t_CKBOOL status( string & );
    t_CKBOOL kill( string & );
    string fullname();
	
private: 
		string hostname;
    t_CKINT port;
};
/*
//-----------------------------------------------------------------------------
// name: class Chuck_Shell_Network_VM
// desc: VM across the network
//-----------------------------------------------------------------------------
class Chuck_Shell_Process_VM : public Chuck_Shell_VM
{
public:
    virtual ~Chuck_Shell_Process_VM() {}
	
public:
	Chuck_Shell_VM * copy();
    t_CKBOOL init( const string &, t_CKINT );
    t_CKBOOL add_shred( const vector< string > &, string & );
    t_CKBOOL remove_shred( const vector< string > &, string & );
    t_CKBOOL remove_all( string & );
    t_CKBOOL remove_last( string & );
    t_CKBOOL replace_shred( const vector< string > &, string & );
    t_CKBOOL status( string & );
    t_CKBOOL kill( string & );
    string fullname();
	
	t_CKUINT last_shred_id();
	
private: 
	Chuck_VM * vm;
	Chuck_Compiler * compiler;
};
*/
//-----------------------------------------------------------------------------
// name: class Chuck_Shell_UI
// desc: superclass to various types of Chuck UIs
//-----------------------------------------------------------------------------
class Chuck_Shell_UI
{
public:
    virtual ~Chuck_Shell_UI() {}

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL next_command( const string &, string & ) = 0;
    virtual void next_result( const string & ) = 0;
};

// prototype for shell thread routine
void * shell_cb( void * p );

#endif //__CHUCK_SHELL_H__
