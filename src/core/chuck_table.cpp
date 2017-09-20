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
// file: chuck_table.cpp
// desc: chuck table implementation for parsers
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
#include <stdio.h>
#include <string.h>
#include "chuck_table.h"
#include "chuck_utils.h"

#define TABSIZE 65437


typedef struct binder_ *binder;
struct binder_ {void *key; void *value; binder next; void *prevtop;};
struct TAB_table_ {
  unsigned long size;
  binder * table;
  void * top;
  TAB_eq_func eq_func;
  TAB_hash_func hash_func;
};


static binder Binder(void *key, void *value, binder next, void *prevtop)
{
    binder b = (binder)checked_malloc(sizeof(struct binder_));
    b->key = key; b->value=value; b->next=next; b->prevtop = prevtop; 
    return b;
}


TAB_table TAB_empty(void)
{ 
    return TAB_empty2(TABSIZE);
}


TAB_table TAB_empty2( unsigned long s )
{
    TAB_table t = (TAB_table)checked_malloc(sizeof(struct TAB_table_));
    unsigned long i;
    t->table = (binder *)checked_malloc(sizeof(binder)*s);
    t->size = s;
    t->top = NULL;
    t->eq_func = NULL;
    t->hash_func = NULL;
    for( i = 0; i < s; i++ )
        t->table[i] = NULL;
    return t;
}


TAB_table TAB_empty3( TAB_eq_func eq, TAB_hash_func hash, unsigned long s )
{
    TAB_table t = TAB_empty2(s);
    t->eq_func = eq;
    t->hash_func = hash;
    return t;
}


void TAB_delete( TAB_table t )
{
    unsigned long i;
    binder p = NULL, n = NULL;
    for(i = 0; i < t->size; i++ )
    {
        p = t->table[i];
        while( p )
        {
            n = p->next;
            free(p);
            p = n;
        }
    }
    
    // free the table
    free( t->table );
    free( t );
}



/* The cast from pointer to integer in the expression
 *   ((unsigned)key) % TABSIZE
 * may lead to a warning message.  However, the code is safe,
 * and will still operate correctly.  This line is just hashing
 * a pointer value into an integer value, and no matter how the
 * conversion is done, as long as it is done consistently, a
 * reasonable and repeatable index into the table will result.
 */

void TAB_enter(TAB_table t, void *key, void *value)
{
    long index;
    unsigned long hval = (unsigned long)key;
    assert(t && key);
    if( t->hash_func )
        hval = (unsigned long)t->hash_func(key);
    index = hval % t->size;
    t->table[index] = Binder(key, value, t->table[index], t->top);
    t->top = key;
}

void *TAB_look(TAB_table t, void *key)
{
    long index;
    unsigned long hval = (unsigned long)key;
    binder b;
    assert(t && key);
    if( t->hash_func )
        hval = (unsigned long)t->hash_func(key);
    index= hval % t->size;
    if( !t->eq_func )
    {
        for(b=t->table[index]; b; b=b->next)
            if (b->key==key) return b->value;
    }
    else
    {
        for(b=t->table[index]; b; b=b->next)
            if (t->eq_func(b->key, key)) return b->value;
    }

    return NULL;
}

void *TAB_pop(TAB_table t)
{
    void *k; binder b; long index;
    unsigned long hval;
    assert (t);
    k = t->top;
    assert (k);
    hval = (unsigned long)k;
    if(t->hash_func) hval = (unsigned long)t->hash_func(k);
    index = ((unsigned long)hval) % t->size;
    b = t->table[index];
    assert(b);
    t->table[index] = b->next;
    t->top=b->prevtop;
    return b->key;
}

void *TAB_topv(TAB_table t)
{
    void *k; binder b; long index;
    unsigned long hval;
    assert(t);
    k = t->top;
    assert(k);
    hval = (unsigned long)k;
    if(t->hash_func) hval = (unsigned long)t->hash_func(k);
    index = hval % t->size;
    b = t->table[index];
    assert(b);
    return b->value;
}

void TAB_dump(TAB_table t, void (*show)(void *key, void *value))
{
    void *k = t->top;
    long index = ((unsigned long)k) % t->size;
    binder b = t->table[index];
    if (b==NULL) return;
    t->table[index]=b->next;
    t->top=b->prevtop;
    show(b->key,b->value);
    TAB_dump(t,show);
    assert(t->top == b->prevtop && t->table[index]==b->next);
    t->top=k;
    t->table[index]=b;
}

long str_eq( void * lhs, void * rhs )
{
    return !strcmp( (char *)lhs, (char *)rhs );
}

long str_hash( void * str )
{
    unsigned long h=0; char *s;
    for(s=(char *)str; *s; s++)  
        h = h*65599 + *s;
    return h;
}
