/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler, Virtual Machine, and Synthesis Engine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the dual-license terms of EITHER the MIT License OR the GNU
  General Public License (the latter as published by the Free Software
  Foundation; either version 2 of the License or, at your option, any
  later version).

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  MIT Licence and/or the GNU General Public License for details.

  You should have received a copy of the MIT License and the GNU General
  Public License (GPL) along with this program; a copy of the GPL can also
  be obtained by writing to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck_compile.h
// desc: chuck compile system unifying parser, type checker, and emitter
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//   date: Autumn 2005 - original
//         Autumn 2017 - major refactor (with Jack Atherton)
//         Summer 2023 - major update to error reporting
//         Autumn 2024 - import system (with Nick Shaheed)
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
#include <string>




//-----------------------------------------------------------------------------
// forward reference
//-----------------------------------------------------------------------------
struct Chuck_DLL;
struct Chuck_CompileTarget;




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
// name: struct ImportTargetNode
// desc: a specific import node, corresponding to a particular @import
//-----------------------------------------------------------------------------
struct ImportTargetNode
{
    Chuck_CompileTarget * target; // actual target
    t_CKINT where; // pos
    t_CKINT line; // line

    ImportTargetNode( Chuck_CompileTarget * t = NULL, t_CKINT wh = 0, t_CKINT ln = 0 )
        : target( t ), where( wh ), line( ln ) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_CompileTarget
// desc: an compilation target
//-----------------------------------------------------------------------------
struct Chuck_CompileTarget
{
public:
    // constructor
    Chuck_CompileTarget( te_HowMuch extent = te_do_all )
        : state(te_compile_inprogress), howMuch(extent), isSystemImport(FALSE),
          fd2parse(NULL), chugin(NULL), lineNum(1), tokPos(0),
          AST(NULL), timestamp(0), the_chuck(NULL)
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
    // get filename
    std::string getFilename() const { return filename; }
    // set absolute path
    void setAbsolutePath( const std::string & fullpath ) { absolutePath = fullpath; }
    // get absolute path
    std::string getAbsolutePath() const { return absolutePath; }
    // hash key
    std::string key() const { return absolutePath; }
    // is chugin?
    t_CKBOOL isChugin() const { return chugin != NULL; }

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
    // is this a system or user import?
    t_CKBOOL isSystemImport;

    // file descriptor to parse
    FILE * fd2parse;
    // code literal (alternative to reading from file)
    std::string codeLiteral;
    // loaded chugin
    Chuck_DLL * chugin;

    // line number (should be at end of file; used for error reporting)
    t_CKINT lineNum;
    // token position (used for error reporting)
    t_CKINT tokPos;
    // file source info (for better error reporting)
    CompileFileSource fileSource;
    // pointer to abstract syntax tree
    a_Program AST;
    // for the current file
    IntList the_linePos;

    // targets this target depends on
    std::vector<ImportTargetNode> dependencies;

    // timestamp of target file when target was compiled
    // used to detect and potentially warn of modified files
    time_t timestamp;
    // reference to ChucK instance
    ChucK * the_chuck;
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
    virtual ~Chuck_ImportRegistry() { shutdown(); }

public:
    // clear in-progress list
    void clearInProgress();
    // clear all user-imported contents (in-progress and imported)
    // NOTE: system-imports are not removed; chugin imports are all system-level
    // NOTE: this is for when VM is cleared
    void clearAllUserImports();

public:
    // look up a compile target by path
    Chuck_CompileTarget * lookup( const std::string & path, Chuck_CompileTarget * relativeTo = NULL );
    // add a compile target to in-progress list
    t_CKBOOL addInProgress( Chuck_CompileTarget * target );
    // remove a compile target by path
    t_CKBOOL remove( const std::string & path );
    // move target from in progress to imported list
    void commit( Chuck_CompileTarget * target );
    // add a chugin
    Chuck_CompileTarget * commit( Chuck_DLL * chugin );

protected:
    // clear everything (system and user)
    void shutdown();

protected:
    // map of targets being compiled (not yet completed)
    std::map<std::string, Chuck_CompileTarget *> m_inProgressCKFiles;
    // map of successfully compiled (ck) targets
    std::map<std::string, Chuck_CompileTarget *> m_importedTargets;
    // map of successfully imported chugins
    std::map<std::string, Chuck_DLL *> m_importedChugins;
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
    // get import registry | 1.5.4.0 (ge)
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

public: // compile from file or code
    // parse, type-check, and emit a program from file
    t_CKBOOL compileFile( const std::string & filename );
    // parse, type-check, and emit a program from code string
    // * can optionally provide a filename (or path ending in '/')
    //   for @import pathing (by default, working dir is used)
    t_CKBOOL compileCode( const std::string & codeLiteral,
                          const std::string & optFilepath = "" );
    // get the code generated from the last compile()
    Chuck_VM_Code * output();

public: // import while observing semantics of chuck @import
    // import a .ck module by file path
    t_CKBOOL importFile( const std::string & filename );
    // import from ChucK code
    t_CKBOOL importCode( const std::string & codeLiteral,
                         const std::string & optFilepath );
    // import a chugin by path (and optional short-hand name)
    t_CKBOOL importChugin( const std::string & path,
                           t_CKBOOL createNamespace,
                           const std::string & name,
                           std::string & errorStr );

public:
    // compile a target | 1.5.4.0 (ge)
    // NOTE: this function will memory-manage `target`
    // (do not access or delete `target` after function call)
    t_CKBOOL compile( Chuck_CompileTarget * target );

public:
    // opens file for compilation...
    // * resolves and set filename and absolutePath for a compile target
    // * set as filename (possibly with modifications, e.g., with .ck appended)
    // * if `importer` is non-empty, will use it as the base of the filename (unless filename is already an absolute path)
    // * if file is unresolved locally and `expandSearchToGlobal` is true, expand search to global search paths
    // * the file is resolved this will open a FILE descriptor in fd2parse
    // * wherePos can be provided to indiciate parser position in containing file (e.g., @import)
    t_CKBOOL openFile( Chuck_CompileTarget * target,
                       const std::string & theFilename,
                       Chuck_CompileTarget * importer,
                       t_CKBOOL expandSearchToGlobal,
                       t_CKINT wherePos = 0 );
    // resolve filename into absolute path...
    // * possibly with modifications, e.g., with .ck appended)
    // * if `importerAbsolutePath` is non-empty, will use it as the base of the filename (unless filename is already an absolute path)
    // * if file is unresolved locally and `expandSearchToGlobal` is true, expand search to global search paths
    std::string resolveFilename( const std::string & filename,
                                 const std::string & importerAbsolutePath,
                                 t_CKBOOL expandSearchToGlobal );

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

public: // built-in binding mechanism
    // bind a new type system module, via query function
    t_CKBOOL bind( f_ck_query query_func, const std::string & name,
                   const std::string & nspc = "global" );

public: // replace-dac | added 1.4.1.0 (jack)
    // sets a "replacement dac": one global UGen is secretly used
    // as a stand-in for "dac" for this compilation;
    // for example, ChuckSubInstances in Chunity use a global Gain as a
    // replacement dac, then use the global getUGenSamples() function to
    // get the samples of the gain. this enables the creation
    // of a new sample sucker.
    void setReplaceDac( t_CKBOOL shouldReplaceDac, const std::string & replacement );

public: // un-used / un-implemented auto-depend stubs
    // set auto depend
    void setAutoDepend( t_CKBOOL v );
    // resolve a type automatically, if auto_depend is on
    t_CKBOOL resolve( const std::string & type );

protected: // internal
    // parse, type-check, and emit a program from file (with option on extent)
    t_CKBOOL compile_file_opt( const std::string & filename, te_HowMuch extent );
    // parse, type-check, and emit a program from code string (with option on extent)
    t_CKBOOL compile_code_opt( const std::string & codeLiteral, const std::string & optFilepath, te_HowMuch extent );
    // compile a single target
    t_CKBOOL compile_single( Chuck_CompileTarget * target );
    // compile entire file
    t_CKBOOL compile_entire_file( Chuck_Context * context );
    // import only: public definitions (e.g., classes and operator overloads)
    t_CKBOOL compile_import_only( Chuck_Context * context ); // 1.5.2.5 (ge) added
    // all except import
    t_CKBOOL compile_all_except_import( Chuck_Context * context );

protected: // import
    // scan for @import statements; builds a list of dependencies in the target
    t_CKBOOL scan_imports( Chuck_CompileTarget * target );
    // scan for @import statements, and return a list of resulting import targets
    t_CKBOOL scan_imports( Chuck_Env * env, Chuck_CompileTarget * target );
    // import chugin
    t_CKBOOL import_chugin_opt( const std::string & path, const std::string & name, std::string & errorStr );

protected: // internal import dependency helpers
    // produce a compilation sequences of targets from a import dependency graph
    static t_CKBOOL generate_compile_sequence( ImportTargetNode * head,
                                               std::vector<ImportTargetNode *> & sequence,
                                               std::vector<ImportTargetNode *> & problems );
    // visit -- recursive function as part of topological sort for dependency serialization
    static t_CKBOOL visit( ImportTargetNode * node,
                           std::vector<ImportTargetNode *> & sequence,
                           std::set<Chuck_CompileTarget *> & permanent,
                           std::set<Chuck_CompileTarget *> & temporary,
                           std::vector<ImportTargetNode *> & problems );
};




//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------
// log a chugin load
void logChuginLoad( const std::string & name, t_CKINT logLevel );
// log a chuck file found
void logCKFileFound( const std::string & name, t_CKINT logLevel );
// scan for subdirs in a dir
t_CKBOOL scan_for_dirs_in_directory( const std::string & directory,
                                     const std::string & extensionForSubdirTest,
                                     t_CKBOOL recursiveSearch,
                                     std::list<std::string> & results );




#endif
