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
// file: chuck_symbol.cpp
// desc: symbols in the syntax, adapted from Tiger compiler by Appel Appel
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// adapted from: Andrew Appel (appel@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "chuck_utils.h"
#include "chuck_symbol.h"
#include "chuck_table.h"


// S_Symbol
struct S_Symbol_ { c_str name; S_Symbol next; };

static S_Symbol mksymbol( c_constr name, S_Symbol next )
{
    S_Symbol s = (S_Symbol)checked_malloc(sizeof(*s));
    s->name = (c_str)checked_malloc(strlen(name)+1);
    strcpy(s->name, (c_str)name); s->next=next;
    return s;
}

#define SIZE 65347  /* should be prime */

static S_Symbol hashtable[SIZE];

static unsigned int hash(const char *s0)
{
    unsigned int h=0; const char *s;
    for(s=s0; *s; s++)  
        h = h*65599 + *s;
    return h;
}
 
static int streq(c_constr a, c_constr b)
{
    return !strcmp((char*)a,(char*)b);
}

S_Symbol insert_symbol(c_constr name)
{
    S_Symbol syms = NULL, sym;
    int index= hash(name) % SIZE;

    if( !name ) return NULL;
    syms = hashtable[index];
    for(sym=syms; sym; sym=sym->next)
        if (streq(sym->name,name)) return sym;
    sym = mksymbol(name,syms);
    hashtable[index]=sym;
    
    return sym;
}

c_str S_name( S_Symbol sym )
{
    return sym->name;
}

S_table S_empty( void ) 
{ 
    return TAB_empty();
}

S_table S_empty2( unsigned int size )
{
    return TAB_empty2(size);
}

void S_enter( S_table t, S_Symbol sym, void *value )
{
    TAB_enter(t,sym,value);
}

void S_enter2( S_table t, c_constr str, void * value )
{
    TAB_enter(t,insert_symbol(str),value);
}

void * S_look( S_table t, S_Symbol sym )
{
    return TAB_look(t,sym);
}

void * S_look2( S_table t, c_constr str )
{
    return TAB_look(t,insert_symbol(str));
}

// BUG: this could result in a bad free if the S_Symbol ever gets cleaned up
static struct S_Symbol_ marksym = { (char *)"<mark>", 0 };

void S_beginScope( S_table t )
{
    S_enter( t, &marksym, NULL );
}

void S_endScope( S_table t )
{
    S_Symbol s;
    do s = (S_Symbol)TAB_pop(t);
    while (s != &marksym);
}

void S_pop( S_table t )
{
    TAB_pop(t);
}

void S_dump( S_table t, void (*show)(S_Symbol sym, void *binding) )
{
    TAB_dump( t, (void (*)(void *, void *)) show );
}

/* the str->whatever table */
