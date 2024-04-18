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
// file: ulib_machine.cpp
// desc: class library for 'Machine'
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_doc.h"
#include "chuck.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_instr.h"
#include "chuck_compile.h"
#include "chuck_errmsg.h"
#include "util_string.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;

// 1.5.0.0 ((ge, spencer) | added
CK_DLL_CTOR( CKDoc_ctor );
CK_DLL_DTOR( CKDoc_dtor );
CK_DLL_MFUN( CKDoc_addGroup_str );
CK_DLL_MFUN( CKDoc_addGroup_type );
CK_DLL_MFUN( CKDoc_addGroup_ext );
CK_DLL_MFUN( CKDoc_numGroups );
CK_DLL_MFUN( CKDoc_clearGroup );
CK_DLL_CTRL( CKDoc_examplesRoot_set );
CK_DLL_CGET( CKDoc_examplesRoot_get );
CK_DLL_CTRL( CKDoc_outputFormat_set );
CK_DLL_CGET( CKDoc_outputFormat_get );
CK_DLL_MFUN( CKDoc_genIndex );
CK_DLL_MFUN( CKDoc_genCSS );
CK_DLL_MFUN( CKDoc_genGroups );
CK_DLL_MFUN( CKDoc_genType_type );
CK_DLL_MFUN( CKDoc_genType_str );
CK_DLL_MFUN( CKDoc_outputToDir );
// offset
static t_CKUINT CKDoc_offset_data = 0;

// static initialization
t_CKINT const CKDoc::FORMAT_NONE = 0;
t_CKINT const CKDoc::FORMAT_HTML = 1;
t_CKINT const CKDoc::FORMAT_TEXT = 2;
t_CKINT const CKDoc::FORMAT_MARKDOWN = 3;
t_CKINT const CKDoc::FORMAT_JSON = 4;




//-----------------------------------------------------------------------------
// name: ckdoc_query()
// desc: add class for ckdoc documentation generator | 1.5.0.0 (ge, spencer, terry)
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

    // const static variables, for format selection, e.g., with CKDoc.HTML or CKDoc.TEXT
    if( !type_engine_import_svar( env, "int", "NONE", TRUE, (t_CKUINT)&CKDoc::FORMAT_NONE, "no output format." ) ) goto error;
    if( !type_engine_import_svar( env, "int", "HTML", TRUE, (t_CKUINT)&CKDoc::FORMAT_HTML, "output HTML format." ) ) goto error;
    if( !type_engine_import_svar( env, "int", "TEXT", TRUE, (t_CKUINT)&CKDoc::FORMAT_TEXT, "output TEXT format (not implemented)." ) ) goto error;
    if( !type_engine_import_svar( env, "int", "MARKDOWN", TRUE, (t_CKUINT)&CKDoc::FORMAT_MARKDOWN, "output MARKDOWN format (not implemented)." ) ) goto error;
    if( !type_engine_import_svar( env, "int", "JSON", TRUE, (t_CKUINT)&CKDoc::FORMAT_JSON, "output JSON format (not implemented)." ) ) goto error;

    // addGroup (by types)
    func = make_new_mfun( "void", "addGroup", CKDoc_addGroup_type );
    func->add_arg( "Type[]", "types" );
    func->add_arg( "string", "name" );
    func->add_arg( "string", "shortName" );
    func->add_arg( "string", "description" );
    func->doc = "Add a group of types to be documented, including group 'name', a 'shortName' to be used for any files, and a group 'description'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // addGroup (by names)
    func = make_new_mfun( "void", "addGroup", CKDoc_addGroup_str );
    func->add_arg( "string[]", "typeNames" );
    func->add_arg( "string", "name" );
    func->add_arg( "string", "shortName" );
    func->add_arg( "string", "description" );
    func->doc = "Add a group of types (by type name) to be documented, including group 'name', a 'shortName' to be used for any files, and a group 'description'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // addGroupExternal | 1.5.2.4 (ge) added
    func = make_new_mfun( "void", "addGroupExternal", CKDoc_addGroup_ext );
    func->add_arg( "string", "name" );
    func->add_arg( "string", "URL" );
    func->add_arg( "string", "description" );
    func->add_arg( "string", "longDesc" );
    func->doc = "Add a group documention at an external URL location";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // numGroups
    func = make_new_mfun( "int", "numGroups", CKDoc_numGroups );
    func->doc = "Get the number of groups added.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // clear
    func = make_new_mfun( "void", "clear", CKDoc_clearGroup );
    func->doc = "Clear all added groups.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // examplesRoot
    func = make_new_mfun( "string", "examplesRoot", CKDoc_examplesRoot_set );
    func->add_arg( "string", "path" );
    func->doc = "Set the examples directory root path; returns what was set.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // examplesRoot
    func = make_new_mfun( "string", "examplesRoot", CKDoc_examplesRoot_get );
    func->doc = "Get the current examples directory root path.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // outputFormat
    func = make_new_mfun( "int", "outputFormat", CKDoc_outputFormat_set );
    func->add_arg( "int", "which" );
    func->doc = "Set which output format to use; see CKDoc.HTML, CKDoc.TEXT, CKDoc.MARKDOWN, CKDoc.JSON.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // CKDoc_outputFormat_set
    func = make_new_mfun( "int", "outputFormat", CKDoc_outputFormat_get );
    func->doc = "Set which output format is selected; see CKDoc.HTML, CKDoc.TEXT, CKDoc.MARKDOWN, CKDoc.JSON.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genIndex
    func = make_new_mfun( "string", "genIndex", CKDoc_genIndex );
    func->add_arg( "string", "indexTitle" );
    func->doc = "Generate top-level index; return as string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genCSS
    func = make_new_mfun( "string", "genCSS", CKDoc_genCSS );
    func->doc = "Generate CSS; return as string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genGroups
    func = make_new_mfun( "void", "genGroups", CKDoc_genGroups );
    func->add_arg( "string[]", "results" );
    func->doc = "Generate documentation for all added groups, return each in a separate entry";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genType
    func = make_new_mfun( "string", "genType", CKDoc_genType_type );
    func->add_arg( "Type", "type" );
    func->doc = "Generate documentation for a single Type.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genType
    func = make_new_mfun( "string", "genType", CKDoc_genType_str );
    func->add_arg( "string", "typeName" );
    func->doc = "Generate documentation for a single Type (by name).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // outputToDir
    func = make_new_mfun( "int", "outputToDir", CKDoc_outputToDir );
    func->add_arg( "string", "path" );
    func->add_arg( "string", "indexTitle" );
    func->doc = "Generate everything as files into the output directory.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return true;

error:

    return false;
}




//-----------------------------------------------------------------------------
// name: struct CKDocGroup | 1.5.0.0 (ge, spencer) added
// desc: structure for a doc group e.g., "UGens"
//-----------------------------------------------------------------------------
struct CKDocGroup
{
    // types in the group
    vector<Chuck_Type *> types;
    // name of the group
    string name;
    // short name of the group (used for filenames and such)
    string shortName;
    // description of the group
    string desc;
    // long description (for groups that point to external CKDoc) | 1.5.2.4 (ge) added
    string longDesc;
};




//-----------------------------------------------------------------------------
// name: type2url()
// desc: convert type name to URL, using groups to detemine the file for each
//-----------------------------------------------------------------------------
string type2url( const string & type, const vector<CKDocGroup *> & groups )
{
    // search
    for( t_CKINT i = 0; i < groups.size(); i++ )
    {
        for( t_CKINT j = 0; j < groups[i]->types.size(); j++ )
        {
            // check
            if( groups[i]->types[j] == NULL ) continue;
            // compare
            if( type == groups[i]->types[j]->base_name )
            {
                // file + location within file: basically: 'group.html#type'
                return groups[i]->shortName + ".html#" + type;
            }
        }
    }

    // TODO: make a list of default classes on chuck website to link to
    // return "[type2url NOT IMPLEMENTED]";
    return "";
}





//-----------------------------------------------------------------------------
// name: class CKDocHTMLOutput | 1.5.0.0 (ge, spencer) added
// desc: HTML output (based on Spencer's ckdoc)
//-----------------------------------------------------------------------------
class CKDocHTMLOutput : public CKDocOutput
{
private:
    Chuck_Func * m_func;
    std::string m_title;
    Chuck_VM * m_vm_ref;
    Chuck_Env * m_env_ref;

public:
    CKDocHTMLOutput( Chuck_VM * vm )
      : m_func(NULL), m_vm_ref(vm)
    {
        // get the env
        m_env_ref = vm != NULL ? vm->env() : NULL;
    }
    // file extension
    virtual string fileExtension() const { return ".html"; }
    // render HTML index
    virtual string renderIndex( const string & indexTitle, const vector<CKDocGroup *> & groups );

public:
    void begin( const string & title )
    {
        // trim
        m_title = trim(title);
        // check length
        if( m_title.length() == 0 ) m_title = "&nbsp;";
        // string to return

        m_outputStr += "<html>\n";
        m_outputStr += "<link rel=\"stylesheet\" type=\"text/css\" href=\"ckdoc.css\" />\n";
        m_outputStr += "<link rel=\"stylesheet\" type=\"text/css\" href=\"class.css\" />\n";
        m_outputStr += "<title>" + m_title + "</title>\n";
        m_outputStr += "<body>\n";
    }

    void heading()
    {
        m_outputStr += "<a name=\"top\"/>\n";
        m_outputStr += "<div id=\"title\">\n";
        m_outputStr += "<div class=\"titleL\">\n";
        m_outputStr += "<div style=\"display: flex; align-items: center;\">";
        m_outputStr += "<div class=\"inline titlelogo\"><a href=\"https://chuck.stanford.edu/\" target=\"_blank\"><img src=\"https://chuck.stanford.edu/doc/images/chuck-logo2023w.png\" /> </div>";
        m_outputStr += "<div class=\"inline\"><h1><a href=\"./\">ChucK Class Library Reference</a></h1></div>\n";
        m_outputStr += "<div class=\"inline gray\"><h1>&nbsp;&nbsp;<span style=\"color:black\">/</span>&nbsp;&nbsp;" + m_title + "</h1></div>\n";
        m_outputStr += "</div>\n";
        m_outputStr += "</div>\n";
        m_outputStr += "</div>\n"; // #title
    }

    void end()
    {
        m_outputStr += "</html>\n";
    }

    void title(const std::string &_title)
    {
    }

    void begin_body()
    {
        m_outputStr += "<div id=\"body\">\n";
    }

    void end_body()
    {
        m_outputStr += "</div>\n"; // #body
        m_outputStr += "</body>\n";
    }

    void begin_toc()
    {
        m_outputStr += "<div class=\"toc\"><a id=\"toc\"/>\n";
    }

    void toc_class(Chuck_Type * type)
    {
        m_outputStr += "<p class=\"toc_class\"><a href=\"#" + type->base_name
                     + "\" class=\"" + css_class_for_type(m_env_ref, type)
                     + "\">" + type->base_name + "</a></p>\n";
    }

    void end_toc()
    {
        m_outputStr += "</div>\n";
    }

    void begin_classes( CKDocGroup * group )
    {
        m_outputStr += "<div class=\"classes\">\n";
        m_outputStr += "<p class=\"group_desc\">" + group->desc + "</p>\n";
    }

    void end_classes()
    {
        m_outputStr += "</div>\n";
    }

    void begin_class(Chuck_Type * type, const vector<CKDocGroup *> & groups )
    {
        m_outputStr += "<a name=\"" + type->base_name + "\" /><div class=\"class\">\n";
        m_outputStr += "<h2 class=\"class_title\" name=\"" + type->base_name + "\">" + type->base_name + "</h2>\n";

        // type heirarchy
        Chuck_Type * parent = type->parent;
        // check if there is a parent class
        if( parent != NULL )
        {
            m_outputStr += "<h4>";
            m_outputStr += "<span class=\"class_inherits\">inherits</span> ";
            // m_outputStr += "<h4 class=\"class_hierarchy\">";
        }
        // iterate over parents
        while( parent != NULL )
        {
            m_outputStr += ": <a href=\"" + type2url(parent->base_name, groups)
                         + "\" class=\"" + css_class_for_type(m_env_ref, parent)
                         + "\">" + parent->base_name + "</a> ",
            // go up the inheritance chain
            parent = parent->parent;
        }
        // end the tag
        if( type->parent != NULL ) m_outputStr += "</h4>\n";

        if(type->doc.size() > 0)
        {
            std::string doc = capitalize_and_periodize(type->doc);
            m_outputStr += "<p class=\"class_description\">" + doc + "<p>\n";
        }
        else
            m_outputStr += "<p class=\"empty_class_description\">No description available</p>\n";
    }

    void end_class()
    {
        m_outputStr += "<p class=\"top_link\">[ <a href=\"#top\">top</a> ]</p>\n";
        m_outputStr += "</div>\n<hr />\n";
    }

    void begin_examples()
    {
        m_outputStr += "<h3 class=\"class_section_header\">examples</h3><div class=\"examples\"><ul>\n";
    }

    void end_examples()
    {
        m_outputStr += "</ul></div>\n";
    }

    void begin_static_member_vars()
    {
        m_outputStr += "<h3 class=\"class_section_header\">static member variables</h3>\n<div class=\"members\">\n";
    }

    void end_static_member_vars()
    {
        m_outputStr += "</div>\n";
    }

    void begin_member_vars()
    {
        m_outputStr += "<h3 class=\"class_section_header\">member variables</h3>\n<div class=\"members\">\n";
    }

    void end_member_vars()
    {
        m_outputStr += "</div>\n";
    }

    void begin_static_member_funcs()
    {
        m_outputStr += "<h3 class=\"class_section_header\">static member functions</h3>\n<div class=\"members\">\n";
    }

    void end_static_member_funcs()
    {
        m_outputStr += "</div>\n";
    }

    void begin_ctors() // 1.5.2.0
    {
        m_outputStr += "<h3 class=\"class_section_header\">constructors</h3>\n<div class=\"members\">\n";
    }

    void begin_dtor() // 1.5.2.0
    {
        m_outputStr += "<h3 class=\"class_section_header\">destructor</h3>\n<div class=\"members\">\n";
    }

    void begin_member_funcs()
    {
        m_outputStr += "<h3 class=\"class_section_header\">member functions</h3>\n<div class=\"members\">\n";
    }

    void end_member_funcs()
    {
        m_outputStr += "</div>\n";
    }

    void example(const std::string &name, const std::string &url)
    {
        m_outputStr += "<li><a target=\"_blank\" href=\"" + url + "\">" + name + "</a></li>\n";
    }

    void static_member_var(Chuck_Value * var)
    {
        m_outputStr += "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\""
                     + css_class_for_type(m_env_ref, var->type) + "\">"
                     + var->type->base_name;
        // check array depth
        if( var->type->array_depth )
        {
            m_outputStr += "</span>";
            m_outputStr += "<span class=\"typename\">";
            for(int i = 0; i < var->type->array_depth; i++)
                m_outputStr += "[]";
        }
        m_outputStr += "</span> ";

        // function name
        m_outputStr += "<span class=\"membername\">" + var->name + "</span></p>";

        if(var->doc.size() > 0)
            m_outputStr += "<p class=\"member_description\">" + capitalize_and_periodize(var->doc) + "</p>\n";
        else
            m_outputStr += "<p class=\"empty_member_description\">No description available</p>\n";

        m_outputStr += "</div>\n";
    }

    void member_var(Chuck_Value * var)
    {
        m_outputStr += "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\""
                     + css_class_for_type(m_env_ref, var->type) + "\">" + var->type->base_name;
        // check array depth
        if( var->type->array_depth )
        {
            m_outputStr += "</span>";
            m_outputStr += "<span class=\"typename\">";
            for(int i = 0; i < var->type->array_depth; i++)
                m_outputStr += "[]";
        }
        m_outputStr += "</span> ";

        // function name
        m_outputStr += "<span class=\"membername\">" + var->name + "</span></p>";

        if(var->doc.size() > 0)
            m_outputStr += "<p class=\"member_description\">" + capitalize_and_periodize(var->doc) + "</p>\n";
        else
            m_outputStr += "<p class=\"empty_member_description\">No description available</p>\n";

        m_outputStr += "</div>\n";
    }

    void begin_static_member_func(Chuck_Func * func)
    {
        // return type
        m_outputStr += "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\""
                     + css_class_for_type(m_env_ref, func->def()->ret_type) + "\">"
                    + func->def()->ret_type->base_name;
        // check array
        if( func->def()->ret_type->array_depth )
        {
            m_outputStr += "</span>";
            m_outputStr += "<span class=\"typename\">";
            for(int i = 0; i < func->def()->ret_type->array_depth; i++)
                m_outputStr += "[]";
        }
        m_outputStr += "</span> ";

        // function name
        m_outputStr += "<span class=\"membername\">" + string(S_name(func->def()->name)) + "</span>(";

        // remember
        m_func = func;
    }

    void end_static_member_func()
    {
        // verify
        if( !m_func ) return;

        // finish output string
        m_outputStr += ")</p>\n";

        if(m_func->doc.size() > 0)
            m_outputStr += "<p class=\"member_description\">" + capitalize_and_periodize(m_func->doc) + "</p>\n";
        else
            m_outputStr += "<p class=\"empty_member_description\">No description available</p>\n";

        m_outputStr += "</div>\n";

        // zero out
        m_func = NULL;
    }

    void begin_member_func(Chuck_Func * func)
    {
        // return type
        m_outputStr += "<div class=\"member\">\n<p class=\"member_declaration\"><span class=\""
                     + css_class_for_type( m_env_ref, func->def() ? func->def()->ret_type : NULL )
                     + "\">" + (func->def() ? func->def()->ret_type->base_name.c_str() : "void");
        // check array
        if( func->def() && func->def()->ret_type->array_depth )
        {
            m_outputStr += "</span>";
            m_outputStr += "<span class=\"typename\">";
            for(int i = 0; i < func->def()->ret_type->array_depth; i++)
                m_outputStr += "[]";
        }
        m_outputStr += "</span> ";

        // function name
        m_outputStr += "<span class=\"membername\">" + func->base_name /* string(S_name(func->def()->name))*/ + "</span>(";

        // save for end_member_func()
        m_func = func;
    }

    void end_member_func()
    {
        // verify
        if( !m_func ) return;

        // finish output from before
        m_outputStr += ")</p>\n";

        if( m_func->doc.size() > 0 )
            m_outputStr += "<p class=\"member_description\">" + capitalize_and_periodize( m_func->doc ) + "</p>\n";
        else
            m_outputStr += "<p class=\"empty_member_description\">No description available</p>\n";

        // finish output
        m_outputStr += "</div>\n";

        // zero out
        m_func = NULL;
    }

    void begin_ctor( Chuck_Func * func )
    {
        // return type
        m_outputStr += "<div class=\"member\">\n<p class=\"member_declaration\">";

        // function name
        m_outputStr += "<span class=\"membername\">" + func->base_name /* string(S_name(func->def()->name))*/ + "</span>(";

        // save for end_member_func()
        m_func = func;
    }

    void end_ctor()
    {
        // verify
        if( !m_func ) return;

        // finish output from before
        m_outputStr += ")</p>\n";

        if( m_func->doc.size() > 0 )
            m_outputStr += "<p class=\"member_description\">" + capitalize_and_periodize( m_func->doc ) + "</p>\n";
        else if( m_func->is_ctor && (!m_func->def() || !m_func->def()->arg_list) ) // default ctor?
            m_outputStr += "<p class=\"member_description\">" + capitalize_and_periodize( "Default constructor for " + m_func->base_name ) + "</p>\n";
        else
            m_outputStr += "<p class=\"empty_member_description\">No description available</p>\n";

        // finish output
        m_outputStr += "</div>\n";

        // zero out
        m_func = NULL;
    }

    void func_arg(a_Arg_List arg)
    {
        // argument type
        m_outputStr += "<span class=\"" + css_class_for_type(m_env_ref, arg->type) + "\">" + arg->type->base_name;
        // check array
        if( arg->type->array_depth )
        {
            m_outputStr += "</span>";
            m_outputStr += "<span class=\"typename\">";
            for(int i = 0; i < arg->type->array_depth; i++)
                m_outputStr += "[]";
        }
        m_outputStr += "</span> ";

        // argument name
        std::string varname = varnameclean(S_name(arg->var_decl->xid));
        m_outputStr += varname;

        if(arg->next != NULL)
            m_outputStr += ", ";
    }

public:
    static bool isugen( Chuck_Type *type ) { return type->ugen_info != NULL; }

    static std::string varnameclean( std::string vn )
    {
        // strip []
        vn.erase(std::remove(vn.begin(), vn.end(), '['), vn.end());
        vn.erase(std::remove(vn.begin(), vn.end(), ']'), vn.end());
        return vn;
    }

    static std::string cssclean( std::string s )
    {
        for(int i = 0; i < s.length(); i++)
        {
            if(!isalnum(s[i])) s[i] = '_';
        }
        return s;
    }

    static std::string css_class_for_type( Chuck_Env * env, Chuck_Type * type )
    {
        if( !type || isprim(env, type) || isvoid(env, type) || (type->array_depth && isprim(env, type->array_type)) )
            return "ckdoc_typename ckdoc_type_primitive";
        else if(isugen(type))
            return "ckdoc_typename ckdoc_type_ugen";
        else
            return "ckdoc_typename ckdoc_type_object";
    }
};




//-----------------------------------------------------------------------------
// name: renderIndex()
// desc: render HTML index
//-----------------------------------------------------------------------------
string CKDocHTMLOutput::renderIndex( const string & title, const vector<CKDocGroup *> & groups )
{
    // output string stream
    ostringstream sout;

    // output index header
    sout << "<html>\n<head>\n";
    sout << "<title>" << title << "</title>\n";
    sout << "<link rel=\"stylesheet\" type=\"text/css\" href=\"ckdoc.css\" />\n";
    sout << "<link rel=\"stylesheet\" type=\"text/css\" href=\"class.css\" />\n";

    sout << "<meta charset=\"utf-8\">\n";
    sout << "<meta name=\"keywords\" content=\"ChucK, strongly-timed,audio, programming language, computer music, Artful Design, Stanford University, CCRMA, Princeton University, SoundLab, Ge Wang, Perry Cook, design, art, computer, music, technology, engineering, education\" />\n";
    sout << "<meta property=\"og:url\" content=\"https://ccrma.stanford.edu/software/chuck/doc/reference/\" />\n";
    sout << "<meta property=\"og:type\" content=\"website\" />\n";
    sout << "<meta property=\"og:title\" content=\"ChucK | Class Library Reference\" />\n";
    sout << "<meta property=\"og:description\" content=\"ChucK programming language core class library reference, including base types, unit generators, unit analyzers, standard libraries, and more.\" />\n";
    sout << "<meta name=\"author\" content=\"ChucK Team\">\n";
    sout << "<meta property=\"og:image\" content=\"https://ccrma.stanford.edu/software/chuck/doc/images/chuck-logo2023w.png\" />\n";
    sout << "<meta property=\"fb:app_id\" content=\"966242223397117\" /> <!--default app id-->\n";
    sout << "<link rel=\"canonical\" href=\"http://chuck.stanford.edu/doc/reference\" />\n";

    sout <<  "</head>\n";
    sout <<  "<body>\n";
    sout <<  "<div id=\"title\">\n";
    sout <<  "<div class=\"titleL\">\n";
    sout <<  "<div style=\"display: flex; align-items: center;\">";
    sout <<  "<div class=\"inline titlelogo\"><a href=\"https://chuck.stanford.edu/\" target=\"_blank\"><img src=\"https://chuck.stanford.edu/doc/images/chuck-logo2023w.png\" /> </div>";
    sout <<  "<div class=\"inline\"><h1>" << title <<  "</h1></div>\n";
    sout <<  "</div>\n";
    sout <<  "</div>\n";
    sout <<  "</div>\n";
    sout <<  "<div id=\"body\">\n";

    // loop over groups
    for( t_CKINT i = 0; i < groups.size(); i++ )
    {
        CKDocGroup * group = groups[i];
        // type A or B output | 1.5.2.4 (ge, azaday) added
        // A == normal; B == group points to external CKDoc, e.g., ChuGL
        t_CKBOOL typeA = group->types.size() != 0;

        sout << "<div class=\"index_group\">\n";
        sout << "<div class=\"index_group_title\">\n";

        // group name, either linking to generate page (Type A) to external page (Type B)
        // 1.5.2.4 (ge, azaday) add type B support
        if( typeA ) sout << "<h2><a href=\"" << group->shortName << ".html\">";
        else sout << "<h2><a target=\"_blank\" href=\"" << group->shortName << "\">";
        sout << group->name << "</a></h2>\n";

        sout << "<p class=\"index_group_desc\">" << group->desc << "</p>\n";
        sout << "</div>\n";

        // type A == normal
        if( typeA )
        {
            sout << "<div class=\"index_group_classes\">\n";
            sout << "<p>\n";
            for( t_CKINT j = 0; j < group->types.size(); j++ )
            {
                Chuck_Type * type = group->types[j];
                if( !type ) continue;
                string cssClass = css_class_for_type( m_env_ref, type );
                // TODO: check for array_depth
                sout << "<a href=\"" << group->shortName << ".html#" << type->base_name << "\" class=\"" << cssClass << "\">" << type->base_name << "</a>\n";
            }
        }
        else // type B | 1.5.2.4 (ge, azaday) added
        {
            sout << "<div class=\"index_group_external\">\n";
            sout << "<p>\n";
            sout << group->longDesc << "\n";
        }

        sout << "</p>\n";
        sout << "</div>\n";
        sout << "<div class=\"clear\"></div>\n";
        sout << "</div>\n";
    }
    sout << "</div>\n";
    sout << "</body>\n</html>\n";

    // return the string literal
    return sout.str();
};



//-----------------------------------------------------------------------------
// name: CKDoc()
// desc: constructor
//-----------------------------------------------------------------------------
CKDoc::CKDoc( Chuck_VM * vm )
{
    // copy VM reference
    m_vm_ref = vm;
    // reset
    m_format = FORMAT_NONE;
    m_output = NULL;

    // default
    setOutputFormat( FORMAT_HTML );
}




//-----------------------------------------------------------------------------
// name: ~CKDoc()
// desc: destructor
//-----------------------------------------------------------------------------
CKDoc::~CKDoc()
{
    // clear groups
    clearGroups();
    // clear output generator
    clearOutput();
    // zero out
    m_vm_ref = NULL;
}




//-----------------------------------------------------------------------------
// name: clearGroups()
// desc: clear all added groups
//-----------------------------------------------------------------------------
void CKDoc::clearGroups()
{
    // iterate groups
    for( t_CKINT i = 0; i < m_groups.size(); i++ )
    {
        // iterate over a group
        for( t_CKINT j = 0; j < m_groups[i]->types.size(); j++ )
        {
            // release ref count
            CK_SAFE_RELEASE( m_groups[i]->types[j] );
        }
        // deallocate the group struct
        CK_SAFE_DELETE( m_groups[i] );
    }

    // clear the arrar
    m_groups.clear();
}




//-----------------------------------------------------------------------------
// name: clearOutput()
// desc: clear output generator
//-----------------------------------------------------------------------------
void CKDoc::clearOutput()
{
    // reste
    m_format = FORMAT_NONE;
    // deallocate
    CK_SAFE_DELETE( m_output );
}




//-----------------------------------------------------------------------------
// name: addGroup()
// add a group of types to document
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::addGroup( const vector<Chuck_Type *> & types, const string & name,
                          const string & shortName, const string & description )
{
    // trim
    string shortNameTrim = trim(shortName);
    // check for empty string; shortName is needed as filename for outputToDir()
    if( shortNameTrim == "" )
    {
        EM_error3( "[CKDoc.addGroup()]: 'shortName' parameter cannot be empty" );
        return FALSE;
    }
    // check for conflicts
    for( t_CKINT i = 0; i < m_groups.size(); i++ )
    {
        if( shortNameTrim == m_groups[i]->shortName )
        {
            EM_error3( "[CKDoc.addGroup()]: duplicate shortName found: '%s'...", shortNameTrim.c_str() );
            return FALSE;
        }
    }

    // allocate group
    CKDocGroup * group = new CKDocGroup();
    // check
    if( !group )
    {
        EM_error3( "[CKDoc.addGroup()]: could not allocate new memory...bailing out" );
        return FALSE;
    }
    // copy
    group->name = trim(name);
    group->shortName = shortNameTrim;
    group->desc = trim(description);
    // iterate
    for( t_CKINT i = 0; i < types.size(); i++ )
    {
        // filter out NULL elements
        if( types[i] == NULL ) continue;

        // ref count
        CK_SAFE_ADD_REF( types[i] );
        // append
        group->types.push_back( types[i] );
    }
    // add to groups
    m_groups.push_back( group );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: addGroupExt() | 1.5.2.4 (ge) added
// desc: add an external group to document
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::addGroupExt( const string & name, const string & URL,
                             const string & desc, const string & longDesc )
{
    // allocate group
    CKDocGroup * group = new CKDocGroup();
    // check
    if( !group )
    {
        EM_error3( "[CKDoc.addGroup()]: could not allocate new memory...bailing out" );
        return FALSE;
    }

    // copy
    group->name = trim(name);
    group->shortName = trim(URL);
    group->desc = trim(desc);
    group->longDesc = trim(longDesc);

    // add to groups
    m_groups.push_back( group );

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: numGroups()
// desc: get number of groups
//-----------------------------------------------------------------------------
t_CKINT CKDoc::numGroups() const
{ return m_groups.size(); }




//-----------------------------------------------------------------------------
// name: setOutputFormat()
// desc: set output format: CKDoc.HTML, CKDoc.TEXT, CKDoc.MARKDOWN, CKDoc.JSON
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::setOutputFormat( t_CKINT which )
{
    // clear output
    clearOutput();
    // format names
    const char * formats[] = { "NONE", "HTML", "TEXT", "MARKDOWN", "JSON" };

    // copy for now
    m_format = which;
    // check
    switch( which )
    {
        // currently supported
        case FORMAT_NONE: break;
        case FORMAT_HTML:
            m_output = new CKDocHTMLOutput( m_vm_ref );
            break;

        // currently unsupported
        case FORMAT_TEXT:
        case FORMAT_MARKDOWN:
        case FORMAT_JSON:
            EM_error3( "[CKDoc]: unsupported format '%s'...", formats[which] );
            goto error;

        // unrecognized
        default:
            EM_error3( "[CKDoc]: unrecognized format ID '%i'...", which );
            goto error;
    }

    return TRUE;

error:
    // set to none
    m_format = FORMAT_NONE;
    // return false
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: getOutputFormat()
// desc: get output format
//-----------------------------------------------------------------------------
t_CKINT CKDoc::getOutpuFormat() const
{
    return m_format;
}




//-----------------------------------------------------------------------------
// name: setExamplesRoot()
// desc: set base examples root path
//-----------------------------------------------------------------------------
void CKDoc::setExamplesRoot( const string & path )
{
    // set
    m_examplesRoot = path;
    // trim
    m_examplesRoot = trim( m_examplesRoot );
    // process
    if( m_examplesRoot.length() == 0 )
    { m_examplesRoot = "./"; }
    // check for trailing /
    if( m_examplesRoot[m_examplesRoot.length()-1] != '/' )
    { m_examplesRoot += "/"; }
}




//-----------------------------------------------------------------------------
// name: getExamplesRoot()
// desc: get base examples root path
//-----------------------------------------------------------------------------
string CKDoc::getExamplesRoot() const
{
    // set
    return m_examplesRoot;
}




//-----------------------------------------------------------------------------
// name: genIndex()
// desc: generate top-level index file; return as string
//-----------------------------------------------------------------------------
string CKDoc::genIndex( const string & title )
{
    // check
    if( !m_output ) return "";
    // generate index
    return m_output->renderIndex( title, m_groups );
}




//-----------------------------------------------------------------------------
// name: genCSS()
// desc: generate CSS; return as string
//-----------------------------------------------------------------------------
string CKDoc::genCSS()
{
    // return the string literal
    return m_ckdocCSS;
}




//-----------------------------------------------------------------------------
// name: genGroups()
// desc: generate all added groups, return each in a separate entry
//-----------------------------------------------------------------------------
void CKDoc::genGroups( vector<string> & results )
{
    // clear results array
    results.clear();
    // set the size
    results.resize( m_groups.size() );

    // iterate over added groups
    for( t_CKINT i = 0; i < m_groups.size(); i++ )
    {
        // generate each group
        results[i] = genGroup( m_groups[i], TRUE );
    }
}




//-----------------------------------------------------------------------------
// name: genGroup()
// desc: generate documentation for a group
//-----------------------------------------------------------------------------
string CKDoc::genGroup( CKDocGroup * group, t_CKBOOL clearOutput )
{
    // check
    if( m_output == NULL ) return "[NO OUTPUT FORMAT SELECTED (genGroup)]";

    bool do_toc = true;
    bool do_heading = true;
    list<string> type_args;

    // the output
    CKDocOutput * output = m_output;
    // clear output
    if( clearOutput ) output->clear();

    // output group title
    output->begin( group->name );
    // do heading
    if( do_heading ) output->heading();

    // begin output body
    output->begin_body();

    // table of contents
    if( do_toc )
    {
        // begin TOC
        output->begin_toc();

        // iterate
        for( vector<Chuck_Type *>::const_iterator t = group->types.begin(); t != group->types.end(); t++ )
        {
            // get the current type
            Chuck_Type * type = *t;
            // skip
            // if( skip(type->base_name) ) continue;
            // output the type in TOC
            output->toc_class( type );
        }

        // end TOC
        output->end_toc();
    }

    // begin type contents
    output->begin_classes( group );

    // iterate
    for( vector<Chuck_Type *>::const_iterator t = group->types.begin(); t != group->types.end(); t++ )
    {
        // skip
        // if( skip(type->base_name) ) continue;
        // gen type, don't clear output
        genType( *t, FALSE );
    }

    // end types
    output->end_classes();
    // end file
    output->end_body();
    // end output
    output->end();

    // return output string
    return output->str();
}




//-----------------------------------------------------------------------------
// name: genType()
// desc: generate documentation for a single Type
//-----------------------------------------------------------------------------
string CKDoc::genType( Chuck_Type * type, t_CKBOOL clearOutput )
{
    // check
    if( m_output == NULL ) return "[NO OUTPUT FORMAT SELECTED (getType)]";

    // the output
    CKDocOutput * output = m_output;
    // clear it
    if( clearOutput ) output->clear();

    // begin the type
    output->begin_class( type, m_groups );

    // output examples
    if( type->examples.size() )
    {
        // begin examples
        output->begin_examples();

        // iterate over examples
        for( vector<string>::iterator _ex = type->examples.begin(); _ex != type->examples.end(); _ex++ )
        {
            string ex = *_ex;
            string name;
            string url;

            // test for absolute vs. relative URL
            if( ex.find("http://") == 0 || ex.find("https://") == 0 )
            {
                // absolute URL
                url = ex;
                // find the example name without path
                std::basic_string<char>::size_type pos = ex.rfind('/');
                if( pos != string::npos )
                    name = string(ex, pos+1);
                else
                    name = ex;
            }
            else
            {
                // relative URL
                url = m_examplesRoot + ex;
                name = ex;
            }
            // output examples
            output->example(name, url);
        }

        // done with examples
        output->end_examples();
    }

    // check type info
    if( type->info )
    {
        // get functions
        vector<Chuck_Func *> funcs;
        type->info->get_funcs( funcs );
        // get values
        vector<Chuck_Value *> vars;
        type->info->get_values( vars );
        // function names
        map<string, int> func_names;

        // member and static functions and values
        vector<Chuck_Func *> ctors;
        Chuck_Func * dtor = NULL;
        vector<Chuck_Func *> mfuncs;
        vector<Chuck_Func *> sfuncs;
        vector<Chuck_Value *> mvars;
        vector<Chuck_Value *> svars;
        // fprintf(stderr, "### %lu vars\n", vars.size());

        // iterate through values
        for( vector<Chuck_Value *>::iterator v = vars.begin(); v != vars.end(); v++ )
        {
            // the value
            Chuck_Value * value = *v;

            // check
            if(value == NULL) continue;
            // zero length name
            if( value->name.length() == 0 )
                continue;
            // special internal values
            if( value->name.size() && value->name[0] == '@' )
                continue;
            // value is a function
            if( isa( value->type, value->type->env()->ckt_function ) )
                continue;

            // static or instance?
            if( value->is_static ) svars.push_back( value );
            else mvars.push_back( value );
        }

        // iterate over functions
        for( vector<Chuck_Func *>::iterator f = funcs.begin(); f != funcs.end(); f++ )
        {
            // the function
            Chuck_Func * func = *f;

            // check
            if(func == NULL) continue;
            // if already seen (overloaded?)
            if( func_names.count(func->name) )
                continue;
            // first one
            func_names[func->name] = 1;
            // static or instance?
            if( func->is_static ) sfuncs.push_back(func);
            else
            {
                // constructor?
                if( func->is_ctor ) ctors.push_back(func);
                // destructor?
                else if( func->is_dtor ) dtor = func;
                // other member function?
                else mfuncs.push_back(func);
            }
        }

        // sort
        sort( svars.begin(), svars.end(), ck_comp_value );
        sort( mvars.begin(), mvars.end(), ck_comp_value );
        sort( sfuncs.begin(), sfuncs.end(), ck_comp_func );
        sort( mfuncs.begin(), mfuncs.end(), ck_comp_func );
        sort( ctors.begin(), ctors.end(), ck_comp_func_args );

        // whether to potentially insert a default constructor | 1.5.2.0
        t_CKBOOL insertDefaultCtor = type_engine_has_implicit_def_ctor( type );

        // constructors | 1.5.2.0 (ge) added
        if( ctors.size() || insertDefaultCtor )
        {
            // begin member functions
            output->begin_ctors();

            // add default constructor, if non-explicitly specified
            if( ((ctors.size() == 0 || (ctors.size() && ctors[0]->def()->arg_list))) && insertDefaultCtor )
            {
                Chuck_Func ftemp;
                ftemp.base_name = type->base_name;
                ftemp.doc = "default constructor for " + type->base_name;
                // begin the func
                output->begin_ctor( &ftemp );
                // but don't use the args
                output->end_ctor();
            }

            // iterate
            for( vector<Chuck_Func *>::iterator f = ctors.begin(); f != ctors.end(); f++ )
            {
                // the func
                Chuck_Func * func = *f;
                // begin the func
                output->begin_ctor(func);
                // argument list
                a_Arg_List args = func->def()->arg_list;
                while(args != NULL)
                {
                    // output argument
                    output->func_arg(args);
                    args = args->next;
                }
                // end the func
                output->end_ctor();
            }

            // end member functions
            output->end_member_funcs();
        }

        // destructor | 1.5.2.0 (ge) added but not added
        // if( dtor )
        // {
        //    // begin member functions
        //    output->begin_dtor();
        //    // begin the func | no args
        //    output->begin_member_func(dtor);
        //    // end the func
        //    output->end_member_func();
        //    // end member functions
        //    output->end_member_funcs();
        // }

        // member functions
        if( mfuncs.size() )
        {
            // begin member functions
            output->begin_member_funcs();
            // iterate
            for( vector<Chuck_Func *>::iterator f = mfuncs.begin(); f != mfuncs.end(); f++ )
            {
                // the func
                Chuck_Func * func = *f;
                // begin the func
                output->begin_member_func(func);
                // argument list
                a_Arg_List args = func->def()->arg_list;
                while(args != NULL)
                {
                    // output argument
                    output->func_arg(args);
                    args = args->next;
                }
                // end the func
                output->end_member_func();
            }
            // end member functions
            output->end_member_funcs();
        }

        // static functions
        if( sfuncs.size() )
        {
            // begin static functions
            output->begin_static_member_funcs();
            // iterate
            for( vector<Chuck_Func *>::iterator f = sfuncs.begin(); f != sfuncs.end(); f++ )
            {
                // the func
                Chuck_Func * func = *f;
                // begin output
                output->begin_static_member_func(func);
                // argument list
                a_Arg_List args = func->def()->arg_list;
                while(args != NULL)
                {
                    // output argument
                    output->func_arg( args );
                    args = args->next;
                }
                // end output
                output->end_static_member_func();
            }
            // end static functions
            output->end_static_member_funcs();
        }

        // member vars
        if( mvars.size() )
        {
            // start
            output->begin_member_vars();
            // iterate
            for( vector<Chuck_Value *>::iterator v = mvars.begin(); v != mvars.end(); v++ )
                output->member_var(*v);
            // end
            output->end_member_vars();
        }

        // static vars
        if( svars.size() )
        {
            // start output
            output->begin_static_member_vars();
            // iterate
            for( vector<Chuck_Value *>::iterator v = svars.begin(); v != svars.end(); v++ )
                output->static_member_var(*v);
            // end output
            output->end_static_member_vars();
        }
    }
    // end the type
    output->end_class();

    // return result
    return output->str();
}




//-----------------------------------------------------------------------------
// name: outputToDir()
// desc: generate everything as files into the output directory
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputToDir( const string & outputDir, const string & indexTitle )
{
    // groups content
    vector<string> groupOutput;
    // trim
    string path = trim( outputDir );
    // process
    if( path.length() == 0 ) { path = "./"; }
    // check for trailing /
    if( path[path.length()-1] != '/' ) { path += "/"; }
    // clear output
    m_output->clear();

    // gen index
    if( !outputToFile( path + "index" + m_output->fileExtension(), genIndex( indexTitle ) ) ) goto error;
    // gen CSS
    if( !outputToFile( path + "ckdoc.css", genCSS() ) ) goto error;
    // gen groups
    genGroups( groupOutput );
    // for each group
    for( t_CKINT i = 0; i < m_groups.size(); i++ )
    {
        // check if Type A or Type B | 1.5.2.4 (ge) added check to distinguish group type
        if( m_groups[i]->types.size() )
        {
            // not external, output new file
            if( !outputToFile( path + m_groups[i]->shortName + m_output->fileExtension(), groupOutput[i] ) )
                goto error;
        }
    }

    // done
    return TRUE;

error:
    // error
    EM_error3( "CKDoc: error encountered in outputToDir(...); bailing out..." );
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: outputToFile()
// desc: write string to file
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputToFile( const std::string & filename, const std::string & content )
{
    // open file
    ofstream fout( filename );
    // if not good
    if( !fout.good() )
    {
        EM_error3( "CKDoc: cannot open file '%s' for output...", filename.c_str() );
        return FALSE;
    }
    // write
    fout << content;
    // done
    fout.close();
    // success
    return TRUE;
}




//-----------------------------------------------------------------------------
// CKDoc implementation
//-----------------------------------------------------------------------------
CK_DLL_CTOR( CKDoc_ctor )
{
    CKDoc * ckdoc = new CKDoc(VM);
    OBJ_MEMBER_UINT( SELF, CKDoc_offset_data ) = (t_CKUINT)ckdoc;
}

CK_DLL_DTOR( CKDoc_dtor )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT( SELF, CKDoc_offset_data );
    CK_SAFE_DELETE( ckdoc );
    OBJ_MEMBER_UINT( SELF, CKDoc_offset_data ) = 0;
}

CK_DLL_MFUN( CKDoc_addGroup_type )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_ArrayInt * typeArray = (Chuck_ArrayInt *)GET_NEXT_OBJECT(ARGS);
    Chuck_String * groupName = GET_NEXT_STRING(ARGS);
    Chuck_String * shortName = GET_NEXT_STRING(ARGS);
    Chuck_String * description = GET_NEXT_STRING(ARGS);

    // check if null
    if( typeArray == NULL )
    {
        CK_FPRINTF_STDERR( "CKDoc.addGroup(): null 'types' array; nothing added...\n" );
        goto error;
    }
    else if( shortName == NULL || shortName->str() == "" )
    {
        CK_FPRINTF_STDERR( "CKDoc.addGroup(): 'shortName' must be non-empty and unique; nothing added...\n" );
        goto error;
    }
    else
    {
        vector<Chuck_Type *> types;
        for( int i = 0; i < typeArray->m_vector.size(); i++ )
        {
            // get pointer as chuck string
            Chuck_Type * t = (Chuck_Type *)typeArray->m_vector[i];
            // check
            if( t != NULL )
            {
                // append
                types.push_back( t );
            }
        }
        // check
        if( types.size() == 0 )
        {
            CK_FPRINTF_STDERR( "CKDoc.addGroup(): empty 'types' array; nothing added...\n" );
            goto error;
        }

        // add group and return success code
        RETURN->v_int = ckdoc->addGroup( types,
                        groupName ? groupName->str() : "",
                        shortName ? shortName->str() : "",
                        description ? description->str() : "" );
    }
    // done
    return;

error:
    RETURN->v_int = FALSE;
}

CK_DLL_MFUN( CKDoc_addGroup_str )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_ArrayInt * strArray = (Chuck_ArrayInt *)GET_NEXT_OBJECT(ARGS);
    Chuck_String * groupName = GET_NEXT_STRING(ARGS);
    Chuck_String * shortName = GET_NEXT_STRING(ARGS);
    Chuck_String * desc = GET_NEXT_STRING(ARGS);
    // check if null
    if( strArray == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck]: CKDoc.addGroup() given null 'typeNames' array; nothing added...\n" );
        goto error;
    }
    else
    {
        // type names
        vector<string> typeNames;
        // get the names from chuck strings to c++ strings
        for( int i = 0; i < strArray->m_vector.size(); i++ )
        {
            // get pointer as chuck string
            Chuck_String * s = (Chuck_String *)strArray->m_vector[i];
            // check
            if( s != NULL )
            {
                // append
                typeNames.push_back( s->str() );
            }
        }
        // types
        vector<Chuck_Type *> types;
        // convert to types
        type_engine_names2types( VM->env(), typeNames, types );

        // check
        if( types.size() == 0 )
        {
            CK_FPRINTF_STDERR( "[chuck]: CKDoc.addGroup() given 'typeNames' array has no associated Types; nothing added...\n" );
            goto error;
        }

        // add group
        ckdoc->addGroup( types,
                         groupName ? groupName->str() : "",
                         shortName ? shortName->str() : "",
                         desc ? desc->str() : "" );
    }

    // set return value
    RETURN->v_int = TRUE;
    // done
    return;

error:
    RETURN->v_int = FALSE;
}

CK_DLL_MFUN( CKDoc_addGroup_ext ) // 1.5.2.4 (ge) added
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * groupName = GET_NEXT_STRING(ARGS);
    Chuck_String * URL = GET_NEXT_STRING(ARGS);
    Chuck_String * desc = GET_NEXT_STRING(ARGS);
    Chuck_String * longDesc = GET_NEXT_STRING(ARGS);

    // add group
    ckdoc->addGroupExt( groupName ? groupName->str() : "",
                        URL ? URL->str() : "",
                        desc ? desc->str() : "",
                        longDesc ? longDesc->str() : "" );

    // set return value
    RETURN->v_int = TRUE;
    // done
    return;
}

CK_DLL_MFUN( CKDoc_numGroups )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    // return (could be different than requested)
    RETURN->v_int = ckdoc->numGroups();
}

CK_DLL_MFUN( CKDoc_clearGroup )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    // clear
    ckdoc->clearGroups();
}

CK_DLL_CTRL( CKDoc_examplesRoot_set )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * path = GET_NEXT_STRING(ARGS);
    // check if null
    if( path == NULL ) {
        // default
        ckdoc->setExamplesRoot( "./" );
    } else {
        // set examples root
        ckdoc->setExamplesRoot( trim( path->str() ) );
    }
    // set return value
    RETURN->v_string = path;
}

CK_DLL_CGET( CKDoc_examplesRoot_get )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( ckdoc->getExamplesRoot() );
    RETURN->v_string = str;
}

CK_DLL_CTRL( CKDoc_outputFormat_set )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    t_CKINT format = GET_NEXT_INT(ARGS);
    // attempt to set
    ckdoc->setOutputFormat( format );
    // return (could be different than requested)
    RETURN->v_int = ckdoc->getOutpuFormat();
}

CK_DLL_CGET( CKDoc_outputFormat_get )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    // return
    RETURN->v_int = ckdoc->getOutpuFormat();
}

CK_DLL_MFUN( CKDoc_genIndex )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * indexTitle = GET_NEXT_STRING(ARGS);
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( ckdoc->genIndex( indexTitle != NULL ? indexTitle->str() : "" ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( CKDoc_genCSS )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( ckdoc->genCSS() );
    RETURN->v_string = str;
}

CK_DLL_MFUN( CKDoc_genGroups )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    // the return array
    Chuck_ArrayInt * strArray = (Chuck_ArrayInt *)GET_NEXT_OBJECT(ARGS);
    // the c++ return vector
    vector<string> results;
    // gen the groups
    ckdoc->genGroups( results );
    // clear the return array
    strArray->clear();

    // loop over results
    for( t_CKINT i = 0; i < results.size(); i++ )
    {
        // instantiate chuck string
        Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
        // set into chuck string
        str->set( ckdoc->genCSS() );
        // push back
        strArray->push_back( (t_CKINT)str );
    }
}

CK_DLL_MFUN( CKDoc_genType_type )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_Type * type = (Chuck_Type *)GET_NEXT_OBJECT(ARGS);
    // return something
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    // gen type
    str->set( type ? ckdoc->genType( type ) : "" );
    // set return
    RETURN->v_string = str;
}

CK_DLL_MFUN( CKDoc_genType_str )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * typeName = GET_NEXT_STRING(ARGS);
    // c++ string
    string name = typeName ? typeName->str() : "";
    // chuck type
    Chuck_Type * type = type_engine_find_type( VM->env(), typeName->str() );
    // return something
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    // gen type
    str->set( type ? ckdoc->genType( type ) : "" );
    // set return
    RETURN->v_string = str;
}

CK_DLL_MFUN( CKDoc_outputToDir )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * path = GET_NEXT_STRING(ARGS);
    Chuck_String * indexTitle = GET_NEXT_STRING(ARGS);
    // the path as c++ string
    string thePath;
    string theTitle;

    // check
    theTitle = indexTitle != NULL ? indexTitle->str() : "";

    // check if null
    if( path == NULL ) {
        thePath = ".";
    } else {
        // set output dir
        thePath = trim( path->str() );
    }

    // output
    ckdoc->outputToDir( thePath, theTitle );
}




// the default CSS (ckdoc.css), literally
string CKDoc::m_ckdocCSS = "\
html\n\
{\n\
    margin: 0;\n\
    padding: 0;\n\
}\n\
\n\
body\n\
{\n\
    margin-left: 2em;\n\
    margin-right: 2em;\n\
    font-family: Helvetica, sans-serif;\n\
    font-size: 14px;\n\
}\n\
\n\
a\n\
{\n\
    scroll-margin-top: 80px;\n\
}\n\
\n\
\n\
.index_group\n\
{\n\
    clear: both;\n\
    background-color: #eee;\n\
    padding: 1em 1em 1em 1em;\n\
    margin-bottom: 2em;\n\
    vertical-align: top;\n\
}\n\
\n\
.index_group_title\n\
{\n\
    float: left;\n\
    margin: 0;\n\
    width: 15%;\n\
    padding-right: 3em;\n\
    border-right: 4px solid #fcfcfc;\n\
}\n\
\n\
.index_group_classes\n\
{\n\
    padding-top: 0.15em;\n\
    padding-left: 1em;\n\
    padding-bottom: 0.15em;\n\
\n\
    margin: 0;\n\
    margin-left: 20%;\n\
    width: 75%;\n\
}\n\
\n\
.index_group_classes a\n\
{\n\
    margin-right: 0.5em;\n\
    line-height: 1.5em;\n\
}\n\
\n\
.index_group_external\n\
{\n\
    padding-top: 0.15em;\n\
    padding-left: 1em;\n\
    padding-bottom: 0.15em;\n\
\n\
    margin: 0;\n\
    margin-left: 20%;\n\
    width: 75%;\n\
}\n\
\n\
.index_group_external a\n\
{\n\
    margin-right: 0;\n\
    line-height: 1.5em;\n\
}\n\
\n\
.index_group_desc\n\
{\n\
    color: #555;\n\
}\n\
\n\
.group_desc\n\
{\n\
    color: #000;\n\
}\n\
\n\
.index_group_title a\n\
{\n\
    color: black;\n\
}\n\
\n\
#title\n\
{\n\
    position: fixed;\n\
    top: 0;\n\
    left: 0;\n\
    right: 0;\n\
    border-bottom: 1px solid black;\n\
    padding: 0.5em 2em;\n\
    margin-bottom: 1.5em;\n\
    background-color: white;\n\
    float: left;\n\
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n\
}\n\
\n\
.titleL\n\
{\n\
    float: left;\n\
    width: auto;\n\
    /* vertically center */\n\
    line-height: 48px;\n\
    height: 48px;\n\
}\n\
\n\
.gray\n\
{\n\
    color: gray;\n\
}\n\
\n\
.inline\n\
{\n\
    display: inline;\n\
}\n\
\n\
\n\
.titleR\n\
{\n\
    float: right;\n\
    width: initial;\n\
    clear: none;\n\
}\n\
\n\
.titleLogo\n\
{\n\
    width: 48px;\n\
    height: 48px;\n\
    padding-right: 8px;\n\
}\n\
\n\
.titleLogo img\n\
{\n\
    width: 48px;\n\
    height: 48px;\n\
}\n\
\n\
h1\n\
{\n\
    font-size: 28px;\n\
    margin: 0;\n\
}\n\
\n\
h1 a\n\
{\n\
    color: black;\n\
    text-decoration: none;\n\
}\n\
\n\
h1 a:hover\n\
{\n\
    color: blue;\n\
    text-decoration: underline;\n\
}\n\
\n\
#body\n\
{\n\
    clear: both;\n\
    margin-top: 80px;\n\
}\n\
\n\
.toc\n\
{\n\
    float: left;\n\
    position: fixed;\n\
    top: 80px;\n\
    clear: both;\n\
    background-color: #eee;\n\
    padding: 0.25em 0.5em 1em 0.75em;\n\
    margin: 0;\n\
    width: 12.5em;\n\
    overflow-x: auto;\n\
    overflow-y: auto;\n\
    max-height: calc(100% - 100px);\n\
    margin-bottom: 20px;\n\
}\n\
\n\
.toc_class\n\
{\n\
    font-size: 12px;\n\
    margin-top: 0.5em;\n\
    margin-bottom: 0.25em;\n\
}\n\
\n\
.classes\n\
{\n\
    clear: right;\n\
    margin-left: 15em;\n\
    margin-right: 0em;\n\
    margin-top: 80px;\n\
}\n\
\n\
.class\n\
{\n\
    background-color: #eee;\n\
    padding: 0.025em 0.5em 0.25em 1em;\n\
}\n\
\n\
.class_title\n\
{\n\
    font-family: Menlo, Monaco, Courier;\n\
    font-size: 18px;\n\
}\n\
\n\
.class_inherits\n\
{\n\
    font-weight: normal;\n\
}\n\
\n\
.class_hierarchy\n\
{\n\
    font-family: Menlo, Monaco, Courier;\n\
    font-size: 14px;\n\
    /* margin-right: 4em; */\n\
}\n\
\n\
.class_section_header\n\
{}\n\
\n\
.class_description\n\
{\n\
    font-family: Helvetica;\n\
    font-size: 14px;\n\
    margin-right: 4em;\n\
}\n\
\n\
.empty_class_description\n\
{\n\
    font-family: Helvetica;\n\
    font-size: 12px;\n\
    font-style: italic;\n\
    color: grey;\n\
    margin-right: 4em;\n\
}\n\
\n\
.members\n\
{\n\
    font-family: Menlo, Monaco, Courier;\n\
    font-size: 13px;\n\
}\n\
\n\
.member\n\
{\n\
    margin-left: 0em;\n\
    padding-left: 0.85em;\n\
    padding-top: 0.85em;\n\
    border-top: 4px solid #fcfcfc;\n\
    border-left: 4px solid #fcfcfc;\n\
\n\
    padding-bottom: 0.85em;\n\
    padding-left: 0.85em;\n\
    margin-bottom: 1.5em;\n\
\n\
    margin-right: 4em;\n\
    padding-right: 0;\n\
}\n\
\n\
.member_declaration\n\
{\n\
    margin-top: 0;\n\
    margin-bottom: 0.5em;\n\
}\n\
\n\
.member_description\n\
{\n\
    font-family: Helvetica;\n\
    font-size: 14px;\n\
    margin-top: 0;\n\
    margin-bottom: 0;\n\
}\n\
\n\
.empty_member_description\n\
{\n\
    font-family: Helvetica;\n\
    font-size: 12px;\n\
    font-style: italic;\n\
    color: grey;\n\
    margin-top: 0;\n\
    margin-bottom: 0;\n\
}\n\
\n\
.ckdoc_typename\n\
{\n\
    font-family: Menlo, Monaco, Courier;\n\
    font-size: 13px;\n\
}\n\
\n\
.membername\n\
{\n\
    font-weight: bold;\n\
}\n\
\n\
.top_link\n\
{\n\
    text-align: right;\n\
    margin-top: 2em;\n\
    margin-right: 1em;\n\
}\n\
\n\
.top_link a\n\
{\n\
    color: #44F;\n\
}\n\
\n\
.examples ul\n\
{\n\
    margin-top: -1em;\n\
}\n\
\n\
.examples ul li a\n\
{\n\
    text-decoration: none;\n\
}\n\
\n\
.clear\n\
{\n\
    clear: both;\n\
}\n\
\n\
.ckdoc_type_primitive\n\
{\n\
    color: blue;\n\
}\n\
\n\
.ckdoc_type_ugen\n\
{\n\
    color: #A200EC;\n\
}\n\
\n\
.ckdoc_type_object\n\
{\n\
    color: #800023;\n\
}\n\
";
