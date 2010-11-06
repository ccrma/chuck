/*----------------------------------------------------------------------------
    ChucK Concurrent, On-the-fly Audio Programming Language
      Compiler and Virtual Machine

    Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

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
// file: chuck_temp.c
// desc: functions to create and manipulate temporary variables which are
//       used in the IR tree representation before it has been determined
//       which variables are to go into registers.
//
// author: Andrew Appel (appel@cs.princeton.edu)
// modified: Ge Wang (gewang@cs.princeton.edu)
//           Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chuck_symbol.h"
#include "chuck_table.h"
#include "chuck_temp.h"




//-----------------------------------------------------------------------------
// name: struct Temp_Temp_
// desc: ...
//-----------------------------------------------------------------------------
struct Temp_Temp_
{
    int num;
};




//-----------------------------------------------------------------------------
// name: Temp_Label_string()
// desc: ...
//-----------------------------------------------------------------------------
c_str Temp_Label_string( Temp_Label s )
{
    return S_name( s );
}





static int labels = 0;

//-----------------------------------------------------------------------------
// name: Temp_new_label()
// desc: ...
//-----------------------------------------------------------------------------
Temp_Label Temp_new_label( void )
{
    char buf[100];
    sprintf( buf,"L%d",labels++ );
    return Temp_named_label( ( c_str )buf );
}




//-----------------------------------------------------------------------------
// name: Temp_named_label()
// desc: the label will be created only if it is not found.
//-----------------------------------------------------------------------------
Temp_Label Temp_named_label( c_str s )
{
    return insert_symbol( s );
}





static int temps = 100;

//-----------------------------------------------------------------------------
// name: Temp_new_temp( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Temp Temp_new_temp( void )
{
    Temp_Temp p = ( Temp_Temp ) checked_malloc( sizeof ( *p ) );
    p->num=temps++;
    {
        char r[16];
        sprintf( r, "%d", p->num );
        Temp_enter( Temp_name( ), p, ( c_str )r );
    }

    return p;
}




//-----------------------------------------------------------------------------
// name: struct Temp_Map_
// desc: ...
//-----------------------------------------------------------------------------
struct Temp_Map_
{
    TAB_table tab; 
    Temp_Map under;
};




//-----------------------------------------------------------------------------
// name: Temp_name( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Map Temp_name( void )
{
    static Temp_Map m = NULL;
    if ( !m ) m = Temp_empty( );
    return m;
}




//-----------------------------------------------------------------------------
// name: new_map( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Map new_map( TAB_table tab, Temp_Map under )
{
    Temp_Map m = (Temp_Map)checked_malloc( sizeof( *m ) );
    m->tab=tab;
    m->under=under;

    return m;
}




//-----------------------------------------------------------------------------
// name: Temp_empty( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Map Temp_empty( void )
{
    return new_map( TAB_empty( ), NULL );
}




//-----------------------------------------------------------------------------
// name: Temp_layer_map( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Map Temp_layer_map( Temp_Map over, Temp_Map under )
{
    if ( over==NULL )
        return under;
    else
        return new_map( over->tab, Temp_layer_map( over->under, under ) );
}




//-----------------------------------------------------------------------------
// name: Temp_enter( )
// desc: ...
//-----------------------------------------------------------------------------
void Temp_enter( Temp_Map m, Temp_Temp t, c_str s )
{
    assert( m && m->tab );
    TAB_enter( m->tab,t,s );
}




//-----------------------------------------------------------------------------
// name: Temp_look( )
// desc: ...
//-----------------------------------------------------------------------------
c_str Temp_look( Temp_Map m, Temp_Temp t )
{
    c_str s;
    assert( m && m->tab );
    s = (c_str)TAB_look( m->tab, t );
    if ( s ) return s;
    else if ( m->under ) return Temp_look( m->under, t );
    else return NULL;
}




//-----------------------------------------------------------------------------
// name: Temp_new_temp_list( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Temp_List Temp_new_temp_list( Temp_Temp h, Temp_Temp_List t ) 
{
    Temp_Temp_List p = ( Temp_Temp_List ) checked_malloc( sizeof ( *p ) );
    p->head=h; p->tail=t;
    return p;
}




//-----------------------------------------------------------------------------
// name: Temp_new_label_list( )
// desc: ...
//-----------------------------------------------------------------------------
Temp_Label_List Temp_new_label_list( Temp_Label h, Temp_Label_List t )
{
    Temp_Label_List p = ( Temp_Label_List ) checked_malloc( sizeof ( *p ) );
    p->head=h; p->tail=t;
    return p;
}




//-----------------------------------------------------------------------------
// name: showit( )
// desc: ...
//-----------------------------------------------------------------------------
static FILE *outfile;
void showit( Temp_Temp t, c_str r )
{
    fprintf( outfile, "t%d -> %s\n", t->num, r );
}




//-----------------------------------------------------------------------------
// name: Temp_dump_map( )
// desc: ...
//-----------------------------------------------------------------------------
void Temp_dump_map( FILE *out, Temp_Map m )
{
    outfile=out;
    TAB_dump( m->tab,( void ( * )( void *, void* ) )showit );
    if ( m->under )
    {
        fprintf( out,"---------\n" );
        Temp_dump_map( out,m->under );
    }
}
