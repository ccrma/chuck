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
// name: chuck_ugen.h
// desc: chuck unit generator interface
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __CHUCK_UGEN_H__
#define __CHUCK_UGEN_H__

#include "chuck_def.h"
#include "chuck_oo.h"
#include <string>
#include <vector>
#include <map>


// forward reference
struct Chuck_UGen;
struct Chuck_UGen_Info;
struct Chuck_Info_Param;
class  Chuck_VM_Shred;


// dynamic linking unit generator interface prototypes
extern "C" {
typedef void *   (* f_ctor)( t_CKTIME now );
typedef void     (* f_dtor)( t_CKTIME now, void * data );
typedef t_CKBOOL (* f_tick)( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
typedef void     (* f_ctrl)( t_CKTIME now, void * data, void * value );
typedef f_ctrl   f_cget;
typedef t_CKBOOL (* f_pmsg)( t_CKTIME now, void * data, const char * msg, void * value );
}




//-----------------------------------------------------------------------------
// name: struct Chuck_Info_Param
// dsec: ugen info param
//-----------------------------------------------------------------------------
struct Chuck_Info_Param
{
    std::string type;  // see above
    std::string name;  // name of param
    f_ctrl ctrl_addr;  // set addr
    f_cget cget_addr;  // get addr

    // constructor
    Chuck_Info_Param( const std::string & t = "", const std::string & n = "",
                      f_ctrl a = NULL, f_cget g = NULL )
    { type = t; name = n; ctrl_addr = a; cget_addr = g; }
    
    // copy constructor
    Chuck_Info_Param( const Chuck_Info_Param & p )
        : type(p.type), name(p.name), ctrl_addr(p.ctrl_addr), cget_addr(p.cget_addr) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_UGen_Info
// dsec: ugen info
//-----------------------------------------------------------------------------
struct Chuck_UGen_Info
{
    // info
    std::string name;
    std::vector<Chuck_Info_Param> param_list;
    std::map<std::string, Chuck_Info_Param> param_map;
    f_ctor ctor;
    f_dtor dtor;
    f_tick tick;
    f_pmsg pmsg;
    
    // optional
    std::string parent;
    t_CKUINT min_src;
    t_CKUINT max_src;
    int linepos;
    
    // set
    void set( f_ctor c, f_dtor d, f_tick t, f_pmsg p )
    { ctor = c; dtor = d; tick = t; pmsg = p; }
    
    // add
    void add( f_ctrl c, f_cget g, const std::string & t, const std::string & n )
    { param_list.push_back( Chuck_Info_Param( t, n, c, g ) );
      param_map[n] = param_list[param_list.size()-1]; }

    //XXX - pld  inherit
    void inherit( Chuck_UGen_Info * parentInfo )
    {
        parent = parentInfo->name;
        //inherit functions ...
        ctor = parentInfo->ctor;
        dtor = parentInfo->dtor;
        tick = parentInfo->tick;
        pmsg = parentInfo->pmsg;

        for ( unsigned int i=0; i <  parentInfo->param_list.size(); i++)
        {
	        //inherit parent class's functions
	        //(functions added subsequently will overwrite entries in param_map)
	        add( parentInfo->param_list[i].ctrl_addr,
	             parentInfo->param_list[i].cget_addr,
	             parentInfo->param_list[i].type,
	             parentInfo->param_list[i].name );
         }
    }

    // constructor
    Chuck_UGen_Info( const std::string & n = "[invalid]", t_CKUINT min = 0, t_CKUINT max = 0xffffffff )
    {
        name = n;
        parent = "";
        min_src = min;
        max_src = max;
        set( NULL, NULL, NULL, NULL );
        linepos = 0;
    }
};


// op mode
#define UGEN_OP_PASS    -1
#define UGEN_OP_STOP    0
#define UGEN_OP_TICK    1


//-----------------------------------------------------------------------------
// name: class Chuck_UGen
// dsec: ugen base
//-----------------------------------------------------------------------------
struct Chuck_UGen : public Chuck_Object
{
public:
    Chuck_UGen( );
    virtual ~Chuck_UGen( );
    virtual void init();
    virtual void done();

public: // src
    t_CKBOOL add( Chuck_UGen * src );
    t_CKBOOL remove( Chuck_UGen * src );
    void     remove_all( );
    t_CKBOOL set_max_src( t_CKUINT num );
    t_CKUINT get_num_src( );
    t_CKUINT disconnect( t_CKBOOL recursive );
    t_CKUINT system_tick( t_CKTIME now );

protected:
    void add_by( Chuck_UGen * dest );
    void remove_by( Chuck_UGen * dest );

public: // interface
    f_ctor ctor;
    f_dtor dtor;
    f_tick tick;
    f_pmsg pmsg;

public: // data
    std::vector<Chuck_UGen *> m_src_list;
    std::vector<Chuck_UGen *> m_dest_list;
    t_CKUINT m_num_src;
    t_CKUINT m_num_dest;
    t_CKUINT m_max_src;
    t_CKTIME m_time;
    t_CKBOOL m_valid;
    SAMPLE m_sum;
    SAMPLE m_current;
    SAMPLE m_last;
    SAMPLE m_gain;
    t_CKINT m_op;
    Chuck_VM_Shred * shred;

public: // dynamic linking client data
    void * state;
};




#endif
