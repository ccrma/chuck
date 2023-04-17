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
// file: ulib_machine.cpp
// desc: class library for 'Machine'
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_machine.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "chuck.h"
#include "util_string.h"

#include <string>
#include <algorithm>
using namespace std;

// exports
CK_DLL_SFUN( machine_crash_impl );
CK_DLL_SFUN( machine_add_impl );
CK_DLL_SFUN( machine_spork_impl );
CK_DLL_SFUN( machine_remove_impl );
CK_DLL_SFUN( machine_replace_impl );
CK_DLL_SFUN( machine_status_impl );
CK_DLL_SFUN( machine_intsize_impl );
CK_DLL_SFUN( machine_shreds_impl );
CK_DLL_SFUN( machine_realtime_impl );
CK_DLL_SFUN( machine_silent_impl );
CK_DLL_SFUN( machine_eval_impl );
CK_DLL_SFUN( machine_eval2_impl );
CK_DLL_SFUN( machine_eval3_impl );
CK_DLL_SFUN( machine_version_impl );
CK_DLL_SFUN( machine_setloglevel_impl );
CK_DLL_SFUN( machine_getloglevel_impl );
CK_DLL_SFUN( machine_refcount_impl);

// 1.5.0.0 ((ge, spencer, terry) | added
CK_DLL_CTOR( CKDoc_ctor );
CK_DLL_DTOR( CKDoc_dtor );
CK_DLL_MFUN( CKDoc_outputDir_set );
CK_DLL_MFUN( CKDoc_outputDir_get );
CK_DLL_MFUN( CKDoc_genCSS );
CK_DLL_MFUN( CKDoc_genHTMLGroup );
CK_DLL_MFUN( CKDoc_genHTMLIndex );
CK_DLL_MFUN( CKDoc_genMarkdownGroup );
CK_DLL_MFUN( CKDoc_genMarkdownIndex );
// offset
static t_CKUINT CKDoc_offset_data = 0;




//-----------------------------------------------------------------------------
// name: machine_query()
// desc: query entry point
//-----------------------------------------------------------------------------
DLL_QUERY machine_query( Chuck_DL_Query * QUERY )
{
    // get global env instance
    Chuck_Env * env = QUERY->env();
    // set name of this query
    QUERY->setname( QUERY, "Machine" );

    /*! \nameinfo
      ChucK runtime interface to the virtual machine.
      this interface can be used to manage shreds.
      They are similar to the <a href="otfp.html">
      On-the-fly Programming Commands</a>, except these are
      invoked from within a ChucK program, and are accessible
      to the timing mechanism.
    */

    // register deprecate
    type_engine_register_deprecate( env, "machine", "Machine" );

    // class
    QUERY->begin_class( QUERY, "Machine", "Object" );
    // documentation
    QUERY->doc_class( QUERY, "Machine is ChucK runtime interface to the virtual machine. This interface can be used to manage shreds, evalute code, set log levels, etc. Machine's shred commands are similar to the On-the-fly Programming Commands, except these are invoked from within a ChucK program, and are subject to the timing mechanism." );

#ifndef __DISABLE_OTF_SERVER__
    // add add
    //! compile and spork a new shred from file at 'path' into the VM now
    //! returns the shred ID
    //! (see example/machine.ck)
    QUERY->add_sfun( QUERY, machine_add_impl, "int", "add" );
    QUERY->doc_func( QUERY, "add/run a new shred from file at 'path'." );
    QUERY->add_arg( QUERY, "string", "path" );
    QUERY->doc_func( QUERY, "Compile and spork a new shred from file at 'path' into the VM now, returns the shred ID.");

    // add spork
    //! same as add
    // QUERY->add_sfun( QUERY, machine_add_impl, "int", "spork" );
    // QUERY->add_arg( QUERY, "string", "path" );

    // add remove
    //! remove shred from VM by shred ID (returned by add/spork)
    QUERY->add_sfun( QUERY, machine_remove_impl, "int", "remove" );
    QUERY->doc_func( QUERY, "remove a running shred by 'id'." );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->doc_func( QUERY, "Remove shred from VM by shred ID (returned by Machine.add).");

    // add replace
    //! replace shred with new shred from file
    //! returns shred ID , or 0 on error
    QUERY->add_sfun( QUERY, machine_replace_impl, "int", "replace" );
    QUERY->doc_func( QUERY, "replace running shred by 'id' with new shred at 'path'." );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->add_arg( QUERY, "string", "path" );
    QUERY->doc_func( QUERY, "Replace shred with new shred from file. Returns shred ID , or 0 on error.");

    // add status
    //! display current status of VM
    //! (see example/status.ck)
    QUERY->add_sfun( QUERY, machine_status_impl, "int", "status" );
    QUERY->doc_func( QUERY, "display current status of virtual machine." );
    QUERY->doc_func( QUERY, "Display the current status of VM.");

#endif // __DISABLE_OTF_SERVER__

    // add crash
    //! explicitly crash the virtual machine
    QUERY->add_sfun( QUERY, machine_crash_impl, "void", "crash" );
    QUERY->doc_func( QUERY, "explicitly crash the virtual machine. The very last resort; or an emphatic gesture. Use with care." );

    // add shreds
    //! get list of active shreds by id
    QUERY->add_sfun( QUERY, machine_shreds_impl, "int[]", "shreds" );
    QUERY->doc_func( QUERY, "Retrieve an array of active shred ids." );

    // add eval
    //! evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
    QUERY->add_sfun( QUERY, machine_eval_impl, "int", "eval" );
    QUERY->doc_func( QUERY, "evaluate a string as ChucK code, compiling it and spork it as a child shred of the current shred." );
    QUERY->add_arg( QUERY, "string", "code" );

    // add eval
    //! evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
    QUERY->add_sfun( QUERY, machine_eval2_impl, "int", "eval" );
    QUERY->doc_func( QUERY, "evaluate a string as ChucK code, with optional arguments (bundled in 'args' as \"ARG1:ARG2:...\", compiling it and spork it as a child shred of the current shred." );
    QUERY->add_arg( QUERY, "string", "code" );
    QUERY->add_arg( QUERY, "string", "args" );

    // add eval
    //! evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
    QUERY->add_sfun( QUERY, machine_eval3_impl, "int", "eval" );
    QUERY->doc_func( QUERY, "evaluate a string as ChucK code, with optional arguments (bundled in 'args' as \"ARG1:ARG2:...\", compiling it and sporking 'count' instances as a child shreds of the current shred." );
    QUERY->add_arg( QUERY, "string", "code" );
    QUERY->add_arg( QUERY, "string", "args" );
    QUERY->add_arg( QUERY, "int", "count" );

    // add get intsize (width)
    //! get the intsize in bits (e.g., 32 or 64)
    QUERY->add_sfun( QUERY, machine_intsize_impl, "int", "intsize" );
    QUERY->doc_func( QUERY, "Return the bit size of an integer.");

    // add check for silent (or realtime audio) | adapted from nshaheed PR #230
    QUERY->add_sfun( QUERY, machine_silent_impl, "int", "silent" );
    QUERY->doc_func( QUERY, "return false if the shred is in realtime mode, true if it's in silent mode (i.e. --silent is enabled)" );

    // add check for realtime (always opposite of silent()) | adapted from nshaheed PR #230
    QUERY->add_sfun( QUERY, machine_realtime_impl, "int", "realtime" );
    QUERY->doc_func( QUERY, "return true if the shred is in realtime mode, false if it's in silent mode (i.e. --silent is enabled)" );

    // get version string
    QUERY->add_sfun( QUERY, machine_version_impl, "string", "version" );
    QUERY->doc_func( QUERY, "return version string" );

    // get set loglevel
    QUERY->add_sfun( QUERY, machine_setloglevel_impl, "int", "loglevel" );
    QUERY->add_arg( QUERY, "int", "level" );
    QUERY->doc_func( QUERY, "set log level\n"
                     "       |- 0: NONE\n"
                     "       |- 1: CORE\n"
                     "       |- 2: SYSTEM\n"
                     "       |- 3: SEVERE\n"
                     "       |- 4: WARNING\n"
                     "       |- 5: INFO\n"
                     "       |- 6: CONFIG\n"
                     "       |- 7: FINE\n"
                     "       |- 8: FINER\n"
                     "       |- 9: FINEST\n"
                     "       |- 10: ALL" );

    // get log level
    QUERY->add_sfun( QUERY, machine_getloglevel_impl, "int", "loglevel" );
    QUERY->doc_func( QUERY, "get log level." );

    // get reference count for any object
    QUERY->add_sfun( QUERY, machine_refcount_impl, "int", "refcount" );
    QUERY->add_arg( QUERY, "Object", "obj" );
    QUERY->doc_func( QUERY, "get an object's current internal reference count; this is intended for testing or curiosity; NOTE: this function intentionally does not take into account any reference counting related to the calling of this function (normally all functions increments the reference count for objects passed as arguments and decrements upon returning)" );

    // end class
    QUERY->end_class( QUERY );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: machine_intsize()
// desc: integer size in bits
//-----------------------------------------------------------------------------
t_CKUINT machine_intsize()
{
    return sizeof(t_CKINT) * 8;
}




//-----------------------------------------------------------------------------
// name: machine_realtime()
// desc: return whether chuck is in realtime mode, or silent mode (--silent is enabled)
//-----------------------------------------------------------------------------
t_CKUINT machine_realtime( Chuck_VM * vm )
{
    return vm->carrier()->hintIsRealtimeAudio();
}




//-----------------------------------------------------------------------------
// name: machine_eval()
// desc: evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
//-----------------------------------------------------------------------------
t_CKUINT machine_eval( Chuck_String * codeStr, Chuck_String * argsTogether,
                       t_CKINT count, Chuck_VM * vm )
{
    // check if null
    if( codeStr == NULL ) return FALSE;

    // get ChucK instance
    ChucK * chuck = vm->carrier()->chuck;
    // get code as string
    string code = codeStr->str();
    // get args, if there
    string args = argsTogether ? argsTogether->str() : "";

    return chuck->compileCode( code, args, count );
}



#ifndef __DISABLE_OTF_SERVER__

// OTF server stuff
static Chuck_Compiler * the_compiler = NULL;
static proc_msg_func the_func = NULL;
//-----------------------------------------------------------------------------
// name: machine_init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL machine_init( Chuck_Compiler * compiler, proc_msg_func proc_msg )
{
    the_compiler = compiler;
    the_func = proc_msg;

    return TRUE;
}

// add
CK_DLL_SFUN( machine_add_impl )
{
    const char * v = GET_CK_STRING(ARGS)->str().c_str();
    Net_Msg msg;

    msg.type = MSG_ADD;
    strcpy( msg.buffer, v );
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// remove
CK_DLL_SFUN( machine_remove_impl )
{
    t_CKINT v = GET_CK_INT(ARGS);
    Net_Msg msg;

    msg.type = MSG_REMOVE;
    msg.param = v;
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// replace
CK_DLL_SFUN( machine_replace_impl )
{
    t_CKINT v = GET_NEXT_INT(ARGS);
    const char * v2 = GET_NEXT_STRING(ARGS)->str().c_str();
    Net_Msg msg;

    msg.type = MSG_REPLACE;
    msg.param = v;
    strcpy( msg.buffer, v2 );
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// status
CK_DLL_SFUN( machine_status_impl )
{
    Net_Msg msg;

    msg.type = MSG_STATUS;
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

#endif // __DISABLE_OTF_SERVER__

// intsize
CK_DLL_SFUN( machine_intsize_impl )
{
    RETURN->v_int = machine_intsize();
}

// realtime
CK_DLL_SFUN( machine_realtime_impl )
{
    RETURN->v_int = machine_realtime( SHRED->vm_ref );
}

// silent
CK_DLL_SFUN( machine_silent_impl )
{
    RETURN->v_int = !machine_realtime( SHRED->vm_ref );
}

CK_DLL_SFUN( machine_shreds_impl )
{
    Chuck_Array4 *array = new Chuck_Array4(FALSE);
    initialize_object(array, SHRED->vm_ref->env()->t_array);
    array->clear();

    Chuck_VM_Status status;
    SHRED->vm_ref->shreduler()->status(&status);

    for(int i = 0; i < status.list.size(); i++)
        array->push_back(status.list[i]->xid);

    status.clear();

    RETURN->v_object = array;
}

CK_DLL_SFUN( machine_eval_impl )
{
    // get arguments
    Chuck_String * code = GET_NEXT_STRING(ARGS);

    RETURN->v_int = machine_eval( code, NULL, 1, VM );
}

CK_DLL_SFUN( machine_eval2_impl )
{
    // get arguments
    Chuck_String * code = GET_NEXT_STRING(ARGS);
    Chuck_String * argsTogether = GET_NEXT_STRING(ARGS);

    RETURN->v_int = machine_eval( code, argsTogether, 1, VM );
}

CK_DLL_SFUN( machine_eval3_impl )
{
    // get arguments
    Chuck_String * code = GET_NEXT_STRING(ARGS);
    Chuck_String * argsTogether = GET_NEXT_STRING(ARGS);
    t_CKINT count = GET_NEXT_INT(ARGS);

    RETURN->v_int = machine_eval( code, argsTogether, count, VM );
}

CK_DLL_SFUN( machine_version_impl )
{
    // get version string
    string vs = VM->carrier()->chuck->version();
    // make chuck string
    Chuck_String * s = new Chuck_String( vs );
    // initialize
    initialize_object(s, VM->carrier()->env->t_string );
    // return
    RETURN->v_string = s;
}

CK_DLL_SFUN( machine_setloglevel_impl )
{
    // arg
    t_CKINT level = GET_NEXT_INT(ARGS);
    // get current log
    t_CKINT current = VM->carrier()->chuck->getLogLevel();
    // clamp compare
    if( level < CK_LOG_NONE ) level = CK_LOG_NONE;
    else if( level > CK_LOG_ALL ) level = CK_LOG_ALL;
    // compare
    if( level != current )
    {
        EM_log( CK_LOG_NONE, "updating log level from %d to %d...", current, level );
        VM->carrier()->chuck->setLogLevel( level );
    }
    // return
    RETURN->v_int = level;
}

CK_DLL_SFUN( machine_getloglevel_impl )
{
    // return
    RETURN->v_int = VM->carrier()->chuck->getLogLevel();
}

CK_DLL_SFUN( machine_refcount_impl )
{
    // get the argument
    Chuck_Object * obj = (Chuck_Object *)GET_NEXT_OBJECT(ARGS);
    // return (-1 to account for the extra refcount as part of calling this function!)
    RETURN->v_int = obj != NULL ? obj->m_ref_count-1 : 0;
}

// add
CK_DLL_SFUN( machine_crash_impl )
{
    CK_FPRINTF_STDERR( "[chuck]: crashing...\n" );
    *(volatile int *)0 = 0;
}




//-----------------------------------------------------------------------------
// name: ckdoc_query()
// desc: add class for ckdoc documentation generator | 1.5.0.0 ((ge, spencer, terry)
//-----------------------------------------------------------------------------
DLL_QUERY ckdoc_query( Chuck_DL_Query * QUERY )
{
    // get environment reference
    Chuck_Env * env = QUERY->env();
    // set module name
    QUERY->setname( QUERY, "CKDoc" );

    // function
    Chuck_DL_Func * func = NULL;
    // documentation string
    std::string doc;

    //---------------------------------------------------------------------
    // init as class
    // 1.5.0.0 added by spencer and ge
    //---------------------------------------------------------------------
    // doc string
    doc = "A ChucK documentation generator. Based on Spencer Salazar's ckdoc utility.";

    // begin class definition
    if( !type_engine_import_class_begin( env, "CKDoc", "Object", env->global(), CKDoc_ctor, CKDoc_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    CKDoc_offset_data = type_engine_import_mvar( env, "int", "@CKDoc_data", FALSE );
    if( CKDoc_offset_data == CK_INVALID_OFFSET ) goto error;

    // outputDir
    func = make_new_mfun( "string", "outputDir", CKDoc_outputDir_set );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Set the output directory; this will be automatically prepended to relative-path filenames for generated files.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // outputDir
    func = make_new_mfun( "string", "outputDir", CKDoc_outputDir_get );
    func->doc = "Get the output directory.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genCSS
    func = make_new_mfun( "int", "genCSS", CKDoc_genCSS );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Generate the CSS file named in 'cssFilename'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genHTMLGroup
    func = make_new_mfun( "int", "genHTMLGroup", CKDoc_genHTMLGroup );
    func->add_arg( "Type[]", "classes" );
    func->add_arg( "string", "htmlFilename" );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Generate the document HTML file named 'htmlFilename' for types in the array 'classes', referencing the CSS file named in 'cssFilename'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genHTMLIndex
    func = make_new_mfun( "int", "genHTMLIndex", CKDoc_genHTMLIndex );
    func->add_arg( "Type[]", "classes" );
    func->add_arg( "string", "htmlFilename" );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Generate the index HTML file named 'htmlFilename' for types in the array 'classes', referencing the CSS file named in 'cssFilename'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genMarkdownGroup
    func = make_new_mfun( "int", "genMarkdownGroup", CKDoc_genMarkdownGroup );
    func->add_arg( "Type[]", "classes" );
    func->add_arg( "string", "mdFilename" );
    func->doc = "Generate the document Markdown file named 'mdFilename' for types in the array 'classes'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genMarkdownIndex
    func = make_new_mfun( "int", "genMarkdownIndex", CKDoc_genMarkdownIndex );
    func->add_arg( "Type[]", "classes" );
    func->add_arg( "string", "mdFilename" );
    func->doc = "Generate the index Markdown file named 'mdFilename' for types in the array 'classes'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return true;

error:

    return false;
}




//-----------------------------------------------------------------------------
// begin CKDoc implemention
//-----------------------------------------------------------------------------
std::string type2url( const std::string & type )
{
    return "[type2url NOT IMPLEMENTED]";
}




//-----------------------------------------------------------------------------
// name: class CKDocTextFilter
// desc: pure abstract class (i.e., interface) for a text filter
//-----------------------------------------------------------------------------
class CKDocTextFilter
{
public:
    // virtal destructor
    virtual ~CKDocTextFilter() { };

public:
    virtual std::string filter(const std::string &) = 0;
};




//-----------------------------------------------------------------------------
// name: class CKDocOutput | 1.5.0.0 (based on Spencer's ckdoc Output class)
// desc: abstract base class for doc output
//-----------------------------------------------------------------------------
class CKDocOutput
{
public:
    // base constructor
    CKDocOutput()
    { m_doc_text_filter = NULL; }
    // virtual destructor
    virtual ~CKDocOutput() { SAFE_DELETE(m_doc_text_filter); }

public:
    void set_doc_text_filter( CKDocTextFilter * filter )
    { m_doc_text_filter = filter; }

public:
    virtual void begin(const std::string &title) = 0;
    virtual void heading() = 0;
    virtual void end() = 0;

    // body
    virtual void begin_body() = 0;
    virtual void end_body() = 0;

    // toc
    virtual void begin_toc() = 0;
    virtual void toc_class(Chuck_Type * type) = 0;
    virtual void end_toc() = 0;

    // classes
    virtual void begin_classes() = 0;
    virtual void end_classes() = 0;

    // one class
    virtual void begin_class( Chuck_Type * type ) = 0;

    // examples
    virtual void begin_examples() = 0;
    virtual void example( const std::string & name, const std::string & url ) = 0;
    virtual void end_examples() = 0;

    // class variables
    virtual void begin_static_member_vars() = 0;
    virtual void end_static_member_vars() = 0;
    virtual void begin_member_vars() = 0;
    virtual void end_member_vars() = 0;

    // class functions
    virtual void begin_static_member_funcs() = 0;
    virtual void end_static_member_funcs() = 0;
    virtual void begin_member_funcs() = 0;
    virtual void end_member_funcs() = 0;

    // individual vars
    virtual void static_member_var( Chuck_Value * var ) = 0;
    virtual void member_var( Chuck_Value * var ) = 0;

    // individual funs
    virtual void begin_static_member_func(Chuck_Func * func) = 0;
    virtual void end_static_member_func() = 0;
    virtual void begin_member_func(Chuck_Func * func) = 0;
    virtual void end_member_func() = 0;

    // func arguments
    virtual void func_arg(a_Arg_List arg) = 0;

    // end class
    virtual void end_class() = 0;

protected:
    // reference to doc text filter
    CKDocTextFilter * m_doc_text_filter;
};




//-----------------------------------------------------------------------------
// name: class CKDocHTMLOutput | 1.5.0.0 (ge, spencer, terry) added
// desc: HTML output (based on Spencer's ckdoc)
//-----------------------------------------------------------------------------
class CKDocHTMLOutput : public CKDocOutput
{
public:
    CKDocHTMLOutput( FILE * output = stdout )
        : m_output(output), m_func(NULL) { }

    void begin( const std::string & title )
    {
        m_title = title;
        if(m_title.length() == 0) m_title = "&nbsp;";

        fprintf(m_output, "<html>\n");
        fprintf(m_output, "<link rel=\"stylesheet\" type=\"text/css\" href=\"ckdoc.css\" />\n");
        fprintf(m_output, "<link rel=\"stylesheet\" type=\"text/css\" href=\"class.css\" />\n");
        fprintf(m_output, "<title>%s</title>\n", m_title.c_str());
        fprintf(m_output, "<body>\n");
    }

    void heading()
    {
        fprintf(m_output, "<a name=\"top\"/>\n");
        fprintf(m_output, "<div id=\"title\">\n");
        fprintf(m_output, "<div class=\"titleL\"><h1><a href=\"index.html\">ChucK Class Library Reference</a></h1></div>\n");
        fprintf(m_output, "<div class=\"titleR\"><h1>%s</h1></div>\n", m_title.c_str());
        fprintf(m_output, "</div>\n"); // #title
    }

    void end()
    {
        fprintf(m_output, "</html>\n");
    }

    void title(const std::string &_title)
    {
    }

    void begin_body()
    {
        fprintf(m_output, "<div id=\"body\">\n");
    }

    void end_body()
    {
        fprintf(m_output, "</div>\n"); // #body
        fprintf(m_output, "</body>\n");
    }

    void begin_toc()
    {
        fprintf(m_output, "<div class=\"toc\"><a id=\"toc\"/>\n");
    }

    void toc_class(Chuck_Type * type)
    {
        fprintf(m_output, "<p class=\"toc_class\"><a href=\"#%s\" class=\"%s\">%s</a></p>\n",
                type->name.c_str(), class_for_type(type), type->name.c_str());
    }

    void end_toc()
    {
        fprintf(m_output, "</div>\n");
    }

    void begin_classes()
    {
        fprintf(m_output, "<div class=\"classes\">\n");
    }

    void end_classes()
    {
        fprintf(m_output, "</div>\n");
    }

    void begin_class(Chuck_Type * type)
    {
        fprintf(m_output, "<a name=\"%s\" /><div class=\"class\">\n", type->name.c_str());
        fprintf(m_output, "<h2 class=\"class_title\" name=\"%s\">%s</h2>\n",
                type->name.c_str(), type->name.c_str());

        // type heirarchy
        Chuck_Type * parent = type->parent;
        if(parent != NULL) fprintf(m_output, "<h4 class=\"class_hierarchy\">");
        while(parent != NULL)
        {
            fprintf(m_output, ": <a href=\"%s\" class=\"%s\">%s</a> ",
                    type2url(parent->name).c_str(), class_for_type(parent), parent->name.c_str());
            parent = parent->parent;
        }
        if(type->parent != NULL) fprintf(m_output, "</h4>\n");

        if(type->doc.size() > 0)
        {
            std::string doc;
            if(m_doc_text_filter)
                doc = m_doc_text_filter->filter(type->doc);
            else
                doc = type->doc;
            fprintf(m_output, "<p class=\"class_description\">%s</p>\n",
                    doc.c_str());
        }
        else
            fprintf(m_output, "<p class=\"empty_class_description\">No description available</p>\n");
    }

    void end_class()
    {
        fprintf(m_output, "<p class=\"top_link\">[ <a href=\"#top\">top</a> ]</p>\n");
        fprintf(m_output, "</div>\n<hr />\n");
    }

    void begin_examples()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">examples</h3><div class=\"examples\"><ul>\n");
    }

    void end_examples()
    {
        fprintf(m_output, "</ul></div>\n");
    }

    void begin_static_member_vars()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">static member variables</h3>\n<div class=\"members\">\n");
    }

    void end_static_member_vars()
    {
        fprintf(m_output, "</div>\n");
    }

    void begin_member_vars()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">member variables</h3>\n<div class=\"members\">\n");
    }

    void end_member_vars()
    {
        fprintf(m_output, "</div>\n");
    }

    void begin_static_member_funcs()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">static member functions</h3>\n<div class=\"members\">\n");
    }

    void end_static_member_funcs()
    {
        fprintf(m_output, "</div>\n");
    }

    void begin_member_funcs()
    {
        fprintf(m_output, "<h3 class=\"class_section_header\">member functions</h3>\n<div class=\"members\">\n");
    }

    void end_member_funcs()
    {
        fprintf(m_output, "</div>\n");
    }

    void example(const std::string &name, const std::string &url)
    {
        fprintf(m_output, "<li><a href=\"%s\">%s</a></li>\n", url.c_str(), name.c_str());
    }

    void static_member_var(Chuck_Value * var)
    {
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s",
                class_for_type(var->type), var->type->name.c_str());
        if(var->type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < var->type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");

        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span></p>", var->name.c_str());

        if(var->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n",
                    var->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");

        fprintf(m_output, "</div>\n");
    }

    void member_var(Chuck_Value * var)
    {
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s",
                class_for_type(var->type), var->type->name.c_str());
        if(var->type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < var->type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");

        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span></p>", var->name.c_str());

        if(var->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n",
                    var->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");

        fprintf(m_output, "</div>\n");
    }

    void begin_static_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s",
                class_for_type(func->def->ret_type),
                func->def->ret_type->name.c_str());
        if(func->def->ret_type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < func->def->ret_type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");

        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span>(", S_name(func->def->name));

        m_func = func;
    }

    void end_static_member_func()
    {
        fprintf(m_output, ")</p>\n");

        if(m_func->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n",
                    m_func->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");

        fprintf(m_output, "</div>\n");

        m_func = NULL;
    }

    void begin_member_func(Chuck_Func * func)
    {
        // return type
        fprintf(m_output, "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\"%s\">%s",
                class_for_type(func->def->ret_type),
                func->def->ret_type->name.c_str());
        if(func->def->ret_type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < func->def->ret_type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");

        // function name
        fprintf(m_output, "<span class=\"membername\">%s</span>(", S_name(func->def->name));

        m_func = func;
    }

    void end_member_func()
    {
        fprintf(m_output, ")</p>\n");

        if(m_func->doc.size() > 0)
            fprintf(m_output, "<p class=\"member_description\">%s</p>\n",
                    m_func->doc.c_str());
        else
            fprintf(m_output, "<p class=\"empty_member_description\">No description available</p>\n");

        fprintf(m_output, "</div>\n");

        m_func = NULL;
    }

    void func_arg(a_Arg_List arg)
    {
        // argument type
        fprintf(m_output, "<span class=\"%s\">%s",
                class_for_type(arg->type), arg->type->name.c_str());
        if(arg->type->array_depth)
        {
            fprintf(m_output, "</span>");
            fprintf(m_output, "<span class=\"typename\">");
            for(int i = 0; i < arg->type->array_depth; i++)
                fprintf(m_output, "[]");
        }
        fprintf(m_output, "</span> ");

        // argument name
        std::string varname = varnameclean(S_name(arg->var_decl->xid));
        fprintf(m_output, "%s", varname.c_str());

        if(arg->next != NULL)
            fprintf(m_output, ", ");
    }

private:
    FILE * m_output;
    Chuck_Func *m_func;
    std::string m_title;

    bool isugen(Chuck_Type *type) { return type->ugen_info != NULL; }

    std::string varnameclean(std::string vn)
    {
        // strip []
        vn.erase(std::remove(vn.begin(), vn.end(), '['), vn.end());
        vn.erase(std::remove(vn.begin(), vn.end(), ']'), vn.end());

        return vn;
    }

    std::string cssclean(std::string s)
    {
        for(int i = 0; i < s.length(); i++)
        {
            if(!isalnum(s[i])) s[i] = '_';
        }

        return s;
    }

    const char * class_for_type( Chuck_Type *type )
    {
        static char buf[1024];
        std::string name = cssclean(type->name);
        snprintf( buf, 1024, "typename type_%s", name.c_str() );
        return buf;
    }
};




//-----------------------------------------------------------------------------
// name: class CKDoc_Impl | 1.5.0.0 (ge, spencer, terry)
// desc: CKDoc internal implementation, based on Spencer's CKDoc utility
//-----------------------------------------------------------------------------
class CKDoc_Impl
{
public:
    // constructor
    CKDoc_Impl() { }

public:
    // generate CSS file
    t_CKBOOL outputCSS( const string & cssFilename );
    // generate an HTML file containing documentation for a list of classes
    t_CKBOOL outputHTMLGroup( const vector<Chuck_Type *> & clasess,
                              const string & htmlFilename,
                              const string & cssFilename );
    // generate an HTML file containing index for a list of classes
    t_CKBOOL outputHTMLIndex( const vector<string> & clasess,
                              const string & htmlFilename,
                              const string & cssFilename );
    // generate an Markdown file containing documentation for a list of classes
    t_CKBOOL outputMarkdownGroup( const vector<string> & clasess,
                               const string & mdFilename );
    // does this even make sense for Markdown?
    t_CKBOOL outputMarkdownIndex( const vector<string> & clasess,
                                  const string & mdFilename );

public:
    // generate HTML for a type
    t_CKBOOL outputHTMLType( const string & typeName, string & output );

public:
    // where to output files
    string m_outputDir;
    // the HTML output
    CKDocHTMLOutput m_htmlOutput;
};




//-----------------------------------------------------------------------------
// name: genCSS()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc_Impl::outputCSS( const string & cssFilename )
{

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genHTMLGroup()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc_Impl::outputHTMLGroup( const vector<Chuck_Type *> & clasess,
                                      const string & htmlFilename,
                                      const string & cssFilename )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genHTMLIndex()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc_Impl::outputHTMLIndex( const vector<string> & clasess,
                                      const string & htmlFilename,
                                      const string & cssFilename )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genMarkdownGroup()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc_Impl::outputMarkdownGroup( const vector<string> & clasess,
                                          const string & mdFilename )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genMarkdownIndex()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc_Impl::outputMarkdownIndex( const vector<string> & clasess,
                                          const string & mdFilename )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// CKDoc implementation
//-----------------------------------------------------------------------------
CK_DLL_CTOR( CKDoc_ctor )
{
    CKDoc_Impl * ckdoc = new CKDoc_Impl();
    OBJ_MEMBER_UINT( SELF, CKDoc_offset_data ) = (t_CKUINT)ckdoc;
}

CK_DLL_DTOR( CKDoc_dtor )
{
    CKDoc_Impl * ckdoc = (CKDoc_Impl *)OBJ_MEMBER_UINT( SELF, CKDoc_offset_data );
    SAFE_DELETE( ckdoc );
    OBJ_MEMBER_UINT( SELF, CKDoc_offset_data ) = 0;
}

CK_DLL_MFUN( CKDoc_outputDir_set )
{
    CKDoc_Impl * ckdoc = (CKDoc_Impl *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * path = GET_CK_STRING(ARGS);
    // check if null
    if( path == NULL ) ckdoc->m_outputDir = ".";
    else
    {
        // set output dir
        ckdoc->m_outputDir = trim( path->str() );
    }
    // set return value
    RETURN->v_string = path;
}

CK_DLL_MFUN( CKDoc_outputDir_get )
{
    CKDoc_Impl * ckdoc = (CKDoc_Impl *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( ckdoc->m_outputDir );
    RETURN->v_string = str;
}

CK_DLL_MFUN( CKDoc_genCSS )
{
    CKDoc_Impl * ckdoc = (CKDoc_Impl *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * cssFilename = GET_CK_STRING(ARGS);
    // check if null
    if( cssFilename == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genCSS() given null argument; nothing generated...\n" );
        RETURN->v_int = FALSE;
    }
    else
    {
        RETURN->v_int = ckdoc->outputCSS( cssFilename->str() );
    }
}

CK_DLL_MFUN( CKDoc_genHTMLGroup )
{
    CKDoc_Impl * ckdoc = (CKDoc_Impl *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_Array4 * typeArray = (Chuck_Array4 *)GET_CK_OBJECT(ARGS);
    Chuck_String * htmlFilename = GET_CK_STRING(ARGS);
    Chuck_String * cssFilename = GET_CK_STRING(ARGS);
    // check if null
    if( typeArray == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLGroup() given null 'classes' argument; nothing generated...\n" );
        goto error;
    }
    else if( htmlFilename == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLGroup() given null 'classes' argument; nothing generated...\n" );
        goto error;
    }
    else
    {
        vector<Chuck_Type *> classes;
        for( int i = 0; i < typeArray->m_vector.size(); i++ )
        {
            // get pointer as chuck string
            Chuck_Type * t = (Chuck_Type *)typeArray->m_vector[i];
            // check
            if( t != NULL )
            {
                // append
                classes.push_back( t );
            }
        }
        // check
        if( classes.size() == 0 )
        {
            CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLGroup() given 'classes' array with null and/or empty strings; nothing generated...\n" );
            goto error;
        }

        // get
        RETURN->v_int = ckdoc->outputHTMLGroup( classes, htmlFilename->str(), cssFilename ? cssFilename->str() : "" );
    }

    // set return value
    RETURN->v_int = TRUE;
    // done
    return;

error:
    RETURN->v_int = FALSE;
}

CK_DLL_MFUN( CKDoc_genHTMLIndex )
{
    CKDoc_Impl * ckdoc = (CKDoc_Impl *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_Array4 * strArray = (Chuck_Array4 *)GET_CK_OBJECT(ARGS);
    Chuck_String * htmlFilename = GET_CK_STRING(ARGS);
    Chuck_String * cssFilename = GET_CK_STRING(ARGS);

    // class names
    vector<string> classes;

    // check if null
    if( strArray == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLIndex() given null 'classes' argument; nothing generated...\n" );
        goto error;
    }
    else if( htmlFilename == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLIndex() given null 'classes' argument; nothing generated...\n" );
        goto error;
    }

    for( int i = 0; i < strArray->m_vector.size(); i++ )
    {
        // get pointer as chuck string
        Chuck_String * str = (Chuck_String *)strArray->m_vector[i];
        // check
        if( str != NULL && trim(str->str()) != "" )
        {
            // append
            classes.push_back( str->str() );
        }
    }
    // check
    if( classes.size() == 0 )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLIndex() given 'classes' array with null and/or empty strings; nothing generated...\n" );
        goto error;
    }

    // get
    RETURN->v_int = ckdoc->outputHTMLIndex( classes, htmlFilename->str(), cssFilename ? cssFilename->str() : "" );

    // set return value
    RETURN->v_int = TRUE;
    // done
    return;

error:
    RETURN->v_int = FALSE;
}

CK_DLL_MFUN( CKDoc_genMarkdownGroup )
{
    CK_FPRINTF_STDERR( "[chuck] CKDoc.genMarkdownGroup() currently not implemented!\n" );
}

CK_DLL_MFUN( CKDoc_genMarkdownIndex )
{
    CK_FPRINTF_STDERR( "[chuck] CKDoc.genMarkdownIndex() currently not implemented!\n" );
}
