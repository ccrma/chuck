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
// file: chuck_table.h
// desc: chuck table
//
// No algorithm should use these functions directly, because
// programming with void* is too error-prone.  Instead,
// each module should make "wrapper" functions that take
// well-typed arguments and call the TAB_ functions.
//
// copyright (c) 1997 Andrew W. Appel.
//
// original author: Andrew Appel (appel@cs.princeton.edu)
// modified: Ge Wang (gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_TABLE_H__
#define __CHUCK_TABLE_H__


typedef struct TAB_table_ * TAB_table;
typedef long (* TAB_eq_func)( void * lhs, void * rhs );
typedef long (* TAB_hash_func)( void * key );

/* Make a new table mapping "keys" to "values". */
TAB_table TAB_empty(void);
TAB_table TAB_empty2(unsigned long size);
TAB_table TAB_empty3( TAB_eq_func eq, TAB_hash_func hash, unsigned int size );
void TAB_delete( TAB_table t );

/* Enter the mapping "key"->"value" into table "t", 
 *    shadowing but not destroying any previous binding for "key". */
void TAB_enter(TAB_table t, void *key, void *value);

/* Look up the most recent binding for "key" in table "t" */
void *TAB_look(TAB_table t, void *key);

/* Pop the most recent binding and return its key.
 * This may expose another binding for the same key, if there was one. */
void *TAB_pop(TAB_table t);
void *TAB_topv(TAB_table t);


/* Call "show" on every "key"->"value" pair in the table,
 *  including shadowed bindings, in order from the most 
 *  recent binding of any key to the oldest binding in the table */
void TAB_dump(TAB_table t, void (*show)(void *key, void *value));

/* str eq function */
long str_eq( void * lhs, void * rhs );
long str_hash( void * key );

#endif
