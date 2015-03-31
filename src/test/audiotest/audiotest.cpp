/*----------------------------------------------------------------------------
 audiotest
 Digital audio comparison suite
 
 Copyright (c) 2015 Spencer Salazar.  All rights reserved.
 
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


#include <stdio.h>
#include <string.h>

#include "stk/OnePole.h"
#include "stk/FileWvIn.h"


using namespace stk;


#define CMD_NAME "audiotest"

/*** Parameters ***/

const float SAMPLE_THRESHOLD = 0.001;

const float RMS_WINDOW = 256;
const float RMS_HOP = RMS_WINDOW/2;
const float RMS_THRESHOLD = 0.001;


void usage()
{
    fprintf(stderr, "usage: " CMD_NAME " [-sample|-rms] file1 file2\n");
}

int argiseq(char *arg, const char *eqto)
{
    return (strncmp(arg, eqto, strlen(eqto)) == 0);
}


int compsample(FileWvIn &file1, FileWvIn &file2);
int comprms(FileWvIn &file1, FileWvIn &file2);

bool equal(const StkFrames &a, const StkFrames &b, float thresh = 0);
bool notequal(const StkFrames &a, const StkFrames &b, float thresh = 0);

enum CompType
{
    COMP_SAMPLE,
    COMP_RMS,
};


int main(int argc, char *argv[])
{
    CompType type;
    const char *file1 = NULL;
    const char *file2 = NULL;
    
    if(argc != 4)
    {
        usage();
        exit(1);
    }
    
    for(int i = 1; i < argc; i++)
    {
        if(argiseq(argv[i], "-sample"))
        {
            type = COMP_SAMPLE;
            file1 = argv[++i];
            file2 = argv[++i];
        }
        else if(argiseq(argv[i], "-rms"))
        {
            type = COMP_RMS;
            file1 = argv[++i];
            file2 = argv[++i];
        }
        else
        {
            usage();
            exit(1);
        }
    }
    
    if(!file1 || !file2)
    {
        fprintf(stderr, CMD_NAME ": error: need two filenames\n");
        exit(1);
    }
    
    FileWvIn wvfile1(INT_MAX, 2048), wvfile2(INT_MAX, 2048);
    
    try {
        wvfile1 = FileWvIn(file1, false, false, INT_MAX);
    }
    catch(StkError e)
    {
        fprintf(stderr, CMD_NAME ": error: opening file %s\n", file1);
        exit(1);
    }
    
    try {
        wvfile2 = FileWvIn(file2, false, false, INT_MAX);
    }
    catch(StkError e)
    {
        fprintf(stderr, CMD_NAME ": error: opening file %s\n", file1);
        exit(1);
    }
    
    switch(type)
    {
        case COMP_SAMPLE:
            return compsample(wvfile1, wvfile2);
        case COMP_RMS:
            return comprms(wvfile1, wvfile2);
    }
    
    return 1;
}


int compsample(FileWvIn &file1, FileWvIn &file2)
{
    if(file1.getSize() != file2.getSize())
        return 1;
    if(file1.getFileRate() != file2.getFileRate())
        return 1;
    if(file1.channelsOut() != file2.channelsOut())
        return 1;
    
    StkFrames frame1, frame2;
    
    int i = 0;
    while(!file1.isFinished() && !file2.isFinished())
    {
        try {
            file1.tick();
            file2.tick();
        } catch(StkError e) {
            fprintf(stderr, CMD_NAME ": error: ticking files: '%s' (frame == %i)\n", e.getMessageCString(), i);
            exit(1);
        }
        
        frame1 = file1.lastFrame();
        frame2 = file2.lastFrame();
        
        if(notequal(frame1,frame2, SAMPLE_THRESHOLD)) {
            return 1;
        }
        
        i++;
    }
    
    return 0;
}



int comprms(FileWvIn &file1, FileWvIn &file2)
{
    return 0;
}


bool equal(const StkFrames &a, const StkFrames &b, float thresh)
{
    if(a.channels() != b.channels()) return false;
    if(a.frames() != b.frames()) return false;
    
    int nf = a.frames();
    int nc = a.channels();
    for(int f = 0; f < nf; f++)
    {
        for(int c = 0; c < nc; c++)
            if(fabsf(a(f, c)-b(f,c)) > thresh) return false;
    }
    
    return true;
}

bool notequal(const StkFrames &a, const StkFrames &b, float thresh)
{
    return !equal(a,b,thresh);
}


