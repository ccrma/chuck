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
// file: chuck_bbq.cpp
// desc: bbq implementation
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "chuck_bbq.h"
#include <math.h>
#include <memory.h>
#include <stdlib.h>

 


//-----------------------------------------------------------------------------
// name: BBQ()
// desc: ...
//-----------------------------------------------------------------------------
BBQ::BBQ()
{
    m_max_midi_device = 1024;
    m_digi_out = new DigitalOut;
    m_digi_in = new DigitalIn;
    m_midi_out = new MidiOut * [m_max_midi_device];
    m_midi_in = new MidiIn * [m_max_midi_device];
    m_in_count = new DWORD__[m_max_midi_device];
    m_out_count = new DWORD__[m_max_midi_device];
    memset( m_midi_out, 0, m_max_midi_device * sizeof(MidiOut *) );
    memset( m_midi_in, 0, m_max_midi_device * sizeof(MidiIn *) );
    memset( m_in_count, 0, m_max_midi_device * sizeof(DWORD__) );
    memset( m_out_count, 0, m_max_midi_device * sizeof(DWORD__) );
    //m_midi_out[0] = new MidiOut;
    //m_midi_in[0] = new MidiIn;
}




//-----------------------------------------------------------------------------
// name: ~BBQ()
// desc: ...
//-----------------------------------------------------------------------------
BBQ::~BBQ()
{
    SAFE_DELETE( m_digi_out );
    SAFE_DELETE( m_digi_in );
    SAFE_DELETE_ARRAY( m_midi_out );
    SAFE_DELETE_ARRAY( m_midi_in );
    SAFE_DELETE_ARRAY( m_in_count );
    SAFE_DELETE_ARRAY( m_out_count );
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ BBQ::initialize( DWORD__ num_dac_channels, DWORD__ num_adc_channels,
                        DWORD__ sampling_rate, DWORD__ bps, DWORD__ buffer_size,
                        DWORD__ num_buffers, DWORD__ dac, DWORD__ adc, 
                        DWORD__ block, Chuck_VM * vm_ref, BOOL__ rt_audio,
                        void * callback, void * data, t_CKBOOL force_srate )
{
    set_inouts( adc, dac );
    set_chans( num_adc_channels, num_dac_channels );

    return Digitalio::initialize( 
        num_dac_channels, num_adc_channels,
        sampling_rate, bps, buffer_size, num_buffers, 
        block, vm_ref, rt_audio, callback, data,
        force_srate
    );
}




//-----------------------------------------------------------------------------
// name: set_srate()
// desc: ...
//-----------------------------------------------------------------------------
void BBQ::set_srate( DWORD__ srate )
{
    Digitalio::m_sampling_rate = srate;
}




//-----------------------------------------------------------------------------
// name: set_bufsize()
// desc: ...
//-----------------------------------------------------------------------------
void BBQ::set_bufsize( DWORD__ bufsize )
{
    Digitalio::m_buffer_size = bufsize;
}



//-----------------------------------------------------------------------------
// name: set_bufnums()
// desc: ...
//-----------------------------------------------------------------------------
void BBQ::set_numbufs( DWORD__ numbufs )
{
    Digitalio::m_num_buffers = numbufs;
}




//-----------------------------------------------------------------------------
// name: set_inouts()
// desc: ...
//-----------------------------------------------------------------------------
void BBQ::set_inouts( DWORD__ adc, DWORD__ dac )
{
    Digitalio::m_dac_n = dac;
    Digitalio::m_adc_n = adc;
}




//-----------------------------------------------------------------------------
// name: set_chans()
// desc: ...
//-----------------------------------------------------------------------------
void BBQ::set_chans( DWORD__ ins, DWORD__ outs )
{
    Digitalio::m_num_channels_in = ins;
    Digitalio::m_num_channels_out = outs;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void BBQ::shutdown()
{
    Digitalio::shutdown();
}




//-----------------------------------------------------------------------------
// name: digi_out()
// desc: digital out
//-----------------------------------------------------------------------------
DigitalOut * BBQ::digi_out()
{
    return m_digi_out;
}




//-----------------------------------------------------------------------------
// name: digi_in()
// desc: digital in
//-----------------------------------------------------------------------------
DigitalIn * BBQ::digi_in()
{
    return m_digi_in;
}




//-----------------------------------------------------------------------------
// name: midi_out()
// desc: midi out
//-----------------------------------------------------------------------------
MidiOut * BBQ::midi_out( DWORD__ index )
{
    if( index >= m_max_midi_device )
        return NULL;

    if( m_midi_out[index] == NULL )
    {
        m_midi_out[index] = new MidiOut;
        if( !m_midi_out[index]->open( index ) )
        {
            delete m_midi_out[index];
            m_midi_out[index] = NULL;
            m_out_count[index]++;
        }
        else
            m_out_count[index] = 0;
    }

    return m_midi_out[index];
}




//-----------------------------------------------------------------------------
// name: midi_in()
// desc: ...
//-----------------------------------------------------------------------------
MidiIn * BBQ::midi_in( DWORD__ index )
{
    if( index >= m_max_midi_device )
        return NULL;
        
    if( m_midi_in[index] == NULL )
    {
        m_midi_in[index] = new MidiIn;
        if( !m_midi_in[index]->open( index ) )
        {
            delete m_midi_in[index];
            m_midi_in[index] = NULL;
            m_in_count[index]++;
        }
        else
            m_in_count[index] = 0;
    }

    return m_midi_in[index];
}




//-----------------------------------------------------------------------------
// name: RangeMapper()
// desc: constructor
//-----------------------------------------------------------------------------
RangeMapper::RangeMapper( SAMPLE low_l, SAMPLE high_l,
                          SAMPLE low_r, SAMPLE high_r )
{
    SAMPLE dx = high_l - low_l;
    if( fabs( (double)dx ) < .000001 )
        m_slope = 0.0f;
    else
        m_slope = ( high_r - low_r ) / dx;

    m_inter = high_r - m_slope * high_l;
}




//-----------------------------------------------------------------------------
// name: map()
// desc: ...
//-----------------------------------------------------------------------------
SAMPLE RangeMapper::map( SAMPLE x )
{
    return m_slope * x + m_inter;
}
