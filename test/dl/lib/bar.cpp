

#include "chuck_dl.h"

#include <stdio.h>



CK_DLL_CTOR(bar_ctor);
CK_DLL_DTOR(bar_dtor);

CK_DLL_SFUN(bar_sfoo);

CK_DLL_MFUN(bar_mfoo);
CK_DLL_MFUN(bar_printMvar);

static t_CKUINT bar_mvar_offset = 0;
static t_CKUINT bar_mvarFloat_offset = 0;

CK_DLL_QUERY
{
    QUERY->setname(QUERY, "bar");
    
    QUERY->begin_class(QUERY, "Bar", "Object");
    
    QUERY->add_ctor(QUERY, bar_ctor);
    
    QUERY->add_dtor(QUERY, bar_dtor);
    
    QUERY->add_sfun(QUERY, bar_sfoo, "void", "sfoo");
    
    QUERY->add_mfun(QUERY, bar_mfoo, "void", "mfoo");
    
    //QUERY->add_mfun(QUERY, bar_mfoo, "void", "mfooArg");
    
    bar_mvar_offset = QUERY->add_mvar(QUERY, "int", "mvar", false);
    bar_mvarFloat_offset = QUERY->add_mvar(QUERY, "float", "mvarFloat", false);
    
    QUERY->add_mfun(QUERY, bar_printMvar, "void", "printMvar");
    
    QUERY->end_class(QUERY);
}

CK_DLL_CTOR(bar_ctor)
{
    fprintf(stderr, "calling bar_ctor\n");
    
    OBJ_MEMBER_INT(SELF, bar_mvar_offset) = 0;
}

CK_DLL_DTOR(bar_dtor)
{
    fprintf(stderr, "calling bar_dtor\n");
}

CK_DLL_SFUN(bar_sfoo)
{
    fprintf(stderr, "calling bar_sfoo\n");
}

CK_DLL_MFUN(bar_mfoo)
{
    fprintf(stderr, "calling bar_mfoo\n");
}

CK_DLL_MFUN(bar_printMvar)
{
    fprintf(stderr, "value of mvar: %li\n", OBJ_MEMBER_INT(SELF, bar_mvar_offset));
}

