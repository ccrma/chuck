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
// file: chuck_stats.h
// desc: statistics - for audicle
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip Davidson (philipd@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __CHUCK_STATS_H__
#define __CHUCK_STATS_H__

#include "chuck_def.h"

// tracking
#if defined(__CHUCK_STAT_TRACK__)

#include "util_thread.h"

#include <string>
#include <map>
#include <queue>


// forward reference
struct Chuck_VM;
struct Chuck_VM_Shred;
struct Shred_Data;
struct Shred_Time;




//-----------------------------------------------------------------------------
// name: struct Shred_Activation
// desc: ...
//-----------------------------------------------------------------------------
struct Shred_Activation
{
    t_CKTIME when;
    t_CKUINT cycles;
    
    Shred_Activation( t_CKTIME a, t_CKUINT b ) { when = a; cycles = b; }
};




//-----------------------------------------------------------------------------
// name: struct Shred_Stat
// desc: ...
//-----------------------------------------------------------------------------
struct Shred_Stat
{
public:
    // instructions computed
    t_CKUINT cycles;
    // shred id
    t_CKUINT xid;
    // parent
    t_CKUINT parent;
    // current state, 0 = inactive, 1 = active, 2 = wait, 3 = deleted
    t_CKUINT state;
    // reference (could be pointing to garbage - see state)
    Chuck_VM_Shred * shred_ref;
    // number of activations
    t_CKUINT activations;
    // average control rate
    t_CKFLOAT average_ctrl;
    // average cycles
    t_CKFLOAT average_cycles;
    // spork time
    t_CKTIME spork_time;
    // active time
    t_CKTIME active_time;
    // wake time
    t_CKTIME wake_time;
    // free time
    t_CKTIME free_time;
    // name
    std::string name;
    // owner
    std::string owner;
    // source
    std::string source;
    
    // ctrl rate calculation
    std::queue<t_CKDUR> diffs;
    // number of diffs
    t_CKUINT num_diffs;
    // total diffs
    t_CKDUR diff_total;
    
    // exe per activation
    std::queue<t_CKUINT> act_cycles;
    // total
    t_CKUINT act_cycles_total;
    // last
    t_CKUINT last_cycles;
    
    // children
    std::vector<Shred_Stat *> children;
    void get_sporked( std::vector<Shred_Stat *> & out );

    std::vector<Shred_Activation> activationss;
    void get_activations( std::vector<Shred_Activation> & out );
    
    // mutex
    XMutex mutex;
    
    // audicle info
    Shred_Data * data;
    Shred_Time * time;

public:
    Shred_Stat() { this->clear(); num_diffs = 8; data = NULL; time = NULL; }
    void clear()
    { xid = 0; parent = 0; state = 0; cycles = 0; activations = 0;
      average_ctrl = 0.0; spork_time = 0.0; active_time = 0.0; wake_time = 0.0;
      free_time = 0.0; name = "no name"; owner = "none"; source = "nowhere";
      while( diffs.size() ) diffs.pop(); diff_total = 0.0;
      while( act_cycles.size() ) act_cycles.pop(); act_cycles_total = 0;
      last_cycles = 0; children.clear(); }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Stats
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Stats
{
public:
    static Chuck_Stats * instance();

public:
    void set_vm_ref( Chuck_VM * _vm ) { vm = _vm; } 

public:
    void add_shred( Chuck_VM_Shred * shred );
    void activate_shred( Chuck_VM_Shred * shred );
    void advance_time( Chuck_VM_Shred * shred, t_CKTIME to );
    void deactivate_shred( Chuck_VM_Shred * shred );
    void remove_shred( Chuck_VM_Shred * shred );

public:
    Shred_Stat * get_shred( t_CKUINT xid )
    { mutex.acquire(); Shred_Stat * s = shreds[xid]; mutex.release(); return s; }
    void get_shreds( std::vector<Shred_Stat *> & out, 
                     std::map<Shred_Stat *, Shred_Stat *> & d );
    static t_CKBOOL activations_yes;

protected:
    Chuck_Stats();
    ~Chuck_Stats();
    
    static Chuck_Stats * our_instance;
    
protected:
    Chuck_VM * vm;
    std::map<t_CKUINT, Shred_Stat *> shreds;
    std::vector<Shred_Stat *> done;
    XMutex mutex;
};




#endif

#endif
