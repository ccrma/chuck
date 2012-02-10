

//-------------------------------------------------------------------
// FAUST architecture file for ChucK.
// Copyright (C) 2012 Spencer Salazar.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
// 02111-1307 USA
//-------------------------------------------------------------------

#include "chuck_dl.h"
#include "chuck_def.h"

#include <stdio.h>
#include <limits.h>

//-------------------------------------------------------------------
// Generic min and max using C++ inline
//-------------------------------------------------------------------

inline int      max (unsigned int a, unsigned int b) { return (a>b) ? a : b; }
inline int      max (int a, int b)          { return (a>b) ? a : b; }

inline long     max (long a, long b)        { return (a>b) ? a : b; }
inline long     max (int a, long b)         { return (a>b) ? a : b; }
inline long     max (long a, int b)         { return (a>b) ? a : b; }

inline float    max (float a, float b)      { return (a>b) ? a : b; }
inline float    max (int a, float b)        { return (a>b) ? a : b; }
inline float    max (float a, int b)        { return (a>b) ? a : b; }
inline float    max (long a, float b)       { return (a>b) ? a : b; }
inline float    max (float a, long b)       { return (a>b) ? a : b; }

inline double   max (double a, double b)    { return (a>b) ? a : b; }
inline double   max (int a, double b)       { return (a>b) ? a : b; }
inline double   max (double a, int b)       { return (a>b) ? a : b; }
inline double   max (long a, double b)      { return (a>b) ? a : b; }
inline double   max (double a, long b)      { return (a>b) ? a : b; }
inline double   max (float a, double b)     { return (a>b) ? a : b; }
inline double   max (double a, float b)     { return (a>b) ? a : b; }


inline int      min (int a, int b)          { return (a<b) ? a : b; }

inline long     min (long a, long b)        { return (a<b) ? a : b; }
inline long     min (int a, long b)         { return (a<b) ? a : b; }
inline long     min (long a, int b)         { return (a<b) ? a : b; }

inline float    min (float a, float b)      { return (a<b) ? a : b; }
inline float    min (int a, float b)        { return (a<b) ? a : b; }
inline float    min (float a, int b)        { return (a<b) ? a : b; }
inline float    min (long a, float b)       { return (a<b) ? a : b; }
inline float    min (float a, long b)       { return (a<b) ? a : b; }

inline double   min (double a, double b)    { return (a<b) ? a : b; }
inline double   min (int a, double b)       { return (a<b) ? a : b; }
inline double   min (double a, int b)       { return (a<b) ? a : b; }
inline double   min (long a, double b)      { return (a<b) ? a : b; }
inline double   min (double a, long b)      { return (a<b) ? a : b; }
inline double   min (float a, double b)     { return (a<b) ? a : b; }
inline double   min (double a, float b)     { return (a<b) ? a : b; }

inline int      lsr (int x, int n)          { return int(((unsigned int)x) >> n); }
inline int      int2pow2 (int x)            { int r=0; while ((1<<r)<x) r++; return r; }


//----------------------------------------------------------------------------
// FAUST generated code
//----------------------------------------------------------------------------

<<includeIntrinsic>>

/******************************************************************************
 *******************************************************************************
 
 FAUST META DATA
 
 *******************************************************************************
 *******************************************************************************/

struct Meta : std::map<std::string, std::string>
{
    void declare(const char* key, const char* value)
    {
        (*this)[key] = value;
    }
};

/******************************************************************************
 *******************************************************************************
 
 FAUST UI
 
 *******************************************************************************
 *******************************************************************************/


class UI
{
public:
    virtual ~UI() { }
    
    // active widgets
    virtual void addButton(const char* label, float* zone) = 0;
    virtual void addToggleButton(const char* label, float* zone) = 0;
    virtual void addCheckButton(const char* label, float* zone) = 0;
    virtual void addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step) = 0;
    virtual void addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step) = 0;
    virtual void addNumEntry(const char* label, float* zone, float init, float min, float max, float step) = 0;
    
    // passive widgets
    virtual void addNumDisplay(const char* label, float* zone, int precision) = 0;
    virtual void addTextDisplay(const char* label, float* zone, char* names[], float min, float max) = 0;
    virtual void addHorizontalBargraph(const char* label, float* zone, float min, float max) = 0;
    virtual void addVerticalBargraph(const char* label, float* zone, float min, float max) = 0;
    
    // layout widgets
    virtual void openFrameBox(const char* label) = 0;
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    virtual void declare(float* zone, const char* key, const char* value) {}
};


/******************************************************************************
 *******************************************************************************
 
 FAUST DSP
 
 *******************************************************************************
 *******************************************************************************/

//----------------------------------------------------------------------------
// Abstract DSP interface
//----------------------------------------------------------------------------

class dsp
{
public:
    virtual ~dsp();
    virtual int getNumInputs()                                      = 0;
    virtual int getNumOutputs()                                     = 0;
    virtual void buildUserInterface(UI* interface)                  = 0;
    virtual void init(int samplingRate)                             = 0;
    virtual void compute(int len, float** inputs, float** outputs)  = 0;
    
protected:
    int fSamplingFreq;
};

dsp::~dsp() { }

//----------------------------------------------------------------------------
// FAUST generated code
//----------------------------------------------------------------------------

<<includeclass>>


/******************************************************************************
 *******************************************************************************
 
 CHUCK DSP INTERFACE
 
 *******************************************************************************
 *******************************************************************************/

#define MACRO_STR_EXPAND(tok) #tok
#define MACRO_STR(tok) MACRO_STR_EXPAND(tok)


CK_DLL_CTOR(fauck_ctor);
CK_DLL_DTOR(fauck_dtor);

//CK_DLL_MFUN(bitcrusher_setBits);
//CK_DLL_MFUN(bitcrusher_getBits);
//CK_DLL_MFUN(bitcrusher_setDownsampleFactor);
//CK_DLL_MFUN(bitcrusher_getDownsampleFactor);

CK_DLL_TICK(fauck_tick);

t_CKINT fauck_data_offset = 0;


class Fauck
{
public:
    Fauck(int samplingRate)
    {
        dsp.init(samplingRate);
    }
    
    mydsp dsp;
};


CK_DLL_QUERY(__CHUGIN_NAME__)
{
    mydsp temp; // needed to get IO channel count
    
    QUERY->setname(QUERY, MACRO_STR(__CHUGIN_NAME__));
    
    QUERY->begin_class(QUERY, MACRO_STR(__CHUGIN_NAME__), "UGen");
    
    QUERY->add_ctor(QUERY, fauck_ctor);
    QUERY->add_dtor(QUERY, fauck_dtor);
    
    QUERY->add_ugen_func(QUERY, fauck_tick, NULL, temp.getNumInputs(), temp.getNumOutputs());
    
//    QUERY->add_mfun(QUERY, bitcrusher_setBits, "int", "bits");
//    QUERY->add_arg(QUERY, "int", "arg");
//    
//    QUERY->add_mfun(QUERY, bitcrusher_getBits, "int", "bits");
//    
//    QUERY->add_mfun(QUERY, bitcrusher_setDownsampleFactor, "int", "downsampleFactor");
//    QUERY->add_arg(QUERY, "int", "arg");
//    
//    QUERY->add_mfun(QUERY, bitcrusher_getDownsampleFactor, "int", "downsampleFactor");
    
    fauck_data_offset = QUERY->add_mvar(QUERY, "int", "@data", false);
    
    QUERY->end_class(QUERY);

    return TRUE;
}


CK_DLL_CTOR(fauck_ctor)
{
    OBJ_MEMBER_INT(SELF, fauck_data_offset) = 0;
    
    Fauck * fckdata = new Fauck(API->vm->get_srate());
    
    OBJ_MEMBER_INT(SELF, fauck_data_offset) = (t_CKINT) fckdata;
}

CK_DLL_DTOR(fauck_dtor)
{
    Fauck * fckdata = (Fauck *) OBJ_MEMBER_INT(SELF, fauck_data_offset);
    if(fckdata)
    {
        delete fckdata;
        OBJ_MEMBER_INT(SELF, fauck_data_offset) = 0;
        fckdata = NULL;
    }
}

CK_DLL_TICK(fauck_tick)
{
    Fauck * fckdata = (Fauck *) OBJ_MEMBER_INT(SELF, fauck_data_offset);
    
    FAUSTFLOAT fin = in;
    FAUSTFLOAT *finv = &fin;
    FAUSTFLOAT fout = 0;
    FAUSTFLOAT *foutv = &fout;
    
    fckdata->dsp.compute(1, &finv, &foutv);
    
    *out = fout;

    return TRUE;
}

//CK_DLL_MFUN(bitcrusher_setBits)
//{
//    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
//    // TODO: sanity check
//    bcdata->bits = GET_NEXT_INT(ARGS);
//    RETURN->v_int = bcdata->bits;
//}
//
//CK_DLL_MFUN(bitcrusher_getBits)
//{
//    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
//    RETURN->v_int = bcdata->bits;
//}
//
//CK_DLL_MFUN(bitcrusher_setDownsampleFactor)
//{
//    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
//    // TODO: sanity check
//    bcdata->downsampleFactor = GET_NEXT_INT(ARGS);
//    RETURN->v_int = bcdata->downsampleFactor;
//}
//
//CK_DLL_MFUN(bitcrusher_getDownsampleFactor)
//{
//    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
//    RETURN->v_int = bcdata->downsampleFactor;
//}
//
//
