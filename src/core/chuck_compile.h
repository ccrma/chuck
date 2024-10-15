/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
// file: chuck_compile.h
// desc: chuck compile system unifying parser, type checker, and emitter
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2005 - riginal
//-----------------------------------------------------------------------------
#ifndef __CHUCK_COMPILE_H__
#define __CHUCK_COMPILE_H__

#include "chuck_def.h"
#include "chuck_parse.h"
#include "chuck_scan.h"
#include "chuck_type.h"
#include "chuck_emit.h"
#include "chuck_vm.h"
#include <list>
#include <set>




//-----------------------------------------------------------------------------
// forward reference
//-----------------------------------------------------------------------------
struct Chuck_DLL;




//-----------------------------------------------------------------------------
// compilation state, e.g., for compile targets
//-----------------------------------------------------------------------------
enum te_CompileState
{
    te_compile_inprogress,
    te_compile_complete
};




//-----------------------------------------------------------------------------
// name: intList
// desc: a linked list of token positions by line | moved from errmsg
//-----------------------------------------------------------------------------
typedef struct intList { t_CKINT i; struct intList *rest; } *IntList;
// constructor
static IntList intList( t_CKINT i, IntList rest )
{
    IntList l = (IntList)checked_malloc(sizeof *l);
    l->i=i; l->rest=rest;
    return l;
}




//-----------------------------------------------------------------------------
// name: struct Chuck_CompileTarget
// desc: an compilation target
//-----------------------------------------------------------------------------
struct Chuck_CompileTarget
{
public:
    // constructor
    Chuck_CompileTarget( te_HowMuch extent = te_do_all )
        : state(te_compile_inprogress),
          howMuch(extent), timestamp(0), fd2parse(NULL),
          lineNum(1), tokPos(0), AST(NULL),
          the_chuck(NULL)
    {
        // initialize
        the_linePos = intList( 0, NULL );
    }

    // destructor
    virtual ~Chuck_CompileTarget()
    { cleanup(); CK_SAFE_FREE( the_linePos ); }

    // performs cleanup (e.g., close file desriptors and reclaims AST)
    // post-parser prep for target to be archived in registry
    void cleanup();
    // clean up AST
    void cleanupAST();

public:
    // resolve and set filename and absolutePath for a compile target
    // * set as filename (possibly with modifications, e.g., with .ck appended)
    // * if `transplantPath` is non-empty, will use it as the base of the filename (unless filename is already an absolute path)
    // * if file is unresolved locally and `expandSearchToGlobal` is true, expand search to global search paths
    // * the file is resolved this will open a FILE descriptor in fd2parse
    t_CKBOOL resolveFilename( const std::string & theFilename,
                              const std::string & transplantPath,
                              t_CKBOOL expandSearchToGlobal );
    // get filename
    std::string getFilename() const { return filename; }
    // set absolute path
    void setAbsolutePath( const std::string & fullpath ) { absolutePath = fullpath; }
    // get absolute path
    std::string getAbsolutePath() const { return absolutePath; }
    // hash key
    std::string key() const { return absolutePath; }

public:
    // filename for reading from file
    std::string filename;
    // this should be unique; also key for import hash
    std::string absolutePath;

public:
    // state
    te_CompileState state;
    // all or import-only or no-import
    te_HowMuch howMuch;
    // code literal (alternative to reading from file)
    std::string codeLiteral;

    // line number (should be at end of file; used for error reporting)
    t_CKINT lineNum;
    // token position (used for error reporting)
    t_CKINT tokPos;
    // file descriptor to parse
    FILE * fd2parse;
    // file source info (for better error reporting)
    CompileFileSource fileSource;
    // pointer to abstract syntax tree
    a_Program AST;
    // for the current file
    IntList the_linePos;

    // targets this target depends on
    std::vector<Chuck_CompileTarget *> dependencies;
    // timestamp of target file when target was compiled
    // used to detect and potentially warn of modified files
    time_t timestamp;

    // reference to ChucK instance
    ChucK * the_chuck;

public: // ONLY USED for topology
    t_CKBOOL mark;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_ImportRegistry
// desc: registry of import targets
//-----------------------------------------------------------------------------
struct Chuck_ImportRegistry
{
public:
    // constructor
    Chuck_ImportRegistry();
    virtual ~Chuck_ImportRegistry() { clearAll(); }

public:
    // clear in-progress list
    void clearInProgress();
    // clear all contents (in-progress and done); this is for when VM is cleared
    void clearAll();

public:
    // look up a compile target by path
    Chuck_CompileTarget * lookup( const std::string & path, Chuck_CompileTarget * relativeTo = NULL );
    // add a compile target to in-progress list
    t_CKBOOL addInProgress( Chuck_CompileTarget * target );
    // remove a compile target by path
    t_CKBOOL remove( const std::string & path );
    // mark a target as complete
    void markComplete( Chuck_CompileTarget * target );

protected:
    // map of targets being compiled (not yet completed)
    std::map<std::string, Chuck_CompileTarget *> m_inProgress;
    // map of successfully compiled targets
    std::map<std::string, Chuck_CompileTarget *> m_done;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Compiler
// desc: the sum of the components in compilation
//-----------------------------------------------------------------------------
struct Chuck_Compiler
{
protected: // data
    // carrier
    Chuck_Carrier * m_carrier;

public: // get protected data
    // REFACTOR-2017: get associated, per-compiler environment
    Chuck_Env * env() const { return m_carrier->env; }
    // REFACTOR-2017: get associated, per-compiler VM
    Chuck_VM * vm() const { return m_carrier->vm; }
    // REFACTOR-2017: get associated, per-compiler carrier
    Chuck_Carrier * carrier() const { return m_carrier; }
    // get import registry | 1.5.3.5 (ge)
    Chuck_ImportRegistry * imports() { return &m_importRegistry; }
    // set carrier
    t_CKBOOL setCarrier( Chuck_Carrier * c ) { m_carrier = c; return TRUE; }

public: // data
    // emitter
    Chuck_Emitter * emitter;
    // generated code
    Chuck_VM_Code * code;

    // import registry
    Chuck_ImportRegistry m_importRegistry;
    // auto-depend flag
    t_CKBOOL m_auto_depend;
    // recent map
    std::map<std::string, Chuck_Context *> m_recent;

    // chugins
    std::list<Chuck_DLL *> m_dlls;
    // libraries (ck code) to import
    std::list<std::string> m_cklibs_to_preload;

    // origin hint; this flag is set to different ckte_Origin values
    // to denote where new entities originate | 1.5.0.0 (ge) added
    ckte_Origin m_originHint;

public: // to all
    // contructor
    Chuck_Compiler();
    // destructor
    virtual ~Chuck_Compiler();

    // initialize
    t_CKBOOL initialize();
    // shutdown
    void shutdown();

public: // additional binding
    // bind a new type system module, via query function
    t_CKBOOL bind( f_ck_query query_func, const std::string & name,
                   const std::string & nspc = "global" );

public: // compile
    // set auto depend
    void set_auto_depend( t_CKBOOL v );
    // parse, type-check, and emit a program from file
    t_CKBOOL compileFile( const std::string & filename );
    // parse, type-check, and emit a program from code string
    t_CKBOOL compileCode( const std::string & codeLiteral );
    // compile a target | 1.5.3.5 (ge)
    // NOTE: this function will memory-manage `target`
    //       (do not access or delete `target` afterwards)
    t_CKBOOL compile( Chuck_CompileTarget * target );
    // resolve a type automatically, if auto_depend is on
    t_CKBOOL resolve( const std::string & type );
    // get the code generated from the last compile()
    Chuck_VM_Code * output( );

public: // replace-dac | added 1.4.1.0 (jack)
    // sets a "replacement dac": one global UGen is secretly used
    // as a stand-in for "dac" for this compilation;
    // for example, ChuckSubInstances in Chunity use a global Gain as a
    // replacement dac, then use the global getUGenSamples() function to
    // get the samples of the gain. this enables the creation
    // of a new sample sucker.
    void setReplaceDac( t_CKBOOL shouldReplaceDac, const std::string & replacement );

public:
    // .chug and .ck modules pre-load sequence | 1.4.1.0 (ge) refactored
    t_CKBOOL load_external_modules( const std::string & extension,
                                    std::list<std::string> & chugin_search_paths,
                                    std::list<std::string> & named_dls,
                                    t_CKBOOL recursiveSearch );
    // load external modules in named directory
    t_CKBOOL load_external_modules_in_directory( const std::string & directory,
                                                 const std::string & extension,
                                                 t_CKBOOL recursiveSearch );
    // load an external module by path and extension | 1.5.2.5 (ge) added
    t_CKBOOL load_external_module( const std::string & path, const std::string & name = "" );
    // load a chugin by path | 1.5.2.5 (ge) exposed API for more dynamic loading
    t_CKBOOL load_external_chugin( const std::string & path, const std::string & name = "" );
    // load a ck module by file path | 1.5.2.5 (ge) exposed API for more dynamic loading
    t_CKBOOL load_external_cklib( const std::string & path, const std::string & name = "" );

public:
    // chugin probe | 1.5.0.4 (ge) added
    static t_CKBOOL probe_external_modules( const std::string & extension,
                                            std::list<std::string> & chugin_search_paths,
                                            std::list<std::string> & named_dls,
                                            t_CKBOOL recursiveSearch,
                                            std::list<std::string> & ck_libs );
    // probe all external modules by extension within a directory
    static t_CKBOOL probe_external_modules_in_directory( const std::string & directory,
                                                         const std::string & extension,
                                                         t_CKBOOL recursiveSearch,
                                                         std::list<std::string> & ck_libs );
    // probe external chugin by file path
    static t_CKBOOL probe_external_chugin( const std::string & path, const std::string & name = "" );

protected: // internal
    // compile a single target
    t_CKBOOL compile_single( Chuck_CompileTarget * target );
    // compile entire file
    t_CKBOOL compile_entire_file( Chuck_Context * context );
    // import only: public definitions (e.g., classes and operator overloads)
    t_CKBOOL compile_import_only( Chuck_Context * context ); // 1.5.2.5 (ge) added
    // all except import
    t_CKBOOL compile_all_except_import( Chuck_Context * context );

protected: // internal import dependency helpers
    // produce a compilation sequences of targets from a import dependency graph
    static t_CKBOOL generate_compile_sequence( Chuck_CompileTarget * head,
                                               std::vector<Chuck_CompileTarget *> & sequence,
                                               std::vector<Chuck_CompileTarget *> & problems );
    // visit
    static t_CKBOOL visit( Chuck_CompileTarget * node,
                           std::vector<Chuck_CompileTarget *> & sequence,
                           std::set<Chuck_CompileTarget *> & permanent,
                           std::set<Chuck_CompileTarget *> & temporary,
                           std::vector<Chuck_CompileTarget *> & problems );

public: // import
    // scan for @import statements; builds a list of dependencies in the target
    t_CKBOOL scan_imports( Chuck_CompileTarget * target );
    // scan for @import statements, and return a list of resulting import targets
    t_CKBOOL scan_imports( Chuck_Env * env, Chuck_CompileTarget * target, Chuck_ImportRegistry * registery );

    // add import path
    t_CKBOOL add_import_path( const std::string & path, Chuck_Context * context );
    // look up in recent
    Chuck_Context * find_import_path( const std::string & path );
    // look up in recent
    Chuck_Context * find_import_type( const std::string & type );
};





#endif
