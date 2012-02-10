//-----------------------------------------------------
//
// Code generated with Faust 0.9.46 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */


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


CK_DLL_CTOR(fauck_ctor);
CK_DLL_DTOR(fauck_dtor);

//CK_DLL_MFUN(bitcrusher_setBits);
//CK_DLL_MFUN(bitcrusher_getBits);
//CK_DLL_MFUN(bitcrusher_setDownsampleFactor);
//CK_DLL_MFUN(bitcrusher_getDownsampleFactor);

CK_DLL_TICK(fauck_tick);

t_CKINT bitcrusher_data_offset = 0;




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

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

typedef long double quad;

#define FAUSTCLASS mydsp

class mydsp : public dsp {
  private:
	FAUSTFLOAT 	fslider0;
  public:
	static void metadata(Meta* m) 	{ 
	}

	virtual int getNumInputs() 	{ return 1; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fslider0 = 0.5f;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("panner");
		interface->addHorizontalSlider("pan", &fslider0, 0.5f, 0.0f, 1.0f, 0.01f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = fslider0;
		float 	fSlow1 = (1 - fSlow0);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			float fTemp0 = (float)input0[i];
			output0[i] = (FAUSTFLOAT)(fSlow1 * fTemp0);
			output1[i] = (FAUSTFLOAT)(fSlow0 * fTemp0);
		}
	}
};



/******************************************************************************
 *******************************************************************************
 
 CHUCK DSP INTERFACE
 
 *******************************************************************************
 *******************************************************************************/


struct FauckData
{
};


CK_DLL_QUERY(Bitcrusher)
{
    QUERY->setname(QUERY, "Bitcrusher");
    
    QUERY->begin_class(QUERY, "Bitcrusher", "UGen");
    
    QUERY->add_ctor(QUERY, bitcrusher_ctor);
    QUERY->add_dtor(QUERY, bitcrusher_dtor);
    
    QUERY->add_ugen_func(QUERY, bitcrusher_tick, NULL, 1, 1);
    
    QUERY->add_mfun(QUERY, bitcrusher_setBits, "int", "bits");
    QUERY->add_arg(QUERY, "int", "arg");
    
    QUERY->add_mfun(QUERY, bitcrusher_getBits, "int", "bits");
    
    QUERY->add_mfun(QUERY, bitcrusher_setDownsampleFactor, "int", "downsampleFactor");
    QUERY->add_arg(QUERY, "int", "arg");
    
    QUERY->add_mfun(QUERY, bitcrusher_getDownsampleFactor, "int", "downsampleFactor");
    
    bitcrusher_data_offset = QUERY->add_mvar(QUERY, "int", "@data", false);
    
    QUERY->end_class(QUERY);

    return TRUE;
}


CK_DLL_CTOR(bitcrusher_ctor)
{
    OBJ_MEMBER_INT(SELF, bitcrusher_data_offset) = 0;
    
    BitcrusherData * bcdata = new BitcrusherData;
    bcdata->bits = 32;
    bcdata->downsampleFactor = 1;
    bcdata->currentSampleCount = 0;
    bcdata->currentSample = 0;
    
    OBJ_MEMBER_INT(SELF, bitcrusher_data_offset) = (t_CKINT) bcdata;
}

CK_DLL_DTOR(bitcrusher_dtor)
{
    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
    if(bcdata)
    {
        delete bcdata;
        OBJ_MEMBER_INT(SELF, bitcrusher_data_offset) = 0;
        bcdata = NULL;
    }
}

CK_DLL_TICK(bitcrusher_tick)
{
    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
    
    SAMPLE theSample;

    if((bcdata->currentSampleCount % bcdata->downsampleFactor) == 0)
    {
        // clamp to [-1,1]
        if(in > 1) in = 1;
        else if(in < -1) in = -1;
        
        // sample and hold
        bcdata->currentSample = theSample = in;
    }
    else
    {
        // decimate!
        theSample = bcdata->currentSample;
    }
    
    bcdata->currentSampleCount = (bcdata->currentSampleCount+1) % bcdata->downsampleFactor;
    
    // convert to 32-bit int
    int shift = 32-bcdata->bits;
    int q32 = theSample * INT_MAX;
    q32 = (q32 >> shift) << shift;
    
    *out = q32 / ((float) INT_MAX);

    return TRUE;
}

CK_DLL_MFUN(bitcrusher_setBits)
{
    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
    // TODO: sanity check
    bcdata->bits = GET_NEXT_INT(ARGS);
    RETURN->v_int = bcdata->bits;
}

CK_DLL_MFUN(bitcrusher_getBits)
{
    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
    RETURN->v_int = bcdata->bits;
}

CK_DLL_MFUN(bitcrusher_setDownsampleFactor)
{
    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
    // TODO: sanity check
    bcdata->downsampleFactor = GET_NEXT_INT(ARGS);
    RETURN->v_int = bcdata->downsampleFactor;
}

CK_DLL_MFUN(bitcrusher_getDownsampleFactor)
{
    BitcrusherData * bcdata = (BitcrusherData *) OBJ_MEMBER_INT(SELF, bitcrusher_data_offset);
    RETURN->v_int = bcdata->downsampleFactor;
}


