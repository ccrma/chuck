//------------------------------------------------------------------------------
// name: ckdl.cpp
// desc: ChucK dynamic link util
//       this util is embedded in chuck, and is accessible via flags
//
// for more documentation, see http://chuck.cs.princeton.edu/
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//------------------------------------------------------------------------------ 
#include "chuck_dl.h"
#include <iostream>
using namespace std;

// entry point
int main( int argc, char ** argv )
{
    if( argc < 2 )
    {
        cerr << "[ckdl]: no input file..." << endl;
        cerr << "usage: ckdl file.ckx [entry point]" << endl;
        cerr << endl;
        cerr << "[ChucK dynamic linking utility]" << endl;
        cerr << "    http://chuck.cs.princeton.edu/" << endl;
        return 1;
    }

    Chuck_DLL dll;
    dll.load( argv[1], (argc > 2 ? argv[2] : CK_QUERY_FUNC) );
    if( !dll.good() )
    {
        cerr << "error: " << dll.last_error() << endl;
        return 1;
    }

    const Chuck_DL_Query * query;
    if( !(query = dll.query( )) )
    {
        cerr << "error: " << dll.last_error() << endl;
        return 1;
    }
    
    cerr << endl;
    cerr << "reading ChucK DLL '" << query->dll_name << "' interface: " << endl;
    cerr << endl;
    cerr << "--- [functions/values] ---" << endl;

    for( int i = 0; i < query->dll_exports.size(); i++ )
    {
        const Chuck_DL_Proto * proto = &query->dll_exports[i];

        if( proto->is_func )
        {
            cerr << "[function]: " << proto->type << " " << proto->name << "( ";
            for( int j = 0; j < proto->params.size(); j++ )
            {
                cerr << proto->params[j].type << " " << proto->params[j].name;
                if( j < ( proto->params.size() - 1 ) )
                    cerr << ",";
                cerr << " ";
            }
            cerr << ");" << endl;
        }
        else
        {
            cerr << "[value]: " << proto->type << " " << proto->name << ";" << endl;
        }
    }

    // none?
    if( query->dll_exports.size() == 0 )
        cerr << "(NO FUNCTION/VALUE IMPORTS)" << endl;

    cerr << endl;
    cerr << "--- [unit generators] ---" << endl;

    for( int m = 0; m < query->ugen_exports.size(); m++ )
    {
        const Chuck_UGen_Info * info = &query->ugen_exports[m];
        t_CKUINT r, w;

        cerr << "[ugen]: " << info->name << endl;
        cerr << "   [ctrl params]: " << endl;
        for( int n = 0; n < info->param_list.size(); n++ )
        {
            cerr << "       .";
            cerr << info->param_list[n].name << "  ----  (" 
                 << info->param_list[n].type << ", ";
            w = info->param_list[n].ctrl_addr != 0;
            r = info->param_list[n].cget_addr != 0;
            if( w && r ) cerr << "READ/WRITE)" << endl;
            else cerr << ( r ? "read ONLY)" : "write ONLY)" ) << endl;
        }
        cerr << "   [funcs]: ( ctor: " << (void *)info->ctor << ", ";
        cerr << "dtor: " << (void *)info->dtor << ", ";
        cerr << "tick: " << (void *)info->tick << ", ";
        cerr << "pmsg: " << (void *)info->pmsg << " )" << endl;
    }

    // none?
    if( query->ugen_exports.size() == 0 )
        cerr << "(NO UNIT GENERATOR IMPORTS)" << endl;
    
    cerr << endl;
    cerr << "---" << endl;
    cerr << endl;

    return 0;
}
