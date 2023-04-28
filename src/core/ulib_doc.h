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
// file: ulib_doc.h
// desc: class library and API for built-in CKDoc documentation system
//       based on the OG 'ckdoc' utility by Spencer Salazar
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
//         Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2023
//-----------------------------------------------------------------------------
#ifndef __ULIB_DOC_H__
#define __ULIB_DOC_H__

#include "chuck_dl.h"
#include "chuck_absyn.h"

// ckdoc query
DLL_QUERY ckdoc_query( Chuck_DL_Query * QUERY );


// forward reference
struct Chuck_VM;
struct Chuck_Type;
struct CKDocGroup;
class CKDocHTMLOutput;




//-----------------------------------------------------------------------------
// name: class CKDocOutput | 1.5.0.0 (based on Spencer's ckdoc Output class)
// desc: abstract base class for doc output
//-----------------------------------------------------------------------------
class CKDocOutput
{
public:
    // base constructor
    CKDocOutput() { }
    // virtual destructor
    virtual ~CKDocOutput() { }

public:
    // clear output string
    void clear() { m_outputStr = ""; }
    // get output string
    const std::string & str() const { return m_outputStr; }
    // get file name: ".html", ".txt", etc.
    virtual std::string fileExtension() const { return ""; }
    // render index
    virtual std::string renderIndex( const std::string & indexTitle, const std::vector<CKDocGroup *> & groups ) { return ""; };

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
    virtual void begin_classes( CKDocGroup * group ) = 0;
    virtual void end_classes() = 0;

    // one class
    virtual void begin_class( Chuck_Type * type, const std::vector<CKDocGroup *> & groups ) = 0;

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
    virtual void func_arg( a_Arg_List arg ) = 0;

    // end class
    virtual void end_class() = 0;

protected:
    // output string
    std::string m_outputStr;
};




//-----------------------------------------------------------------------------
// name: class CKDoc | 1.5.0.0 (ge, spencer)
// desc: CKDoc internal implementation, based on Spencer's CKDoc utility
//-----------------------------------------------------------------------------
class CKDoc
{
public:
    // constructor
    CKDoc( Chuck_VM * vm );
    // destructor
    ~CKDoc();

public:
    // clear all added groups
    void clearGroups();
    // clear output generator
    void clearOutput();
    // add a group of types to document
    t_CKBOOL addGroup( const std::vector<Chuck_Type *> & types,
                       const std::string & name,
                       const std::string & shortName,
                       const std::string & description );
    // get number of groups
    t_CKINT numGroups() const;
    // set output format: CKDoc.HTML, CKDoc.TEXT, CKDoc.MARKDOWN, CKDoc.JSON
    t_CKBOOL setOutputFormat( t_CKINT which );
    // get output format
    t_CKINT getOutpuFormat() const;
    // set base examples root path
    void setExamplesRoot( const std::string & path );
    // get base examples root path
    std::string getExamplesRoot() const;

public:
    // generate everything as files into the output directory
    t_CKBOOL outputToDir( const std::string & outputDir, const std::string & indexTitle );
    // generate top-level index file; return as string
    std::string genIndex( const std::string & title );
    // generate CSS; return as string
    std::string genCSS();
    // generate all added groups, return each in a separate entry
    void genGroups( std::vector<std::string> & results );
    // generate documentation for a group
    std::string genGroup( CKDocGroup * group, t_CKBOOL clearOutput = TRUE );
    // generate documentation for a single Type
    std::string genType( Chuck_Type * type, t_CKBOOL clearOutput = TRUE );

public:
    // enumeration for output format types
    static const t_CKINT FORMAT_NONE;     // no output format
    static const t_CKINT FORMAT_HTML;     // default
    static const t_CKINT FORMAT_TEXT;     // same as help
    static const t_CKINT FORMAT_MARKDOWN; // not implemented
    static const t_CKINT FORMAT_JSON;     // not implemented

protected:
    // write string to file
    t_CKBOOL outputToFile( const std::string & filename, const std::string & content );

protected:
    // literally ckdoc.css
    static std::string m_ckdocCSS;

protected:
    // groups
    std::vector<CKDocGroup *> m_groups;
    // examples root path
    std::string m_examplesRoot;
    // format
    t_CKINT m_format;
    // output reference
    CKDocOutput * m_output;
    // chuck VM reference
    Chuck_VM * m_vm_ref;
};




#endif
//
//
//public:
//    // generate CSS file
//    t_CKBOOL outputCSS( const std::string & cssFilename );
//    // generate an HTML file containing documentation for a list of classes
//    t_CKBOOL outputHTMLGroup( const std::vector<Chuck_Type *> & clasess,
//                              const std::string & title,
//                              const std::string & htmlFilename,
//                              const std::string & cssFilename );
//    // generate an HTML file containing index for a list of classes
//    t_CKBOOL outputHTMLIndex( const std::vector<std::string> & clasess,
//                              const std::string & htmlFilename,
//                              const std::string & cssFilename );
//    // generate HTML for a type
//    t_CKBOOL outputHTMLType( const std::string & typeName, std::string & output );
//    // generate an Markdown file containing documentation for a list of classes
//    t_CKBOOL outputMarkdownGroup( const std::vector<std::string> & clasess,
//                               const std::string & mdFilename );
//    // does this even make sense for Markdown?
//    t_CKBOOL outputMarkdownIndex( const std::vector<std::string> & clasess,
//                                  const std::string & mdFilename );
