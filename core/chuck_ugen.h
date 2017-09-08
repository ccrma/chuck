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
// file: chuck_ugen.h
// desc: chuck unit generator interface
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//          Rebecca Fiebrink (fiebrink@cs.princeton.edu)
//          Spencer Salazar (spencer@ccrma.stanford.edu)
// date: spring 2004 - 1.1
//       spring 2005 - 1.2
//       spring 2007 - UAna
//-----------------------------------------------------------------------------
#ifndef __CHUCK_UGEN_H__
#define __CHUCK_UGEN_H__

#include "chuck_def.h"
#include "chuck_oo.h"
#include "chuck_dl.h"


// forward reference
struct Chuck_VM_Shred;
struct Chuck_UAnaBlobProxy;


// op mode
#define UGEN_OP_PASS    -1
#define UGEN_OP_STOP    0
#define UGEN_OP_TICK    1




//-----------------------------------------------------------------------------
// name: struct Chuck_UGen
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
    t_CKBOOL add( Chuck_UGen * src, t_CKBOOL isUpChuck );
    t_CKBOOL remove( Chuck_UGen * src );
    t_CKVOID remove_all( );
    t_CKBOOL set_max_src( t_CKUINT num );
    t_CKUINT get_num_src( );
    t_CKBOOL is_connected_from( Chuck_UGen * src );
    t_CKUINT disconnect( t_CKBOOL recursive );
    t_CKUINT system_tick( t_CKTIME now );
    t_CKUINT system_tick_v( t_CKTIME now, t_CKUINT numFrames );
    t_CKBOOL alloc_v( t_CKUINT size );
    
    Chuck_UGen *src_chan( t_CKUINT chan );
    Chuck_UGen *dst_for_src_chan( t_CKUINT chan );

protected:
    t_CKVOID add_by( Chuck_UGen * dest, t_CKBOOL isUpChuck );
    t_CKVOID remove_by( Chuck_UGen * dest );

public:
    // tick function
    f_tick tick;
    // multichannel/vectorized tick function (added 1.3.0.0)
    f_tickf tickf;
    // msg function
    f_pmsg pmsg;
    // channels (if more than one is required)
    Chuck_UGen ** m_multi_chan;
    // size of m_multi_chan;
    t_CKUINT m_multi_chan_size;
    // number of channels
    t_CKUINT m_num_ins;
    // number of channels
    t_CKUINT m_num_outs;
    // alloc multi channels
    void alloc_multi_chan( t_CKUINT num_ins, t_CKUINT num_outs );
    // sets up ugen as a subgraph type ugen (added 1.3.0.0)
    void init_subgraph();

public: // data
    Chuck_UGen ** m_src_list;
    t_CKUINT m_src_cap;
    t_CKUINT m_num_src;
    Chuck_UGen ** m_dest_list;
    t_CKUINT m_dest_cap;
    t_CKUINT m_num_dest;
    Chuck_UGen ** m_src_uana_list;
    t_CKUINT m_src_uana_cap;
    t_CKUINT m_num_uana_src;
    Chuck_UGen ** m_dest_uana_list;
    t_CKUINT m_dest_uana_cap;
    t_CKUINT m_num_uana_dest;
    t_CKUINT m_max_src;
    t_CKTIME m_time;
    t_CKBOOL m_valid;
    t_CKBOOL m_use_next;
    SAMPLE m_sum;
    SAMPLE m_current;
    SAMPLE m_next;
    SAMPLE m_last;
    SAMPLE m_gain;
    SAMPLE m_pan;
    t_CKINT m_op;
    t_CKINT m_max_block_size;
    
    // SPENCERTODO: combine with block processing (added 1.3.0.0)
    SAMPLE * m_multi_in_v;
    SAMPLE * m_multi_out_v;
    
    // SPENCERTODO: better way to handle this (added 1.3.0.0)
    t_CKBOOL m_is_subgraph;
    Chuck_UGen * m_inlet, * m_outlet;
    Chuck_UGen * inlet();
    Chuck_UGen * outlet();
    
    // block processing
    SAMPLE * m_sum_v;
    SAMPLE * m_current_v;

    // the shred on which the ugen is created
    Chuck_VM_Shred * shred;
    // owner
    Chuck_UGen * owner;
    
    // what a hack!
    t_CKBOOL m_is_uana;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_UAna
// dsec: uana base
//-----------------------------------------------------------------------------
struct Chuck_UAna : public Chuck_UGen
{
public:
    Chuck_UAna( );
    virtual ~Chuck_UAna( );

public:
    t_CKBOOL system_tock( t_CKTIME now );
    t_CKBOOL is_up_connected_from( Chuck_UAna * src );

public: // blob retrieval
    t_CKINT numIncomingUAnae() const;
    Chuck_UAna * getIncomingUAna( t_CKUINT index ) const;
    Chuck_UAnaBlobProxy * getIncomingBlob( t_CKUINT index ) const;
    Chuck_UAnaBlobProxy * blobProxy() const;

public:
    // tock function
    f_tock tock;

public: // data
    t_CKTIME m_uana_time;
    // Chuck_UAnaBlobProxy * m_blob_proxy;
};



t_CKINT ugen_generic_num_in( Chuck_Object * obj, t_CKBOOL isArray );
Chuck_UGen *ugen_generic_get_src( Chuck_Object * obj, t_CKINT chan, t_CKBOOL isArray );
Chuck_UGen *ugen_generic_get_dst( Chuck_Object * obj, t_CKINT chan, t_CKBOOL isArray );



#endif
