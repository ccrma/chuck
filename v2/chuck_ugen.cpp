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
// name: chuck_ugen.cpp
// desc: chuck unit generator interface
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: spring 2004 - 1.1
//       spring 2005 - 1.2
//-----------------------------------------------------------------------------
#include "chuck_ugen.h"
#include "chuck_vm.h"
#include "chuck_lang.h"
#include "chuck_errmsg.h"
using namespace std;




//-----------------------------------------------------------------------------
// fast array
//-----------------------------------------------------------------------------
void fa_init( Chuck_UGen ** & base, t_CKUINT & capacity );
void fa_done( Chuck_UGen ** & base, t_CKUINT & capacity );
void fa_resize( Chuck_UGen ** & base, t_CKUINT & capacity );
void fa_push_back( Chuck_UGen ** & base, t_CKUINT & capacity, 
                   t_CKUINT size, Chuck_UGen * value );
t_CKBOOL fa_lookup( Chuck_UGen ** base, t_CKUINT size,
                    const Chuck_UGen * value );



//-----------------------------------------------------------------------------
// name: fa_init()
// desc: ...
//-----------------------------------------------------------------------------
void fa_init( Chuck_UGen ** & base, t_CKUINT & capacity )
{
    base = NULL;
    capacity = 0;
}




//-----------------------------------------------------------------------------
// name: fa_done()
// desc: ...
//-----------------------------------------------------------------------------
void fa_done( Chuck_UGen ** & base, t_CKUINT & capacity )
{
    if( base ) delete [] base;
    base = NULL;
    capacity = 0;
}




//-----------------------------------------------------------------------------
// name: fa_resize()
// desc: ...
//-----------------------------------------------------------------------------
void fa_resize( Chuck_UGen ** & base, t_CKUINT & capacity )
{
    // initial
    if( capacity == 0 )
        capacity = 8;
    else
        capacity *= 2;

    // allocate
    Chuck_UGen ** new_base = new Chuck_UGen *[capacity];
    // delete
    if( base )
    {
        // copy
        memcpy( new_base, base, capacity / 2 * sizeof(Chuck_UGen *) );
        // delete
        delete [] base;
    }

    // done
    base = new_base;
}




//-----------------------------------------------------------------------------
// name: fa_push_back()
// desc: ...
//-----------------------------------------------------------------------------
void fa_push_back( Chuck_UGen ** & base, t_CKUINT & capacity, 
                   t_CKUINT size, Chuck_UGen * value )
{
    // resize
    if( size == capacity ) fa_resize( base, capacity );
    // add
    base[size] = value;
}




//-----------------------------------------------------------------------------
// name: fa_lookup()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL fa_lookup( Chuck_UGen ** base, t_CKUINT size, const Chuck_UGen * value )
{
    // loop
    for( t_CKUINT i = 0; i < size; i++ )
        if( base[i] == value )
            return TRUE;

    // not found
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: Chuck_UGen()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_UGen::Chuck_UGen()
{
    this->init();
}




//-----------------------------------------------------------------------------
// name: ~Chuck_UGen()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_UGen::~Chuck_UGen()
{
    this->done();
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_UGen::init()
{
    tick = NULL;
    pmsg = NULL;
    m_multi_chan = NULL;
    m_multi_chan_size = 0;
    m_num_ins = 1;
    m_num_outs = 1;

    fa_init( m_src_list, m_src_cap );
    fa_init( m_dest_list, m_dest_cap );
    fa_init( m_src_uana_list, m_src_uana_cap );
    fa_init( m_dest_uana_list, m_dest_uana_cap );
    m_num_src = 0;
    m_num_dest = 0;
    m_num_uana_src = 0;
    m_num_uana_dest = 0;
    m_max_src = 0xffffffff;
    m_time = 0;
    m_valid = TRUE;
    m_sum = 0.0f;
    m_current = 0.0f;
    m_last = 0.0f;
    m_op = UGEN_OP_TICK;
    m_gain = 1.0f;
    m_pan = 1.0f;
    m_next = 0.0f;
    m_use_next = FALSE;
    
    m_sum_v = NULL;
    m_current_v = NULL;

    shred = NULL;
    owner = NULL;
    
    // what a hack
    m_is_uana = FALSE;
}




//-----------------------------------------------------------------------------
// name: done()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_UGen::done()
{
    if( this->shred )
        shred->remove( this );

    assert( this->m_ref_count == 0 );

    // disconnect
    this->disconnect( TRUE );
    m_valid = FALSE;

    fa_done( m_src_list, m_src_cap );
    fa_done( m_dest_list, m_dest_cap );
    fa_done( m_src_uana_list, m_src_uana_cap );
    fa_done( m_dest_uana_list, m_dest_uana_cap );
    
    // reclaim
    SAFE_DELETE_ARRAY( m_sum_v );
    SAFE_DELETE_ARRAY( m_current_v );

    // TODO: m_multi_chan, break ref count loop
}




//-----------------------------------------------------------------------------
// name: alloc_v()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::alloc_v( t_CKUINT size )
{
    // reclaim
    SAFE_DELETE_ARRAY( m_sum_v );
    SAFE_DELETE_ARRAY( m_current_v );

    // go
    if( size > 0 )
    {
        m_sum_v = new SAMPLE[size];
        m_current_v = new SAMPLE[size];

        return ( m_sum_v != NULL && m_current_v != NULL );
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: alloc_multi_chan()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_UGen::alloc_multi_chan( t_CKUINT num_ins, t_CKUINT num_outs )
{
    // get max of num_ins and num_outs
    m_multi_chan_size = ( num_ins > num_outs ? num_ins : num_outs );

    // allocate
    m_multi_chan = new Chuck_UGen *[m_multi_chan_size];
    // zero it out, whoever call this will fill in
    memset( m_multi_chan, 0, m_multi_chan_size * sizeof(Chuck_UGen *) );

    // mono
    if( m_multi_chan_size == 1 )
    {
        // zero out
        m_multi_chan_size = 0;
        // self
        m_multi_chan[0] = this;
    }
    
    // remember
    m_num_ins = num_ins;
    m_num_outs = num_outs;
}




//-----------------------------------------------------------------------------
// name: set_max_src()
// dsec: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::set_max_src( t_CKUINT num )
{
    m_max_src = num;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_num_src()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_UGen::get_num_src()
{
    return m_num_src;
}




//-----------------------------------------------------------------------------
// name: add()
// dsec: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::add( Chuck_UGen * src, t_CKBOOL isUpChuck )
{
    // examine ins and outs
    t_CKUINT outs = src->m_num_outs;
    t_CKUINT ins = this->m_num_ins;
    t_CKUINT i;

    if( outs >= 1 && ins == 1 )
    {
        // check if already connected
        // if( fa_lookup( m_src_list, m_num_src, src ) )
        //     return FALSE;
        // check for limit
        if( m_num_src >= m_max_src )
            return FALSE;

        // append
        fa_push_back( m_src_list, m_src_cap, m_num_src, src );
        m_num_src++;
        src->add_ref();
        src->add_by( this, isUpChuck );
        
        // upchuck
        if( isUpChuck )
        {
            // add to uana list
            fa_push_back( m_src_uana_list, m_src_uana_cap, m_num_uana_src, src );
            m_num_uana_src++;
            // TODO: verify that we don't need to reference count
        }
    }
    /* else if( outs >= 2 && ins == 1 )
    {
        // check if already connect
        if( fa_lookup( m_src_list, m_num_src, src ) )
            return FALSE;
        // check for limit
        if( m_num_src >= m_max_src )
            return FALSE;

        // append
        fa_push_back( m_src_list, m_src_cap, m_num_src, src );
        m_num_src++;
        src->add_ref();
        src->add_by( this );
    } */
    else if( outs == 1 && ins >= 2 )
    {
        // add to each channel
        for( i = 0; i < ins; i++ )
            if( !this->m_multi_chan[i]->add( src, isUpChuck ) ) return FALSE;
    }
    else if( outs >= 2 && ins >= 2 )
    {
        // add to each channel
        for( i = 0; i < ins; i++ )
            if( !this->m_multi_chan[i]->add( src->m_multi_chan[i%outs], isUpChuck ) ) return FALSE;
    }
    else
    {
        EM_error3( "internal error: unhandled UGen add: outs: %d ins: %d", outs, ins );
        assert( FALSE );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: add_by()
// dsec: ...
//-----------------------------------------------------------------------------
void Chuck_UGen::add_by( Chuck_UGen * dest, t_CKBOOL isUpChuck )
{
    // append
    fa_push_back( m_dest_list, m_dest_cap, m_num_dest, dest );
    dest->add_ref();
    m_num_dest++;
    
    // uana
    if( isUpChuck )
    {
        // add to uana list
        fa_push_back( m_dest_uana_list, m_dest_uana_cap, m_num_uana_dest, dest );
        m_num_uana_dest++;
        // TODO: verify we don't need to ref count
    }
}




//-----------------------------------------------------------------------------
// name: remove()
// dsec: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::remove( Chuck_UGen * src )
{
    // ins and outs
    t_CKUINT outs = src->m_num_outs;
    t_CKUINT ins = this->m_num_ins;
    t_CKUINT i;
    t_CKBOOL ret = FALSE;

    // take action
    if( outs >= 1 && ins == 1 )
    {
        if( m_num_src == 0 ) return FALSE;

        // remove from uana list (first, due to ref count)
        for( t_CKUINT j = 0; j < m_num_uana_src; j++ )
            if( m_src_uana_list[j] == src )
            {
                // since src list is a super set of this list,
                // removing here -> removing at least one from src list
                for( t_CKUINT k = j+1; k < m_num_uana_src; k++ )
                    m_src_uana_list[k-1] = m_src_uana_list[k];
                
                m_src_uana_list[--m_num_uana_src] = NULL;
                --j;
            }

        // remove
        for( t_CKUINT i = 0; i < m_num_src; i++ )
            if( m_src_list[i] == src )
            {
                ret = TRUE;
                for( t_CKUINT j = i+1; j < m_num_src; j++ )
                    m_src_list[j-1] = m_src_list[j];

                m_src_list[--m_num_src] = NULL;
                src->remove_by( this );
                src->release();
                --i;
            }
        
    }
    /* else if( outs >= 2 && ins == 1 )
    {
        if( m_num_src == 0 ) return FALSE;

        // remove
        for( t_CKUINT i = 0; i < m_num_src; i++ )
            if( m_src_list[i] == src )
            {
                ret = TRUE;
                for( t_CKUINT j = i+1; j < m_num_src; j++ )
                    m_src_list[j-1] = m_src_list[j];

                m_src_list[--m_num_src] = NULL;
                src->remove_by( this );
                src->release();
            }
    } */
    else if( outs == 1 && ins >= 2 )
    {
        for( i = 0; i < ins; i++ )
            if( !m_multi_chan[i]->remove( src ) ) return FALSE;
        ret = TRUE;
    }
    else if( outs >= 2 && ins >= 2 )
    {
        for( i = 0; i < ins; i++ )
            if( !m_multi_chan[i]->remove( src->m_multi_chan[i%outs] ) ) return FALSE;
        ret = TRUE;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: remove_by()
// dsec: ...
//-----------------------------------------------------------------------------
void Chuck_UGen::remove_by( Chuck_UGen * dest )
{
    // remove from uana list (first due to reference count)
    for( t_CKUINT j = 0; j < m_num_uana_dest; j++ )
        if( m_dest_uana_list[j] == dest )
        {
            // get rid of it
            for( t_CKUINT k = j+1; k < m_num_uana_dest; k++ )
                m_dest_uana_list[k-1] = m_dest_uana_list[k];
            
            // null last element
            m_dest_uana_list[--m_num_uana_dest] = NULL;
            j--;
        }

    // remove
    for( t_CKUINT i = 0; i < m_num_dest; i++ )
        if( m_dest_list[i] == dest )
        {
            // get rid of it
            for( t_CKUINT j = i+1; j < m_num_dest; j++ )
                m_dest_list[j-1] = m_dest_list[j];

            // release
            dest->release();
            // null the last element
            m_dest_list[--m_num_dest] = NULL;
            i--;
        }
}




//-----------------------------------------------------------------------------
// name: add()
// dsec: ...
//-----------------------------------------------------------------------------
void Chuck_UGen::remove_all( )
{
    assert( this->m_num_dest == 0 );
    
    // remove
    while( m_num_src > 0 )
    {
        // make sure at least one got disconnected
        if( !this->remove( m_src_list[0] ) )
        {
            // TODO: figure out why this is necessary!

            // get rid of it, but don't release
            for( t_CKUINT j = 1; j < m_num_src; j++ )
                m_src_list[j-1] = m_src_list[j];

            // null the last element
            m_src_list[--m_num_src] = NULL;
        }
    }
}




//-----------------------------------------------------------------------------
// name: disconnect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::disconnect( t_CKBOOL recursive )
{
    // remove
    while( m_num_dest > 0 )
    {
        // make sure at least one got disconnected
        if( !m_dest_list[0]->remove( this ) )
        {
            // get rid of it, but don't release
            for( t_CKUINT j = 1; j < m_num_dest; j++ )
                m_dest_list[j-1] = m_dest_list[j];

            // null the last element
            m_dest_list[--m_num_dest] = NULL;
        }
    }

    // for( unsigned int i = 0; i < m_num_dest; i++ )
    //    m_dest_list[i]->remove( this );
    // m_num_dest = 0;

    // disconnect src too?
    if( recursive )
        this->remove_all();
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: is_connected_from()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::is_connected_from( Chuck_UGen * src )
{
    if( m_src_list != NULL && fa_lookup( m_src_list, m_num_src, src ) )
        return TRUE;

    // multichannel
    if( m_multi_chan != NULL )
    {
        for( t_CKUINT i = 0; i < m_multi_chan_size; i++ )
        {
            if( fa_lookup( m_multi_chan[i]->m_src_list,
                           m_multi_chan[i]->m_num_src, src ) )
                return TRUE;
        }
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: tick()
// dsec: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::system_tick( t_CKTIME now )
{
    if( m_time >= now )
        return m_valid;

    t_CKUINT i; Chuck_UGen * ugen; SAMPLE multi;

    // inc time
    m_time = now;
    // initial sum
    m_sum = 0.0f;
    if( m_num_src )
    {
        ugen = m_src_list[0];
        if( ugen->m_time < now ) ugen->system_tick( now );
        m_sum = ugen->m_current;

        // tick the src list
        for( i = 1; i < m_num_src; i++ )
        {
            ugen = m_src_list[i];
            if( ugen->m_time < now ) ugen->system_tick( now );
            if( ugen->m_valid )
            {
                if( m_op <= 1 )
                    m_sum += ugen->m_current;
                else // special ops
                {
                    switch( m_op )
                    {
                    case 2: m_sum -= ugen->m_current; break;
                    case 3: m_sum *= ugen->m_current; break;
                    case 4: m_sum /= ugen->m_current; break;
                    default: m_sum += ugen->m_current; break;
                    }
                }
            }
        }
    }

    // tick multiple channels
    multi = 0.0f;
    if( m_multi_chan_size )
    {
        for( i = 0; i < m_multi_chan_size; i++ )
        {
            ugen = m_multi_chan[i];
            if( ugen->m_time < now ) ugen->system_tick( now );
            // multiple channels are added
            multi += ugen->m_current;
        }
    
        // scale multi
        multi /= m_multi_chan_size;
        m_sum += multi;
    }

    // if owner
    if( owner != NULL && owner->m_time < now )
        owner->system_tick( now );

    if( m_op > 0 )  // UGEN_OP_TICK
    {
        // tick the ugen
        if( tick ) m_valid = tick( this, m_sum, &m_current, NULL );
        if( !m_valid ) m_current = 0.0f;
		// apply gain and pan
        m_current *= m_gain * m_pan;
		// dedenormal
		CK_DDN( m_current );
		// save as last
        m_last = m_current;
        return m_valid;
    }
    else if( m_op < 0 ) // UGEN_OP_PASS
    {
        // pass through
        m_current = m_sum;
        m_last = m_current;
        return TRUE;
    }
    else // UGEN_OP_STOP
        m_current = 0.0f;
    
    m_last = m_current;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_v()
// dsec: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UGen::system_tick_v( t_CKTIME now, t_CKUINT numFrames )
{
    if( m_time >= now )
        return m_valid;
    
    t_CKUINT i, j; Chuck_UGen * ugen; SAMPLE factor;
    
    // inc time
    m_time = now;

    if( m_num_src )
    {
        ugen = m_src_list[0];
        if( ugen->m_time < now ) ugen->system_tick_v( now, numFrames );
        memcpy( m_sum_v, ugen->m_current_v, numFrames * sizeof(SAMPLE) );
        
        // tick the src list
        for( i = 1; i < m_num_src; i++ )
        {
            ugen = m_src_list[i];
            if( ugen->m_time < now ) ugen->system_tick_v( now, numFrames );
            if( ugen->m_valid )
            {
                if( m_op <= 1 )
                    for( j = 0; j < numFrames; j++ )
                        m_sum_v[j] += ugen->m_current_v[j];
                else // special ops
                {
                    switch( m_op )
                    {
                        case 2: for( j = 0; j < numFrames; j++ )
                            m_sum_v[j] -= ugen->m_current_v[j]; break;
                        case 3: for( j = 0; j < numFrames; j++ )
                            m_sum_v[j] *= ugen->m_current_v[j]; break;
                        case 4: for( j = 0; j < numFrames; j++ )
                            m_sum_v[j] /= ugen->m_current_v[j]; break;
                        default: for( j = 0; j < numFrames; j++ )
                            m_sum_v[j] += ugen->m_current_v[j]; break;
                    }
                }
            }
        }
    }
    else
    {
        memset( m_sum_v, 0, numFrames * sizeof(SAMPLE) );
    }

    // tick multiple channels
    if( m_multi_chan_size )
    {
        // initialize
        factor = 1.0f / m_multi_chan_size;
        // iterate
        for( i = 0; i < m_multi_chan_size; i++ )
        {
            ugen = m_multi_chan[i];
            if( ugen->m_time < now ) ugen->system_tick_v( now, numFrames );
            for( j = 0; j < numFrames; j++ )
                m_sum_v[j] += ugen->m_current_v[j] * factor;
        }
    }
    
    // if owner
    if( owner != NULL && owner->m_time < now )
        owner->system_tick_v( now, numFrames );
    
    if( m_op > 0 )  // UGEN_OP_TICK
    {
        // tick the ugen
        if( tick ) 
            for( j = 0; j < numFrames; j++ )
                m_valid = tick( this, m_sum_v[j], &(m_current_v[j]), NULL );
        if( !m_valid )
            for( j = 0; j < numFrames; j++ )
                m_current_v[j] = 0.0f;
        else
            for( j = 0; j < numFrames; j++ )
            {
                // apply gain and pan
                m_current_v[j] *= m_gain * m_pan;
                // dedenormal
                CK_DDN( m_current_v[j] );
            }
        // save as last
        m_last = m_current_v[numFrames-1];
        return m_valid;
    }
    else if( m_op < 0 ) // UGEN_OP_PASS
    {
        for( j = 0; j < numFrames; j++ )
        {
            // pass through
            m_current_v[j] = m_sum_v[j];
        }
        m_last = m_current_v[numFrames-1];
        return TRUE;
    }
    else // UGEN_OP_STOP
    {
        memset( m_current_v, 0, numFrames * sizeof(SAMPLE) );
        // m_current = 0.0f;
    }
    
    m_last = m_current_v[numFrames-1];
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: Chuck_UAna()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_UAna::Chuck_UAna() : Chuck_UGen()
{
    // mark as true
    m_is_uana = TRUE;
    // reset uana time (HACK: negative so upchuck() works at now=0)
    m_uana_time = -1;
    // zero out proxy
    // m_blob_proxy = NULL;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_UAna()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_UAna::~Chuck_UAna()
{
    // do nothing for now
}




//-----------------------------------------------------------------------------
// name: blobProxy()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_UAnaBlobProxy * Chuck_UAna::blobProxy() const { return getBlobProxy( this ); }




//-----------------------------------------------------------------------------
// name: numIncomingUAnae()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_UAna::numIncomingUAnae() const { return m_num_uana_src; }




//-----------------------------------------------------------------------------
// name: getIncomingBlob()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_UAnaBlobProxy * Chuck_UAna::getIncomingBlob( t_CKUINT index ) const
{
    // sanity check
    if( index >= m_num_uana_src )
        return NULL;

    // TODO: DANGER: this cast is very dangerous!
    return ((Chuck_UAna *)m_src_uana_list[index])->blobProxy();
}




//-----------------------------------------------------------------------------
// name: getIncomingUAna()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_UAna * Chuck_UAna::getIncomingUAna( t_CKUINT index ) const
{
    // sanity check
    if( index >= m_num_uana_src )
        return NULL;

    // TODO: DANGER: this cast is very dangerous!
    return ((Chuck_UAna *)m_src_uana_list[index]);
}




//-----------------------------------------------------------------------------
// name: is_up_connected_from()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UAna::is_up_connected_from( Chuck_UAna * src )
{
    if( m_src_uana_list != NULL && fa_lookup( m_src_uana_list, m_num_uana_src, src ) )
        return TRUE;

    // TODO: multichannel?

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: tock()
// dsec: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_UAna::system_tock( t_CKTIME now )
{
    if( m_uana_time >= now )
        return m_valid;

    t_CKUINT i; Chuck_UGen * ugen; Chuck_UAna * uana; SAMPLE multi;

    // inc time
    m_uana_time = now;
    if( m_num_src )
    {
        ugen = m_src_list[0];
        // TODO: address this horrendous hack
        if( ugen->m_is_uana )
        {
            // cast to uana
            uana = (Chuck_UAna *)ugen;
            // tock it
            if( uana->m_uana_time < now ) uana->system_tock( now );
        }
        // sum
        m_sum = ugen->m_current;

        // tick the src list
        for( i = 1; i < m_num_src; i++ )
        {
            ugen = m_src_list[i];
            // TODO: address this horrendous hack
            if( ugen->m_is_uana )
            {
                // cast to uana
                uana = (Chuck_UAna *)ugen;
                // tock it
                if( uana->m_uana_time < now ) uana->system_tock( now );
            }

            if( ugen->m_valid )
            {
                // TODO: operate on blob data
            }
        }
    }

    // tock multiple channels
    multi = 0.0f;
    if( m_multi_chan_size )
    {
        for( i = 0; i < m_multi_chan_size; i++ )
        {
            ugen = m_multi_chan[i];
            // TODO: address this horrendous hack
            if( ugen->m_is_uana )
            {
                // cast to uana
                uana = (Chuck_UAna *)ugen;
                // tock it
                if( uana->m_uana_time < now ) uana->system_tock( now );
            }
        }
    }

    // if owner
    if( owner != NULL && owner->m_is_uana )
    {
        // cast to uana
        uana = (Chuck_UAna *)owner;
        // tock it
        if( uana->m_uana_time < now ) uana->system_tock( now );
    }

    if( m_op > 0 )  // UGEN_OP_TOCK
    {
        // tock the uana
        if( tock ) m_valid = tock( this, this, blobProxy(), NULL );
        if( !m_valid ) { /* clear out blob? */ }
		// timestamp the blob
		blobProxy()->when() = now;
        // TODO: set current_blob to out_blob
        // TODO: set last_blob to current
        return m_valid;
    }
    else if( m_op < 0 ) // UGEN_OP_PASS
    {
        // pass through
        // TODO: anything?
        return TRUE;
    }
    else // UGEN_OP_STOP
    {
        // TODO: anything?
    }
    
    // TODO: set m_last_blob to current?

    return TRUE;
}
