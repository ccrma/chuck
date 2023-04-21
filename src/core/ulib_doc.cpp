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
#include "ulib_doc.h"
#include "chuck.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_instr.h"
#include "chuck_compile.h"
#include "chuck_errmsg.h"

#include <string>
#include <algorithm>
using namespace std;

// 1.5.0.0 ((ge, spencer) | added
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
    func->add_arg( "string", "path" );
    func->doc = "Set the output directory; this will be automatically prepended to relative-path filenames for generated files.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // outputDir
    func = make_new_mfun( "string", "outputDir", CKDoc_outputDir_get );
    func->doc = "Get the output directory.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genCSS
    func = make_new_mfun( "int", "genCSS", CKDoc_genCSS );
    func->add_arg( "Type[]", "types" );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Generate the CSS file named in 'cssFilename'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genHTMLGroup
    func = make_new_mfun( "int", "genHTMLGroup", CKDoc_genHTMLGroup );
    func->add_arg( "Type[]", "types" );
    func->add_arg( "string", "groupTitle" );
    func->add_arg( "string", "htmlFilename" );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Generate the group HTML with 'groupTitle'in 'htmlFilename' for types in the array 'types', referencing the CSS file named in 'cssFilename'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genHTMLIndex
    func = make_new_mfun( "int", "genHTMLIndex", CKDoc_genHTMLIndex );
    func->add_arg( "Type[]", "types" );
    func->add_arg( "string", "htmlFilename" );
    func->add_arg( "string", "cssFilename" );
    func->doc = "Generate the index HTML file named 'htmlFilename' for types in the array 'types', referencing the CSS file named in 'cssFilename'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genMarkdownGroup
    func = make_new_mfun( "int", "genMarkdownGroup", CKDoc_genMarkdownGroup );
    func->add_arg( "Type[]", "types" );
    func->add_arg( "string", "mdFilename" );
    func->doc = "Generate the document Markdown file named 'mdFilename' for types in the array 'types'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // genMarkdownIndex
    func = make_new_mfun( "int", "genMarkdownIndex", CKDoc_genMarkdownIndex );
    func->add_arg( "Type[]", "types" );
    func->add_arg( "string", "mdFilename" );
    func->doc = "Generate the index Markdown file named 'mdFilename' for types in the array 'types'.";
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
    CKDocHTMLOutput( Chuck_VM * vm, FILE * output = stdout )
        : m_vm(vm), m_output(output), m_func(NULL) { }

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
                type->name.c_str(), css_class_for_type(m_vm->env(), type), type->name.c_str());
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
                    type2url(parent->name).c_str(), css_class_for_type(m_vm->env(), parent), parent->name.c_str());
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
                css_class_for_type(m_vm->env(), var->type), var->type->name.c_str());
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
                css_class_for_type(m_vm->env(), var->type), var->type->name.c_str());
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
                css_class_for_type(m_vm->env(), func->def->ret_type),
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
                css_class_for_type(m_vm->env(), func->def->ret_type),
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
                css_class_for_type(m_vm->env(), arg->type), arg->type->name.c_str());
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
    Chuck_Func * m_func;
    std::string m_title;
    Chuck_VM * m_vm;

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

    static const char * css_class_for_type( Chuck_Env * env, Chuck_Type * type )
    {
        if( isprim(env, type) || isvoid(env, type) || (type->array_depth && isprim(env, type->array_type)) )
            return "ckdoc_typename ckdoc_primitive_type";
        else if(isugen(type))
            return "ckdoc_typename ckdoc_ugen_type";
        else
            return "ckdoc_typename ckdoc_object_type";
    }
};




//-----------------------------------------------------------------------------
// name: CKDoc()
// desc: constructor
//-----------------------------------------------------------------------------
CKDoc::CKDoc( Chuck_VM * vm )
    : m_vm(vm)
{
    m_htmlOutput = new CKDocHTMLOutput( vm );
}




//-----------------------------------------------------------------------------
// name: ~CKDoc()
// desc: destructor
//-----------------------------------------------------------------------------
CKDoc::~CKDoc()
{
    SAFE_DELETE( m_htmlOutput );
}




//-----------------------------------------------------------------------------
// name: genCSS()
// desc: output CSS for array of types
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputCSS( Chuck_Env * env, const vector<Chuck_Type *> & types, const string & cssFilename )
{
    // return string of static CSS file
    return TRUE;
}




//-----------------------------------------------------------------------------
// compare functions
//-----------------------------------------------------------------------------
bool comp_func(Chuck_Func *a, Chuck_Func *b)
{ return a->name < b->name; }
bool comp_value(Chuck_Value *a, Chuck_Value *b)
{ return a->name < b->name; }




//-----------------------------------------------------------------------------
// name: genHTMLGroup()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputHTMLGroup( const vector<Chuck_Type *> & types,
                                      const string & title,
                                      const string & htmlFilename,
                                      const string & cssFilename )
{
    string examples_path = "https://chuck.stanford.edu/doc/examples/";
    list<string> type_args;
    bool do_toc = true;
    bool do_heading = true;

    CKDocHTMLOutput * output = m_htmlOutput;

    // output group title
    output->begin( title );
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
        for( vector<Chuck_Type *>::const_iterator t = types.begin(); t != types.end(); t++ )
        {
            // get the current type
            Chuck_Type * type = *t;

            // skip
            // if( skip(type->name) ) continue;

            // output the type in TOC
            output->toc_class( type );
        }

        // end TOC
        output->end_toc();
    }

    // begin type contents
    output->begin_classes();

    // iterate
    for( vector<Chuck_Type *>::const_iterator t = types.begin(); t != types.end(); t++ )
    {
        // get the type
        Chuck_Type * type = *t;
        // if( skip(type->name) ) continue;

        // fprintf(stderr, "## %s\n", type->name.c_str());
        // begin the type
        output->begin_class( type );

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
                    url = examples_path + ex;
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
                if(value->name[0] == '@')
                    continue;
                // value is a function
                if( value->type->name == "[function]" )
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
                if(func->def->static_decl == ae_key_static) sfuncs.push_back(func);
                else mfuncs.push_back(func);
            }

            // sort
            sort(svars.begin(), svars.end(), comp_value);
            sort(mvars.begin(), mvars.end(), comp_value);
            sort(sfuncs.begin(), sfuncs.end(), comp_func);
            sort(mfuncs.begin(), mfuncs.end(), comp_func);

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
                    a_Arg_List args = func->def->arg_list;
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
                    a_Arg_List args = func->def->arg_list;
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
        }
        // end the type
        output->end_class();
    }

    // end types
    output->end_classes();
    // end file
    output->end_body();
    // end output
    output->end();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genHTMLIndex()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputHTMLIndex( const vector<string> & clasess,
                                      const string & htmlFilename,
                                      const string & cssFilename )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genMarkdownGroup()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputMarkdownGroup( const vector<string> & clasess,
                                          const string & mdFilename )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: genMarkdownIndex()
//-----------------------------------------------------------------------------
t_CKBOOL CKDoc::outputMarkdownIndex( const vector<string> & clasess,
                                          const string & mdFilename )
{
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
    SAFE_DELETE( ckdoc );
    OBJ_MEMBER_UINT( SELF, CKDoc_offset_data ) = 0;
}

CK_DLL_MFUN( CKDoc_outputDir_set )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
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
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( ckdoc->m_outputDir );
    RETURN->v_string = str;
}

CK_DLL_MFUN( CKDoc_genCSS )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_Array4 * typeArray = (Chuck_Array4 *)GET_CK_OBJECT(ARGS);
    Chuck_String * cssFilename = GET_CK_STRING(ARGS);
    // check if null
    // check if null
    if( typeArray == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genCSS() given null 'classes' argument; nothing generated...\n" );
        goto error;
    }
    if( cssFilename == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genCSS() given null argument; nothing generated...\n" );
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
        RETURN->v_int = ckdoc->outputCSS( VM->carrier()->env, classes, cssFilename->str() );
    }

    // done
    return;

error:
    RETURN->v_int = FALSE;
}

CK_DLL_MFUN( CKDoc_genHTMLGroup )
{
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
    Chuck_Array4 * typeArray = (Chuck_Array4 *)GET_CK_OBJECT(ARGS);
    Chuck_String * groupTitle = GET_CK_STRING(ARGS);
    Chuck_String * htmlFilename = GET_CK_STRING(ARGS);
    Chuck_String * cssFilename = GET_CK_STRING(ARGS);
    // check if null
    if( typeArray == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLGroup() given null 'classes' argument; nothing generated...\n" );
        goto error;
    }
    else if( groupTitle == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLGroup() given null 'groupTitle' argument; nothing generated...\n" );
        goto error;
    }
    else if( htmlFilename == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck] CKDoc.genHTMLGroup() given null 'htmlFilename' argument; nothing generated...\n" );
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
        RETURN->v_int = ckdoc->outputHTMLGroup( classes, groupTitle->str(), htmlFilename->str(), cssFilename ? cssFilename->str() : "" );
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
    CKDoc * ckdoc = (CKDoc *)OBJ_MEMBER_UINT(SELF, CKDoc_offset_data);
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
