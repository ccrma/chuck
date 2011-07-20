

#include "chuck_dl.h"

#include <stdio.h>



CK_DLL_CTOR(bar_ctor);
CK_DLL_DTOR(bar_dtor);

CK_DLL_SFUN(bar_sfoo);

CK_DLL_MFUN(bar_mfoo);
CK_DLL_MFUN(bar_printMvarInt);
CK_DLL_MFUN(bar_printMvarFloat);
CK_DLL_MFUN(bar_printMvarDur);
CK_DLL_MFUN(bar_printMvarTime);
CK_DLL_MFUN(bar_printMvarString);

CK_DLL_MFUN(bar_intArg);
CK_DLL_MFUN(bar_floatArg);
CK_DLL_MFUN(bar_durArg);
CK_DLL_MFUN(bar_timeArg);
CK_DLL_MFUN(bar_stringArg);
CK_DLL_MFUN(bar_objectArg);
CK_DLL_MFUN(bar_intFloatStringArg);

static t_CKUINT bar_mvarInt_offset = 0;
static t_CKUINT bar_mvarFloat_offset = 0;
static t_CKUINT bar_mvarDur_offset = 0;
static t_CKUINT bar_mvarTime_offset = 0;
static t_CKUINT bar_mvarString_offset = 0;


CK_DLL_QUERY(bar)
{
    QUERY->setname(QUERY, "bar");
    
    QUERY->begin_class(QUERY, "Bar", "Object");
    
    QUERY->add_ctor(QUERY, bar_ctor);
    
    QUERY->add_dtor(QUERY, bar_dtor);
    
    QUERY->add_sfun(QUERY, bar_sfoo, "void", "sfoo");
    
    QUERY->add_mfun(QUERY, bar_mfoo, "void", "mfoo");
    QUERY->add_mfun(QUERY, bar_printMvarInt, "void", "printMvarInt");
    QUERY->add_mfun(QUERY, bar_printMvarFloat, "void", "printMvarFloat");
    QUERY->add_mfun(QUERY, bar_printMvarDur, "void", "printMvarDur");
    QUERY->add_mfun(QUERY, bar_printMvarTime, "void", "printMvarTime");
    QUERY->add_mfun(QUERY, bar_printMvarString, "void", "printMvarString");
    
    QUERY->add_mfun(QUERY, bar_intArg, "void", "intArg");
    QUERY->add_arg(QUERY, "int", "arg");
    
    QUERY->add_mfun(QUERY, bar_floatArg, "void", "floatArg");
    QUERY->add_arg(QUERY, "float", "arg");

    QUERY->add_mfun(QUERY, bar_durArg, "void", "durArg");
    QUERY->add_arg(QUERY, "dur", "arg");
    
    QUERY->add_mfun(QUERY, bar_timeArg, "void", "timeArg");
    QUERY->add_arg(QUERY, "time", "arg");
    
    QUERY->add_mfun(QUERY, bar_stringArg, "void", "stringArg");
    QUERY->add_arg(QUERY, "string", "arg");
    
    QUERY->add_mfun(QUERY, bar_objectArg, "void", "objectArg");
    QUERY->add_arg(QUERY, "Object", "arg");
    
    QUERY->add_mfun(QUERY, bar_intFloatStringArg, "void", "intFloatStringArg");
    QUERY->add_arg(QUERY, "int", "intArg");
    QUERY->add_arg(QUERY, "float", "floatArg");
    QUERY->add_arg(QUERY, "string", "stringArg");
        
        
    bar_mvarInt_offset = QUERY->add_mvar(QUERY, "int", "mvarInt", false);
    bar_mvarFloat_offset = QUERY->add_mvar(QUERY, "float", "mvarFloat", false);
    bar_mvarDur_offset = QUERY->add_mvar(QUERY, "dur", "mvarDur", false);
    bar_mvarTime_offset = QUERY->add_mvar(QUERY, "time", "mvarTime", false);
    bar_mvarString_offset = QUERY->add_mvar(QUERY, "string", "mvarString", false);
    
    
    QUERY->end_class(QUERY);

    return TRUE;
}

CK_DLL_CTOR(bar_ctor)
{
    fprintf(stderr, "calling bar_ctor\n");
    
    OBJ_MEMBER_INT(SELF, bar_mvarInt_offset) = 0;
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

CK_DLL_MFUN(bar_printMvarInt)
{
    fprintf(stderr, "value of mvarInt: %li\n", 
            OBJ_MEMBER_INT(SELF, bar_mvarInt_offset));
}

CK_DLL_MFUN(bar_printMvarFloat)
{
    fprintf(stderr, "value of mvarFloat: %lf\n", 
            OBJ_MEMBER_FLOAT(SELF, bar_mvarFloat_offset));
}

CK_DLL_MFUN(bar_printMvarDur)
{
    fprintf(stderr, "value of mvarDur: %lf\n", 
            OBJ_MEMBER_DUR(SELF, bar_mvarDur_offset));
}

CK_DLL_MFUN(bar_printMvarTime)
{
    fprintf(stderr, "value of mvarTime: %lf\n", 
            OBJ_MEMBER_TIME(SELF, bar_mvarTime_offset));
}

CK_DLL_MFUN(bar_printMvarString)
{
    fprintf(stderr, "value of mvarString: %s\n", 
            OBJ_MEMBER_STRING(SELF, bar_mvarString_offset)->str.c_str());
}


CK_DLL_MFUN(bar_intArg)
{
    fprintf(stderr, "calling bar_intArg with arg: %li\n", 
            GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(bar_floatArg)
{
    fprintf(stderr, "calling bar_floatArg with arg: %lf\n", 
            GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(bar_durArg)
{
    fprintf(stderr, "calling bar_durArg with arg: %lf\n", 
            GET_NEXT_DUR(ARGS));
}

CK_DLL_MFUN(bar_timeArg)
{
    fprintf(stderr, "calling bar_timeArg with arg: %lf\n", 
            GET_NEXT_TIME(ARGS));
}

CK_DLL_MFUN(bar_stringArg)
{
    fprintf(stderr, "calling bar_stringArg with arg: %s\n", 
            GET_NEXT_STRING(ARGS)->str.c_str());
}

CK_DLL_MFUN(bar_objectArg)
{
    //fprintf(stderr, "calling bar_intArg with arg: %li\n", GET_NEXT_INT(ARGS));
}


CK_DLL_MFUN(bar_intFloatStringArg)
{
    t_CKINT intArg = GET_NEXT_INT(ARGS);
    t_CKFLOAT floatArg = GET_NEXT_FLOAT(ARGS);
    Chuck_String * stringArg = GET_NEXT_STRING(ARGS);
    
    fprintf(stderr, "calling bar_intFloatStringArg with args: %li %lf %s\n", 
            intArg, floatArg, stringArg->str.c_str());
}

