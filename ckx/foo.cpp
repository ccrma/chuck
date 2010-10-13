//------------------------------------------------------------------------------
// name: foo_ckx.cpp
// desc: source for ChucK DLL (foo.ckx) - an example
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//
// *see http://chuck.cs.princeton.edu/ for complete documentation
//
// compile this file in the following way:
// LINUX/CYGWIN:
// > gcc -shared -I$(CHUCK_DIR) foo_ckx.cpp -o foo.ckx -lstdc++
// MacOS X:
// > gcc -bundle -I$(CHUCK_DIR) foo_ckx.cpp -o foo.ckx -lstdc++
//
// ChucK DLL description:
// - every DLL should have exactly one query-function called 'ck_query'
// - for every exported function there should be
//   - the implementation function
// for example, to export the function 'int foo( int x )'
//   - the implementation
//     'CK_DLL_EXPORT(void) foo( void * ARGS, Chuck_DL_Return * RETURN )'
//------------------------------------------------------------------------------
#include "chuck_dl.h"
#include "chuck_ugen.h"

// query function prototype
CK_DLL_QUERY;

// functions to export
DLL_FUNC bar( void * ARGS, Chuck_DL_Return * RETURN );
DLL_FUNC sum( void * ARGS, Chuck_DL_Return * RETURN );

// ugen prototypes

// ChucK DLL query function
// note: this uses the CK_DLL_QUERY macro which expands to:
//   CK_DLL_EXPORT(t_CKBOOL) ck_query( Chuck_DL_Query * QUERY )
CK_DLL_QUERY
{
    QUERY->set_name( QUERY, "foo" );
    
    // add foo
    QUERY->add_export( QUERY, "int", "bar", bar, TRUE );
    QUERY->add_param( QUERY, "int", "x" );
    
    // add sum
    QUERY->add_export( QUERY, "float", "sum", sum, TRUE );
    QUERY->add_param( QUERY, "float", "lhs" );
    QUERY->add_param( QUERY, "float", "rhs" );

    return TRUE;
}


// ChucK DLL implementation for 'bar'
// note: this uses the DLL_FUNC macro which expands to:
//   CK_DLL_EXPORT(void)
DLL_FUNC bar( void * ARGS, Chuck_DL_Return * RETURN )
{
    RETURN->v_int = *((int *)ARGS) + 1;
}


// ChucK DLL implementation for 'sum'
// note: this uses the CK_DLL_FUNC(x) macro which expands to:
//   CK_DLL_EXPORT(void) x(void *ARGS, Chuck_DL_Return * RETURN );
CK_DLL_FUNC(sum)
{
    // use a struct
    struct s_sum{ t_CKFLOAT lhs; t_CKFLOAT rhs; };

    s_sum * s = (s_sum*)ARGS;
    RETURN->v_float = s->lhs + s->rhs;
}
