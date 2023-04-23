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

// ckdoc query
DLL_QUERY ckdoc_query( Chuck_DL_Query * QUERY );


// forward reference
struct Chuck_VM;
class CKDocHTMLOutput;




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
    // generate CSS file
    t_CKBOOL outputCSS( const std::string & cssFilename );
    // generate an HTML file containing documentation for a list of classes
    t_CKBOOL outputHTMLGroup( const std::vector<Chuck_Type *> & clasess,
                              const std::string & title,
                              const std::string & htmlFilename,
                              const std::string & cssFilename );
    // generate an HTML file containing index for a list of classes
    t_CKBOOL outputHTMLIndex( const std::vector<std::string> & clasess,
                              const std::string & htmlFilename,
                              const std::string & cssFilename );
    // generate HTML for a type
    t_CKBOOL outputHTMLType( const std::string & typeName, std::string & output );
    // generate an Markdown file containing documentation for a list of classes
    t_CKBOOL outputMarkdownGroup( const std::vector<std::string> & clasess,
                               const std::string & mdFilename );
    // does this even make sense for Markdown?
    t_CKBOOL outputMarkdownIndex( const std::vector<std::string> & clasess,
                                  const std::string & mdFilename );

public:
    // literally ckdoc.css
    static std::string m_ckdocCSS;

public:
    // where to output files
    std::string m_outputDir;
    // HTML output reference
    CKDocHTMLOutput * m_htmlOutput;
    // chuck VM reference
    Chuck_VM * m_vm;
};




#endif
