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
// file: chuck_compile.cpp
// desc: chuck compile system unifying parser, type checker, and emitter
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//   date: (see header for information)
//-----------------------------------------------------------------------------
#include "chuck_compile.h"
#include "chuck_lang.h"
#include "chuck_errmsg.h"
#include "chuck.h"
#include "chuck_io.h"

#ifndef  __DISABLE_OTF_SERVER__
#include "chuck_otf.h"
#endif

#include "ugen_osc.h"
#include "ugen_xxx.h"
#include "ugen_filter.h"
#include "ugen_stk.h"
#include "uana_xform.h"
#include "uana_extract.h"

#include "ulib_ai.h"
#include "ulib_doc.h"
#include "ulib_machine.h"
#include "ulib_math.h"
#include "util_platforms.h"
#include "ulib_std.h"
#include "util_string.h"

#ifndef __DISABLE_NETWORK__
#include "ulib_opsc.h"
#endif

#if defined(__PLATFORM_WINDOWS__)
  #include "dirent_win32.h"
#endif

#include <sys/stat.h>

#include <string>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL load_internal_modules( Chuck_Compiler * compiler );
t_CKBOOL load_external_modules( Chuck_Compiler * compiler,
                                const char * extension,
                                list<string> & chugin_search_paths,
                                list<string> & named_dls);
t_CKBOOL load_module( Chuck_Compiler * compiler, Chuck_Env * env,
                      f_ck_query query, const char * name, const char * nspc );




//-----------------------------------------------------------------------------
// name: Chuck_Compiler()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Compiler::Chuck_Compiler()
{
    emitter = NULL;
    code = NULL;

    // REFACTOR-2017: add carrier
    m_carrier = NULL;
    // initialize
    m_auto_depend = FALSE;

    // origin hint | 1.5.0.0 (ge) added
    m_originHint = ckte_origin_UNKNOWN;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Compiler()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Compiler::~Chuck_Compiler()
{
    // call shutdown
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize the compiler
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::initialize()
{
    // this should have been set by initialization
    assert( m_carrier != NULL );

    // set origin hint
    m_originHint = ckte_origin_BUILTIN;

    // allocate the type checker
    if( !type_engine_init( m_carrier ) )
        goto error;
    // ensure carrier's env is set in type_engine_init
    assert( m_carrier->env != NULL );

    // allocate the emitter
    emitter = emit_engine_init( m_carrier->env );
    // add reference
    CK_SAFE_ADD_REF( emitter );
    // set auto depend to 0
    m_auto_depend = FALSE;

    // load internal libs
    if( !load_internal_modules( this ) )
        goto error;

    // unset origin hint
    m_originHint = ckte_origin_UNKNOWN;

    return TRUE;

error:
    // clean up | 1.4.1.0 (ge) removed -- should be cleaned up outside
    // this->shutdown();

    // pop indent
    EM_poplog();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown the compiler
//-----------------------------------------------------------------------------
void Chuck_Compiler::shutdown()
{
    // log
    EM_log( CK_LOG_SYSTEM, "shutting down compiler..." ) ;
    // push indent
    EM_pushlog();

    // free emitter (and set emitter to NULL)
    emit_engine_shutdown( emitter );
    // check the pointer is now NULL
    assert( emitter == NULL );

    // clear more state
    code = NULL;
    m_auto_depend = FALSE;
    m_recent.clear();

    // log | 1.5.1.8
    EM_log( CK_LOG_SYSTEM, "compiler shutown complete." ) ;
    // pop indent
    EM_poplog();

    // if we have carrier
    if( m_carrier != NULL )
    {
        // free type engine env within carrier
        type_engine_shutdown( m_carrier );
        // check the pointer is now NULL
        assert( m_carrier->env == NULL );
        // zero out the carrier reference | 1.5.1.1
        m_carrier = NULL;
    }
}




//-----------------------------------------------------------------------------
// name: compileFile()
// desc: parse, type-check, and emit a program from file
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compileFile( const string & filename )
{
    // call internal compile file with option
    return this->compile_file_opt( filename, te_do_all );
}




//-----------------------------------------------------------------------------
// name: compileCode()
// desc: parse, type-check, and emit a program from code
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compileCode( const string & codeLiteral,
                                      const string & optFilepath )
{
    // call internal compile file with option
    return this->compile_code_opt( codeLiteral, optFilepath, te_do_all );
}




//-----------------------------------------------------------------------------
// name: importFile()
// desc: import a CK file, observing the semantics of chuck @import
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::importFile( const string & filename )
{
    // call internal compile file with option
    return this->compile_file_opt( filename, te_do_import_only );
}




//-----------------------------------------------------------------------------
// name: importCode()
// desc: import from code, observing the semantics of chuck @import
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::importCode( const string & codeLiteral,
                                     const string & optFilepath )
{
    // call internal compile file with option
    return this->compile_code_opt( codeLiteral, optFilepath, te_do_import_only );
}




//-----------------------------------------------------------------------------
// name: importChugin()
// desc: import a chugin by path (and optional short-hand name)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::importChugin( const string & path,
                                       t_CKBOOL createNamespace,
                                       const string & name,
                                       string & errorStr )
{
    // check if this import should be in its namespace | 1.5.4.0 (ge) added
    if( createNamespace )
    {
        // make context for this chugin import and load it
        // this is necessary to make sure the chugin is imported into its own
        // namespace, to avoid being imported into say a host namespace -- e.g.,
        // a chuck file that @import this chugin
        // ALSO: if the chuck file that imports the chugin contains an error,
        // its context would be cleaned up, which leads to a crash on chuck exit;
        // this created context prevent that crash
        // ... further investigation may be beneficial to fully understand this
        // mechanism
        type_engine_load_context( this->carrier()->env, type_engine_make_context( NULL, "@[chugin-import]" ) );
    }

    // call internal import chugin
    t_CKBOOL ret = this->import_chugin_opt( path, name, errorStr );

    // if create namespace
    if( createNamespace )
    {
        // unload the context
        type_engine_unload_context( this->carrier()->env );
    }

    // done
    return ret;
}




//-----------------------------------------------------------------------------
// name: compile_file_opt()
// desc: parse, type-check, and emit a program, with option for how much to compile
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile_file_opt( const string & filename, te_HowMuch extent )
{
    // create a compile target
    Chuck_CompileTarget * target = new Chuck_CompileTarget( extent );
    // resolve filename locally
    if( !this->openFile( target, filename, NULL, FALSE ) )
    {
        // delete target
        CK_SAFE_DELETE( target );
        // return
        return FALSE;
    }

    // compile target; compile() will memory-manage target (so no need to delete here)
    return this->compile( target );
}




//-----------------------------------------------------------------------------
// name: compile_code_opt()
// desc: parse, type-check, and emit a program, with options for filepath
//       and for how much to compile
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile_code_opt( const string & codeLiteral,
                                           const string & optFilepath,
                                           te_HowMuch extent )
{
    // create a compile target
    Chuck_CompileTarget * target = new Chuck_CompileTarget( extent );
    // set filename to code literal constant
    target->filename = CHUCK_CODE_LITERAL_SIGNIFIER;
    // check for optional filename
    string opt = trim(optFilepath);
    // the path base (e.g., for import path)
    string pathBase;
    // is it empty?
    if( opt == "" )
    {
        // get working directory
        pathBase = this->carrier()->chuck->getParamString( CHUCK_PARAM_WORKING_DIRECTORY );
    }
    else // not empty
    {
        // use the optionally provided path, make into full path if not already
        opt = get_full_path( opt );
        // get the path portion of the optionally provide path
        pathBase = extract_filepath_dir( opt );
    }
    // construct full path to be associated with the file so me.sourceDir() works
    target->absolutePath = pathBase + target->filename;
    // set code literal
    target->codeLiteral = codeLiteral;

    // compile target; compile() will memory-manage target (so no need to delete here)
    return this->compile( target );
}




//-----------------------------------------------------------------------------
// name: compile()
// desc: compile a single target
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile( Chuck_CompileTarget * target )
{
    // hold return value
    t_CKBOOL ret = TRUE;
    // compilation sequence
    std::vector<ImportTargetNode *> sequence;
    // if cycle encountered, this returns cycle path
    std::vector<ImportTargetNode *> problems;
    // local import target node
    ImportTargetNode node( target, 0, 0 );

    // set the chuck
    target->the_chuck = this->carrier()->chuck;
    // clear in-progress
    this->imports()->clearInProgress();
    // add current target to registry to avoid cycles
    this->imports()->addInProgress( target );
    // scan for imports
    if( !this->scan_imports( target ) )
    {
        // error encountered
        ret = FALSE;
        // clean up
        goto cleanup;
    }

    // topological sort for serial compilation
    if( !this->generate_compile_sequence( &node, sequence, problems ) )
    {
        // cycle detected
        EM_error2( 0, "@import error -- cycle detected:" );
        // print head of cycle
        EM_error2( 0, " |- '%s' is imported from...", problems[0]->target->filename.c_str() );
        for( t_CKINT i = 1; i < problems.size()-1; i++ )
        {
            // print middle of cycle
            EM_error2( 0, " |- '%s':[line %d], which is imported from...", problems[i]->target->filename.c_str(), problems[i-1]->line );
            // log for more detail
            EM_log_opts( CK_LOG_INFO, EM_LOG_NO_PREFIX, "[chuck]:  |- (full path: '%s')", problems[i]->target->absolutePath.c_str() );
        }
        // print origin of cycle
        EM_error2( 0, " |- '%s':[line %d] (this is the originating file)", problems.back()->target->filename.c_str(), problems[problems.size()-2]->line );
        // error encountered
        ret = FALSE;
        // clean up
        goto cleanup;
    }

    // iterator over sequence
    // NOTE: the base target should be the last one in the sequence
    for( t_CKINT i = 0; i < sequence.size(); i++ )
    {
        // log
        EM_log( CK_LOG_FINER, "compiling import target '%s'...", target->filename.c_str() );

        // compile dependency (import only)
        ret = compile_single( sequence[i]->target );
        if( !ret )
        {
            // if there is a container
            if( sequence[i]->target->howMuch == te_do_import_only && sequence[i]->target != target )
            {
                // set target for printing filename where error is
                EM_setCurrentTarget( sequence[i]->target );
                EM_error2( 0, "...in file '%s'", sequence[i]->target->absolutePath.c_str() );
                // set target for error reporting for the originating file
                EM_setCurrentTarget( target );
                // report container
                EM_error2( 0, "(error originates in an imported file that '%s' depends on...", target->filename.c_str() );
                EM_error2( 0, "...i.e., '%s' directly or indirectly imports '%s')", target->filename.c_str(), sequence[i]->target->filename.c_str() );
                // unset target
                EM_setCurrentTarget( NULL );
            }
            // error encountered
            ret = FALSE;
            // clean up
            goto cleanup;
        }

        // if target was an import (i.e., don't do this with the base target)
        if( sequence[i]->target->howMuch == te_do_import_only )
        {
            // move target from in-progress to imported in registry
            this->imports()->commit( sequence[i]->target );
        }
    }

cleanup:
    // 1.4.1.0 (ge) | added to unset the fileName reference, which determines
    // how messages print to console (e.g., [file.ck]: or [chuck]:)
    // 1.5.0.5 (ge) | changed to reset_parse() which does more reset
    // reset the parser, including reset filename and unset current chuck *
    reset_parse();

    // clear the in-progress list; should include the original target
    this->imports()->clearInProgress();

    return ret;
}




//-----------------------------------------------------------------------------
// name: generate_compile_sequence()
// desc: produce a compilation sequences of targets from a import dependency graph
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::generate_compile_sequence( ImportTargetNode * head,
                                                    std::vector<ImportTargetNode *> & sequence,
                                                    std::vector<ImportTargetNode *> & problems )
{
    // clear return vector
    sequence.clear();

    // sets of markings
    std::vector<Chuck_CompileTarget *> v;
    std::set<Chuck_CompileTarget *> permanent;
    std::set<Chuck_CompileTarget *> temporary;

    // topological sort
    if( !visit( head, sequence, permanent, temporary, problems ) ) return FALSE;

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: visit()
// desc: recursive componet of topological sort function
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::visit( ImportTargetNode * node,
                                std::vector<ImportTargetNode *> & sequence,
                                std::set<Chuck_CompileTarget *> & permanent,
                                std::set<Chuck_CompileTarget *> & temporary,
                                std::vector<ImportTargetNode *> & problems )
{
    // cycle
    t_CKBOOL acyclic = TRUE;
    // the target
    Chuck_CompileTarget * target = node->target;

    // if node already complete, can safely ignore
    if( target->state == te_compile_complete ) return TRUE;
    // is node marked permanent?
    if( permanent.find(target) != permanent.end() ) return TRUE;
    // is node marked temporary? if so, CYCLE detected
    if( temporary.find(target) != temporary.end() )
    { problems.push_back(node); return FALSE; }

    // add node to temporary
    temporary.insert(target);

    // loop over dependencies
    for( t_CKINT i = 0; i < target->dependencies.size(); i++ )
    {
        // visit dependency
        acyclic = visit( &(target->dependencies[i]), sequence, permanent, temporary, problems );
        // if cycle detected
        if( !acyclic )
        { problems.push_back(node); return FALSE; }
    }

    // add
    permanent.insert(target);
    // append node (should be prepend, but we will reverse later)
    sequence.push_back(node);

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: matchFilename()
// desc: test for filename match; if 'ext' is empty, will test with extensions
//       this function updates 'filename' with the first match
//-----------------------------------------------------------------------------
static t_CKBOOL matchFilename( std::string & filename,
                               const std::string & ext,
                               const std::vector<std::string> & extensions )
{
    // test filename as is
    if( ck_fileexists( filename ) && !ck_isdir( filename ) ) return TRUE;

    // if no extension
    if( ext == "" )
    {
        // go over the extensions
        for( t_CKINT i = 0; i < extensions.size(); i++ )
        {
            // concat
            string f = filename + extensions[i];
            // test for match
            if( ck_fileexists(f) && !ck_isdir( f ) )
            {
                // set
                filename = f;
                // done
                return TRUE;
            }
        }
    }
    // if no match and has .chug extension
    else if( ext == ".chug" )
    {
        // try appending .wasm
        string f = filename + ".wasm";
        // test for match
        if( ck_fileexists(f) && !ck_isdir( f ) )
        {
            // set
            filename = f;
            // done
            return TRUE;
        }
    }

    // no match
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: resolveFilename()
// desc: resolve filename into an absolutePath (see header file for more desc)
//-----------------------------------------------------------------------------
std::string Chuck_Compiler::resolveFilename( const std::string & filename,
                                             const std::string & importerAbsolutePath,
                                             t_CKBOOL expandSearchToGlobal )
{
    // return value
    string absolutePath = "";
    // trim, expand, normalize to single /
    string fname = normalize_directory_separator(expand_filepath(trim(filename)));
    // whether fname is already an absolute path
    t_CKBOOL isAlreadyAbsolutePath = is_absolute_path( fname );
    // current extension, e.g., ".ck" ".chug" ".chug.wasm"
    string extension = tolower(extract_filepath_ext( fname ));
    // extensions to test (if none is present)
    vector<string> exts; exts.push_back(".ck"); exts.push_back(".chug"); exts.push_back(".chug.wasm");

    // log
    EM_log( CK_LOG_FINE, "resolving filename: '%s' importer: '%s' expand-search: %s",
            filename.c_str(), importerAbsolutePath.c_str(), expandSearchToGlobal ? "YES" : "NO" );

    // check if already absolute path
    if( isAlreadyAbsolutePath )
    {
        // if already absolute path, do not expand search
        expandSearchToGlobal = FALSE;
        // tentative set as filename to open
        absolutePath = fname;
    }
    else // not already absolute path, create tentative absolute path
    {
        // if this is part of an @import
        if( importerAbsolutePath != "" )
        {
            // the importer is a file; transplant filename
            absolutePath = transplant_filepath( importerAbsolutePath, fname );
        }
        else // otherwise, construct path from working directory
        {
            // use current working directory
            absolutePath = carrier()->chuck->getParamString(CHUCK_PARAM_WORKING_DIRECTORY) + fname;
        }
    }

    // protocol for filename resolution:
    // |- test file existence + directory:
    //    |- use filename as is (e.g., "Foo.ck" or "Foo")
    //    |- failing that, try again with appended extensions
    //       |- if no current extension, try .ck, .chug, .chug.wasm
    //       |- if filename is .chug, try with .wasm
    // |- if still no resolution at this point, and if expandSearchToGlobal,
    //    move to next directory in search path

    // test for match
    t_CKBOOL hasMatch = matchFilename( absolutePath, extension, exts );
    // need for expanded search
    if( !isAlreadyAbsolutePath && !hasMatch && expandSearchToGlobal )
    {
        // TODO: possible caching -- search for match first in registry
        // match-right between each entry and fname; if found, return entry absolute path

        // search paths: start with system paths
        std::list<std::string> searchPaths = this->carrier()->chuck->getParamStringList( CHUCK_PARAM_IMPORT_PATH_SYSTEM );
        // next, process packages paths (e.g., as managed by ChuMP)
        std::list<std::string> packages_paths = this->carrier()->chuck->getParamStringList( CHUCK_PARAM_IMPORT_PATH_PACKAGES);
        // append packages paths to search paths
        append_path_list( searchPaths, packages_paths );
        // iterate over packages paths | 1.5.4.1 (ge & nshaheed) added
        for( std::list<std::string>::iterator it = packages_paths.begin(); it != packages_paths.end(); it++ )
        {
            // scan for subdirs, but only one-level in each packages path
            scan_for_dirs_in_directory( *it, "", FALSE, searchPaths );
        }
        // finally, add user-managed search paths (no auto-load; all must be @imported)
        append_path_list( searchPaths, this->carrier()->chuck->getParamStringList( CHUCK_PARAM_IMPORT_PATH_USER) );

        // go over paths
        for( list<string>::iterator it = searchPaths.begin(); it != searchPaths.end(); it++ )
        {
            // construct path; expand path again here in case search path has things like ~
            // make sure to get_full_path on the directory itself before appending fname,
            // FYI get_full_path() verifies presence of file, fname could be without extension e.g., @import "Foo"
            absolutePath = get_full_path( expand_filepath(*it), TRUE ) + fname;
            // try to match, by known extensions if none specified
            hasMatch = matchFilename( absolutePath, extension, exts );
            // log
            EM_log( CK_LOG_FINER, "testing match: '%s' ('%s')", absolutePath.c_str(), hasMatch ? "yes" : "no" );
            // if match found, break out
            if( hasMatch ) break;
        }
    }

    // log
    EM_pushlog();
    if( hasMatch ) EM_log( CK_LOG_FINE, "resolved: '%s'", absolutePath.c_str() );
    else EM_log( CK_LOG_FINE, "filename '%s' unresolved", filename.c_str() );
    EM_poplog();

    // return value
    return hasMatch ? absolutePath : "";
}




//-----------------------------------------------------------------------------
// name: compile_entire_file()
// desc: scan, type-check, and emit a program
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile_entire_file( Chuck_Context * context )
{
    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env(), context->parse_tree, te_do_all ) )
         return FALSE;

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), context->parse_tree, te_do_all ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), context->parse_tree, te_do_all ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_do_all ) )
        return FALSE;

    // emit (pass 4)
    this->code = emit_engine_emit_prog( emitter, context->parse_tree, te_do_all );
    if( !code ) return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL_DONE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: compile_import_only() // 1.5.2.5 (ge) added
// desc: import only public definitions (classes and operator overloads)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile_import_only( Chuck_Context * context )
{
    // set the state of the context to done
    context->progress = Chuck_Context::P_IMPORTING;

    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env(), context->parse_tree, te_do_import_only ) )
         return FALSE;

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), context->parse_tree, te_do_import_only ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), context->parse_tree, te_do_import_only ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_do_import_only ) )
        return FALSE;

    // emit (pass 4)
    if( !emit_engine_emit_prog( emitter, context->parse_tree, te_do_import_only ) )
        return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_IMPORTED;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: compile_all_except_import()
// desc: compile everything except public definitions
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile_all_except_import( Chuck_Context * context )
{
    // 0th scan only deals with classes, so is not needed

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), context->parse_tree, te_skip_import ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), context->parse_tree, te_skip_import ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_skip_import ) )
        return FALSE;

    // emit (pass 4)
    code = emit_engine_emit_prog( emitter, context->parse_tree, te_skip_import );
    if( !code ) return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL_DONE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan_import()
// desc: scan for a list of imports
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan_import( Chuck_Env * env, a_Stmt_List stmt_list,
                                  Chuck_CompileTarget * target,
                                  Chuck_Compiler * compiler )
{
    // type check the stmt_list
    while( stmt_list )
    {
        // the current statement
        if( stmt_list->stmt && stmt_list->stmt->s_type == ae_stmt_import )
        {
            // get the import list
            a_Import import = stmt_list->stmt->stmt_import.list;
            // loop over import list
            while( import )
            {
                // resolve, using importer's absolute path, expandSearch == TRUE
                string abs = compiler->resolveFilename( import->what, target->absolutePath, TRUE );
                // if cannot resolve filename
                if( abs == "" )
                {
                    // print error
                    EM_error2( import->where, "no such file: '%s'", import->what );
                    // done
                    return FALSE;
                }

                // lookup to see there is already a target
                Chuck_CompileTarget * t = compiler->imports()->lookup( abs, target );
                if( !t )
                {
                    // the filename (without full path)
                    string theFile = extract_filepath_file(abs);
                    // get extension
                    string ext = tolower(extract_filepath_ext(abs));
                    // error string for reporting, in case of error
                    string errorStr;
                    // test extension
                    if( ext == ".chug" || ext == ".wasm" )
                    {
                        // load the chugin, in its own namespace == TRUE
                        if( !compiler->importChugin( abs, TRUE, theFile, errorStr ) )
                        {
                            // print error (chugin loading only prints to log)
                            EM_error2( import->where, "cannot load chugin: '%s'...", theFile.c_str() );
                            EM_error2( 0, "...(reason) %s", errorStr.length() ? errorStr.c_str() : "[none provided, unhelpfully (try running with more verbose log-level)]" );
                            EM_error2( 0, "...in file '%s'", abs.c_str() );
                            // error encountered in chugin load, bailing out
                            return FALSE;
                        }

                        // get the target
                        t = compiler->imports()->lookup( abs, target );
                        // check
                        if( !t )
                        {
                            // print error (chugin loading only prints to log)
                            EM_error2( 0, "(internal error) cannot locate imported chugin..." );
                            EM_error2( 0, " |- location: '%s'", abs.c_str() );
                            // error encountered in chugin load, bailing out
                            return FALSE;
                        }
                    }
                    else
                    {
                        // make new target with import only
                        t = new Chuck_CompileTarget( te_do_import_only );
                        // set filename, with transplant path, expand search if necessary
                        if( !compiler->openFile( t, import->what, target, TRUE, import->where ) )
                        {
                            // clean up
                            CK_SAFE_DELETE( t );
                            // error encountered, bailing out
                            return FALSE;
                        }
                        // add to registry's in-progress list
                        compiler->imports()->addInProgress( t );
                    }
                }
                // add to target
                target->dependencies.push_back( ImportTargetNode( t, import->where, import->line ) );
                // next in list
                import = import->next;
            }
        }

        // advance to the next statement
        stmt_list = stmt_list->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan_import()
// desc: scan for a list of imports
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan_import( Chuck_Env * env,
                                  Chuck_CompileTarget * target,
                                  Chuck_Compiler * compiler )
{
    t_CKBOOL ret = TRUE;

    // get AST
    a_Program prog = target->AST;
    if( !prog ) return FALSE;

    // clear target dependencies
    target->dependencies.clear();

    // push indent
    // EM_pushlog();

    // go through each of the program sections
    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            // scan the statements
            ret = type_engine_scan_import( env, prog->section->stmt_list, target, compiler );
            break;

        case ae_section_func:
            break;

        case ae_section_class:
            break;

        default:
            EM_error2( prog->where,
                "(internal error) unrecognized program section in type checker pre-scan..." );
            ret = FALSE;
            break;
        }

        // next section
        prog = prog->next;
    }

    // pop indent
    // EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: scan_imports()
// desc: scan for @import statements; return a list of resulting import targets
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::scan_imports( Chuck_Env * env,
                                       Chuck_CompileTarget * target )
{
    return type_engine_scan_import( env, target, this );
}




//-----------------------------------------------------------------------------
// name: scan_imports()
// desc: scan for imports
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::scan_imports( Chuck_CompileTarget * target )
{
    // return value
    t_CKBOOL ret = TRUE;

    // set as current target
    EM_setCurrentTarget( target );

    // parse the code
    if( !chuck_parse( target ) )
    {
        // error encountered
        ret = FALSE;
        // clean up
        goto cleanup;
    }

    // set the AST
    target->AST = g_program;

    // log
    EM_log( CK_LOG_INFO, "@import scanning within target '%s'...", target->filename.c_str() );

    // add the current file name to targets (to avoid duplicates/cycles)
    // scan for @import statements
    if( !scan_imports( env(), target ) )
    {
        // error encountered
        ret = FALSE;
        // bail out
        goto cleanup;
    }

    // push log
    EM_pushlog();
    // loop over dependencies
    for( int i = 0; i < target->dependencies.size(); i++ )
    {
        // current dependency
        Chuck_CompileTarget * t = target->dependencies[i].target;

        // check if the target has been successfully compiled already
        // (if so, we can assume that its dependencies has already been resolved)
        // (if not, scan for imports if it is not a chugin and does yet have an AST)
        if( t->state != te_compile_complete && t->chugin == NULL && t->AST == NULL )
        {
            // log
            EM_log( CK_LOG_FINE, "@import dependency found: '%s'", t->absolutePath.c_str() );
            // parse for scan import on imported file
            if( !scan_imports( t ) )
            {
                // error encountered
                ret = FALSE;
                // bail out; break here to make sure poplog() is run
                break;
            }
        }
    }
    // pop log
    EM_poplog();

cleanup:
    // null current target
    EM_setCurrentTarget( NULL );

    // done
    return ret;
}




//-----------------------------------------------------------------------------
// name: compile_single()
// desc: scan, type-check, emit a single target (post parse and post import)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::compile_single( Chuck_CompileTarget * target )
{
    t_CKBOOL ret = TRUE;

    // clear any error messages
    EM_reset_msg();
    // make the target current in the parser
    EM_setCurrentTarget( target );

    // make the context
    Chuck_Context * context = type_engine_make_context( target->AST, target->filename );
    if( !context ) return FALSE;

    // remember full path (added 1.3.0.0)
    context->full_path = target->absolutePath;

    // reset the env
    env()->reset();

    // load the context
    if( !type_engine_load_context( env(), context ) )
        return FALSE;

    // compile
    if( target->howMuch == te_do_all )
    {
        if( !compile_entire_file( context ) )
        { ret = FALSE; goto cleanup; }
    }
    else if( target->howMuch == te_do_import_only )
    {
        if( !compile_import_only( context ) )
        { ret = FALSE; goto cleanup; }
    }
    else if( target->howMuch == te_skip_import )
    {
        if( !compile_all_except_import( context ) )
        { ret = FALSE; goto cleanup; }
    }

cleanup:

    // make the target current in the parser
    EM_setCurrentTarget( NULL );

    // commit
    if( ret ) // 1.5.4.3 (ge) update to call commit_namespaces | was: env()->global()->commit();
    { env()->commit_namespaces(); }
    // or rollback
    else // 1.5.4.3 (ge) update to call commit_namespaces | was: env()->global()->rollback();
    { env()->rollback_namespaces(); }

    // unload the context from the type-checker
    if( !type_engine_unload_context( env() ) )
    {
        EM_error2( 0, "(internal error) unloading context..." );
        return FALSE;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: output()
// desc: get the code generated by the last do()
//-----------------------------------------------------------------------------
Chuck_VM_Code * Chuck_Compiler::output()
{
    return this->code;
}




//-----------------------------------------------------------------------------
// name: setReplaceDac()
// desc: tell the compiler whether dac should be replaced in scripts
//       with the name of an external UGen, and if so which one
//-----------------------------------------------------------------------------
void Chuck_Compiler::setReplaceDac( t_CKBOOL shouldReplaceDac,
                                    const string & replacement )
{
    emitter->should_replace_dac = shouldReplaceDac;
    emitter->dac_replacement = replacement;
}



//-----------------------------------------------------------------------------
// name: bind()
// desc: bind a new type system module, via query function
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::bind( f_ck_query query_func, const string & name,
                               const string & nspc )
{
    // log
    EM_log( CK_LOG_SYSTEM, "on-demand binding compiler module '%s'...",
            name.c_str() );
    // push indent level
    EM_pushlog();

    // get env
    Chuck_Env * env = this->env();
    // make context
    Chuck_Context * context = type_engine_make_context(
        NULL, string("@[bind:") + name + string("]") );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // do it
    if( !load_module( this, env, query_func, name.c_str(), nspc.c_str() ) ) goto error;

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    // pop indent level
    EM_poplog();

    return TRUE;

error:

    // probably dangerous: rollback
    env->global()->rollback();

    // clear context
    type_engine_unload_context( env );

    // pop indent level
    EM_poplog();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: load_module()
// desc: load an internal module and add it to the type system
//-----------------------------------------------------------------------------
t_CKBOOL load_module( Chuck_Compiler * compiler, Chuck_Env * env, f_ck_query query,
                      const char * name, const char * nspc )
{
    Chuck_DLL * dll = NULL;
    t_CKBOOL query_failed = FALSE;

    // create a DLL as a module (here DLL is not actually dynamically loaded
    // from disk, but rather is a construct that contains a query function
    dll = new Chuck_DLL( compiler->carrier(), name );
    // (fixed: 1.3.0.0) query_failed now catches either failure of load or query
    query_failed = !(dll->load( query ) && dll->query());
    // check and add contents to type system
    if( query_failed || !type_engine_add_dll( env, dll, nspc ) )
    {
        // print error
        EM_error2( 0, "internal error loading module '%s.%s'...", nspc, name );
        // print error if it came from DLL query
        if( query_failed ) { EM_error2( 0, "...(reason: %s)", dll->last_error() ); }
        // done
        return FALSE;
    }

    // TODO: SAFE_DELETE( dll ) -- possibly in both error and success cases?
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_internal_modules()
// desc: load internal base modules
//-----------------------------------------------------------------------------
t_CKBOOL load_internal_modules( Chuck_Compiler * compiler )
{
    // log
    EM_log( CK_LOG_HERALD, "loading built-in modules..." );
    // push indent level
    EM_pushlog();

    // get env
    Chuck_Env * env = compiler->env();
    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[internal]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // still compile MidiMsg even if __DISABLE_MIDI__
    if( !init_class_Midi( env ) ) goto error;
#ifndef __DISABLE_MIDI__
    if( !init_class_MidiRW( env ) ) goto error;
#endif // __DISABLE_MIDI__

    // load
    EM_log( CK_LOG_HERALD, "module 'math'" );
    if( !load_module( compiler, env, libmath_query, "math", "global" ) ) goto error;
    EM_log( CK_LOG_HERALD, "module 'osc'" );
    load_module( compiler, env, osc_query, "osc", "global" );
    EM_log( CK_LOG_HERALD, "module 'ai'" );
    if( !load_module( compiler, env, libai_query, "ai", "global" ) ) goto error;
    EM_log( CK_LOG_HERALD, "module 'extract'" );
    load_module( compiler, env, extract_query, "extract", "global" );
    EM_log( CK_LOG_HERALD, "module 'filter'" );
    load_module( compiler, env, filter_query, "filter", "global" );
    EM_log( CK_LOG_HERALD, "module 'STK'" );
    load_module( compiler, env, stk_query, "stk", "global" );
    EM_log( CK_LOG_HERALD, "module 'xform'" );
    load_module( compiler, env, xform_query, "xform", "global" );
    EM_log( CK_LOG_HERALD, "module 'xxx'" );
    load_module( compiler, env, xxx_query, "xxx", "global" );
    EM_log( CK_LOG_HERALD, "module 'std'" );
    if( !load_module( compiler, env, libstd_query, "std", "global" ) ) goto error;

    // load
    EM_log( CK_LOG_HERALD, "module 'machine'" );
    if( !load_module( compiler, env, machine_query, "machine", "global" ) ) goto error;

    #ifndef __DISABLE_NETWORK__
    EM_log( CK_LOG_HERALD, "module 'opsc'" );
    if( !load_module( compiler, env, opensoundcontrol_query, "opsc", "global" ) ) goto error;
    #endif

    // deprecated:
    // if( !load_module( compiler, env, net_query, "net", "global" ) ) goto error;

    #ifndef __DISABLE_HID__
    EM_log( CK_LOG_HERALD, "module 'HID'" );
    if( !init_class_HID( env ) ) goto error;
    #endif

    #ifndef __DISABLE_SERIAL__
    EM_log( CK_LOG_HERALD, "module 'SerialIO'" );
    if( !init_class_serialio( env ) ) goto error;
    #endif

    EM_log( CK_LOG_HERALD, "module 'CKDoc'" );
    if( !load_module( compiler, env, ckdoc_query, "ckdoc", "global" ) ) goto error;

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    // preserve all operator overloads currently in registry | 1.5.1.5
    env->op_registry.preserve();

    // pop indent level
    EM_poplog();

    return TRUE;

error:
    // probably dangerous: rollback
    env->global()->rollback();

    // undo op registry entries | 1.5.1.5
    env->op_registry.reset2local();

    // clear context
    type_engine_unload_context( env );

    // pop indent level
    EM_poplog();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: getDirEntryAttribute()
// desc: get directory entry attribute | 1.5.0.4 (ge) moved into function
//-----------------------------------------------------------------------------
t_CKBOOL getDirEntryAttribute( dirent * de, t_CKBOOL & isDirectory, t_CKBOOL & isRegular )
{
#if defined(__PLATFORM_WINDOWS__)
    isDirectory = de->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    isRegular = ((de->data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ||
                  (de->data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ||
                  (de->data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE));
#elif defined(__WINDOWS_PTHREAD__) // Cygwin -- doesn't have dirent d_type
    string absolute_path = string(directory) + "/" + de->d_name;
    struct stat st;
    if( stat(absolute_path.c_str(), &st) == 0 )
    {
        isDirectory = st.st_mode & S_IFDIR;
        isRegular = st.st_mode & S_IFREG;
    }
    else
    {
        // uhh ...
        EM_log( CK_LOG_INFO,
               "unable to open file '%s', ignoring for chugins",
               absolute_path.c_str() );
        return false;
    }
#else
    isDirectory = de->d_type == DT_DIR;
    isRegular = de->d_type == DT_REG;
#endif

    return true;
}




//-----------------------------------------------------------------------------
// name: struct ChuginFileInfo()
// desc: file info for loading
//-----------------------------------------------------------------------------
struct ChuginFileInfo
{
    string filename;
    string path;
    bool isBundle;

    // constructor
    ChuginFileInfo() { isBundle = false;  }

    // constructor
    ChuginFileInfo( const string & f, const string & p, bool bundle = false )
    {
        filename = f; path = p; isBundle = bundle;
    }
};
// compare function for sorting ChuginFileInfo
static bool comp_func_chuginfileinfo( const ChuginFileInfo & a, const ChuginFileInfo & b )
{
    return tolower( a.filename ) < tolower( b.filename );
}




//-----------------------------------------------------------------------------
// name: format_dir_name_for_display()
// desc: format directory name for display, considering underlying platform
//-----------------------------------------------------------------------------
static string format_dir_name_for_display( const string & path )
{
#ifndef __PLATFORM_WINDOWS__
    return path;
#else // __PLATFORM_WINDOWS__
    return expand_filepath( path );
#endif
}



//-----------------------------------------------------------------------------
// name: scan_external_modules_in_directory()
// desc: scan all external modules by extension within a directory
//-----------------------------------------------------------------------------
t_CKBOOL scan_external_modules_in_directory( const string & directory,
    const string & extension,
    t_CKBOOL recursiveSearch,
    vector<ChuginFileInfo> & chugins2load,
    vector<string> & dirs2search,
    vector<string> & ckfiles2load )
{
    // expand directory path
    string path = expand_filepath( directory, FALSE );
    // normalize path
    path = normalize_directory_name( path );
    // open the directory
    DIR * dir = opendir( path.c_str() );

    // cannot open
    if( !dir ) return FALSE;

    // while( (de = readdir(dir)) ) <- UE5 forces us to not do this
    // iterate over directory entries | 1.5.0.0 (ge + eito) #chunreal
    for( struct dirent * de = readdir( dir ); de != NULL; de = readdir(dir) )
    {
        t_CKBOOL is_regular = false;
        t_CKBOOL is_directory = false;
        // get attributes
        if( !getDirEntryAttribute( de, is_directory, is_regular ) ) continue;

        // check | TODO: follow links?
        if( is_regular )
        {
            // check for ck files to pre-load
            if( extension_matches( de->d_name, ".ck" ) )
            {
                // construct absolute path
                string absolute_path = path + de->d_name;
                // append file path
                ckfiles2load.push_back( absolute_path );
            }
            // check extension passed in
            else if( extension_matches( de->d_name, extension ) )
            {
                // construct absolute path
                string absolute_path = path + de->d_name;
                // append module
                chugins2load.push_back( ChuginFileInfo( de->d_name, absolute_path ) );
            }
        }
        // recursive search? into directories...
        // (whose name doesn't end in .chug, unless it was precisely .chug)
        else if( recursiveSearch && is_directory && subdir_ok2recurse( de->d_name, extension ) )
        {
            // recurse | TODO: max depth?
            if( strncmp( de->d_name, ".", sizeof( "." ) ) != 0 &&
                strncmp( de->d_name, "..", sizeof( ".." ) ) != 0 )
            {
                // construct absolute path (use the non-expanded path for consistency when printing)
                string absolute_path = string(directory) + de->d_name;
                // queue search in sub-directory
                dirs2search.push_back( absolute_path );
            }
        }
#ifdef __PLATFORM_APPLE__
        else if( is_directory && extension_matches( de->d_name, extension ) )
        {
            // On macOS, a chugin can either be a regular file (a .dylib simply renamed to .chug)
            // or it can be a MODULE like using CMake with add_library(FooChugin MODULE foo.cpp)
            // If it's a module bundle, then because .chug is a nonstandard extension,
            // on the filesystem it shows up as a directory ending in .chug.
            // If we see one of these directories, we can dive directly to the Contents/MacOS subfolder
            // and look for a regular file. | 1.5.0.1 (dbraun) added
            string absolute_path = path + "/" + de->d_name + "/Contents/MacOS";
            // probe NAME.chug/Contents/MacOS/NAME
            string actualName = extension_removed( de->d_name, extension );
            // construct
            absolute_path += "/" + actualName;
            // get cstr
            const char * subdirectory = absolute_path.c_str();
            // load directly
            chugins2load.push_back( ChuginFileInfo( de->d_name, subdirectory, true ) );
        }
#endif // #ifdef __PLATFORM_APPLE__
    }

    // close
    closedir( dir );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: scan_for_dirs_in_directory() | 1.5.4.1 (ge & nshaheed) added
// desc: scan all subdirectories within a directory
//-----------------------------------------------------------------------------
t_CKBOOL scan_for_dirs_in_directory( const string & directory,
                                     const string & extensionForSubdirTest,
                                     t_CKBOOL recursiveSearch,
                                     list<string> & results )
{
    // expand directory path
    string path = normalize_directory_name( expand_filepath( directory, FALSE ) );
    // open the directory
    DIR * dir = opendir( path.c_str() );
    // cannot open
    if( !dir ) return FALSE;

    // local results
    list<string> localResults;

    // do first read | 1.5.0.0 (ge + eito) #chunreal
    struct dirent * de = readdir( dir );
    // while( (de = readdir(dir)) ) <- UE5 forces us to not do this
    for( de = readdir(dir); de != NULL; de = readdir(dir) )
    {
        t_CKBOOL is_regularFile = false;
        t_CKBOOL is_directory = false;
        // get attributes
        if( !getDirEntryAttribute( de, is_directory, is_regularFile ) ) continue;

        // check if directory
        if( is_directory )
        {
            // test for special cases (e.g., chugins that are directories)
            if( extensionForSubdirTest.length() && !subdir_ok2recurse( de->d_name, extensionForSubdirTest ) ) continue;

            // check dir entry
            if( strncmp( de->d_name, ".", sizeof( "." ) ) != 0 &&
                strncmp( de->d_name, "..", sizeof( ".." ) ) != 0 )
            {
                // construct absolute path (use the non-expanded path for consistency when printing)
                string absolute_path = string(path) + de->d_name;
                // queue sub-directory
                localResults.push_back( normalize_directory_name(absolute_path) );
            }
        }
    }

    // close
    closedir( dir );

    // sort the local results
    localResults.sort();
    // copy local results
    append_path_list( results, localResults );

    // recurse?
    if( recursiveSearch )
    {
        // iterate over local directoriesug
        for( list<string>::iterator it = localResults.begin(); it != localResults.end(); it++ )
        {
            // scan subdirs in this local dir
            scan_for_dirs_in_directory( *it, extensionForSubdirTest, recursiveSearch, results );
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: logChuginLoad() | 1.5.2.5 (ge)
// desc: local function quick-hand for logging a chugin load
//-----------------------------------------------------------------------------
void logChuginLoad( const string & name, t_CKINT logLevel )
{
    // log with no newline; print `[chugin] X.chug`
    EM_log_opts( logLevel, EM_LOG_NO_NEWLINE, "[%s] %s ", TC::magenta("chugin",true).c_str(), name.c_str() );
}




//-----------------------------------------------------------------------------
// name: logCKFileFound() | 1.5.4.1 (ge & nshaheed)
// desc: local function quick-hand for logging a chuck file found
//-----------------------------------------------------------------------------
void logCKFileFound( const string & name, t_CKINT logLevel )
{
    // log with no newline; print `[chuck file] X.ck`
    EM_log_opts( logLevel, EM_LOG_NO_NEWLINE, "[%s] %s ", TC::blue("chuck file",true).c_str(), name.c_str() );

    // print success status
    EM_log_opts( logLevel, EM_LOG_NO_PREFIX, "[%s]", TC::green("FOUND",true).c_str() );
}




//-----------------------------------------------------------------------------
// name: import_chugin_opt()
// desc: load chugin module by path, with options
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::import_chugin_opt( const string & path, const string & name, string & errorStr )
{
    // get env
    Chuck_Env * env = this->env();

    // NOTE this (verbose >= 5) is more informative if the chugin crashes, we can see the name
    EM_log( CK_LOG_INFO, "@import loading [chugin] %s...", name.c_str() );
    EM_log( CK_LOG_FINE, " |- path: '%s'", path.c_str() );

    // create chuck DLL data structure
    Chuck_DLL * dll = new Chuck_DLL( this->carrier(), name != "" ? name.c_str() : (extract_filepath_file(path)).c_str() );
    t_CKBOOL query_failed = FALSE;

    // clear error string
    errorStr = "";

    // load (but don't query yet; lazy mode == TRUE)
    if( dll->load( path.c_str(), CK_QUERY_FUNC, TRUE) )
    {
        // probe it
        dll->probe();
        // if not compatible
        if( !dll->compatible() )
        {
            // print `[chugin] X.chug`
            logChuginLoad( name, CK_LOG_HERALD );
            // print
            EM_log_opts( CK_LOG_HERALD, EM_LOG_NO_PREFIX, "[%s] (API version: %d.%d)", TC::red("FAILED",true).c_str(), dll->versionMajor(), dll->versionMinor() );
            // push
            EM_pushlog();
            EM_log( CK_LOG_HERALD, "reason: %s", TC::orange(dll->last_error(),true).c_str() );
            EM_poplog();
            // set error string
            errorStr = dll->last_error();
            // go to error for cleanup
            goto error;
        }

        // try to query the chugin
        query_failed = !dll->query();
        // try to add to type system
        if( query_failed || !type_engine_add_dll2( env, dll, "global" ) )
        {
            // print `[chugin] X.chug`
            logChuginLoad( name, CK_LOG_HERALD );
            // print
            EM_log_opts( CK_LOG_HERALD, EM_LOG_NO_PREFIX, "[%s] (API version: %d.%d)", TC::red("FAILED",true).c_str(), dll->versionMajor(), dll->versionMinor() );
            EM_pushlog();
            // if add_dll2 failed, an error should have already been output
            if( query_failed )
            {
                // print reason
                EM_log( CK_LOG_HERALD, "reason: %s", TC::orange(dll->last_error(),true).c_str() );
            }
            EM_log( CK_LOG_HERALD, "%s '%s'...", TC::blue("skipping",true).c_str(), path.c_str() );
            EM_poplog();
            // set error string
            errorStr = dll->last_error();
            // go to error for cleanup
            goto error;
        }
    }
    else
    {
        // print `[chugin] X.chug`
        logChuginLoad( name, CK_LOG_HERALD );
        // print error status
        EM_log_opts( CK_LOG_HERALD, EM_LOG_NO_PREFIX, "[%s]", TC::red("FAILED",true).c_str() );
        // more info
        EM_pushlog();
        EM_log( CK_LOG_HERALD, "reason: %s", TC::orange(dll->last_error(),true).c_str() );
        EM_poplog();
        // set error string
        errorStr = dll->last_error();
        // go to error for cleanup
        goto error;
    }

    // print `[chugin] X.chug`
    logChuginLoad( name, CK_LOG_HERALD );
    // print success status
    EM_log_opts( CK_LOG_HERALD, EM_LOG_NO_PREFIX, "[%s]", TC::green("OK",true).c_str() );
    // add to registry
    m_importRegistry.commit( dll );
    // commit operator overloads | 1.5.1.5
    env->op_registry.preserve();

    // return home successful
    return TRUE;

error:
    // clean up
    CK_SAFE_DELETE( dll );
    // rollback operator overloads | 1.5.1.5
    env->op_registry.reset2local();

    // return home :(
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: load_external_modules_in_directory()
// desc: load all external modules by extension within a directory
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::load_external_modules_in_directory(
    const string & directory,
    const string & extension,
    t_CKBOOL recursiveSearch )
{
    vector<ChuginFileInfo> chugins2load;
    vector<string> dirs2search;
    vector<string> ckfiles2load;
    string errorStr;

    // print directory to examine
    EM_log( CK_LOG_HERALD, "searching '%s'", format_dir_name_for_display(directory).c_str() );
    // push
    EM_pushlog();

    // scan
    t_CKBOOL retval = scan_external_modules_in_directory( directory, extension, recursiveSearch, chugins2load, dirs2search, ckfiles2load );
    if( !retval )
    {
        // log (1.3.1.2: changed to 2 lines to stay within 80 chars)
        EM_log( CK_LOG_INFO, "unable to open directory..." );
        EM_log( CK_LOG_INFO, "ignoring for chugins..." );
        // pop
        EM_poplog();
        // go ahead and return true
        return TRUE;
    }

    // sort
    sort( chugins2load.begin(), chugins2load.end(), comp_func_chuginfileinfo );
    sort( dirs2search.begin(), dirs2search.end() );
    sort( ckfiles2load.begin(), ckfiles2load.end() );

    // loop over chugins to load
    for( t_CKINT i = 0; i < chugins2load.size(); i++ )
    {
        // load module
        // ...in its own namespace == FALSE | 1.5.4.0 (ge) added
        // ...since already in namespace (e.g., @[external]) from load_external_modules()
        t_CKBOOL loaded = this->importChugin( chugins2load[i].path, FALSE, chugins2load[i].filename, errorStr );
        // if no error
        if( chugins2load[i].isBundle && loaded) {
            // log
            EM_pushlog();
            EM_log( CK_LOG_INFO, "macOS bundle was detected..." );
            string shortenSubdir = chugins2load[i].path;
            shortenSubdir = shortenSubdir.substr( shortenSubdir.find( chugins2load[i].filename ) );
            EM_log( CK_LOG_INFO, "loaded %s", shortenSubdir.c_str() );
            EM_poplog();
        }
    }

    // loop over ck files to load (later)
    for( t_CKINT i = 0; i < ckfiles2load.size(); i++ )
    {
        // save for later
        this->m_cklibs_to_preload.push_back( ckfiles2load[i] );
    }

    // pop log
    EM_poplog();

    // loop over dirs2 to search | if not recursive, this should be empty
    for( t_CKINT i = 0; i < dirs2search.size(); i++ )
    {
        // search in dir
        load_external_modules_in_directory( dirs2search[i], extension, recursiveSearch );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_external_modules()
// desc: load external modules (e.g., .chug and .ck library files)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::load_external_modules( const string & extension,
                                                list<string> & chugin_search_paths,
                                                list<string> & named_dls,
                                                t_CKBOOL recursiveSearch )
{
    // get env
    Chuck_Env * env = this->env();
    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[external]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // first load dynamic libraries explicitly named on the command line
    for( list<string>::iterator i_dl = named_dls.begin();
         i_dl != named_dls.end(); i_dl++ )
    {
        // get chugin name
        std::string & dl_path = *i_dl;
        // error string
        std::string error_str;
        // expand the filepath (e.g., ~) | 1.5.2.6 (ge) added
        dl_path = expand_filepath( dl_path );
        // get full path | 1.5.4.2 (ge) added
        dl_path = get_full_path( dl_path );
        // check extension, append if no match
        if( !extension_matches( dl_path, extension ) )
            dl_path += extension;
        // load the module, in its own namespace == FALSE
        this->importChugin( dl_path, FALSE, mini(dl_path.c_str()), error_str );
    }

    // now recurse through search paths and load any .chug files found
    for( list<string>::iterator i_sp = chugin_search_paths.begin();
         i_sp != chugin_search_paths.end(); i_sp++ )
    {
        // expand the filepath (e.g., ~) | 1.5.2.6 (ge) added
        string dl_path = expand_filepath( *i_sp );
        // get full path | 1.5.4.2 (ge) added
        dl_path = get_full_path( dl_path, TRUE );
        // search directory and load contents
        load_external_modules_in_directory( dl_path, extension, recursiveSearch );
    }

    // clear context
    type_engine_unload_context( env );
    // commit what is in the type checker at this point
    env->global()->commit();
    // preserve all operator overloads currently in registry | 1.5.1.5
    env->op_registry.preserve();

    // always return true...
    // a failed chugin load should not prevent chuck from starting
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: probe_external_chugin()
// desc: probe external module by path
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::probe_external_chugin( const string & path, const string & theName )
{
    // name
    string name = theName != "" ? theName : extract_filepath_file(path);
    // create dynamic module
    Chuck_DLL * dll = new Chuck_DLL( NULL, name.c_str() );

    // NOTE this (verbose >= 5) is more informative if the chugin crashes, we can see the name
    EM_log( CK_LOG_INFO, "probing [chugin] %s...", name.c_str() );
    EM_log( CK_LOG_FINE, " |- path: '%s'", path.c_str() );

    // load the dll, lazy mode
    if( dll->load(path.c_str(), CK_QUERY_FUNC, TRUE) )
    {
        // probe it
        dll->probe();
        // if not compatible
        if( dll->compatible() )
        {
            // print `[chugin] X.chug`
            logChuginLoad( name, CK_LOG_SYSTEM );
            // print
            EM_log_opts( CK_LOG_SYSTEM, EM_LOG_NO_PREFIX, "[%s] (API version: %d.%d)", TC::green("OK",true).c_str(),
                         dll->versionMajor(), dll->versionMinor() );
        }
        else
        {
            // print `[chugin] X.chug`
            logChuginLoad( name, CK_LOG_SYSTEM );
            // print
            EM_log_opts( CK_LOG_SYSTEM, EM_LOG_NO_PREFIX, "[%s] (API version: %d.%d)", TC::red("FAILED",true).c_str(),
                         dll->versionMajor(), dll->versionMinor() );
            // push
            EM_pushlog();
            EM_log( CK_LOG_SYSTEM, "reason: %s", TC::orange(dll->last_error(),true).c_str() );
            EM_poplog();
        }

        // print info if available
        string dllName = dll->name();
        string dllPath = dll->filepath();
        string authors = dll->getinfo( CHUGIN_INFO_AUTHORS );
        string version = dll->getinfo( CHUGIN_INFO_CHUGIN_VERSION );
        string desc = dll->getinfo( CHUGIN_INFO_DESCRIPTION );
        string url = dll->getinfo( CHUGIN_INFO_URL );
        string email = dll->getinfo( CHUGIN_INFO_EMAIL );

        // print additional information
        if( authors.length() || version.length() || desc.length() || url.length() )
        {
            EM_pushlog();
            if( version.length() ) EM_log( CK_LOG_SYSTEM, "version: %s", version.c_str() );
            if( authors.length() ) EM_log( CK_LOG_SYSTEM, "author: %s", authors.c_str() );
            if( desc.length() ) EM_log( CK_LOG_HERALD, "description: %s", desc.c_str() );
            if( url.length() ) EM_log( CK_LOG_HERALD, "URL: %s", url.c_str() );
            if( email.length() ) EM_log( CK_LOG_INFO, "email: %s", email.c_str() );
            EM_poplog();
        }
    }
    else
    {
        // print `[chugin] X.chug`
        logChuginLoad( name, CK_LOG_SYSTEM );
        // print
        EM_log_opts( CK_LOG_SYSTEM, EM_LOG_NO_PREFIX, "[%s]...", TC::red("FAILED",true).c_str() );
        // more info
        EM_pushlog();
        EM_log( CK_LOG_SYSTEM, "reason: %s", TC::orange(dll->last_error(),true).c_str() );
        EM_poplog();
    }

    // clean up | 1.5.4.1 (ge) added
    CK_SAFE_DELETE( dll );

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: probe_external_modules_in_directory()
// desc: probe all external modules by extension within a directory
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::probe_external_modules_in_directory(
    const string & directory,
    const string & extension,
    t_CKBOOL recursiveSearch,
    list<string> & ck_libs )
{
    vector<ChuginFileInfo> chugins2load;
    vector<string> dirs2search;
    vector<string> ckfiles2load;

    // print directory to examine
    EM_log( CK_LOG_SYSTEM, "searching '%s'", format_dir_name_for_display(directory).c_str() );
    // push
    EM_pushlog();

    // scan
    t_CKBOOL retval = scan_external_modules_in_directory( directory, extension, recursiveSearch, chugins2load, dirs2search, ckfiles2load );
    if( !retval )
    {
        // log (1.3.1.2: changed to 2 lines to stay within 80 chars)
        EM_log( CK_LOG_INFO, "unable to open directory..." );
        EM_log( CK_LOG_INFO, "ignoring for chugins..." );
        // pop log
        EM_poplog();
        // go ahead and return true
        return TRUE;
    }

    // sort
    sort( chugins2load.begin(), chugins2load.end(), comp_func_chuginfileinfo );
    sort( dirs2search.begin(), dirs2search.end() );
    sort( ckfiles2load.begin(), ckfiles2load.end() );

    // loop over chugins to load
    for( t_CKINT i = 0; i < chugins2load.size(); i++ )
    {
        // load module
        t_CKBOOL probed = probe_external_chugin(
            chugins2load[i].path, chugins2load[i].filename );
        // if no error
        if( chugins2load[i].isBundle && probed) {
            // log
            EM_pushlog();
            EM_log( CK_LOG_INFO, "macOS bundle was detected..." );
            string shortenSubdir = chugins2load[i].path;
            shortenSubdir = shortenSubdir.substr( shortenSubdir.find( chugins2load[i].filename ) );
            EM_log( CK_LOG_INFO, "loaded %s", shortenSubdir.c_str() );
            EM_poplog();
        }
    }

    // loop over ck files to load (later)
    for( t_CKINT i = 0; i < ckfiles2load.size(); i++ )
    {
        // save for later
        ck_libs.push_back( ckfiles2load[i] );
    }

    // pop log
    EM_poplog();

    // loop over dirs2 to search | if not recursive, this should be empty
    for( t_CKINT i = 0; i < dirs2search.size(); i++ )
    {
        // search in dir
        probe_external_modules_in_directory( dirs2search[i], extension, recursiveSearch, ck_libs );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: probe_external_modules()
// desc: probe external modules (e.g., .chug and .ck library files)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::probe_external_modules( const string & extension,
                                                 list<string> & chugin_search_paths,
                                                 list<string> & named_dls,
                                                 t_CKBOOL recursiveSearch,
                                                 list<string> & ck_libs )
{
    // print
    EM_log( CK_LOG_SYSTEM, "probing specified chugins (e.g., via --chugin)..." );
    // push
    EM_pushlog();

    // first load dynamic libraries explicitly named on the command line
    for( list<string>::iterator i_dl = named_dls.begin();
         i_dl != named_dls.end(); i_dl++ )
    {
        // get chugin name
        std::string & dl_path = *i_dl;
        // expand the filepath (e.g., ~) | 1.5.2.6 (ge) added
        dl_path = expand_filepath( dl_path );
        // get full path | 1.5.4.2 (ge) added
        dl_path = get_full_path( dl_path );
        // check extension, append if no match
        if( !extension_matches( dl_path, extension ) )
            dl_path += extension;
        // load the module
        probe_external_chugin( dl_path, mini(dl_path.c_str()) );
    }

    // check
    if( named_dls.size() == 0 )
        EM_log( CK_LOG_INFO, "(none specified)" );

    // pop
    EM_poplog();
    // print
    EM_log( CK_LOG_SYSTEM, "probing chugins in search paths..." );
    // push
    EM_pushlog();

    // now recurse through search paths and note any .chug files found
    for( list<string>::iterator i_sp = chugin_search_paths.begin();
         i_sp != chugin_search_paths.end(); i_sp++ )
    {
        // expand the filepath (e.g., ~) | 1.5.2.6 (ge) added
        string dl_path = expand_filepath( *i_sp );
        // get full path | 1.5.4.2 (ge) added
        dl_path = get_full_path( dl_path, TRUE );
        // search directory and load contents
        probe_external_modules_in_directory( dl_path, extension, recursiveSearch, ck_libs );
    }

    // pop
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setAutoDepend()
// desc: auto dependency resolve for types
//-----------------------------------------------------------------------------
void Chuck_Compiler::setAutoDepend( t_CKBOOL v )
{
    // log
    EM_log( CK_LOG_SYSTEM, "type dependency resolution: %s",
            v ? "AUTO" : "MANUAL" );
    m_auto_depend = v;
}




//-----------------------------------------------------------------------------
// name: resolve()
// desc: resolve type automatically - if auto_depend is off, return FALSE
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::resolve( const string & type )
{
    t_CKBOOL ret = TRUE;

    // check auto_depend
    if( !m_auto_depend )
        return FALSE;

    // look up if name is already parsed

    return ret;
}




//-----------------------------------------------------------------------------
// name: Chuck_ImportRegistry()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_ImportRegistry::Chuck_ImportRegistry()
{ }




//-----------------------------------------------------------------------------
// name: lookupTarget()
// desc: look up if target exists by path (relative or full)
//-----------------------------------------------------------------------------
Chuck_CompileTarget * Chuck_ImportRegistry::lookup( const std::string & path,
                                                    Chuck_CompileTarget * importer )
{
    // ignore if not importing
    if( !importer ) return NULL;

    // return value
    Chuck_CompileTarget * ret = NULL;
    // search path
    std::string key = expand_filepath( path );

    // log
    EM_log( CK_LOG_FINER, "@import registry search: '%s' ", path.c_str() );

    // if importer is a file
    if( importer->filename != CHUCK_CODE_LITERAL_SIGNIFIER )
    {
        // transplant to absolute path, resolves to realpath
        key = transplant_filepath( importer->absolutePath, key );
    }
    else // if not, importer is code
    {
        // if not an absolute path
        if( !is_absolute_path( key ) )
        {
            // use the current working directory
            key = importer->the_chuck->getParamString( CHUCK_PARAM_WORKING_DIRECTORY ) + path;
        }
    }

    // push log
    EM_pushlog();

    // attempt to find in done list
    map<string, Chuck_CompileTarget *>::iterator it = m_importedTargets.find( key );
    // if not found
    if( it != m_importedTargets.end() )
    {
        ret = it->second;
        // log (if import only)
        EM_log( CK_LOG_FINER, "found (compiled): '%s'", key.c_str() );
    }

    // if not found
    if( !ret )
    {
        // attempt to find in in-progress list
        it = m_inProgressCKFiles.find( key );
        // if not found
        if( it != m_inProgressCKFiles.end() )
        {
            ret = it->second;
            // log
            EM_log( CK_LOG_FINER, "found (in-progress): '%s'", key.c_str() );
        }
    }

    // if still not found
    if( !ret )
    { EM_log( CK_LOG_FINER, "(not found)" ); }

    // pop log (if import only)
    EM_poplog();

    // return findings
    return ret;
}




//-----------------------------------------------------------------------------
// name: addTarget()
// desc: add a new compile target to import system
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_ImportRegistry::addInProgress( Chuck_CompileTarget * target )
{
    // check
    if( !target ) return FALSE;

    // cannot add if key already exists
    if( m_inProgressCKFiles.find( target->key() ) != m_inProgressCKFiles.end() )
    {
        // internal error; should have been verified before calling this function
        EM_error2( 0, "(import registry) cannot add target to in-progress list..." );
        EM_error2( 0, " |- '%s' already present...", target->key().c_str() );
        return FALSE;
    }

    // insert
    m_inProgressCKFiles[target->key()] = target;
    // return
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: commit()
// desc: move target from in progress to imported list
//-----------------------------------------------------------------------------
void Chuck_ImportRegistry::commit( Chuck_CompileTarget * target )
{
    // check
    if( !target ) return;

    // set state to COMPLETE
    target->state = te_compile_complete;
    // get key
    string key = target->key();

    // remove from in-progress list
    m_inProgressCKFiles.erase( key );

    // check if in map
    if( m_importedTargets.find( key ) != m_importedTargets.end() )
    {
        // warn
        EM_log( CK_LOG_WARNING, "import registry duplicate imported target: '%s'", key.c_str() );
    }

    // should be safe to cleanup AST and close any file handles
    target->cleanup();

    // insert into imported map
    m_importedTargets[key] = target;
}




//-----------------------------------------------------------------------------
// name: commit()
// desc: add a chugin to the imported list
//-----------------------------------------------------------------------------
Chuck_CompileTarget * Chuck_ImportRegistry::commit( Chuck_DLL * chugin )
{
    // check
    if( !chugin ) return NULL;

    // get key
    string key = chugin->filepath();

    // check if key in map
    if( m_importedTargets.find( key ) != m_importedTargets.end() )
    {
        // already there; return corresponding target
        return m_importedTargets.find( key )->second;
    }

    // insert into chugin map
    m_importedChugins[key] = chugin;

    // make new target for chugin
    Chuck_CompileTarget * t = new Chuck_CompileTarget( te_do_all );
    // set appropriate settings for chugin
    t->chugin = chugin;
    // is complete
    t->state = te_compile_complete;
    // all chugins are considered "system" (and will persists across VM clears)
    t->isSystemImport = TRUE;
    // set name to chugin short hand
    t->filename = extract_filepath_file(key);
    // set absolute path
    t->absolutePath = key;

    // insert into import map
    m_importedTargets[key] = t;

    // return the target
    return t;
}




//-----------------------------------------------------------------------------
// name: clearInProgress()
// desc: remove in-progress targets
//-----------------------------------------------------------------------------
void Chuck_ImportRegistry::clearInProgress()
{
    // iterator
    map<string, Chuck_CompileTarget *>::iterator it;
    // iterate
    for( it = m_inProgressCKFiles.begin(); it != m_inProgressCKFiles.end(); it++ )
    {
        // delete the target
        CK_SAFE_DELETE( it->second );
    }
    // clear map
    m_inProgressCKFiles.clear();
}




//-----------------------------------------------------------------------------
// name: clearAllUserImports()
// desc: remove all user imports (system imports unaffected)
//-----------------------------------------------------------------------------
void Chuck_ImportRegistry::clearAllUserImports()
{
    // clear in progress
    clearInProgress();

    // list of iterators to erase after iterating
    vector<string> itersToErase;

    // clear imported CK Files
    map<std::string, Chuck_CompileTarget *>::iterator iterT;
    for( iterT = m_importedTargets.begin(); iterT != m_importedTargets.end(); iterT++ )
    {
        if( iterT->second->isSystemImport == FALSE )
        {
            // delete each imported CK file
            CK_SAFE_DELETE( iterT->second );
            // add key to list to be erased after this loop
            itersToErase.push_back( iterT->first );
        }
    }

    // erase entries from map
    for( t_CKINT i = 0; i < itersToErase.size(); i++ )
    {
        // erase each entry by key
        m_importedTargets.erase( itersToErase[i] );
    }
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: remove all imports (system and user)
//-----------------------------------------------------------------------------
void Chuck_ImportRegistry::shutdown()
{
    // clear in progress
    clearInProgress();

    // clear imported CK Files
    map<std::string, Chuck_CompileTarget *>::iterator iterT;
    for( iterT = m_importedTargets.begin(); iterT != m_importedTargets.end(); iterT++ )
    {
        // delete each DLL
        CK_SAFE_DELETE( iterT->second );
    }
    // clear the map
    m_importedTargets.clear();

    // clean up imported chugins
    map<std::string, Chuck_DLL *>::iterator iterC;
    for( iterC = m_importedChugins.begin(); iterC != m_importedChugins.end(); iterC++ )
    {
        // delete each DLL
        CK_SAFE_DELETE( iterC->second );
    }
    // clear the list
    m_importedChugins.clear();
}




//-----------------------------------------------------------------------------
// name: cleanupAST()
// desc: clean up abstract syntax tree in target
//-----------------------------------------------------------------------------
void Chuck_CompileTarget::cleanupAST()
{
    // clean up abstract syntax tree
    if( AST )
    {
        // delete tree
        delete_program( AST );
        // null out
        AST = NULL;
    }
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: performs cleanup (e.g., close file descriptors and reclaims AST)
//       prepares target to be archived in import system after successful compilation
//-----------------------------------------------------------------------------
void Chuck_CompileTarget::cleanup()
{
    // clean up abstract syntax tree
    cleanupAST();

    // clean up the file source
    fileSource.reset();

    // if open
    if( fd2parse )
    {
        // close file descriptor
        fclose( fd2parse );
        // null out
        fd2parse = NULL;
    }

    // free the intList
    IntList curr = NULL;
    while( the_linePos )
    {
        curr = the_linePos;
        the_linePos = the_linePos->rest;
        // free
        free( curr );
    }
    // make new intList
    the_linePos = intList( 0, NULL );

    // reset line number
    lineNum = 1;
}




//-----------------------------------------------------------------------------
// name: openFile()
// desc: open a file
//   * resolve and set filename and absolutePath for a compile target
//   * set as filename (possibly with modifications, e.g., with .ck appended)
//   * if file is unresolved locally and `expandSearchToGlobal` is true,
//     expand search to global search paths
//   * the file is resolved this will open a FILE descriptor in fd2parse
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::openFile( Chuck_CompileTarget * target,
                                   const std::string & theFilename,
                                   Chuck_CompileTarget * importer,
                                   t_CKBOOL expandSearchToGlobal,
                                   t_CKINT wherePos )
{
    // clean up
    target->cleanup();

    // reset fields (in case of error)
    target->filename = "";
    target->absolutePath = "";
    target->codeLiteral = "";

    // resolve
    target->absolutePath = resolveFilename( theFilename, importer ? importer->absolutePath : "", expandSearchToGlobal );
    // if unresolved
    if( target->absolutePath == "" )
    {
        // print error
        EM_error2( wherePos, "no such file: '%s'", mini( theFilename.c_str() ) );
        // done
        return FALSE;
    }

    // open file
    target->fd2parse = fopen( target->absolutePath.c_str(), "rb" );
    // check for directory
    if( ck_isdir( target->absolutePath ) )
    {
        // print error
        EM_error2( wherePos, "cannot parse file: '%s' is a directory", mini(target->absolutePath.c_str()) );
        // done
        return FALSE;
    }

    // if still unresolved
    if( !target->fd2parse )
    {
        // print error
        EM_error2( wherePos, "cannot open file: '%s'", target->absolutePath.c_str() );
        // done
        return FALSE;
    }

    // set file descriptor to beginning
    fseek( target->fd2parse, 0, SEEK_SET );

    // shorthand name
    target->filename = mini(target->absolutePath.c_str());

    // done
    return TRUE;
}
