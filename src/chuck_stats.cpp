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
// file: chuck_stats.cpp
// desc: statistics - for audicle
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip Davidson (philipd@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "chuck_stats.h"

// tracking
#if defined(__CHUCK_STAT_TRACK__)

#include "chuck_vm.h"
using namespace std;

// static members
Chuck_Stats * Chuck_Stats::our_instance = NULL;
t_CKBOOL Chuck_Stats::activations_yes = FALSE;


//-----------------------------------------------------------------------------
// name: instance()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Stats * Chuck_Stats::instance()
{
    if( !our_instance )
    {
        our_instance = new Chuck_Stats;
        assert( our_instance );
    }
    
    return our_instance;
}




//-----------------------------------------------------------------------------
// name: add_shred()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Stats::add_shred( Chuck_VM_Shred * shred )
{
    assert( vm != NULL );
    assert( shred->stat == NULL );
    assert( shred->xid != 0 );
    
    Shred_Stat * stat = new Shred_Stat;
    stat->xid = shred->xid;
    stat->parent = shred->parent ? shred->parent->xid : 0;
    stat->state = 0; // inactive
    stat->shred_ref = shred;
    stat->spork_time = shred->wake_time;
    stat->wake_time = shred->wake_time;
    stat->name = shred->name;
    stat->owner = "noone";
    stat->source = "remote";
    if( strncmp( shred->name.c_str(), "editor->", 8 ) == 0 )
    {
        stat->name = shred->name.c_str() + 8;
        stat->source = "editor";
    }
    if( shred->parent )
    {
        Shred_Stat * another = shreds[shred->parent->xid];
        another->mutex.acquire();
        another->children.push_back( stat );
        another->mutex.release();
    }
    
    // attach to shred
    shred->stat = stat;
    // add
    mutex.acquire();
    shreds[shred->xid] = stat;
    mutex.release();
}




//-----------------------------------------------------------------------------
// name: activate_shred()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Stats::activate_shred( Chuck_VM_Shred * shred )
{
    assert( vm !=  NULL );
    //assert( shred->stat != NULL );
    //assert( shred->id != 0 );
    
    Shred_Stat * stat = shred->stat;
    // set active state
    stat->state = 1;  // active
    // increment activations
    stat->activations++;
    // set the active time
    stat->active_time = shred->wake_time;
}




//-----------------------------------------------------------------------------
// name: advance_time()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Stats::advance_time( Chuck_VM_Shred * shred, t_CKTIME to )
{
    assert( vm != NULL );
    
    Shred_Stat * stat = shred->stat;
    // set the wake_time
    stat->wake_time = to;
    // find the period
    t_CKDUR diff = stat->wake_time - stat->active_time;
    // add to diffs
    stat->diff_total += diff;
    // put in queue
    stat->diffs.push( diff );
    // subtract
    if( stat->diffs.size() > stat->num_diffs )
    {
        stat->diff_total -= stat->diffs.front();
        stat->diffs.pop();
    }
    // calculate average
    stat->average_ctrl = stat->diff_total / stat->diffs.size();
}




//-----------------------------------------------------------------------------
// name: deactivate_shred()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Stats::deactivate_shred( Chuck_VM_Shred * shred )
{
    assert( vm != NULL );

    Shred_Stat * stat = shred->stat;
    // get diff
    t_CKUINT diff = stat->cycles - stat->last_cycles;
    // set active state
    stat->state = 2;
    // find average
    stat->act_cycles.push( diff );
    // add to total
    stat->act_cycles_total += diff;
    // subtract
    if( stat->act_cycles.size() > stat->num_diffs )
    {
        stat->act_cycles_total -= stat->act_cycles.front();
        stat->act_cycles.pop();
    }
    // the average
    stat->average_cycles = (t_CKFLOAT)stat->act_cycles_total / stat->act_cycles.size();
    // remember the cycle count
    stat->last_cycles = stat->cycles;
    if( activations_yes )
    {
        stat->mutex.acquire();
        stat->activationss.push_back( Shred_Activation( vm->shreduler()->now_system, diff ) );
        stat->mutex.release();
    }
}




//-----------------------------------------------------------------------------
// name: remove_shred()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Stats::remove_shred( Chuck_VM_Shred * shred )
{
    assert( vm != NULL );
    
    Shred_Stat * stat = shred->stat;
    // set state
    stat->state = 3;  // deleted
    // set free time
    stat->free_time = vm->shreduler()->now_system;
    // remove
    mutex.acquire();
    shreds.erase( shreds.find( stat->xid ) );
    done.push_back( stat );
    mutex.release();
}




//-----------------------------------------------------------------------------
// name: get_shreds()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Stats::get_shreds( vector<Shred_Stat *> & out, map<Shred_Stat *, Shred_Stat *> & d )
{
    assert( vm != NULL );
    
    map<t_CKUINT, Shred_Stat *>::iterator iter;
    out.clear();

    mutex.acquire();
    for( iter = shreds.begin(); iter != shreds.end(); iter++ )
        out.push_back( (*iter).second );
    for( int i = 0; i < done.size(); i++ )
        d[done[i]] = done[i];
    done.clear(); 
    mutex.release();
}





//-----------------------------------------------------------------------------
// name: Chuck_Stats()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Stats::Chuck_Stats()
{
    vm = NULL;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Stats()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Stats::~Chuck_Stats()
{
}




//-----------------------------------------------------------------------------
// name: get_sporked()
// desc: ...
//-----------------------------------------------------------------------------
void Shred_Stat::get_sporked( vector<Shred_Stat *> & out )
{
    out.clear();
    
    mutex.acquire();
    for( int i = 0; i < children.size(); i++ )
        out.push_back( children[i] );
    mutex.release();
}




//-----------------------------------------------------------------------------
// name: get_activations()
// desc: ...
//-----------------------------------------------------------------------------
void Shred_Stat::get_activations( vector<Shred_Activation> & out )
{
    out.clear();
    
    mutex.acquire();
    out = activationss;
    activationss.clear();
    mutex.release();
}


#endif
