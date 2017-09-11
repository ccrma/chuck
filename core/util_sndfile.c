/*----------------------------------------------------------------------------
    ChucK Concurrent, On-the-fly Audio Programming Language
      Compiler and Virtual Machine

    Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

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

/*
** libsndfile Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//-----------------------------------------------------------------------------
// name: util_sndfile.c
// desc: libsndfile for ChucK
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ari Lazier (alazier@alumni.princeton.edu)
// libsndfile: Erik de Castro Lopo (erikd@mega-nerd.com)
//-----------------------------------------------------------------------------
#include "util_sndfile.h"
#include "chuck_def.h"
#include <math.h>

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*
 *  See private.h for the more commonly used macro versions.
 */

#include	<stdio.h>
#include	<assert.h>


#define	saturate(x) 	\
	((x) < MIN_WORD ? MIN_WORD : (x) > MAX_WORD ? MAX_WORD: (x))

word gsm_add ( word a, word b)
{
	longword sum = (longword)a + (longword)b;
	return saturate(sum);
}

word gsm_sub ( word a, word b)
{
	longword diff = (longword)a - (longword)b;
	return saturate(diff);
}

word gsm_mult ( word a, word b)
{
	if (a == MIN_WORD && b == MIN_WORD)
		return MAX_WORD;
	
	return SASR_L( (longword)a * (longword)b, 15 );
}

word gsm_mult_r ( word a, word b)
{
	if (b == MIN_WORD && a == MIN_WORD) return MAX_WORD;
	else {
		longword prod = (longword)a * (longword)b + 16384;
		prod >>= 15;
		return prod & 0xFFFF;
	}
}

word gsm_abs (word a)
{
	return a < 0 ? (a == MIN_WORD ? MAX_WORD : -a) : a;
}

longword gsm_L_mult (word a, word b)
{
	assert( a != MIN_WORD || b != MIN_WORD );
	return ((longword)a * (longword)b) << 1;
}

longword gsm_L_add ( longword a, longword b)
{
	if (a < 0) {
		if (b >= 0) return a + b;
		else {
			ulongword A = (ulongword)-(a + 1) + (ulongword)-(b + 1);
			return A >= MAX_LONGWORD ? MIN_LONGWORD :-(longword)A-2;
		}
	}
	else if (b <= 0) return a + b;
	else {
		ulongword A = (ulongword)a + (ulongword)b;
		return A > MAX_LONGWORD ? MAX_LONGWORD : A;
	}
}

longword gsm_L_sub ( longword a, longword b)
{
	if (a >= 0) {
		if (b >= 0) return a - b;
		else {
			/* a>=0, b<0 */

			ulongword A = (ulongword)a + -(b + 1);
			return A >= MAX_LONGWORD ? MAX_LONGWORD : (A + 1);
		}
	}
	else if (b <= 0) return a - b;
	else {
		/* a<0, b>0 */  

		ulongword A = (ulongword)-(a + 1) + b;
		return A >= MAX_LONGWORD ? MIN_LONGWORD : -(longword)A - 1;
	}
}

static unsigned char const bitoff[ 256 ] = {
	 8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

word gsm_norm (longword a )
/*
 * the number of left shifts needed to normalize the 32 bit
 * variable L_var1 for positive values on the interval
 *
 * with minimum of
 * minimum of 1073741824  (01000000000000000000000000000000) and 
 * maximum of 2147483647  (01111111111111111111111111111111)
 *
 *
 * and for negative values on the interval with
 * minimum of -2147483648 (-10000000000000000000000000000000) and
 * maximum of -1073741824 ( -1000000000000000000000000000000).
 *
 * in order to normalize the result, the following
 * operation must be done: L_norm_var1 = L_var1 << norm( L_var1 );
 *
 * (That's 'ffs', only from the left, not the right..)
 */
{
	assert(a != 0);

	if (a < 0) {
		if (a <= -1073741824) return 0;
		a = ~a;
	}

	return    a & 0xffff0000 
		? ( a & 0xff000000
		  ?  -1 + bitoff[ 0xFF & (a >> 24) ]
		  :   7 + bitoff[ 0xFF & (a >> 16) ] )
		: ( a & 0xff00
		  ?  15 + bitoff[ 0xFF & (a >> 8) ]
		  :  23 + bitoff[ 0xFF & a ] );
}

longword gsm_L_asl (longword a, int n)
{
	if (n >= 32) return 0;
	if (n <= -32) return -(a < 0);
	if (n < 0) return gsm_L_asr(a, -n);
	return a << n;
}

word gsm_asr (word a, int n)
{
	if (n >= 16) return -(a < 0);
	if (n <= -16) return 0;
	if (n < 0) return a << -n;

	return SASR_W (a, (word) n);
}

word gsm_asl (word a, int n)
{
	if (n >= 16) return 0;
	if (n <= -16) return -(a < 0);
	if (n < 0) return gsm_asr(a, -n);
	return a << n;
}

longword gsm_L_asr (longword a, int n)
{
	if (n >= 32) return -(a < 0);
	if (n <= -32) return 0;
	if (n < 0) return a << -n;

	return SASR_L (a, (word) n);
}

/*
**	word gsm_asr (word a, int n)
**	{
**		if (n >= 16) return -(a < 0);
**		if (n <= -16) return 0;
**		if (n < 0) return a << -n;
**	
**	#	ifdef	SASR_W
**			return a >> n;
**	#	else
**			if (a >= 0) return a >> n;
**			else return -(word)( -(uword)a >> n );
**	#	endif
**	}
**	
*/
/* 
 *  (From p. 46, end of section 4.2.5)
 *
 *  NOTE: The following lines gives [sic] one correct implementation
 *  	  of the div(num, denum) arithmetic operation.  Compute div
 *        which is the integer division of num by denum: with denum
 *	  >= num > 0
 */

word gsm_div (word num, word denum)
{
	longword	L_num   = num;
	longword	L_denum = denum;
	word		div 	= 0;
	int		k 	= 15;

	/* The parameter num sometimes becomes zero.
	 * Although this is explicitly guarded against in 4.2.5,
	 * we assume that the result should then be zero as well.
	 */

	/* assert(num != 0); */

	assert(num >= 0 && denum >= num);
	if (num == 0)
	    return 0;

	while (k--) {
		div   <<= 1;
		L_num <<= 1;

		if (L_num >= L_denum) {
			L_num -= L_denum;
			div++;
		}
	}

	return div;
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: a7398579-e2e1-4733-aa2d-4c6efc0c58ff
*/

/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


/*------------------------------------------------------------------------------
 * Macros to handle big/little endian issues.
 */

#define FORM_MARKER	(MAKE_MARKER ('F', 'O', 'R', 'M'))
#define AIFF_MARKER	(MAKE_MARKER ('A', 'I', 'F', 'F'))
#define AIFC_MARKER	(MAKE_MARKER ('A', 'I', 'F', 'C'))
#define COMM_MARKER	(MAKE_MARKER ('C', 'O', 'M', 'M'))
#define SSND_MARKER	(MAKE_MARKER ('S', 'S', 'N', 'D'))
#define MARK_MARKER	(MAKE_MARKER ('M', 'A', 'R', 'K'))
#define INST_MARKER	(MAKE_MARKER ('I', 'N', 'S', 'T'))
#define APPL_MARKER	(MAKE_MARKER ('A', 'P', 'P', 'L'))

#define c_MARKER	(MAKE_MARKER ('(', 'c', ')', ' '))
#define NAME_MARKER	(MAKE_MARKER ('N', 'A', 'M', 'E'))
#define AUTH_MARKER	(MAKE_MARKER ('A', 'U', 'T', 'H'))
#define ANNO_MARKER	(MAKE_MARKER ('A', 'N', 'N', 'O'))
#define COMT_MARKER	(MAKE_MARKER ('C', 'O', 'M', 'T'))
#define FVER_MARKER	(MAKE_MARKER ('F', 'V', 'E', 'R'))
#define SFX_MARKER	(MAKE_MARKER ('S', 'F', 'X', '!'))

#define PEAK_MARKER	(MAKE_MARKER ('P', 'E', 'A', 'K'))

/* Supported AIFC encodings.*/
#define NONE_MARKER	(MAKE_MARKER ('N', 'O', 'N', 'E'))
#define sowt_MARKER	(MAKE_MARKER ('s', 'o', 'w', 't'))
#define twos_MARKER	(MAKE_MARKER ('t', 'w', 'o', 's'))
#define raw_MARKER	(MAKE_MARKER ('r', 'a', 'w', ' '))
#define in32_MARKER	(MAKE_MARKER ('i', 'n', '3', '2'))
#define ni32_MARKER	(MAKE_MARKER ('2', '3', 'n', 'i'))

#define fl32_MARKER	(MAKE_MARKER ('f', 'l', '3', '2'))
#define FL32_MARKER	(MAKE_MARKER ('F', 'L', '3', '2'))
#define fl64_MARKER	(MAKE_MARKER ('f', 'l', '6', '4'))
#define FL64_MARKER	(MAKE_MARKER ('F', 'L', '6', '4'))

#define ulaw_MARKER	(MAKE_MARKER ('u', 'l', 'a', 'w'))
#define ULAW_MARKER	(MAKE_MARKER ('U', 'L', 'A', 'W'))
#define alaw_MARKER	(MAKE_MARKER ('a', 'l', 'a', 'w'))
#define ALAW_MARKER	(MAKE_MARKER ('A', 'L', 'A', 'W'))

#define DWVW_MARKER	(MAKE_MARKER ('D', 'W', 'V', 'W'))
#define GSM_MARKER	(MAKE_MARKER ('G', 'S', 'M', ' '))
#define ima4_MARKER	(MAKE_MARKER ('i', 'm', 'a', '4'))

/* Unsupported AIFC encodings.*/

#define MAC3_MARKER	(MAKE_MARKER ('M', 'A', 'C', '3'))
#define MAC6_MARKER	(MAKE_MARKER ('M', 'A', 'C', '6'))
#define ADP4_MARKER	(MAKE_MARKER ('A', 'D', 'P', '4'))

/* Predfined chunk sizes. */
#define SIZEOF_AIFF_COMM		18
#define SIZEOF_AIFC_COMM_MIN	22
#define SIZEOF_AIFC_COMM		24
#define SIZEOF_SSND_CHUNK		8
#define SIZEOF_INST_CHUNK		20

/* AIFC/IMA4 defines. */
#define AIFC_IMA4_BLOCK_LEN				34
#define	AIFC_IMA4_SAMPLES_PER_BLOCK		64

/*------------------------------------------------------------------------------
 * Typedefs for file chunks.
 */

enum
{	aiffHAVE_FORM	= 0x01,
	HAVE_AIFF	= 0x02,
	HAVE_COMM	= 0x04,
	HAVE_SSND	= 0x08
} ;

typedef struct
{	unsigned int	size ;
	short			numChannels ;
	unsigned int	numSampleFrames ;
	short			sampleSize ;
	unsigned char	sampleRate [10] ;
	unsigned int	encoding ;
	char			zero_bytes [2] ;
} COMM_CHUNK ;

typedef struct
{	unsigned int	offset ;
	unsigned int	blocksize ;
} SSND_CHUNK ;

typedef struct
{	short			playMode ;
    unsigned short	beginLoop ;
	unsigned short	endLoop ;
} INST_LOOP ;

typedef struct
{	char		baseNote ;		/* all notes are MIDI note numbers */
	char		detune ;		/* cents off, only -50 to +50 are significant */
	char		lowNote ;
	char		highNote ;
	char		lowVelocity ;	/* 1 to 127 */
	char		highVelocity ;	/* 1 to 127 */
	short		gain ;			/* in dB, 0 is normal */
	INST_LOOP	sustain_loop ;
	INST_LOOP	release_loop ;
} INST_CHUNK ;

/*------------------------------------------------------------------------------
 * Private static functions.
 */

static int	aiff_close	(SF_PRIVATE *psf) ;

static int	tenbytefloat2int (unsigned char *bytes) ;
static void	uint2tenbytefloat (unsigned int num, unsigned char *bytes) ;

static int	aiff_read_comm_chunk (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt) ;

static int	aiff_read_header (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt) ;

static int	aiff_write_header (SF_PRIVATE *psf, int calc_length) ;
static int	aiff_write_tailer (SF_PRIVATE *psf) ;
static void aiff_write_strings (SF_PRIVATE *psf, int location) ;

static int	aiff_command (SF_PRIVATE *psf, int command, void *data, int datasize) ;

static const char *get_loop_mode_str (short mode) ;


/*------------------------------------------------------------------------------
** Public function.
*/

int
aiff_open	(SF_PRIVATE *psf)
{	COMM_CHUNK comm_fmt ;
	int error, subformat ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = aiff_read_header (psf, &comm_fmt)))
			return error ;

		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_AIFF)
			return	SFE_BAD_OPEN_FORMAT ;

		if (psf->mode == SFM_WRITE && (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE))
		{	psf->pchunk = calloc (1, sizeof (PEAK_CHUNK) * psf->sf.channels * sizeof (PEAK_POS)) ;
			if (psf->pchunk == NULL)
				return SFE_MALLOC_FAILED ;
			psf->has_peak = SF_TRUE ;
			psf->peak_loc = SF_PEAK_START ;
			} ;

		if (psf->mode != SFM_RDWR || psf->filelength < 40)
		{	psf->filelength = 0 ;
			psf->datalength = 0 ;
			psf->dataoffset = 0 ;
			psf->sf.frames = 0 ;
			} ;

		psf->str_flags = SF_STR_ALLOW_START | SF_STR_ALLOW_END ;

		if ((error = aiff_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = aiff_write_header ;
		} ;

	psf->close = aiff_close ;
	psf->command = aiff_command ;

	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_U8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_S8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_ULAW :
				error = ulaw_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :
				error = alaw_init (psf) ;
				break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :
				error = double64_init (psf) ;
				break ;

		case SF_FORMAT_DWVW_12 :
				error = dwvw_init (psf, 12) ;
				break ;

		case SF_FORMAT_DWVW_16 :
				error = dwvw_init (psf, 16) ;
				break ;

		case SF_FORMAT_DWVW_24 :
				error = dwvw_init (psf, 24) ;
				break ;

		case SF_FORMAT_DWVW_N :
				if (psf->mode != SFM_READ)
				{	error = SFE_DWVW_BAD_BITWIDTH ;
					break ;
					} ;
				if (comm_fmt.sampleSize >= 8 && comm_fmt.sampleSize < 24)
				{	error = dwvw_init (psf, comm_fmt.sampleSize) ;
					psf->sf.frames = comm_fmt.numSampleFrames ;
					break ;
					} ;
				psf_log_printf (psf, "AIFC/DWVW : Bad bitwidth %d\n", comm_fmt.sampleSize) ;
				error = SFE_DWVW_BAD_BITWIDTH ;
				break ;

		case SF_FORMAT_IMA_ADPCM :
				/*
				**	IMA ADPCM encoded AIFF files always have a block length
				**	of 34 which decodes to 64 samples.
				*/
				error = aiff_ima_init (psf, AIFC_IMA4_BLOCK_LEN, AIFC_IMA4_SAMPLES_PER_BLOCK) ;
				break ;
		/* Lite remove end */

		case SF_FORMAT_GSM610 :
				error = gsm610_init (psf) ;
				break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	if (psf->mode == SFM_READ)
		psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	return error ;
} /* aiff_open */

/*==========================================================================================
** Private functions.
*/

static int
aiff_read_header (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt)
{	SSND_CHUNK	ssnd_fmt ;
	int			marker, dword, bytesread, k ;
	int			FORMsize, SSNDsize ;
	int			filetype, found_chunk = 0, done = 0, error = 0 ;
	char		*cptr, byte ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "p", 0) ;

	memset (comm_fmt, 0, sizeof (COMM_CHUNK)) ;

	/* Until recently AIF* file were all BIG endian. */
	psf->endian = SF_ENDIAN_BIG ;

	/* 	AIFF files can apparently have their chunks in any order. However, they
	**	must have a FORM chunk. Approach here is to read all the chunks one by
	**  one and then check for the mandatory chunks at the end.
	*/
	while (! done)
	{	psf_binheader_readf (psf, "m", &marker) ;

		if (psf->mode == SFM_RDWR && (found_chunk & HAVE_SSND))
			return SFE_AIFF_RW_SSND_NOT_LAST ;

		switch (marker)
		{	case FORM_MARKER :
					if (found_chunk)
						return SFE_AIFF_NO_FORM ;

					psf_binheader_readf (psf, "E4", &FORMsize) ;

					if (psf->fileoffset > 0 && psf->filelength > FORMsize + 8)
					{	/* Set file length. */
						psf->filelength = FORMsize + 8 ;
						psf_log_printf (psf, "FORM : %u\n", FORMsize) ;
						}
					else if (FORMsize != psf->filelength - 2 * SIGNED_SIZEOF (dword))
					{	dword = psf->filelength - 2 * sizeof (dword) ;
						psf_log_printf (psf, "FORM : %u (should be %u)\n", FORMsize, dword) ;
						FORMsize = dword ;
						}
					else
						psf_log_printf (psf, "FORM : %u\n", FORMsize) ;
					found_chunk |= aiffHAVE_FORM ;
					break ;

			case AIFC_MARKER :
			case AIFF_MARKER :
					if (! (found_chunk & aiffHAVE_FORM))
						return SFE_AIFF_AIFF_NO_FORM ;
					filetype = marker ;
					psf_log_printf (psf, " %M\n", marker) ;
					found_chunk |= HAVE_AIFF ;
					break ;

			case COMM_MARKER :
					error = aiff_read_comm_chunk (psf, comm_fmt) ;

					psf->sf.samplerate 	= tenbytefloat2int (comm_fmt->sampleRate) ;
					psf->sf.frames 	= comm_fmt->numSampleFrames ;
					psf->sf.channels 	= comm_fmt->numChannels ;
					psf->bytewidth 		= BITWIDTH2BYTES (comm_fmt->sampleSize) ;

					if (error)
						return error ;

					found_chunk |= HAVE_COMM ;
					break ;

			case PEAK_MARKER :
					/* Must have COMM chunk before PEAK chunk. */
					if ((found_chunk & (aiffHAVE_FORM | HAVE_AIFF | HAVE_COMM)) != (aiffHAVE_FORM | HAVE_AIFF | HAVE_COMM))
						return SFE_AIFF_PEAK_B4_COMM ;

					psf_binheader_readf (psf, "E4", &dword) ;

					psf_log_printf (psf, "%M : %d\n", marker, dword) ;
					if (dword != SIGNED_SIZEOF (PEAK_CHUNK) + psf->sf.channels * SIGNED_SIZEOF (PEAK_POS))
					{	psf_binheader_readf (psf, "j", dword) ;
						psf_log_printf (psf, "*** File PEAK chunk bigger than sizeof (PEAK_CHUNK).\n") ;
						return SFE_WAV_BAD_PEAK ;
						} ;

					psf->pchunk = calloc (1, sizeof (PEAK_CHUNK) * psf->sf.channels * sizeof (PEAK_POS)) ;
					if (psf->pchunk == NULL)
						return SFE_MALLOC_FAILED ;

					/* read in rest of PEAK chunk. */
					psf_binheader_readf (psf, "E44", &(psf->pchunk->version), &(psf->pchunk->timestamp)) ;

					if (psf->pchunk->version != 1)
						psf_log_printf (psf, "  version    : %d *** (should be version 1)\n", psf->pchunk->version) ;
					else
						psf_log_printf (psf, "  version    : %d\n", psf->pchunk->version) ;

					psf_log_printf (psf, "  time stamp : %d\n", psf->pchunk->timestamp) ;
					psf_log_printf (psf, "    Ch   Position       Value\n") ;

					cptr = (char *) psf->buffer ;
					for (dword = 0 ; dword < psf->sf.channels ; dword++)
					{	psf_binheader_readf (psf, "Ef4", &(psf->pchunk->peaks [dword].value),
														&(psf->pchunk->peaks [dword].position)) ;

						LSF_SNPRINTF (cptr, sizeof (psf->buffer), "    %2d   %-12d   %g\n",
								dword, psf->pchunk->peaks [dword].position, psf->pchunk->peaks [dword].value) ;
						cptr [sizeof (psf->buffer) - 1] = 0 ;
						psf_log_printf (psf, cptr) ;
						} ;

					psf->has_peak = SF_TRUE ; /* Found PEAK chunk. */
					break ;

			case SSND_MARKER :
					psf_binheader_readf (psf, "E444", &SSNDsize, &(ssnd_fmt.offset), &(ssnd_fmt.blocksize)) ;

					psf->datalength = SSNDsize - sizeof (ssnd_fmt) ;
					psf->dataoffset = psf_ftell (psf) ;

					if (psf->datalength > psf->filelength - psf->dataoffset || psf->datalength < 0)
					{	psf_log_printf (psf, " SSND : %u (should be %D)\n", SSNDsize, psf->filelength - psf->dataoffset + sizeof (SSND_CHUNK)) ;
						psf->datalength = psf->filelength - psf->dataoffset ;
						}
					else
						psf_log_printf (psf, " SSND : %u\n", SSNDsize) ;

					/* Only set dataend if there really is data at the end. */
					if (psf->datalength + psf->dataoffset < psf->filelength)
						psf->dataend = psf->datalength + psf->dataoffset ;

					psf_log_printf (psf, "  Offset     : %u\n", ssnd_fmt.offset) ;
					psf_log_printf (psf, "  Block Size : %u\n", ssnd_fmt.blocksize) ;

					found_chunk |= HAVE_SSND ;

					if (! psf->sf.seekable)
						break ;

					/* 	Seek to end of SSND chunk. */
					psf_fseek (psf, psf->dataoffset + psf->datalength + (SSNDsize & 1), SEEK_SET) ;
					break ;

			case c_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_COPYRIGHT, cptr) ;
					break ;

			case AUTH_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_ARTIST, cptr) ;
					break ;

			case COMT_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_COMMENT, cptr) ;
					break ;

			case APPL_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword >= SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big, skipping)\n", marker, dword) ;
						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;

					for (k = 0 ; k < dword ; k++)
						if (! isprint (cptr [k]))
						{	cptr [k] = 0 ;
							break ;
							} ;

					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_SOFTWARE, cptr) ;
					break ;

			case NAME_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_TITLE, cptr) ;
					break ;

			case ANNO_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					break ;

			case INST_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					if (dword != SIZEOF_INST_CHUNK)
					{	psf_log_printf (psf, " %M : %d (should be %d)\n", marker, dword, SIZEOF_INST_CHUNK) ;
						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;
					psf_log_printf (psf, " %M : %d\n", marker, dword) ;
					{	unsigned char bytes [6] ;
						short gain ;

						psf_binheader_readf (psf, "b", bytes, 6) ;
						psf_log_printf (psf, 	"  Base Note : %u\n  Detune    : %u\n"
												"  Low  Note : %u\n  High Note : %u\n"
												"  Low  Vel. : %u\n  High Vel. : %u\n",
								bytes [0], bytes [1], bytes [2], bytes [3], bytes [4], bytes [5]) ;

						psf_binheader_readf (psf, "E2", &gain) ;
						psf_log_printf (psf, 	"  Gain (dB) : %d\n", gain) ;

						} ;
					{	short	mode ; /* 0 - no loop, 1 - forward looping, 2 - backward looping */
						const char	*loop_mode ;
						unsigned short begin, end ;

						psf_binheader_readf (psf, "E222", &mode, &begin, &end) ;
						loop_mode = get_loop_mode_str (mode) ;
						psf_log_printf (psf, 	"  Sustain\n   mode  : %d => %s\n   begin : %u\n   end   : %u\n",
								mode, loop_mode, begin, end) ;
						psf_binheader_readf (psf, "E222", &mode, &begin, &end) ;
						loop_mode = get_loop_mode_str (mode) ;
						psf_log_printf (psf, 	"  Release\n   mode  : %d => %s\n   begin : %u\n   end   : %u\n",
								mode, loop_mode, begin, end) ;
						} ;
					break ;

			case MARK_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					psf_log_printf (psf, " %M : %d\n", marker, dword) ;
					{	unsigned short	mark_count, mark_id ;
						unsigned char	pstr_len ;
						unsigned int	position ;

						bytesread = psf_binheader_readf (psf, "E2", &mark_count) ;
						psf_log_printf (psf, "  Count : %d\n", mark_count) ;

						while (mark_count && bytesread < dword)
						{	bytesread += psf_binheader_readf (psf, "E241", &mark_id, &position, &pstr_len) ;
							psf_log_printf (psf, "   Mark ID  : %u\n   Position : %u\n", mark_id, position) ;

							pstr_len += (pstr_len & 1) + 1 ; /* fudgy, fudgy, hack, hack */

							bytesread += psf_binheader_readf (psf, "b", psf->buffer, pstr_len) ;
							psf_log_printf (psf, "   Name     : %s\n", psf->buffer) ;

							mark_count -- ;
							} ;
						} ;
					psf_binheader_readf (psf, "j", dword - bytesread) ;
					break ;

			case FVER_MARKER :
			case SFX_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					psf_log_printf (psf, " %M : %d\n", marker, dword) ;

					psf_binheader_readf (psf, "j", dword) ;
					break ;

			case NONE_MARKER :
					/* Fix for broken AIFC files with incorrect COMM chunk length. */
					psf_binheader_readf (psf, "1", &byte) ;
					dword = byte ;
					psf_binheader_readf (psf, "j", dword) ;
					break ;

			default :
					if (isprint ((marker >> 24) & 0xFF) && isprint ((marker >> 16) & 0xFF)
						&& isprint ((marker >> 8) & 0xFF) && isprint (marker & 0xFF))
					{	psf_binheader_readf (psf, "E4", &dword) ;
						psf_log_printf (psf, "%M : %d (unknown marker)\n", marker, dword) ;

						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;
					if ((dword = psf_ftell (psf)) & 0x03)
					{	psf_log_printf (psf, "  Unknown chunk marker %X at position %d. Resyncing.\n", marker, dword - 4) ;

						psf_binheader_readf (psf, "j", -3) ;
						break ;
						} ;
					psf_log_printf (psf, "*** Unknown chunk marker %X at position %D. Exiting parser.\n", marker, psf_ftell (psf)) ;
					done = 1 ;
					break ;
			} ;	/* switch (marker) */

		if ((! psf->sf.seekable) && (found_chunk & HAVE_SSND))
			break ;

		if (psf_ftell (psf) >= psf->filelength - (2 * SIGNED_SIZEOF (dword)))
			break ;

		if (psf->logindex >= SIGNED_SIZEOF (psf->logbuffer) - 2)
			return SFE_LOG_OVERRUN ;
		} ; /* while (1) */

	if (! (found_chunk & aiffHAVE_FORM))
		return SFE_AIFF_NO_FORM ;

	if (! (found_chunk & HAVE_AIFF))
		return SFE_AIFF_COMM_NO_FORM ;

	if (! (found_chunk & HAVE_COMM))
		return SFE_AIFF_SSND_NO_COMM ;

	if (! psf->dataoffset)
		return SFE_AIFF_NO_DATA ;

	return 0 ;
} /* aiff_read_header */

static int
aiff_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	aiff_write_tailer (psf) ;

		aiff_write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* aiff_close */

static int
aiff_read_comm_chunk (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt)
{	int error = 0, bytesread, subformat ;

	bytesread = psf_binheader_readf (psf, "E4", &(comm_fmt->size)) ;

	/* The COMM chunk has an int aligned to an odd word boundary. Some
	** procesors are not able to deal with this (ie bus fault) so we have
	** to take special care.
	*/
	comm_fmt->size += comm_fmt->size & 1 ;

	bytesread +=
	psf_binheader_readf (psf, "E242b", &(comm_fmt->numChannels), &(comm_fmt->numSampleFrames),
			&(comm_fmt->sampleSize), &(comm_fmt->sampleRate), SIGNED_SIZEOF (comm_fmt->sampleRate)) ;

	if (comm_fmt->size == SIZEOF_AIFF_COMM)
		comm_fmt->encoding = NONE_MARKER ;
	else if (comm_fmt->size == SIZEOF_AIFC_COMM_MIN)
		bytesread += psf_binheader_readf (psf, "Em", &(comm_fmt->encoding)) ;
	else if (comm_fmt->size >= SIZEOF_AIFC_COMM)
	{	unsigned char encoding_len ;

		bytesread += psf_binheader_readf (psf, "Em1", &(comm_fmt->encoding), &encoding_len) ;

		memset (psf->buffer, 0, comm_fmt->size) ;

		bytesread += psf_binheader_readf (psf, "b", psf->buffer,
							comm_fmt->size - SIZEOF_AIFC_COMM + 1) ;
		psf->buffer [encoding_len] = 0 ;
		} ;

	psf_log_printf (psf, " COMM : %d\n", comm_fmt->size) ;
	psf_log_printf (psf, "  Sample Rate : %d\n", tenbytefloat2int (comm_fmt->sampleRate)) ;
	psf_log_printf (psf, "  Frames      : %u%s\n", comm_fmt->numSampleFrames, (comm_fmt->numSampleFrames == 0 && psf->filelength > 100) ? " (Should not be 0)" : "") ;
	psf_log_printf (psf, "  Channels    : %d\n", comm_fmt->numChannels) ;

	/*	Found some broken 'fl32' files with comm.samplesize == 16. Fix it here. */

	if ((comm_fmt->encoding == fl32_MARKER || comm_fmt->encoding == FL32_MARKER) && comm_fmt->sampleSize != 32)
	{	psf_log_printf (psf, "  Sample Size : %d (should be 32)\n", comm_fmt->sampleSize) ;
		comm_fmt->sampleSize = 32 ;
		}
	else if ((comm_fmt->encoding == fl64_MARKER || comm_fmt->encoding == FL64_MARKER) && comm_fmt->sampleSize != 64)
	{	psf_log_printf (psf, "  Sample Size : %d (should be 64)\n", comm_fmt->sampleSize) ;
		comm_fmt->sampleSize = 64 ;
		}
	else
		psf_log_printf (psf, "  Sample Size : %d\n", comm_fmt->sampleSize) ;

	subformat = s_bitwidth_to_subformat (comm_fmt->sampleSize) ;

	psf->endian = SF_ENDIAN_BIG ;

	switch (comm_fmt->encoding)
	{	case NONE_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | subformat) ;
				break ;

		case twos_MARKER :
		case in32_MARKER :
				psf->sf.format = (SF_ENDIAN_BIG | SF_FORMAT_AIFF | subformat) ;
				break ;

		case sowt_MARKER :
		case ni32_MARKER :
				psf->endian = SF_ENDIAN_LITTLE ;
				psf->sf.format = (SF_ENDIAN_LITTLE | SF_FORMAT_AIFF | subformat) ;
				break ;

		case fl32_MARKER :
		case FL32_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_FLOAT) ;
				break ;

		case ulaw_MARKER :
		case ULAW_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_ULAW) ;
				break ;

		case alaw_MARKER :
		case ALAW_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_ALAW) ;
				break ;

		case fl64_MARKER :
		case FL64_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_DOUBLE) ;
				break ;

		case raw_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_PCM_U8) ;
				break ;

		case DWVW_MARKER :
				psf->sf.format = SF_FORMAT_AIFF ;
				switch (comm_fmt->sampleSize)
				{	case 12 :
						psf->sf.format |= SF_FORMAT_DWVW_12 ;
						break ;
					case 16 :
						psf->sf.format |= SF_FORMAT_DWVW_16 ;
						break ;
					case 24 :
						psf->sf.format |= SF_FORMAT_DWVW_24 ;
						break ;

					default :
						psf->sf.format |= SF_FORMAT_DWVW_N ;
						break ;
					} ;
				break ;

		case GSM_MARKER :
				psf->sf.format = SF_FORMAT_AIFF ;
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_GSM610) ;
				break ;


		case ima4_MARKER :
				psf->endian = SF_ENDIAN_BIG ;
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_IMA_ADPCM) ;
				break ;

		default :
			psf_log_printf (psf, "AIFC : Unimplemented format : %M\n", comm_fmt->encoding) ;
			error = SFE_UNIMPLEMENTED ;
		} ;

	if (! psf->buffer [0])
		psf_log_printf (psf, "  Encoding    : %M\n", comm_fmt->encoding) ;
	else
		psf_log_printf (psf, "  Encoding    : %M => %s\n", comm_fmt->encoding, (char*) psf->buffer) ;

	return error ;
} /* aiff_read_comm_chunk */

static int
aiff_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t		current ;
	unsigned char 	comm_sample_rate [10], comm_zero_bytes [2] = { 0, 0 } ;
	unsigned int 	comm_type, comm_size, comm_encoding, comm_frames ;
	int				k, endian ;
	short			bit_width ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		if (psf->bytewidth > 0)
			psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	if (psf->mode == SFM_RDWR && psf->dataoffset > 0)
	{	/* Assuming here that the header has already been written and just
		** needs to be corrected for new data length. That means that we
		** only change the length fields of the FORM and SSND chunks;
		** everything else can be skipped over.
		*/

		/* First write new FORM chunk. */
		psf->headindex = 0 ;
		psf_fseek (psf, 0, SEEK_SET) ;

		psf_binheader_writef (psf, "Etm8", FORM_MARKER, psf->filelength - 8) ;
		psf_fwrite (psf->header, psf->headindex, 1, psf) ;

		/* Now write frame count field of COMM chunk header. */
		psf->headindex = 0 ;
		psf_fseek (psf, 22, SEEK_SET) ;

		psf_binheader_writef (psf, "Et8", psf->sf.frames) ;
		psf_fwrite (psf->header, psf->headindex, 1, psf) ;

		/* Now write new SSND chunk header. */
		psf->headindex = 0 ;
		psf_fseek (psf, psf->dataoffset - 16, SEEK_SET) ;

		psf_binheader_writef (psf, "Etm8", SSND_MARKER, psf->datalength + SIZEOF_SSND_CHUNK) ;
		psf_fwrite (psf->header, psf->headindex, 1, psf) ;

		if (current < psf->dataoffset)
			psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		else if (current > 0)
			psf_fseek (psf, current, SEEK_SET) ;

		return 0 ;
		} ;

	endian = psf->sf.format & SF_FORMAT_ENDMASK ;
	if (CPU_IS_LITTLE_ENDIAN && endian == SF_ENDIAN_CPU)
		endian = SF_ENDIAN_LITTLE ;

	/* Standard value here. */
	bit_width = psf->bytewidth * 8 ;
	comm_frames = (psf->sf.frames > 0xFFFFFFFF) ? 0xFFFFFFFF : psf->sf.frames ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				switch (endian)
				{	case SF_ENDIAN_BIG :
							psf->endian = SF_ENDIAN_BIG ;
							comm_type = AIFC_MARKER ;
							comm_size = SIZEOF_AIFC_COMM ;
							comm_encoding = twos_MARKER ;
							break ;

					case SF_ENDIAN_LITTLE :
							psf->endian = SF_ENDIAN_LITTLE ;
							comm_type = AIFC_MARKER ;
							comm_size = SIZEOF_AIFC_COMM ;
							comm_encoding = sowt_MARKER ;
							break ;

					default : /* SF_ENDIAN_FILE */
							psf->endian = SF_ENDIAN_BIG ;
							comm_type = AIFF_MARKER ;
							comm_size = SIZEOF_AIFF_COMM ;
							comm_encoding = 0 ;
							break ;
					} ;
				break ;

		case SF_FORMAT_FLOAT :					/* Big endian floating point. */
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = FL32_MARKER ;	/* Use 'FL32' because its easier to read. */
				break ;

		case SF_FORMAT_DOUBLE :					/* Big endian double precision floating point. */
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = FL64_MARKER ;	/* Use 'FL64' because its easier to read. */
				break ;

		case SF_FORMAT_ULAW :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = ulaw_MARKER ;
				break ;

		case SF_FORMAT_ALAW :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = alaw_MARKER ;
				break ;

		case SF_FORMAT_PCM_U8 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = raw_MARKER ;
				break ;

		case SF_FORMAT_DWVW_12 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 12 ;
				break ;

		case SF_FORMAT_DWVW_16 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				break ;

		case SF_FORMAT_DWVW_24 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 24 ;
				break ;

		case SF_FORMAT_GSM610 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = GSM_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				break ;

		case SF_FORMAT_IMA_ADPCM :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = ima4_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				comm_frames = psf->sf.frames / AIFC_IMA4_SAMPLES_PER_BLOCK ;
				break ;

		default : return SFE_BAD_OPEN_FORMAT ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	psf_binheader_writef (psf, "Etm8", FORM_MARKER, psf->filelength - 8) ;

	/* Write COMM chunk. */
	psf_binheader_writef (psf, "Emm4", comm_type, COMM_MARKER, comm_size) ;

	uint2tenbytefloat (psf->sf.samplerate, comm_sample_rate) ;

	psf_binheader_writef (psf, "Et242", psf->sf.channels, comm_frames, bit_width) ;
	psf_binheader_writef (psf, "b", comm_sample_rate, sizeof (comm_sample_rate)) ;

	/* AIFC chunks have some extra data. */
	if (comm_type == AIFC_MARKER)
		psf_binheader_writef (psf, "mb", comm_encoding, comm_zero_bytes, sizeof (comm_zero_bytes)) ;

	if (psf->str_flags & SF_STR_LOCATE_START)
		aiff_write_strings (psf, SF_STR_LOCATE_START) ;

	if (psf->has_peak && psf->peak_loc == SF_PEAK_START)
	{	psf_binheader_writef (psf, "Em4", PEAK_MARKER,
			sizeof (PEAK_CHUNK) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "Ef4", psf->pchunk->peaks [k].value, psf->pchunk->peaks [k].position) ; /* XXXXX */
		} ;

	/* Write SSND chunk. */
	psf_binheader_writef (psf, "Etm844", SSND_MARKER, psf->datalength + SIZEOF_SSND_CHUNK, 0, 0) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current < psf->dataoffset)
		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
	else if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* aiff_write_header */

static int
aiff_write_tailer (SF_PRIVATE *psf)
{	int		k ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	psf->dataend = psf_fseek (psf, 0, SEEK_END) ;

	if (psf->has_peak && psf->peak_loc == SF_PEAK_END)
	{	psf_binheader_writef (psf, "Em4", PEAK_MARKER,
			sizeof (PEAK_CHUNK) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "Ef4", psf->pchunk->peaks [k].value, psf->pchunk->peaks [k].position) ; /* XXXXX */
		} ;

	if (psf->str_flags & SF_STR_LOCATE_END)
		aiff_write_strings (psf, SF_STR_LOCATE_END) ;

	/* Write the tailer. */
	if (psf->headindex)
		psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	return 0 ;
} /* aiff_write_tailer */

static void
aiff_write_strings (SF_PRIVATE *psf, int location)
{	int	k ;

	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
	{	if (psf->strings [k].type == 0)
			break ;

		if (psf->strings [k].flags != location)
			continue ;

		switch (psf->strings [k].type)
		{	case SF_STR_SOFTWARE :
				psf_binheader_writef (psf, "Ems", APPL_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_TITLE :
				psf_binheader_writef (psf, "Ems", NAME_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_COPYRIGHT :
				psf_binheader_writef (psf, "Ems", c_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_ARTIST :
				psf_binheader_writef (psf, "Ems", AUTH_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_COMMENT :
				psf_binheader_writef (psf, "Ems", COMT_MARKER, psf->strings [k].str) ;
				break ;

			/*
			case SF_STR_DATE :
				psf_binheader_writef (psf, "Ems", ICRD_MARKER, psf->strings [k].str) ;
				break ;
			*/
			} ;
		} ;

	return ;
} /* aiff_write_strings */

static int
aiff_command (SF_PRIVATE *psf, int command, void *data, int datasize)
{
	/* Avoid compiler warnings. */
	psf = psf ;
	data = data ;
	datasize = datasize ;

	switch (command)
	{	default : break ;
		} ;

	return 0 ;
} /* aiff_command */

static const char*
get_loop_mode_str (short mode)
{	switch (mode)
	{	case 0 : return "none" ;
		case 1 : return "forward" ;
		case 2 : return "backward" ;
		} ;

	return "*** unknown" ;
} /* get_loop_mode_str */

/*==========================================================================================
**	Rough hack at converting from 80 bit IEEE float in AIFF header to an int and
**	back again. It assumes that all sample rates are between 1 and 800MHz, which
**	should be OK as other sound file formats use a 32 bit integer to store sample
**	rate.
**	There is another (probably better) version in the source code to the SoX but it
**	has a copyright which probably prevents it from being allowable as GPL/LGPL.
*/

static int
tenbytefloat2int (unsigned char *bytes)
{	int val = 3 ;

	if (bytes [0] & 0x80)	/* Negative number. */
		return 0 ;

	if (bytes [0] <= 0x3F)	/* Less than 1. */
		return 1 ;

	if (bytes [0] > 0x40)	/* Way too big. */
		return 0x4000000 ;

	if (bytes [0] == 0x40 && bytes [1] > 0x1C) /* Too big. */
		return 800000000 ;

	/* Ok, can handle it. */

	val = (bytes [2] << 23) | (bytes [3] << 15) | (bytes [4] << 7) | (bytes [5] >> 1) ;

	val >>= (29 - bytes [1]) ;

	return val ;
} /* tenbytefloat2int */

static void
uint2tenbytefloat (unsigned int num, unsigned char *bytes)
{	unsigned int mask = 0x40000000 ;
	int		count ;


	memset (bytes, 0, 10) ;

	if (num <= 1)
	{	bytes [0] = 0x3F ;
		bytes [1] = 0xFF ;
		bytes [2] = 0x80 ;
		return ;
		} ;

	bytes [0] = 0x40 ;

	if (num >= mask)
	{	bytes [1] = 0x1D ;
		return ;
		} ;

	for (count = 0 ; count <= 32 ; count ++)
	{	if (num & mask)
			break ;
		mask >>= 1 ;
		} ;

	num <<= count + 1 ;
	bytes [1] = 29 - count ;
	bytes [2] = (num >> 24) & 0xFF ;
	bytes [3] = (num >> 16) & 0xFF ;
	bytes [4] = (num >> 8) & 0xFF ;
	bytes [5] = num & 0xFF ;

} /* uint2tenbytefloat */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 7dec56ca-d6f2-48cf-863b-a72e7e17a5d9
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


static sf_count_t	alaw_read_alaw2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	alaw_read_alaw2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	alaw_read_alaw2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	alaw_read_alaw2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	alaw_write_s2alaw (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	alaw_write_i2alaw (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	alaw_write_f2alaw (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	alaw_write_d2alaw (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	void	alaw2s_array	(unsigned char *buffer, unsigned int count, short *ptr) ;
static	void	alaw2i_array	(unsigned char *buffer, unsigned int count, int *ptr) ;
static	void	alaw2f_array	(unsigned char *buffer, unsigned int count, float *ptr, float normfact) ;
static	void	alaw2d_array	(unsigned char *buffer, unsigned int count, double *ptr, double normfact) ;

static	void	s2alaw_array	(short *buffer, unsigned int count, unsigned char *ptr) ;
static	void	i2alaw_array	(int *buffer, unsigned int count, unsigned char *ptr) ;
static	void	f2alaw_array	(float *buffer, unsigned int count, unsigned char *ptr, float normfact) ;
static	void	d2alaw_array	(double *buffer, unsigned int count, unsigned char *ptr, double normfact) ;


int
alaw_init (SF_PRIVATE *psf)
{
	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	psf->read_short		= alaw_read_alaw2s ;
		psf->read_int		= alaw_read_alaw2i ;
		psf->read_float		= alaw_read_alaw2f ;
		psf->read_double	= alaw_read_alaw2d ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	psf->write_short	= alaw_write_s2alaw ;
		psf->write_int		= alaw_write_i2alaw ;
		psf->write_float	= alaw_write_f2alaw ;
		psf->write_double	= alaw_write_d2alaw ;
		} ;

	psf->bytewidth = 1 ;
	psf->blockwidth = psf->sf.channels ;


	if (psf->filelength > psf->dataoffset)
		psf->datalength = (psf->dataend) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
	else
		psf->datalength = 0 ;

	psf->sf.frames = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* alaw_init */

static sf_count_t
alaw_read_alaw2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		alaw2s_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* alaw_read_alaw2s */

static sf_count_t
alaw_read_alaw2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		alaw2i_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* alaw_read_alaw2i */

static sf_count_t
alaw_read_alaw2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		alaw2f_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* alaw_read_alaw2f */

static sf_count_t
alaw_read_alaw2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double	normfact ;

	normfact = (psf->norm_double) ? 1.0 / ((double) 0x8000) : 1.0 ;
	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		alaw2d_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* alaw_read_alaw2d */

/*=============================================================================================
*/

static sf_count_t
alaw_write_s2alaw	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2alaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer)) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;

		len -= thiswrite ;
		} ;

	return total ;
} /* alaw_write_s2alaw */

static sf_count_t
alaw_write_i2alaw	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2alaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer)) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;

		len -= thiswrite ;
		} ;

	return total ;
} /* alaw_write_i2alaw */

static sf_count_t
alaw_write_f2alaw	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2alaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer), normfact) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;

		len -= thiswrite ;
		} ;

	return total ;
} /* alaw_write_f2alaw */

static sf_count_t
alaw_write_d2alaw	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double	normfact ;

	normfact = (psf->norm_double) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2alaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer), normfact) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;

		len -= thiswrite ;
		} ;

	return total ;
} /* alaw_write_d2alaw */

/*=============================================================================================
 *	Private static functions and data.
 */

static
short	alaw_decode [128] =
{	-5504,	-5248,	-6016,	-5760,	-4480,	-4224,	-4992,	-4736,
	-7552,	-7296,	-8064,	-7808,	-6528,	-6272,	-7040,	-6784,
	-2752,	-2624,	-3008,	-2880,	-2240,	-2112,	-2496,	-2368,
	-3776,	-3648,	-4032,	-3904,	-3264,	-3136,	-3520,	-3392,
	-22016,	-20992,	-24064,	-23040,	-17920,	-16896,	-19968,	-18944,
	-30208,	-29184,	-32256,	-31232,	-26112,	-25088,	-28160,	-27136,
	-11008,	-10496,	-12032,	-11520,	-8960,	-8448,	-9984,	-9472,
	-15104,	-14592,	-16128,	-15616,	-13056,	-12544,	-14080,	-13568,
	-344,	-328,	-376,	-360,	-280,	-264,	-312,	-296,
	-472,	-456,	-504,	-488,	-408,	-392,	-440,	-424,
	-88,	-72,	-120,	-104,	-24,	-8,		-56,	-40,
	-216,	-200,	-248,	-232,	-152,	-136,	-184,	-168,
	-1376,	-1312,	-1504,	-1440,	-1120,	-1056,	-1248,	-1184,
	-1888,	-1824,	-2016,	-1952,	-1632,	-1568,	-1760,	-1696,
	-688,	-656,	-752,	-720,	-560,	-528,	-624,	-592,
	-944,	-912,	-1008,	-976,	-816,	-784,	-880,	-848
} ; /* alaw_decode */

static
unsigned char	alaw_encode [2049] =
{	0xD5, 0xD4, 0xD7, 0xD6, 0xD1, 0xD0, 0xD3, 0xD2, 0xDD, 0xDC, 0xDF, 0xDE,
	0xD9, 0xD8, 0xDB, 0xDA, 0xC5, 0xC4, 0xC7, 0xC6, 0xC1, 0xC0, 0xC3, 0xC2,
	0xCD, 0xCC, 0xCF, 0xCE, 0xC9, 0xC8, 0xCB, 0xCA, 0xF5, 0xF5, 0xF4, 0xF4,
	0xF7, 0xF7, 0xF6, 0xF6, 0xF1, 0xF1, 0xF0, 0xF0, 0xF3, 0xF3, 0xF2, 0xF2,
	0xFD, 0xFD, 0xFC, 0xFC, 0xFF, 0xFF, 0xFE, 0xFE, 0xF9, 0xF9, 0xF8, 0xF8,
	0xFB, 0xFB, 0xFA, 0xFA, 0xE5, 0xE5, 0xE5, 0xE5, 0xE4, 0xE4, 0xE4, 0xE4,
	0xE7, 0xE7, 0xE7, 0xE7, 0xE6, 0xE6, 0xE6, 0xE6, 0xE1, 0xE1, 0xE1, 0xE1,
	0xE0, 0xE0, 0xE0, 0xE0, 0xE3, 0xE3, 0xE3, 0xE3, 0xE2, 0xE2, 0xE2, 0xE2,
	0xED, 0xED, 0xED, 0xED, 0xEC, 0xEC, 0xEC, 0xEC, 0xEF, 0xEF, 0xEF, 0xEF,
	0xEE, 0xEE, 0xEE, 0xEE, 0xE9, 0xE9, 0xE9, 0xE9, 0xE8, 0xE8, 0xE8, 0xE8,
	0xEB, 0xEB, 0xEB, 0xEB, 0xEA, 0xEA, 0xEA, 0xEA, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0xB5, 0xB5, 0xB5, 0xB5,
	0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
	0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
	0xB5, 0xB5, 0xB5, 0xB5, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7,
	0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7,
	0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
	0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0,
	0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0,
	0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB3, 0xB3, 0xB3, 0xB3,
	0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
	0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
	0xB3, 0xB3, 0xB3, 0xB3, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
	0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
	0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
	0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD,
	0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD,
	0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBC, 0xBC, 0xBC, 0xBC,
	0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC,
	0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC,
	0xBC, 0xBC, 0xBC, 0xBC, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
	0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
	0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
	0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBA, 0xBA, 0xBA, 0xBA,
	0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA,
	0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA,
	0xBA, 0xBA, 0xBA, 0xBA, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A
} ; /* alaw_encode */

static void
alaw2s_array	(unsigned char *buffer, unsigned int count, short *ptr)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -1 * alaw_decode [((int) buffer [count]) & 0x7F] ;
		else
			ptr [count] = alaw_decode [((int) buffer [count]) & 0x7F] ;
		} ;
} /* alaw2s_array */

static void
alaw2i_array	(unsigned char *buffer, unsigned int count, int *ptr)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = (-1 * alaw_decode [((int) buffer [count]) & 0x7F]) << 16 ;
		else
			ptr [count] = alaw_decode [((int) buffer [count]) & 0x7F] << 16 ;
		} ;
} /* alaw2i_array */

static void
alaw2f_array	(unsigned char *buffer, unsigned int count, float *ptr, float normfact)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -normfact * alaw_decode [((int) buffer [count]) & 0x7F] ;
		else
			ptr [count] = normfact * alaw_decode [((int) buffer [count]) & 0x7F] ;
		} ;
} /* alaw2f_array */

static void
alaw2d_array	(unsigned char *buffer, unsigned int count, double *ptr, double normfact)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -normfact * alaw_decode [((int) buffer [count]) & 0x7F] ;
		else
			ptr [count] = normfact * alaw_decode [((int) buffer [count]) & 0x7F] ;
		} ;
} /* alaw2d_array */

static void
s2alaw_array	(short *ptr, unsigned int count, unsigned char *buffer)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = alaw_encode [ptr [count] / 16] ;
		else
			buffer [count] = 0x7F & alaw_encode [ptr [count] / -16] ;
		} ;
} /* s2alaw_array */

static void
i2alaw_array	(int *ptr, unsigned int count, unsigned char *buffer)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = alaw_encode [ptr [count] >> (16 + 4)] ;
		else
			buffer [count] = 0x7F & alaw_encode [- ptr [count] >> (16 + 4)] ;
		} ;
} /* i2alaw_array */

static void
f2alaw_array	(float *ptr, unsigned int count, unsigned char *buffer, float normfact)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = alaw_encode [(lrintf (normfact * ptr [count])) / 16] ;
		else
			buffer [count] = 0x7F & alaw_encode [(lrintf (normfact * ptr [count])) / -16] ;
		} ;
} /* f2alaw_array */

static void
d2alaw_array	(double *ptr, unsigned int count, unsigned char *buffer, double normfact)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = alaw_encode [(lrint (normfact * ptr [count])) / 16] ;
		else
			buffer [count] = 0x7F & alaw_encode [(lrint (normfact * ptr [count])) / -16] ;
		} ;
} /* d2alaw_array */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 289ccfc2-42a6-4f1f-a29f-4dcc9bfa8752
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define DOTSND_MARKER	(MAKE_MARKER ('.', 's', 'n', 'd'))
#define DNSDOT_MARKER	(MAKE_MARKER ('d', 'n', 's', '.'))

#define AU_DATA_OFFSET	24

/*------------------------------------------------------------------------------
** Known AU file encoding types.
*/

enum
{	AU_ENCODING_ULAW_8					= 1,	/* 8-bit u-law samples */
	AU_ENCODING_PCM_8					= 2,	/* 8-bit linear samples */
	AU_ENCODING_PCM_16					= 3,	/* 16-bit linear samples */
	AU_ENCODING_PCM_24					= 4,	/* 24-bit linear samples */
	AU_ENCODING_PCM_32					= 5,	/* 32-bit linear samples */

	AU_ENCODING_FLOAT					= 6,	/* floating-point samples */
	AU_ENCODING_DOUBLE					= 7,	/* double-precision float samples */
	AU_ENCODING_INDIRECT				= 8,	/* fragmented sampled data */
	AU_ENCODING_NESTED					= 9,	/* ? */
	AU_ENCODING_DSP_CORE				= 10,	/* DSP program */
	AU_ENCODING_DSP_DATA_8				= 11,	/* 8-bit fixed-point samples */
	AU_ENCODING_DSP_DATA_16				= 12,	/* 16-bit fixed-point samples */
	AU_ENCODING_DSP_DATA_24				= 13,	/* 24-bit fixed-point samples */
	AU_ENCODING_DSP_DATA_32				= 14,	/* 32-bit fixed-point samples */

	AU_ENCODING_DISPLAY					= 16,	/* non-audio display data */
	AU_ENCODING_MULAW_SQUELCH			= 17,	/* ? */
	AU_ENCODING_EMPHASIZED				= 18,	/* 16-bit linear with emphasis */
	AU_ENCODING_NEXT					= 19,	/* 16-bit linear with compression (NEXT) */
	AU_ENCODING_COMPRESSED_EMPHASIZED	= 20,	/* A combination of the two above */
	AU_ENCODING_DSP_COMMANDS			= 21,	/* Music Kit DSP commands */
	AU_ENCODING_DSP_COMMANDS_SAMPLES	= 22,	/* ? */

	AU_ENCODING_ADPCM_G721_32			= 23,	/* G721 32 kbs ADPCM - 4 bits per sample. */
	AU_ENCODING_ADPCM_G722				= 24,	/* G722 64 kbs ADPCM */
	AU_ENCODING_ADPCM_G723_24			= 25,	/* G723 24 kbs ADPCM - 3 bits per sample. */
	AU_ENCODING_ADPCM_G723_40			= 26,	/* G723 40 kbs ADPCM - 5 bits per sample. */

	AU_ENCODING_ALAW_8					= 27
} ;

/*------------------------------------------------------------------------------
** Typedefs.
*/

typedef	struct
{	int		dataoffset ;
	int		datasize ;
	int		encoding ;
    int		samplerate ;
    int		channels ;
} AU_FMT ;


/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int		au_close		(SF_PRIVATE *psf) ;

static	int 	au_format_to_encoding	(int format) ;

static int		au_write_header (SF_PRIVATE *psf, int calc_length) ;
static int		au_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
au_open	(SF_PRIVATE *psf)
{	int		subformat ;
	int		error = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = au_read_header (psf)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_AU)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		if (CPU_IS_LITTLE_ENDIAN && psf->endian == SF_ENDIAN_CPU)
			psf->endian = SF_ENDIAN_LITTLE ;
		else if (psf->endian != SF_ENDIAN_LITTLE)
			psf->endian = SF_ENDIAN_BIG ;

		if (au_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = au_write_header ;
		} ;

	psf->close = au_close ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case SF_FORMAT_ULAW :	/* 8-bit Ulaw encoding. */
				ulaw_init (psf) ;
				break ;

		case SF_FORMAT_PCM_S8 :	/* 8-bit linear PCM. */
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :	/* 16-bit linear PCM. */
		case SF_FORMAT_PCM_24 :	/* 24-bit linear PCM */
		case SF_FORMAT_PCM_32 :	/* 32-bit linear PCM. */
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :	/* 8-bit Alaw encoding. */
				alaw_init (psf) ;
				break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :	/* 32-bit floats. */
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :	/* 64-bit double precision floats. */
				error = double64_init (psf) ;
				break ;

		case SF_FORMAT_G721_32 :
				if (psf->mode == SFM_READ)
					error = au_g72x_reader_init (psf, AU_H_G721_32) ;
				else if (psf->mode == SFM_WRITE)
					error = au_g72x_writer_init (psf, AU_H_G721_32) ;
				psf->sf.seekable = SF_FALSE ;
				break ;

		case SF_FORMAT_G723_24 :
				if (psf->mode == SFM_READ)
					error = au_g72x_reader_init (psf, AU_H_G723_24) ;
				else if (psf->mode == SFM_WRITE)
					error = au_g72x_writer_init (psf, AU_H_G723_24) ;
				psf->sf.seekable = SF_FALSE ;
				break ;

		case SF_FORMAT_G723_40 :
				if (psf->mode == SFM_READ)
					error = au_g72x_reader_init (psf, AU_H_G723_40) ;
				else if (psf->mode == SFM_WRITE)
					error = au_g72x_writer_init (psf, AU_H_G723_40) ;
				psf->sf.seekable = SF_FALSE ;
				break ;
		/* Lite remove end */

		default :	break ;
		} ;

	return error ;
} /* au_open */

int
au_nh_open	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_RDWR)
		return SFE_BAD_OPEN_FORMAT ;

	if (psf_fseek (psf, psf->dataoffset, SEEK_SET))
		return SFE_BAD_SEEK ;

	psf_log_printf (psf, "Header-less u-law encoded file.\n") ;
	psf_log_printf (psf, "Setting up for 8kHz, mono, u-law.\n") ;

	psf->sf.format = SF_FORMAT_AU | SF_FORMAT_ULAW ;

 	psf->dataoffset		= 0 ;
	psf->endian			= 0 ;	/* Irrelevant but it must be something. */
	psf->sf.samplerate	= 8000 ;
	psf->sf.channels	= 1 ;
	psf->bytewidth		= 1 ;	/* Before decoding */

	ulaw_init (psf) ;

	psf->close = au_close ;

	psf->blockwidth = 1 ;
	psf->sf.frames = psf->filelength ;
	psf->datalength = psf->filelength - AU_DATA_OFFSET ;

	return 0 ;
} /* au_nh_open */

/*------------------------------------------------------------------------------
*/

static int
au_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		au_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* au_close */

static int
au_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int			encoding, datalength ;

	if (psf->pipeoffset > 0)
		return 0 ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	encoding = au_format_to_encoding (psf->sf.format & SF_FORMAT_SUBMASK) ;
	if (! encoding)
		return (psf->error = SFE_BAD_OPEN_FORMAT) ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	/*
	** Only attempt to seek if we are not writng to a pipe. If we are
	** writing to a pipe we shouldn't be here anyway.
	*/
	if (psf->is_pipe == SF_FALSE)
		psf_fseek (psf, 0, SEEK_SET) ;

	/*
	**	AU format files allow a datalength value of -1 if the datalength
	**	is not know at the time the header is written.
	**	Also use this value of -1 if the datalength > 2 gigabytes.
	*/
	if (psf->datalength	< 0 || psf->datalength > 0x7FFFFFFF)
		datalength = -1 ;
	else
		datalength = (int) (psf->datalength & 0x7FFFFFFF) ;

	if (psf->endian == SF_ENDIAN_BIG)
	{	psf_binheader_writef (psf, "Em4", DOTSND_MARKER, AU_DATA_OFFSET) ;
		psf_binheader_writef (psf, "E4444", datalength, encoding, psf->sf.samplerate, psf->sf.channels) ;
		}
	else if (psf->endian == SF_ENDIAN_LITTLE)
	{	psf_binheader_writef (psf, "em4", DNSDOT_MARKER, AU_DATA_OFFSET) ;
		psf_binheader_writef (psf, "e4444", datalength, encoding, psf->sf.samplerate, psf->sf.channels) ;
		}
	else
		return (psf->error = SFE_BAD_OPEN_FORMAT) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* au_write_header */

static int
au_format_to_encoding (int format)
{
	switch (format)
	{	case SF_FORMAT_PCM_S8 : 	return AU_ENCODING_PCM_8 ;
		case SF_FORMAT_PCM_16 :		return AU_ENCODING_PCM_16 ;
		case SF_FORMAT_PCM_24 : 	return AU_ENCODING_PCM_24 ;
		case SF_FORMAT_PCM_32 : 	return AU_ENCODING_PCM_32 ;

		case SF_FORMAT_FLOAT :		return AU_ENCODING_FLOAT ;
		case SF_FORMAT_DOUBLE :		return AU_ENCODING_DOUBLE ;

		case SF_FORMAT_ULAW :		return AU_ENCODING_ULAW_8 ;
		case SF_FORMAT_ALAW :		return AU_ENCODING_ALAW_8 ;

		case SF_FORMAT_G721_32 : 	return AU_ENCODING_ADPCM_G721_32 ;
		case SF_FORMAT_G723_24 :	return AU_ENCODING_ADPCM_G723_24 ;
		case SF_FORMAT_G723_40 :	return AU_ENCODING_ADPCM_G723_40 ;

		default : break ;
		} ;
	return 0 ;
} /* au_format_to_encoding */

static int
au_read_header (SF_PRIVATE *psf)
{	AU_FMT	au_fmt ;
	int		marker, dword ;

	psf_binheader_readf (psf, "pm", 0, &marker) ;
	psf_log_printf (psf, "%M\n", marker) ;

	if (marker == DOTSND_MARKER)
	{	psf->endian = SF_ENDIAN_BIG ;

		psf_binheader_readf (psf, "E44444", &(au_fmt.dataoffset), &(au_fmt.datasize),
					&(au_fmt.encoding), &(au_fmt.samplerate), &(au_fmt.channels)) ;
		}
	else if (marker == DNSDOT_MARKER)
	{	psf->endian = SF_ENDIAN_LITTLE ;
		psf_binheader_readf (psf, "e44444", &(au_fmt.dataoffset), &(au_fmt.datasize),
					&(au_fmt.encoding), &(au_fmt.samplerate), &(au_fmt.channels)) ;
		}
	else
		return SFE_AU_NO_DOTSND ;

	psf_log_printf (psf, "  Data Offset : %d\n", au_fmt.dataoffset) ;

	if (psf->fileoffset > 0 && au_fmt.datasize == -1)
	{	psf_log_printf (psf, "  Data Size   : -1\n") ;
		return SFE_AU_EMBED_BAD_LEN ;
		} ;

	if (psf->fileoffset > 0)
	{	psf->filelength = au_fmt.dataoffset + au_fmt.datasize ;
		psf_log_printf (psf, "  Data Size   : %d\n", au_fmt.datasize) ;
		}
	else if (au_fmt.datasize == -1 || au_fmt.dataoffset + au_fmt.datasize == psf->filelength)
		psf_log_printf (psf, "  Data Size   : %d\n", au_fmt.datasize) ;
	else if (au_fmt.dataoffset + au_fmt.datasize < psf->filelength)
	{	psf->filelength = au_fmt.dataoffset + au_fmt.datasize ;
		psf_log_printf (psf, "  Data Size   : %d\n", au_fmt.datasize) ;
		}
	else
	{	dword = psf->filelength - au_fmt.dataoffset ;
		psf_log_printf (psf, "  Data Size   : %d (should be %d)\n", au_fmt.datasize, dword) ;
		au_fmt.datasize = dword ;
		} ;

 	psf->dataoffset = au_fmt.dataoffset ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	if (psf_ftell (psf) < psf->dataoffset)
		psf_binheader_readf (psf, "j", psf->dataoffset - psf_ftell (psf)) ;

	psf->close = au_close ;

	psf->sf.samplerate	= au_fmt.samplerate ;
	psf->sf.channels 	= au_fmt.channels ;

	/* Only fill in type major. */
	if (psf->endian == SF_ENDIAN_BIG)
		psf->sf.format = SF_FORMAT_AU ;
	else if (psf->endian == SF_ENDIAN_LITTLE)
		psf->sf.format = SF_ENDIAN_LITTLE | SF_FORMAT_AU ;

	psf_log_printf (psf, "  Encoding    : %d => ", au_fmt.encoding) ;

	psf->sf.format = psf->sf.format & SF_FORMAT_ENDMASK ;

	switch (au_fmt.encoding)
	{	case AU_ENCODING_ULAW_8 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_ULAW ;
				psf->bytewidth = 1 ;	/* Before decoding */
				psf_log_printf (psf, "8-bit ISDN u-law\n") ;
				break ;

		case AU_ENCODING_PCM_8 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_PCM_S8 ;
				psf->bytewidth = 1 ;
				psf_log_printf (psf, "8-bit linear PCM\n") ;
				break ;

		case AU_ENCODING_PCM_16 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_PCM_16 ;
				psf->bytewidth = 2 ;
				psf_log_printf (psf, "16-bit linear PCM\n") ;
				break ;

		case AU_ENCODING_PCM_24 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_PCM_24 ;
				psf->bytewidth = 3 ;
				psf_log_printf (psf, "24-bit linear PCM\n") ;
				break ;

		case AU_ENCODING_PCM_32 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_PCM_32 ;
				psf->bytewidth = 4 ;
				psf_log_printf (psf, "32-bit linear PCM\n") ;
				break ;

		case AU_ENCODING_FLOAT :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_FLOAT ;
				psf->bytewidth = 4 ;
				psf_log_printf (psf, "32-bit float\n") ;
				break ;

		case AU_ENCODING_DOUBLE :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_DOUBLE ;
				psf->bytewidth = 8 ;
				psf_log_printf (psf, "64-bit double precision float\n") ;
				break ;

		case AU_ENCODING_ALAW_8 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_ALAW ;
				psf->bytewidth = 1 ;	/* Before decoding */
				psf_log_printf (psf, "8-bit ISDN A-law\n") ;
				break ;

		case AU_ENCODING_ADPCM_G721_32 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_G721_32 ;
				psf->bytewidth = 0 ;
				psf_log_printf (psf, "G721 32kbs ADPCM\n") ;
				break ;

		case AU_ENCODING_ADPCM_G723_24 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_G723_24 ;
				psf->bytewidth = 0 ;
				psf_log_printf (psf, "G723 24kbs ADPCM\n") ;
				break ;

		case AU_ENCODING_ADPCM_G723_40 :
				psf->sf.format |= SF_FORMAT_AU | SF_FORMAT_G723_40 ;
				psf->bytewidth = 0 ;
				psf_log_printf (psf, "G723 40kbs ADPCM\n") ;
				break ;

		case AU_ENCODING_ADPCM_G722 :
				psf_log_printf (psf, "G722 64 kbs ADPCM (unsupported)\n") ;
				break ;

		case AU_ENCODING_NEXT :
				psf_log_printf (psf, "Weird NeXT encoding format (unsupported)\n") ;
				break ;

		default :
				psf_log_printf (psf, "Unknown!!\n") ;
				break ;
		} ;

	psf_log_printf (psf, "  Sample Rate : %d\n", au_fmt.samplerate) ;
	psf_log_printf (psf, "  Channels    : %d\n", au_fmt.channels) ;

	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	if (! psf->sf.frames && psf->blockwidth)
		psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;

	return 0 ;
} /* au_read_header */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 31f691b1-cde9-4ed2-9469-6bca60fb9cd0
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static	int au_g72x_read_block	(SF_PRIVATE *psf, G72x_DATA *pg72x, short *ptr, int len) ;
static	int au_g72x_write_block	(SF_PRIVATE *psf, G72x_DATA *pg72x, short *ptr, int len) ;

static	int	au_g72x_decode_block	(SF_PRIVATE *psf, G72x_DATA *pg72x) ;
static	int	au_g72x_encode_block	(SF_PRIVATE *psf, G72x_DATA *pg72x) ;

static	sf_count_t	au_g72x_read_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static	sf_count_t	au_g72x_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static	sf_count_t	au_g72x_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static	sf_count_t	au_g72x_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	sf_count_t	au_g72x_write_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static	sf_count_t	au_g72x_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static	sf_count_t	au_g72x_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static	sf_count_t	au_g72x_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	sf_count_t au_g72x_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset) ;

static	int	au_g72x_close	(SF_PRIVATE *psf) ;


/*============================================================================================
** WAV G721 Reader initialisation function.
*/

int
au_g72x_reader_init	(SF_PRIVATE *psf, int codec)
{	G72x_DATA	*pg72x ;
	int	bitspersample ;

	psf->sf.seekable = SF_FALSE ;

	if (psf->sf.channels != 1)
		return SFE_G72X_NOT_MONO ;

	if (! (pg72x = malloc (sizeof (G72x_DATA))))
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pg72x ;

	pg72x->blockcount = 0 ;
	pg72x->samplecount = 0 ;

	switch (codec)
	{	case AU_H_G721_32 :
				g72x_reader_init (pg72x, G721_32_BITS_PER_SAMPLE) ;
				pg72x->bytesperblock = G721_32_BYTES_PER_BLOCK ;
				bitspersample = G721_32_BITS_PER_SAMPLE ;
				break ;

		case AU_H_G723_24:
				g72x_reader_init (pg72x, G723_24_BITS_PER_SAMPLE) ;
				pg72x->bytesperblock = G723_24_BYTES_PER_BLOCK ;
				bitspersample = G723_24_BITS_PER_SAMPLE ;
				break ;

		case AU_H_G723_40:
				g72x_reader_init (pg72x, G723_40_BITS_PER_SAMPLE) ;
				pg72x->bytesperblock = G723_40_BYTES_PER_BLOCK ;
				bitspersample = G723_40_BITS_PER_SAMPLE ;
				break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	psf->read_short		= au_g72x_read_s ;
	psf->read_int		= au_g72x_read_i ;
	psf->read_float		= au_g72x_read_f ;
	psf->read_double	= au_g72x_read_d ;

 	psf->seek	= au_g72x_seek ;
 	psf->close	= au_g72x_close ;

	psf->blockwidth = psf->bytewidth = 1 ;

	psf->filelength = psf_get_filelen (psf) ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	if (psf->datalength % pg72x->blocksize)
		pg72x->blocks = (psf->datalength / pg72x->blocksize) + 1 ;
	else
		pg72x->blocks = psf->datalength / pg72x->blocksize ;

	psf->sf.frames = (8 * psf->datalength) / bitspersample ;

	if ((psf->sf.frames * bitspersample) / 8 != psf->datalength)
		psf_log_printf (psf, "*** Warning : weird psf->datalength.\n") ;

	au_g72x_decode_block (psf, pg72x) ;

	return 0 ;
} /* au_g72x_reader_init */

/*============================================================================================
** WAV G721 writer initialisation function.
*/

int
au_g72x_writer_init	(SF_PRIVATE *psf, int codec)
{	G72x_DATA	*pg72x ;
	int bitspersample ;

	psf->sf.seekable = SF_FALSE ;

	if (psf->sf.channels != 1)
		return SFE_G72X_NOT_MONO ;

	if (! (pg72x = malloc (sizeof (G72x_DATA))))
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pg72x ;

	pg72x->blockcount = 0 ;
	pg72x->samplecount = 0 ;

	switch (codec)
	{	case AU_H_G721_32 :
				g72x_writer_init (pg72x, G721_32_BITS_PER_SAMPLE) ;
				pg72x->bytesperblock = G721_32_BYTES_PER_BLOCK ;
				bitspersample = G721_32_BITS_PER_SAMPLE ;
				break ;

		case AU_H_G723_24:
				g72x_writer_init (pg72x, G723_24_BITS_PER_SAMPLE) ;
				pg72x->bytesperblock = G723_24_BYTES_PER_BLOCK ;
				bitspersample = G723_24_BITS_PER_SAMPLE ;
				break ;

		case AU_H_G723_40:
				g72x_writer_init (pg72x, G723_40_BITS_PER_SAMPLE) ;
				pg72x->bytesperblock = G723_40_BYTES_PER_BLOCK ;
				bitspersample = G723_40_BITS_PER_SAMPLE ;
				break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	psf->write_short	= au_g72x_write_s ;
	psf->write_int		= au_g72x_write_i ;
	psf->write_float	= au_g72x_write_f ;
	psf->write_double	= au_g72x_write_d ;

 	psf->close = au_g72x_close ;

	psf->blockwidth = psf->bytewidth = 1 ;

	psf->filelength = psf_get_filelen (psf) ;
	if (psf->filelength < psf->dataoffset)
		psf->filelength = psf->dataoffset ;

	psf->datalength = psf->filelength - psf->dataoffset ;

	if (psf->datalength % pg72x->blocksize)
		pg72x->blocks = (psf->datalength / pg72x->blocksize) + 1 ;
	else
		pg72x->blocks = psf->datalength / pg72x->blocksize ;

	if (psf->datalength > 0)
		psf->sf.frames = (8 * psf->datalength) / bitspersample ;

	if ((psf->sf.frames * bitspersample) / 8 != psf->datalength)
		psf_log_printf (psf, "*** Warning : weird psf->datalength.\n") ;

	return 0 ;
} /* au_g72x_writer_init */

/*============================================================================================
** G721 Read Functions.
*/

static int
au_g72x_decode_block	(SF_PRIVATE *psf, G72x_DATA *pg72x)
{	int	k ;

	pg72x->blockcount ++ ;
	pg72x->samplecount = 0 ;

	if (pg72x->samplecount > pg72x->blocksize)
	{	memset (pg72x->samples, 0, G72x_BLOCK_SIZE * sizeof (short)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (pg72x->block, 1, pg72x->bytesperblock, psf)) != pg72x->bytesperblock)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, pg72x->bytesperblock) ;

	pg72x->blocksize = k ;
	g72x_decode_block (pg72x) ;

	return 1 ;
} /* au_g72x_decode_block */

static int
au_g72x_read_block	(SF_PRIVATE *psf, G72x_DATA *pg72x, short *ptr, int len)
{	int	count, total = 0, indx = 0 ;

	while (indx < len)
	{	if (pg72x->blockcount >= pg72x->blocks && pg72x->samplecount >= pg72x->samplesperblock)
		{	memset (&(ptr [indx]), 0, (len - indx) * sizeof (short)) ;
			return total ;
			} ;

		if (pg72x->samplecount >= pg72x->samplesperblock)
			au_g72x_decode_block (psf, pg72x) ;

		count = pg72x->samplesperblock - pg72x->samplecount ;
		count = (len - indx > count) ? count : len - indx ;

		memcpy (&(ptr [indx]), &(pg72x->samples [pg72x->samplecount]), count * sizeof (short)) ;
		indx += count ;
		pg72x->samplecount += count ;
		total = indx ;
		} ;

	return total ;
} /* au_g72x_read_block */

static sf_count_t
au_g72x_read_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	G72x_DATA 	*pg72x ;
	int			readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	while (len > 0)
	{	readcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = au_g72x_read_block (psf, pg72x, ptr, readcount) ;

		total += count ;
		len -= count ;

		if (count != readcount)
			break ;
		} ;

	return total ;
} /* au_g72x_read_s */

static sf_count_t
au_g72x_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	G72x_DATA *pg72x ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = au_g72x_read_block (psf, pg72x, sptr, readcount) ;

		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = sptr [k] << 16 ;

		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* au_g72x_read_i */

static sf_count_t
au_g72x_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	G72x_DATA *pg72x ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	float 		normfact ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = au_g72x_read_block (psf, pg72x, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * sptr [k] ;

		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* au_g72x_read_f */

static sf_count_t
au_g72x_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	G72x_DATA *pg72x ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = au_g72x_read_block (psf, pg72x, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (double) (sptr [k]) ;

		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* au_g72x_read_d */

static sf_count_t
au_g72x_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset)
{
	/* Prevent compiler warnings. */
	mode ++ ;
	offset ++ ;

	psf_log_printf (psf, "seek unsupported\n") ;

	/*	No simple solution. To do properly, would need to seek
	**	to start of file and decode everything up to seek position.
	**	Maybe implement SEEK_SET to 0 only?
	*/
	return 0 ;

/*
**		G72x_DATA	*pg72x ;
**		int			newblock, newsample, samplecount ;
**
**		if (! psf->fdata)
**			return 0 ;
**		pg72x = (G72x_DATA*) psf->fdata ;
**
**		if (! (psf->datalength && psf->dataoffset))
**		{	psf->error = SFE_BAD_SEEK ;
**			return	((sf_count_t) -1) ;
**			} ;
**
**		samplecount = (8 * psf->datalength) / G721_32_BITS_PER_SAMPLE ;
**
**		switch (whence)
**		{	case SEEK_SET :
**					if (offset < 0 || offset > samplecount)
**					{	psf->error = SFE_BAD_SEEK ;
**						return	((sf_count_t) -1) ;
**						} ;
**					newblock  = offset / pg72x->samplesperblock ;
**					newsample = offset % pg72x->samplesperblock ;
**					break ;
**
**			case SEEK_CUR :
**					if (psf->current + offset < 0 || psf->current + offset > samplecount)
**					{	psf->error = SFE_BAD_SEEK ;
**						return	((sf_count_t) -1) ;
**						} ;
**					newblock  = (8 * (psf->current + offset)) / pg72x->samplesperblock ;
**					newsample = (8 * (psf->current + offset)) % pg72x->samplesperblock ;
**					break ;
**
**			case SEEK_END :
**					if (offset > 0 || samplecount + offset < 0)
**					{	psf->error = SFE_BAD_SEEK ;
**						return	((sf_count_t) -1) ;
**						} ;
**					newblock  = (samplecount + offset) / pg72x->samplesperblock ;
**					newsample = (samplecount + offset) % pg72x->samplesperblock ;
**					break ;
**
**			default :
**					psf->error = SFE_BAD_SEEK ;
**					return	((sf_count_t) -1) ;
**			} ;
**
**		if (psf->mode == SFM_READ)
**		{	psf_fseek (psf, psf->dataoffset + newblock * pg72x->blocksize, SEEK_SET) ;
**			pg72x->blockcount  = newblock ;
**			au_g72x_decode_block (psf, pg72x) ;
**			pg72x->samplecount = newsample ;
**			}
**		else
**		{	/+* What to do about write??? *+/
**			psf->error = SFE_BAD_SEEK ;
**			return	((sf_count_t) -1) ;
**			} ;
**
**		psf->current = newblock * pg72x->samplesperblock + newsample ;
**		return psf->current ;
**
*/
} /* au_g72x_seek */

/*==========================================================================================
** G72x Write Functions.
*/

static int
au_g72x_encode_block	(SF_PRIVATE *psf, G72x_DATA *pg72x)
{	int k ;

	/* Encode the samples. */
	g72x_encode_block (pg72x) ;

	/* Write the block to disk. */
	if ((k = psf_fwrite (pg72x->block, 1, pg72x->blocksize, psf)) != pg72x->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, pg72x->blocksize) ;

	pg72x->samplecount = 0 ;
	pg72x->blockcount ++ ;

	/* Set samples to zero for next block. */
	memset (pg72x->samples, 0, G72x_BLOCK_SIZE * sizeof (short)) ;

	return 1 ;
} /* au_g72x_encode_block */

static int
au_g72x_write_block	(SF_PRIVATE *psf, G72x_DATA *pg72x, short *ptr, int len)
{	int	count, total = 0, indx = 0 ;

	while (indx < len)
	{	count = pg72x->samplesperblock - pg72x->samplecount ;

		if (count > len - indx)
			count = len - indx ;

		memcpy (&(pg72x->samples [pg72x->samplecount]), &(ptr [indx]), count * sizeof (short)) ;
		indx += count ;
		pg72x->samplecount += count ;
		total = indx ;

		if (pg72x->samplecount >= pg72x->samplesperblock)
			au_g72x_encode_block (psf, pg72x) ;
		} ;

	return total ;
} /* au_g72x_write_block */

static sf_count_t
au_g72x_write_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	G72x_DATA 	*pg72x ;
	int			writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	while (len > 0)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = au_g72x_write_block (psf, pg72x, ptr, writecount) ;

		total += count ;
		len -= count ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* au_g72x_write_s */

static sf_count_t
au_g72x_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	G72x_DATA *pg72x ;
	short		*sptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = ((SF_BUFFER_LEN / psf->blockwidth) * psf->blockwidth) / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = ptr [total + k] >> 16 ;
		count = au_g72x_write_block (psf, pg72x, sptr, writecount) ;

		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;
	return total ;
} /* au_g72x_write_i */

static sf_count_t
au_g72x_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	G72x_DATA *pg72x ;
	short		*sptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = ((SF_BUFFER_LEN / psf->blockwidth) * psf->blockwidth) / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = au_g72x_write_block (psf, pg72x, sptr, writecount) ;

		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* au_g72x_write_f */

static sf_count_t
au_g72x_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	G72x_DATA *pg72x ;
	short		*sptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if (! psf->fdata)
		return 0 ;
	pg72x = (G72x_DATA*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = ((SF_BUFFER_LEN / psf->blockwidth) * psf->blockwidth) / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrint (normfact * ptr [total + k]) ;
		count = au_g72x_write_block (psf, pg72x, sptr, writecount) ;

		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* au_g72x_write_d */

static int
au_g72x_close	(SF_PRIVATE *psf)
{	G72x_DATA *pg72x ;

	if (! psf->fdata)
		return 0 ;

	pg72x = (G72x_DATA*) psf->fdata ;

	if (psf->mode == SFM_WRITE)
	{	/*	If a block has been partially assembled, write it out
		**	as the final block.
		*/

		if (pg72x->samplecount && pg72x->samplecount < G72x_BLOCK_SIZE)
			au_g72x_encode_block (psf, pg72x) ;

		if (psf->write_header)
			psf->write_header (psf, SF_FALSE) ;
		} ;

	return 0 ;
} /* au_g72x_close */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 3cc5439e-7247-486b-b2e6-11a4affa5744
*/
/*
** Copyright (C) 2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <string.h>


#define TWOBIT_MARKER	(MAKE_MARKER ('2', 'B', 'I', 'T'))
#define	AVR_HDR_SIZE	128

#define	SFE_AVR_X	666

/*
** From: hyc@hanauma.Jpl.Nasa.Gov (Howard Chu)
**
** A lot of PD software exists to play Mac .snd files on the ST. One other
** format that seems pretty popular (used by a number of commercial packages)
** is the AVR format (from Audio Visual Research). This format has a 128 byte
** header that looks like this (its actually packed, but thats not portable):
*/

typedef struct
{	int		marker ;	/* 2BIT */
	char	name [8] ;	/* null-padded sample name */
	short	mono ;		/* 0 = mono, 0xffff = stereo */
	short	rez ;		/* 8 = 8 bit, 16 = 16 bit */
	short	sign ;		/* 0 = unsigned, 0xffff = signed */

	short	loop ;		/* 0 = no loop, 0xffff = looping sample */
	short	midi ;		/* 0xffff = no MIDI note assigned,  */
						/*	0xffXX = single key note assignment */
						/*	0xLLHH = key split, low/hi note */
	int		srate ;		/* sample frequency in hertz */
	int		frames ;	/* sample length in bytes or words (see rez) */
	int		lbeg ;		/* offset to start of loop in bytes or words. */
						/* set to zero if unused */
	int		lend ;		/* offset to end of loop in bytes or words. */
						/* set to sample length if unused */
	short	res1 ;		/* Reserved, MIDI keyboard split */
	short	res2 ;		/* Reserved, sample compression */
	short	res3 ;		/* Reserved */
	char	ext [20] ;	/* Additional filename space, used if (name[7] != 0) */
	char	user [64] ; /* User defined. Typically ASCII message */
} AVR_HEADER ;

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int		avr_close (SF_PRIVATE *psf) ;

static int		avr_read_header (SF_PRIVATE *psf) ;
static int		avr_write_header (SF_PRIVATE *psf, int calc_length) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
avr_open	(SF_PRIVATE *psf)
{	int		subformat ;
	int		error = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = avr_read_header (psf)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_AVR)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		psf->endian = SF_ENDIAN_BIG ;

		if (avr_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = avr_write_header ;
		} ;

	psf->close = avr_close ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	error = pcm_init (psf) ;

	return error ;
} /* avr_open */

static int
avr_read_header (SF_PRIVATE *psf)
{	AVR_HEADER	hdr ;

	memset (&hdr, 0, sizeof (hdr)) ;

	psf_binheader_readf (psf, "pmb", 0, &hdr.marker, &hdr.name, sizeof (hdr.name)) ;
	psf_log_printf (psf, "%M\n", hdr.marker) ;

	if (hdr.marker != TWOBIT_MARKER)
		return SFE_AVR_X ;

	psf_log_printf (psf, "  Name        : %s\n", hdr.name) ;

	psf_binheader_readf (psf, "E22222", &hdr.mono, &hdr.rez, &hdr.sign, &hdr.loop, &hdr.midi) ;

	psf->sf.channels = (hdr.mono & 1) + 1 ;

	psf_log_printf (psf, "  Channels    : %d\n  Bit width   : %d\n  Signed      : %s\n",
			(hdr.mono & 1) + 1, hdr.rez, hdr.sign ? "yes" : "no") ;

	switch ((hdr.rez << 16) + (hdr.sign & 1))
	{	case ((8 << 16) + 0) :
			psf->sf.format = SF_FORMAT_AVR | SF_FORMAT_PCM_U8 ;
			psf->bytewidth = 1 ;
			break ;

		case ((8 << 16) + 1) :
			psf->sf.format = SF_FORMAT_AVR | SF_FORMAT_PCM_S8 ;
			psf->bytewidth = 1 ;
			break ;

		case ((16 << 16) + 1) :
			psf->sf.format = SF_FORMAT_AVR | SF_FORMAT_PCM_16 ;
			psf->bytewidth = 2 ;
			break ;

		default :
			psf_log_printf (psf, "Error : bad rez/sign combination.\n") ;
			return SFE_AVR_X ;
			break ;
		} ;

	psf_binheader_readf (psf, "E4444", &hdr.srate, &hdr.frames, &hdr.lbeg, &hdr.lend) ;

	psf->sf.frames = hdr.frames ;
	psf->sf.samplerate = hdr.srate ;

	psf_log_printf (psf, "  Frames      : %D\n", psf->sf.frames) ;
	psf_log_printf (psf, "  Sample rate : %d\n", psf->sf.samplerate) ;

	psf_binheader_readf (psf, "E222", &hdr.res1, &hdr.res2, &hdr.res3) ;
	psf_binheader_readf (psf, "bb", hdr.ext, sizeof (hdr.ext), hdr.user, sizeof (hdr.user)) ;

	psf_log_printf (psf, "  Ext         : %s\n  User        : %s\n", hdr.ext, hdr.user) ;

	psf->endian = SF_ENDIAN_BIG ;

 	psf->dataoffset = AVR_HDR_SIZE ;
	psf->datalength = hdr.frames * (hdr.rez / 8) ;

	if (psf->fileoffset > 0)
		psf->filelength = AVR_HDR_SIZE + psf->datalength ;

	if (psf_ftell (psf) != psf->dataoffset)
		psf_binheader_readf (psf, "j", psf->dataoffset - psf_ftell (psf)) ;

	psf->close = avr_close ;

	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	if (psf->sf.frames == 0 && psf->blockwidth)
		psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;

	return 0 ;
} /* avr_read_header */

static int
avr_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int			sign, datalength ;

	if (psf->pipeoffset > 0)
		return 0 ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	/*
	** Only attempt to seek if we are not writng to a pipe. If we are
	** writing to a pipe we shouldn't be here anyway.
	*/
	if (psf->is_pipe == SF_FALSE)
		psf_fseek (psf, 0, SEEK_SET) ;

	datalength = (int) (psf->datalength & 0x7FFFFFFF) ;

	psf_binheader_writef (psf, "Emz22", TWOBIT_MARKER, (size_t) 8,
			psf->sf.channels == 2 ? 0xFFFF : 0, psf->bytewidth * 8) ;

	sign = ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_U8) ? 0 : 0xFFFF ;

	psf_binheader_writef (psf, "E222", sign, 0, 0xFFFF) ;
	psf_binheader_writef (psf, "E4444", psf->sf.samplerate, psf->sf.frames, 0, 0) ;

	psf_binheader_writef (psf, "E222zz", 0, 0, 0, (size_t) 20, (size_t) 64) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* avr_write_header */

static int
avr_close (SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		avr_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* avr_close */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 0823d454-f39a-4a28-a776-607f1ef33b52
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */


#include	<stdlib.h>
#include	<string.h>



/* 
 *  4.2 FIXED POINT IMPLEMENTATION OF THE RPE-LTP CODER 
 */

void Gsm_Coder (

	struct gsm_state	* State,

	word	* s,	/* [0..159] samples		  	IN	*/

/*
 * The RPE-LTD coder works on a frame by frame basis.  The length of
 * the frame is equal to 160 samples.  Some computations are done
 * once per frame to produce at the output of the coder the
 * LARc[1..8] parameters which are the coded LAR coefficients and 
 * also to realize the inverse filtering operation for the entire
 * frame (160 samples of signal d[0..159]).  These parts produce at
 * the output of the coder:
 */

	word	* LARc,	/* [0..7] LAR coefficients		OUT	*/

/*
 * Procedure 4.2.11 to 4.2.18 are to be executed four times per
 * frame.  That means once for each sub-segment RPE-LTP analysis of
 * 40 samples.  These parts produce at the output of the coder:
 */

	word	* Nc,	/* [0..3] LTP lag			OUT 	*/
	word	* bc,	/* [0..3] coded LTP gain		OUT 	*/
	word	* Mc,	/* [0..3] RPE grid selection		OUT     */
	word	* xmaxc,/* [0..3] Coded maximum amplitude	OUT	*/
	word	* xMc	/* [13*4] normalized RPE samples	OUT	*/
)
{
	int	k;
	word	* dp  = State->dp0 + 120;	/* [ -120...-1 ] */
	word	* dpp = dp;		/* [ 0...39 ]	 */

	word	so[160];

	Gsm_Preprocess			(State, s, so);
	Gsm_LPC_Analysis		(State, so, LARc);
	Gsm_Short_Term_Analysis_Filter	(State, LARc, so);

	for (k = 0; k <= 3; k++, xMc += 13) {

		Gsm_Long_Term_Predictor	( State,
					 so+k*40, /* d      [0..39] IN	*/
					 dp,	  /* dp  [-120..-1] IN	*/
					State->e + 5,	  /* e      [0..39] OUT	*/
					dpp,	  /* dpp    [0..39] OUT */
					 Nc++,
					 bc++);

		Gsm_RPE_Encoding	( /*-S,-*/
					State->e + 5,	/* e	  ][0..39][ IN/OUT */
					  xmaxc++, Mc++, xMc );
		/*
		 * Gsm_Update_of_reconstructed_short_time_residual_signal
		 *			( dpp, e + 5, dp );
		 */

		{ register int i;
		  for (i = 0; i <= 39; i++)
			dp[ i ] = GSM_ADD( State->e[5 + i], dpp[i] );
		}
		dp  += 40;
		dpp += 40;

	}
	(void)memcpy( (char *)State->dp0, (char *)(State->dp0 + 160),
		120 * sizeof(*State->dp0) );
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: ae8ef1b2-5a1e-4263-94cd-42b15dca81a3
*/

/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<string.h>
#include	<math.h>


static SF_FORMAT_INFO const simple_formats [] =
{
	{	SF_FORMAT_AIFF | SF_FORMAT_PCM_16,
		"AIFF (Apple/SGI 16 bit PCM)", "aiff"
		},

	{	SF_FORMAT_AIFF | SF_FORMAT_FLOAT,
		"AIFF (Apple/SGI 32 bit float)", "aifc"
		},

	{	SF_FORMAT_AIFF | SF_FORMAT_PCM_S8,
		"AIFF (Apple/SGI 8 bit PCM)", "aiff"
		},

	{	SF_FORMAT_AU | SF_FORMAT_PCM_16,
		"AU (Sun/Next 16 bit PCM)", "au"
		},

	{	SF_FORMAT_AU | SF_FORMAT_ULAW,
		"AU (Sun/Next 8-bit u-law)", "au"
		},

	{	SF_FORMAT_RAW | SF_FORMAT_VOX_ADPCM,
		"OKI Dialogic VOX ADPCM", "vox"
		},

	{	SF_FORMAT_WAV | SF_FORMAT_PCM_16,
		"WAV (Microsoft 16 bit PCM)", "wav"
		},

	{	SF_FORMAT_WAV | SF_FORMAT_FLOAT,
		"WAV (Microsoft 32 bit float)", "wav"
		},

	{	SF_FORMAT_WAV | SF_FORMAT_IMA_ADPCM,
		"WAV (Microsoft 4 bit IMA ADPCM)", "wav"
		},

	{	SF_FORMAT_WAV | SF_FORMAT_MS_ADPCM,
		"WAV (Microsoft 4 bit MS ADPCM)", "wav"
		},

	{	SF_FORMAT_WAV | SF_FORMAT_PCM_U8,
		"WAV (Microsoft 8 bit PCM)", "wav"
		},

} ; /* simple_formats */

int
psf_get_format_simple_count	(void)
{	return (sizeof (simple_formats) / sizeof (SF_FORMAT_INFO)) ;
} /* psf_get_format_simple_count */

int
psf_get_format_simple (SF_FORMAT_INFO *data)
{	int indx ;

	if (data->format < 0 || data->format >= (SIGNED_SIZEOF (simple_formats) / SIGNED_SIZEOF (SF_FORMAT_INFO)))
		return SFE_BAD_CONTROL_CMD ;

	indx = data->format ;
	memcpy (data, &(simple_formats [indx]), SIGNED_SIZEOF (SF_FORMAT_INFO)) ;

	return 0 ;
} /* psf_get_format_simple */

/*============================================================================
** Major format info.
*/

static SF_FORMAT_INFO const major_formats [] =
{
	{	SF_FORMAT_AIFF,		"AIFF (Apple/SGI)",						"aiff" 	},
	{	SF_FORMAT_AU,		"AU (Sun/NeXT)", 						"au"	},
	{	SF_FORMAT_AVR,		"AVR (Audio Visual Research)",	 		"avr"	},
	{	SF_FORMAT_HTK,		"HTK (HMM Tool Kit)",					"htk"	},
	{	SF_FORMAT_SVX,		"IFF (Amiga IFF/SVX8/SV16)",			"iff"	},
	{	SF_FORMAT_MAT4,		"MAT4 (GNU Octave 2.0 / Matlab 4.2)",	"mat"	},
	{	SF_FORMAT_MAT5,		"MAT5 (GNU Octave 2.1 / Matlab 5.0)",	"mat"	},
	{	SF_FORMAT_PAF,		"PAF (Ensoniq PARIS)", 					"paf"	},
	{	SF_FORMAT_PVF,		"PVF (Portable Voice Format)",			"pvf"	},
	{	SF_FORMAT_RAW,		"RAW (header-less)",				 	"raw"	},
	{	SF_FORMAT_SDS,		"SDS (Midi Sample Dump Standard)", 		"sds"	},
	/* Not ready for mainstream use yet.
	{	SF_FORMAT_SD2,		"SD2 (Sound Designer II)", 			"sd2"	},
	*/
	{	SF_FORMAT_IRCAM,	"SF (Berkeley/IRCAM/CARL)",				"sf"	},
	{	SF_FORMAT_VOC,		"VOC (Creative Labs)",					"voc"	},
	{	SF_FORMAT_W64,		"W64 (SoundFoundry WAVE 64)",			"w64"	},
	{	SF_FORMAT_WAV,		"WAV (Microsoft)",						"wav"	},
	{	SF_FORMAT_NIST,		"WAV (NIST Sphere)",	 				"wav"	},
	{	SF_FORMAT_WAVEX,	"WAVEX (Microsoft)",					"wav"	},
	{	SF_FORMAT_XI,		"XI (FastTracker 2)",					"xi"	},

} ; /* major_formats */

int
psf_get_format_major_count	(void)
{	return (sizeof (major_formats) / sizeof (SF_FORMAT_INFO)) ;
} /* psf_get_format_major_count */

int
psf_get_format_major (SF_FORMAT_INFO *data)
{	int indx ;

	if (data->format < 0 || data->format >= (SIGNED_SIZEOF (major_formats) / SIGNED_SIZEOF (SF_FORMAT_INFO)))
		return SFE_BAD_CONTROL_CMD ;

	indx = data->format ;
	memcpy (data, &(major_formats [indx]), SIGNED_SIZEOF (SF_FORMAT_INFO)) ;

	return 0 ;
} /* psf_get_format_major */

/*============================================================================
** Subtype format info.
*/

static SF_FORMAT_INFO subtype_formats [] =
{
	{	SF_FORMAT_PCM_S8,		"Signed 8 bit PCM",		NULL 	},
	{	SF_FORMAT_PCM_16,		"Signed 16 bit PCM",	NULL 	},
	{	SF_FORMAT_PCM_24,		"Signed 24 bit PCM",	NULL 	},
	{	SF_FORMAT_PCM_32,		"Signed 32 bit PCM",	NULL 	},

	{	SF_FORMAT_PCM_U8,		"Unsigned 8 bit PCM",	NULL 	},

	{	SF_FORMAT_FLOAT,		"32 bit float",			NULL 	},
	{	SF_FORMAT_DOUBLE,		"64 bit float",			NULL 	},

	{	SF_FORMAT_ULAW,			"U-Law",				NULL 	},
	{	SF_FORMAT_ALAW,			"A-Law",				NULL 	},
	{	SF_FORMAT_IMA_ADPCM,	"IMA ADPCM",			NULL 	},
	{	SF_FORMAT_MS_ADPCM,		"Microsoft ADPCM",		NULL 	},

	{	SF_FORMAT_GSM610,		"GSM 6.10",				NULL 	},

	{	SF_FORMAT_G721_32,		"32kbs G721 ADPCM",		NULL 	},
	{	SF_FORMAT_G723_24,		"24kbs G723 ADPCM",		NULL 	},

	{	SF_FORMAT_DWVW_12,		"12 bit DWVW",			NULL 	},
	{	SF_FORMAT_DWVW_16,		"16 bit DWVW",			NULL 	},
	{	SF_FORMAT_DWVW_24,		"24 bit DWVW",			NULL 	},
	{	SF_FORMAT_VOX_ADPCM,	"VOX ADPCM",			"vox" 	},

	{	SF_FORMAT_DPCM_16,		"16 bit DPCM",			NULL 	},
	{	SF_FORMAT_DPCM_8,		"8 bit DPCM",			NULL 	},
} ; /* subtype_formats */

int
psf_get_format_subtype_count	(void)
{	return (sizeof (subtype_formats) / sizeof (SF_FORMAT_INFO)) ;
} /* psf_get_format_subtype_count */

int
psf_get_format_subtype (SF_FORMAT_INFO *data)
{	int indx ;

	if (data->format < 0 || data->format >= (SIGNED_SIZEOF (subtype_formats) / SIGNED_SIZEOF (SF_FORMAT_INFO)))
		return SFE_BAD_CONTROL_CMD ;

	indx = data->format ;
	memcpy (data, &(subtype_formats [indx]), sizeof (SF_FORMAT_INFO)) ;

	return 0 ;
} /* psf_get_format_subtype */

/*==============================================================================
*/

int
psf_get_format_info (SF_FORMAT_INFO *data)
{	int k, format ;

	if (data->format & SF_FORMAT_TYPEMASK)
	{	format = data->format & SF_FORMAT_TYPEMASK ;

		for (k = 0 ; k < (SIGNED_SIZEOF (major_formats) / SIGNED_SIZEOF (SF_FORMAT_INFO)) ; k++)
		{	if (format == major_formats [k].format)
			{	memcpy (data, &(major_formats [k]), sizeof (SF_FORMAT_INFO)) ;
				return 0 ;
				} ;
			} ;
		}
	else if (data->format & SF_FORMAT_SUBMASK)
	{	format = data->format & SF_FORMAT_SUBMASK ;

		for (k = 0 ; k < (SIGNED_SIZEOF (subtype_formats) / SIGNED_SIZEOF (SF_FORMAT_INFO)) ; k++)
		{	if (format == subtype_formats [k].format)
			{	memcpy (data, &(subtype_formats [k]), sizeof (SF_FORMAT_INFO)) ;
				return 0 ;
				} ;
			} ;
		} ;

	memset (data, 0, sizeof (SF_FORMAT_INFO)) ;

	return SFE_BAD_CONTROL_CMD ;
} /* psf_get_format_info */

/*==============================================================================
*/

double
psf_calc_signal_max (SF_PRIVATE *psf, int normalize)
{	sf_count_t	position ;
	double 		max_val = 0.0, temp, *data ;
	int			k, len, readcount, save_state ;

	/* If the file is not seekable, there is nothing we can do. */
	if (! psf->sf.seekable)
	{	psf->error = SFE_NOT_SEEKABLE ;
		return	0.0 ;
		} ;

	if (! psf->read_double)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0.0 ;
		} ;

	save_state = sf_command ((SNDFILE*) psf, SFC_GET_NORM_DOUBLE, NULL, 0) ;

	sf_command ((SNDFILE*) psf, SFC_SET_NORM_DOUBLE, NULL, normalize) ;

	/* Brute force. Read the whole file and find the biggest sample. */
	position = sf_seek ((SNDFILE*) psf, 0, SEEK_CUR) ; /* Get current position in file */
	sf_seek ((SNDFILE*) psf, 0, SEEK_SET) ;			/* Go to start of file. */

	len = sizeof (psf->buffer) / sizeof (double) ;

	data = (double*) psf->buffer ;

	readcount = len ;
	while (readcount > 0)
	{	readcount = sf_read_double ((SNDFILE*) psf, data, len) ;
		for (k = 0 ; k < readcount ; k++)
		{	temp = fabs (data [k]) ;
			max_val = temp > max_val ? temp : max_val ;
			} ;
		} ;

	sf_seek ((SNDFILE*) psf, position, SEEK_SET) ;		/* Return to original position. */

	sf_command ((SNDFILE*) psf, SFC_SET_NORM_DOUBLE, NULL, save_state) ;

	return	max_val ;
} /* psf_calc_signal_max */

int
psf_calc_max_all_channels (SF_PRIVATE *psf, double *peaks, int normalize)
{	sf_count_t	position ;
	double 		temp, *data ;
	int			k, len, readcount, save_state ;
	int			chan ;

	/* If the file is not seekable, there is nothing we can do. */
	if (! psf->sf.seekable)
		return (psf->error = SFE_NOT_SEEKABLE) ;

	if (! psf->read_double)
		return (psf->error = SFE_UNIMPLEMENTED) ;

	save_state = sf_command ((SNDFILE*) psf, SFC_GET_NORM_DOUBLE, NULL, 0) ;
	sf_command ((SNDFILE*) psf, SFC_SET_NORM_DOUBLE, NULL, normalize) ;

	memset (peaks, 0, sizeof (double) * psf->sf.channels) ;

	/* Brute force. Read the whole file and find the biggest sample for each channel. */
	position = sf_seek ((SNDFILE*) psf, 0, SEEK_CUR) ; /* Get current position in file */
	sf_seek ((SNDFILE*) psf, 0, SEEK_SET) ;			/* Go to start of file. */

	len = sizeof (psf->buffer) / sizeof (double) ;

	data = (double*) psf->buffer ;

	chan = 0 ;
	readcount = len ;
	while (readcount > 0)
	{	readcount = sf_read_double ((SNDFILE*) psf, data, len) ;
		for (k = 0 ; k < readcount ; k++)
		{	temp = fabs (data [k]) ;
			peaks [chan] = temp > peaks [chan] ? temp : peaks [chan] ;
			chan = (chan + 1) % psf->sf.channels ;
			} ;
		} ;

	sf_seek ((SNDFILE*) psf, position, SEEK_SET) ;		/* Return to original position. */

	sf_command ((SNDFILE*) psf, SFC_SET_NORM_DOUBLE, NULL, save_state) ;

	return	0 ;
} /* psf_calc_signal_max */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 0aae0d9d-ab2b-4d70-ade3-47a534666f8e
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdarg.h>
#include	<string.h>
#include	<ctype.h>
#include	<math.h>
#include	<time.h>


/*-----------------------------------------------------------------------------------------------
** psf_log_printf allows libsndfile internal functions to print to an internal logbuffer which
** can later be displayed.
** The format specifiers are as for printf but without the field width and other modifiers.
** Printing is performed to the logbuffer char array of the SF_PRIVATE struct.
** Printing is done in such a way as to guarantee that the log never overflows the end of the
** logbuffer array.
*/

#define LOG_PUTCHAR(a,b)									\
		{	if ((a)->logindex < SIGNED_SIZEOF ((a)->logbuffer) - 1)\
			{	(a)->logbuffer [(a)->logindex++] = (b) ;	\
				(a)->logbuffer [(a)->logindex] = 0 ;		\
				}											\
			}

void
psf_log_printf (SF_PRIVATE *psf, const char *format, ...)
{	va_list			ap ;
	unsigned int	u ;
	int				d, tens, shift, width, width_specifier, left_align ;
	char			c, *strptr, istr [5], lead_char, sign_char ;

	va_start (ap, format) ;

	while ((c = *format++))
	{	if (c != '%')
		{	LOG_PUTCHAR (psf, c) ;
			continue ;
			} ;

		if (format [0] == '%') /* Handle %% */
		{ 	LOG_PUTCHAR (psf, '%') ;
			format ++ ;
			continue ;
			} ;

		sign_char = 0 ;
		left_align = SF_FALSE ;
		while (1)
		{	switch (format [0])
			{	case ' ' :
				case '+' :
					sign_char = format [0] ;
					format ++ ;
					continue ;

				case '-' :
					left_align = SF_TRUE ;
					format ++ ;
					continue ;

				default : break ;
				} ;

			break ;
			} ;

		if (format [0] == 0)
			break ;

		lead_char = ' ' ;
		if (format [0] == '0')
			lead_char = '0' ;

		width_specifier = 0 ;
		while ((c = *format++) && isdigit (c))
			width_specifier = width_specifier * 10 + (c - '0') ;

		switch (c)
		{	case 0 : /* NULL character. */
					va_end (ap) ;
					return ;

			case 's': /* string */
					strptr = va_arg (ap, char *) ;
					if (strptr == NULL)
						break ;
					width_specifier -= strlen (strptr) ;
					if (left_align == SF_FALSE)
						while (width_specifier -- > 0)
							LOG_PUTCHAR (psf, ' ') ;
					while (*strptr)
						LOG_PUTCHAR (psf, *strptr++) ;
					while (width_specifier -- > 0)
						LOG_PUTCHAR (psf, ' ') ;
					break ;

			case 'd': /* int */
					d = va_arg (ap, int) ;

					if (d < 0)
					{	d = -d ;
						sign_char = '-' ;
						if (lead_char != '0' && left_align == SF_FALSE)
							width_specifier -- ;
						} ;

					tens = 1 ;
					width = 1 ;
					while (d / tens >= 10)
					{	tens *= 10 ;
						width ++ ;
						} ;

					width_specifier -= width ;

					if (sign_char == ' ')
					{	LOG_PUTCHAR (psf, ' ') ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE && lead_char != '0')
					{	if (sign_char == '+')
							width_specifier -- ;

						while (width_specifier -- > 0)
							LOG_PUTCHAR (psf, lead_char) ;
						} ;

					if (sign_char == '+' || sign_char == '-')
					{	LOG_PUTCHAR (psf, sign_char) ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE)
						while (width_specifier -- > 0)
							LOG_PUTCHAR (psf, lead_char) ;

					while (tens > 0)
					{	LOG_PUTCHAR (psf, '0' + d / tens) ;
						d %= tens ;
						tens /= 10 ;
						} ;

					while (width_specifier -- > 0)
						LOG_PUTCHAR (psf, lead_char) ;
					break ;

			case 'D': /* sf_count_t */
					{	sf_count_t		D, Tens ;

						D = va_arg (ap, sf_count_t) ;

						if (D == 0)
						{	while (-- width_specifier > 0)
								LOG_PUTCHAR (psf, lead_char) ;
							LOG_PUTCHAR (psf, '0') ;
							break ;
							}
						if (D < 0)
						{	LOG_PUTCHAR (psf, '-') ;
							D = -D ;
							} ;
						Tens = 1 ;
						width = 1 ;
						while (D / Tens >= 10)
						{	Tens *= 10 ;
							width ++ ;
							} ;

						while (width_specifier > width)
						{	LOG_PUTCHAR (psf, lead_char) ;
							width_specifier-- ;
							} ;

						while (Tens > 0)
						{	LOG_PUTCHAR (psf, '0' + D / Tens) ;
							D %= Tens ;
							Tens /= 10 ;
							} ;
						} ;
					break ;

			case 'u': /* unsigned int */
					u = va_arg (ap, unsigned int) ;

					tens = 1 ;
					width = 1 ;
					while (u / tens >= 10)
					{	tens *= 10 ;
						width ++ ;
						} ;

					width_specifier -= width ;

					if (sign_char == ' ')
					{	LOG_PUTCHAR (psf, ' ') ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE && lead_char != '0')
					{	if (sign_char == '+')
							width_specifier -- ;

						while (width_specifier -- > 0)
							LOG_PUTCHAR (psf, lead_char) ;
						} ;

					if (sign_char == '+' || sign_char == '-')
					{	LOG_PUTCHAR (psf, sign_char) ;
						width_specifier -- ;
						} ;

					if (left_align == SF_FALSE)
						while (width_specifier -- > 0)
							LOG_PUTCHAR (psf, lead_char) ;

					while (tens > 0)
					{	LOG_PUTCHAR (psf, '0' + u / tens) ;
						u %= tens ;
						tens /= 10 ;
						} ;

					while (width_specifier -- > 0)
						LOG_PUTCHAR (psf, lead_char) ;
					break ;

			case 'c': /* char */
					c = va_arg (ap, int) & 0xFF ;
					LOG_PUTCHAR (psf, c) ;
					break ;

			case 'X': /* hex */
					d = va_arg (ap, int) ;

					if (d == 0)
					{	while (--width_specifier > 0)
							LOG_PUTCHAR (psf, lead_char) ;
						LOG_PUTCHAR (psf, '0') ;
						break ;
						} ;
					shift = 28 ;
					width = (width_specifier < 8) ? 8 : width_specifier ;
					while (! ((0xF << shift) & d))
					{	shift -= 4 ;
						width -- ;
						} ;

					while (width > 0 && width_specifier > width)
					{	LOG_PUTCHAR (psf, lead_char) ;
						width_specifier-- ;
						} ;

					while (shift >= 0)
					{	c = (d >> shift) & 0xF ;
						LOG_PUTCHAR (psf, (c > 9) ? c + 'A' - 10 : c + '0') ;
						shift -= 4 ;
						} ;
					break ;

			case 'M': /* int2str */
					d = va_arg (ap, int) ;
					if (CPU_IS_LITTLE_ENDIAN)
					{	istr [0] = d & 0xFF ;
						istr [1] = (d >> 8) & 0xFF ;
						istr [2] = (d >> 16) & 0xFF ;
						istr [3] = (d >> 24) & 0xFF ;
						}
					else
					{	istr [3] = d & 0xFF ;
						istr [2] = (d >> 8) & 0xFF ;
						istr [1] = (d >> 16) & 0xFF ;
						istr [0] = (d >> 24) & 0xFF ;
						} ;
					istr [4] = 0 ;
					strptr = istr ;
					while (*strptr)
					{	c = *strptr++ ;
						LOG_PUTCHAR (psf, c) ;
						} ;
					break ;

			default :
					LOG_PUTCHAR (psf, '*') ;
					LOG_PUTCHAR (psf, c) ;
					LOG_PUTCHAR (psf, '*') ;
					break ;
			} /* switch */
		} /* while */

	va_end (ap) ;
	return ;
} /* psf_log_printf */

#ifndef PSF_LOG_PRINTF_ONLY
/*-----------------------------------------------------------------------------------------------
**  ASCII header printf functions.
**  Some formats (ie NIST) use ascii text in their headers.
**  Format specifiers are the same as the standard printf specifiers (uses vsnprintf).
**  If this generates a compile error on any system, the author should be notified
**  so an alternative vsnprintf can be provided.
*/

void
psf_asciiheader_printf (SF_PRIVATE *psf, const char *format, ...)
{	va_list	argptr ;
	int		maxlen ;
	char	*start ;

	maxlen = strlen ((char*) psf->header) ;
	start	= ((char*) psf->header) + maxlen ;
	maxlen	= sizeof (psf->header) - maxlen ;

	va_start (argptr, format) ;
	LSF_VSNPRINTF (start, maxlen, format, argptr) ;
	va_end (argptr) ;

	/* Make sure the string is properly terminated. */
	start [maxlen - 1] = 0 ;

	psf->headindex = strlen ((char*) psf->header) ;

	return ;
} /* psf_asciiheader_printf */

/*-----------------------------------------------------------------------------------------------
**  Binary header writing functions. Returns number of bytes written.
**
**  Format specifiers for psf_binheader_writef are as follows
**		m	- marker - four bytes - no endian manipulation
**
**		e   - all following numerical values will be little endian
**		E   - all following numerical values will be big endian
**
**		t   - all following O types will be truncated to 4 bytes
**		T   - switch off truncation of all following O types
**
**		1	- single byte value
**		2	- two byte value
**		3	- three byte value
**		4	- four byte value
**		8	- eight byte value (sometimes written as 4 bytes)
**
**		s   - string preceded by a four byte length
**		S   - string including null terminator
**		f	- floating point data
**		d	- double precision floating point data
**		h	- 16 binary bytes value
**
**		b	- binary data (see below)
**		z   - zero bytes (ses below)
**		j	- jump forwards or backwards
**
**	To write a word followed by an int (both little endian) use:
**		psf_binheader_writef ("e24", wordval, longval) ;
**
**	To write binary data use:
**		psf_binheader_writef ("b", &bindata, sizeof (bindata)) ;
**
**	To write N zero bytes use:
**		psf_binheader_writef ("z", N) ;
*/

/* These macros may seem a bit messy but do prevent problems with processors which
** seg. fault when asked to write an int or short to a non-int/short aligned address.
*/

#define	PUT_BYTE(psf,x)		if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 1)	\
							{ (psf)->header [(psf)->headindex++] = (x) ; }

#if (CPU_IS_BIG_ENDIAN == 1)
#define	PUT_MARKER(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 4)	\
							{	(psf)->header [(psf)->headindex++] = ((x) >> 24) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = (x) ;	}

#elif (CPU_IS_LITTLE_ENDIAN == 1)
#define	PUT_MARKER(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 4)	\
							{	(psf)->header [(psf)->headindex++] = (x) ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 24) ;	}

#else
#	error "Cannot determine endian-ness of processor."
#endif


#define	PUT_BE_SHORT(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 2)	\
							{	(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = (x) ; 		}

#define	PUT_LE_SHORT(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 2)	\
							{	(psf)->header [(psf)->headindex++] = (x) ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;	}

#define	PUT_BE_3BYTE(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 3)	\
							{	(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = (x) ;		}

#define	PUT_LE_3BYTE(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 3)	\
							{	(psf)->header [(psf)->headindex++] = (x) ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;	}

#define	PUT_BE_INT(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 4)	\
							{	(psf)->header [(psf)->headindex++] = ((x) >> 24) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = (x) ;		}

#define	PUT_LE_INT(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 4)	\
							{	(psf)->header [(psf)->headindex++] = (x) ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 24) ;	}

#if (SIZEOF_SF_COUNT_T == 4)
#define	PUT_BE_8BYTE(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 8)	\
							{	(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 24) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = (x) ;		}

#define	PUT_LE_8BYTE(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 8)	\
							{	(psf)->header [(psf)->headindex++] = (x) ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 24) ;		\
								(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = 0 ;				\
								(psf)->header [(psf)->headindex++] = 0 ;	}

#elif (SIZEOF_SF_COUNT_T == 8)
#define	PUT_BE_8BYTE(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 8)	\
							{	(psf)->header [(psf)->headindex++] = ((x) >> 56) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 48) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 40) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 32) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 24) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = (x) ;		}

#define	PUT_LE_8BYTE(psf,x)	if ((psf)->headindex < SIGNED_SIZEOF ((psf)->header) - 8)	\
							{	(psf)->header [(psf)->headindex++] = (x) ;				\
								(psf)->header [(psf)->headindex++] = ((x) >> 8) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 16) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 24) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 32) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 40) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 48) ;		\
								(psf)->header [(psf)->headindex++] = ((x) >> 56) ;	}
#else
#error "SIZEOF_SF_COUNT_T is not defined."
#endif

int
psf_binheader_writef (SF_PRIVATE *psf, const char *format, ...)
{	va_list	argptr ;
	sf_count_t 		countdata ;
	unsigned long 	longdata ;
	unsigned int 	data ;
	float			floatdata ;
	double			doubledata ;
	void			*bindata ;
	size_t			size ;
	char			c, *strptr ;
	int				count = 0, trunc_8to4 ;

	trunc_8to4 = SF_FALSE ;

	va_start (argptr, format) ;

	while ((c = *format++))
	{	switch (c)
		{	case 'e' : /* All conversions are now from LE to host. */
					psf->rwf_endian = SF_ENDIAN_LITTLE ;
					break ;

			case 'E' : /* All conversions are now from BE to host. */
					psf->rwf_endian = SF_ENDIAN_BIG ;
					break ;

			case 't' : /* All 8 byte values now get written as 4 bytes. */
					trunc_8to4 = SF_TRUE ;
					break ;

			case 'T' : /* All 8 byte values now get written as 8 bytes. */
					trunc_8to4 = SF_FALSE ;
					break ;

			case 'm' :
					data = va_arg (argptr, unsigned int) ;
					PUT_MARKER (psf, data) ;
					count += 4 ;
					break ;

			case '1' :
					data = va_arg (argptr, unsigned int) ;
					PUT_BYTE (psf, data) ;
					count += 1 ;
					break ;

			case '2' :
					data = va_arg (argptr, unsigned int) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
					{	PUT_BE_SHORT (psf, data) ;
						}
					else
					{	PUT_LE_SHORT (psf, data) ;
						} ;
					count += 2 ;
					break ;

			case '3' : /* tribyte */
					data = va_arg (argptr, unsigned int) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
					{	PUT_BE_3BYTE (psf, data) ;
						}
					else
					{	PUT_LE_3BYTE (psf, data) ;
						} ;
					count += 3 ;
					break ;

			case '4' :
					data = va_arg (argptr, unsigned int) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
					{	PUT_BE_INT (psf, data) ;
						}
					else
					{	PUT_LE_INT (psf, data) ;
						} ;
					count += 4 ;
					break ;

			case '8' :
					countdata = va_arg (argptr, sf_count_t) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG && trunc_8to4 == SF_FALSE)
					{	PUT_BE_8BYTE (psf, countdata) ;
						count += 8 ;
						}
					else if (psf->rwf_endian == SF_ENDIAN_LITTLE && trunc_8to4 == SF_FALSE)
					{	PUT_LE_8BYTE (psf, countdata) ;
						count += 8 ;
						}
					else if (psf->rwf_endian == SF_ENDIAN_BIG && trunc_8to4 == SF_TRUE)
					{	longdata = countdata & 0xFFFFFFFF ;
						PUT_BE_INT (psf, longdata) ;
						count += 4 ;
						}
					else if (psf->rwf_endian == SF_ENDIAN_LITTLE && trunc_8to4 == SF_TRUE)
					{	longdata = countdata & 0xFFFFFFFF ;
						PUT_LE_INT (psf, longdata) ;
						count += 4 ;
						}
					break ;

			case 'f' :
					/* Floats are passed as doubles. Is this always true? */
					floatdata = (float) va_arg (argptr, double) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						float32_be_write (floatdata, psf->header + psf->headindex) ;
					else
						float32_le_write (floatdata, psf->header + psf->headindex) ;
					psf->headindex += 4 ;
					count += 4 ;
					break ;

			case 'd' :
					doubledata = va_arg (argptr, double) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						double64_be_write (doubledata, psf->header + psf->headindex) ;
					else
						double64_le_write (doubledata, psf->header + psf->headindex) ;
					psf->headindex += 8 ;
					count += 8 ;
					break ;

			case 's' :
					strptr = va_arg (argptr, char *) ;
					size = strlen (strptr) + 1 ;
					size += (size & 1) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
					{	PUT_BE_INT (psf, size) ;
						}
					else
					{	PUT_LE_INT (psf, size) ;
						} ;
					memcpy (&(psf->header [psf->headindex]), strptr, size) ;
					psf->headindex += size ;
					psf->header [psf->headindex - 1] = 0 ;
					count += 4 + size ;
					break ;

			case 'S' :
					strptr	= va_arg (argptr, char *) ;
					size	= strlen (strptr) + 1 ;
					memcpy (&(psf->header [psf->headindex]), strptr, size) ;
					psf->headindex += size ;
					count += size ;
					break ;

			case 'b' :
					bindata	= va_arg (argptr, void *) ;
					size	= va_arg (argptr, size_t) ;
					memcpy (&(psf->header [psf->headindex]), bindata, size) ;
					psf->headindex += size ;
					count += size ;
					break ;

			case 'z' :
					size = va_arg (argptr, size_t) ;
					count += size ;
					while (size)
					{	psf->header [psf->headindex] = 0 ;
						psf->headindex ++ ;
						size -- ;
						} ;
					break ;

			case 'h' :
					bindata = va_arg (argptr, void *) ;
					memcpy (&(psf->header [psf->headindex]), bindata, 16) ;
					psf->headindex += 16 ;
					count += 16 ;
					break ;

			case 'j' :
					size = va_arg (argptr, int) ;
					psf->headindex += size ;
					count = size ;
					break ;

			default :
				psf_log_printf (psf, "*** Invalid format specifier `%c'\n", c) ;
				psf->error = SFE_INTERNAL ;
				break ;
			} ;
		} ;

	va_end (argptr) ;
	return count ;
} /* psf_binheader_writef */

/*-----------------------------------------------------------------------------------------------
**  Binary header reading functions. Returns number of bytes read.
**
**	Format specifiers are the same as for header write function above with the following
**	additions:
**
**		p   - jump a given number of position from start of file.
**
**	If format is NULL, psf_binheader_readf returns the current offset.
*/

#if (CPU_IS_BIG_ENDIAN == 1)
#define	GET_MARKER(ptr)	(	((ptr) [0] << 24)	| ((ptr) [1] << 16) |	\
							((ptr) [2] << 8)	| ((ptr) [3]) )

#elif (CPU_IS_LITTLE_ENDIAN == 1)
#define	GET_MARKER(ptr)	(	((ptr) [0])			| ((ptr) [1] << 8) |	\
							((ptr) [2] << 16)	| ((ptr) [3] << 24) )

#else
#	error "Cannot determine endian-ness of processor."
#endif

#define	GET_LE_SHORT(ptr)	( ((ptr) [1] << 8) | ((ptr) [0]) )
#define	GET_BE_SHORT(ptr)	( ((ptr) [0] << 8) | ((ptr) [1]) )

#define	GET_LE_3BYTE(ptr)	( 	((ptr) [2] << 16) | ((ptr) [1] << 8) | ((ptr) [0]) )
#define	GET_BE_3BYTE(ptr)	( 	((ptr) [0] << 16) | ((ptr) [1] << 8) | ((ptr) [2]) )

#define	GET_LE_INT(ptr)		( 	((ptr) [3] << 24)	| ((ptr) [2] << 16) |	\
								((ptr) [1] << 8)	| ((ptr) [0]) )

#define	GET_BE_INT(ptr)		( 	((ptr) [0] << 24)	| ((ptr) [1] << 16) |	\
							 	((ptr) [2] << 8)	| ((ptr) [3]) )

#if (SIZEOF_LONG == 4)
#define	GET_LE_8BYTE(ptr)	( 	((ptr) [3] << 24)	| ((ptr) [2] << 16) |	\
							 	((ptr) [1] << 8)	| ((ptr) [0]) )

#define	GET_BE_8BYTE(ptr)	( 	((ptr) [4] << 24)	| ((ptr) [5] << 16) |	\
								((ptr) [6] << 8)	| ((ptr) [7]) )
#else
#define	GET_LE_8BYTE(ptr)	( 	(((ptr) [7] * 1L) << 56) | (((ptr) [6] * 1L) << 48) |	\
							 	(((ptr) [5] * 1L) << 40) | (((ptr) [4] * 1L) << 32) |	\
							 	(((ptr) [3] * 1L) << 24) | (((ptr) [2] * 1L) << 16) |	\
							 	(((ptr) [1] * 1L) << 8 ) | ((ptr) [0]))

#define	GET_BE_8BYTE(ptr)	( 	(((ptr) [0] * 1L) << 56) | (((ptr) [1] * 1L) << 48) |	\
							 	(((ptr) [2] * 1L) << 40) | (((ptr) [3] * 1L) << 32) |	\
							 	(((ptr) [4] * 1L) << 24) | (((ptr) [5] * 1L) << 16) |	\
							 	(((ptr) [6] * 1L) << 8 ) | ((ptr) [7]))

#endif

static int
header_read (SF_PRIVATE *psf, void *ptr, int bytes)
{	int count = 0 ;

	if (psf->headindex + bytes > SIGNED_SIZEOF (psf->header))
	{	if (psf->headend < SIGNED_SIZEOF (psf->header))
			psf_log_printf (psf, "Warning : Further header read would overflow buffer.\n") ;
		psf->headend = SIGNED_SIZEOF (psf->header) ;

		/* This is the best that we can do. */
		return psf_fread (ptr, 1, bytes, psf) ;
		} ;

	if (psf->headindex + bytes > psf->headend)
	{	count = psf_fread (psf->header + psf->headend, 1, bytes - (psf->headend - psf->headindex), psf) ;
		if (count != bytes - (int) (psf->headend - psf->headindex))
		{	psf_log_printf (psf, "Error : psf_fread returned short count.\n") ;
			return 0 ;
			} ;
		psf->headend += count ;
		} ;

	memcpy (ptr, psf->header + psf->headindex, bytes) ;
	psf->headindex += bytes ;

	return bytes ;
} /* header_read */

static void
header_seek (SF_PRIVATE *psf, sf_count_t position, int whence)
{

	switch (whence)
	{	case SEEK_SET :
			if (position > SIGNED_SIZEOF (psf->header))
			{	/* Too much header to cache so just seek instead. */
				psf_fseek (psf, position, whence) ;
				return ;
				} ;
			if (position > psf->headend)
				psf->headend += psf_fread (psf->header + psf->headend, 1, position - psf->headend, psf) ;
			psf->headindex = position ;
			break ;

		case SEEK_CUR :
			if (psf->headindex + position < 0)
				break ;

			if (psf->headindex >= SIGNED_SIZEOF (psf->header))
			{	psf_fseek (psf, position, whence) ;
				return ;
				} ;

			if (psf->headindex + position <= psf->headend)
			{	psf->headindex += position ;
				break ;
				} ;

			if (psf->headindex + position > SIGNED_SIZEOF (psf->header))
			{	/* Need to jump this without caching it. */
				psf->headindex = psf->headend ;
				psf_fseek (psf, position, SEEK_CUR) ;
				break ;
				} ;

			psf->headend += psf_fread (psf->header + psf->headend, 1, position - (psf->headend - psf->headindex), psf) ;
			psf->headindex = psf->headend ;
			break ;

		case SEEK_END :
		default :
			psf_log_printf (psf, "Bad whence param in header_seek().\n") ;
			break ;
		} ;

	return ;
} /* header_seek */

static int
header_gets (SF_PRIVATE *psf, char *ptr, int bufsize)
{
	int		k ;

	for (k = 0 ; k < bufsize - 1 ; k++)
	{	if (psf->headindex < psf->headend)
		{	ptr [k] = psf->header [psf->headindex] ;
			psf->headindex ++ ;
			}
		else
		{	psf->headend += psf_fread (psf->header + psf->headend, 1, 1, psf) ;
			ptr [k] = psf->header [psf->headindex] ;
			psf->headindex = psf->headend ;
			} ;

		if (ptr [k] == '\n')
			break ;
		} ;

	ptr [k] = 0 ;

	return k ;
} /* header_gets */

int
psf_binheader_readf (SF_PRIVATE *psf, char const *format, ...)
{	va_list			argptr ;
	sf_count_t		*countptr, countdata ;
	unsigned char	*ucptr, sixteen_bytes [16] ;
	unsigned int 	*intptr, intdata ;
	unsigned short	*shortptr ;
	char			*charptr ;
	float			*floatptr ;
	double			*doubleptr ;
	char			c ;
	int				byte_count = 0, count ;

	if (! format)
		return psf_ftell (psf) ;

	va_start (argptr, format) ;

	while ((c = *format++))
	{	switch (c)
		{	case 'e' : /* All conversions are now from LE to host. */
					psf->rwf_endian = SF_ENDIAN_LITTLE ;
					break ;

			case 'E' : /* All conversions are now from BE to host. */
					psf->rwf_endian = SF_ENDIAN_BIG ;
					break ;

			case 'm' :
					intptr = va_arg (argptr, unsigned int*) ;
					ucptr = (unsigned char*) intptr ;
					byte_count += header_read (psf, ucptr, sizeof (int)) ;
					*intptr = GET_MARKER (ucptr) ;
					break ;

			case 'h' :
					intptr = va_arg (argptr, unsigned int*) ;
					ucptr = (unsigned char*) intptr ;
					byte_count += header_read (psf, sixteen_bytes, sizeof (sixteen_bytes)) ;
					{	int k ;
						intdata = 0 ;
						for (k = 0 ; k < 16 ; k++)
							intdata ^= sixteen_bytes [k] << k ;
						}
					*intptr = intdata ;
					break ;

			case '1' :
					charptr = va_arg (argptr, char*) ;
					byte_count += header_read (psf, charptr, sizeof (char)) ;
					break ;

			case '2' :
					shortptr = va_arg (argptr, unsigned short*) ;
					ucptr = (unsigned char*) shortptr ;
					byte_count += header_read (psf, ucptr, sizeof (short)) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						*shortptr = GET_BE_SHORT (ucptr) ;
					else
						*shortptr = GET_LE_SHORT (ucptr) ;
					break ;

			case '3' :
					intptr = va_arg (argptr, unsigned int*) ;
					byte_count += header_read (psf, sixteen_bytes, 3) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						*intptr = GET_BE_3BYTE (sixteen_bytes) ;
					else
						*intptr = GET_LE_3BYTE (sixteen_bytes) ;
					break ;

			case '4' :
					intptr = va_arg (argptr, unsigned int*) ;
					ucptr = (unsigned char*) intptr ;
					byte_count += header_read (psf, ucptr, sizeof (int)) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						*intptr = GET_BE_INT (ucptr) ;
					else
						*intptr = GET_LE_INT (ucptr) ;
					break ;

			case '8' :
					countptr = va_arg (argptr, sf_count_t*) ;
					byte_count += header_read (psf, sixteen_bytes, 8) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						countdata = GET_BE_8BYTE (sixteen_bytes) ;
					else
						countdata = GET_LE_8BYTE (sixteen_bytes) ;
					*countptr = countdata ;
					break ;

			case 'f' : /* Float conversion */
					floatptr = va_arg (argptr, float *) ;
					*floatptr = 0.0 ;
					byte_count += header_read (psf, floatptr, sizeof (float)) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						*floatptr = float32_be_read ((unsigned char*) floatptr) ;
					else
						*floatptr = float32_le_read ((unsigned char*) floatptr) ;
					break ;

			case 'd' : /* double conversion */
					doubleptr = va_arg (argptr, double *) ;
					*doubleptr = 0.0 ;
					byte_count += header_read (psf, doubleptr, sizeof (double)) ;
					if (psf->rwf_endian == SF_ENDIAN_BIG)
						*doubleptr = double64_be_read ((unsigned char*) doubleptr) ;
					else
						*doubleptr = double64_le_read ((unsigned char*) doubleptr) ;
					break ;

			case 's' :
					psf_log_printf (psf, "Format conversion 's' not implemented yet.\n") ;
					/*
					strptr = va_arg (argptr, char *) ;
					size   = strlen (strptr) + 1 ;
					size  += (size & 1) ;
					longdata = H2LE_INT (size) ;
					get_int (psf, longdata) ;
					memcpy (&(psf->header [psf->headindex]), strptr, size) ;
					psf->headindex += size ;
					*/
					break ;

			case 'b' :
					charptr = va_arg (argptr, char*) ;
					count = va_arg (argptr, int) ;
					if (count > 0)
						byte_count += header_read (psf, charptr, count) ;
					break ;

			case 'G' :
					charptr = va_arg (argptr, char*) ;
					count = va_arg (argptr, int) ;
					if (count > 0)
						byte_count += header_gets (psf, charptr, count) ;
					break ;

			case 'z' :
					psf_log_printf (psf, "Format conversion 'z' not implemented yet.\n") ;
					/*
					size    = va_arg (argptr, size_t) ;
					while (size)
					{	psf->header [psf->headindex] = 0 ;
						psf->headindex ++ ;
						size -- ;
						} ;
					*/
					break ;

			case 'p' :
					/* Get the seek position first. */
					count = va_arg (argptr, int) ;
					header_seek (psf, count, SEEK_SET) ;
					byte_count = count ;
					break ;

			case 'j' :
					/* Get the seek position first. */
					count = va_arg (argptr, int) ;
					header_seek (psf, count, SEEK_CUR) ;
					byte_count += count ;
					break ;

			default :
				psf_log_printf (psf, "*** Invalid format specifier `%c'\n", c) ;
				psf->error = SFE_INTERNAL ;
				break ;
			} ;
		} ;

	va_end (argptr) ;

	return byte_count ;
} /* psf_binheader_readf */

/*-----------------------------------------------------------------------------------------------
*/

sf_count_t
psf_default_seek (SF_PRIVATE *psf, int mode, sf_count_t samples_from_start)
{	sf_count_t position, retval ;

	if (! (psf->blockwidth && psf->dataoffset >= 0))
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	if (! psf->sf.seekable)
	{	psf->error = SFE_NOT_SEEKABLE ;
		return	((sf_count_t) -1) ;
		} ;

	position = psf->dataoffset + psf->blockwidth * samples_from_start ;

	if ((retval = psf_fseek (psf, position, SEEK_SET)) != position)
	{	psf->error = SFE_SEEK_FAILED ;
		return ((sf_count_t) -1) ;
		} ;

	mode = mode ;

	return samples_from_start ;
} /* psf_default_seek */

/*-----------------------------------------------------------------------------------------------
*/

void
psf_hexdump (void *ptr, int len)
{	char	ascii [17], *data ;
	int		k, m ;

	if ((data = ptr) == NULL)
		return ;
	if (len <= 0)
		return ;

	puts ("") ;
	for (k = 0 ; k < len ; k += 16)
	{	memset (ascii, ' ', sizeof (ascii)) ;

		printf ("%08X: ", k) ;
		for (m = 0 ; m < 16 && k + m < len ; m++)
		{	printf (m == 8 ? " %02X " : "%02X ", data [k + m] & 0xFF) ;
			ascii [m] = isprint (data [k + m]) ? data [k + m] : '.' ;
			} ;

		if (m <= 8) printf (" ") ;
		for ( ; m < 16 ; m++) printf ("   ") ;

		ascii [16] = 0 ;
		printf (" %s\n", ascii) ;
		} ;

	puts ("") ;
} /* psf_hexdump */

void
psf_log_SF_INFO (SF_PRIVATE *psf)
{	psf_log_printf (psf, "---------------------------------\n") ;

	psf_log_printf (psf, " Sample rate :   %d\n", psf->sf.samplerate) ;
	psf_log_printf (psf, " Frames      :   %C\n", psf->sf.frames) ;
	psf_log_printf (psf, " Channels    :   %d\n", psf->sf.channels) ;

	psf_log_printf (psf, " Format      :   0x%X\n", psf->sf.format) ;
	psf_log_printf (psf, " Sections    :   %d\n", psf->sf.sections) ;
	psf_log_printf (psf, " Seekable    :   %s\n", psf->sf.seekable ? "TRUE" : "FALSE") ;

	psf_log_printf (psf, "---------------------------------\n") ;
} /* psf_dump_SFINFO */

/*========================================================================================
*/

void*
psf_memset (void *s, int c, sf_count_t len)
{	char	*ptr ;
	int 	setcount ;

	ptr = (char *) s ;

	while (len > 0)
	{	setcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		memset (ptr, c, setcount) ;

		ptr += setcount ;
		len -= setcount ;
		} ;

	return s ;
} /* psf_memset */

void psf_get_date_str (char *str, int maxlen)
{	time_t		current ;
	struct tm	timedata, *tmptr ;

	time (&current) ;

#if defined (HAVE_GMTIME_R)
	/* If the re-entrant version is available, use it. */
	tmptr = gmtime_r (&current, &timedata) ;
#elif defined (HAVE_GMTIME)
	/* Otherwise use the standard one and copy the data to local storage. */
	tmptr = gmtime (&current) ;
	memcpy (&timedata, tmptr, sizeof (timedata)) ;
#else
	tmptr = NULL ;
#endif

	if (tmptr)
		LSF_SNPRINTF (str, maxlen, "%4d-%02d-%02d %02d:%02d:%02d UTC",
			1900 + timedata.tm_year, timedata.tm_mon, timedata.tm_mday,
			timedata.tm_hour, timedata.tm_min, timedata.tm_sec) ;
	else
		LSF_SNPRINTF (str, maxlen, "Unknown date") ;

	return ;
} /* psf_get_date_str */

int
subformat_to_bytewidth (int format)
{
	switch (format)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_S8 :
				return 1 ;
		case SF_FORMAT_PCM_16 :
				return 2 ;
		case SF_FORMAT_PCM_24 :
				return 3 ;
		case SF_FORMAT_PCM_32 :
		case SF_FORMAT_FLOAT :
				return 4 ;
		case SF_FORMAT_DOUBLE :
				return 8 ;
		} ;

	return 0 ;
} /* subformat_to_bytewidth */

int
s_bitwidth_to_subformat (int bits)
{	static int array [] =
	{	SF_FORMAT_PCM_S8, SF_FORMAT_PCM_16, SF_FORMAT_PCM_24, SF_FORMAT_PCM_32
		} ;

	if (bits < 8 || bits > 32)
		return 0 ;

	return array [((bits + 7) / 8) - 1] ;
} /* bitwidth_to_subformat */

int
u_bitwidth_to_subformat (int bits)
{	static int array [] =
	{	SF_FORMAT_PCM_U8, SF_FORMAT_PCM_16, SF_FORMAT_PCM_24, SF_FORMAT_PCM_32
		} ;

	if (bits < 8 || bits > 32)
		return 0 ;

	return array [((bits + 7) / 8) - 1] ;
} /* bitwidth_to_subformat */

#endif /* PSF_LOG_PRINTF_ONLY */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 33e9795e-f717-461a-9feb-65d083a56395
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include <stdio.h>


/*
 *  4.3 FIXED POINT IMPLEMENTATION OF THE RPE-LTP DECODER
 */

static void Postprocessing (
	struct gsm_state	* S,
	register word 		* s)
{
	register int		k;
	register word		msr = S->msr;
	register word		tmp;

	for (k = 160; k--; s++) {
		tmp = GSM_MULT_R( msr, 28180 );
		msr = GSM_ADD(*s, tmp);  	   /* Deemphasis 	     */
		*s  = GSM_ADD(msr, msr) & 0xFFF8;  /* Truncation & Upscaling */
	}
	S->msr = msr;
}

void Gsm_Decoder (
	struct gsm_state	* S,

	word		* LARcr,	/* [0..7]		IN	*/

	word		* Ncr,		/* [0..3] 		IN 	*/
	word		* bcr,		/* [0..3]		IN	*/
	word		* Mcr,		/* [0..3] 		IN 	*/
	word		* xmaxcr,	/* [0..3]		IN 	*/
	word		* xMcr,		/* [0..13*4]		IN	*/

	word		* s)		/* [0..159]		OUT 	*/
{
	int		j, k;
	word		erp[40], wt[160];
	word		* drp = S->dp0 + 120;

	for (j=0; j <= 3; j++, xmaxcr++, bcr++, Ncr++, Mcr++, xMcr += 13) {

		Gsm_RPE_Decoding( /*-S,-*/ *xmaxcr, *Mcr, xMcr, erp );
		Gsm_Long_Term_Synthesis_Filtering( S, *Ncr, *bcr, erp, drp );

		for (k = 0; k <= 39; k++) wt[ j * 40 + k ] =  drp[ k ];
	}

	Gsm_Short_Term_Synthesis_Filter( S, LARcr, wt, s );
	Postprocessing(S, s);
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 11ae5b90-2e8b-400b-ac64-a69a1fc6cc41
*/

/*
** Copyright (C) 2003,2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdlib.h>


/*============================================================================
**	Rule number 1 is to only apply dither when going from a larger bitwidth
**	to a smaller bitwidth. This can happen on both read and write.
**
**	Need to apply dither on all conversions marked X below.
**
**	Dither on write:
**
**										Input
**					|	short		int			float		double
**			--------+-----------------------------------------------
**		O	8 bit	|	X			X			X			X
**		u	16 bit	|	none		X			X			X
**		t	24 bit	|	none		X			X			X
**		p	32 bit	|	none		none		X			X
**		u	float	|	none		none		none		none
**		t	double	|	none		none		none		none
**
**	Dither on read:
**
**										Input
**		O			|	8 bit	16 bit	24 bit	32 bit	float	double
**		u	--------+-------------------------------------------------
**		t	short	|	none	none	X		X		X		X
**		p	int		|	none	none	none	X		X		X
**		u	float	|	none	none	none	none	none	none
**		t	double	|	none	none	none	none	none	none
*/

#define	SFE_DITHER_BAD_PTR	666
#define	SFE_DITHER_BAD_TYPE	667

typedef struct
{	int			read_short_dither_bits, read_int_dither_bits ;
	int			write_short_dither_bits, write_int_dither_bits ;
	double		read_float_dither_scale, read_double_dither_bits ;
	double		write_float_dither_scale, write_double_dither_bits ;

	sf_count_t	(*read_short)	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
	sf_count_t	(*read_int)		(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
	sf_count_t	(*read_float)	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
	sf_count_t	(*read_double)	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

	sf_count_t	(*write_short)	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
	sf_count_t	(*write_int)	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
	sf_count_t	(*write_float)	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
	sf_count_t	(*write_double)	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

	double buffer [SF_BUFFER_LEN / sizeof (double)] ;
} DITHER_DATA ;

static sf_count_t dither_read_short		(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dither_read_int		(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;

static sf_count_t dither_write_short	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dither_write_int		(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dither_write_float	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dither_write_double	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

int
dither_init (SF_PRIVATE *psf, int mode)
{	DITHER_DATA *pdither ;

	pdither = psf->dither ; /* This may be NULL. */

	/* Turn off dither on read. */
	if (mode == SFM_READ && psf->read_dither.type == SFD_NO_DITHER)
	{	if (pdither == NULL)
			return 0 ; /* Dither is already off, so just return. */

		if (pdither->read_short)
			psf->read_short = pdither->read_short ;
		if (pdither->read_int)
			psf->read_int = pdither->read_int ;
		if (pdither->read_float)
			psf->read_float = pdither->read_float ;
		if (pdither->read_double)
			psf->read_double = pdither->read_double ;
		return 0 ;
		} ;

	/* Turn off dither on write. */
	if (mode == SFM_WRITE && psf->write_dither.type == SFD_NO_DITHER)
	{	if (pdither == NULL)
			return 0 ; /* Dither is already off, so just return. */

		if (pdither->write_short)
			psf->write_short = pdither->write_short ;
		if (pdither->write_int)
			psf->write_int = pdither->write_int ;
		if (pdither->write_float)
			psf->write_float = pdither->write_float ;
		if (pdither->write_double)
			psf->write_double = pdither->write_double ;
		return 0 ;
		} ;

	/* Turn on dither on read if asked. */
	if (mode == SFM_READ && psf->read_dither.type != 0)
	{	if (pdither == NULL)
			pdither = psf->dither = calloc (1, sizeof (DITHER_DATA)) ;
		if (pdither == NULL)
			return SFE_MALLOC_FAILED ;

		switch (psf->sf.format & SF_FORMAT_SUBMASK)
		{	case SF_FORMAT_DOUBLE :
			case SF_FORMAT_FLOAT :
					pdither->read_int = psf->read_int ;
					psf->read_int = dither_read_int ;

			case SF_FORMAT_PCM_32 :
			case SF_FORMAT_PCM_24 :
			case SF_FORMAT_PCM_16 :
			case SF_FORMAT_PCM_S8 :
			case SF_FORMAT_PCM_U8 :
					pdither->read_short = psf->read_short ;
					psf->read_short = dither_read_short ;

			default : break ;
			} ;
		} ;

	/* Turn on dither on write if asked. */
	if (mode == SFM_WRITE && psf->write_dither.type != 0)
	{	if (pdither == NULL)
			pdither = psf->dither = calloc (1, sizeof (DITHER_DATA)) ;
		if (pdither == NULL)
			return SFE_MALLOC_FAILED ;

		switch (psf->sf.format & SF_FORMAT_SUBMASK)
		{	case SF_FORMAT_DOUBLE :
			case SF_FORMAT_FLOAT :
					pdither->write_int = psf->write_int ;
					psf->write_int = dither_write_int ;

			case SF_FORMAT_PCM_32 :
			case SF_FORMAT_PCM_24 :
			case SF_FORMAT_PCM_16 :
			case SF_FORMAT_PCM_S8 :
			case SF_FORMAT_PCM_U8 :

			default : break ;
			} ;

		pdither->write_short = psf->write_short ;
		psf->write_short = dither_write_short ;

		pdither->write_int = psf->write_int ;
		psf->write_int = dither_write_int ;

		pdither->write_float = psf->write_float ;
		psf->write_float = dither_write_float ;

		pdither->write_double = psf->write_double ;
		psf->write_double = dither_write_double ;
		} ;

	return 0 ;
} /* dither_init */

/*==============================================================================
*/

static void dither_short	(const short *in, short *out, int frames, int channels) ;
static void dither_int		(const int *in, int *out, int frames, int channels) ;

static void dither_float	(const float *in, float *out, int frames, int channels) ;
static void dither_double	(const double *in, double *out, int frames, int channels) ;

static sf_count_t
dither_read_short (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	psf = psf ;
	ptr = ptr ;
	return len ;
} /* dither_read_short */

static sf_count_t
dither_read_int (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	psf = psf ;
	ptr = ptr ;
	return len ;
} /* dither_read_int */

/*------------------------------------------------------------------------------
*/

static sf_count_t
dither_write_short	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	DITHER_DATA *pdither ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pdither = psf->dither) == NULL)
	{	psf->error = SFE_DITHER_BAD_PTR ;
		return 0 ;
		} ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_DPCM_8 :
				break ;

		default :
			return pdither->write_short (psf, ptr, len) ;
		} ;

	bufferlen = sizeof (pdither->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		writecount /= psf->sf.channels ;
		writecount *= psf->sf.channels ;

		dither_short (ptr, (short*) pdither->buffer, writecount / psf->sf.channels, psf->sf.channels) ;

		thiswrite = pdither->write_short (psf, (short*) pdither->buffer, writecount) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dither_write_short */

static sf_count_t
dither_write_int	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	DITHER_DATA *pdither ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pdither = psf->dither) == NULL)
	{	psf->error = SFE_DITHER_BAD_PTR ;
		return 0 ;
		} ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :

		case SF_FORMAT_DPCM_8 :
		case SF_FORMAT_DPCM_16 :
				break ;

		default :
			return pdither->write_int (psf, ptr, len) ;
		} ;


	bufferlen = sizeof (pdither->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		writecount /= psf->sf.channels ;
		writecount *= psf->sf.channels ;

		dither_int (ptr, (int*) pdither->buffer, writecount / psf->sf.channels, psf->sf.channels) ;

		thiswrite = pdither->write_int (psf, (int*) pdither->buffer, writecount) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dither_write_int */

static sf_count_t
dither_write_float	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	DITHER_DATA *pdither ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pdither = psf->dither) == NULL)
	{	psf->error = SFE_DITHER_BAD_PTR ;
		return 0 ;
		} ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :

		case SF_FORMAT_DPCM_8 :
		case SF_FORMAT_DPCM_16 :
				break ;

		default :
			return pdither->write_float (psf, ptr, len) ;
		} ;

	bufferlen = sizeof (pdither->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (float) len ;
		writecount /= psf->sf.channels ;
		writecount *= psf->sf.channels ;

		dither_float (ptr, (float*) pdither->buffer, writecount / psf->sf.channels, psf->sf.channels) ;

		thiswrite = pdither->write_float (psf, (float*) pdither->buffer, writecount) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dither_write_float */

static sf_count_t
dither_write_double	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	DITHER_DATA *pdither ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pdither = psf->dither) == NULL)
	{	psf->error = SFE_DITHER_BAD_PTR ;
		return 0 ;
		} ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :

		case SF_FORMAT_DPCM_8 :
		case SF_FORMAT_DPCM_16 :
				break ;

		default :
			return pdither->write_double (psf, ptr, len) ;
		} ;


	bufferlen = sizeof (pdither->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (double) len ;
		writecount /= psf->sf.channels ;
		writecount *= psf->sf.channels ;

		dither_double (ptr, (double*) pdither->buffer, writecount / psf->sf.channels, psf->sf.channels) ;

		thiswrite = pdither->write_double (psf, (double*) pdither->buffer, writecount) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dither_write_double */

/*==============================================================================
*/

static void
dither_short (const short *in, short *out, int frames, int channels)
{	int ch, k ;

	for (ch = 0 ; ch < channels ; ch++)
		for (k = ch ; k < channels * frames ; k += channels)
			out [k] = in [k] ;

} /* dither_short */

static void
dither_int (const int *in, int *out, int frames, int channels)
{	int ch, k ;

	for (ch = 0 ; ch < channels ; ch++)
		for (k = ch ; k < channels * frames ; k += channels)
			out [k] = in [k] ;

} /* dither_int */

static void
dither_float (const float *in, float *out, int frames, int channels)
{	int ch, k ;

	for (ch = 0 ; ch < channels ; ch++)
		for (k = ch ; k < channels * frames ; k += channels)
			out [k] = in [k] ;

} /* dither_float */

static void
dither_double (const double *in, double *out, int frames, int channels)
{	int ch, k ;

	for (ch = 0 ; ch < channels ; ch++)
		for (k = ch ; k < channels * frames ; k += channels)
			out [k] = in [k] ;

} /* dither_double */

/*==============================================================================
*/
#if 0

/*
** Not made public because this (maybe) requires storage of state information.
**
** Also maybe need separate state info for each channel!!!!
*/

int
DO_NOT_USE_sf_dither_short (const SF_DITHER_INFO *dither, const short *in, short *out, int frames, int channels)
{	int ch, k ;

	if (! dither)
		return SFE_DITHER_BAD_PTR ;

	switch (dither->type & SFD_TYPEMASK)
	{	case SFD_WHITE :
		case SFD_TRIANGULAR_PDF :
				for (ch = 0 ; ch < channels ; ch++)
					for (k = ch ; k < channels * frames ; k += channels)
						out [k] = in [k] ;
				break ;

		default :
			return SFE_DITHER_BAD_TYPE ;
		} ;

	return 0 ;
} /* DO_NOT_USE_sf_dither_short */

int
DO_NOT_USE_sf_dither_int (const SF_DITHER_INFO *dither, const int *in, int *out, int frames, int channels)
{	int ch, k ;

	if (! dither)
		return SFE_DITHER_BAD_PTR ;

	switch (dither->type & SFD_TYPEMASK)
	{	case SFD_WHITE :
		case SFD_TRIANGULAR_PDF :
				for (ch = 0 ; ch < channels ; ch++)
					for (k = ch ; k < channels * frames ; k += channels)
						out [k] = in [k] ;
				break ;

		default :
			return SFE_DITHER_BAD_TYPE ;
		} ;

	return 0 ;
} /* DO_NOT_USE_sf_dither_int */

int
DO_NOT_USE_sf_dither_float (const SF_DITHER_INFO *dither, const float *in, float *out, int frames, int channels)
{	int ch, k ;

	if (! dither)
		return SFE_DITHER_BAD_PTR ;

	switch (dither->type & SFD_TYPEMASK)
	{	case SFD_WHITE :
		case SFD_TRIANGULAR_PDF :
				for (ch = 0 ; ch < channels ; ch++)
					for (k = ch ; k < channels * frames ; k += channels)
						out [k] = in [k] ;
				break ;

		default :
			return SFE_DITHER_BAD_TYPE ;
		} ;

	return 0 ;
} /* DO_NOT_USE_sf_dither_float */

int
DO_NOT_USE_sf_dither_double (const SF_DITHER_INFO *dither, const double *in, double *out, int frames, int channels)
{	int ch, k ;

	if (! dither)
		return SFE_DITHER_BAD_PTR ;

	switch (dither->type & SFD_TYPEMASK)
	{	case SFD_WHITE :
		case SFD_TRIANGULAR_PDF :
				for (ch = 0 ; ch < channels ; ch++)
					for (k = ch ; k < channels * frames ; k += channels)
						out [k] = in [k] ;
				break ;

		default :
			return SFE_DITHER_BAD_TYPE ;
		} ;

	return 0 ;
} /* DO_NOT_USE_sf_dither_double */

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 673fad58-5314-421c-9144-9d54bfdf104c
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


#if CPU_IS_LITTLE_ENDIAN
	#define DOUBLE64_READ	double64_le_read
	#define DOUBLE64_WRITE	double64_le_write
#elif CPU_IS_BIG_ENDIAN
	#define DOUBLE64_READ	double64_be_read
	#define DOUBLE64_WRITE	double64_be_write
#endif

/*--------------------------------------------------------------------------------------------
**	Processor floating point capabilities. double64_get_capability () returns one of the
**	latter three values.
*/

enum
{	DOUBLE_UNKNOWN		= 0x00,
	DOUBLE_CAN_RW_LE	= 0x23,
	DOUBLE_CAN_RW_BE	= 0x34,
	DOUBLE_BROKEN_LE	= 0x45,
	DOUBLE_BROKEN_BE	= 0x56
} ;

/*--------------------------------------------------------------------------------------------
**	Prototypes for private functions.
*/

static sf_count_t		host_read_d2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t		host_read_d2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t		host_read_d2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t		host_read_d		(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t		host_write_s2d	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t		host_write_i2d	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t		host_write_f2d	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t		host_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	void	d2s_array 	(double *buffer, unsigned int count, short *ptr) ;
static	void	d2i_array 	(double *buffer, unsigned int count, int *ptr) ;
static	void	double64d2f_array 	(double *buffer, unsigned int count, float *ptr) ;

static 	void	s2d_array 	(short *ptr, double *buffer, unsigned int count) ;
static 	void	i2d_array 	(int *ptr, double *buffer, unsigned int count) ;
static 	void	double64f2d_array 	(float *ptr, double *buffer, unsigned int count) ;

static void		double64_peak_update	(SF_PRIVATE *psf, double *buffer, int count, int indx) ;

static int		double64_get_capability	(SF_PRIVATE *psf) ;

static sf_count_t	replace_read_d2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	replace_read_d2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	replace_read_d2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	replace_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	replace_write_s2d	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	replace_write_i2d	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	replace_write_f2d	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	replace_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	void	d2bd_read (double *buffer, int count) ;
static	void	bd2d_write (double *buffer, int count) ;

/*--------------------------------------------------------------------------------------------
**	Exported functions.
*/

int
double64_init	(SF_PRIVATE *psf)
{	static int double64_caps ;

	double64_caps = double64_get_capability (psf) ;

	psf->blockwidth = sizeof (double) * psf->sf.channels ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	switch (psf->endian + double64_caps)
		{	case (SF_ENDIAN_BIG + DOUBLE_CAN_RW_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= host_read_d2s ;
					psf->read_int		= host_read_d2i ;
					psf->read_float		= host_read_d2f ;
					psf->read_double	= host_read_d ;
					break ;

			case (SF_ENDIAN_LITTLE + DOUBLE_CAN_RW_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= host_read_d2s ;
					psf->read_int		= host_read_d2i ;
					psf->read_float		= host_read_d2f ;
					psf->read_double	= host_read_d ;
					break ;

			case (SF_ENDIAN_BIG + DOUBLE_CAN_RW_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= host_read_d2s ;
					psf->read_int		= host_read_d2i ;
					psf->read_float		= host_read_d2f ;
					psf->read_double	= host_read_d ;
					break ;

			case (SF_ENDIAN_LITTLE + DOUBLE_CAN_RW_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= host_read_d2s ;
					psf->read_int		= host_read_d2i ;
					psf->read_float		= host_read_d2f ;
					psf->read_double	= host_read_d ;
					break ;

			/* When the CPU is not IEEE compatible. */
			case (SF_ENDIAN_BIG + DOUBLE_BROKEN_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= replace_read_d2s ;
					psf->read_int		= replace_read_d2i ;
					psf->read_float		= replace_read_d2f ;
					psf->read_double	= replace_read_d ;
					break ;

			case (SF_ENDIAN_LITTLE + DOUBLE_BROKEN_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= replace_read_d2s ;
					psf->read_int		= replace_read_d2i ;
					psf->read_float		= replace_read_d2f ;
					psf->read_double	= replace_read_d ;
					break ;

			case (SF_ENDIAN_BIG + DOUBLE_BROKEN_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= replace_read_d2s ;
					psf->read_int		= replace_read_d2i ;
					psf->read_float		= replace_read_d2f ;
					psf->read_double	= replace_read_d ;
					break ;

			case (SF_ENDIAN_LITTLE + DOUBLE_BROKEN_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= replace_read_d2s ;
					psf->read_int		= replace_read_d2i ;
					psf->read_float		= replace_read_d2f ;
					psf->read_double	= replace_read_d ;
					break ;

			default : break ;
			} ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	switch (psf->endian + double64_caps)
		{	case (SF_ENDIAN_LITTLE + DOUBLE_CAN_RW_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= host_write_s2d ;
					psf->write_int		= host_write_i2d ;
					psf->write_float	= host_write_f2d ;
					psf->write_double	= host_write_d ;
					break ;

			case (SF_ENDIAN_BIG + DOUBLE_CAN_RW_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= host_write_s2d ;
					psf->write_int		= host_write_i2d ;
					psf->write_float	= host_write_f2d ;
					psf->write_double	= host_write_d ;
					break ;

			case (SF_ENDIAN_BIG + DOUBLE_CAN_RW_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= host_write_s2d ;
					psf->write_int		= host_write_i2d ;
					psf->write_float	= host_write_f2d ;
					psf->write_double	= host_write_d ;
					break ;

			case (SF_ENDIAN_LITTLE + DOUBLE_CAN_RW_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= host_write_s2d ;
					psf->write_int		= host_write_i2d ;
					psf->write_float	= host_write_f2d ;
					psf->write_double	= host_write_d ;
					break ;

			/* When the CPU is not IEEE compatible. */
			case (SF_ENDIAN_LITTLE + DOUBLE_BROKEN_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= replace_write_s2d ;
					psf->write_int		= replace_write_i2d ;
					psf->write_float	= replace_write_f2d ;
					psf->write_double	= replace_write_d ;
					break ;

			case (SF_ENDIAN_BIG + DOUBLE_BROKEN_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= replace_write_s2d ;
					psf->write_int		= replace_write_i2d ;
					psf->write_float	= replace_write_f2d ;
					psf->write_double	= replace_write_d ;
					break ;

			case (SF_ENDIAN_BIG + DOUBLE_BROKEN_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= replace_write_s2d ;
					psf->write_int		= replace_write_i2d ;
					psf->write_float	= replace_write_f2d ;
					psf->write_double	= replace_write_d ;
					break ;

			case (SF_ENDIAN_LITTLE + DOUBLE_BROKEN_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= replace_write_s2d ;
					psf->write_int		= replace_write_i2d ;
					psf->write_float	= replace_write_f2d ;
					psf->write_double	= replace_write_d ;
					break ;

			default : break ;
			} ;
		} ;

	if (psf->filelength > psf->dataoffset)
	{	psf->datalength = (psf->dataend > 0) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
		}
	else
		psf->datalength = 0 ;

	psf->sf.frames = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* double64_init */

/*----------------------------------------------------------------------------
** From : http://www.hpcf.cam.ac.uk/fp_formats.html
**
** 64 bit double precision layout (big endian)
** 	  Sign				bit 0
** 	  Exponent			bits 1-11
** 	  Mantissa			bits 12-63
** 	  Exponent Offset	1023
**
**            double             single
**
** +INF     7FF0000000000000     7F800000
** -INF     FFF0000000000000     FF800000
**  NaN     7FF0000000000001     7F800001
**                to               to
**          7FFFFFFFFFFFFFFF     7FFFFFFF
**                and              and
**          FFF0000000000001     FF800001
**                to               to
**          FFFFFFFFFFFFFFFF     FFFFFFFF
** +OVER    7FEFFFFFFFFFFFFF     7F7FFFFF
** -OVER    FFEFFFFFFFFFFFFF     FF7FFFFF
** +UNDER   0010000000000000     00800000
** -UNDER   8010000000000000     80800000
*/

double
double64_be_read (unsigned char *cptr)
{	int		exponent, negative ;
	double	dvalue ;

	negative = (cptr [0] & 0x80) ? 1 : 0 ;
	exponent = ((cptr [0] & 0x7F) << 4) | ((cptr [1] >> 4) & 0xF) ;

	/* Might not have a 64 bit long, so load the mantissa into a double. */
	dvalue = (((cptr [1] & 0xF) << 24) | (cptr [2] << 16) | (cptr [3] << 8) | cptr [4]) ;
	dvalue += ((cptr [5] << 16) | (cptr [6] << 8) | cptr [7]) / ((double) 0x1000000) ;

	if (exponent == 0 && dvalue == 0.0)
		return 0.0 ;

	dvalue += 0x10000000 ;

	exponent = exponent - 0x3FF ;

	dvalue = dvalue / ((double) 0x10000000) ;

	if (negative)
		dvalue *= -1 ;

	if (exponent > 0)
		dvalue *= (1 << exponent) ;
	else if (exponent < 0)
		dvalue /= (1 << abs (exponent)) ;

	return dvalue ;
} /* double64_be_read */

double
double64_le_read (unsigned char *cptr)
{	int		exponent, negative ;
	double	dvalue ;

	negative = (cptr [7] & 0x80) ? 1 : 0 ;
	exponent = ((cptr [7] & 0x7F) << 4) | ((cptr [6] >> 4) & 0xF) ;

	/* Might not have a 64 bit long, so load the mantissa into a double. */
	dvalue = (((cptr [6] & 0xF) << 24) | (cptr [5] << 16) | (cptr [4] << 8) | cptr [3]) ;
	dvalue += ((cptr [2] << 16) | (cptr [1] << 8) | cptr [0]) / ((double) 0x1000000) ;

	if (exponent == 0 && dvalue == 0.0)
		return 0.0 ;

	dvalue += 0x10000000 ;

	exponent = exponent - 0x3FF ;

	dvalue = dvalue / ((double) 0x10000000) ;

	if (negative)
		dvalue *= -1 ;

	if (exponent > 0)
		dvalue *= (1 << exponent) ;
	else if (exponent < 0)
		dvalue /= (1 << abs (exponent)) ;

	return dvalue ;
} /* double64_le_read */

void
double64_be_write (double in, unsigned char *out)
{	int		exponent, mantissa ;

	memset (out, 0, sizeof (double)) ;

	if (in == 0.0)
		return ;

	if (in < 0.0)
	{	in *= -1.0 ;
		out [0] |= 0x80 ;
		} ;

	in = frexp (in, &exponent) ;

	exponent += 1022 ;

	out [0] |= (exponent >> 4) & 0x7F ;
	out [1] |= (exponent << 4) & 0xF0 ;

	in *= 0x20000000 ;
	mantissa = lrint (floor (in)) ;

	out [1] |= (mantissa >> 24) & 0xF ;
	out [2] = (mantissa >> 16) & 0xFF ;
	out [3] = (mantissa >> 8) & 0xFF ;
	out [4] = mantissa & 0xFF ;

	in = fmod (in, 1.0) ;
	in *= 0x1000000 ;
	mantissa = lrint (floor (in)) ;

	out [5] = (mantissa >> 16) & 0xFF ;
	out [6] = (mantissa >> 8) & 0xFF ;
	out [7] = mantissa & 0xFF ;

	return ;
} /* double64_be_write */

void
double64_le_write (double in, unsigned char *out)
{	int		exponent, mantissa ;

	memset (out, 0, sizeof (double)) ;

	if (in == 0.0)
		return ;

	if (in < 0.0)
	{	in *= -1.0 ;
		out [7] |= 0x80 ;
		} ;

	in = frexp (in, &exponent) ;

	exponent += 1022 ;

	out [7] |= (exponent >> 4) & 0x7F ;
	out [6] |= (exponent << 4) & 0xF0 ;

	in *= 0x20000000 ;
	mantissa = lrint (floor (in)) ;

	out [6] |= (mantissa >> 24) & 0xF ;
	out [5] = (mantissa >> 16) & 0xFF ;
	out [4] = (mantissa >> 8) & 0xFF ;
	out [3] = mantissa & 0xFF ;

	in = fmod (in, 1.0) ;
	in *= 0x1000000 ;
	mantissa = lrint (floor (in)) ;

	out [2] = (mantissa >> 16) & 0xFF ;
	out [1] = (mantissa >> 8) & 0xFF ;
	out [0] = mantissa & 0xFF ;

	return ;
} /* double64_le_write */

/*==============================================================================================
**	Private functions.
*/

static void
double64_peak_update	(SF_PRIVATE *psf, double *buffer, int count, int indx)
{	int 	chan ;
	int		k, position ;
	float	fmaxval ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	fmaxval = fabs (buffer [chan]) ;
		position = 0 ;
		for (k = chan ; k < count ; k += psf->sf.channels)
			if (fmaxval < fabs (buffer [k]))
			{	fmaxval = fabs (buffer [k]) ;
				position = k ;
				} ;

		if (fmaxval > psf->pchunk->peaks [chan].value)
		{	psf->pchunk->peaks [chan].value = fmaxval ;
			psf->pchunk->peaks [chan].position = psf->write_current + indx + (position / psf->sf.channels) ;
			} ;
		} ;

	return ;
} /* double64_peak_update */

static int
double64_get_capability	(SF_PRIVATE *psf)
{	union
	{	double			d ;
		int				i [2] ;
		unsigned char	c [8] ;
	} data ;

	data.d = 1.234567890123456789 ; /* Some abitrary value. */

	if (! psf->ieee_replace)
	{	/* If this test is true ints and floats are compatible and little endian. */
		if (data.i [0] == 0x428c59fb && data.i [1] == 0x3ff3c0ca &&
			data.c [0] == 0xfb && data.c [2] == 0x8c && data.c [4] == 0xca && data.c [6] == 0xf3)
			return DOUBLE_CAN_RW_LE ;

		/* If this test is true ints and floats are compatible and big endian. */
		if ((data.i [0] == 0x3ff3c0ca && data.i [1] == 0x428c59fb) &&
			(data.c [0] == 0x3f && data.c [2] == 0xc0 && data.c [4] == 0x42 && data.c [6] == 0x59))
			return DOUBLE_CAN_RW_BE ;
		} ;

	/* Doubles are broken. Don't expect reading or writing to be fast. */
	psf_log_printf (psf, "Using IEEE replacement code for double.\n") ;

	return (CPU_IS_LITTLE_ENDIAN) ? DOUBLE_BROKEN_LE : DOUBLE_BROKEN_BE ;
} /* double64_get_capability */

/*----------------------------------------------------------------------------------------------
*/


static sf_count_t
host_read_d2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, readcount) ;

		d2s_array ((double*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* host_read_d2s */

static sf_count_t
host_read_d2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, readcount) ;

		d2i_array ((double*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* host_read_d2i */

static sf_count_t
host_read_d2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, readcount) ;

		double64d2f_array ((double*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* host_read_d2f */

static sf_count_t
host_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	if (psf->float_endswap != SF_TRUE)
		return psf_fread (ptr, sizeof (double), len, psf) ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		endswap_long_copy ((long*) (ptr + total), (long*) psf->buffer, thisread) ;

		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* host_read_d */

static sf_count_t
host_write_s2d	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;

		s2d_array (ptr + total, (double*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			double64_peak_update (psf, (double*) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* host_write_s2d */

static sf_count_t
host_write_i2d	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2d_array (ptr + total, (double*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			double64_peak_update (psf, (double*) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* host_write_i2d */

static sf_count_t
host_write_f2d	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		double64f2d_array (ptr + total, (double*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			double64_peak_update (psf, (double*) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* host_write_f2d */

static sf_count_t
host_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if (psf->has_peak)
		double64_peak_update (psf, ptr, len, 0) ;

	if (psf->float_endswap != SF_TRUE)
		return psf_fwrite (ptr, sizeof (double), len, psf) ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;

		endswap_long_copy ((long*) psf->buffer, (long*) (ptr + total), writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* host_write_d */

/*=======================================================================================
*/

static void
d2s_array (double *src, unsigned int count, short *dest)
{	while (count)
	{	count -- ;
		dest [count] = lrint (src [count]) ;
		} ;
} /* d2s_array */

static void
d2i_array (double *src, unsigned int count, int *dest)
{	while (count)
	{	count -- ;
		dest [count] = lrint (src [count]) ;
		} ;
} /* d2i_array */

static void
double64d2f_array (double *src, unsigned int count, float *dest)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;
} /* double64d2f_array */

static void
s2d_array (short *src, double *dest, unsigned int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;

} /* s2d_array */

static void
i2d_array (int *src, double *dest, unsigned int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;
} /* i2d_array */

static void
double64f2d_array (float *src, double *dest, unsigned int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;
} /* double64f2d_array */

/*=======================================================================================
*/

static sf_count_t
replace_read_d2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, readcount) ;

		d2bd_read ((double *) (psf->buffer), readcount) ;

		d2s_array ((double*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_d2s */

static sf_count_t
replace_read_d2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, readcount) ;

		d2bd_read ((double *) (psf->buffer), readcount) ;

		d2i_array ((double*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_d2i */

static sf_count_t
replace_read_d2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long *) psf->buffer, readcount) ;

		d2bd_read ((double *) (psf->buffer), readcount) ;

		memcpy (ptr + total, psf->buffer, readcount * sizeof (double)) ;

		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_d2f */

static sf_count_t
replace_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	/* FIXME : This is probably nowhere near optimal. */
	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (double), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, thisread) ;

		d2bd_read ((double *) (psf->buffer), thisread) ;

		memcpy (ptr + total, psf->buffer, thisread * sizeof (double)) ;

		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_d */

static sf_count_t
replace_write_s2d	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2d_array (ptr + total, (double *) (psf->buffer), writecount) ;

		if (psf->has_peak)
			double64_peak_update (psf, (double *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		bd2d_write ((double *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_s2d */

static sf_count_t
replace_write_i2d	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2d_array (ptr + total, (double*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			double64_peak_update (psf, (double *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		bd2d_write ((double *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_i2d */

static sf_count_t
replace_write_f2d	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		double64f2d_array (ptr + total, (double*) (psf->buffer), writecount) ;

		bd2d_write ((double *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_f2d */

static sf_count_t
replace_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	/* FIXME : This is probably nowhere near optimal. */
	if (psf->has_peak)
		double64_peak_update (psf, ptr, len, 0) ;

	bufferlen = sizeof (psf->buffer) / sizeof (double) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;

		memcpy (psf->buffer, ptr + total, writecount * sizeof (double)) ;

		bd2d_write ((double *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_long_array ((long*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (double), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_d */

/*----------------------------------------------------------------------------------------------
*/

static void
d2bd_read (double *buffer, int count)
{	while (count)
	{	count -- ;
		buffer [count] = DOUBLE64_READ ((unsigned char *) (buffer + count)) ;
		} ;
} /* d2bd_read */

static void
bd2d_write (double *buffer, int count)
{	while (count)
	{	count -- ;
		DOUBLE64_WRITE (buffer [count], (unsigned char*) (buffer + count)) ;
		} ;
} /* bd2d_write */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 4ee243b7-8c7a-469b-869c-e9aa0ee3b77f
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
dwd_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* dwd_open */

#else

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define SFE_DWD_NO_DWD			1666
#define SFE_DWD_BAND_BIT_WIDTH 	1667
#define SFE_DWD_COMPRESSION		1668

#define	DWD_IDENTIFIER		"DiamondWare Digitized\n\0\x1a"
#define	DWD_IDENTIFIER_LEN	24

#define	DWD_HEADER_LEN		57

/*------------------------------------------------------------------------------
** Typedefs.
*/

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int	dwd_read_header (SF_PRIVATE *psf) ;

static int	dwd_close		(SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
dwd_open (SF_PRIVATE *psf)
{	int	subformat, error = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = dwd_read_header (psf)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_DWD)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{
		/*-psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		if (CPU_IS_LITTLE_ENDIAN && psf->endian == SF_ENDIAN_CPU)
			psf->endian = SF_ENDIAN_LITTLE ;
		else if (psf->endian != SF_ENDIAN_LITTLE)
			psf->endian = SF_ENDIAN_BIG ;

		if (! (encoding = dwd_write_header (psf, SF_FALSE)))
			return psf->error ;

		psf->write_header = dwd_write_header ;
		-*/
		} ;

	psf->close = dwd_close ;

	/*-psf->blockwidth = psf->bytewidth * psf->sf.channels ;-*/

	return error ;
} /* dwd_open */

/*------------------------------------------------------------------------------
*/

static int
dwd_close	(SF_PRIVATE *psf)
{
	psf = psf ;

	return 0 ;
} /* dwd_close */

/* This struct contains all the fields of interest om the DWD header, but does not
** do so in the same order and layout as the actual file, header.
** No assumptions are made about the packing of this struct.
*/
typedef struct
{	unsigned char major, minor, compression, channels, bitwidth ;
	unsigned short srate, maxval ;
	unsigned int id, datalen, frames, offset ;
} DWD_HEADER ;

static int
dwd_read_header (SF_PRIVATE *psf)
{	DWD_HEADER	dwdh ;

	memset (psf->buffer, 0, sizeof (psf->buffer)) ;
	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "pb", 0, psf->buffer, DWD_IDENTIFIER_LEN) ;

	if (memcmp (psf->buffer, DWD_IDENTIFIER, DWD_IDENTIFIER_LEN) != 0)
		return SFE_DWD_NO_DWD ;

	psf_log_printf (psf, "Read only : DiamondWare Digitized (.dwd)\n", psf->buffer) ;

	psf_binheader_readf (psf, "11", &dwdh.major, &dwdh.minor) ;
	psf_binheader_readf (psf, "e4j1", &dwdh.id, 1, &dwdh.compression) ;
	psf_binheader_readf (psf, "e211", &dwdh.srate, &dwdh.channels, &dwdh.bitwidth) ;
	psf_binheader_readf (psf, "e24", &dwdh.maxval, &dwdh.datalen) ;
	psf_binheader_readf (psf, "e44", &dwdh.frames, &dwdh.offset) ;

	psf_log_printf (psf, "  Version Major : %d\n  Version Minor : %d\n  Unique ID     : %08X\n",
						dwdh.major, dwdh.minor, dwdh.id) ;
	psf_log_printf (psf, "  Compression   : %d => ", dwdh.compression) ;

	if (dwdh.compression != 0)
	{	psf_log_printf (psf, "Unsupported compression\n") ;
		return SFE_DWD_COMPRESSION ;
		}
	else
		psf_log_printf (psf, "None\n") ;

	psf_log_printf (psf, "  Sample Rate   : %d\n  Channels      : %d\n"
						 "  Bit Width     : %d\n",
						 dwdh.srate, dwdh.channels, dwdh.bitwidth) ;

	switch (dwdh.bitwidth)
	{	case 8 :
				psf->sf.format = SF_FORMAT_DWD | SF_FORMAT_PCM_S8 ;
				psf->bytewidth = 1 ;
				break ;

		case 16 :
				psf->sf.format = SF_FORMAT_DWD | SF_FORMAT_PCM_16 ;
				psf->bytewidth = 2 ;
				break ;

		default :
				psf_log_printf (psf, "*** Bad bit width %d\n", dwdh.bitwidth) ;
				return SFE_DWD_BAND_BIT_WIDTH ;
				} ;

	if (psf->filelength != dwdh.offset + dwdh.datalen)
	{	psf_log_printf (psf, "  Data Length   : %d (should be %D)\n", dwdh.datalen, psf->filelength - dwdh.offset) ;
		dwdh.datalen = (unsigned int) (psf->filelength - dwdh.offset) ;
		}
	else
		psf_log_printf (psf, "  Data Length   : %d\n", dwdh.datalen) ;

	psf_log_printf (psf, "  Max Value     : %d\n", dwdh.maxval) ;
	psf_log_printf (psf, "  Frames        : %d\n", dwdh.frames) ;
	psf_log_printf (psf, "  Data Offset   : %d\n", dwdh.offset) ;

	psf->datalength = dwdh.datalen ;
	psf->dataoffset = dwdh.offset ;

	psf->endian = SF_ENDIAN_LITTLE ;

	psf->sf.samplerate = dwdh.srate ;
	psf->sf.channels = dwdh.channels ;
	psf->sf.sections = 1 ;

	return pcm_init (psf) ;
} /* dwd_read_header */

/*------------------------------------------------------------------------------
*/

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: a5e1d2a6-a840-4039-a0e7-e1a43eb05a4f
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*===========================================================================
** Delta Word Variable Width
**
** This decoder and encoder were implemented using information found in this
** document : http://home.swbell.net/rubywand/R011SNDFMTS.TXT
**
** According to the document, the algorithm "was invented 1991 by Magnus
** Lidstrom and is copyright 1993 by NuEdge Development".
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


typedef struct
{	int		dwm_maxsize, bit_width, max_delta, span ;
	int		samplecount ;
	int		bit_count, bits, last_delta_width, last_sample ;
	struct
	{	int				index, end ;
		unsigned char	buffer [256] ;
	} b ;
} DWVW_PRIVATE ;

/*============================================================================================
*/

static sf_count_t dwvw_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dwvw_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dwvw_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dwvw_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t dwvw_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dwvw_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dwvw_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dwvw_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	dwvw_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset) ;
static int	dwvw_close	(SF_PRIVATE *psf) ;

static int	dwvw_decode_data (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int *ptr, int len) ;
static int	dwvw_decode_load_bits (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int bit_count) ;

static int	dwvw_encode_data (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int *ptr, int len) ;
static void dwvw_encode_store_bits (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int data, int new_bits) ;
static void dwvw_read_reset (DWVW_PRIVATE *pdwvw) ;

/*============================================================================================
** DWVW initialisation function.
*/

int
dwvw_init (SF_PRIVATE *psf, int bitwidth)
{	DWVW_PRIVATE	*pdwvw ;

	if (bitwidth > 24)
		return SFE_DWVW_BAD_BITWIDTH ;

	if (psf->mode == SFM_RDWR)
		return SFE_BAD_MODE_RW ;

	if ((pdwvw = calloc (1, sizeof (DWVW_PRIVATE))) == NULL)
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pdwvw ;

	pdwvw->bit_width 	= bitwidth ;
	pdwvw->dwm_maxsize	= bitwidth / 2 ;
	pdwvw->max_delta	= 1 << (bitwidth - 1) ;
	pdwvw->span			= 1 << bitwidth ;

	dwvw_read_reset (pdwvw) ;

	if (psf->mode == SFM_READ)
	{	psf->read_short		= dwvw_read_s ;
		psf->read_int		= dwvw_read_i ;
		psf->read_float		= dwvw_read_f ;
		psf->read_double	= dwvw_read_d ;
		} ;

	if (psf->mode == SFM_WRITE)
	{	psf->write_short	= dwvw_write_s ;
		psf->write_int		= dwvw_write_i ;
		psf->write_float	= dwvw_write_f ;
		psf->write_double	= dwvw_write_d ;
		} ;

	psf->seek	= dwvw_seek ;
	psf->close	= dwvw_close ;

	/* FIXME : This s bogus. */
	psf->sf.frames = SF_COUNT_MAX ;
	psf->datalength = psf->sf.frames ;
	/* EMXIF : This s bogus. */

	return 0 ;
} /* dwvw_init */

/*--------------------------------------------------------------------------------------------
*/

static int
dwvw_close (SF_PRIVATE *psf)
{	DWVW_PRIVATE *pdwvw ;

	if (psf->fdata == NULL)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	if (psf->mode == SFM_WRITE)
	{	static int last_values [12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;

		/* Write 8 zero samples to fully flush output. */
		dwvw_encode_data (psf, pdwvw, last_values, 12) ;

		/* Write the last buffer worth of data to disk. */
		psf_fwrite (pdwvw->b.buffer, 1, pdwvw->b.index, psf) ;

		if (psf->write_header)
			psf->write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* dwvw_close */

static sf_count_t
dwvw_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset)
{	DWVW_PRIVATE *pdwvw ;

	mode = mode ;

	if (! psf->fdata)
	{	psf->error = SFE_INTERNAL ;
		return ((sf_count_t) -1) ;
		} ;

	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	if (offset == 0)
	{	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		dwvw_read_reset (pdwvw) ;
		return 0 ;
		} ;

	psf->error = SFE_BAD_SEEK ;
	return	((sf_count_t) -1) ;
} /* dwvw_seek */


/*==============================================================================
*/

static sf_count_t
dwvw_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int		*iptr ;
	int		k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	iptr = (int*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = dwvw_decode_data (psf, pdwvw, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = iptr [k] >> 16 ;

		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* dwvw_read_s */

static sf_count_t
dwvw_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int			readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	readcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = dwvw_decode_data (psf, pdwvw, ptr, readcount) ;

		total += count ;
		len -= count ;

		if (count != readcount)
			break ;
		} ;

	return total ;
} /* dwvw_read_i */

static sf_count_t
dwvw_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int		*iptr ;
	int		k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	float	normfact ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 ;

	iptr = (int*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = dwvw_decode_data (psf, pdwvw, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (float) (iptr [k]) ;

		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* dwvw_read_f */

static sf_count_t
dwvw_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int		*iptr ;
	int		k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	double 	normfact ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 ;

	iptr = (int*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = dwvw_decode_data (psf, pdwvw, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (double) (iptr [k]) ;

		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* dwvw_read_d */

static int
dwvw_decode_data (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int *ptr, int len)
{	int	count ;
	int delta_width_modifier, delta_width, delta_negative, delta, sample ;

	/* Restore state from last decode call. */
	delta_width = pdwvw->last_delta_width ;
	sample = pdwvw->last_sample ;

	for (count = 0 ; count < len ; count++)
	{	/* If bit_count parameter is zero get the delta_width_modifier. */
		delta_width_modifier = dwvw_decode_load_bits (psf, pdwvw, -1) ;

		/* Check for end of input bit stream. Break loop if end. */
		if (delta_width_modifier < 0)
			break ;

		if (delta_width_modifier && dwvw_decode_load_bits (psf, pdwvw, 1))
			delta_width_modifier = - delta_width_modifier ;

		/* Calculate the current word width. */
		delta_width = (delta_width + delta_width_modifier + pdwvw->bit_width) % pdwvw->bit_width ;

		/* Load the delta. */
		delta = 0 ;
		if (delta_width)
		{	delta = dwvw_decode_load_bits (psf, pdwvw, delta_width - 1) | (1 << (delta_width - 1)) ;
			delta_negative = dwvw_decode_load_bits (psf, pdwvw, 1) ;
			if (delta == pdwvw->max_delta - 1)
				delta += dwvw_decode_load_bits (psf, pdwvw, 1) ;
			if (delta_negative)
				delta = -delta ;
			} ;

		/* Calculate the sample */
		sample += delta ;

		if (sample >= pdwvw->max_delta)
			sample -= pdwvw->span ;
		else if (sample < - pdwvw->max_delta)
			sample += pdwvw->span ;

		/* Store the sample justifying to the most significant bit. */
		ptr [count] = sample << (32 - pdwvw->bit_width) ;

		if (pdwvw->b.end == 0 && pdwvw->bit_count == 0)
			break ;
		} ;

	pdwvw->last_delta_width = delta_width ;
	pdwvw->last_sample = sample ;

	pdwvw->samplecount += count ;

	return count ;
} /* dwvw_decode_data */

static int
dwvw_decode_load_bits (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int bit_count)
{	int output = 0, get_dwm = SF_FALSE ;

	/*
	**	Depending on the value of parameter bit_count, either get the
	**	required number of bits (ie bit_count > 0) or the
	**	delta_width_modifier (otherwise).
	*/

	if (bit_count < 0)
	{	get_dwm = SF_TRUE ;
		/* modify bit_count to ensure we have enought bits for finding dwm. */
		bit_count = pdwvw->dwm_maxsize ;
		} ;

	/* Load bits in bit reseviour. */
	while (pdwvw->bit_count < bit_count)
	{	if (pdwvw->b.index >= pdwvw->b.end)
		{	pdwvw->b.end = psf_fread (pdwvw->b.buffer, 1, sizeof (pdwvw->b.buffer), psf) ;
			pdwvw->b.index = 0 ;
			} ;

		/* Check for end of input stream. */
		if (bit_count < 8 && pdwvw->b.end == 0)
			return -1 ;

		pdwvw->bits = (pdwvw->bits << 8) ;

		if (pdwvw->b.index < pdwvw->b.end)
		{	pdwvw->bits |= pdwvw->b.buffer [pdwvw->b.index] ;
			pdwvw->b.index ++ ;
			} ;
		pdwvw->bit_count += 8 ;
		} ;

	/* If asked to get bits do so. */
	if (! get_dwm)
	{	output = (pdwvw->bits >> (pdwvw->bit_count - bit_count)) & ((1 << bit_count) - 1) ;
		pdwvw->bit_count -= bit_count ;
		return output ;
		} ;

	/* Otherwise must have been asked to get delta_width_modifier. */
	while (output < (pdwvw->dwm_maxsize))
	{	pdwvw->bit_count -= 1 ;
		if (pdwvw->bits & (1 << pdwvw->bit_count))
			break ;
		output += 1 ;
		} ;

	return output ;
} /* dwvw_decode_load_bits */

static void
dwvw_read_reset (DWVW_PRIVATE *pdwvw)
{	pdwvw->samplecount		= 0 ;
	pdwvw->b.index			= 0 ;
	pdwvw->b.end			= 0 ;
	pdwvw->bit_count		= 0 ;
	pdwvw->bits				= 0 ;
	pdwvw->last_delta_width = 0 ;
	pdwvw->last_sample		= 0 ;
} /* dwvw_read_reset */

static void
dwvw_encode_store_bits (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int data, int new_bits)
{	int 	byte ;

	/* Shift the bits into the resevoir. */
	pdwvw->bits = (pdwvw->bits << new_bits) | (data & ((1 << new_bits) - 1)) ;
	pdwvw->bit_count += new_bits ;

	/* Transfer bit to buffer. */
	while (pdwvw->bit_count >= 8)
	{	byte = pdwvw->bits >> (pdwvw->bit_count - 	8) ;
		pdwvw->bit_count -= 8 ;
		pdwvw->b.buffer [pdwvw->b.index] = byte & 0xFF ;
		pdwvw->b.index ++ ;
		} ;

	if (pdwvw->b.index > SIGNED_SIZEOF (pdwvw->b.buffer) - 4)
	{	psf_fwrite (pdwvw->b.buffer, 1, pdwvw->b.index, psf) ;
		pdwvw->b.index = 0 ;
		} ;

	return ;
} /* dwvw_encode_store_bits */

#if 0
/* Debigging routine. */
static void
dump_bits (DWVW_PRIVATE *pdwvw)
{	int k, mask ;

	for (k = 0 ; k < 10 && k < pdwvw->b.index ; k++)
	{	mask = 0x80 ;
		while (mask)
		{	putchar (mask & pdwvw->b.buffer [k] ? '1' : '0') ;
			mask >>= 1 ;
			} ;
		putchar (' ') ;
		}

	for (k = pdwvw->bit_count - 1 ; k >= 0 ; k --)
		putchar (pdwvw->bits & (1 << k) ? '1' : '0') ;

	putchar ('\n') ;
} /* dump_bits */
#endif

#define HIGHEST_BIT(x,count)		\
			{	int y = x ;			\
				(count) = 0 ;	 	\
				while (y)			\
				{	(count) ++ ;	\
					y >>= 1 ;		\
					} ;				\
				} ;

static int
dwvw_encode_data (SF_PRIVATE *psf, DWVW_PRIVATE *pdwvw, int *ptr, int len)
{	int	count ;
	int delta_width_modifier, delta, delta_negative, delta_width, extra_bit ;

	for (count = 0 ; count < len ; count++)
	{	delta = (ptr [count] >> (32 - pdwvw->bit_width)) - pdwvw->last_sample ;

		/* Calculate extra_bit if needed. */
		extra_bit = -1 ;
		delta_negative = 0 ;
		if (delta < -pdwvw->max_delta)
			delta = pdwvw->max_delta + (delta % pdwvw->max_delta) ;
		else if (delta == -pdwvw->max_delta)
		{	extra_bit = 1 ;
			delta_negative = 1 ;
			delta = pdwvw->max_delta - 1 ;
			}
		else if (delta > pdwvw->max_delta)
		{	delta_negative = 1 ;
			delta = pdwvw->span - delta ;
			delta = abs (delta) ;
			}
		else if (delta == pdwvw->max_delta)
		{	extra_bit = 1 ;
			delta = pdwvw->max_delta - 1 ;
			}
		else if (delta < 0)
		{	delta_negative = 1 ;
			delta = abs (delta) ;
			} ;

		if (delta == pdwvw->max_delta - 1 && extra_bit == -1)
			extra_bit = 0 ;

		/* Find width in bits of delta */
		HIGHEST_BIT (delta, delta_width) ;

		/* Calculate the delta_width_modifier */
		delta_width_modifier = (delta_width - pdwvw->last_delta_width) % pdwvw->bit_width ;
		if (delta_width_modifier > pdwvw->dwm_maxsize)
			delta_width_modifier -= pdwvw->bit_width ;
		if (delta_width_modifier < -pdwvw->dwm_maxsize)
			delta_width_modifier += pdwvw->bit_width ;

		/* Write delta_width_modifier zeros, followed by terminating '1'. */
		dwvw_encode_store_bits (psf, pdwvw, 0, abs (delta_width_modifier)) ;
		if (abs (delta_width_modifier) != pdwvw->dwm_maxsize)
			dwvw_encode_store_bits (psf, pdwvw, 1, 1) ;

		/*  Write delta_width_modifier sign. */
		if (delta_width_modifier < 0)
			dwvw_encode_store_bits (psf, pdwvw, 1, 1) ;
		if (delta_width_modifier > 0)
			dwvw_encode_store_bits (psf, pdwvw, 0, 1) ;

		/* Write delta and delta sign bit. */
		if (delta_width)
		{	dwvw_encode_store_bits (psf, pdwvw, delta, abs (delta_width) - 1) ;
			dwvw_encode_store_bits (psf, pdwvw, (delta_negative ? 1 : 0), 1) ;
			} ;

		/* Write extra bit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		if (extra_bit >= 0)
			dwvw_encode_store_bits (psf, pdwvw, extra_bit, 1) ;

		pdwvw->last_sample = ptr [count] >> (32 - pdwvw->bit_width) ;
		pdwvw->last_delta_width = delta_width ;
		} ;

	pdwvw->samplecount += count ;

	return count ;
} /* dwvw_encode_data */

static sf_count_t
dwvw_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int		*iptr ;
	int		k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	iptr = (int*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = ptr [total + k] << 16 ;
		count = dwvw_encode_data (psf, pdwvw, iptr, writecount) ;

		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* dwvw_write_s */

static sf_count_t
dwvw_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int			writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = dwvw_encode_data (psf, pdwvw, ptr, writecount) ;

		total += count ;
		len -= count ;

		if (count != writecount)
			break ;
		} ;

	return total ;
} /* dwvw_write_i */

static sf_count_t
dwvw_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int			*iptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : 1.0 ;

	iptr = (int*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = dwvw_encode_data (psf, pdwvw, iptr, writecount) ;

		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* dwvw_write_f */

static sf_count_t
dwvw_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	DWVW_PRIVATE *pdwvw ;
	int			*iptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	if (! psf->fdata)
		return 0 ;
	pdwvw = (DWVW_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : 1.0 ;

	iptr = (int*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = lrint (normfact * ptr [total + k]) ;
		count = dwvw_encode_data (psf, pdwvw, iptr, writecount) ;

		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* dwvw_write_d */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 1ca09552-b01f-4d7f-9bcf-612f834fe41d
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
** Copyright (C) 2003 Ross Bencina <rbencina@iprimus.com.au>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
** This header file MUST be included before the others to ensure that
** large file support is enabled.
*/


#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if (HAVE_DECL_S_IRGRP == 0)
#include <sf_unistd.h>
#endif

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#if (defined (__MWERKS__) && defined (macintosh))
typedef int ssize_t ;
#include <Files.h>
#endif


#define	SENSIBLE_SIZE	(0x40000000)

static void psf_log_syserr (SF_PRIVATE *psf, int error) ;

#if ((defined (WIN32) || defined (_WIN32)) == 0)

/*------------------------------------------------------------------------------
** Win32 stuff at the bottom of the file. Unix and other sensible OSes here.
*/

int
psf_fopen (SF_PRIVATE *psf, const char *pathname, int open_mode)
{	int oflag, mode ;

	/*
	** Sanity check. If everything is OK, this test and the printfs will
	** be optimised out. This is meant to catch the problems caused by
	** "config.h" being included after <stdio.h>.
	*/
	if (sizeof (off_t) != sizeof (sf_count_t))
	{	puts ("\n\n*** Fatal error : sizeof (off_t) != sizeof (sf_count_t)") ;
		puts ("*** This means that libsndfile was not configured correctly.\n") ;
		exit (1) ;
		} ;

	switch (open_mode)
	{	case SFM_READ :
				oflag = O_RDONLY ;
				mode = 0 ;
				break ;

		case SFM_WRITE :
				oflag = O_WRONLY | O_CREAT | O_TRUNC ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ;
				break ;

		case SFM_RDWR :
				oflag = O_RDWR | O_CREAT ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ;
				break ;

		default :
				psf->error = SFE_BAD_OPEN_MODE ;
				return psf->error ;
				break ;
		} ;

#if defined (__CYGWIN__)
	oflag |= O_BINARY ;
#endif

	if (mode == 0)
		psf->filedes = open (pathname, oflag) ;
	else
		psf->filedes = open (pathname, oflag, mode) ;

	if (psf->filedes == -1)
		psf_log_syserr (psf, errno) ;

	psf->mode = open_mode ;

	return psf->error ;
} /* psf_fopen */

int
psf_set_stdio (SF_PRIVATE *psf, int mode)
{	int	error = 0 ;

	switch (mode)
	{	case SFM_RDWR :
				error = SFE_OPEN_PIPE_RDWR ;
				break ;

		case SFM_READ :
				psf->filedes = 0 ;
				break ;

		case SFM_WRITE :
				psf->filedes = 1 ;
				break ;

		default :
				error = SFE_BAD_OPEN_MODE ;
				break ;
		} ;
	psf->filelength = 0 ;

	return error ;
} /* psf_set_stdio */

void
psf_set_file (SF_PRIVATE *psf, int fd)
{	psf->filedes = fd ;
} /* psf_set_file */

int
psf_filedes_valid (SF_PRIVATE *psf)
{	return (psf->filedes >= 0) ? SF_TRUE : SF_FALSE ;
} /* psf_set_file */

sf_count_t
psf_fseek (SF_PRIVATE *psf, sf_count_t offset, int whence)
{	sf_count_t	new_position ;

	switch (whence)
	{	case SEEK_SET :
				offset += psf->fileoffset ;
				break ;

		case SEEK_END :
				if (psf->mode == SFM_WRITE)
				{	new_position = lseek (psf->filedes, offset, whence) ;

					if (new_position < 0)
						psf_log_syserr (psf, errno) ;

					return new_position - psf->fileoffset ;
					} ;

				/* Transform SEEK_END into a SEEK_SET, ie find the file
				** length add the requested offset (should be <= 0) to
				** get the offset wrt the start of file.
				*/
				whence = SEEK_SET ;
				offset = lseek (psf->filedes, 0, SEEK_END) + offset ;
				break ;

		default :
				/* No need to do anything about SEEK_CUR. */
				break ;
		} ;

	new_position = lseek (psf->filedes, offset, whence) ;

	if (new_position < 0)
		psf_log_syserr (psf, errno) ;

	new_position -= psf->fileoffset ;

	return new_position ;
} /* psf_fseek */

sf_count_t
psf_fread (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t	count ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : (ssize_t) items ;

		count = read (psf->filedes, ((char*) ptr) + total, (size_t) count) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	if (psf->is_pipe)
		psf->pipeoffset += total ;

	return total / bytes ;
} /* psf_fread */

sf_count_t
psf_fwrite (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t	count ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : items ;

		count = write (psf->filedes, ((char*) ptr) + total, count) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	if (psf->is_pipe)
		psf->pipeoffset += total ;

	return total / bytes ;
} /* psf_fwrite */

sf_count_t
psf_ftell (SF_PRIVATE *psf)
{	sf_count_t pos ;

	if (psf->is_pipe)
		return psf->pipeoffset ;

	pos = lseek (psf->filedes, 0, SEEK_CUR) ;

	if (pos == ((sf_count_t) -1))
	{	psf_log_syserr (psf, errno) ;
		return -1 ;
		} ;

	return pos - psf->fileoffset ;
} /* psf_ftell */

int
psf_fclose (SF_PRIVATE *psf)
{	int retval ;

#if ((defined (__MWERKS__) && defined (macintosh)) == 0)
	/* Must be MacOS9 which doesn't have fsync(). */
	if (fsync (psf->filedes) == -1 && errno == EBADF)
		return 0 ;
#endif

	if (psf->do_not_close_descriptor)
	{	psf->filedes = -1 ;
		return 0 ;
		} ;

	while ((retval = close (psf->filedes)) == -1 && errno == EINTR)
		/* Do nothing. */ ;

	if (retval == -1)
		psf_log_syserr (psf, errno) ;

	psf->filedes = -1 ;

	return retval ;
} /* psf_fclose */

sf_count_t
psf_fgets (char *buffer, sf_count_t bufsize, SF_PRIVATE *psf)
{	sf_count_t	k = 0 ;
	sf_count_t		count ;

	while (k < bufsize - 1)
	{	count = read (psf->filedes, &(buffer [k]), 1) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0 || buffer [k++] == '\n')
			break ;
		} ;

	buffer [k] = 0 ;

	return k ;
} /* psf_fgets */

int
psf_is_pipe (SF_PRIVATE *psf)
{	struct stat statbuf ;

	if (fstat (psf->filedes, &statbuf) == -1)
	{	psf_log_syserr (psf, errno) ;
		/* Default to maximum safety. */
		return SF_TRUE ;
		} ;

	if (S_ISFIFO (statbuf.st_mode) || S_ISSOCK (statbuf.st_mode))
		return SF_TRUE ;

	return SF_FALSE ;
} /* psf_is_pipe */

sf_count_t
psf_get_filelen (SF_PRIVATE *psf)
{	struct stat statbuf ;
	sf_count_t	filelen ;

	/*
	** Sanity check.
	** If everything is OK, this will be optimised out.
	*/
	if (sizeof (statbuf.st_size) == 4 && sizeof (sf_count_t) == 8)
		return SFE_BAD_STAT_SIZE ;

/* Cygwin seems to need this. */
#if (defined (__CYGWIN__) && HAVE_FSYNC)
	fsync (psf->filedes) ;
#endif

	if (fstat (psf->filedes, &statbuf) == -1)
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	switch (psf->mode)
	{	case SFM_WRITE :
			filelen = statbuf.st_size - psf->fileoffset ;
			break ;

		case SFM_READ :
			if (psf->fileoffset > 0 && psf->filelength > 0)
				filelen = psf->filelength ;
			else
				filelen = statbuf.st_size ;
			break ;

		case SFM_RDWR :
			/*
			** Cannot open embedded files SFM_RDWR so we don't need to
			** subtract psf->fileoffset. We already have the answer we
			** need.
			*/
			filelen = statbuf.st_size ;
			break ;

		default :
			/* Shouldn't be here, so return error. */
			filelen = -1 ;
		} ;

	return filelen ;
} /* psf_get_filelen */

int
psf_ftruncate (SF_PRIVATE *psf, sf_count_t len)
{	int retval ;

	/* Returns 0 on success, non-zero on failure. */
	if (len < 0)
		return -1 ;

	if ((sizeof (off_t) < sizeof (sf_count_t)) && len > 0x7FFFFFFF)
		return -1 ;

#if (defined (__MWERKS__) && defined (macintosh))
	retval = FSSetForkSize (psf->filedes, fsFromStart, len) ;
#else
	retval = ftruncate (psf->filedes, len) ;
#endif

	if (retval == -1)
		psf_log_syserr (psf, errno) ;

	return retval ;
} /* psf_ftruncate */


static void
psf_log_syserr (SF_PRIVATE *psf, int error)
{
	/* Only log an error if no error has been set yet. */
	if (psf->error == 0)
	{	psf->error = SFE_SYSTEM ;
		LSF_SNPRINTF (psf->syserr, sizeof (psf->syserr), "System error : %s.", strerror (error)) ;
		} ;

	return ;
} /* psf_log_syserr */

//XXX formerly OS_IS_WIN32
#elif	__PLATFORM_WIN32__

/* Win32 file i/o functions implemented using native Win32 API */

#include <windows.h>
#include <io.h>

#ifndef HAVE_SSIZE_T
    // ge: this is needed in some/earlier versions of windows
    #ifndef ssize_t // REFACTOR-2017; was #ifndef __WINDOWS_MODERN__
    typedef long ssize_t ;
    #endif
#endif

/* Win32 */ static void
psf_log_syserr (SF_PRIVATE *psf, int error)
{	LPVOID lpMsgBuf ;

	/* Only log an error if no error has been set yet. */
	if (psf->error == 0)
	{	psf->error = SFE_SYSTEM ;

		FormatMessage (
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			error,
			MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			) ;

		LSF_SNPRINTF (psf->syserr, sizeof (psf->syserr), "System error : %s", lpMsgBuf) ;
		LocalFree (lpMsgBuf) ;
		} ;

	return ;
} /* psf_log_syserr */


/* Win32 */ int
psf_fopen (SF_PRIVATE *psf, const char *pathname, int open_mode)
{	DWORD dwDesiredAccess ;
	DWORD dwShareMode ;
	DWORD dwCreationDistribution ;
	HANDLE handle ;

	switch (open_mode)
	{	case SFM_READ :
				dwDesiredAccess = GENERIC_READ ;
				dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE ;
				dwCreationDistribution = OPEN_EXISTING ;
				break ;

		case SFM_WRITE :
				dwDesiredAccess = GENERIC_WRITE ;
				dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE ;
				dwCreationDistribution = CREATE_ALWAYS ;
				break ;

		case SFM_RDWR :
				dwDesiredAccess = GENERIC_READ | GENERIC_WRITE ;
				dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE ;
				dwCreationDistribution = OPEN_ALWAYS ;
				break ;

		default :
				psf->error = SFE_BAD_OPEN_MODE ;
				return psf->error ;
		} ;

	handle = CreateFile (
			pathname,					/* pointer to name of the file */
			dwDesiredAccess,			/* access (read-write) mode */
			dwShareMode,				/* share mode */
			0,							/* pointer to security attributes */
			dwCreationDistribution,		/* how to create */
			FILE_ATTRIBUTE_NORMAL,		/* file attributes (could use FILE_FLAG_SEQUENTIAL_SCAN) */
			NULL						/* handle to file with attributes to copy */
			) ;

	if (handle == INVALID_HANDLE_VALUE)
	{	psf_log_syserr (psf, GetLastError ()) ;
		return psf->error ;
		} ;

	psf->filedes = (int) handle ;
	psf->mode = open_mode ;

	return psf->error ;
} /* psf_fopen */

/* Win32 */ int
psf_set_stdio (SF_PRIVATE *psf, int mode)
{	HANDLE	handle = NULL ;
	int	error = 0 ;

	switch (mode)
	{	case SFM_RDWR :
				error = SFE_OPEN_PIPE_RDWR ;
				break ;

		case SFM_READ :
				handle = GetStdHandle (STD_INPUT_HANDLE) ;
				psf->do_not_close_descriptor = 1 ;
				break ;

		case SFM_WRITE :
				handle = GetStdHandle (STD_OUTPUT_HANDLE) ;
				psf->do_not_close_descriptor = 1 ;
				break ;

		default :
				error = SFE_BAD_OPEN_MODE ;
				break ;
		} ;

	psf->filedes = (int) handle ;
	psf->filelength = 0 ;

	return error ;
} /* psf_set_stdio */

/* Win32 */ void
psf_set_file (SF_PRIVATE *psf, int fd)
{	HANDLE handle ;
	long osfhandle ;

	osfhandle = _get_osfhandle (fd) ;
	handle = (HANDLE) osfhandle ;

	if (GetFileType (handle) == FILE_TYPE_DISK)
		psf->filedes = (int) handle ;
	else
		psf->filedes = fd ;
} /* psf_set_file */

/* Win32 */ int
psf_filedes_valid (SF_PRIVATE *psf)
{	return (((HANDLE) psf->filedes) != INVALID_HANDLE_VALUE) ? SF_TRUE : SF_FALSE ;
} /* psf_set_file */

/* Win32 */ sf_count_t
psf_fseek (SF_PRIVATE *psf, sf_count_t offset, int whence)
{	sf_count_t new_position ;
	LONG lDistanceToMove, lDistanceToMoveHigh ;
	DWORD dwMoveMethod ;
	DWORD dwResult, dwError ;

	switch (whence)
	{	case SEEK_SET :
				offset += psf->fileoffset ;
				dwMoveMethod = FILE_BEGIN ;
				break ;

		case SEEK_END :
				dwMoveMethod = FILE_END ;
				break ;

		default :
				dwMoveMethod = FILE_CURRENT ;
				break ;
		} ;

	lDistanceToMove = (DWORD) (offset & 0xFFFFFFFF) ;
#ifndef __WINDOWS_DS__
	lDistanceToMoveHigh = (DWORD) (0xFFFFFFFF & (offset >> 32)) ;
#else
    lDistanceToMoveHigh = 0;
#endif

	dwResult = SetFilePointer ((HANDLE) psf->filedes, lDistanceToMove, &lDistanceToMoveHigh, dwMoveMethod) ;

	if (dwResult == 0xFFFFFFFF)
		dwError = GetLastError () ;
	else
		dwError = NO_ERROR ;

	if (dwError != NO_ERROR)
	{	psf_log_syserr (psf, dwError) ;
		return -1 ;
		} ;

	new_position = (dwResult + ((__int64) lDistanceToMoveHigh << 32)) - psf->fileoffset ;

	return new_position ;
} /* psf_fseek */

/* Win32 */ sf_count_t
psf_fread (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t count ;
	DWORD dwNumberOfBytesRead ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : (ssize_t) items ;
        if( items == 16384 ) count = 1024;

		if (ReadFile ((HANDLE) psf->filedes, ((char*) ptr) + total, count, &dwNumberOfBytesRead, 0) == 0)
		{	psf_log_syserr (psf, GetLastError ()) ;
			break ;
			}
		else
			count = dwNumberOfBytesRead ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	if (psf->is_pipe)
		psf->pipeoffset += total ;

	return total / bytes ;
} /* psf_fread */

/* Win32 */ sf_count_t
psf_fwrite (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t	 count ;
	DWORD dwNumberOfBytesWritten ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : (ssize_t) items ;

		if (WriteFile ((HANDLE) psf->filedes, ((char*) ptr) + total, count, &dwNumberOfBytesWritten, 0) == 0)
		{	psf_log_syserr (psf, GetLastError ()) ;
			break ;
			}
		else
			count = dwNumberOfBytesWritten ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	if (psf->is_pipe)
		psf->pipeoffset += total ;

	return total / bytes ;
} /* psf_fwrite */

/* Win32 */ sf_count_t
psf_ftell (SF_PRIVATE *psf)
{	sf_count_t pos ;
	LONG lDistanceToMoveLow, lDistanceToMoveHigh ;
	DWORD dwResult, dwError ;

	if (psf->is_pipe)
		return psf->pipeoffset ;

	lDistanceToMoveLow = 0 ;
	lDistanceToMoveHigh = 0 ;

	dwResult = SetFilePointer ((HANDLE) psf->filedes, lDistanceToMoveLow, NULL, FILE_CURRENT) ;

	if (dwResult == 0xFFFFFFFF)
		dwError = GetLastError () ;
	else
		dwError = NO_ERROR ;

	if (dwError != NO_ERROR)
	{	psf_log_syserr (psf, dwError) ;
		return -1 ;
		} ;

	pos = (dwResult + ((__int64) lDistanceToMoveHigh << 32)) ;

	return pos - psf->fileoffset ;
} /* psf_ftell */

/* Win32 */ int
psf_fclose (SF_PRIVATE *psf)
{	int retval = 0 ;

	if (psf->do_not_close_descriptor)
	{
// ge: needed for some/earlier versions of windows
#ifndef __WINDOWS_MODERN__
        (HANDLE)
#endif
        psf->filedes = INVALID_HANDLE_VALUE ;
		return 0 ;
		} ;

	if (CloseHandle ((HANDLE) psf->filedes) == 0)
	{	retval = -1 ;
		psf_log_syserr (psf, GetLastError ()) ;
		} ;

	psf->filedes = (int) INVALID_HANDLE_VALUE ;

	return retval ;
} /* psf_fclose */

/* Win32 */ sf_count_t
psf_fgets (char *buffer, sf_count_t bufsize, SF_PRIVATE *psf)
{	sf_count_t k = 0 ;
	sf_count_t count ;
	DWORD dwNumberOfBytesRead ;

	while (k < bufsize - 1)
	{	if (ReadFile ((HANDLE) psf->filedes, &(buffer [k]), 1, &dwNumberOfBytesRead, 0) == 0)
		{	psf_log_syserr (psf, GetLastError ()) ;
			break ;
			}
		else
		{	count = dwNumberOfBytesRead ;
			/* note that we only check for '\n' not other line endings such as CRLF */
			if (count == 0 || buffer [k++] == '\n')
				break ;
			} ;
		} ;

	buffer [k] = '\0' ;

	return k ;
} /* psf_fgets */

/* Win32 */ int
psf_is_pipe (SF_PRIVATE *psf)
{	if (GetFileType ((HANDLE) psf->filedes) == FILE_TYPE_DISK)
		return SF_FALSE ;

	/* Default to maximum safety. */
	return SF_TRUE ;
} /* psf_is_pipe */

/* Win32 */ sf_count_t
psf_get_filelen (SF_PRIVATE *psf)
{	sf_count_t filelen ;
	DWORD dwFileSizeLow, dwFileSizeHigh, dwError = NO_ERROR ;

	dwFileSizeLow = GetFileSize ((HANDLE) psf->filedes, &dwFileSizeHigh) ;

	if (dwFileSizeLow == 0xFFFFFFFF)
		dwError = GetLastError () ;

	if (dwError != NO_ERROR)
	{	psf_log_syserr (psf, GetLastError ()) ;
		return 0 ;
		}
	else
		filelen = dwFileSizeLow + ((__int64) dwFileSizeHigh << 32) ;

	switch (psf->mode)
	{	case SFM_WRITE :
			filelen = filelen - psf->fileoffset ;
			break ;

		case SFM_READ :
			if (psf->fileoffset > 0 && psf->filelength > 0)
				filelen = psf->filelength ;
			break ;

		case SFM_RDWR :
			/*
			** Cannot open embedded files SFM_RDWR so we don't need to
			** subtract psf->fileoffset. We already have the answer we
			** need.
			*/
			break ;

		default :
			/* Shouldn't be here, so return error. */
			filelen = -1 ;
		} ;

	return filelen ;
} /* psf_get_filelen */

/* Win32 */ int
psf_ftruncate (SF_PRIVATE *psf, sf_count_t len)
{	int retval = 0 ;
	LONG lDistanceToMoveLow, lDistanceToMoveHigh ;
	DWORD dwResult, dwError = NO_ERROR ;

	/* This implementation trashes the current file position.
	** should it save and restore it? what if the current position is past
	** the new end of file?
	*/

	/* Returns 0 on success, non-zero on failure. */
	if (len < 0)
		return 1 ;

	lDistanceToMoveLow = (DWORD) (len & 0xFFFFFFFF) ;
#ifndef __WINDOWS_DS__
	lDistanceToMoveHigh = (DWORD) ((len >> 32) & 0xFFFFFFFF) ;
#else
    lDistanceToMoveHigh = 0;
#endif

	dwResult = SetFilePointer ((HANDLE) psf->filedes, lDistanceToMoveLow, &lDistanceToMoveHigh, FILE_BEGIN) ;

	if (dwResult == 0xFFFFFFFF)
		dwError = GetLastError () ;

	if (dwError != NO_ERROR)
	{	retval = -1 ;
		psf_log_syserr (psf, dwError) ;
		}
	else
	{	/* Note: when SetEndOfFile is used to extend a file, the contents of the
		** new portion of the file is undefined. This is unlike chsize(),
		** which guarantees that the new portion of the file will be zeroed.
		** Not sure if this is important or not.
		*/
		if (SetEndOfFile ((HANDLE) psf->filedes) == 0)
		{	retval = -1 ;
			psf_log_syserr (psf, GetLastError ()) ;
			} ;
		} ;

	return retval ;
} /* psf_ftruncate */


#else
/* Win32 file i/o functions implemented using Unix-style file i/o API */

/* Win32 has a 64 file offset seek function:
**
**		__int64 _lseeki64 (int handle, __int64 offset, int origin) ;
**
** It also has a 64 bit fstat function:
**
**		int fstati64 (int, struct _stati64) ;
**
** but the fscking thing doesn't work!!!!! The file size parameter returned
** by this function is only valid up until more data is written at the end of
** the file. That makes this function completely 100% useless.
*/

#include <io.h>
#include <direct.h>

#ifndef HAVE_SSIZE_T
typedef long ssize_t ;
#endif

/* Win32 */ int
psf_fopen (SF_PRIVATE *psf, const char *pathname, int open_mode)
{	int oflag, mode ;

	switch (open_mode)
	{	case SFM_READ :
				oflag = O_RDONLY | O_BINARY ;
				mode = 0 ;
				break ;

		case SFM_WRITE :
				oflag = O_WRONLY | O_CREAT | O_TRUNC | O_BINARY ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP ;
				break ;

		case SFM_RDWR :
				oflag = O_RDWR | O_CREAT | O_BINARY ;
				mode = S_IRUSR | S_IWUSR | S_IRGRP ;
				break ;

		default :
				psf->error = SFE_BAD_OPEN_MODE ;
				return -1 ;
				break ;
		} ;

	if (mode == 0)
		psf->filedes = open (pathname, oflag) ;
	else
		psf->filedes = open (pathname, oflag, mode) ;

	if (psf->filedes == -1)
		psf_log_syserr (psf, errno) ;

	return psf->filedes ;
} /* psf_fopen */

/* Win32 */ sf_count_t
psf_fseek (SF_PRIVATE *psf, sf_count_t offset, int whence)
{	sf_count_t	new_position ;

	switch (whence)
	{	case SEEK_SET :
				offset += psf->fileoffset ;
				break ;

		case SEEK_END :
				if (psf->mode == SFM_WRITE)
				{	new_position = _lseeki64 (psf->filedes, offset, whence) ;

					if (new_position < 0)
						psf_log_syserr (psf, errno) ;

					return new_position - psf->fileoffset ;
					} ;

				/* Transform SEEK_END into a SEEK_SET, ie find the file
				** length add the requested offset (should be <= 0) to
				** get the offset wrt the start of file.
				*/
				whence = SEEK_SET ;
				offset = _lseeki64 (psf->filedes, 0, SEEK_END) + offset ;
				break ;

		default :
				/* No need to do anything about SEEK_CUR. */
				break ;
		} ;

	/*
	** Bypass weird Win32-ism if necessary.
	** _lseeki64() returns an "invalid parameter" error if called with the
	** offset == 0 and whence == SEEK_CUR.
	*** Use the _telli64() function instead.
	*/
	if (offset == 0 && whence == SEEK_CUR)
		new_position = _telli64 (psf->filedes) ;
	else
		new_position = _lseeki64 (psf->filedes, offset, whence) ;

	if (new_position < 0)
		psf_log_syserr (psf, errno) ;

	new_position -= psf->fileoffset ;

	return new_position ;
} /* psf_fseek */

/* Win32 */ sf_count_t
psf_fread (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t	 count ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : (ssize_t) items ;

		count = read (psf->filedes, ((char*) ptr) + total, (size_t) count) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	return total / bytes ;
} /* psf_fread */

/* Win32 */ sf_count_t
psf_fwrite (void *ptr, sf_count_t bytes, sf_count_t items, SF_PRIVATE *psf)
{	sf_count_t total = 0 ;
	ssize_t	 count ;

	items *= bytes ;

	/* Do this check after the multiplication above. */
	if (items <= 0)
		return 0 ;

	while (items > 0)
	{	/* Break the writes down to a sensible size. */
		count = (items > SENSIBLE_SIZE) ? SENSIBLE_SIZE : items ;

		count = write (psf->filedes, ((char*) ptr) + total, count) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0)
			break ;

		total += count ;
		items -= count ;
		} ;

	return total / bytes ;
} /* psf_fwrite */

/* Win32 */ sf_count_t
psf_ftell (SF_PRIVATE *psf)
{	sf_count_t pos ;

	pos = _telli64 (psf->filedes) ;

	if (pos == ((sf_count_t) -1))
	{	psf_log_syserr (psf, errno) ;
		return -1 ;
		} ;

	return pos - psf->fileoffset ;
} /* psf_ftell */

/* Win32 */ int
psf_fclose (SF_PRIVATE *psf)
{	int retval ;

	while ((retval = close (psf->filedes)) == -1 && errno == EINTR)
		/* Do nothing. */ ;

	if (retval == -1)
		psf_log_syserr (psf, errno) ;

	psf->filedes = -1 ;

	return retval ;
} /* psf_fclose */

/* Win32 */ sf_count_t
psf_fgets (char *buffer, sf_count_t bufsize, SF_PRIVATE *psf)
{	sf_count_t	k = 0 ;
	sf_count_t	count ;

	while (k < bufsize - 1)
	{	count = read (psf->filedes, &(buffer [k]), 1) ;

		if (count == -1)
		{	if (errno == EINTR)
				continue ;

			psf_log_syserr (psf, errno) ;
			break ;
			} ;

		if (count == 0 || buffer [k++] == '\n')
			break ;
		} ;

	buffer [k] = 0 ;

	return k ;
} /* psf_fgets */

/* Win32 */ int
psf_is_pipe (SF_PRIVATE *psf)
{	struct stat statbuf ;

	/* Not sure if this works. */

	if (fstat (psf->filedes, &statbuf) == -1)
	{	psf_log_syserr (psf, errno) ;
		/* Default to maximum safety. */
		return SF_TRUE ;
		} ;

	/* These macros are defined in Win32/unistd.h. */
	if (S_ISFIFO (statbuf.st_mode) || S_ISSOCK (statbuf.st_mode))
		return SF_TRUE ;

	return SF_FALSE ;
} /* psf_checkpipe */

/* Win32 */ sf_count_t
psf_get_filelen (SF_PRIVATE *psf)
{
#if 0
	/*
	** Windoze is SOOOOO FUCKED!!!!!!!
	** This code should work but doesn't. Why?
	** Code below does work.
	*/
	struct _stati64 statbuf ;

	if (_fstati64 (psf->filedes, &statbuf))
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	return statbuf.st_size ;
#else
	sf_count_t current, filelen ;

	if ((current = _telli64 (psf->filedes)) < 0)
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	/*
	** Lets face it, windoze if FUBAR!!!
	**
	** For some reason, I have to call _lseeki64() TWICE to get to the
	** end of the file.
	**
	** This might have been avoided if windows had implemented the POSIX
	** standard function fsync() but NO, that would have been too easy.
	**
	** I am VERY close to saying that windoze will no longer be supported
	** by libsndfile and changing the license to GPL at the same time.
	*/

	_lseeki64 (psf->filedes, 0, SEEK_END) ;

	if ((filelen = _lseeki64 (psf->filedes, 0, SEEK_END)) < 0)
	{	psf_log_syserr (psf, errno) ;
		return (sf_count_t) -1 ;
		} ;

	if (filelen > current)
		_lseeki64 (psf->filedes, current, SEEK_SET) ;

	switch (psf->mode)
	{	case SFM_WRITE :
			filelen = filelen - psf->fileoffset ;
			break ;

		case SFM_READ :
			if (psf->fileoffset > 0 && psf->filelength > 0)
				filelen = psf->filelength ;
			break ;

		case SFM_RDWR :
			/*
			** Cannot open embedded files SFM_RDWR so we don't need to
			** subtract psf->fileoffset. We already have the answer we
			** need.
			*/
			break ;

		default :
			filelen = 0 ;
		} ;

	return filelen ;
#endif
} /* psf_get_filelen */

/* Win32 */ int
psf_ftruncate (SF_PRIVATE *psf, sf_count_t len)
{	int retval ;

	/* Returns 0 on success, non-zero on failure. */
	if (len < 0)
		return 1 ;

	/* The global village idiots at micorsoft decided to implement
	** nearly all the required 64 bit file offset functions except
	** for one, truncate. The fscking morons!
	**
	** This is not 64 bit file offset clean. Somone needs to clean
	** this up.
	*/
	if (len > 0x7FFFFFFF)
		return -1 ;

	retval = chsize (psf->filedes, len) ;

	if (retval == -1)
		psf_log_syserr (psf, errno) ;

	return retval ;
} /* psf_ftruncate */


static void
psf_log_syserr (SF_PRIVATE *psf, int error)
{
	/* Only log an error if no error has been set yet. */
	if (psf->error == 0)
	{	psf->error = SFE_SYSTEM ;
		LSF_SNPRINTF (psf->syserr, sizeof (psf->syserr), "System error : %s", strerror (error)) ;
		} ;

	return ;
} /* psf_log_syserr */

#endif

/*==============================================================================
*/

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 749740d7-ecc7-47bd-8cf7-600f31d32e6d
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


#if CPU_IS_LITTLE_ENDIAN
	#define FLOAT32_READ	float32_le_read
	#define FLOAT32_WRITE	float32_le_write
#elif CPU_IS_BIG_ENDIAN
	#define FLOAT32_READ	float32_be_read
	#define FLOAT32_WRITE	float32_be_write
#endif

/*--------------------------------------------------------------------------------------------
**	Processor floating point capabilities. float32_get_capability () returns one of the
**	latter four values.
*/

enum
{	FLOAT_UNKNOWN		= 0x00,
	FLOAT_CAN_RW_LE		= 0x12,
	FLOAT_CAN_RW_BE		= 0x23,
	FLOAT_BROKEN_LE		= 0x34,
	FLOAT_BROKEN_BE		= 0x45
} ;

/*--------------------------------------------------------------------------------------------
**	Prototypes for private functions.
*/

static sf_count_t	host_read_f2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	host_read_f2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	host_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	host_read_f2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	host_write_s2f	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	host_write_i2f	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	host_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	host_write_d2f	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	void	f2s_array 	(float *src, int count, short *dest) ;
static	void	f2i_array 	(float *src, int count, int *dest) ;
static	void	float32f2d_array 	(float *src, int count, double *dest) ;

static 	void	s2f_array 	(short *src, float *dest, int count) ;
static 	void	i2f_array 	(int *src, float *dest, int count) ;
static 	void	float32d2f_array 	(double *src, float *dest, int count) ;

static void		float32_peak_update	(SF_PRIVATE *psf, float *buffer, int count, int indx) ;

static sf_count_t	replace_read_f2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	replace_read_f2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	replace_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	replace_read_f2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	replace_write_s2f	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	replace_write_i2f	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	replace_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	replace_write_d2f	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	void	bf2f_array (float *buffer, int count) ;
static	void	f2bf_array (float *buffer, int count) ;

static int		float32_get_capability	(SF_PRIVATE *psf) ;

/*--------------------------------------------------------------------------------------------
**	Exported functions.
*/

int
float32_init	(SF_PRIVATE *psf)
{	static int float_caps ;

	float_caps = float32_get_capability (psf) ;

	psf->blockwidth = sizeof (float) * psf->sf.channels ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	switch (psf->endian + float_caps)
		{	case (SF_ENDIAN_BIG + FLOAT_CAN_RW_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= host_read_f2s ;
					psf->read_int		= host_read_f2i ;
					psf->read_float		= host_read_f ;
					psf->read_double	= host_read_f2d ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_CAN_RW_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= host_read_f2s ;
					psf->read_int		= host_read_f2i ;
					psf->read_float		= host_read_f ;
					psf->read_double	= host_read_f2d ;
					break ;

			case (SF_ENDIAN_BIG + FLOAT_CAN_RW_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= host_read_f2s ;
					psf->read_int		= host_read_f2i ;
					psf->read_float		= host_read_f ;
					psf->read_double	= host_read_f2d ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_CAN_RW_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= host_read_f2s ;
					psf->read_int		= host_read_f2i ;
					psf->read_float		= host_read_f ;
					psf->read_double	= host_read_f2d ;
					break ;

			/* When the CPU is not IEEE compatible. */
			case (SF_ENDIAN_BIG + FLOAT_BROKEN_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= replace_read_f2s ;
					psf->read_int		= replace_read_f2i ;
					psf->read_float		= replace_read_f ;
					psf->read_double	= replace_read_f2d ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_BROKEN_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= replace_read_f2s ;
					psf->read_int		= replace_read_f2i ;
					psf->read_float		= replace_read_f ;
					psf->read_double	= replace_read_f2d ;
					break ;

			case (SF_ENDIAN_BIG + FLOAT_BROKEN_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->read_short		= replace_read_f2s ;
					psf->read_int		= replace_read_f2i ;
					psf->read_float		= replace_read_f ;
					psf->read_double	= replace_read_f2d ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_BROKEN_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->read_short		= replace_read_f2s ;
					psf->read_int		= replace_read_f2i ;
					psf->read_float		= replace_read_f ;
					psf->read_double	= replace_read_f2d ;
					break ;

			default : break ;
			} ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	switch (psf->endian + float_caps)
		{	case (SF_ENDIAN_LITTLE + FLOAT_CAN_RW_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= host_write_s2f ;
					psf->write_int		= host_write_i2f ;
					psf->write_float	= host_write_f ;
					psf->write_double	= host_write_d2f ;
					break ;

			case (SF_ENDIAN_BIG + FLOAT_CAN_RW_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= host_write_s2f ;
					psf->write_int		= host_write_i2f ;
					psf->write_float	= host_write_f ;
					psf->write_double	= host_write_d2f ;
					break ;

			case (SF_ENDIAN_BIG + FLOAT_CAN_RW_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= host_write_s2f ;
					psf->write_int		= host_write_i2f ;
					psf->write_float	= host_write_f ;
					psf->write_double	= host_write_d2f ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_CAN_RW_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= host_write_s2f ;
					psf->write_int		= host_write_i2f ;
					psf->write_float	= host_write_f ;
					psf->write_double	= host_write_d2f ;
					break ;

			/* When the CPU is not IEEE compatible. */
			case (SF_ENDIAN_BIG + FLOAT_BROKEN_LE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= replace_write_s2f ;
					psf->write_int		= replace_write_i2f ;
					psf->write_float	= replace_write_f ;
					psf->write_double	= replace_write_d2f ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_BROKEN_LE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= replace_write_s2f ;
					psf->write_int		= replace_write_i2f ;
					psf->write_float	= replace_write_f ;
					psf->write_double	= replace_write_d2f ;
					break ;

			case (SF_ENDIAN_BIG + FLOAT_BROKEN_BE) :
					psf->float_endswap = SF_FALSE ;
					psf->write_short	= replace_write_s2f ;
					psf->write_int		= replace_write_i2f ;
					psf->write_float	= replace_write_f ;
					psf->write_double	= replace_write_d2f ;
					break ;

			case (SF_ENDIAN_LITTLE + FLOAT_BROKEN_BE) :
					psf->float_endswap = SF_TRUE ;
					psf->write_short	= replace_write_s2f ;
					psf->write_int		= replace_write_i2f ;
					psf->write_float	= replace_write_f ;
					psf->write_double	= replace_write_d2f ;
					break ;

			default : break ;
			} ;
		} ;

	if (psf->filelength > psf->dataoffset)
	{	psf->datalength = (psf->dataend > 0) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
		}
	else
		psf->datalength = 0 ;

	psf->sf.frames = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* float32_init */

float
float32_be_read (unsigned char *cptr)
{	int		exponent, mantissa, negative ;
	float	fvalue ;

	negative = cptr [0] & 0x80 ;
	exponent = ((cptr [0] & 0x7F) << 1) | ((cptr [1] & 0x80) ? 1 : 0) ;
	mantissa = ((cptr [1] & 0x7F) << 16) | (cptr [2] << 8) | (cptr [3]) ;

	if (! (exponent || mantissa))
		return 0.0 ;

	mantissa |= 0x800000 ;
	exponent = exponent ? exponent - 127 : 0 ;

	fvalue = mantissa ? ((float) mantissa) / ((float) 0x800000) : 0.0 ;

	if (negative)
		fvalue *= -1 ;

	if (exponent > 0)
		fvalue *= (1 << exponent) ;
	else if (exponent < 0)
		fvalue /= (1 << abs (exponent)) ;

	return fvalue ;
} /* float32_be_read */

float
float32_le_read (unsigned char *cptr)
{	int		exponent, mantissa, negative ;
	float	fvalue ;

	negative = cptr [3] & 0x80 ;
	exponent = ((cptr [3] & 0x7F) << 1) | ((cptr [2] & 0x80) ? 1 : 0) ;
	mantissa = ((cptr [2] & 0x7F) << 16) | (cptr [1] << 8) | (cptr [0]) ;

	if (! (exponent || mantissa))
		return 0.0 ;

	mantissa |= 0x800000 ;
	exponent = exponent ? exponent - 127 : 0 ;

	fvalue = mantissa ? ((float) mantissa) / ((float) 0x800000) : 0.0 ;

	if (negative)
		fvalue *= -1 ;

	if (exponent > 0)
		fvalue *= (1 << exponent) ;
	else if (exponent < 0)
		fvalue /= (1 << abs (exponent)) ;

	return fvalue ;
} /* float32_le_read */

void
float32_le_write (float in, unsigned char *out)
{	int		exponent, mantissa, negative = 0 ;

	memset (out, 0, sizeof (int)) ;

	if (in == 0.0)
		return ;

	if (in < 0.0)
	{	in *= -1.0 ;
		negative = 1 ;
		} ;

	in = frexp (in, &exponent) ;

	exponent += 126 ;

	in *= (float) 0x1000000 ;
	mantissa = (((int) in) & 0x7FFFFF) ;

	if (negative)
		out [3] |= 0x80 ;

	if (exponent & 0x01)
		out [2] |= 0x80 ;

	out [0] = mantissa & 0xFF ;
	out [1] = (mantissa >> 8) & 0xFF ;
	out [2] |= (mantissa >> 16) & 0x7F ;
	out [3] |= (exponent >> 1) & 0x7F ;

	return ;
} /* float32_le_write */

void
float32_be_write (float in, unsigned char *out)
{	int		exponent, mantissa, negative = 0 ;

	memset (out, 0, sizeof (int)) ;

	if (in == 0.0)
		return ;

	if (in < 0.0)
	{	in *= -1.0 ;
		negative = 1 ;
		} ;

	in = frexp (in, &exponent) ;

	exponent += 126 ;

	in *= (float) 0x1000000 ;
	mantissa = (((int) in) & 0x7FFFFF) ;

	if (negative)
		out [0] |= 0x80 ;

	if (exponent & 0x01)
		out [1] |= 0x80 ;

	out [3] = mantissa & 0xFF ;
	out [2] = (mantissa >> 8) & 0xFF ;
	out [1] |= (mantissa >> 16) & 0x7F ;
	out [0] |= (exponent >> 1) & 0x7F ;

	return ;
} /* float32_be_write */

/*==============================================================================================
**	Private functions.
*/

static void
float32_peak_update	(SF_PRIVATE *psf, float *buffer, int count, int indx)
{	int 	chan ;
	int		k, position ;
	float	fmaxval ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	fmaxval = fabs (buffer [chan]) ;
		position = 0 ;
		for (k = chan ; k < count ; k += psf->sf.channels)
			if (fmaxval < fabs (buffer [k]))
			{	fmaxval = fabs (buffer [k]) ;
				position = k ;
				} ;

		if (fmaxval > psf->pchunk->peaks [chan].value)
		{	psf->pchunk->peaks [chan].value = fmaxval ;
			psf->pchunk->peaks [chan].position = psf->write_current + indx + (position / psf->sf.channels) ;
			} ;
		} ;

	return ;
} /* float32_peak_update */

static int
float32_get_capability	(SF_PRIVATE *psf)
{	union
	{	float			f ;
		int				i ;
		unsigned char	c [4] ;
	} data ;

	data.f = (float) 1.23456789 ; /* Some abitrary value. */

	if (! psf->ieee_replace)
	{	/* If this test is true ints and floats are compatible and little endian. */
		if (data.c [0] == 0x52 && data.c [1] == 0x06 && data.c [2] == 0x9e && data.c [3] == 0x3f)
			return FLOAT_CAN_RW_LE ;

		/* If this test is true ints and floats are compatible and big endian. */
		if (data.c [3] == 0x52 && data.c [2] == 0x06 && data.c [1] == 0x9e && data.c [0] == 0x3f)
			return FLOAT_CAN_RW_BE ;
		} ;

	/* Floats are broken. Don't expect reading or writing to be fast. */
	psf_log_printf (psf, "Using IEEE replacement code for float.\n") ;

	return (CPU_IS_LITTLE_ENDIAN) ? FLOAT_BROKEN_LE : FLOAT_BROKEN_BE ;
} /* float32_get_capability */

/*----------------------------------------------------------------------------------------------
*/

static sf_count_t
host_read_f2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

/* Fix me : Need lef2s_array */
		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

		f2s_array ((float*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* host_read_f2s */

static sf_count_t
host_read_f2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

		f2i_array ((float*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* host_read_f2i */

static sf_count_t
host_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	if (psf->float_endswap != SF_TRUE)
		return psf_fread (ptr, sizeof (float), len, psf) ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		endswap_int_copy ((int*) (ptr + total), (int*) psf->buffer, thisread) ;

		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* host_read_f */

static sf_count_t
host_read_f2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

/* Fix me : Need lefloat32f2d_array */
		float32f2d_array ((float*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* host_read_f2d */

static sf_count_t
host_write_s2f	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2f_array (ptr + total, (float*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			float32_peak_update (psf, (float *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* host_write_s2f */

static sf_count_t
host_write_i2f	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2f_array (ptr + total, (float*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			float32_peak_update (psf, (float *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float) , writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* host_write_i2f */

static sf_count_t
host_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if (psf->has_peak)
		float32_peak_update (psf, ptr, len, 0) ;

	if (psf->float_endswap != SF_TRUE)
		return psf_fwrite (ptr, sizeof (float), len, psf) ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;

		endswap_int_copy ((int*) psf->buffer, (int*) (ptr + total), writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* host_write_f */

static sf_count_t
host_write_d2f	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;

		float32d2f_array (ptr + total, (float*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			float32_peak_update (psf, (float *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* host_write_d2f */

/*=======================================================================================
*/

static void
f2s_array (float *src, int count, short *dest)
{	while (count)
	{	count -- ;
		dest [count] = lrintf (src [count]) ;
		} ;
} /* f2s_array */

static void
f2i_array (float *src, int count, int *dest)
{	while (count)
	{	count -- ;
		dest [count] = lrintf (src [count]) ;
		} ;
} /* f2i_array */

static void
float32f2d_array (float *src, int count, double *dest)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;
} /* float32f2d_array */

static void
s2f_array (short *src, float *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;

} /* s2f_array */

static void
i2f_array (int *src, float *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;
} /* i2f_array */

static void
float32d2f_array (double *src, float *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] ;
		} ;
} /* float32d2f_array */

/*=======================================================================================
*/

static sf_count_t
replace_read_f2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

		bf2f_array ((float *) (psf->buffer), readcount) ;

		f2s_array ((float*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_f2s */

static sf_count_t
replace_read_f2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

		bf2f_array ((float *) (psf->buffer), readcount) ;

		f2i_array ((float*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_f2i */

static sf_count_t
replace_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	/* FIX THIS */

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

		bf2f_array ((float *) (psf->buffer), readcount) ;

		memcpy (ptr + total, psf->buffer, readcount * sizeof (float)) ;

		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_f */

static sf_count_t
replace_read_f2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (float), readcount, psf) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, readcount) ;

		bf2f_array ((float *) (psf->buffer), readcount) ;

		float32f2d_array ((float*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* replace_read_f2d */

static sf_count_t
replace_write_s2f	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2f_array (ptr + total, (float*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			float32_peak_update (psf, (float *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		f2bf_array ((float *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_s2f */

static sf_count_t
replace_write_i2f	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2f_array (ptr + total, (float*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			float32_peak_update (psf, (float *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		f2bf_array ((float *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_i2f */

static sf_count_t
replace_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	/* FIX THIS */
	if (psf->has_peak)
		float32_peak_update (psf, ptr, len, 0) ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;

		memcpy (psf->buffer, ptr + total, writecount * sizeof (float)) ;

		f2bf_array ((float *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float) , writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_f */

static sf_count_t
replace_write_d2f	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			writecount, bufferlen, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (float) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		float32d2f_array (ptr + total, (float*) (psf->buffer), writecount) ;

		if (psf->has_peak)
			float32_peak_update (psf, (float *) (psf->buffer), writecount, (int) (total / psf->sf.channels)) ;

		f2bf_array ((float *) (psf->buffer), writecount) ;

		if (psf->float_endswap == SF_TRUE)
			endswap_int_array ((int*) psf->buffer, writecount) ;

		thiswrite = psf_fwrite (psf->buffer, sizeof (float), writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* replace_write_d2f */

/*----------------------------------------------------------------------------------------------
*/

static void
bf2f_array (float *buffer, int count)
{	while (count)
	{	count -- ;
		buffer [count] = FLOAT32_READ ((unsigned char *) (buffer + count)) ;
		} ;
} /* bf2f_array */

static void
f2bf_array (float *buffer, int count)
{	while (count)
	{	count -- ;
		FLOAT32_WRITE (buffer [count], (unsigned char*) (buffer + count)) ;
		} ;
} /* f2bf_array */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: b6c34917-488c-4145-9648-f4371fc4c889
*/
/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * g721.c
 *
 * Description:
 *
 * g721_encoder(), g721_decoder()
 *
 * These routines comprise an implementation of the CCITT G.721 ADPCM
 * coding algorithm.  Essentially, this implementation is identical to
 * the bit level description except for a few deviations which
 * take advantage of work station attributes, such as hardware 2's
 * complement arithmetic and large memory.  Specifically, certain time
 * consuming operations such as multiplications are replaced
 * with lookup tables and software 2's complement operations are
 * replaced with hardware 2's complement.
 *
 * The deviation from the bit level specification (lookup tables)
 * preserves the bit level performance specifications.
 *
 * As outlined in the G.721 Recommendation, the algorithm is broken
 * down into modules.  Each section of code below is preceded by
 * the name of the module which it is implementing.
 *
 */


static short qtab_721[7] = {-124, 80, 178, 246, 300, 349, 400};
/*
 * Maps G.721 code word to reconstructed scale factor normalized log
 * magnitude values.
 */
static short	g721_dqlntab[16] = {-2048, 4, 135, 213, 273, 323, 373, 425,
				425, 373, 323, 273, 213, 135, 4, -2048};

/* Maps G.721 code word to log of scale factor multiplier. */
static short	g721_witab[16] = {-12, 18, 41, 64, 112, 198, 355, 1122,
				1122, 355, 198, 112, 64, 41, 18, -12};
/*
 * Maps G.721 code words to a set of values whose long and short
 * term averages are computed and then compared to give an indication
 * how stationary (steady state) the signal is.
 */
static short	g721_fitab[16] = {0, 0, 0, 0x200, 0x200, 0x200, 0x600, 0xE00,
				0xE00, 0x600, 0x200, 0x200, 0x200, 0, 0, 0};

/*
 * g721_encoder()
 *
 * Encodes the input vale of linear PCM, A-law or u-law data sl and returns
 * the resulting code. -1 is returned for unknown input coding value.
 */
int
g721_encoder(
	int		sl,
	G72x_STATE *state_ptr)
{
	short		sezi, se, sez;		/* ACCUM */
	short		d;			/* SUBTA */
	short		sr;			/* ADDB */
	short		y;			/* MIX */
	short		dqsez;			/* ADDC */
	short		dq, i;

	/* linearize input sample to 14-bit PCM */
	sl >>= 2;			/* 14-bit dynamic range */

	sezi = predictor_zero(state_ptr);
	sez = sezi >> 1;
	se = (sezi + predictor_pole(state_ptr)) >> 1;	/* estimated signal */

	d = sl - se;				/* estimation difference */

	/* quantize the prediction difference */
	y = step_size(state_ptr);		/* quantizer step size */
	i = quantize(d, y, qtab_721, 7);	/* i = ADPCM code */

	dq = reconstruct(i & 8, g721_dqlntab[i], y);	/* quantized est diff */

	sr = (dq < 0) ? se - (dq & 0x3FFF) : se + dq;	/* reconst. signal */

	dqsez = sr + sez - se;			/* pole prediction diff. */

	update(4, y, g721_witab[i] << 5, g721_fitab[i], dq, sr, dqsez, state_ptr);

	return (i);
}

/*
 * g721_decoder()
 *
 * Description:
 *
 * Decodes a 4-bit code of G.721 encoded data of i and
 * returns the resulting linear PCM, A-law or u-law value.
 * return -1 for unknown out_coding value.
 */
int
g721_decoder(
	int		i,
	G72x_STATE *state_ptr)
{
	short		sezi, sei, sez, se;	/* ACCUM */
	short		y;			/* MIX */
	short		sr;			/* ADDB */
	short		dq;
	short		dqsez;

	i &= 0x0f;			/* mask to get proper bits */
	sezi = predictor_zero(state_ptr);
	sez = sezi >> 1;
	sei = sezi + predictor_pole(state_ptr);
	se = sei >> 1;			/* se = estimated signal */

	y = step_size(state_ptr);	/* dynamic quantizer step size */

	dq = reconstruct(i & 0x08, g721_dqlntab[i], y); /* quantized diff. */

	sr = (dq < 0) ? (se - (dq & 0x3FFF)) : se + dq;	/* reconst. signal */

	dqsez = sr - se + sez;			/* pole prediction diff. */

	update(4, y, g721_witab[i] << 5, g721_fitab[i], dq, sr, dqsez, state_ptr);

	/* sr was 14-bit dynamic range */
	return (sr << 2);	
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 101b6e25-457d-490a-99ae-e2e74a26ea24
*/

/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
/* 16kbps version created, used 24kbps code and changing as little as possible.
 * G.726 specs are available from ITU's gopher or WWW site (http://www.itu.ch)
 * If any errors are found, please contact me at mrand@tamu.edu
 *      -Marc Randolph
 */

/*
 * g723_16.c
 *
 * Description:
 *
 * g723_16_encoder(), g723_16_decoder()
 *
 * These routines comprise an implementation of the CCITT G.726 16 Kbps
 * ADPCM coding algorithm.  Essentially, this implementation is identical to
 * the bit level description except for a few deviations which take advantage
 * of workstation attributes, such as hardware 2's complement arithmetic.
 *
 */


/*
 * Maps G.723_16 code word to reconstructed scale factor normalized log
 * magnitude values.  Comes from Table 11/G.726
 */
static short   g723_16_dqlntab[4] = { 116, 365, 365, 116}; 

/* Maps G.723_16 code word to log of scale factor multiplier.
 *
 * g723_16_witab[4] is actually {-22 , 439, 439, -22}, but FILTD wants it
 * as WI << 5  (multiplied by 32), so we'll do that here 
 */
static short   g723_16_witab[4] = {-704, 14048, 14048, -704};

/*
 * Maps G.723_16 code words to a set of values whose long and short
 * term averages are computed and then compared to give an indication
 * how stationary (steady state) the signal is.
 */

/* Comes from FUNCTF */
static short   g723_16_fitab[4] = {0, 0xE00, 0xE00, 0};

/* Comes from quantizer decision level tables (Table 7/G.726)
 */
static short qtab_723_16[1] = {261};


/*
 * g723_16_encoder()
 *
 * Encodes a linear PCM, A-law or u-law input sample and returns its 2-bit code.
 * Returns -1 if invalid input coding value.
 */
int
g723_16_encoder(
       int             sl,
       G72x_STATE *state_ptr)
{
       short           sei, sezi, se, sez;     /* ACCUM */
       short           d;                      /* SUBTA */
       short           y;                      /* MIX */
       short           sr;                     /* ADDB */
       short           dqsez;                  /* ADDC */
       short           dq, i;

		/* linearize input sample to 14-bit PCM */
		sl >>= 2;               /* sl of 14-bit dynamic range */

       sezi = predictor_zero(state_ptr);
       sez = sezi >> 1;
       sei = sezi + predictor_pole(state_ptr);
       se = sei >> 1;                  /* se = estimated signal */

       d = sl - se;                    /* d = estimation diff. */

       /* quantize prediction difference d */
       y = step_size(state_ptr);       /* quantizer step size */
       i = quantize(d, y, qtab_723_16, 1);  /* i = ADPCM code */

             /* Since quantize() only produces a three level output
              * (1, 2, or 3), we must create the fourth one on our own
              */
       if (i == 3)                          /* i code for the zero region */
         if ((d & 0x8000) == 0)             /* If d > 0, i=3 isn't right... */
           i = 0;
           
       dq = reconstruct(i & 2, g723_16_dqlntab[i], y); /* quantized diff. */

       sr = (dq < 0) ? se - (dq & 0x3FFF) : se + dq; /* reconstructed signal */

       dqsez = sr + sez - se;          /* pole prediction diff. */

       update(2, y, g723_16_witab[i], g723_16_fitab[i], dq, sr, dqsez, state_ptr);

       return (i);
}

/*
 * g723_16_decoder()
 *
 * Decodes a 2-bit CCITT G.723_16 ADPCM code and returns
 * the resulting 16-bit linear PCM, A-law or u-law sample value.
 * -1 is returned if the output coding is unknown.
 */
int
g723_16_decoder(
       int             i,
       G72x_STATE *state_ptr)
{
       short           sezi, sei, sez, se;     /* ACCUM */
       short           y;                      /* MIX */
       short           sr;                     /* ADDB */
       short           dq;
       short           dqsez;

       i &= 0x03;                      /* mask to get proper bits */
       sezi = predictor_zero(state_ptr);
       sez = sezi >> 1;
       sei = sezi + predictor_pole(state_ptr);
       se = sei >> 1;                  /* se = estimated signal */

       y = step_size(state_ptr);       /* adaptive quantizer step size */
       dq = reconstruct(i & 0x02, g723_16_dqlntab[i], y); /* unquantize pred diff */

       sr = (dq < 0) ? (se - (dq & 0x3FFF)) : (se + dq); /* reconst. signal */

       dqsez = sr - se + sez;                  /* pole prediction diff. */

       update(2, y, g723_16_witab[i], g723_16_fitab[i], dq, sr, dqsez, state_ptr);

		/* sr was of 14-bit dynamic range */
		return (sr << 2);       
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: ae265466-c3fc-4f83-bb32-edae488a5ca5
*/

/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * g723_24.c
 *
 * Description:
 *
 * g723_24_encoder(), g723_24_decoder()
 *
 * These routines comprise an implementation of the CCITT G.723 24 Kbps
 * ADPCM coding algorithm.  Essentially, this implementation is identical to
 * the bit level description except for a few deviations which take advantage
 * of workstation attributes, such as hardware 2's complement arithmetic.
 *
 */


/*
 * Maps G.723_24 code word to reconstructed scale factor normalized log
 * magnitude values.
 */
static short	g723_24_dqlntab[8] = {-2048, 135, 273, 373, 373, 273, 135, -2048};

/* Maps G.723_24 code word to log of scale factor multiplier. */
static short	g723_24_witab[8] = {-128, 960, 4384, 18624, 18624, 4384, 960, -128};

/*
 * Maps G.723_24 code words to a set of values whose long and short
 * term averages are computed and then compared to give an indication
 * how stationary (steady state) the signal is.
 */
static short	g723_24_fitab[8] = {0, 0x200, 0x400, 0xE00, 0xE00, 0x400, 0x200, 0};

static short qtab_723_24[3] = {8, 218, 331};

/*
 * g723_24_encoder()
 *
 * Encodes a linear PCM, A-law or u-law input sample and returns its 3-bit code.
 * Returns -1 if invalid input coding value.
 */
int
g723_24_encoder(
	int		sl,
	G72x_STATE *state_ptr)
{
	short		sei, sezi, se, sez;	/* ACCUM */
	short		d;			/* SUBTA */
	short		y;			/* MIX */
	short		sr;			/* ADDB */
	short		dqsez;			/* ADDC */
	short		dq, i;

	/* linearize input sample to 14-bit PCM */
	sl >>= 2;		/* sl of 14-bit dynamic range */

	sezi = predictor_zero(state_ptr);
	sez = sezi >> 1;
	sei = sezi + predictor_pole(state_ptr);
	se = sei >> 1;			/* se = estimated signal */

	d = sl - se;			/* d = estimation diff. */

	/* quantize prediction difference d */
	y = step_size(state_ptr);	/* quantizer step size */
	i = quantize(d, y, qtab_723_24, 3);	/* i = ADPCM code */
	dq = reconstruct(i & 4, g723_24_dqlntab[i], y); /* quantized diff. */

	sr = (dq < 0) ? se - (dq & 0x3FFF) : se + dq; /* reconstructed signal */

	dqsez = sr + sez - se;		/* pole prediction diff. */

	update(3, y, g723_24_witab[i], g723_24_fitab[i], dq, sr, dqsez, state_ptr);

	return (i);
}

/*
 * g723_24_decoder()
 *
 * Decodes a 3-bit CCITT G.723_24 ADPCM code and returns
 * the resulting 16-bit linear PCM, A-law or u-law sample value.
 * -1 is returned if the output coding is unknown.
 */
int
g723_24_decoder(
	int		i,
	G72x_STATE *state_ptr)
{
	short		sezi, sei, sez, se;	/* ACCUM */
	short		y;			/* MIX */
	short		sr;			/* ADDB */
	short		dq;
	short		dqsez;

	i &= 0x07;			/* mask to get proper bits */
	sezi = predictor_zero(state_ptr);
	sez = sezi >> 1;
	sei = sezi + predictor_pole(state_ptr);
	se = sei >> 1;			/* se = estimated signal */

	y = step_size(state_ptr);	/* adaptive quantizer step size */
	dq = reconstruct(i & 0x04, g723_24_dqlntab[i], y); /* unquantize pred diff */

	sr = (dq < 0) ? (se - (dq & 0x3FFF)) : (se + dq); /* reconst. signal */

	dqsez = sr - se + sez;			/* pole prediction diff. */

	update(3, y, g723_24_witab[i], g723_24_fitab[i], dq, sr, dqsez, state_ptr);

	return (sr << 2);	/* sr was of 14-bit dynamic range */
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 75389236-650b-4427-98f3-0df6e8fb24bc
*/

/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * g723_40.c
 *
 * Description:
 *
 * g723_40_encoder(), g723_40_decoder()
 *
 * These routines comprise an implementation of the CCITT G.723 40Kbps
 * ADPCM coding algorithm.  Essentially, this implementation is identical to
 * the bit level description except for a few deviations which
 * take advantage of workstation attributes, such as hardware 2's
 * complement arithmetic.
 *
 * The deviation from the bit level specification (lookup tables),
 * preserves the bit level performance specifications.
 *
 * As outlined in the G.723 Recommendation, the algorithm is broken
 * down into modules.  Each section of code below is preceded by
 * the name of the module which it is implementing.
 *
 */


/*
 * Maps G.723_40 code word to ructeconstructed scale factor normalized log
 * magnitude values.
 */
static short	g723_40_dqlntab[32] = {-2048, -66, 28, 104, 169, 224, 274, 318,
				358, 395, 429, 459, 488, 514, 539, 566,
				566, 539, 514, 488, 459, 429, 395, 358,
				318, 274, 224, 169, 104, 28, -66, -2048};

/* Maps G.723_40 code word to log of scale factor multiplier. */
static short	g723_40_witab[32] = {448, 448, 768, 1248, 1280, 1312, 1856, 3200,
			4512, 5728, 7008, 8960, 11456, 14080, 16928, 22272,
			22272, 16928, 14080, 11456, 8960, 7008, 5728, 4512,
			3200, 1856, 1312, 1280, 1248, 768, 448, 448};

/*
 * Maps G.723_40 code words to a set of values whose long and short
 * term averages are computed and then compared to give an indication
 * how stationary (steady state) the signal is.
 */
static short	g723_40_fitab[32] = {0, 0, 0, 0, 0, 0x200, 0x200, 0x200,
			0x200, 0x200, 0x400, 0x600, 0x800, 0xA00, 0xC00, 0xC00,
			0xC00, 0xC00, 0xA00, 0x800, 0x600, 0x400, 0x200, 0x200,
			0x200, 0x200, 0x200, 0, 0, 0, 0, 0};

static short qtab_723_40[15] = {-122, -16, 68, 139, 198, 250, 298, 339,
				378, 413, 445, 475, 502, 528, 553};

/*
 * g723_40_encoder()
 *
 * Encodes a 16-bit linear PCM, A-law or u-law input sample and retuens
 * the resulting 5-bit CCITT G.723 40Kbps code.
 * Returns -1 if the input coding value is invalid.
 */
int	g723_40_encoder (int sl, G72x_STATE *state_ptr)
{
	short		sei, sezi, se, sez;	/* ACCUM */
	short		d;			/* SUBTA */
	short		y;			/* MIX */
	short		sr;			/* ADDB */
	short		dqsez;			/* ADDC */
	short		dq, i;

	/* linearize input sample to 14-bit PCM */
	sl >>= 2;		/* sl of 14-bit dynamic range */

	sezi = predictor_zero(state_ptr);
	sez = sezi >> 1;
	sei = sezi + predictor_pole(state_ptr);
	se = sei >> 1;			/* se = estimated signal */

	d = sl - se;			/* d = estimation difference */

	/* quantize prediction difference */
	y = step_size(state_ptr);	/* adaptive quantizer step size */
	i = quantize(d, y, qtab_723_40, 15);	/* i = ADPCM code */

	dq = reconstruct(i & 0x10, g723_40_dqlntab[i], y);	/* quantized diff */

	sr = (dq < 0) ? se - (dq & 0x7FFF) : se + dq; /* reconstructed signal */

	dqsez = sr + sez - se;		/* dqsez = pole prediction diff. */

	update(5, y, g723_40_witab[i], g723_40_fitab[i], dq, sr, dqsez, state_ptr);

	return (i);
}

/*
 * g723_40_decoder()
 *
 * Decodes a 5-bit CCITT G.723 40Kbps code and returns
 * the resulting 16-bit linear PCM, A-law or u-law sample value.
 * -1 is returned if the output coding is unknown.
 */
int	g723_40_decoder	(int i, G72x_STATE *state_ptr)
{
	short		sezi, sei, sez, se;	/* ACCUM */
	short		y ;			/* MIX */
	short		sr;			/* ADDB */
	short		dq;
	short		dqsez;

	i &= 0x1f;			/* mask to get proper bits */
	sezi = predictor_zero(state_ptr);
	sez = sezi >> 1;
	sei = sezi + predictor_pole(state_ptr);
	se = sei >> 1;			/* se = estimated signal */

	y = step_size(state_ptr);	/* adaptive quantizer step size */
	dq = reconstruct(i & 0x10, g723_40_dqlntab[i], y);	/* estimation diff. */

	sr = (dq < 0) ? (se - (dq & 0x7FFF)) : (se + dq); /* reconst. signal */

	dqsez = sr - se + sez;		/* pole prediction diff. */

	update(5, y, g723_40_witab[i], g723_40_fitab[i], dq, sr, dqsez, state_ptr);

	return (sr << 2);	/* sr was of 14-bit dynamic range */
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: eb8d9a00-32bf-4dd2-b287-01b0336d72bf
*/

/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * the part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS SOFTWARE
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * g72x.c
 *
 * Common routines for G.721 and G.723 conversions.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static 
short power2 [15] = 
{	1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000
} ;

/*
 * quan()
 *
 * quantizes the input val against the table of size short integers.
 * It returns i if table[i - 1] <= val < table[i].
 *
 * Using linear search for simple coding.
 */
static 
int quan (int val, short *table, int size)
{
	int		i;

	for (i = 0; i < size; i++)
		if (val < *table++)
			break;
	return (i);
}

/*
 * fmult()
 *
 * returns the integer product of the 14-bit integer "an" and
 * "floating point" representation (4-bit exponent, 6-bit mantessa) "srn".
 */
static 
int fmult (int an, int srn)
{
	short		anmag, anexp, anmant;
	short		wanexp, wanmant;
	short		retval;

	anmag = (an > 0) ? an : ((-an) & 0x1FFF);
	anexp = quan(anmag, power2, 15) - 6;
	anmant = (anmag == 0) ? 32 :
	    (anexp >= 0) ? anmag >> anexp : anmag << -anexp;
	wanexp = anexp + ((srn >> 6) & 0xF) - 13;

	/*
	** The original was :
	**		wanmant = (anmant * (srn & 0x37) + 0x30) >> 4 ;
	** but could see no valid reason for the + 0x30.
	** Removed it and it improved the SNR of the codec.
	*/

	wanmant = (anmant * (srn & 0x37)) >> 4 ;
	
	retval = (wanexp >= 0) ? ((wanmant << wanexp) & 0x7FFF) :
	    (wanmant >> -wanexp);

	return (((an ^ srn) < 0) ? -retval : retval);
}

/*
 * private_init_state()
 *
 * This routine initializes and/or resets the G72x_PRIVATE structure
 * pointed to by 'state_ptr'.
 * All the initial state values are specified in the CCITT G.721 document.
 */
void private_init_state (G72x_STATE *state_ptr)
{
	int		cnta;

	state_ptr->yl = 34816;
	state_ptr->yu = 544;
	state_ptr->dms = 0;
	state_ptr->dml = 0;
	state_ptr->ap = 0;
	for (cnta = 0; cnta < 2; cnta++) {
		state_ptr->a[cnta] = 0;
		state_ptr->pk[cnta] = 0;
		state_ptr->sr[cnta] = 32;
	}
	for (cnta = 0; cnta < 6; cnta++) {
		state_ptr->b[cnta] = 0;
		state_ptr->dq[cnta] = 32;
	}
	state_ptr->td = 0;
}	/* private_init_state */

int g72x_reader_init (G72x_DATA *data, int codec)
{	G72x_STATE *pstate ;

	if (sizeof (data->sprivateo) < sizeof (G72x_STATE))
	{	/* This is for safety only. */
		return 1 ;
		} ;

	memset (data, 0, sizeof (G72x_DATA)) ;
	
	pstate = (G72x_STATE*) data->sprivateo ;
	private_init_state (pstate) ;
		
	pstate->encoder = NULL ;
	
	switch (codec)
	{	case G723_16_BITS_PER_SAMPLE : /* 2 bits per sample. */
				pstate->decoder = g723_16_decoder ;
				data->blocksize = G723_16_BYTES_PER_BLOCK ;
				data->samplesperblock = G723_16_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 2 ;
				break ;
				
		case G723_24_BITS_PER_SAMPLE : /* 3 bits per sample. */ 
				pstate->decoder = g723_24_decoder ;
				data->blocksize = G723_24_BYTES_PER_BLOCK ;
				data->samplesperblock = G723_24_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 3 ;
				break ;
				
		case G721_32_BITS_PER_SAMPLE : /* 4 bits per sample. */
				pstate->decoder = g721_decoder ;
				data->blocksize = G721_32_BYTES_PER_BLOCK ;
				data->samplesperblock = G721_32_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 4 ;
				break ;
				
		case G721_40_BITS_PER_SAMPLE : /* 5 bits per sample. */
				pstate->decoder = g723_40_decoder ;
				data->blocksize = G721_40_BYTES_PER_BLOCK ;
				data->samplesperblock = G721_40_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 5 ;
				break ;
				
		default : return 1 ;
		} ;

	return 0 ;
}	/* g72x_reader_init */

int g72x_writer_init (G72x_DATA *data, int codec)
{	G72x_STATE *pstate ;

	if (sizeof (data->sprivateo) < sizeof (G72x_STATE))
	{	/* This is for safety only. Gets optimised out. */
		return 1 ;
		} ;

	memset (data, 0, sizeof (G72x_DATA)) ;
	
	pstate = (G72x_STATE*) data->sprivateo ;
	private_init_state (pstate) ;
		
	pstate->decoder = NULL ;
	
	switch (codec)
	{	case G723_16_BITS_PER_SAMPLE : /* 2 bits per sample. */
				pstate->encoder = g723_16_encoder ;
				data->blocksize = G723_16_BYTES_PER_BLOCK ;
				data->samplesperblock = G723_16_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 2 ;
				break ;
				
		case G723_24_BITS_PER_SAMPLE : /* 3 bits per sample. */ 
				pstate->encoder = g723_24_encoder ;
				data->blocksize = G723_24_BYTES_PER_BLOCK ;
				data->samplesperblock = G723_24_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 3 ;
				break ;
				
		case G721_32_BITS_PER_SAMPLE : /* 4 bits per sample. */
				pstate->encoder = g721_encoder ;
				data->blocksize = G721_32_BYTES_PER_BLOCK ;
				data->samplesperblock = G721_32_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 4 ;
				break ;
				
		case G721_40_BITS_PER_SAMPLE : /* 5 bits per sample. */
				pstate->encoder = g723_40_encoder ;
				data->blocksize = G721_40_BYTES_PER_BLOCK ;
				data->samplesperblock = G721_40_SAMPLES_PER_BLOCK ;
				pstate->codec_bits = 5 ;
				break ;
				
		default : return 1 ;
		} ;

	return 0 ;
}	/* g72x_writer_init */

int unpack_bytes (G72x_DATA *data, int bits)
{	unsigned int    in_buffer = 0 ;
	unsigned char	in_byte ;
	int				k, in_bits = 0, bindex = 0 ;
	
	for (k = 0 ; bindex <= data->blocksize && k < G72x_BLOCK_SIZE ; k++)
	{	if (in_bits < bits) 
		{	in_byte = data->block [bindex++] ;

			in_buffer |= (in_byte << in_bits);
			in_bits += 8;
			}
		data->samples [k] = in_buffer & ((1 << bits) - 1);
		in_buffer >>= bits;
		in_bits -= bits;
		} ;
		
	return k ;
} /* unpack_bytes */

int g72x_decode_block (G72x_DATA *data)
{	G72x_STATE *pstate ;
	int	k, count ;
	
	pstate = (G72x_STATE*) data->sprivateo ;
	
	count = unpack_bytes (data, pstate->codec_bits) ;
	
	for (k = 0 ; k < count ; k++)
		data->samples [k] = pstate->decoder (data->samples [k], pstate) ;
	
	return 0 ;
}	/* g72x_decode_block */

int pack_bytes (G72x_DATA *data, int bits)
{
	unsigned int	out_buffer = 0 ;
	int				k, bindex = 0, out_bits = 0 ;
	unsigned char	out_byte ;

	for (k = 0 ; k < G72x_BLOCK_SIZE ; k++)
	{	out_buffer |= (data->samples [k] << out_bits) ;
		out_bits += bits ;
		if (out_bits >= 8) 
		{	out_byte = out_buffer & 0xFF ;
			out_bits -= 8 ;
			out_buffer >>= 8 ;
			data->block [bindex++] = out_byte ;
			}
		} ;

	return bindex ;
} /* pack_bytes */

int g72x_encode_block (G72x_DATA *data)
{	G72x_STATE *pstate ;
	int k, count ;

	pstate = (G72x_STATE*) data->sprivateo ;
	
	for (k = 0 ; k < data->samplesperblock ; k++)
		data->samples [k] = pstate->encoder (data->samples [k], pstate) ;
	
	count = pack_bytes (data, pstate->codec_bits) ;
	
	return count ;
}	/* g72x_encode_block */

/*
 * predictor_zero()
 *
 * computes the estimated signal from 6-zero predictor.
 *
 */
int  predictor_zero (G72x_STATE *state_ptr)
{
	int		i;
	int		sezi;

	sezi = fmult(state_ptr->b[0] >> 2, state_ptr->dq[0]);
	for (i = 1; i < 6; i++)			/* ACCUM */
		sezi += fmult(state_ptr->b[i] >> 2, state_ptr->dq[i]);
	return (sezi);
}
/*
 * predictor_pole()
 *
 * computes the estimated signal from 2-pole predictor.
 *
 */
int  predictor_pole(G72x_STATE *state_ptr)
{
	return (fmult(state_ptr->a[1] >> 2, state_ptr->sr[1]) +
	    fmult(state_ptr->a[0] >> 2, state_ptr->sr[0]));
}
/*
 * step_size()
 *
 * computes the quantization step size of the adaptive quantizer.
 *
 */
int  step_size (G72x_STATE *state_ptr)
{
	int		y;
	int		dif;
	int		al;

	if (state_ptr->ap >= 256)
		return (state_ptr->yu);
	else {
		y = state_ptr->yl >> 6;
		dif = state_ptr->yu - y;
		al = state_ptr->ap >> 2;
		if (dif > 0)
			y += (dif * al) >> 6;
		else if (dif < 0)
			y += (dif * al + 0x3F) >> 6;
		return (y);
	}
}

/*
 * quantize()
 *
 * Given a raw sample, 'd', of the difference signal and a
 * quantization step size scale factor, 'y', this routine returns the
 * ADPCM codeword to which that sample gets quantized.  The step
 * size scale factor division operation is done in the log base 2 domain
 * as a subtraction.
 */
int quantize(
	int		d,	/* Raw difference signal sample */
	int		y,	/* Step size multiplier */
	short	*table,	/* quantization table */
	int		size)	/* table size of short integers */
{
	short		dqm;	/* Magnitude of 'd' */
	short		expon;	/* Integer part of base 2 log of 'd' */
	short		mant;	/* Fractional part of base 2 log */
	short		dl;	/* Log of magnitude of 'd' */
	short		dln;	/* Step size scale factor normalized log */
	int		i;

	/*
	 * LOG
	 *
	 * Compute base 2 log of 'd', and store in 'dl'.
	 */
	dqm = abs(d);
	expon = quan(dqm >> 1, power2, 15);
	mant = ((dqm << 7) >> expon) & 0x7F;	/* Fractional portion. */
	dl = (expon << 7) + mant;

	/*
	 * SUBTB
	 *
	 * "Divide" by step size multiplier.
	 */
	dln = dl - (y >> 2);

	/*
	 * QUAN
	 *
	 * Obtain codword i for 'd'.
	 */
	i = quan(dln, table, size);
	if (d < 0)			/* take 1's complement of i */
		return ((size << 1) + 1 - i);
	else if (i == 0)		/* take 1's complement of 0 */
		return ((size << 1) + 1); /* new in 1988 */
	else
		return (i);
}
/*
 * reconstruct()
 *
 * Returns reconstructed difference signal 'dq' obtained from
 * codeword 'i' and quantization step size scale factor 'y'.
 * Multiplication is performed in log base 2 domain as addition.
 */
int
reconstruct(
	int		sign,	/* 0 for non-negative value */
	int		dqln,	/* G.72x codeword */
	int		y)	/* Step size multiplier */
{
	short		dql;	/* Log of 'dq' magnitude */
	short		dex;	/* Integer part of log */
	short		dqt;
	short		dq;	/* Reconstructed difference signal sample */

	dql = dqln + (y >> 2);	/* ADDA */

	if (dql < 0) {
		return ((sign) ? -0x8000 : 0);
	} else {		/* ANTILOG */
		dex = (dql >> 7) & 15;
		dqt = 128 + (dql & 127);
		dq = (dqt << 7) >> (14 - dex);
		return ((sign) ? (dq - 0x8000) : dq);
	}
}


/*
 * update()
 *
 * updates the state variables for each output code
 */
void
update(
	int		code_size,	/* distinguish 723_40 with others */
	int		y,		/* quantizer step size */
	int		wi,		/* scale factor multiplier */
	int		fi,		/* for long/short term energies */
	int		dq,		/* quantized prediction difference */
	int		sr,		/* reconstructed signal */
	int		dqsez,		/* difference from 2-pole predictor */
	G72x_STATE *state_ptr)	/* coder state pointer */
{
	int		cnt;
	short		mag, expon;	/* Adaptive predictor, FLOAT A */
	short		a2p = 0;	/* LIMC */
	short		a1ul;		/* UPA1 */
	short		pks1;		/* UPA2 */
	short		fa1;
	char		tr;		/* tone/transition detector */
	short		ylint, thr2, dqthr;
	short  		ylfrac, thr1;
	short		pk0;

	pk0 = (dqsez < 0) ? 1 : 0;	/* needed in updating predictor poles */

	mag = dq & 0x7FFF;		/* prediction difference magnitude */
	/* TRANS */
	ylint = state_ptr->yl >> 15;	/* exponent part of yl */
	ylfrac = (state_ptr->yl >> 10) & 0x1F;	/* fractional part of yl */
	thr1 = (32 + ylfrac) << ylint;		/* threshold */
	thr2 = (ylint > 9) ? 31 << 10 : thr1;	/* limit thr2 to 31 << 10 */
	dqthr = (thr2 + (thr2 >> 1)) >> 1;	/* dqthr = 0.75 * thr2 */
	if (state_ptr->td == 0)		/* signal supposed voice */
		tr = 0;
	else if (mag <= dqthr)		/* supposed data, but small mag */
		tr = 0;			/* treated as voice */
	else				/* signal is data (modem) */
		tr = 1;

	/*
	 * Quantizer scale factor adaptation.
	 */

	/* FUNCTW & FILTD & DELAY */
	/* update non-steady state step size multiplier */
	state_ptr->yu = y + ((wi - y) >> 5);

	/* LIMB */
	if (state_ptr->yu < 544)	/* 544 <= yu <= 5120 */
		state_ptr->yu = 544;
	else if (state_ptr->yu > 5120)
		state_ptr->yu = 5120;

	/* FILTE & DELAY */
	/* update steady state step size multiplier */
	state_ptr->yl += state_ptr->yu + ((-state_ptr->yl) >> 6);

	/*
	 * Adaptive predictor coefficients.
	 */
	if (tr == 1) {			/* reset a's and b's for modem signal */
		state_ptr->a[0] = 0;
		state_ptr->a[1] = 0;
		state_ptr->b[0] = 0;
		state_ptr->b[1] = 0;
		state_ptr->b[2] = 0;
		state_ptr->b[3] = 0;
		state_ptr->b[4] = 0;
		state_ptr->b[5] = 0;
	} else {			/* update a's and b's */
		pks1 = pk0 ^ state_ptr->pk[0];		/* UPA2 */

		/* update predictor pole a[1] */
		a2p = state_ptr->a[1] - (state_ptr->a[1] >> 7);
		if (dqsez != 0) {
			fa1 = (pks1) ? state_ptr->a[0] : -state_ptr->a[0];
			if (fa1 < -8191)	/* a2p = function of fa1 */
				a2p -= 0x100;
			else if (fa1 > 8191)
				a2p += 0xFF;
			else
				a2p += fa1 >> 5;

			if (pk0 ^ state_ptr->pk[1])
			{	/* LIMC */
				if (a2p <= -12160)
					a2p = -12288;
				else if (a2p >= 12416)
					a2p = 12288;
				else
					a2p -= 0x80;
				}
			else if (a2p <= -12416)
				a2p = -12288;
			else if (a2p >= 12160)
				a2p = 12288;
			else
				a2p += 0x80;
		}

		/* TRIGB & DELAY */
		state_ptr->a[1] = a2p;

		/* UPA1 */
		/* update predictor pole a[0] */
		state_ptr->a[0] -= state_ptr->a[0] >> 8;
		if (dqsez != 0)
		{	if (pks1 == 0)
				state_ptr->a[0] += 192;
			else
				state_ptr->a[0] -= 192;
			} ;

		/* LIMD */
		a1ul = 15360 - a2p;
		if (state_ptr->a[0] < -a1ul)
			state_ptr->a[0] = -a1ul;
		else if (state_ptr->a[0] > a1ul)
			state_ptr->a[0] = a1ul;

		/* UPB : update predictor zeros b[6] */
		for (cnt = 0; cnt < 6; cnt++) {
			if (code_size == 5)		/* for 40Kbps G.723 */
				state_ptr->b[cnt] -= state_ptr->b[cnt] >> 9;
			else			/* for G.721 and 24Kbps G.723 */
				state_ptr->b[cnt] -= state_ptr->b[cnt] >> 8;
			if (dq & 0x7FFF) {			/* XOR */
				if ((dq ^ state_ptr->dq[cnt]) >= 0)
					state_ptr->b[cnt] += 128;
				else
					state_ptr->b[cnt] -= 128;
			}
		}
	}

	for (cnt = 5; cnt > 0; cnt--)
		state_ptr->dq[cnt] = state_ptr->dq[cnt-1];
	/* FLOAT A : convert dq[0] to 4-bit exp, 6-bit mantissa f.p. */
	if (mag == 0) {
		state_ptr->dq[0] = (dq >= 0) ? 0x20 : 0xFC20;
	} else {
		expon = quan(mag, power2, 15);
		state_ptr->dq[0] = (dq >= 0) ?
		    (expon << 6) + ((mag << 6) >> expon) :
		    (expon << 6) + ((mag << 6) >> expon) - 0x400;
	}

	state_ptr->sr[1] = state_ptr->sr[0];
	/* FLOAT B : convert sr to 4-bit exp., 6-bit mantissa f.p. */
	if (sr == 0) {
		state_ptr->sr[0] = 0x20;
	} else if (sr > 0) {
		expon = quan(sr, power2, 15);
		state_ptr->sr[0] = (expon << 6) + ((sr << 6) >> expon);
	} else if (sr > -32768) {
		mag = -sr;
		expon = quan(mag, power2, 15);
		state_ptr->sr[0] =  (expon << 6) + ((mag << 6) >> expon) - 0x400;
	} else
		state_ptr->sr[0] = (short) 0xFC20;

	/* DELAY A */
	state_ptr->pk[1] = state_ptr->pk[0];
	state_ptr->pk[0] = pk0;

	/* TONE */
	if (tr == 1)		/* this sample has been treated as data */
		state_ptr->td = 0;	/* next one will be treated as voice */
	else if (a2p < -11776)	/* small sample-to-sample correlation */
		state_ptr->td = 1;	/* signal may be data */
	else				/* signal is voice */
		state_ptr->td = 0;

	/*
	 * Adaptation speed control.
	 */
	state_ptr->dms += (fi - state_ptr->dms) >> 5;		/* FILTA */
	state_ptr->dml += (((fi << 2) - state_ptr->dml) >> 7);	/* FILTB */

	if (tr == 1)
		state_ptr->ap = 256;
	else if (y < 1536)					/* SUBTC */
		state_ptr->ap += (0x200 - state_ptr->ap) >> 4;
	else if (state_ptr->td == 1)
		state_ptr->ap += (0x200 - state_ptr->ap) >> 4;
	else if (abs((state_ptr->dms << 2) - state_ptr->dml) >=
	    (state_ptr->dml >> 3))
		state_ptr->ap += (0x200 - state_ptr->ap) >> 4;
	else
		state_ptr->ap += (-state_ptr->ap) >> 4;
		
	return ;
} /* update */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 6298dc75-fd0f-4062-9b90-f73ed69f22d4
*/

/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define	GSM610_BLOCKSIZE		33
#define	GSM610_SAMPLES			160

typedef struct gsm610_tag
{	int				blocks ;
	int				blockcount, samplecount ;
	int				samplesperblock, blocksize ;

	int				(*decode_block)	(SF_PRIVATE *psf, struct gsm610_tag *pgsm610) ;
	int				(*encode_block)	(SF_PRIVATE *psf, struct gsm610_tag *pgsm610) ;

	short			samples [WAV_W64_GSM610_SAMPLES] ;
	unsigned char	block [WAV_W64_GSM610_BLOCKSIZE] ;

	gsm				gsm_data ;
} GSM610_PRIVATE ;

static sf_count_t	gsm610_read_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	gsm610_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	gsm610_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	gsm610_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	gsm610_write_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	gsm610_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	gsm610_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	gsm610_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static	int gsm610_read_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610, short *ptr, int len) ;
static	int gsm610_write_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610, short *ptr, int len) ;

static	int	gsm610_decode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610) ;
static	int	gsm610_encode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610) ;

static	int	gsm610_wav_decode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610) ;
static	int	gsm610_wav_encode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610) ;

static sf_count_t	gsm610_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset) ;

static int	gsm610_close	(SF_PRIVATE *psf) ;

/*============================================================================================
** WAV GSM610 initialisation function.
*/

int
gsm610_init	(SF_PRIVATE *psf)
{	GSM610_PRIVATE	*pgsm610 ;
	int		true_flag = 1 ;

	if (psf->mode == SFM_RDWR)
		return SFE_BAD_MODE_RW ;

	psf->sf.seekable = SF_FALSE ;

	if (! (pgsm610 = malloc (sizeof (GSM610_PRIVATE))))
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pgsm610 ;

	memset (pgsm610, 0, sizeof (GSM610_PRIVATE)) ;

/*============================================================

Need separate gsm_data structs for encode and decode.

============================================================*/

	if (! (pgsm610->gsm_data = gsm_create ()))
		return SFE_MALLOC_FAILED ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV ||
				(psf->sf.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_W64)
	{	gsm_option (pgsm610->gsm_data, GSM_OPT_WAV49, &true_flag) ;

		pgsm610->encode_block = gsm610_wav_encode_block ;
		pgsm610->decode_block = gsm610_wav_decode_block ;

		pgsm610->samplesperblock = WAV_W64_GSM610_SAMPLES ;
		pgsm610->blocksize = WAV_W64_GSM610_BLOCKSIZE ;
		}
	else
	{	pgsm610->encode_block = gsm610_encode_block ;
		pgsm610->decode_block = gsm610_decode_block ;

		pgsm610->samplesperblock = GSM610_SAMPLES ;
		pgsm610->blocksize = GSM610_BLOCKSIZE ;
		} ;

	if (psf->mode == SFM_READ)
	{	if (psf->datalength % pgsm610->blocksize)
		{	psf_log_printf (psf, "*** Warning : data chunk seems to be truncated.\n") ;
			pgsm610->blocks = psf->datalength / pgsm610->blocksize + 1 ;
			}
		else
			pgsm610->blocks = psf->datalength / pgsm610->blocksize ;

		psf->sf.frames = pgsm610->samplesperblock * pgsm610->blocks ;

		pgsm610->decode_block (psf, pgsm610) ;	/* Read first block. */

		psf->read_short		= gsm610_read_s ;
		psf->read_int		= gsm610_read_i ;
		psf->read_float		= gsm610_read_f ;
		psf->read_double	= gsm610_read_d ;
		} ;

	if (psf->mode == SFM_WRITE)
	{	pgsm610->blockcount = 0 ;
		pgsm610->samplecount = 0 ;

		psf->write_short	= gsm610_write_s ;
		psf->write_int		= gsm610_write_i ;
		psf->write_float	= gsm610_write_f ;
		psf->write_double	= gsm610_write_d ;
		} ;

	psf->close = gsm610_close ;
	psf->seek = gsm610_seek ;

	psf->filelength = psf_get_filelen (psf) ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	return 0 ;
} /* gsm610_init */

/*============================================================================================
** GSM 6.10 Read Functions.
*/

static int
gsm610_wav_decode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610)
{	int	k ;

	pgsm610->blockcount ++ ;
	pgsm610->samplecount = 0 ;

	if (pgsm610->blockcount > pgsm610->blocks)
	{	memset (pgsm610->samples, 0, WAV_W64_GSM610_SAMPLES * sizeof (short)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (pgsm610->block, 1, WAV_W64_GSM610_BLOCKSIZE, psf)) != WAV_W64_GSM610_BLOCKSIZE)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, WAV_W64_GSM610_BLOCKSIZE) ;

	if (gsm_decode (pgsm610->gsm_data, pgsm610->block, pgsm610->samples) < 0)
	{	psf_log_printf (psf, "Error from gsm_decode() on frame : %d\n", pgsm610->blockcount) ;
		return 0 ;
		} ;

	if (gsm_decode (pgsm610->gsm_data, pgsm610->block + (WAV_W64_GSM610_BLOCKSIZE + 1) / 2, pgsm610->samples + WAV_W64_GSM610_SAMPLES / 2) < 0)
	{	psf_log_printf (psf, "Error from gsm_decode() on frame : %d.5\n", pgsm610->blockcount) ;
		return 0 ;
		} ;

	return 1 ;
} /* gsm610_wav_decode_block */

static int
gsm610_decode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610)
{	int	k ;

	pgsm610->blockcount ++ ;
	pgsm610->samplecount = 0 ;

	if (pgsm610->blockcount > pgsm610->blocks)
	{	memset (pgsm610->samples, 0, GSM610_SAMPLES * sizeof (short)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (pgsm610->block, 1, GSM610_BLOCKSIZE, psf)) != GSM610_BLOCKSIZE)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, GSM610_BLOCKSIZE) ;

	if (gsm_decode (pgsm610->gsm_data, pgsm610->block, pgsm610->samples) < 0)
	{	psf_log_printf (psf, "Error from gsm_decode() on frame : %d\n", pgsm610->blockcount) ;
		return 0 ;
		} ;

	return 1 ;
} /* gsm610_decode_block */

static int
gsm610_read_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610, short *ptr, int len)
{	int	count, total = 0, indx = 0 ;

	while (indx < len)
	{	if (pgsm610->blockcount >= pgsm610->blocks && pgsm610->samplecount >= pgsm610->samplesperblock)
		{	memset (&(ptr [indx]), 0, (size_t) ((len - indx) * sizeof (short))) ;
			return total ;
			} ;

		if (pgsm610->samplecount >= pgsm610->samplesperblock)
			pgsm610->decode_block (psf, pgsm610) ;

		count = pgsm610->samplesperblock - pgsm610->samplecount ;
		count = (len - indx > count) ? count : len - indx ;

		memcpy (&(ptr [indx]), &(pgsm610->samples [pgsm610->samplecount]), count * sizeof (short)) ;
		indx += count ;
		pgsm610->samplecount += count ;
		total = indx ;
		} ;

	return total ;
} /* gsm610_read_block */

static sf_count_t
gsm610_read_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	GSM610_PRIVATE 	*pgsm610 ;
	int			readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	readcount = (len > 0x10000000) ? 0x1000000 : (int) len ;

		count = gsm610_read_block (psf, pgsm610, ptr, readcount) ;

		total += count ;
		len -= count ;

		if (count != readcount)
			break ;
		} ;

	return total ;
} /* gsm610_read_s */

static sf_count_t
gsm610_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	GSM610_PRIVATE *pgsm610 ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = gsm610_read_block (psf, pgsm610, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = sptr [k] << 16 ;

		total += count ;
		len -= readcount ;
		} ;
	return total ;
} /* gsm610_read_i */

static sf_count_t
gsm610_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	GSM610_PRIVATE *pgsm610 ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = gsm610_read_block (psf, pgsm610, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * sptr [k] ;

		total += count ;
		len -= readcount ;
		} ;
	return total ;
} /* gsm610_read_f */

static sf_count_t
gsm610_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	GSM610_PRIVATE *pgsm610 ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = gsm610_read_block (psf, pgsm610, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * sptr [k] ;

		total += count ;
		len -= readcount ;
		} ;
	return total ;
} /* gsm610_read_d */

static sf_count_t
gsm610_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset)
{	GSM610_PRIVATE *pgsm610 ;
	int			newblock, newsample ;

	mode = mode ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	if (psf->dataoffset < 0)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	if (offset == 0)
	{	int true_flag = 1 ;

		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		pgsm610->blockcount = 0 ;

		gsm_init (pgsm610->gsm_data) ;
		if ((psf->sf.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV ||
				(psf->sf.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_W64)
			gsm_option (pgsm610->gsm_data, GSM_OPT_WAV49, &true_flag) ;

		pgsm610->decode_block (psf, pgsm610) ;
		pgsm610->samplecount = 0 ;
		return 0 ;
		} ;

	if (offset < 0 || offset > pgsm610->blocks * pgsm610->samplesperblock)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	newblock	= offset / pgsm610->samplesperblock ;
	newsample	= offset % pgsm610->samplesperblock ;

	if (psf->mode == SFM_READ)
	{	if (psf->read_current != newblock * pgsm610->samplesperblock + newsample)
		{	psf_fseek (psf, psf->dataoffset + newblock * pgsm610->samplesperblock, SEEK_SET) ;
			pgsm610->blockcount = newblock ;
			pgsm610->decode_block (psf, pgsm610) ;
			pgsm610->samplecount = newsample ;
			} ;

		return newblock * pgsm610->samplesperblock + newsample ;
		} ;

	/* What to do about write??? */
	psf->error = SFE_BAD_SEEK ;
	return	((sf_count_t) -1) ;
} /* gsm610_seek */

/*==========================================================================================
** GSM 6.10 Write Functions.
*/

static int
gsm610_encode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610)
{	int k ;

	/* Encode the samples. */
	gsm_encode (pgsm610->gsm_data, pgsm610->samples, pgsm610->block) ;

	/* Write the block to disk. */
	if ((k = psf_fwrite (pgsm610->block, 1, GSM610_BLOCKSIZE, psf)) != GSM610_BLOCKSIZE)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, GSM610_BLOCKSIZE) ;

	pgsm610->samplecount = 0 ;
	pgsm610->blockcount ++ ;

	/* Set samples to zero for next block. */
	memset (pgsm610->samples, 0, WAV_W64_GSM610_SAMPLES * sizeof (short)) ;

	return 1 ;
} /* gsm610_encode_block */

static int
gsm610_wav_encode_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610)
{	int k ;

	/* Encode the samples. */
	gsm_encode (pgsm610->gsm_data, pgsm610->samples, pgsm610->block) ;
	gsm_encode (pgsm610->gsm_data, pgsm610->samples+WAV_W64_GSM610_SAMPLES/2, pgsm610->block+WAV_W64_GSM610_BLOCKSIZE/2) ;

	/* Write the block to disk. */
	if ((k = psf_fwrite (pgsm610->block, 1, WAV_W64_GSM610_BLOCKSIZE, psf)) != WAV_W64_GSM610_BLOCKSIZE)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, WAV_W64_GSM610_BLOCKSIZE) ;

	pgsm610->samplecount = 0 ;
	pgsm610->blockcount ++ ;

	/* Set samples to zero for next block. */
	memset (pgsm610->samples, 0, WAV_W64_GSM610_SAMPLES * sizeof (short)) ;

	return 1 ;
} /* gsm610_wav_encode_block */

static int
gsm610_write_block	(SF_PRIVATE *psf, GSM610_PRIVATE *pgsm610, short *ptr, int len)
{	int		count, total = 0, indx = 0 ;

	while (indx < len)
	{	count = pgsm610->samplesperblock - pgsm610->samplecount ;

		if (count > len - indx)
			count = len - indx ;

		memcpy (&(pgsm610->samples [pgsm610->samplecount]), &(ptr [indx]), count * sizeof (short)) ;
		indx += count ;
		pgsm610->samplecount += count ;
		total = indx ;

		if (pgsm610->samplecount >= pgsm610->samplesperblock)
			pgsm610->encode_block (psf, pgsm610) ;
		} ;

	return total ;
} /* gsm610_write_block */

static sf_count_t
gsm610_write_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	GSM610_PRIVATE 	*pgsm610 ;
	int			writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = gsm610_write_block (psf, pgsm610, ptr, writecount) ;

		total += count ;
		len -= count ;

		if (count != writecount)
			break ;
		} ;

	return total ;
} /* gsm610_write_s */

static sf_count_t
gsm610_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	GSM610_PRIVATE *pgsm610 ;
	short		*sptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = ptr [total + k] >> 16 ;
		count = gsm610_write_block (psf, pgsm610, sptr, writecount) ;

		total += count ;
		len -= writecount ;
		} ;
	return total ;
} /* gsm610_write_i */

static sf_count_t
gsm610_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	GSM610_PRIVATE *pgsm610 ;
	short		*sptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = gsm610_write_block (psf, pgsm610, sptr, writecount) ;

		total += count ;
		len -= writecount ;
		} ;
	return total ;
} /* gsm610_write_f */

static sf_count_t
gsm610_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	GSM610_PRIVATE *pgsm610 ;
	short		*sptr ;
	int			k, bufferlen, writecount = 0, count ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if (! psf->fdata)
		return 0 ;
	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrint (normfact * ptr [total + k]) ;
		count = gsm610_write_block (psf, pgsm610, sptr, writecount) ;

		total += count ;
		len -= writecount ;
		} ;
	return total ;
} /* gsm610_write_d */

static int
gsm610_close	(SF_PRIVATE *psf)
{	GSM610_PRIVATE *pgsm610 ;

	if (! psf->fdata)
		return 0 ;

	pgsm610 = (GSM610_PRIVATE*) psf->fdata ;

	if (psf->mode == SFM_WRITE)
	{	/*	If a block has been partially assembled, write it out
		**	as the final block.
		*/

		if (pgsm610->samplecount && pgsm610->samplecount < pgsm610->samplesperblock)
			pgsm610->encode_block (psf, pgsm610) ;

		if (psf->write_header)
			psf->write_header (psf, SF_TRUE) ;
		} ;

	if (pgsm610->gsm_data)
		gsm_destroy (pgsm610->gsm_data) ;

	return 0 ;
} /* gsm610_close */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 8575187d-af4f-4acf-b9dd-6ff705628345
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>




gsm gsm_create (void)
{
	gsm  r;

	r = malloc (sizeof(struct gsm_state));
	if (!r) return r;
	
	memset((char *)r, 0, sizeof (struct gsm_state));
	r->nrp = 40;

	return r;
}

/* Added for libsndfile : May 6, 2002. Not sure if it works. */
void gsm_init (gsm state)
{
	memset (state, 0, sizeof (struct gsm_state)) ;
	state->nrp = 40 ;
} 
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 9fedb6b3-ed99-40c2-aac1-484c536261fe
*/

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */



int gsm_decode (gsm s, gsm_byte * c, gsm_signal * target)
{
	word  	LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];

#ifdef WAV49
	if (s->wav_fmt) {

		uword sr = 0;

		s->frame_index = !s->frame_index;
		if (s->frame_index) {

			sr = *c++;
			LARc[0] = sr & 0x3f;  sr >>= 6;
			sr |= (uword)*c++ << 2;
			LARc[1] = sr & 0x3f;  sr >>= 6;
			sr |= (uword)*c++ << 4;
			LARc[2] = sr & 0x1f;  sr >>= 5;
			LARc[3] = sr & 0x1f;  sr >>= 5;
			sr |= (uword)*c++ << 2;
			LARc[4] = sr & 0xf;  sr >>= 4;
			LARc[5] = sr & 0xf;  sr >>= 4;
			sr |= (uword)*c++ << 2;			/* 5 */
			LARc[6] = sr & 0x7;  sr >>= 3;
			LARc[7] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 4;
			Nc[0] = sr & 0x7f;  sr >>= 7;
			bc[0] = sr & 0x3;  sr >>= 2;
			Mc[0] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 1;
			xmaxc[0] = sr & 0x3f;  sr >>= 6;
			xmc[0] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[1] = sr & 0x7;  sr >>= 3;
			xmc[2] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[3] = sr & 0x7;  sr >>= 3;
			xmc[4] = sr & 0x7;  sr >>= 3;
			xmc[5] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;			/* 10 */
			xmc[6] = sr & 0x7;  sr >>= 3;
			xmc[7] = sr & 0x7;  sr >>= 3;
			xmc[8] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[9] = sr & 0x7;  sr >>= 3;
			xmc[10] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[11] = sr & 0x7;  sr >>= 3;
			xmc[12] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 4;
			Nc[1] = sr & 0x7f;  sr >>= 7;
			bc[1] = sr & 0x3;  sr >>= 2;
			Mc[1] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 1;
			xmaxc[1] = sr & 0x3f;  sr >>= 6;
			xmc[13] = sr & 0x7;  sr >>= 3;
			sr = *c++;				/* 15 */
			xmc[14] = sr & 0x7;  sr >>= 3;
			xmc[15] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[16] = sr & 0x7;  sr >>= 3;
			xmc[17] = sr & 0x7;  sr >>= 3;
			xmc[18] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[19] = sr & 0x7;  sr >>= 3;
			xmc[20] = sr & 0x7;  sr >>= 3;
			xmc[21] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[22] = sr & 0x7;  sr >>= 3;
			xmc[23] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[24] = sr & 0x7;  sr >>= 3;
			xmc[25] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 4;			/* 20 */
			Nc[2] = sr & 0x7f;  sr >>= 7;
			bc[2] = sr & 0x3;  sr >>= 2;
			Mc[2] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 1;
			xmaxc[2] = sr & 0x3f;  sr >>= 6;
			xmc[26] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[27] = sr & 0x7;  sr >>= 3;
			xmc[28] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[29] = sr & 0x7;  sr >>= 3;
			xmc[30] = sr & 0x7;  sr >>= 3;
			xmc[31] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[32] = sr & 0x7;  sr >>= 3;
			xmc[33] = sr & 0x7;  sr >>= 3;
			xmc[34] = sr & 0x7;  sr >>= 3;
			sr = *c++;				/* 25 */
			xmc[35] = sr & 0x7;  sr >>= 3;
			xmc[36] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[37] = sr & 0x7;  sr >>= 3;
			xmc[38] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 4;
			Nc[3] = sr & 0x7f;  sr >>= 7;
			bc[3] = sr & 0x3;  sr >>= 2;
			Mc[3] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 1;
			xmaxc[3] = sr & 0x3f;  sr >>= 6;
			xmc[39] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[40] = sr & 0x7;  sr >>= 3;
			xmc[41] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;			/* 30 */
			xmc[42] = sr & 0x7;  sr >>= 3;
			xmc[43] = sr & 0x7;  sr >>= 3;
			xmc[44] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[45] = sr & 0x7;  sr >>= 3;
			xmc[46] = sr & 0x7;  sr >>= 3;
			xmc[47] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[48] = sr & 0x7;  sr >>= 3;
			xmc[49] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[50] = sr & 0x7;  sr >>= 3;
			xmc[51] = sr & 0x7;  sr >>= 3;

			s->frame_chain = sr & 0xf;
		}
		else {
			sr = s->frame_chain;
			sr |= (uword)*c++ << 4;			/* 1 */
			LARc[0] = sr & 0x3f;  sr >>= 6;
			LARc[1] = sr & 0x3f;  sr >>= 6;
			sr = *c++;
			LARc[2] = sr & 0x1f;  sr >>= 5;
			sr |= (uword)*c++ << 3;
			LARc[3] = sr & 0x1f;  sr >>= 5;
			LARc[4] = sr & 0xf;  sr >>= 4;
			sr |= (uword)*c++ << 2;
			LARc[5] = sr & 0xf;  sr >>= 4;
			LARc[6] = sr & 0x7;  sr >>= 3;
			LARc[7] = sr & 0x7;  sr >>= 3;
			sr = *c++;				/* 5 */
			Nc[0] = sr & 0x7f;  sr >>= 7;
			sr |= (uword)*c++ << 1;
			bc[0] = sr & 0x3;  sr >>= 2;
			Mc[0] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 5;
			xmaxc[0] = sr & 0x3f;  sr >>= 6;
			xmc[0] = sr & 0x7;  sr >>= 3;
			xmc[1] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[2] = sr & 0x7;  sr >>= 3;
			xmc[3] = sr & 0x7;  sr >>= 3;
			xmc[4] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[5] = sr & 0x7;  sr >>= 3;
			xmc[6] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;			/* 10 */
			xmc[7] = sr & 0x7;  sr >>= 3;
			xmc[8] = sr & 0x7;  sr >>= 3;
			xmc[9] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[10] = sr & 0x7;  sr >>= 3;
			xmc[11] = sr & 0x7;  sr >>= 3;
			xmc[12] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			Nc[1] = sr & 0x7f;  sr >>= 7;
			sr |= (uword)*c++ << 1;
			bc[1] = sr & 0x3;  sr >>= 2;
			Mc[1] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 5;
			xmaxc[1] = sr & 0x3f;  sr >>= 6;
			xmc[13] = sr & 0x7;  sr >>= 3;
			xmc[14] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;			/* 15 */
			xmc[15] = sr & 0x7;  sr >>= 3;
			xmc[16] = sr & 0x7;  sr >>= 3;
			xmc[17] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[18] = sr & 0x7;  sr >>= 3;
			xmc[19] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[20] = sr & 0x7;  sr >>= 3;
			xmc[21] = sr & 0x7;  sr >>= 3;
			xmc[22] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[23] = sr & 0x7;  sr >>= 3;
			xmc[24] = sr & 0x7;  sr >>= 3;
			xmc[25] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			Nc[2] = sr & 0x7f;  sr >>= 7;
			sr |= (uword)*c++ << 1;			/* 20 */
			bc[2] = sr & 0x3;  sr >>= 2;
			Mc[2] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 5;
			xmaxc[2] = sr & 0x3f;  sr >>= 6;
			xmc[26] = sr & 0x7;  sr >>= 3;
			xmc[27] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;	
			xmc[28] = sr & 0x7;  sr >>= 3;
			xmc[29] = sr & 0x7;  sr >>= 3;
			xmc[30] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			xmc[31] = sr & 0x7;  sr >>= 3;
			xmc[32] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[33] = sr & 0x7;  sr >>= 3;
			xmc[34] = sr & 0x7;  sr >>= 3;
			xmc[35] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;			/* 25 */
			xmc[36] = sr & 0x7;  sr >>= 3;
			xmc[37] = sr & 0x7;  sr >>= 3;
			xmc[38] = sr & 0x7;  sr >>= 3;
			sr = *c++;
			Nc[3] = sr & 0x7f;  sr >>= 7;
			sr |= (uword)*c++ << 1;		
			bc[3] = sr & 0x3;  sr >>= 2;
			Mc[3] = sr & 0x3;  sr >>= 2;
			sr |= (uword)*c++ << 5;
			xmaxc[3] = sr & 0x3f;  sr >>= 6;
			xmc[39] = sr & 0x7;  sr >>= 3;
			xmc[40] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[41] = sr & 0x7;  sr >>= 3;
			xmc[42] = sr & 0x7;  sr >>= 3;
			xmc[43] = sr & 0x7;  sr >>= 3;
			sr = *c++;				/* 30 */
			xmc[44] = sr & 0x7;  sr >>= 3;
			xmc[45] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 2;
			xmc[46] = sr & 0x7;  sr >>= 3;
			xmc[47] = sr & 0x7;  sr >>= 3;
			xmc[48] = sr & 0x7;  sr >>= 3;
			sr |= (uword)*c++ << 1;
			xmc[49] = sr & 0x7;  sr >>= 3;
			xmc[50] = sr & 0x7;  sr >>= 3;
			xmc[51] = sr & 0x7;  sr >>= 3;
		}
	}
	else
#endif
	{
		/* GSM_MAGIC  = (*c >> 4) & 0xF; */

		if (((*c >> 4) & 0x0F) != GSM_MAGIC) return -1;

		LARc[0]  = (*c++ & 0xF) << 2;		/* 1 */
		LARc[0] |= (*c >> 6) & 0x3;
		LARc[1]  = *c++ & 0x3F;
		LARc[2]  = (*c >> 3) & 0x1F;
		LARc[3]  = (*c++ & 0x7) << 2;
		LARc[3] |= (*c >> 6) & 0x3;
		LARc[4]  = (*c >> 2) & 0xF;
		LARc[5]  = (*c++ & 0x3) << 2;
		LARc[5] |= (*c >> 6) & 0x3;
		LARc[6]  = (*c >> 3) & 0x7;
		LARc[7]  = *c++ & 0x7;
		Nc[0]  = (*c >> 1) & 0x7F;
		bc[0]  = (*c++ & 0x1) << 1;
		bc[0] |= (*c >> 7) & 0x1;
		Mc[0]  = (*c >> 5) & 0x3;
		xmaxc[0]  = (*c++ & 0x1F) << 1;
		xmaxc[0] |= (*c >> 7) & 0x1;
		xmc[0]  = (*c >> 4) & 0x7;
		xmc[1]  = (*c >> 1) & 0x7;
		xmc[2]  = (*c++ & 0x1) << 2;
		xmc[2] |= (*c >> 6) & 0x3;
		xmc[3]  = (*c >> 3) & 0x7;
		xmc[4]  = *c++ & 0x7;
		xmc[5]  = (*c >> 5) & 0x7;
		xmc[6]  = (*c >> 2) & 0x7;
		xmc[7]  = (*c++ & 0x3) << 1;		/* 10 */
		xmc[7] |= (*c >> 7) & 0x1;
		xmc[8]  = (*c >> 4) & 0x7;
		xmc[9]  = (*c >> 1) & 0x7;
		xmc[10]  = (*c++ & 0x1) << 2;
		xmc[10] |= (*c >> 6) & 0x3;
		xmc[11]  = (*c >> 3) & 0x7;
		xmc[12]  = *c++ & 0x7;
		Nc[1]  = (*c >> 1) & 0x7F;
		bc[1]  = (*c++ & 0x1) << 1;
		bc[1] |= (*c >> 7) & 0x1;
		Mc[1]  = (*c >> 5) & 0x3;
		xmaxc[1]  = (*c++ & 0x1F) << 1;
		xmaxc[1] |= (*c >> 7) & 0x1;
		xmc[13]  = (*c >> 4) & 0x7;
		xmc[14]  = (*c >> 1) & 0x7;
		xmc[15]  = (*c++ & 0x1) << 2;
		xmc[15] |= (*c >> 6) & 0x3;
		xmc[16]  = (*c >> 3) & 0x7;
		xmc[17]  = *c++ & 0x7;
		xmc[18]  = (*c >> 5) & 0x7;
		xmc[19]  = (*c >> 2) & 0x7;
		xmc[20]  = (*c++ & 0x3) << 1;
		xmc[20] |= (*c >> 7) & 0x1;
		xmc[21]  = (*c >> 4) & 0x7;
		xmc[22]  = (*c >> 1) & 0x7;
		xmc[23]  = (*c++ & 0x1) << 2;
		xmc[23] |= (*c >> 6) & 0x3;
		xmc[24]  = (*c >> 3) & 0x7;
		xmc[25]  = *c++ & 0x7;
		Nc[2]  = (*c >> 1) & 0x7F;
		bc[2]  = (*c++ & 0x1) << 1;		/* 20 */
		bc[2] |= (*c >> 7) & 0x1;
		Mc[2]  = (*c >> 5) & 0x3;
		xmaxc[2]  = (*c++ & 0x1F) << 1;
		xmaxc[2] |= (*c >> 7) & 0x1;
		xmc[26]  = (*c >> 4) & 0x7;
		xmc[27]  = (*c >> 1) & 0x7;
		xmc[28]  = (*c++ & 0x1) << 2;
		xmc[28] |= (*c >> 6) & 0x3;
		xmc[29]  = (*c >> 3) & 0x7;
		xmc[30]  = *c++ & 0x7;
		xmc[31]  = (*c >> 5) & 0x7;
		xmc[32]  = (*c >> 2) & 0x7;
		xmc[33]  = (*c++ & 0x3) << 1;
		xmc[33] |= (*c >> 7) & 0x1;
		xmc[34]  = (*c >> 4) & 0x7;
		xmc[35]  = (*c >> 1) & 0x7;
		xmc[36]  = (*c++ & 0x1) << 2;
		xmc[36] |= (*c >> 6) & 0x3;
		xmc[37]  = (*c >> 3) & 0x7;
		xmc[38]  = *c++ & 0x7;
		Nc[3]  = (*c >> 1) & 0x7F;
		bc[3]  = (*c++ & 0x1) << 1;
		bc[3] |= (*c >> 7) & 0x1;
		Mc[3]  = (*c >> 5) & 0x3;
		xmaxc[3]  = (*c++ & 0x1F) << 1;
		xmaxc[3] |= (*c >> 7) & 0x1;
		xmc[39]  = (*c >> 4) & 0x7;
		xmc[40]  = (*c >> 1) & 0x7;
		xmc[41]  = (*c++ & 0x1) << 2;
		xmc[41] |= (*c >> 6) & 0x3;
		xmc[42]  = (*c >> 3) & 0x7;
		xmc[43]  = *c++ & 0x7;			/* 30  */
		xmc[44]  = (*c >> 5) & 0x7;
		xmc[45]  = (*c >> 2) & 0x7;
		xmc[46]  = (*c++ & 0x3) << 1;
		xmc[46] |= (*c >> 7) & 0x1;
		xmc[47]  = (*c >> 4) & 0x7;
		xmc[48]  = (*c >> 1) & 0x7;
		xmc[49]  = (*c++ & 0x1) << 2;
		xmc[49] |= (*c >> 6) & 0x3;
		xmc[50]  = (*c >> 3) & 0x7;
		xmc[51]  = *c & 0x7;			/* 33 */
	}

	Gsm_Decoder(s, LARc, Nc, bc, Mc, xmaxc, xmc, target);

	return 0;
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 6a9b6628-821c-4a96-84c1-485ebd35f170
*/

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */


#ifdef	HAS_STDLIB_H
#	include	<stdlib.h>
#else
#	ifdef	HAS_MALLOC_H
#		include 	<malloc.h>
#	else
		extern void free();
#	endif
#endif

void gsm_destroy (gsm S)
{
	if (S) free((char *)S);
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: f423d09b-6ccc-47e0-9b18-ee1cf7a8e473
*/

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */


void gsm_encode (gsm s, gsm_signal * source, gsm_byte * c)
{
	word	 	LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];

	Gsm_Coder(s, source, LARc, Nc, bc, Mc, xmaxc, xmc);


	/*	variable	size

		GSM_MAGIC	4

		LARc[0]		6
		LARc[1]		6
		LARc[2]		5
		LARc[3]		5
		LARc[4]		4
		LARc[5]		4
		LARc[6]		3
		LARc[7]		3

		Nc[0]		7
		bc[0]		2
		Mc[0]		2
		xmaxc[0]	6
		xmc[0]		3
		xmc[1]		3
		xmc[2]		3
		xmc[3]		3
		xmc[4]		3
		xmc[5]		3
		xmc[6]		3
		xmc[7]		3
		xmc[8]		3
		xmc[9]		3
		xmc[10]		3
		xmc[11]		3
		xmc[12]		3

		Nc[1]		7
		bc[1]		2
		Mc[1]		2
		xmaxc[1]	6
		xmc[13]		3
		xmc[14]		3
		xmc[15]		3
		xmc[16]		3
		xmc[17]		3
		xmc[18]		3
		xmc[19]		3
		xmc[20]		3
		xmc[21]		3
		xmc[22]		3
		xmc[23]		3
		xmc[24]		3
		xmc[25]		3

		Nc[2]		7
		bc[2]		2
		Mc[2]		2
		xmaxc[2]	6
		xmc[26]		3
		xmc[27]		3
		xmc[28]		3
		xmc[29]		3
		xmc[30]		3
		xmc[31]		3
		xmc[32]		3
		xmc[33]		3
		xmc[34]		3
		xmc[35]		3
		xmc[36]		3
		xmc[37]		3
		xmc[38]		3

		Nc[3]		7
		bc[3]		2
		Mc[3]		2
		xmaxc[3]	6
		xmc[39]		3
		xmc[40]		3
		xmc[41]		3
		xmc[42]		3
		xmc[43]		3
		xmc[44]		3
		xmc[45]		3
		xmc[46]		3
		xmc[47]		3
		xmc[48]		3
		xmc[49]		3
		xmc[50]		3
		xmc[51]		3
	*/

#ifdef WAV49

	if (s->wav_fmt) {
		s->frame_index = !s->frame_index;
		if (s->frame_index) {

			uword sr;

			sr = 0;
			sr = sr >> 6 | LARc[0] << 10;
			sr = sr >> 6 | LARc[1] << 10;
			*c++ = sr >> 4;
			sr = sr >> 5 | LARc[2] << 11;
			*c++ = sr >> 7;
			sr = sr >> 5 | LARc[3] << 11;
			sr = sr >> 4 | LARc[4] << 12;
			*c++ = sr >> 6;
			sr = sr >> 4 | LARc[5] << 12;
			sr = sr >> 3 | LARc[6] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | LARc[7] << 13;
			sr = sr >> 7 | Nc[0] << 9;
			*c++ = sr >> 5;
			sr = sr >> 2 | bc[0] << 14;
			sr = sr >> 2 | Mc[0] << 14;
			sr = sr >> 6 | xmaxc[0] << 10;
			*c++ = sr >> 3;
			sr = sr >> 3 | xmc[0] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[1] << 13;
			sr = sr >> 3 | xmc[2] << 13;
			sr = sr >> 3 | xmc[3] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[4] << 13;
			sr = sr >> 3 | xmc[5] << 13;
			sr = sr >> 3 | xmc[6] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[7] << 13;
			sr = sr >> 3 | xmc[8] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[9] << 13;
			sr = sr >> 3 | xmc[10] << 13;
			sr = sr >> 3 | xmc[11] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[12] << 13;
			sr = sr >> 7 | Nc[1] << 9;
			*c++ = sr >> 5;
			sr = sr >> 2 | bc[1] << 14;
			sr = sr >> 2 | Mc[1] << 14;
			sr = sr >> 6 | xmaxc[1] << 10;
			*c++ = sr >> 3;
			sr = sr >> 3 | xmc[13] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[14] << 13;
			sr = sr >> 3 | xmc[15] << 13;
			sr = sr >> 3 | xmc[16] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[17] << 13;
			sr = sr >> 3 | xmc[18] << 13;
			sr = sr >> 3 | xmc[19] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[20] << 13;
			sr = sr >> 3 | xmc[21] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[22] << 13;
			sr = sr >> 3 | xmc[23] << 13;
			sr = sr >> 3 | xmc[24] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[25] << 13;
			sr = sr >> 7 | Nc[2] << 9;
			*c++ = sr >> 5;
			sr = sr >> 2 | bc[2] << 14;
			sr = sr >> 2 | Mc[2] << 14;
			sr = sr >> 6 | xmaxc[2] << 10;
			*c++ = sr >> 3;
			sr = sr >> 3 | xmc[26] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[27] << 13;
			sr = sr >> 3 | xmc[28] << 13;
			sr = sr >> 3 | xmc[29] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[30] << 13;
			sr = sr >> 3 | xmc[31] << 13;
			sr = sr >> 3 | xmc[32] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[33] << 13;
			sr = sr >> 3 | xmc[34] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[35] << 13;
			sr = sr >> 3 | xmc[36] << 13;
			sr = sr >> 3 | xmc[37] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[38] << 13;
			sr = sr >> 7 | Nc[3] << 9;
			*c++ = sr >> 5;
			sr = sr >> 2 | bc[3] << 14;
			sr = sr >> 2 | Mc[3] << 14;
			sr = sr >> 6 | xmaxc[3] << 10;
			*c++ = sr >> 3;
			sr = sr >> 3 | xmc[39] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[40] << 13;
			sr = sr >> 3 | xmc[41] << 13;
			sr = sr >> 3 | xmc[42] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[43] << 13;
			sr = sr >> 3 | xmc[44] << 13;
			sr = sr >> 3 | xmc[45] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[46] << 13;
			sr = sr >> 3 | xmc[47] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[48] << 13;
			sr = sr >> 3 | xmc[49] << 13;
			sr = sr >> 3 | xmc[50] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[51] << 13;
			sr = sr >> 4;
			*c = sr >> 8;
			s->frame_chain = *c;
		}
		else {
			uword sr;

			sr = 0;
			sr = sr >> 4 | s->frame_chain << 12;
			sr = sr >> 6 | LARc[0] << 10;
			*c++ = sr >> 6;
			sr = sr >> 6 | LARc[1] << 10;
			*c++ = sr >> 8;
			sr = sr >> 5 | LARc[2] << 11;
			sr = sr >> 5 | LARc[3] << 11;
			*c++ = sr >> 6;
			sr = sr >> 4 | LARc[4] << 12;
			sr = sr >> 4 | LARc[5] << 12;
			*c++ = sr >> 6;
			sr = sr >> 3 | LARc[6] << 13;
			sr = sr >> 3 | LARc[7] << 13;
			*c++ = sr >> 8;
			sr = sr >> 7 | Nc[0] << 9;
			sr = sr >> 2 | bc[0] << 14;
			*c++ = sr >> 7;
			sr = sr >> 2 | Mc[0] << 14;
			sr = sr >> 6 | xmaxc[0] << 10;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[0] << 13;
			sr = sr >> 3 | xmc[1] << 13;
			sr = sr >> 3 | xmc[2] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[3] << 13;
			sr = sr >> 3 | xmc[4] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[5] << 13;
			sr = sr >> 3 | xmc[6] << 13;
			sr = sr >> 3 | xmc[7] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[8] << 13;
			sr = sr >> 3 | xmc[9] << 13;
			sr = sr >> 3 | xmc[10] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[11] << 13;
			sr = sr >> 3 | xmc[12] << 13;
			*c++ = sr >> 8;
			sr = sr >> 7 | Nc[1] << 9;
			sr = sr >> 2 | bc[1] << 14;
			*c++ = sr >> 7;
			sr = sr >> 2 | Mc[1] << 14;
			sr = sr >> 6 | xmaxc[1] << 10;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[13] << 13;
			sr = sr >> 3 | xmc[14] << 13;
			sr = sr >> 3 | xmc[15] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[16] << 13;
			sr = sr >> 3 | xmc[17] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[18] << 13;
			sr = sr >> 3 | xmc[19] << 13;
			sr = sr >> 3 | xmc[20] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[21] << 13;
			sr = sr >> 3 | xmc[22] << 13;
			sr = sr >> 3 | xmc[23] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[24] << 13;
			sr = sr >> 3 | xmc[25] << 13;
			*c++ = sr >> 8;
			sr = sr >> 7 | Nc[2] << 9;
			sr = sr >> 2 | bc[2] << 14;
			*c++ = sr >> 7;
			sr = sr >> 2 | Mc[2] << 14;
			sr = sr >> 6 | xmaxc[2] << 10;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[26] << 13;
			sr = sr >> 3 | xmc[27] << 13;
			sr = sr >> 3 | xmc[28] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[29] << 13;
			sr = sr >> 3 | xmc[30] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[31] << 13;
			sr = sr >> 3 | xmc[32] << 13;
			sr = sr >> 3 | xmc[33] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[34] << 13;
			sr = sr >> 3 | xmc[35] << 13;
			sr = sr >> 3 | xmc[36] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[37] << 13;
			sr = sr >> 3 | xmc[38] << 13;
			*c++ = sr >> 8;
			sr = sr >> 7 | Nc[3] << 9;
			sr = sr >> 2 | bc[3] << 14;
			*c++ = sr >> 7;
			sr = sr >> 2 | Mc[3] << 14;
			sr = sr >> 6 | xmaxc[3] << 10;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[39] << 13;
			sr = sr >> 3 | xmc[40] << 13;
			sr = sr >> 3 | xmc[41] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[42] << 13;
			sr = sr >> 3 | xmc[43] << 13;
			*c++ = sr >> 8;
			sr = sr >> 3 | xmc[44] << 13;
			sr = sr >> 3 | xmc[45] << 13;
			sr = sr >> 3 | xmc[46] << 13;
			*c++ = sr >> 7;
			sr = sr >> 3 | xmc[47] << 13;
			sr = sr >> 3 | xmc[48] << 13;
			sr = sr >> 3 | xmc[49] << 13;
			*c++ = sr >> 6;
			sr = sr >> 3 | xmc[50] << 13;
			sr = sr >> 3 | xmc[51] << 13;
			*c++ = sr >> 8;
		}
	}

	else

#endif	/* WAV49 */
	{

		*c++ =   ((GSM_MAGIC & 0xF) << 4)		/* 1 */
		       | ((LARc[0] >> 2) & 0xF);
		*c++ =   ((LARc[0] & 0x3) << 6)
		       | (LARc[1] & 0x3F);
		*c++ =   ((LARc[2] & 0x1F) << 3)
		       | ((LARc[3] >> 2) & 0x7);
		*c++ =   ((LARc[3] & 0x3) << 6)
		       | ((LARc[4] & 0xF) << 2)
		       | ((LARc[5] >> 2) & 0x3);
		*c++ =   ((LARc[5] & 0x3) << 6)
		       | ((LARc[6] & 0x7) << 3)
		       | (LARc[7] & 0x7);
		*c++ =   ((Nc[0] & 0x7F) << 1)
		       | ((bc[0] >> 1) & 0x1);
		*c++ =   ((bc[0] & 0x1) << 7)
		       | ((Mc[0] & 0x3) << 5)
		       | ((xmaxc[0] >> 1) & 0x1F);
		*c++ =   ((xmaxc[0] & 0x1) << 7)
		       | ((xmc[0] & 0x7) << 4)
		       | ((xmc[1] & 0x7) << 1)
		       | ((xmc[2] >> 2) & 0x1);
		*c++ =   ((xmc[2] & 0x3) << 6)
		       | ((xmc[3] & 0x7) << 3)
		       | (xmc[4] & 0x7);
		*c++ =   ((xmc[5] & 0x7) << 5)			/* 10 */
		       | ((xmc[6] & 0x7) << 2)
		       | ((xmc[7] >> 1) & 0x3);
		*c++ =   ((xmc[7] & 0x1) << 7)
		       | ((xmc[8] & 0x7) << 4)
		       | ((xmc[9] & 0x7) << 1)
		       | ((xmc[10] >> 2) & 0x1);
		*c++ =   ((xmc[10] & 0x3) << 6)
		       | ((xmc[11] & 0x7) << 3)
		       | (xmc[12] & 0x7);
		*c++ =   ((Nc[1] & 0x7F) << 1)
		       | ((bc[1] >> 1) & 0x1);
		*c++ =   ((bc[1] & 0x1) << 7)
		       | ((Mc[1] & 0x3) << 5)
		       | ((xmaxc[1] >> 1) & 0x1F);
		*c++ =   ((xmaxc[1] & 0x1) << 7)
		       | ((xmc[13] & 0x7) << 4)
		       | ((xmc[14] & 0x7) << 1)
		       | ((xmc[15] >> 2) & 0x1);
		*c++ =   ((xmc[15] & 0x3) << 6)
		       | ((xmc[16] & 0x7) << 3)
		       | (xmc[17] & 0x7);
		*c++ =   ((xmc[18] & 0x7) << 5)
		       | ((xmc[19] & 0x7) << 2)
		       | ((xmc[20] >> 1) & 0x3);
		*c++ =   ((xmc[20] & 0x1) << 7)
		       | ((xmc[21] & 0x7) << 4)
		       | ((xmc[22] & 0x7) << 1)
		       | ((xmc[23] >> 2) & 0x1);
		*c++ =   ((xmc[23] & 0x3) << 6)
		       | ((xmc[24] & 0x7) << 3)
		       | (xmc[25] & 0x7);
		*c++ =   ((Nc[2] & 0x7F) << 1)			/* 20 */
		       | ((bc[2] >> 1) & 0x1);
		*c++ =   ((bc[2] & 0x1) << 7)
		       | ((Mc[2] & 0x3) << 5)
		       | ((xmaxc[2] >> 1) & 0x1F);
		*c++ =   ((xmaxc[2] & 0x1) << 7)
		       | ((xmc[26] & 0x7) << 4)
		       | ((xmc[27] & 0x7) << 1)
		       | ((xmc[28] >> 2) & 0x1);
		*c++ =   ((xmc[28] & 0x3) << 6)
		       | ((xmc[29] & 0x7) << 3)
		       | (xmc[30] & 0x7);
		*c++ =   ((xmc[31] & 0x7) << 5)
		       | ((xmc[32] & 0x7) << 2)
		       | ((xmc[33] >> 1) & 0x3);
		*c++ =   ((xmc[33] & 0x1) << 7)
		       | ((xmc[34] & 0x7) << 4)
		       | ((xmc[35] & 0x7) << 1)
		       | ((xmc[36] >> 2) & 0x1);
		*c++ =   ((xmc[36] & 0x3) << 6)
		       | ((xmc[37] & 0x7) << 3)
		       | (xmc[38] & 0x7);
		*c++ =   ((Nc[3] & 0x7F) << 1)
		       | ((bc[3] >> 1) & 0x1);
		*c++ =   ((bc[3] & 0x1) << 7)
		       | ((Mc[3] & 0x3) << 5)
		       | ((xmaxc[3] >> 1) & 0x1F);
		*c++ =   ((xmaxc[3] & 0x1) << 7)
		       | ((xmc[39] & 0x7) << 4)
		       | ((xmc[40] & 0x7) << 1)
		       | ((xmc[41] >> 2) & 0x1);
		*c++ =   ((xmc[41] & 0x3) << 6)			/* 30 */
		       | ((xmc[42] & 0x7) << 3)
		       | (xmc[43] & 0x7);
		*c++ =   ((xmc[44] & 0x7) << 5)
		       | ((xmc[45] & 0x7) << 2)
		       | ((xmc[46] >> 1) & 0x3);
		*c++ =   ((xmc[46] & 0x1) << 7)
		       | ((xmc[47] & 0x7) << 4)
		       | ((xmc[48] & 0x7) << 1)
		       | ((xmc[49] >> 2) & 0x1);
		*c++ =   ((xmc[49] & 0x3) << 6)
		       | ((xmc[50] & 0x7) << 3)
		       | (xmc[51] & 0x7);

	}
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: cfe9c43d-d97c-4216-b5e5-ccd6a25b582b
*/

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */



int gsm_option (gsm r, int opt, int * val)
{
	int 	result = -1;

	switch (opt) {
	case GSM_OPT_LTP_CUT:
#ifdef 	LTP_CUT
		result = r->ltp_cut;
		if (val) r->ltp_cut = *val;
#endif
		break;

	case GSM_OPT_VERBOSE:
#ifndef	NDEBUG
		result = r->verbose;
		if (val) r->verbose = *val;
#endif
		break;

	case GSM_OPT_FAST:

#if	defined(FAST) && defined(USE_FLOAT_MUL)
		result = r->fast;
		if (val) r->fast = !!*val;
#endif
		break;

	case GSM_OPT_FRAME_CHAIN:

#ifdef WAV49
		result = r->frame_chain;
		if (val) r->frame_chain = *val;
#endif
		break;

	case GSM_OPT_FRAME_INDEX:

#ifdef WAV49
		result = r->frame_index;
		if (val) r->frame_index = *val;
#endif
		break;

	case GSM_OPT_WAV49:

#ifdef WAV49 
		result = r->wav_fmt;
		if (val) r->wav_fmt = !!*val;
#endif
		break;

	default:
		break;
	}
	return result;
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 963ff156-506f-4359-9145-371e9060b030
*/

/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define	SFE_HTK_BAD_FILE_LEN 	1666
#define	SFE_HTK_NOT_WAVEFORM	1667

/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int		htk_close		(SF_PRIVATE *psf) ;

static int		htk_write_header (SF_PRIVATE *psf, int calc_length) ;
static int		htk_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
htk_open	(SF_PRIVATE *psf)
{	int		subformat ;
	int		error = 0 ;

	if (psf->is_pipe)
		return SFE_HTK_NO_PIPE ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = htk_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_HTK)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = SF_ENDIAN_BIG ;

		if (htk_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = htk_write_header ;
		} ;

	psf->close = htk_close ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case SF_FORMAT_PCM_16 :	/* 16-bit linear PCM. */
				error = pcm_init (psf) ;
				break ;

		default : break ;
		} ;

	return error ;
} /* htk_open */

/*------------------------------------------------------------------------------
*/

static int
htk_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		htk_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* htk_close */

static int
htk_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int			sample_count, sample_period ;

	current = psf_ftell (psf) ;

	if (calc_length)
		psf->filelength = psf_get_filelen (psf) ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	if (psf->filelength > 12)
		sample_count = (psf->filelength - 12) / 2 ;
	else
		sample_count = 0 ;

	sample_period = 10000000 / psf->sf.samplerate ;

	psf_binheader_writef (psf, "E444", sample_count, sample_period, 0x20000) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* htk_write_header */

/*
** Found the following info in a comment block within Bill Schottstaedt's
** sndlib library.
**
** HTK format files consist of a contiguous sequence of samples preceded by a
** header. Each sample is a vector of either 2-byte integers or 4-byte floats.
** 2-byte integers are used for compressed forms as described below and for
** vector quantised data as described later in section 5.11. HTK format data
** files can also be used to store speech waveforms as described in section 5.8.
**
** The HTK file format header is 12 bytes long and contains the following data
**   nSamples   -- number of samples in file (4-byte integer)
**   sampPeriod -- sample period in 100ns units (4-byte integer)
**   sampSize   -- number of bytes per sample (2-byte integer)
**   parmKind   -- a code indicating the sample kind (2-byte integer)
**
** The parameter kind  consists of a 6 bit code representing the basic
** parameter kind plus additional bits for each of the possible qualifiers.
** The basic parameter kind codes are
**
**  0    WAVEFORM    sampled waveform
**  1    LPC         linear prediction filter coefficients
**  2    LPREFC      linear prediction reflection coefficients
**  3    LPCEPSTRA   LPC cepstral coefficients
**  4    LPDELCEP    LPC cepstra plus delta coefficients
**  5    IREFC       LPC reflection coef in 16 bit integer format
**  6    MFCC        mel-frequency cepstral coefficients
**  7    FBANK       log mel-filter bank channel outputs
**  8    MELSPEC     linear mel-filter bank channel outputs
**  9    USER        user defined sample kind
**  10   DISCRETE    vector quantised data
**
** and the bit-encoding for the qualifiers (in octal) is
**   _E   000100      has energy
**   _N   000200      absolute energy suppressed
**   _D   000400      has delta coefficients
**   _A   001000      has acceleration coefficients
**   _C   002000      is compressed
**   _Z   004000      has zero mean static coef.
**   _K   010000      has CRC checksum
**   _O   020000      has 0'th cepstral coef.
*/

static int
htk_read_header (SF_PRIVATE *psf)
{	int		sample_count, sample_period, marker ;

	psf_binheader_readf (psf, "pE444", 0, &sample_count, &sample_period, &marker) ;

	if (2 * sample_count + 12 != psf->filelength)
		return SFE_HTK_BAD_FILE_LEN ;

	if (marker != 0x20000)
		return SFE_HTK_NOT_WAVEFORM ;

	psf->sf.channels = 1 ;
	psf->sf.samplerate = 10000000 / sample_period ;

	psf_log_printf (psf, "HTK Waveform file\n  Sample Count  : %d\n  Sample Period : %d => %d Hz\n",
				sample_count, sample_period, psf->sf.samplerate) ;

	psf->sf.format = SF_FORMAT_HTK | SF_FORMAT_PCM_16 ;
	psf->bytewidth = 2 ;

	/* HTK always has a 12 byte header. */
	psf->dataoffset = 12 ;
	psf->endian = SF_ENDIAN_BIG ;

	psf->datalength = psf->filelength - psf->dataoffset ;

	psf->close = htk_close ;

	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	if (! psf->sf.frames && psf->blockwidth)
		psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;

	return 0 ;
} /* htk_read_header */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: c350e972-082e-4c20-8934-03391a723560
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


typedef struct IMA_ADPCM_PRIVATE_tag
{	int 			(*decode_block) (SF_PRIVATE *psf, struct IMA_ADPCM_PRIVATE_tag *pima) ;
	int 			(*encode_block) (SF_PRIVATE *psf, struct IMA_ADPCM_PRIVATE_tag *pima) ;

	int				channels, blocksize, samplesperblock, blocks ;
	int				blockcount, samplecount ;
	int				previous [2] ;
	int				stepindx [2] ;
	unsigned char	*block ;
	short			*samples ;
#if HAVE_FLEXIBLE_ARRAY
	unsigned short	data	[] ; /* ISO C99 struct flexible array. */
#else
	unsigned short	data	[1] ; /* This is a hack and might not work. */
#endif
} IMA_ADPCM_PRIVATE ;

/*============================================================================================
** Predefined IMA ADPCM data.
*/

static int ima_indx_adjust [16] =
{	-1, -1, -1, -1,		/* +0 - +3, decrease the step size */
     2,  4,  6,  8,     /* +4 - +7, increase the step size */
    -1, -1, -1, -1,		/* -0 - -3, decrease the step size */
     2,  4,  6,  8,		/* -4 - -7, increase the step size */
} ;

static int ima_step_size [89] =
{	7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230,
	253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963,
	1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
	3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
	11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
	32767
} ;

static int ima_reader_init (SF_PRIVATE *psf, int blockalign, int samplesperblock) ;
static int ima_writer_init (SF_PRIVATE *psf, int blockalign) ;

static int ima_read_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima, short *ptr, int len) ;
static int ima_write_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima, short *ptr, int len) ;

static sf_count_t ima_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t ima_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t ima_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t ima_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t ima_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t ima_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t ima_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t ima_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	ima_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset) ;

static int	wav_w64_ima_close	(SF_PRIVATE *psf) ;
static int	aiff_ima_close	(SF_PRIVATE *psf) ;

static int wav_w64_ima_decode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima) ;
static int wav_w64_ima_encode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima) ;

/*-static int aiff_ima_reader_init (SF_PRIVATE *psf, int blockalign, int samplesperblock) ;-*/
static int aiff_ima_decode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima) ;
static int aiff_ima_encode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima) ;


/*============================================================================================
** IMA ADPCM Reader initialisation function.
*/

int
wav_w64_ima_init (SF_PRIVATE *psf, int blockalign, int samplesperblock)
{	int error ;

	if (psf->mode == SFM_RDWR)
		return SFE_BAD_MODE_RW ;

	if (psf->mode == SFM_READ)
		if ((error = ima_reader_init (psf, blockalign, samplesperblock)))
			return error ;

	if (psf->mode == SFM_WRITE)
		if ((error = ima_writer_init (psf, blockalign)))
			return error ;

	psf->seek = ima_seek ;
	psf->close = wav_w64_ima_close ;

	return 0 ;
} /* wav_w64_ima_init */

static int
wav_w64_ima_close	(SF_PRIVATE *psf)
{	IMA_ADPCM_PRIVATE *pima ;

	if (! psf->fdata)
		return 0 ;

	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	if (psf->mode == SFM_WRITE)
	{	/*	If a block has been partially assembled, write it out
		**	as the final block.
		*/
		if (pima->samplecount && pima->samplecount < pima->samplesperblock)
			pima->encode_block (psf, pima) ;

		psf->sf.frames = pima->samplesperblock * pima->blockcount / psf->sf.channels ;

		if (psf->write_header)
			psf->write_header (psf, SF_TRUE) ;
		} ;

	free (psf->fdata) ;
	psf->fdata = NULL ;

	return 0 ;
} /* wav_w64_ima_close */

int
aiff_ima_init (SF_PRIVATE *psf, int blockalign, int samplesperblock)
{	int error ;

	if (psf->mode == SFM_RDWR)
		return SFE_BAD_MODE_RW ;

	if (psf->mode == SFM_READ)
		if ((error = ima_reader_init (psf, blockalign, samplesperblock)))
			return error ;

	if (psf->mode == SFM_WRITE)
		if ((error = ima_writer_init (psf, blockalign)))
			return error ;

	psf->seek = ima_seek ;
	psf->close = aiff_ima_close ;

	return 0 ;
} /* aiff_ima_init */

static int
aiff_ima_close	(SF_PRIVATE *psf)
{	IMA_ADPCM_PRIVATE *pima ;

	if (! psf->fdata)
		return 0 ;

	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	if (psf->mode == SFM_WRITE)
	{	/*	If a block has been partially assembled, write it out
		**	as the final block.
		*/

		if (pima->samplecount && pima->samplecount < pima->samplesperblock)
			pima->encode_block (psf, pima) ;

		if (psf->write_header)
			psf->write_header (psf, SF_TRUE) ;
		} ;

	free (psf->fdata) ;
	psf->fdata = NULL ;

	return 0 ;
} /* aiff_ima_close */

/*============================================================================================
** IMA ADPCM Read Functions.
*/

static int
ima_reader_init (SF_PRIVATE *psf, int blockalign, int samplesperblock)
{	IMA_ADPCM_PRIVATE	*pima ;
	int		pimasize, count ;

	if (psf->mode != SFM_READ)
		return SFE_BAD_MODE_RW ;

	pimasize = sizeof (IMA_ADPCM_PRIVATE) + blockalign * psf->sf.channels + 3 * psf->sf.channels * samplesperblock ;

	if (! (pima = malloc (pimasize)))
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pima ;

	memset (pima, 0, pimasize) ;

	pima->samples	= (short *)pima->data ;
	pima->block		= (unsigned char*) (pima->data + samplesperblock * psf->sf.channels) ;

	pima->channels			= psf->sf.channels ;
	pima->blocksize			= blockalign ;
	pima->samplesperblock	= samplesperblock ;

	psf->filelength = psf_get_filelen (psf) ;
	psf->datalength = (psf->dataend) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;

	if (psf->datalength % pima->blocksize)
		pima->blocks = psf->datalength / pima->blocksize + 1 ;
	else
		pima->blocks = psf->datalength / pima->blocksize ;

	switch (psf->sf.format & SF_FORMAT_TYPEMASK)
	{	case SF_FORMAT_WAV :
		case SF_FORMAT_W64 :
				count = 2 * (pima->blocksize - 4 * pima->channels) / pima->channels + 1 ;

				if (pima->samplesperblock != count)
					psf_log_printf (psf, "*** Warning : samplesperblock should be %d.\n", count) ;

				pima->decode_block = wav_w64_ima_decode_block ;

				psf->sf.frames = pima->samplesperblock * pima->blocks ;
				break ;

		case SF_FORMAT_AIFF :
				psf_log_printf (psf, "still need to check block count\n") ;
				pima->decode_block = aiff_ima_decode_block ;
				psf->sf.frames = pima->samplesperblock * pima->blocks / pima->channels ;
				break ;

		default :
				psf_log_printf (psf, "ima_reader_init: bad psf->sf.format\n") ;
				return SFE_INTERNAL ;
				break ;
		} ;

	pima->decode_block (psf, pima) ;	/* Read first block. */

	psf->read_short		= ima_read_s ;
	psf->read_int		= ima_read_i ;
	psf->read_float		= ima_read_f ;
	psf->read_double	= ima_read_d ;

	return 0 ;
} /* ima_reader_init */

static int
aiff_ima_decode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima)
{	unsigned char *blockdata ;
	int		chan, k, diff, bytecode ;
	short	step, stepindx, predictor, *sampledata ;

static int count = 0 ;
count ++ ;

	pima->blockcount += pima->channels ;
	pima->samplecount = 0 ;

	if (pima->blockcount > pima->blocks)
	{	memset (pima->samples, 0, pima->samplesperblock * pima->channels * sizeof (short)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (pima->block, 1, pima->blocksize * pima->channels, psf)) != pima->blocksize * pima->channels)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, pima->blocksize) ;

	/* Read and check the block header. */
	for (chan = 0 ; chan < pima->channels ; chan++)
	{	blockdata = pima->block + chan * 34 ;
		sampledata = pima->samples + chan ;

		predictor = (blockdata [0] << 8) | (blockdata [1] & 0x80) ;
		stepindx = blockdata [1] & 0x7F ;

{
if (count < 5)
printf ("\nchan: %d    predictor: %d    stepindx: %d (%d)\n",
	chan, predictor, stepindx, ima_step_size [stepindx]) ;
}
		/* FIXME : Do this a better way. */
		if (stepindx < 0) stepindx = 0 ;
		else if (stepindx > 88)	stepindx = 88 ;

		/*
		**	Pull apart the packed 4 bit samples and store them in their
		**	correct sample positions.
		*/
		for (k = 0 ; k < pima->blocksize - 2 ; k++)
		{	bytecode = blockdata [k + 2] ;
			sampledata [pima->channels * (2 * k + 0)] = bytecode & 0xF ;
			sampledata [pima->channels * (2 * k + 1)] = (bytecode >> 4) & 0xF ;
			} ;

		/* Decode the encoded 4 bit samples. */
		for (k = 0 ; k < pima->samplesperblock ; k ++)
		{	step = ima_step_size [stepindx] ;

			bytecode = pima->samples [pima->channels * k + chan] ;

			stepindx += ima_indx_adjust [bytecode] ;

			if (stepindx < 0) stepindx = 0 ;
			else if (stepindx > 88) stepindx = 88 ;

			diff = step >> 3 ;
			if (bytecode & 1)	diff += step >> 2 ;
			if (bytecode & 2)	diff += step >> 1 ;
			if (bytecode & 4)	diff += step ;
			if (bytecode & 8)	diff = -diff ;

			predictor += diff ;

			pima->samples [pima->channels * k + chan] = predictor ;
			} ;
		} ;

if (count < 5)
{
	for (k = 0 ; k < 10 ; k++)
		printf ("% 7d,", pima->samples [k]) ;
	puts ("") ;
}

	return 1 ;
} /* aiff_ima_decode_block */

static int
aiff_ima_encode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima)
{	int		chan, k, step, diff, vpdiff, blockindx, indx ;
	short	bytecode, mask ;

static int count = 0 ;
if (0 && count == 0)
{	pima->samples [0] = 0 ;
	printf ("blocksize : %d\n", pima->blocksize) ;
	printf ("pima->stepindx [0] : %d\n", pima->stepindx [0]) ;
	}
count ++ ;

	/* Encode the block header. */
	for (chan = 0 ; chan < pima->channels ; chan ++)
	{	blockindx = chan * pima->blocksize ;

		pima->block [blockindx] = (pima->samples [chan] >> 8) & 0xFF ;
		pima->block [blockindx + 1] = (pima->samples [chan] & 0x80) + (pima->stepindx [chan] & 0x7F) ;
		
		pima->previous [chan] = pima->samples [chan] ;
		} ;

	/* Encode second and later samples for every block as a 4 bit value. */
	for (k = pima->channels ; k < (pima->samplesperblock * pima->channels) ; k ++)
	{	chan = (pima->channels > 1) ? (k % 2) : 0 ;

		diff = pima->samples [k] - pima->previous [chan] ;

		bytecode = 0 ;
		step = ima_step_size [pima->stepindx [chan]] ;
		vpdiff = step >> 3 ;
		if (diff < 0)
		{	bytecode = 8 ;
			diff = -diff ;
			} ;
		mask = 4 ;
		while (mask)
		{	if (diff >= step)
			{	bytecode |= mask ;
				diff -= step ;
				vpdiff += step ;
				} ;
			step >>= 1 ;
			mask >>= 1 ;
			} ;

		if (bytecode & 8)
			pima->previous [chan] -= vpdiff ;
		else
			pima->previous [chan] += vpdiff ;

		if (pima->previous [chan] > 32767)
			pima->previous [chan] = 32767 ;
		else if (pima->previous [chan] < -32768)
			pima->previous [chan] = -32768 ;

		pima->stepindx [chan] += ima_indx_adjust [bytecode] ;
		if (pima->stepindx [chan] < 0)
			pima->stepindx [chan] = 0 ;
		else if (pima->stepindx [chan] > 88)
			pima->stepindx [chan] = 88 ;

		pima->samples [k] = bytecode ;
		} ;

	/* Pack the 4 bit encoded samples. */

	for (chan = 0 ; chan < pima->channels ; chan ++)
	{	for (indx = pima->channels ; indx < pima->channels * pima->samplesperblock ; indx += 2 * pima->channels)
		{	blockindx = chan * pima->blocksize + 2  + indx / 2;

if (0 && count ++ < 5)
	printf ("chan: %d    blockindx: %3d    indx: %3d\n", chan, blockindx, indx) ;
		
			pima->block [blockindx] = pima->samples [indx] & 0x0F ;
			pima->block [blockindx] |= (pima->samples [indx + pima->channels] << 4) & 0xF0 ;
			} ;
		} ;

	/* Write the block to disk. */

	if ((k = psf_fwrite (pima->block, 1, pima->channels * pima->blocksize, psf)) != pima->channels * pima->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, pima->channels * pima->blocksize) ;

	memset (pima->samples, 0, pima->channels * pima->samplesperblock * sizeof (short)) ;
	pima->samplecount = 0 ;
	pima->blockcount ++ ;

	return 1 ;
} /* aiff_ima_encode_block */

static int
wav_w64_ima_decode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima)
{	int		chan, k, current, blockindx, indx, indxstart, diff ;
	short	step, bytecode, stepindx [2] ;

	pima->blockcount ++ ;
	pima->samplecount = 0 ;

	if (pima->blockcount > pima->blocks)
	{	memset (pima->samples, 0, pima->samplesperblock * pima->channels * sizeof (short)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (pima->block, 1, pima->blocksize, psf)) != pima->blocksize)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, pima->blocksize) ;

	/* Read and check the block header. */

	for (chan = 0 ; chan < pima->channels ; chan++)
	{	current = pima->block [chan*4] | (pima->block [chan*4+1] << 8) ;
		if (current & 0x8000)
			current -= 0x10000 ;

		stepindx [chan] = pima->block [chan*4+2] ;
		if (stepindx [chan] < 0)
			stepindx [chan] = 0 ;
		else if (stepindx [chan] > 88)
			stepindx [chan] = 88 ;

		if (pima->block [chan*4+3] != 0)
			psf_log_printf (psf, "IMA ADPCM synchronisation error.\n") ;

		pima->samples [chan] = current ;
		} ;

	/*
	**	Pull apart the packed 4 bit samples and store them in their
	**	correct sample positions.
	*/

	blockindx = 4 * pima->channels ;

	indxstart = pima->channels ;
	while (blockindx < pima->blocksize)
	{	for (chan = 0 ; chan < pima->channels ; chan++)
		{	indx = indxstart + chan ;
			for (k = 0 ; k < 4 ; k++)
			{	bytecode = pima->block [blockindx++] ;
				pima->samples [indx] = bytecode & 0x0F ;
				indx += pima->channels ;
				pima->samples [indx] = (bytecode >> 4) & 0x0F ;
				indx += pima->channels ;
				} ;
			} ;
		indxstart += 8 * pima->channels ;
		} ;

	/* Decode the encoded 4 bit samples. */

	for (k = pima->channels ; k < (pima->samplesperblock * pima->channels) ; k ++)
	{	chan = (pima->channels > 1) ? (k % 2) : 0 ;

		bytecode = pima->samples [k] & 0xF ;

		step = ima_step_size [stepindx [chan]] ;
		current = pima->samples [k - pima->channels] ;

		diff = step >> 3 ;
		if (bytecode & 1)
			diff += step >> 2 ;
		if (bytecode & 2)
			diff += step >> 1 ;
		if (bytecode & 4)
			diff += step ;
		if (bytecode & 8)
			diff = -diff ;

		current += diff ;

		if (current > 32767)
			current = 32767 ;
		else if (current < -32768)
			current = -32768 ;

		stepindx [chan] += ima_indx_adjust [bytecode] ;

		if (stepindx [chan] < 0)
			stepindx [chan] = 0 ;
		else if (stepindx [chan] > 88)
			stepindx [chan] = 88 ;

		pima->samples [k] = current ;
		} ;

	return 1 ;
} /* wav_w64_ima_decode_block */

static int
wav_w64_ima_encode_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima)
{	int		chan, k, step, diff, vpdiff, blockindx, indx, indxstart ;
	short	bytecode, mask ;

	/* Encode the block header. */
	for (chan = 0 ; chan < pima->channels ; chan++)
	{	pima->block [chan*4]	= pima->samples [chan] & 0xFF ;
		pima->block [chan*4+1]	= (pima->samples [chan] >> 8) & 0xFF ;

		pima->block [chan*4+2] = pima->stepindx [chan] ;
		pima->block [chan*4+3] = 0 ;

		pima->previous [chan] = pima->samples [chan] ;
		} ;

	/* Encode the samples as 4 bit. */

	for (k = pima->channels ; k < (pima->samplesperblock * pima->channels) ; k ++)
	{	chan = (pima->channels > 1) ? (k % 2) : 0 ;

		diff = pima->samples [k] - pima->previous [chan] ;

		bytecode = 0 ;
		step = ima_step_size [pima->stepindx [chan]] ;
		vpdiff = step >> 3 ;
		if (diff < 0)
		{	bytecode = 8 ;
			diff = -diff ;
			} ;
		mask = 4 ;
		while (mask)
		{	if (diff >= step)
			{	bytecode |= mask ;
				diff -= step ;
				vpdiff += step ;
				} ;
			step >>= 1 ;
			mask >>= 1 ;
			} ;

		if (bytecode & 8)
			pima->previous [chan] -= vpdiff ;
		else
			pima->previous [chan] += vpdiff ;

		if (pima->previous [chan] > 32767)
			pima->previous [chan] = 32767 ;
		else if (pima->previous [chan] < -32768)
			pima->previous [chan] = -32768 ;

		pima->stepindx [chan] += ima_indx_adjust [bytecode] ;
		if (pima->stepindx [chan] < 0)
			pima->stepindx [chan] = 0 ;
		else if (pima->stepindx [chan] > 88)
			pima->stepindx [chan] = 88 ;

		pima->samples [k] = bytecode ;
		} ;

	/* Pack the 4 bit encoded samples. */

	blockindx = 4 * pima->channels ;

	indxstart = pima->channels ;
	while (blockindx < pima->blocksize)
	{	for (chan = 0 ; chan < pima->channels ; chan++)
		{	indx = indxstart + chan ;
			for (k = 0 ; k < 4 ; k++)
			{	pima->block [blockindx] = pima->samples [indx] & 0x0F ;
				indx += pima->channels ;
				pima->block [blockindx] |= (pima->samples [indx] << 4) & 0xF0 ;
				indx += pima->channels ;
				blockindx ++ ;
				} ;
			} ;
		indxstart += 8 * pima->channels ;
		} ;

	/* Write the block to disk. */

	if ((k = psf_fwrite (pima->block, 1, pima->blocksize, psf)) != pima->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, pima->blocksize) ;

	memset (pima->samples, 0, pima->samplesperblock * sizeof (short)) ;
	pima->samplecount = 0 ;
	pima->blockcount ++ ;

	return 1 ;
} /* wav_w64_ima_encode_block */

static int
ima_read_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima, short *ptr, int len)
{	int		count, total = 0, indx = 0 ;

	while (indx < len)
	{	if (pima->blockcount >= pima->blocks && pima->samplecount >= pima->samplesperblock)
		{	memset (&(ptr [indx]), 0, (size_t) ((len - indx) * sizeof (short))) ;
			return total ;
			} ;

		if (pima->samplecount >= pima->samplesperblock)
			pima->decode_block (psf, pima) ;

		count = (pima->samplesperblock - pima->samplecount) * pima->channels ;
		count = (len - indx > count) ? count : len - indx ;

		memcpy (&(ptr [indx]), &(pima->samples [pima->samplecount * pima->channels]), count * sizeof (short)) ;
		indx += count ;
		pima->samplecount += count / pima->channels ;
		total = indx ;
		} ;

	return total ;
} /* ima_read_block */

static sf_count_t
ima_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE 	*pima ;
	int			readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	readcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = ima_read_block (psf, pima, ptr, readcount) ;

		total += count ;
		len -= count ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* ima_read_s */

static sf_count_t
ima_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE *pima ;
	short		*sptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		count = ima_read_block (psf, pima, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = ((int) sptr [k]) << 16 ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* ima_read_i */

static sf_count_t
ima_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE *pima ;
	short		*sptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		count = ima_read_block (psf, pima, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (float) (sptr [k]) ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* ima_read_f */

static sf_count_t
ima_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE *pima ;
	short		*sptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		count = ima_read_block (psf, pima, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (double) (sptr [k]) ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* ima_read_d */

static sf_count_t
ima_seek (SF_PRIVATE *psf, int mode, sf_count_t offset)
{	IMA_ADPCM_PRIVATE *pima ;
	int			newblock, newsample ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	if (psf->datalength < 0 || psf->dataoffset < 0)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	if (offset == 0)
	{	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		pima->blockcount = 0 ;
		pima->decode_block (psf, pima) ;
		pima->samplecount = 0 ;
		return 0 ;
		} ;

	if (offset < 0 || offset > pima->blocks * pima->samplesperblock)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	newblock	= offset / pima->samplesperblock ;
	newsample	= offset % pima->samplesperblock ;

	if (mode == SFM_READ)
	{	psf_fseek (psf, psf->dataoffset + newblock * pima->blocksize, SEEK_SET) ;
		pima->blockcount = newblock ;
		pima->decode_block (psf, pima) ;
		pima->samplecount = newsample ;
		}
	else
	{	/* What to do about write??? */
		psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	return newblock * pima->samplesperblock + newsample ;
} /* ima_seek */

/*==========================================================================================
** IMA ADPCM Write Functions.
*/

static int
ima_writer_init (SF_PRIVATE *psf, int blockalign)
{	IMA_ADPCM_PRIVATE	*pima ;
	int					samplesperblock ;
	unsigned int 		pimasize ;

	if (psf->mode != SFM_WRITE)
		return SFE_BAD_MODE_RW ;

	samplesperblock = 2 * (blockalign - 4 * psf->sf.channels) / psf->sf.channels + 1 ;

	pimasize = sizeof (IMA_ADPCM_PRIVATE) + blockalign + 3 * psf->sf.channels * samplesperblock ;

	if ((pima = calloc (1, pimasize)) == NULL)
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pima ;

	pima->channels			= psf->sf.channels ;
	pima->blocksize			= blockalign ;
	pima->samplesperblock	= samplesperblock ;

	pima->block		= (unsigned char*) pima->data ;
	pima->samples	= (short*) (pima->data + blockalign) ;

	pima->samplecount = 0 ;

	switch (psf->sf.format & SF_FORMAT_TYPEMASK)
	{	case SF_FORMAT_WAV :
		case SF_FORMAT_W64 :
				pima->encode_block = wav_w64_ima_encode_block ;
				break ;

		case SF_FORMAT_AIFF :
				pima->encode_block = aiff_ima_encode_block ;
				break ;

		default :
				psf_log_printf (psf, "ima_reader_init: bad psf->sf.format\n") ;
				return SFE_INTERNAL ;
				break ;
		} ;

	psf->write_short	= ima_write_s ;
	psf->write_int		= ima_write_i ;
	psf->write_float	= ima_write_f ;
	psf->write_double	= ima_write_d ;

	return 0 ;
} /* ima_writer_init */

/*==========================================================================================
*/

static int
ima_write_block (SF_PRIVATE *psf, IMA_ADPCM_PRIVATE *pima, short *ptr, int len)
{	int		count, total = 0, indx = 0 ;

	while (indx < len)
	{	count = (pima->samplesperblock - pima->samplecount) * pima->channels ;

		if (count > len - indx)
			count = len - indx ;

		memcpy (&(pima->samples [pima->samplecount * pima->channels]), &(ptr [total]), count * sizeof (short)) ;
		indx += count ;
		pima->samplecount += count / pima->channels ;
		total = indx ;

		if (pima->samplecount >= pima->samplesperblock)
			pima->encode_block (psf, pima) ;
		} ;

	return total ;
} /* ima_write_block */

static sf_count_t
ima_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE 	*pima ;
	int			writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	while (len)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = ima_write_block (psf, pima, ptr, writecount) ;

		total += count ;
		len -= count ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* ima_write_s */

static sf_count_t
ima_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE *pima ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = ptr [total + k] >> 16 ;
		count = ima_write_block (psf, pima, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* ima_write_i */

static sf_count_t
ima_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE *pima ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = ima_write_block (psf, pima, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* ima_write_f */

static sf_count_t
ima_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	IMA_ADPCM_PRIVATE *pima ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	if (! psf->fdata)
		return 0 ;
	pima = (IMA_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrint (normfact * ptr [total + k]) ;
		count = ima_write_block (psf, pima, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* ima_write_d */


/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 75a54b82-ad18-4758-9933-64e00a7f24e0
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdlib.h>


#define		INTERLEAVE_CHANNELS		6

typedef struct
{	double	buffer [SF_BUFFER_LEN / sizeof (double)] ;

	sf_count_t		channel_len ;

	sf_count_t		(*read_short)	(SF_PRIVATE*, short *ptr, sf_count_t len) ;
	sf_count_t		(*read_int)	(SF_PRIVATE*, int *ptr, sf_count_t len) ;
	sf_count_t		(*read_float)	(SF_PRIVATE*, float *ptr, sf_count_t len) ;
	sf_count_t		(*read_double)	(SF_PRIVATE*, double *ptr, sf_count_t len) ;

} INTERLEAVE_DATA ;



static sf_count_t	interleave_read_short	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	interleave_read_int	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	interleave_read_float	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	interleave_read_double	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	interleave_seek	(SF_PRIVATE*, int mode, sf_count_t samples_from_start) ;




int
interleave_init	(SF_PRIVATE *psf)
{	INTERLEAVE_DATA *pdata ;

	if (psf->mode != SFM_READ)
		return SFE_INTERLEAVE_MODE ;

	if (psf->interleave)
	{	psf_log_printf (psf, "*** Weird, already have interleave.\n") ;
		return 666 ;
		} ;

	/* Free this in sf_close() function. */
	if (! (pdata = malloc (sizeof (INTERLEAVE_DATA))))
		return SFE_MALLOC_FAILED ;

puts ("interleave_init") ;

	psf->interleave = pdata ;

	/* Save the existing methods. */
	pdata->read_short	= psf->read_short ;
	pdata->read_int		= psf->read_int ;
	pdata->read_float	= psf->read_float ;
	pdata->read_double	= psf->read_double ;

	pdata->channel_len = psf->sf.frames * psf->bytewidth ;

	/* Insert our new methods. */
	psf->read_short		= interleave_read_short ;
	psf->read_int		= interleave_read_int ;
	psf->read_float		= interleave_read_float ;
	psf->read_double	= interleave_read_double ;

	psf->seek = interleave_seek ;

	return 0 ;
} /* pcm_interleave_init */

/*------------------------------------------------------------------------------
*/

static sf_count_t
interleave_read_short	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	INTERLEAVE_DATA *pdata ;
	sf_count_t	offset, templen ;
	int			chan, count, k ;
	short		*inptr, *outptr ;

	if (! (pdata = psf->interleave))
		return 0 ;

	inptr = (short*) pdata->buffer ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	outptr = ptr + chan ;

		offset = psf->dataoffset + chan * psf->bytewidth * psf->read_current ;

		if (psf_fseek (psf, offset, SEEK_SET) != offset)
		{	psf->error = SFE_INTERLEAVE_SEEK ;
			return 0 ;
			} ;

		templen = len / psf->sf.channels ;

		while (templen > 0)
		{	if (templen > SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (short))
				count = SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (short) ;
			else
				count = (int) templen ;

			if (pdata->read_short (psf, inptr, count) != count)
			{	psf->error = SFE_INTERLEAVE_READ ;
				return 0 ;
				} ;

			for (k = 0 ; k < count ; k++)
			{	*outptr = inptr [k] ;
				outptr += psf->sf.channels ;
				} ;

			templen -= count ;
			} ;
		} ;

	return len ;
} /* interleave_read_short */

static sf_count_t
interleave_read_int	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	INTERLEAVE_DATA *pdata ;
	sf_count_t	offset, templen ;
	int			chan, count, k ;
	int		*inptr, *outptr ;

	if (! (pdata = psf->interleave))
		return 0 ;

	inptr = (int*) pdata->buffer ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	outptr = ptr + chan ;

		offset = psf->dataoffset + chan * psf->bytewidth * psf->read_current ;

		if (psf_fseek (psf, offset, SEEK_SET) != offset)
		{	psf->error = SFE_INTERLEAVE_SEEK ;
			return 0 ;
			} ;

		templen = len / psf->sf.channels ;

		while (templen > 0)
		{	if (templen > SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (int))
				count = SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (int) ;
			else
				count = (int) templen ;

			if (pdata->read_int (psf, inptr, count) != count)
			{	psf->error = SFE_INTERLEAVE_READ ;
				return 0 ;
				} ;

			for (k = 0 ; k < count ; k++)
			{	*outptr = inptr [k] ;
				outptr += psf->sf.channels ;
				} ;

			templen -= count ;
			} ;
		} ;

	return len ;
} /* interleave_read_int */

static sf_count_t
interleave_read_float	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	INTERLEAVE_DATA *pdata ;
	sf_count_t	offset, templen ;
	int			chan, count, k ;
	float		*inptr, *outptr ;

	if (! (pdata = psf->interleave))
		return 0 ;

	inptr = (float*) pdata->buffer ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	outptr = ptr + chan ;

		offset = psf->dataoffset + pdata->channel_len * chan + psf->read_current * psf->bytewidth ;

/*-printf ("chan : %d     read_current : %6lld    offset : %6lld\n", chan, psf->read_current, offset) ;-*/

		if (psf_fseek (psf, offset, SEEK_SET) != offset)
		{	psf->error = SFE_INTERLEAVE_SEEK ;
/*-puts ("interleave_seek error") ; exit (1) ;-*/
			return 0 ;
			} ;

		templen = len / psf->sf.channels ;

		while (templen > 0)
		{	if (templen > SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (float))
				count = SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (float) ;
			else
				count = (int) templen ;

			if (pdata->read_float (psf, inptr, count) != count)
			{	psf->error = SFE_INTERLEAVE_READ ;
/*-puts ("interleave_read error") ; exit (1) ;-*/
				return 0 ;
				} ;

			for (k = 0 ; k < count ; k++)
			{	*outptr = inptr [k] ;
				outptr += psf->sf.channels ;
				} ;

			templen -= count ;
			} ;
		} ;

	return len ;
} /* interleave_read_float */

static sf_count_t
interleave_read_double	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	INTERLEAVE_DATA *pdata ;
	sf_count_t	offset, templen ;
	int			chan, count, k ;
	double		*inptr, *outptr ;

	if (! (pdata = psf->interleave))
		return 0 ;

	inptr = (double*) pdata->buffer ;

	for (chan = 0 ; chan < psf->sf.channels ; chan++)
	{	outptr = ptr + chan ;

		offset = psf->dataoffset + chan * psf->bytewidth * psf->read_current ;

		if (psf_fseek (psf, offset, SEEK_SET) != offset)
		{	psf->error = SFE_INTERLEAVE_SEEK ;
			return 0 ;
			} ;

		templen = len / psf->sf.channels ;

		while (templen > 0)
		{	if (templen > SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (double))
				count = SIGNED_SIZEOF (pdata->buffer) / SIGNED_SIZEOF (double) ;
			else
				count = (int) templen ;

			if (pdata->read_double (psf, inptr, count) != count)
			{	psf->error = SFE_INTERLEAVE_READ ;
				return 0 ;
				} ;

			for (k = 0 ; k < count ; k++)
			{	*outptr = inptr [k] ;
				outptr += psf->sf.channels ;
				} ;

			templen -= count ;
			} ;
		} ;

	return len ;
} /* interleave_read_double */

/*------------------------------------------------------------------------------
*/

static sf_count_t
interleave_seek	(SF_PRIVATE *psf, int mode, sf_count_t samples_from_start)
{	psf = psf ;	mode = mode ;

	/*
	** Do nothing here. This is a place holder to prevent the default
	** seek function from being called.
	*/

	return samples_from_start ;
} /* interleave_seek */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 82314e13-0225-4408-a2f2-e6dab3f38904
*/
/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

/* The IRCAM magic number is weird in that one byte in the number can have
** values of 0x1, 0x2, 0x03 or 0x04. Hence the need for a marker and a mask.
*/

#define IRCAM_BE_MASK		(MAKE_MARKER (0xFF, 0xFF, 0x00, 0xFF))
#define IRCAM_BE_MARKER		(MAKE_MARKER (0x64, 0xA3, 0x00, 0x00))

#define IRCAM_LE_MASK		(MAKE_MARKER (0xFF, 0x00, 0xFF, 0xFF))
#define IRCAM_LE_MARKER		(MAKE_MARKER (0x00, 0x00, 0xA3, 0x64))

#define IRCAM_02B_MARKER	(MAKE_MARKER (0x00, 0x02, 0xA3, 0x64))
#define IRCAM_03L_MARKER	(MAKE_MARKER (0x64, 0xA3, 0x03, 0x00))

#define IRCAM_DATA_OFFSET	(1024)

/*------------------------------------------------------------------------------
** Typedefs.
*/

enum
{	IRCAM_PCM_16	= 0x00002,
	IRCAM_FLOAT		= 0x00004,
	IRCAM_ALAW		= 0x10001,
	IRCAM_ULAW		= 0x20001,
	IRCAM_PCM_32	= 0x40004
} ;


/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int		ircam_close			(SF_PRIVATE *psf) ;
static	int		ircam_write_header	(SF_PRIVATE *psf, int calc_length) ;
static	int		ircam_read_header	(SF_PRIVATE *psf) ;

static	int		get_encoding (int subformat) ;

static	const char*	get_encoding_str (int encoding) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
ircam_open	(SF_PRIVATE *psf)
{	int		subformat ;
	int		error = SFE_NO_ERROR ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = ircam_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_IRCAM)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		if (psf->endian == 0 || psf->endian == SF_ENDIAN_CPU)
			psf->endian = (CPU_IS_BIG_ENDIAN) ? SF_ENDIAN_BIG : SF_ENDIAN_LITTLE ;

	 	psf->dataoffset = IRCAM_DATA_OFFSET ;

		if ((error = ircam_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = ircam_write_header ;
		} ;

	psf->close = ircam_close ;

	switch (subformat)
	{	case SF_FORMAT_ULAW :		/* 8-bit Ulaw encoding. */
				error = ulaw_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :		/* 8-bit Alaw encoding. */
				error = alaw_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :	/* 16-bit linear PCM. */
		case SF_FORMAT_PCM_32 :	/* 32-bit linear PCM. */
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_FLOAT :	/* 32-bit linear PCM. */
				error = float32_init (psf) ;
				break ;

		default : break ;
		} ;

	return error ;
} /* ircam_open */

/*------------------------------------------------------------------------------
*/

static int
ircam_read_header	(SF_PRIVATE *psf)
{	unsigned int	marker, encoding ;
	float			samplerate ;
	int				error = SFE_NO_ERROR ;

	psf_binheader_readf (psf, "epmf44", 0, &marker, &samplerate, &(psf->sf.channels), &encoding) ;

	if (((marker & IRCAM_LE_MASK) != IRCAM_LE_MARKER) &&
		((marker & IRCAM_BE_MASK) != IRCAM_BE_MARKER))
	{	psf_log_printf (psf, "marker: 0x%X\n", marker) ;
		return SFE_IRCAM_NO_MARKER ;
		} ;

	psf->endian = SF_ENDIAN_LITTLE ;

	if (psf->sf.channels > 256)
	{	psf_binheader_readf (psf, "Epmf44", 0, &marker, &samplerate, &(psf->sf.channels), &encoding) ;

		/* Sanity checking for endian-ness detection. */
		if (psf->sf.channels > 256)
		{	psf_log_printf (psf, "marker: 0x%X\n", marker) ;
			return SFE_IRCAM_BAD_CHANNELS ;
			} ;

		psf->endian = SF_ENDIAN_BIG ;
		} ;

	psf_log_printf (psf, "marker: 0x%X\n", marker) ;

	psf->sf.samplerate = (int) samplerate ;

	psf_log_printf (psf, "  Sample Rate : %d\n"
						 "  Channels    : %d\n"
						 "  Encoding    : %X => %s\n", psf->sf.samplerate, psf->sf.channels, encoding, get_encoding_str (encoding)) ;

	switch (encoding)
	{	case IRCAM_PCM_16 :
				psf->bytewidth = 2 ;
				psf->blockwidth = psf->sf.channels * psf->bytewidth ;

				psf->sf.format = SF_FORMAT_IRCAM | SF_FORMAT_PCM_16 ;
				break ;

		case IRCAM_PCM_32 :
				psf->bytewidth = 4 ;
				psf->blockwidth = psf->sf.channels * psf->bytewidth ;

				psf->sf.format = SF_FORMAT_IRCAM | SF_FORMAT_PCM_32 ;
				break ;

		case IRCAM_FLOAT :
				psf->bytewidth = 4 ;
				psf->blockwidth = psf->sf.channels * psf->bytewidth ;

				psf->sf.format = SF_FORMAT_IRCAM | SF_FORMAT_FLOAT ;
				break ;

		case IRCAM_ALAW :
				psf->bytewidth = 1 ;
				psf->blockwidth = psf->sf.channels * psf->bytewidth ;

				psf->sf.format = SF_FORMAT_IRCAM | SF_FORMAT_ALAW ;
				break ;

		case IRCAM_ULAW :
				psf->bytewidth = 1 ;
				psf->blockwidth = psf->sf.channels * psf->bytewidth ;

				psf->sf.format = SF_FORMAT_IRCAM | SF_FORMAT_ULAW ;
				break ;

		default :
				error = SFE_IRCAM_UNKNOWN_FORMAT ;
				break ;
		} ;

	if (psf->endian == SF_ENDIAN_BIG)
		psf->sf.format |= SF_ENDIAN_BIG ;
	else
		psf->sf.format |= SF_ENDIAN_LITTLE ;

	if (error)
		return error ;

	psf->dataoffset = IRCAM_DATA_OFFSET ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	if (psf->sf.frames == 0 && psf->blockwidth)
		psf->sf.frames = psf->datalength / psf->blockwidth ;

	psf_log_printf (psf, "  Samples     : %d\n", psf->sf.frames) ;

	psf_binheader_readf (psf, "p", IRCAM_DATA_OFFSET) ;

	return 0 ;
} /* ircam_read_header */

static int
ircam_close	(SF_PRIVATE *psf)
{
	psf_log_printf (psf, "close\n") ;

	return 0 ;
} /* ircam_close */

static int
ircam_write_header (SF_PRIVATE *psf, int calc_length)
{	int			encoding ;
	float		samplerate ;
	sf_count_t	current ;

	if (psf->pipeoffset > 0)
		return 0 ;

	current = psf_ftell (psf) ;

	calc_length = calc_length ;

	/* This also sets psf->endian. */
	encoding = get_encoding (psf->sf.format & SF_FORMAT_SUBMASK) ;

	if (encoding == 0)
		return SFE_BAD_OPEN_FORMAT ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	if (psf->is_pipe == SF_FALSE)
		psf_fseek (psf, 0, SEEK_SET) ;

	samplerate = psf->sf.samplerate ;

	switch (psf->endian)
	{	case SF_ENDIAN_BIG :
			psf_binheader_writef (psf, "Emf", IRCAM_02B_MARKER, samplerate) ;
			psf_binheader_writef (psf, "E44", psf->sf.channels, encoding) ;
			break ;

		case SF_ENDIAN_LITTLE :
			psf_binheader_writef (psf, "emf", IRCAM_03L_MARKER, samplerate) ;
			psf_binheader_writef (psf, "e44", psf->sf.channels, encoding) ;
			break ;

		default : return SFE_BAD_OPEN_FORMAT ;
		} ;

	psf_binheader_writef (psf, "z", (size_t) (IRCAM_DATA_OFFSET - psf->headindex)) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* ircam_write_header */

static int
get_encoding (int subformat)
{	switch (subformat)
	{	case SF_FORMAT_PCM_16 :	return IRCAM_PCM_16 ;
		case SF_FORMAT_PCM_32 :	return IRCAM_PCM_32 ;

		case SF_FORMAT_FLOAT :	return IRCAM_FLOAT ;

		case SF_FORMAT_ULAW :	return IRCAM_ULAW ;
		case SF_FORMAT_ALAW :	return IRCAM_ALAW ;

		default : break ;
		} ;

	return 0 ;
} /* get_encoding */

static const char*
get_encoding_str (int encoding)
{	switch (encoding)
	{	case IRCAM_PCM_16	: return "16 bit PCM" ;
		case IRCAM_FLOAT	: return "32 bit float" ;
		case IRCAM_ALAW		: return "A law" ;
		case IRCAM_ULAW		: return "u law" ;
		case IRCAM_PCM_32	: return "32 bit PCM" ;
		} ;
	return "Unknown encoding" ;
} /* get_encoding_str */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: f2714ab8-f286-4c94-9740-edaf673a1c71
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>



/*
 *  4.2.11 .. 4.2.12 LONG TERM PREDICTOR (LTP) SECTION
 */


/*
 * This module computes the LTP gain (bc) and the LTP lag (Nc)
 * for the long term analysis filter.   This is done by calculating a
 * maximum of the cross-correlation function between the current
 * sub-segment short term residual signal d[0..39] (output of
 * the short term analysis filter; for simplification the index
 * of this array begins at 0 and ends at 39 for each sub-segment of the
 * RPE-LTP analysis) and the previous reconstructed short term
 * residual signal dp[ -120 .. -1 ].  A dynamic scaling must be
 * performed to avoid overflow.
 */

 /* The next procedure exists in six versions.  First two integer
  * version (if USE_FLOAT_MUL is not defined); then four floating
  * point versions, twice with proper scaling (USE_FLOAT_MUL defined),
  * once without (USE_FLOAT_MUL and FAST defined, and fast run-time
  * option used).  Every pair has first a Cut version (see the -C
  * option to toast or the LTP_CUT option to gsm_option()), then the
  * uncut one.  (For a detailed explanation of why this is altogether
  * a bad idea, see Henry Spencer and Geoff Collyer, ``#ifdef Considered
  * Harmful''.)
  */

#ifndef  USE_FLOAT_MUL

#ifdef	LTP_CUT

static void Cut_Calculation_of_the_LTP_parameters (

	struct gsm_state * st,

	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;
	word		wt[40];

	longword	L_result;
	longword	L_max, L_power;
	word		R, S, dmax, scal, best_k;
	word		ltp_cut;

	register word	temp, wt_k;

	/*  Search of the optimum scaling of d[0..39].
	 */
	dmax = 0;
	for (k = 0; k <= 39; k++) {
		temp = d[k];
		temp = GSM_ABS( temp );
		if (temp > dmax) {
			dmax = temp;
			best_k = k;
		}
	}
	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		assert(dmax > 0);
		temp = gsm_norm( (longword)dmax << 16 );
	}
	if (temp > 6) scal = 0;
	else scal = 6 - temp;
	assert(scal >= 0);

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */
	wt_k  = SASR_W(d[best_k], scal);

	for (lambda = 40; lambda <= 120; lambda++) {
		L_result = (longword)wt_k * dp[best_k - lambda];
		if (L_result > L_max) {
			Nc    = lambda;
			L_max = L_result;
		}
	}
	*Nc_out = Nc;
	L_max <<= 1;

	/*  Rescaling of L_max
	 */
	assert(scal <= 100 && scal >= -100);
	L_max = L_max >> (6 - scal);	/* sub(6, scal) */

	assert( Nc <= 120 && Nc >= 40);

	/*   Compute the power of the reconstructed short term residual
	 *   signal dp[..]
	 */
	L_power = 0;
	for (k = 0; k <= 39; k++) {

		register longword L_temp;

		L_temp   = SASR_W( dp[k - Nc], 3 );
		L_power += L_temp * L_temp;
	}
	L_power <<= 1;	/* from L_MULT */

	/*  Normalization of L_max and L_power
	 */

	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR( L_max   << temp, 16 );
	S = SASR( L_power << temp, 16 );

	/*  Coding of the LTP gain
	 */

	/*  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}

#endif 	/* LTP_CUT */

static void Calculation_of_the_LTP_parameters (
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;
	word		wt[40];

	longword	L_max, L_power;
	word		R, S, dmax, scal;
	register word	temp;

	/*  Search of the optimum scaling of d[0..39].
	 */
	dmax = 0;

	for (k = 0; k <= 39; k++) {
		temp = d[k];
		temp = GSM_ABS( temp );
		if (temp > dmax) dmax = temp;
	}

	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		assert(dmax > 0);
		temp = gsm_norm( (longword)dmax << 16 );
	}

	if (temp > 6) scal = 0;
	else scal = 6 - temp;

	assert(scal >= 0);

	/*  Initialization of a working array wt
	 */

	for (k = 0; k <= 39; k++) wt[k] = SASR_W( d[k], scal );

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda++) {

# undef long_termSTEP
#		define long_termSTEP(k) 	(longword)wt[k] * dp[k - lambda]

		register longword L_result;

		L_result  = long_termSTEP(0)  ; L_result += long_termSTEP(1) ;
		L_result += long_termSTEP(2)  ; L_result += long_termSTEP(3) ;
		L_result += long_termSTEP(4)  ; L_result += long_termSTEP(5)  ;
		L_result += long_termSTEP(6)  ; L_result += long_termSTEP(7)  ;
		L_result += long_termSTEP(8)  ; L_result += long_termSTEP(9)  ;
		L_result += long_termSTEP(10) ; L_result += long_termSTEP(11) ;
		L_result += long_termSTEP(12) ; L_result += long_termSTEP(13) ;
		L_result += long_termSTEP(14) ; L_result += long_termSTEP(15) ;
		L_result += long_termSTEP(16) ; L_result += long_termSTEP(17) ;
		L_result += long_termSTEP(18) ; L_result += long_termSTEP(19) ;
		L_result += long_termSTEP(20) ; L_result += long_termSTEP(21) ;
		L_result += long_termSTEP(22) ; L_result += long_termSTEP(23) ;
		L_result += long_termSTEP(24) ; L_result += long_termSTEP(25) ;
		L_result += long_termSTEP(26) ; L_result += long_termSTEP(27) ;
		L_result += long_termSTEP(28) ; L_result += long_termSTEP(29) ;
		L_result += long_termSTEP(30) ; L_result += long_termSTEP(31) ;
		L_result += long_termSTEP(32) ; L_result += long_termSTEP(33) ;
		L_result += long_termSTEP(34) ; L_result += long_termSTEP(35) ;
		L_result += long_termSTEP(36) ; L_result += long_termSTEP(37) ;
		L_result += long_termSTEP(38) ; L_result += long_termSTEP(39) ;

		if (L_result > L_max) {

			Nc    = lambda;
			L_max = L_result;
		}
	}

	*Nc_out = Nc;

	L_max <<= 1;

	/*  Rescaling of L_max
	 */
	assert(scal <= 100 && scal >=  -100);
	L_max = L_max >> (6 - scal);	/* sub(6, scal) */

	assert( Nc <= 120 && Nc >= 40);

	/*   Compute the power of the reconstructed short term residual
	 *   signal dp[..]
	 */
	L_power = 0;
	for (k = 0; k <= 39; k++) {

		register longword L_temp;

		L_temp   = SASR_W( dp[k - Nc], 3 );
		L_power += L_temp * L_temp;
	}
	L_power <<= 1;	/* from L_MULT */

	/*  Normalization of L_max and L_power
	 */

	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR_L( L_max   << temp, 16 );
	S = SASR_L( L_power << temp, 16 );

	/*  Coding of the LTP gain
	 */

	/*  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}

#else	/* USE_FLOAT_MUL */

#ifdef	LTP_CUT

static void Cut_Calculation_of_the_LTP_parameters (
	struct gsm_state * st,		/*              IN 	*/
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;
	word		ltp_cut;

	float		wt_float[40];
	float		dp_float_base[120], * dp_float = dp_float_base + 120;

	longword	L_max, L_power;
	word		R, S, dmax, scal;
	register word	temp;

	/*  Search of the optimum scaling of d[0..39].
	 */
	dmax = 0;

	for (k = 0; k <= 39; k++) {
		temp = d[k];
		temp = GSM_ABS( temp );
		if (temp > dmax) dmax = temp;
	}

	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		assert(dmax > 0);
		temp = gsm_norm( (longword)dmax << 16 );
	}

	if (temp > 6) scal = 0;
	else scal = 6 - temp;

	assert(scal >= 0);
	ltp_cut = (longword)SASR_W(dmax, scal) * st->ltp_cut / 100; 


	/*  Initialization of a working array wt
	 */

	for (k = 0; k < 40; k++) {
		register word w = SASR_W( d[k], scal );
		if (w < 0 ? w > -ltp_cut : w < ltp_cut) {
			wt_float[k] = 0.0;
		}
		else {
			wt_float[k] =  w;
		}
	}
	for (k = -120; k <  0; k++) dp_float[k] =  dp[k];

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda += 9) {

		/*  Calculate L_result for l = lambda .. lambda + 9.
		 */
		register float *lp = dp_float - lambda;

		register float	W;
		register float	a = lp[-8], b = lp[-7], c = lp[-6],
				d = lp[-5], e = lp[-4], f = lp[-3],
				g = lp[-2], h = lp[-1];
		register float  E; 
		register float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
				S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#		undef long_termSTEP
#		define	long_termSTEP(K, a, b, c, d, e, f, g, h) \
			if ((W = wt_float[K]) != 0.0) {	\
			E = W * a; S8 += E;		\
			E = W * b; S7 += E;		\
			E = W * c; S6 += E;		\
			E = W * d; S5 += E;		\
			E = W * e; S4 += E;		\
			E = W * f; S3 += E;		\
			E = W * g; S2 += E;		\
			E = W * h; S1 += E;		\
			a  = lp[K];			\
			E = W * a; S0 += E; } else (a = lp[K])

#		define	long_termSTEP_A(K)	long_termSTEP(K, a, b, c, d, e, f, g, h)
#		define	long_termSTEP_B(K)	long_termSTEP(K, b, c, d, e, f, g, h, a)
#		define	long_termSTEP_C(K)	long_termSTEP(K, c, d, e, f, g, h, a, b)
#		define	long_termSTEP_D(K)	long_termSTEP(K, d, e, f, g, h, a, b, c)
#		define	long_termSTEP_E(K)	long_termSTEP(K, e, f, g, h, a, b, c, d)
#		define	long_termSTEP_F(K)	long_termSTEP(K, f, g, h, a, b, c, d, e)
#		define	long_termSTEP_G(K)	long_termSTEP(K, g, h, a, b, c, d, e, f)
#		define	long_termSTEP_H(K)	long_termSTEP(K, h, a, b, c, d, e, f, g)

		long_termSTEP_A( 0); long_termSTEP_B( 1); long_termSTEP_C( 2); long_termSTEP_D( 3);
		long_termSTEP_E( 4); long_termSTEP_F( 5); long_termSTEP_G( 6); long_termSTEP_H( 7);

		long_termSTEP_A( 8); long_termSTEP_B( 9); long_termSTEP_C(10); long_termSTEP_D(11);
		long_termSTEP_E(12); long_termSTEP_F(13); long_termSTEP_G(14); long_termSTEP_H(15);

		long_termSTEP_A(16); long_termSTEP_B(17); long_termSTEP_C(18); long_termSTEP_D(19);
		long_termSTEP_E(20); long_termSTEP_F(21); long_termSTEP_G(22); long_termSTEP_H(23);

		long_termSTEP_A(24); long_termSTEP_B(25); long_termSTEP_C(26); long_termSTEP_D(27);
		long_termSTEP_E(28); long_termSTEP_F(29); long_termSTEP_G(30); long_termSTEP_H(31);

		long_termSTEP_A(32); long_termSTEP_B(33); long_termSTEP_C(34); long_termSTEP_D(35);
		long_termSTEP_E(36); long_termSTEP_F(37); long_termSTEP_G(38); long_termSTEP_H(39);

		if (S0 > L_max) { L_max = S0; Nc = lambda;     }
		if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
		if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
		if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
		if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
		if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
		if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
		if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
		if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }

	}
	*Nc_out = Nc;

	L_max <<= 1;

	/*  Rescaling of L_max
	 */
	assert(scal <= 100 && scal >=  -100);
	L_max = L_max >> (6 - scal);	/* sub(6, scal) */

	assert( Nc <= 120 && Nc >= 40);

	/*   Compute the power of the reconstructed short term residual
	 *   signal dp[..]
	 */
	L_power = 0;
	for (k = 0; k <= 39; k++) {

		register longword L_temp;

		L_temp   = SASR_W( dp[k - Nc], 3 );
		L_power += L_temp * L_temp;
	}
	L_power <<= 1;	/* from L_MULT */

	/*  Normalization of L_max and L_power
	 */

	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR( L_max   << temp, 16 );
	S = SASR( L_power << temp, 16 );

	/*  Coding of the LTP gain
	 */

	/*  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}

#endif /* LTP_CUT */

static void Calculation_of_the_LTP_parameters (
	register word	* din,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;

	float		wt_float[40];
	float		dp_float_base[120], * dp_float = dp_float_base + 120;

	longword	L_max, L_power;
	word		R, S, dmax, scal;
	register word	temp;

	/*  Search of the optimum scaling of d[0..39].
	 */
	dmax = 0;

	for (k = 0; k <= 39; k++) {
		temp = din [k] ;
		temp = GSM_ABS (temp) ;
		if (temp > dmax) dmax = temp;
	}

	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		assert(dmax > 0);
		temp = gsm_norm( (longword)dmax << 16 );
	}

	if (temp > 6) scal = 0;
	else scal = 6 - temp;

	assert(scal >= 0);

	/*  Initialization of a working array wt
	 */

	for (k =    0; k < 40; k++) wt_float[k] =  SASR_W (din [k], scal) ;
	for (k = -120; k <  0; k++) dp_float[k] =  dp[k];

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda += 9) {

		/*  Calculate L_result for l = lambda .. lambda + 9.
		 */
		register float *lp = dp_float - lambda;

		register float	W;
		register float	a = lp[-8], b = lp[-7], c = lp[-6],
				d = lp[-5], e = lp[-4], f = lp[-3],
				g = lp[-2], h = lp[-1];
		register float  E; 
		register float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
				S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#		undef long_termSTEP
#		define	long_termSTEP(K, a, b, c, d, e, f, g, h) \
			W = wt_float[K];		\
			E = W * a; S8 += E;		\
			E = W * b; S7 += E;		\
			E = W * c; S6 += E;		\
			E = W * d; S5 += E;		\
			E = W * e; S4 += E;		\
			E = W * f; S3 += E;		\
			E = W * g; S2 += E;		\
			E = W * h; S1 += E;		\
			a  = lp[K];			\
			E = W * a; S0 += E

#		define	long_termSTEP_A(K)	long_termSTEP(K, a, b, c, d, e, f, g, h)
#		define	long_termSTEP_B(K)	long_termSTEP(K, b, c, d, e, f, g, h, a)
#		define	long_termSTEP_C(K)	long_termSTEP(K, c, d, e, f, g, h, a, b)
#		define	long_termSTEP_D(K)	long_termSTEP(K, d, e, f, g, h, a, b, c)
#		define	long_termSTEP_E(K)	long_termSTEP(K, e, f, g, h, a, b, c, d)
#		define	long_termSTEP_F(K)	long_termSTEP(K, f, g, h, a, b, c, d, e)
#		define	long_termSTEP_G(K)	long_termSTEP(K, g, h, a, b, c, d, e, f)
#		define	long_termSTEP_H(K)	long_termSTEP(K, h, a, b, c, d, e, f, g)

		long_termSTEP_A( 0); long_termSTEP_B( 1); long_termSTEP_C( 2); long_termSTEP_D( 3);
		long_termSTEP_E( 4); long_termSTEP_F( 5); long_termSTEP_G( 6); long_termSTEP_H( 7);

		long_termSTEP_A( 8); long_termSTEP_B( 9); long_termSTEP_C(10); long_termSTEP_D(11);
		long_termSTEP_E(12); long_termSTEP_F(13); long_termSTEP_G(14); long_termSTEP_H(15);

		long_termSTEP_A(16); long_termSTEP_B(17); long_termSTEP_C(18); long_termSTEP_D(19);
		long_termSTEP_E(20); long_termSTEP_F(21); long_termSTEP_G(22); long_termSTEP_H(23);

		long_termSTEP_A(24); long_termSTEP_B(25); long_termSTEP_C(26); long_termSTEP_D(27);
		long_termSTEP_E(28); long_termSTEP_F(29); long_termSTEP_G(30); long_termSTEP_H(31);

		long_termSTEP_A(32); long_termSTEP_B(33); long_termSTEP_C(34); long_termSTEP_D(35);
		long_termSTEP_E(36); long_termSTEP_F(37); long_termSTEP_G(38); long_termSTEP_H(39);

		if (S0 > L_max) { L_max = S0; Nc = lambda;     }
		if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
		if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
		if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
		if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
		if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
		if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
		if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
		if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }
	}
	*Nc_out = Nc;

	L_max <<= 1;

	/*  Rescaling of L_max
	 */
	assert(scal <= 100 && scal >=  -100);
	L_max = L_max >> (6 - scal);	/* sub(6, scal) */

	assert( Nc <= 120 && Nc >= 40);

	/*   Compute the power of the reconstructed short term residual
	 *   signal dp[..]
	 */
	L_power = 0;
	for (k = 0; k <= 39; k++) {

		register longword L_temp;

		L_temp   = SASR_W( dp[k - Nc], 3 );
		L_power += L_temp * L_temp;
	}
	L_power <<= 1;	/* from L_MULT */

	/*  Normalization of L_max and L_power
	 */

	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR_L ( L_max   << temp, 16 );
	S = SASR_L ( L_power << temp, 16 );

	/*  Coding of the LTP gain
	 */

	/*  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}

#ifdef	FAST
#ifdef	LTP_CUT

static void Cut_Fast_Calculation_of_the_LTP_parameters (
	struct gsm_state * st,		/*              IN	*/
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	register float	wt_float;
	word		Nc, bc;
	word		wt_max, best_k, ltp_cut;

	float		dp_float_base[120], * dp_float = dp_float_base + 120;

	register float	L_result, L_max, L_power;

	wt_max = 0;

	for (k = 0; k < 40; ++k) {
		if      ( d[k] > wt_max) wt_max =  d[best_k = k];
		else if (-d[k] > wt_max) wt_max = -d[best_k = k];
	}

	assert(wt_max >= 0);
	wt_float = (float)wt_max;

	for (k = -120; k < 0; ++k) dp_float[k] = (float)dp[k];

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda++) {
		L_result = wt_float * dp_float[best_k - lambda];
		if (L_result > L_max) {
			Nc    = lambda;
			L_max = L_result;
		}
	}

	*Nc_out = Nc;
	if (L_max <= 0.)  {
		*bc_out = 0;
		return;
	}

	/*  Compute the power of the reconstructed short term residual
	 *  signal dp[..]
	 */
	dp_float -= Nc;
	L_power = 0;
	for (k = 0; k < 40; ++k) {
		register float f = dp_float[k];
		L_power += f * f;
	}

	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	/*  Coding of the LTP gain
	 *  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	lambda = L_max / L_power * 32768.;
	for (bc = 0; bc <= 2; ++bc) if (lambda <= gsm_DLB[bc]) break;
	*bc_out = bc;
}

#endif /* LTP_CUT */

static void Fast_Calculation_of_the_LTP_parameters (
	register word	* din,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	register int  	k, lambda;
	word		Nc, bc;

	float		wt_float[40];
	float		dp_float_base[120], * dp_float = dp_float_base + 120;

	register float	L_max, L_power;

	for (k = 0; k < 40; ++k) wt_float[k] = (float) din [k] ;
	for (k = -120; k < 0; ++k) dp_float[k] = (float) dp [k] ;

	/* Search for the maximum cross-correlation and coding of the LTP lag
	 */
	L_max = 0;
	Nc    = 40;	/* index for the maximum cross-correlation */

	for (lambda = 40; lambda <= 120; lambda += 9) {

		/*  Calculate L_result for l = lambda .. lambda + 9.
		 */
		register float *lp = dp_float - lambda;

		register float	W;
		register float	a = lp[-8], b = lp[-7], c = lp[-6],
				d = lp[-5], e = lp[-4], f = lp[-3],
				g = lp[-2], h = lp[-1];
		register float  E; 
		register float  S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0,
				S5 = 0, S6 = 0, S7 = 0, S8 = 0;

#		undef long_termSTEP
#		define	long_termSTEP(K, a, b, c, d, e, f, g, h) \
			W = wt_float[K];		\
			E = W * a; S8 += E;		\
			E = W * b; S7 += E;		\
			E = W * c; S6 += E;		\
			E = W * d; S5 += E;		\
			E = W * e; S4 += E;		\
			E = W * f; S3 += E;		\
			E = W * g; S2 += E;		\
			E = W * h; S1 += E;		\
			a  = lp[K];			\
			E = W * a; S0 += E

#		define	long_termSTEP_A(K)	long_termSTEP(K, a, b, c, d, e, f, g, h)
#		define	long_termSTEP_B(K)	long_termSTEP(K, b, c, d, e, f, g, h, a)
#		define	long_termSTEP_C(K)	long_termSTEP(K, c, d, e, f, g, h, a, b)
#		define	long_termSTEP_D(K)	long_termSTEP(K, d, e, f, g, h, a, b, c)
#		define	long_termSTEP_E(K)	long_termSTEP(K, e, f, g, h, a, b, c, d)
#		define	long_termSTEP_F(K)	long_termSTEP(K, f, g, h, a, b, c, d, e)
#		define	long_termSTEP_G(K)	long_termSTEP(K, g, h, a, b, c, d, e, f)
#		define	long_termSTEP_H(K)	long_termSTEP(K, h, a, b, c, d, e, f, g)

		long_termSTEP_A( 0); long_termSTEP_B( 1); long_termSTEP_C( 2); long_termSTEP_D( 3);
		long_termSTEP_E( 4); long_termSTEP_F( 5); long_termSTEP_G( 6); long_termSTEP_H( 7);

		long_termSTEP_A( 8); long_termSTEP_B( 9); long_termSTEP_C(10); long_termSTEP_D(11);
		long_termSTEP_E(12); long_termSTEP_F(13); long_termSTEP_G(14); long_termSTEP_H(15);

		long_termSTEP_A(16); long_termSTEP_B(17); long_termSTEP_C(18); long_termSTEP_D(19);
		long_termSTEP_E(20); long_termSTEP_F(21); long_termSTEP_G(22); long_termSTEP_H(23);

		long_termSTEP_A(24); long_termSTEP_B(25); long_termSTEP_C(26); long_termSTEP_D(27);
		long_termSTEP_E(28); long_termSTEP_F(29); long_termSTEP_G(30); long_termSTEP_H(31);

		long_termSTEP_A(32); long_termSTEP_B(33); long_termSTEP_C(34); long_termSTEP_D(35);
		long_termSTEP_E(36); long_termSTEP_F(37); long_termSTEP_G(38); long_termSTEP_H(39);

		if (S0 > L_max) { L_max = S0; Nc = lambda;     }
		if (S1 > L_max) { L_max = S1; Nc = lambda + 1; }
		if (S2 > L_max) { L_max = S2; Nc = lambda + 2; }
		if (S3 > L_max) { L_max = S3; Nc = lambda + 3; }
		if (S4 > L_max) { L_max = S4; Nc = lambda + 4; }
		if (S5 > L_max) { L_max = S5; Nc = lambda + 5; }
		if (S6 > L_max) { L_max = S6; Nc = lambda + 6; }
		if (S7 > L_max) { L_max = S7; Nc = lambda + 7; }
		if (S8 > L_max) { L_max = S8; Nc = lambda + 8; }
	}
	*Nc_out = Nc;

	if (L_max <= 0.)  {
		*bc_out = 0;
		return;
	}

	/*  Compute the power of the reconstructed short term residual
	 *  signal dp[..]
	 */
	dp_float -= Nc;
	L_power = 0;
	for (k = 0; k < 40; ++k) {
		register float f = dp_float[k];
		L_power += f * f;
	}

	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	/*  Coding of the LTP gain
	 *  Table 4.3a must be used to obtain the level DLB[i] for the
	 *  quantization of the LTP gain b to get the coded version bc.
	 */
	lambda = L_max / L_power * 32768.;
	for (bc = 0; bc <= 2; ++bc) if (lambda <= gsm_DLB[bc]) break;
	*bc_out = bc;
}

#endif	/* FAST 	 */
#endif	/* USE_FLOAT_MUL */


/* 4.2.12 */

static void Long_term_analysis_filtering (
	word		bc,	/* 					IN  */
	word		Nc,	/* 					IN  */
	register word	* dp,	/* previous d	[-120..-1]		IN  */
	register word	* d,	/* d		[0..39]			IN  */
	register word	* dpp,	/* estimate	[0..39]			OUT */
	register word	* e	/* long term res. signal [0..39]	OUT */
)
/*
 *  In this part, we have to decode the bc parameter to compute
 *  the samples of the estimate dpp[0..39].  The decoding of bc needs the
 *  use of table 4.3b.  The long term residual signal e[0..39]
 *  is then calculated to be fed to the RPE encoding section.
 */
{
	register int      k;

#	undef long_termSTEP
#	define long_termSTEP(BP)					\
	for (k = 0; k <= 39; k++) {			\
		dpp[k]  = GSM_MULT_R( BP, dp[k - Nc]);	\
		e[k]	= GSM_SUB( d[k], dpp[k] );	\
	}

	switch (bc) {
	case 0:	long_termSTEP(  3277 ); break;
	case 1:	long_termSTEP( 11469 ); break;
	case 2: long_termSTEP( 21299 ); break;
	case 3: long_termSTEP( 32767 ); break; 
	}
}

void Gsm_Long_Term_Predictor (	/* 4x for 160 samples */

	struct gsm_state	* S,

	word	* d,	/* [0..39]   residual signal	IN	*/
	word	* dp,	/* [-120..-1] d'		IN	*/

	word	* e,	/* [0..39] 			OUT	*/
	word	* dpp,	/* [0..39] 			OUT	*/
	word	* Nc,	/* correlation lag		OUT	*/
	word	* bc	/* gain factor			OUT	*/
)
{
	assert( d  ); assert( dp ); assert( e  );
	assert( dpp); assert( Nc ); assert( bc );

#if defined(FAST) && defined(USE_FLOAT_MUL)
	if (S->fast) 
#if   defined (LTP_CUT)
		if (S->ltp_cut)
			Cut_Fast_Calculation_of_the_LTP_parameters(S,
				d, dp, bc, Nc);
		else
#endif /* LTP_CUT */
			Fast_Calculation_of_the_LTP_parameters(d, dp, bc, Nc );
	else 
#endif /* FAST & USE_FLOAT_MUL */
#ifdef LTP_CUT
		if (S->ltp_cut)
			Cut_Calculation_of_the_LTP_parameters(S, d, dp, bc, Nc);
		else
#endif
			Calculation_of_the_LTP_parameters(d, dp, bc, Nc);

	Long_term_analysis_filtering( *bc, *Nc, dp, d, dpp, e );
}

/* 4.3.2 */
void Gsm_Long_Term_Synthesis_Filtering (
	struct gsm_state	* S,

	word			Ncr,
	word			bcr,
	register word		* erp,	   /* [0..39]		  	 IN */
	register word		* drp	   /* [-120..-1] IN, [-120..40] OUT */
)
/*
 *  This procedure uses the bcr and Ncr parameter to realize the
 *  long term synthesis filtering.  The decoding of bcr needs
 *  table 4.3b.
 */
{
	register int 		k;
	word			brp, drpp, Nr;

	/*  Check the limits of Nr.
	 */
	Nr = Ncr < 40 || Ncr > 120 ? S->nrp : Ncr;
	S->nrp = Nr;
	assert(Nr >= 40 && Nr <= 120);

	/*  Decoding of the LTP gain bcr
	 */
	brp = gsm_QLB[ bcr ];

	/*  Computation of the reconstructed short term residual 
	 *  signal drp[0..39]
	 */
	assert(brp != MIN_WORD);

	for (k = 0; k <= 39; k++) {
		drpp   = GSM_MULT_R( brp, drp[ k - Nr ] );
		drp[k] = GSM_ADD( erp[k], drpp );
	}

	/*
	 *  Update of the reconstructed short term residual signal
	 *  drp[ -1..-120 ]
	 */

	for (k = 0; k <= 119; k++) drp[ -120 + k ] = drp[ -80 + k ];
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: b369b90d-0284-42a0-87b0-99a25bbd93ac
*/

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>



/*
 *  4.2.4 .. 4.2.7 LPC ANALYSIS SECTION
 */

/* 4.2.4 */


static void Autocorrelation (
	word     * s,		/* [0..159]	IN/OUT  */
 	longword * L_ACF)	/* [0..8]	OUT     */
/*
 *  The goal is to compute the array L_ACF[k].  The signal s[i] must
 *  be scaled in order to avoid an overflow situation.
 */
{
	register int	k, i;

	word		temp, smax, scalauto;

#ifdef	USE_FLOAT_MUL
	float		float_s[160];
#endif

	/*  Dynamic scaling of the array  s[0..159]
	 */

	/*  Search for the maximum.
	 */
	smax = 0;
	for (k = 0; k <= 159; k++) {
		temp = GSM_ABS( s[k] );
		if (temp > smax) smax = temp;
	}

	/*  Computation of the scaling factor.
	 */
	if (smax == 0) scalauto = 0;
	else {
		assert(smax > 0);
		scalauto = 4 - gsm_norm( (longword)smax << 16 );/* sub(4,..) */
	}

	/*  Scaling of the array s[0...159]
	 */

	if (scalauto > 0) {

# ifdef USE_FLOAT_MUL
#   define SCALE(n)	\
	case n: for (k = 0; k <= 159; k++) \
			float_s[k] = (float)	\
				(s[k] = GSM_MULT_R(s[k], 16384 >> (n-1)));\
		break;
# else 
#   define SCALE(n)	\
	case n: for (k = 0; k <= 159; k++) \
			s[k] = GSM_MULT_R( s[k], 16384 >> (n-1) );\
		break;
# endif /* USE_FLOAT_MUL */

		switch (scalauto) {
		SCALE(1)
		SCALE(2)
		SCALE(3)
		SCALE(4)
		}
# undef	SCALE
	}
# ifdef	USE_FLOAT_MUL
	else for (k = 0; k <= 159; k++) float_s[k] = (float) s[k];
# endif

	/*  Compute the L_ACF[..].
	 */
	{
# ifdef	USE_FLOAT_MUL
		register float * sp = float_s;
		register float   sl = *sp;

#		define lpcSTEP(k)	 L_ACF[k] += (longword)(sl * sp[ -(k) ]);
# else
		word  * sp = s;
		word    sl = *sp;

#		define lpcSTEP(k)	 L_ACF[k] += ((longword)sl * sp[ -(k) ]);
# endif

#	define NEXTI	 sl = *++sp


	for (k = 9; k--; L_ACF[k] = 0) ;

	lpcSTEP (0);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1); lpcSTEP(2);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1); lpcSTEP(2); lpcSTEP(3);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1); lpcSTEP(2); lpcSTEP(3); lpcSTEP(4);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1); lpcSTEP(2); lpcSTEP(3); lpcSTEP(4); lpcSTEP(5);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1); lpcSTEP(2); lpcSTEP(3); lpcSTEP(4); lpcSTEP(5); lpcSTEP(6);
	NEXTI;
	lpcSTEP(0); lpcSTEP(1); lpcSTEP(2); lpcSTEP(3); lpcSTEP(4); lpcSTEP(5); lpcSTEP(6); lpcSTEP(7);

	for (i = 8; i <= 159; i++) {

		NEXTI;

		lpcSTEP(0);
		lpcSTEP(1); lpcSTEP(2); lpcSTEP(3); lpcSTEP(4);
		lpcSTEP(5); lpcSTEP(6); lpcSTEP(7); lpcSTEP(8);
	}

	for (k = 9; k--; L_ACF[k] <<= 1) ; 

	}
	/*   Rescaling of the array s[0..159]
	 */
	if (scalauto > 0) {
		assert(scalauto <= 4); 
		for (k = 160; k--; *s++ <<= scalauto) ;
	}
}

#if defined(USE_FLOAT_MUL) && defined(FAST)

static void Fast_Autocorrelation (
	word * s,		/* [0..159]	IN/OUT  */
 	longword * L_ACF)	/* [0..8]	OUT     */
{
	register int	k, i;
	float f_L_ACF[9];
	float scale;

	float          s_f[160];
	register float *sf = s_f;

	for (i = 0; i < 160; ++i) sf[i] = s[i];
	for (k = 0; k <= 8; k++) {
		register float L_temp2 = 0;
		register float *sfl = sf - k;
		for (i = k; i < 160; ++i) L_temp2 += sf[i] * sfl[i];
		f_L_ACF[k] = L_temp2;
	}
	scale = MAX_LONGWORD / f_L_ACF[0];

	for (k = 0; k <= 8; k++) {
		L_ACF[k] = f_L_ACF[k] * scale;
	}
}
#endif	/* defined (USE_FLOAT_MUL) && defined (FAST) */

/* 4.2.5 */

static void Reflection_coefficients (
	longword	* L_ACF,		/* 0...8	IN	*/
	register word	* r			/* 0...7	OUT 	*/
)
{
	register int	i, m, n;
	register word	temp;
	word		ACF[9];	/* 0..8 */
	word		P[  9];	/* 0..8 */
	word		K[  9]; /* 2..8 */

	/*  Schur recursion with 16 bits arithmetic.
	 */

	if (L_ACF[0] == 0) {
		for (i = 8; i--; *r++ = 0) ;
		return;
	}

	assert( L_ACF[0] != 0 );
	temp = gsm_norm( L_ACF[0] );

	assert(temp >= 0 && temp < 32);

	/* ? overflow ? */
	for (i = 0; i <= 8; i++) ACF[i] = SASR_L( L_ACF[i] << temp, 16 );

	/*   Initialize array P[..] and K[..] for the recursion.
	 */

	for (i = 1; i <= 7; i++) K[ i ] = ACF[ i ];
	for (i = 0; i <= 8; i++) P[ i ] = ACF[ i ];

	/*   Compute reflection coefficients
	 */
	for (n = 1; n <= 8; n++, r++) {

		temp = P[1];
		temp = GSM_ABS(temp);
		if (P[0] < temp) {
			for (i = n; i <= 8; i++) *r++ = 0;
			return;
		}

		*r = gsm_div( temp, P[0] );

		assert(*r >= 0);
		if (P[1] > 0) *r = -*r;		/* r[n] = sub(0, r[n]) */
		assert (*r != MIN_WORD);
		if (n == 8) return; 

		/*  Schur recursion
		 */
		temp = GSM_MULT_R( P[1], *r );
		P[0] = GSM_ADD( P[0], temp );

		for (m = 1; m <= 8 - n; m++) {
			temp     = GSM_MULT_R( K[ m   ],    *r );
			P[m]     = GSM_ADD(    P[ m+1 ],  temp );

			temp     = GSM_MULT_R( P[ m+1 ],    *r );
			K[m]     = GSM_ADD(    K[ m   ],  temp );
		}
	}
}

/* 4.2.6 */

static void Transformation_to_Log_Area_Ratios (
	register word	* r 			/* 0..7	   IN/OUT */
)
/*
 *  The following scaling for r[..] and LAR[..] has been used:
 *
 *  r[..]   = integer( real_r[..]*32768. ); -1 <= real_r < 1.
 *  LAR[..] = integer( real_LAR[..] * 16384 );
 *  with -1.625 <= real_LAR <= 1.625
 */
{
	register word	temp;
	register int	i;


	/* Computation of the LAR[0..7] from the r[0..7]
	 */
	for (i = 1; i <= 8; i++, r++) {

		temp = *r;
		temp = GSM_ABS(temp);
		assert(temp >= 0);

		if (temp < 22118) {
			temp >>= 1;
		} else if (temp < 31130) {
			assert( temp >= 11059 );
			temp -= 11059;
		} else {
			assert( temp >= 26112 );
			temp -= 26112;
			temp <<= 2;
		}

		*r = *r < 0 ? -temp : temp;
		assert( *r != MIN_WORD );
	}
}

/* 4.2.7 */

static void Quantization_and_coding (
	register word * LAR    	/* [0..7]	IN/OUT	*/
)
{
	register word	temp;

	/*  This procedure needs four tables; the following equations
	 *  give the optimum scaling for the constants:
	 *  
	 *  A[0..7] = integer( real_A[0..7] * 1024 )
	 *  B[0..7] = integer( real_B[0..7] *  512 )
	 *  MAC[0..7] = maximum of the LARc[0..7]
	 *  MIC[0..7] = minimum of the LARc[0..7]
	 */

#	undef lpcSTEP
#	define	lpcSTEP( A, B, MAC, MIC )		\
		temp = GSM_MULT( A,   *LAR );	\
		temp = GSM_ADD(  temp,   B );	\
		temp = GSM_ADD(  temp, 256 );	\
		temp = SASR_W(     temp,   9 );	\
		*LAR  =  temp>MAC ? MAC - MIC : (temp<MIC ? 0 : temp - MIC); \
		LAR++;

	lpcSTEP(  20480,     0,  31, -32 );
	lpcSTEP(  20480,     0,  31, -32 );
	lpcSTEP(  20480,  2048,  15, -16 );
	lpcSTEP(  20480, -2560,  15, -16 );

	lpcSTEP(  13964,    94,   7,  -8 );
	lpcSTEP(  15360, -1792,   7,  -8 );
	lpcSTEP(   8534,  -341,   3,  -4 );
	lpcSTEP(   9036, -1144,   3,  -4 );

#	undef	lpcSTEP
}

void Gsm_LPC_Analysis (
	struct gsm_state *S,
	word 		 * s,		/* 0..159 signals	IN/OUT	*/
        word 		 * LARc)	/* 0..7   LARc's	OUT	*/
{
	longword	L_ACF[9];

#if defined(USE_FLOAT_MUL) && defined(FAST)
	if (S->fast) Fast_Autocorrelation (s,	  L_ACF );
	else
#endif
	Autocorrelation			  (s,	  L_ACF	);
	Reflection_coefficients		  (L_ACF, LARc	);
	Transformation_to_Log_Area_Ratios (LARc);
	Quantization_and_coding		  (LARc);
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 63146664-a002-4e1e-8b7b-f0cc8a6a53da
*/

/*
** Copyright (C) 2003,2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include	<stdlib.h>
#include	<string.h>


#if (OS_IS_MACOSX == 1)


int
macbinary3_open (SF_PRIVATE *psf)
{
	if (psf)
		return 0 ;

	return 0 ;
} /* macbinary3_open */

#else

int
macbinary3_open (SF_PRIVATE *psf)
{
	psf = psf ;
	return 0 ;
} /* macbinary3_open */

#endif /* OS_IS_MACOSX */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: c397a7d7-1a31-4349-9684-bd29ef06211e
*/
/*
** Copyright (C) 2003,2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>


#define	STR_MARKER	MAKE_MARKER ('S', 'T', 'R', ' ')

int
macos_guess_file_type (SF_PRIVATE *psf, const char *filename)
{	static char rsrc_name [1024] ;
	struct stat statbuf ;
	int format ;

	psf = psf ;

	snprintf (rsrc_name, sizeof (rsrc_name), "%s/rsrc", filename);

	/* If there is no resource fork, just return. */
	if (stat (rsrc_name, &statbuf) != 0)
	{	psf_log_printf (psf, "No resource fork.\n") ;
		return 0 ;
		} ;

	if (statbuf.st_size == 0)
	{	psf_log_printf (psf, "Have zero size resource fork.\n") ;
		return 0 ;
		} ;

	format = 0 ;

	return format ;
} /* macos_guess_file_type */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 5fbf66d7-9547-442a-9c73-92fd164f3a95
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
** Information on how to decode and encode this file was obtained in a PDF
** file which I found on http://www.wotsit.org/.
** Also did a lot of testing with GNU Octave but do not have access to
** Matlab (tm) and so could not test it there.
*/

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define MAT4_BE_DOUBLE	(MAKE_MARKER (0, 0, 0x03, 0xE8))
#define MAT4_LE_DOUBLE	(MAKE_MARKER (0, 0, 0, 0))

#define MAT4_BE_FLOAT	(MAKE_MARKER (0, 0, 0x03, 0xF2))
#define MAT4_LE_FLOAT	(MAKE_MARKER (0x0A, 0, 0, 0))

#define MAT4_BE_PCM_32	(MAKE_MARKER (0, 0, 0x03, 0xFC))
#define MAT4_LE_PCM_32	(MAKE_MARKER (0x14, 0, 0, 0))

#define MAT4_BE_PCM_16	(MAKE_MARKER (0, 0, 0x04, 0x06))
#define MAT4_LE_PCM_16	(MAKE_MARKER (0x1E, 0, 0, 0))

/* Can't see any reason to ever implement this. */
#define MAT4_BE_PCM_U8	(MAKE_MARKER (0, 0, 0x04, 0x1A))
#define MAT4_LE_PCM_U8	(MAKE_MARKER (0x32, 0, 0, 0))

/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int		mat4_close		(SF_PRIVATE *psf) ;

static	int 	mat4_format_to_encoding	(int format, int endian) ;

static int		mat4_write_header (SF_PRIVATE *psf, int calc_length) ;
static int		mat4_read_header (SF_PRIVATE *psf) ;

static const char * mat4_marker_to_str (int marker) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
mat4_open	(SF_PRIVATE *psf)
{	int		subformat, error = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = mat4_read_header (psf)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_MAT4)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		if (CPU_IS_LITTLE_ENDIAN && (psf->endian == SF_ENDIAN_CPU || psf->endian == 0))
			psf->endian = SF_ENDIAN_LITTLE ;
		else if (CPU_IS_BIG_ENDIAN && (psf->endian == SF_ENDIAN_CPU || psf->endian == 0))
			psf->endian = SF_ENDIAN_BIG ;

		if ((error = mat4_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = mat4_write_header ;
		} ;

	psf->close = mat4_close ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_FLOAT :
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :
				error = double64_init (psf) ;
				break ;

		default : break ;
		} ;

	if (error)
		return error ;

	return error ;
} /* mat4_open */

/*------------------------------------------------------------------------------
*/

static int
mat4_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		mat4_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* mat4_close */

/*------------------------------------------------------------------------------
*/

static int
mat4_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int			encoding ;
	double		samplerate ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	encoding = mat4_format_to_encoding (psf->sf.format & SF_FORMAT_SUBMASK, psf->endian) ;

	if (encoding == -1)
		return SFE_BAD_OPEN_FORMAT ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	/* Need sample rate as a double for writing to the header. */
	samplerate = psf->sf.samplerate ;

	if (psf->endian == SF_ENDIAN_BIG)
	{	psf_binheader_writef (psf, "Em444", MAT4_BE_DOUBLE, 1, 1, 0) ;
		psf_binheader_writef (psf, "E4bd", 11, "samplerate", 11, samplerate) ;
		psf_binheader_writef (psf, "tEm484", encoding, psf->sf.channels, psf->sf.frames, 0) ;
		psf_binheader_writef (psf, "E4b", 9, "wavedata", 9) ;
		}
	else if (psf->endian == SF_ENDIAN_LITTLE)
	{	psf_binheader_writef (psf, "em444", MAT4_LE_DOUBLE, 1, 1, 0) ;
		psf_binheader_writef (psf, "e4bd", 11, "samplerate", 11, samplerate) ;
		psf_binheader_writef (psf, "tem484", encoding, psf->sf.channels, psf->sf.frames, 0) ;
		psf_binheader_writef (psf, "e4b", 9, "wavedata", 9) ;
		}
	else
		return SFE_BAD_OPEN_FORMAT ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* mat4_write_header */

static int
mat4_read_header (SF_PRIVATE *psf)
{	int		marker, namesize, rows, cols, imag ;
	double	value ;
	const char *marker_str ;
	char	name [64] ;

	psf_binheader_readf (psf, "pm", 0, &marker) ;

	/* MAT4 file must start with a double for the samplerate. */
	if (marker == MAT4_BE_DOUBLE)
	{	psf->endian = psf->rwf_endian = SF_ENDIAN_BIG ;
		marker_str = "big endian double" ;
		}
	else if (marker == MAT4_LE_DOUBLE)
	{	psf->endian = psf->rwf_endian = SF_ENDIAN_LITTLE ;
		marker_str = "little endian double" ;
		}
	else
		return SFE_UNIMPLEMENTED ;

	psf_log_printf (psf, "GNU Octave 2.0 / MATLAB v4.2 format\nMarker : %s\n", marker_str) ;

	psf_binheader_readf (psf, "444", &rows, &cols, &imag) ;

	psf_log_printf (psf, " Rows  : %d\n Cols  : %d\n Imag  : %s\n", rows, cols, imag ? "True" : "False") ;

	psf_binheader_readf (psf, "4", &namesize) ;

	if (namesize >= SIGNED_SIZEOF (name))
		return SFE_MAT4_BAD_NAME ;

	psf_binheader_readf (psf, "b", name, namesize) ;
	name [namesize] = 0 ;

	psf_log_printf (psf, " Name  : %s\n", name) ;

	psf_binheader_readf (psf, "d", &value) ;

	LSF_SNPRINTF ((char*) psf->buffer, sizeof (psf->buffer), " Value : %f\n", value) ;
	psf_log_printf (psf, (char*) psf->buffer) ;

	if ((rows != 1) || (cols != 1))
		return SFE_MAT4_NO_SAMPLERATE ;

	psf->sf.samplerate = (int) value ;

	/* Now write out the audio data. */

	psf_binheader_readf (psf, "m", &marker) ;

	psf_log_printf (psf, "Marker : %s\n", mat4_marker_to_str (marker)) ;

	psf_binheader_readf (psf, "444", &rows, &cols, &imag) ;

	psf_log_printf (psf, " Rows  : %d\n Cols  : %d\n Imag  : %s\n", rows, cols, imag ? "True" : "False") ;

	psf_binheader_readf (psf, "4", &namesize) ;

	if (namesize >= SIGNED_SIZEOF (name))
		return SFE_MAT4_BAD_NAME ;

	psf_binheader_readf (psf, "b", name, namesize) ;
	name [namesize] = 0 ;

	psf_log_printf (psf, " Name  : %s\n", name) ;

	psf->dataoffset = psf_ftell (psf) ;

	if (rows == 0 && cols == 0)
	{	psf_log_printf (psf, "*** Error : zero channel count.\n") ;
		return SFE_MAT4_ZERO_CHANNELS ;
		} ;

	psf->sf.channels	= rows ;
	psf->sf.frames		= cols ;

	psf->sf.format = psf->endian | SF_FORMAT_MAT4 ;
	switch (marker)
	{	case MAT4_BE_DOUBLE :
		case MAT4_LE_DOUBLE :
				psf->sf.format |= SF_FORMAT_DOUBLE ;
				psf->bytewidth = 8 ;
				break ;

		case MAT4_BE_FLOAT :
		case MAT4_LE_FLOAT :
				psf->sf.format |= SF_FORMAT_FLOAT ;
				psf->bytewidth = 4 ;
				break ;

		case MAT4_BE_PCM_32	:
		case MAT4_LE_PCM_32	:
				psf->sf.format |= SF_FORMAT_PCM_32 ;
				psf->bytewidth = 4 ;
				break ;

		case MAT4_BE_PCM_16	:
		case MAT4_LE_PCM_16	:
				psf->sf.format |= SF_FORMAT_PCM_16 ;
				psf->bytewidth = 2 ;
				break ;

		default :
				psf_log_printf (psf, "*** Error : Bad marker %08X\n", marker) ;
				return SFE_UNIMPLEMENTED ;
		} ;

	if ((psf->filelength - psf->dataoffset) < psf->sf.channels * psf->sf.frames * psf->bytewidth)
	{	psf_log_printf (psf, "*** File seems to be truncated. %D <--> %D\n",
				psf->filelength - psf->dataoffset, psf->sf.channels * psf->sf.frames * psf->bytewidth) ;
		}
	else if ((psf->filelength - psf->dataoffset) > psf->sf.channels * psf->sf.frames * psf->bytewidth)
		psf->dataend = psf->dataoffset + rows * cols * psf->bytewidth ;

	psf->datalength = psf->filelength - psf->dataoffset - psf->dataend ;

	psf->sf.sections = 1 ;

	return 0 ;
} /* mat4_read_header */

static int
mat4_format_to_encoding (int format, int endian)
{
	switch (format | endian)
	{	case (SF_FORMAT_PCM_16 | SF_ENDIAN_BIG) :
				return MAT4_BE_PCM_16 ;

		case (SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE) :
				return MAT4_LE_PCM_16 ;

		case (SF_FORMAT_PCM_32 | SF_ENDIAN_BIG) :
				return MAT4_BE_PCM_32 ;

		case (SF_FORMAT_PCM_32 | SF_ENDIAN_LITTLE) :
				return MAT4_LE_PCM_32 ;

		case (SF_FORMAT_FLOAT | SF_ENDIAN_BIG) :
				return MAT4_BE_FLOAT ;

		case (SF_FORMAT_FLOAT | SF_ENDIAN_LITTLE) :
				return MAT4_LE_FLOAT ;

		case (SF_FORMAT_DOUBLE | SF_ENDIAN_BIG) :
				return MAT4_BE_DOUBLE ;

		case (SF_FORMAT_DOUBLE | SF_ENDIAN_LITTLE) :
				return MAT4_LE_DOUBLE ;

		default : break ;
		} ;

	return -1 ;
} /* mat4_format_to_encoding */

static const char *
mat4_marker_to_str (int marker)
{	static char str [32] ;

	switch (marker)
	{
		case MAT4_BE_PCM_16	:	return "big endian 16 bit PCM" ;
		case MAT4_LE_PCM_16	:	return "little endian 16 bit PCM" ;

		case MAT4_BE_PCM_32	:	return "big endian 32 bit PCM" ;
		case MAT4_LE_PCM_32	:	return "little endian 32 bit PCM" ;


		case MAT4_BE_FLOAT :	return "big endian float" ;
		case MAT4_LE_FLOAT :	return "big endian float" ;

		case MAT4_BE_DOUBLE	:	return "big endian double" ;
		case MAT4_LE_DOUBLE	:	return "little endian double" ;
		} ;

	/* This is a little unsafe but is really only for debugging. */
	str [sizeof (str) - 1] = 0 ;
	LSF_SNPRINTF (str, sizeof (str) - 1, "%08X", marker) ;
	return str ;
} /* mat4_marker_to_str */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: f7e5f5d6-fc39-452e-bc4a-59627116ff59
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
** Information on how to decode and encode this file was obtained in a PDF
** file which I found on http://www.wotsit.org/.
** Also did a lot of testing with GNU Octave but do not have access to
** Matlab (tm) and so could not test it there.
*/

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define MATL_MARKER	(MAKE_MARKER ('M', 'A', 'T', 'L'))

#define IM_MARKER	(('I' << 8) + 'M')
#define MI_MARKER	(('M' << 8) + 'I')

/*------------------------------------------------------------------------------
** Enums and typedefs.
*/

enum
{	MAT5_TYPE_SCHAR			= 0x1,
	MAT5_TYPE_UCHAR			= 0x2,
	MAT5_TYPE_INT16			= 0x3,
	MAT5_TYPE_UINT16		= 0x4,
	MAT5_TYPE_INT32			= 0x5,
	MAT5_TYPE_UINT32		= 0x6,
	MAT5_TYPE_FLOAT			= 0x7,
	MAT5_TYPE_DOUBLE		= 0x9,
	MAT5_TYPE_ARRAY			= 0xE,

	MAT5_TYPE_COMP_USHORT	= 0x00020004,
	MAT5_TYPE_COMP_UINT		= 0x00040006
} ;

typedef struct
{	sf_count_t	size ;
	int			rows, cols ;
	char		name [32] ;
} MAT5_MATRIX ;

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int		mat5_close		(SF_PRIVATE *psf) ;

static int		mat5_write_header (SF_PRIVATE *psf, int calc_length) ;
static int		mat5_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
mat5_open	(SF_PRIVATE *psf)
{	int		subformat, error = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = mat5_read_header (psf)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_MAT5)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		if (CPU_IS_LITTLE_ENDIAN && (psf->endian == SF_ENDIAN_CPU || psf->endian == 0))
			psf->endian = SF_ENDIAN_LITTLE ;
		else if (CPU_IS_BIG_ENDIAN && (psf->endian == SF_ENDIAN_CPU || psf->endian == 0))
			psf->endian = SF_ENDIAN_BIG ;

		if ((error = mat5_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = mat5_write_header ;
		} ;

	psf->close = mat5_close ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_FLOAT :
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :
				error = double64_init (psf) ;
				break ;

		default : break ;
		} ;

	return error ;
} /* mat5_open */

/*------------------------------------------------------------------------------
*/

static int
mat5_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		mat5_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* mat5_close */

/*------------------------------------------------------------------------------
*/

static int
mat5_write_header (SF_PRIVATE *psf, int calc_length)
{	static const char	*sr_name = "samplerate\0\0\0\0\0\0\0\0\0\0\0" ;
	static const char	*wd_name = "wavedata\0" ;
	sf_count_t	current, datasize ;
	int			encoding ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf_fseek (psf, 0, SEEK_END) ;
		psf->filelength = psf_ftell (psf) ;
		psf_fseek (psf, 0, SEEK_SET) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_U8 :
				encoding = MAT5_TYPE_UCHAR ;
				break ;

		case SF_FORMAT_PCM_16 :
				encoding = MAT5_TYPE_INT16 ;
				break ;

		case SF_FORMAT_PCM_32 :
				encoding = MAT5_TYPE_INT32 ;
				break ;

		case SF_FORMAT_FLOAT :
				encoding = MAT5_TYPE_FLOAT ;
				break ;

		case SF_FORMAT_DOUBLE :
				encoding = MAT5_TYPE_DOUBLE ;
				break ;

		default :
				return SFE_BAD_OPEN_FORMAT ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	psf_binheader_writef (psf, "S", "MATLAB 5.0 MAT-file, written by " PACKAGE "-" VERSION ", ") ;
	psf_get_date_str ((char*) psf->buffer, sizeof (psf->buffer)) ;
	psf_binheader_writef (psf, "jS", -1, psf->buffer) ;

	memset (psf->buffer, ' ', 124 - psf->headindex) ;
	psf_binheader_writef (psf, "b", psf->buffer, 124 - psf->headindex) ;

	psf->rwf_endian = psf->endian ;

	if (psf->rwf_endian == SF_ENDIAN_BIG)
		psf_binheader_writef (psf, "2b", 0x0100, "MI", 2) ;
	else
		psf_binheader_writef (psf, "2b", 0x0100, "IM", 2) ;

	psf_binheader_writef (psf, "444444", MAT5_TYPE_ARRAY, 64, MAT5_TYPE_UINT32, 8, 6, 0) ;
	psf_binheader_writef (psf, "4444", MAT5_TYPE_INT32, 8, 1, 1) ;
	psf_binheader_writef (psf, "44b", MAT5_TYPE_SCHAR, strlen (sr_name), sr_name, 16) ;

	if (psf->sf.samplerate > 0xFFFF)
		psf_binheader_writef (psf, "44", MAT5_TYPE_COMP_UINT, psf->sf.samplerate) ;
	else
	{	unsigned short samplerate = psf->sf.samplerate ;

		psf_binheader_writef (psf, "422", MAT5_TYPE_COMP_USHORT, samplerate, 0) ;
		} ;

	datasize = psf->sf.frames * psf->sf.channels * psf->bytewidth ;

	psf_binheader_writef (psf, "t484444", MAT5_TYPE_ARRAY, datasize + 64, MAT5_TYPE_UINT32, 8, 6, 0) ;
	psf_binheader_writef (psf, "t4448", MAT5_TYPE_INT32, 8, psf->sf.channels, psf->sf.frames) ;
	psf_binheader_writef (psf, "44b", MAT5_TYPE_SCHAR, strlen (wd_name), wd_name, strlen (wd_name)) ;

	datasize = psf->sf.frames * psf->sf.channels * psf->bytewidth ;
	if (datasize > 0x7FFFFFFF)
		datasize = 0x7FFFFFFF ;

	psf_binheader_writef (psf, "t48", encoding, datasize) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* mat5_write_header */

static int
mat5_read_header (SF_PRIVATE *psf)
{	char	name [32] ;
	short	version, endian ;
	int		type, size, flags1, flags2, rows, cols ;

	psf_binheader_readf (psf, "pb", 0, psf->buffer, 124) ;

	psf->buffer [125] = 0 ;

	if (strlen ((char*) psf->buffer) >= 124)
		return SFE_UNIMPLEMENTED ;

	if (strstr ((char*) psf->buffer, "MATLAB 5.0 MAT-file") == (char*) psf->buffer)
		psf_log_printf (psf, "%s\n", psf->buffer) ;


	psf_binheader_readf (psf, "E22", &version, &endian) ;

	if (endian == MI_MARKER)
	{	psf->endian = psf->rwf_endian = SF_ENDIAN_BIG ;
		if (CPU_IS_LITTLE_ENDIAN) version = ENDSWAP_SHORT (version) ;
		}
	else if (endian == IM_MARKER)
	{	psf->endian = psf->rwf_endian = SF_ENDIAN_LITTLE ;
		if (CPU_IS_BIG_ENDIAN) version = ENDSWAP_SHORT (version) ;
		}
	else
		return SFE_MAT5_BAD_ENDIAN ;

	if ((CPU_IS_LITTLE_ENDIAN && endian == IM_MARKER) ||
			(CPU_IS_BIG_ENDIAN && endian == MI_MARKER))
		version = ENDSWAP_SHORT (version) ;

	psf_log_printf (psf, "Version : 0x%04X\n", version) ;
	psf_log_printf (psf, "Endian  : 0x%04X => %s\n", endian,
				(psf->endian == SF_ENDIAN_LITTLE) ? "Little" : "Big") ;

	/*========================================================*/
	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, "Block\n Type : %X    Size : %d\n", type, size) ;

	if (type != MAT5_TYPE_ARRAY)
		return SFE_MAT5_NO_BLOCK ;

	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;

	if (type != MAT5_TYPE_UINT32)
		return SFE_MAT5_NO_BLOCK ;

	psf_binheader_readf (psf, "44", &flags1, &flags2) ;
	psf_log_printf (psf, "    Flg1 : %X    Flg2 : %d\n", flags1, flags2) ;

	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;

	if (type != MAT5_TYPE_INT32)
		return SFE_MAT5_NO_BLOCK ;

	psf_binheader_readf (psf, "44", &rows, &cols) ;
	psf_log_printf (psf, "    Rows : %X    Cols : %d\n", rows, cols) ;

	if (rows != 1 || cols != 1)
		return SFE_MAT5_SAMPLE_RATE ;

	psf_binheader_readf (psf, "4", &type) ;

	if (type == MAT5_TYPE_SCHAR)
	{	psf_binheader_readf (psf, "4", &size) ;
		psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;
		if (size > SIGNED_SIZEOF (name) - 1)
		{	psf_log_printf (psf, "Error : Bad name length.\n") ;
			return SFE_MAT5_NO_BLOCK ;
			} ;

		psf_binheader_readf (psf, "bj", name, size, (8 - (size % 8)) % 8) ;
		name [size] = 0 ;
		}
	else if ((type & 0xFFFF) == MAT5_TYPE_SCHAR)
	{	size = type >> 16 ;
		if (size > 4)
		{	psf_log_printf (psf, "Error : Bad name length.\n") ;
			return SFE_MAT5_NO_BLOCK ;
			} ;

		psf_log_printf (psf, "    Type : %X\n", type) ;
		psf_binheader_readf (psf, "4", &name) ;
		name [size] = 0 ;
		}
	else
		return SFE_MAT5_NO_BLOCK ;

	psf_log_printf (psf, "    Name : %s\n", name) ;

	/*-----------------------------------------*/

	psf_binheader_readf (psf, "44", &type, &size) ;

	switch (type)
	{	case MAT5_TYPE_DOUBLE :
				{	double	samplerate ;

					psf_binheader_readf (psf, "d", &samplerate) ;
					LSF_SNPRINTF (name, sizeof (name), "%f\n", samplerate) ;
					psf_log_printf (psf, "    Val  : %s\n", name) ;

					psf->sf.samplerate = lrint (samplerate) ;
					} ;
				break ;

		case MAT5_TYPE_COMP_USHORT :
				{	unsigned short samplerate ;

					psf_binheader_readf (psf, "j2j", -4, &samplerate, 2) ;
					psf_log_printf (psf, "    Val  : %u\n", samplerate) ;
					psf->sf.samplerate = samplerate ;
					}
				break ;

		case MAT5_TYPE_COMP_UINT :
				psf_log_printf (psf, "    Val  : %u\n", size) ;
				psf->sf.samplerate = size ;
				break ;

		default :
			psf_log_printf (psf, "    Type : %X    Size : %d  ***\n", type, size) ;
			return SFE_MAT5_SAMPLE_RATE ;
		} ;

	/*-----------------------------------------*/


	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, " Type : %X    Size : %d\n", type, size) ;

	if (type != MAT5_TYPE_ARRAY)
		return SFE_MAT5_NO_BLOCK ;

	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;

	if (type != MAT5_TYPE_UINT32)
		return SFE_MAT5_NO_BLOCK ;

	psf_binheader_readf (psf, "44", &flags1, &flags2) ;
	psf_log_printf (psf, "    Flg1 : %X    Flg2 : %d\n", flags1, flags2) ;

	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;

	if (type != MAT5_TYPE_INT32)
		return SFE_MAT5_NO_BLOCK ;

	psf_binheader_readf (psf, "44", &rows, &cols) ;
	psf_log_printf (psf, "    Rows : %X    Cols : %d\n", rows, cols) ;

	psf_binheader_readf (psf, "4", &type) ;

	if (type == MAT5_TYPE_SCHAR)
	{	psf_binheader_readf (psf, "4", &size) ;
		psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;
		if (size > SIGNED_SIZEOF (name) - 1)
		{	psf_log_printf (psf, "Error : Bad name length.\n") ;
			return SFE_MAT5_NO_BLOCK ;
			} ;

		psf_binheader_readf (psf, "bj", name, size, (8 - (size % 8)) % 8) ;
		name [size] = 0 ;
		}
	else if ((type & 0xFFFF) == MAT5_TYPE_SCHAR)
	{	size = type >> 16 ;
		if (size > 4)
		{	psf_log_printf (psf, "Error : Bad name length.\n") ;
			return SFE_MAT5_NO_BLOCK ;
			} ;

		psf_log_printf (psf, "    Type : %X\n", type) ;
		psf_binheader_readf (psf, "4", &name) ;
		name [size] = 0 ;
		}
	else
		return SFE_MAT5_NO_BLOCK ;

	psf_log_printf (psf, "    Name : %s\n", name) ;

	psf_binheader_readf (psf, "44", &type, &size) ;
	psf_log_printf (psf, "    Type : %X    Size : %d\n", type, size) ;

	/*++++++++++++++++++++++++++++++++++++++++++++++++++*/

	if (rows == 0 && cols == 0)
	{	psf_log_printf (psf, "*** Error : zero channel count.\n") ;
		return SFE_MAT5_ZERO_CHANNELS ;
		} ;

	psf->sf.channels	= rows ;
	psf->sf.frames		= cols ;

	psf->sf.format = psf->endian | SF_FORMAT_MAT5 ;

	switch (type)
	{	case MAT5_TYPE_DOUBLE :
				psf_log_printf (psf, "Data type : double\n") ;
				psf->sf.format |= SF_FORMAT_DOUBLE ;
				psf->bytewidth = 8 ;
				break ;

		case MAT5_TYPE_FLOAT :
				psf_log_printf (psf, "Data type : float\n") ;
				psf->sf.format |= SF_FORMAT_FLOAT ;
				psf->bytewidth = 4 ;
				break ;

		case MAT5_TYPE_INT32 :
				psf_log_printf (psf, "Data type : 32 bit PCM\n") ;
				psf->sf.format |= SF_FORMAT_PCM_32 ;
				psf->bytewidth = 4 ;
				break ;

		case MAT5_TYPE_INT16 :
				psf_log_printf (psf, "Data type : 16 bit PCM\n") ;
				psf->sf.format |= SF_FORMAT_PCM_16 ;
				psf->bytewidth = 2 ;
				break ;

		case MAT5_TYPE_UCHAR :
				psf_log_printf (psf, "Data type : unsigned 8 bit PCM\n") ;
				psf->sf.format |= SF_FORMAT_PCM_U8 ;
				psf->bytewidth = 1 ;
				break ;

		default :
				psf_log_printf (psf, "*** Error : Bad marker %08X\n", type) ;
				return SFE_UNIMPLEMENTED ;
		} ;

	psf->dataoffset = psf_ftell (psf) ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	return 0 ;
} /* mat5_read_header */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: dfdb6742-b2be-4be8-b390-d0c674e8bc8e
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


/* These required here because we write the header in this file. */

#define RIFF_MARKER	(MAKE_MARKER ('R', 'I', 'F', 'F'))
#define WAVE_MARKER	(MAKE_MARKER ('W', 'A', 'V', 'E'))
#define fmt_MARKER	(MAKE_MARKER ('f', 'm', 't', ' '))
#define fact_MARKER	(MAKE_MARKER ('f', 'a', 'c', 't'))
#define data_MARKER	(MAKE_MARKER ('d', 'a', 't', 'a'))

#define WAVE_FORMAT_MS_ADPCM	0x0002

typedef struct
{	int				channels, blocksize, samplesperblock, blocks, dataremaining ;
	int				blockcount ;
	sf_count_t			samplecount ;
	short			*samples ;
	unsigned char	*block ;
#if HAVE_FLEXIBLE_ARRAY
	unsigned short	dummydata [] ; /* ISO C99 struct flexible array. */
#else
	unsigned short	dummydata [1] ; /* This is a hack an might not work. */
#endif
} MSADPCM_PRIVATE ;

/*============================================================================================
** MS ADPCM static data and functions.
*/

static int AdaptationTable [] =
{	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230
} ;

/* TODO : The first 7 coef's are are always hardcode and must
   appear in the actual WAVE file.  They should be read in
   in case a sound program added extras to the list. */

static int AdaptCoeff1 [MSADPCM_ADAPT_COEFF_COUNT] =
{	256, 512, 0, 192, 240, 460, 392
} ;

static int AdaptCoeff2 [MSADPCM_ADAPT_COEFF_COUNT] =
{	0, -256, 0, 64, 0, -208, -232
} ;

/*============================================================================================
**	MS ADPCM Block Layout.
**	======================
**	Block is usually 256, 512 or 1024 bytes depending on sample rate.
**	For a mono file, the block is laid out as follows:
**		byte	purpose
**		0		block predictor [0..6]
**		1,2		initial idelta (positive)
**		3,4		sample 1
**		5,6		sample 0
**		7..n	packed bytecodes
**
**	For a stereo file, the block is laid out as follows:
**		byte	purpose
**		0		block predictor [0..6] for left channel
**		1		block predictor [0..6] for right channel
**		2,3		initial idelta (positive) for left channel
**		4,5		initial idelta (positive) for right channel
**		6,7		sample 1 for left channel
**		8,9		sample 1 for right channel
**		10,11	sample 0 for left channel
**		12,13	sample 0 for right channel
**		14..n	packed bytecodes
*/

/*============================================================================================
** Static functions.
*/

static	int	msadpcm_decode_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms) ;
static sf_count_t msadpcm_read_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms, short *ptr, int len) ;

static	int	msadpcm_encode_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms) ;
static sf_count_t msadpcm_write_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms, short *ptr, int len) ;

static sf_count_t	msadpcm_read_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	msadpcm_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	msadpcm_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	msadpcm_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t	msadpcm_write_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	msadpcm_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	msadpcm_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	msadpcm_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t msadpcm_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset) ;
static	int	msadpcm_close	(SF_PRIVATE *psf) ;

static	void	choose_predictor (unsigned int channels, short *data, int *bpred, int *idelta) ;

/*============================================================================================
** MS ADPCM Read Functions.
*/

int
wav_w64_msadpcm_init	(SF_PRIVATE *psf, int blockalign, int samplesperblock)
{	MSADPCM_PRIVATE	*pms ;
	unsigned int	pmssize ;
	int				count ;

	if (psf->mode == SFM_WRITE)
		samplesperblock = 2 + 2 * (blockalign - 7 * psf->sf.channels) / psf->sf.channels ;

	pmssize = sizeof (MSADPCM_PRIVATE) + blockalign + 3 * psf->sf.channels * samplesperblock ;

	if (! (psf->fdata = malloc (pmssize)))
		return SFE_MALLOC_FAILED ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;
	memset (pms, 0, pmssize) ;

	pms->samples	= (short *)pms->dummydata ;
	pms->block		= (unsigned char*) (pms->dummydata + psf->sf.channels * samplesperblock) ;

	pms->channels	= psf->sf.channels ;
	pms->blocksize	= blockalign ;
	pms->samplesperblock = samplesperblock ;

	if (psf->mode == SFM_READ)
	{	pms->dataremaining	 = psf->datalength ;

		if (psf->datalength % pms->blocksize)
			pms->blocks = psf->datalength / pms->blocksize + 1 ;
		else
			pms->blocks = psf->datalength / pms->blocksize ;

		count = 2 * (pms->blocksize - 6 * pms->channels) / pms->channels ;
		if (pms->samplesperblock != count)
			psf_log_printf (psf, "*** Warning : samplesperblock shoud be %d.\n", count) ;

		psf->sf.frames = (psf->datalength / pms->blocksize) * pms->samplesperblock ;

		psf_log_printf (psf, " bpred   idelta\n") ;

		msadpcm_decode_block (psf, pms) ;

		psf->read_short		= msadpcm_read_s ;
		psf->read_int		= msadpcm_read_i ;
		psf->read_float		= msadpcm_read_f ;
		psf->read_double	= msadpcm_read_d ;
		} ;

	if (psf->mode == SFM_WRITE)
	{	pms->samples = (short *)pms->dummydata ;

		pms->samplecount = 0 ;

		psf->write_short	= msadpcm_write_s ;
		psf->write_int		= msadpcm_write_i ;
		psf->write_float	= msadpcm_write_f ;
		psf->write_double	= msadpcm_write_d ;
		} ;

	psf->seek	= msadpcm_seek ;
	psf->close		= msadpcm_close ;

	return 0 ;
} /* wav_w64_msadpcm_init */

static int
msadpcm_decode_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms)
{	int		chan, k, blockindx, sampleindx ;
	short	bytecode, bpred [2], chan_idelta [2] ;

    int predict ;
    int current ;
    int idelta ;

	pms->blockcount ++ ;
	pms->samplecount = 0 ;

	if (pms->blockcount > pms->blocks)
	{	memset (pms->samples, 0, pms->samplesperblock * pms->channels) ;
		return 1 ;
		} ;

	if ((k = psf_fread (pms->block, 1, pms->blocksize, psf)) != pms->blocksize)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, pms->blocksize) ;

	/* Read and check the block header. */

	if (pms->channels == 1)
	{	bpred [0] = pms->block [0] ;

		if (bpred [0] >= 7)
			psf_log_printf (psf, "MS ADPCM synchronisation error (%d).\n", bpred [0]) ;

		chan_idelta [0] = pms->block [1] | (pms->block [2] << 8) ;
		chan_idelta [1] = 0 ;

		psf_log_printf (psf, "(%d) (%d)\n", bpred [0], chan_idelta [0]) ;

		pms->samples [1] = pms->block [3] | (pms->block [4] << 8) ;
		pms->samples [0] = pms->block [5] | (pms->block [6] << 8) ;
		blockindx = 7 ;
		}
	else
	{	bpred [0] = pms->block [0] ;
		bpred [1] = pms->block [1] ;

		if (bpred [0] >= 7 || bpred [1] >= 7)
			psf_log_printf (psf, "MS ADPCM synchronisation error (%d %d).\n", bpred [0], bpred [1]) ;

		chan_idelta [0] = pms->block [2] | (pms->block [3] << 8) ;
		chan_idelta [1] = pms->block [4] | (pms->block [5] << 8) ;

		psf_log_printf (psf, "(%d, %d) (%d, %d)\n", bpred [0], bpred [1], chan_idelta [0], chan_idelta [1]) ;

		pms->samples [2] = pms->block [6] | (pms->block [7] << 8) ;
		pms->samples [3] = pms->block [8] | (pms->block [9] << 8) ;

		pms->samples [0] = pms->block [10] | (pms->block [11] << 8) ;
		pms->samples [1] = pms->block [12] | (pms->block [13] << 8) ;

		blockindx = 14 ;
		} ;

	/*--------------------------------------------------------
	This was left over from a time when calculations were done
	as ints rather than shorts. Keep this around as a reminder
	in case I ever find a file which decodes incorrectly.

    if (chan_idelta [0] & 0x8000)
		chan_idelta [0] -= 0x10000 ;
    if (chan_idelta [1] & 0x8000)
		chan_idelta [1] -= 0x10000 ;
	--------------------------------------------------------*/

	/* Pull apart the packed 4 bit samples and store them in their
	** correct sample positions.
	*/

	sampleindx = 2 * pms->channels ;
	while (blockindx < pms->blocksize)
	{	bytecode = pms->block [blockindx++] ;
  		pms->samples [sampleindx++] = (bytecode >> 4) & 0x0F ;
		pms->samples [sampleindx++] = bytecode & 0x0F ;
		} ;

	/* Decode the encoded 4 bit samples. */

	for (k = 2 * pms->channels ; k < (pms->samplesperblock * pms->channels) ; k ++)
	{	chan = (pms->channels > 1) ? (k % 2) : 0 ;

		bytecode = pms->samples [k] & 0xF ;

		/* Compute next Adaptive Scale Factor (ASF) */
		idelta = chan_idelta [chan] ;
		chan_idelta [chan] = (AdaptationTable [bytecode] * idelta) >> 8 ;	/* => / 256 => FIXED_POINT_ADAPTATION_BASE == 256 */
		if (chan_idelta [chan] < 16)
			chan_idelta [chan] = 16 ;
		if (bytecode & 0x8)
			bytecode -= 0x10 ;

    	predict = ((pms->samples [k - pms->channels] * AdaptCoeff1 [bpred [chan]])
					+ (pms->samples [k - 2 * pms->channels] * AdaptCoeff2 [bpred [chan]])) >> 8 ; /* => / 256 => FIXED_POINT_COEFF_BASE == 256 */
		current = (bytecode * idelta) + predict ;

		if (current > 32767)
			current = 32767 ;
		else if (current < -32768)
			current = -32768 ;

		pms->samples [k] = current ;
		} ;

	return 1 ;
} /* msadpcm_decode_block */

static sf_count_t
msadpcm_read_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms, short *ptr, int len)
{	int	count, total = 0, indx = 0 ;

	while (indx < len)
	{	if (pms->blockcount >= pms->blocks && pms->samplecount >= pms->samplesperblock)
		{	memset (&(ptr [indx]), 0, (size_t) ((len - indx) * sizeof (short))) ;
			return total ;
			} ;

		if (pms->samplecount >= pms->samplesperblock)
			msadpcm_decode_block (psf, pms) ;

		count = (pms->samplesperblock - pms->samplecount) * pms->channels ;
		count = (len - indx > count) ? count : len - indx ;

		memcpy (&(ptr [indx]), &(pms->samples [pms->samplecount * pms->channels]), count * sizeof (short)) ;
		indx += count ;
		pms->samplecount += count / pms->channels ;
		total = indx ;
		} ;

	return total ;
} /* msadpcm_read_block */

static sf_count_t
msadpcm_read_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	MSADPCM_PRIVATE 	*pms ;
	int			readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	readcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = msadpcm_read_block (psf, pms, ptr, readcount) ;

		total += count ;
		len -= count ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* msadpcm_read_s */

static sf_count_t
msadpcm_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = msadpcm_read_block (psf, pms, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = sptr [k] << 16 ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;
	return total ;
} /* msadpcm_read_i */

static sf_count_t
msadpcm_read_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;
	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = msadpcm_read_block (psf, pms, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (float) (sptr [k]) ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;
	return total ;
} /* msadpcm_read_f */

static sf_count_t
msadpcm_read_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	short		*sptr ;
	int			k, bufferlen, readcount = 0, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = msadpcm_read_block (psf, pms, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (double) (sptr [k]) ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;
	return total ;
} /* msadpcm_read_d */

static sf_count_t
msadpcm_seek	(SF_PRIVATE *psf, int mode, sf_count_t offset)
{	MSADPCM_PRIVATE *pms ;
	int			newblock, newsample ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	if (psf->datalength < 0 || psf->dataoffset < 0)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	if (offset == 0)
	{	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		pms->blockcount = 0 ;
		msadpcm_decode_block (psf, pms) ;
		pms->samplecount = 0 ;
		return 0 ;
		} ;

	if (offset < 0 || offset > pms->blocks * pms->samplesperblock)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	newblock	= offset / pms->samplesperblock ;
	newsample	= offset % pms->samplesperblock ;

	if (mode == SFM_READ)
	{	psf_fseek (psf, psf->dataoffset + newblock * pms->blocksize, SEEK_SET) ;
		pms->blockcount = newblock ;
		msadpcm_decode_block (psf, pms) ;
		pms->samplecount = newsample ;
		}
	else
	{	/* What to do about write??? */
		psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	return newblock * pms->samplesperblock + newsample ;
} /* msadpcm_seek */

/*==========================================================================================
** MS ADPCM Write Functions.
*/

void
msadpcm_write_adapt_coeffs	(SF_PRIVATE *psf)
{	int k ;

	for (k = 0 ; k < MSADPCM_ADAPT_COEFF_COUNT ; k++)
		psf_binheader_writef (psf, "e22", AdaptCoeff1 [k], AdaptCoeff2 [k]) ;
} /* msadpcm_write_adapt_coeffs */

/*==========================================================================================
*/

static int
msadpcm_encode_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms)
{	unsigned int	blockindx ;
	unsigned char	byte ;
	int				chan, k, predict, bpred [2], idelta [2], errordelta, newsamp ;

	choose_predictor (pms->channels, pms->samples, bpred, idelta) ;

	/* Write the block header. */

	if (pms->channels == 1)
	{	pms->block [0]	= bpred [0] ;
		pms->block [1]	= idelta [0] & 0xFF ;
		pms->block [2]	= idelta [0] >> 8 ;
		pms->block [3]	= pms->samples [1] & 0xFF ;
		pms->block [4]	= pms->samples [1] >> 8 ;
		pms->block [5]	= pms->samples [0] & 0xFF ;
		pms->block [6]	= pms->samples [0] >> 8 ;

		blockindx = 7 ;
		byte = 0 ;

		/* Encode the samples as 4 bit. */

		for (k = 2 ; k < pms->samplesperblock ; k++)
		{	predict = (pms->samples [k-1] * AdaptCoeff1 [bpred [0]] + pms->samples [k-2] * AdaptCoeff2 [bpred [0]]) >> 8 ;
			errordelta = (pms->samples [k] - predict) / idelta [0] ;
			if (errordelta < -8)
				errordelta = -8 ;
			else if (errordelta > 7)
				errordelta = 7 ;
			newsamp = predict + (idelta [0] * errordelta) ;
			if (newsamp > 32767)
				newsamp = 32767 ;
			else if (newsamp < -32768)
				newsamp = -32768 ;
			if (errordelta < 0)
				errordelta += 0x10 ;

			byte = (byte << 4) | (errordelta & 0xF) ;
			if (k % 2)
			{	pms->block [blockindx++] = byte ;
				byte = 0 ;
				} ;

			idelta [0] = (idelta [0] * AdaptationTable [errordelta]) >> 8 ;
			if (idelta [0] < 16)
				idelta [0] = 16 ;
			pms->samples [k] = newsamp ;
			} ;
		}
	else
	{	/* Stereo file. */
		pms->block [0]	= bpred [0] ;
		pms->block [1]	= bpred [1] ;

		pms->block [2]	= idelta [0] & 0xFF ;
		pms->block [3]	= idelta [0] >> 8 ;
		pms->block [4]	= idelta [1] & 0xFF ;
		pms->block [5]	= idelta [1] >> 8 ;

		pms->block [6]	= pms->samples [2] & 0xFF ;
		pms->block [7]	= pms->samples [2] >> 8 ;
		pms->block [8]	= pms->samples [3] & 0xFF ;
		pms->block [9]	= pms->samples [3] >> 8 ;

		pms->block [10]	= pms->samples [0] & 0xFF ;
		pms->block [11]	= pms->samples [0] >> 8 ;
		pms->block [12]	= pms->samples [1] & 0xFF ;
		pms->block [13]	= pms->samples [1] >> 8 ;

		blockindx = 14 ;
		byte = 0 ;
		chan = 1 ;

		for (k = 4 ; k < 2 * pms->samplesperblock ; k++)
		{	chan = k & 1 ;

			predict = (pms->samples [k-2] * AdaptCoeff1 [bpred [chan]] + pms->samples [k-4] * AdaptCoeff2 [bpred [chan]]) >> 8 ;
			errordelta = (pms->samples [k] - predict) / idelta [chan] ;


			if (errordelta < -8)
				errordelta = -8 ;
			else if (errordelta > 7)
				errordelta = 7 ;
			newsamp = predict + (idelta [chan] * errordelta) ;
			if (newsamp > 32767)
				newsamp = 32767 ;
			else if (newsamp < -32768)
				newsamp = -32768 ;
			if (errordelta < 0)
				errordelta += 0x10 ;

			byte = (byte << 4) | (errordelta & 0xF) ;

			if (chan)
			{	pms->block [blockindx++] = byte ;
				byte = 0 ;
				} ;

			idelta [chan] = (idelta [chan] * AdaptationTable [errordelta]) >> 8 ;
			if (idelta [chan] < 16)
				idelta [chan] = 16 ;
			pms->samples [k] = newsamp ;
			} ;
		} ;

	/* Write the block to disk. */

	if ((k = psf_fwrite (pms->block, 1, pms->blocksize, psf)) != pms->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, pms->blocksize) ;

	memset (pms->samples, 0, pms->samplesperblock * sizeof (short)) ;

	pms->blockcount ++ ;
	pms->samplecount = 0 ;

	return 1 ;
} /* msadpcm_encode_block */

static sf_count_t
msadpcm_write_block	(SF_PRIVATE *psf, MSADPCM_PRIVATE *pms, short *ptr, int len)
{	int		count, total = 0, indx = 0 ;

	while (indx < len)
	{	count = (pms->samplesperblock - pms->samplecount) * pms->channels ;

		if (count > len - indx)
			count = len - indx ;

		memcpy (&(pms->samples [pms->samplecount * pms->channels]), &(ptr [total]), count * sizeof (short)) ;
		indx += count ;
		pms->samplecount += count / pms->channels ;
		total = indx ;

		if (pms->samplecount >= pms->samplesperblock)
			msadpcm_encode_block (psf, pms) ;
		} ;

	return total ;
} /* msadpcm_write_block */

static sf_count_t
msadpcm_write_s	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	int			writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = msadpcm_write_block (psf, pms, ptr, writecount) ;

		total += count ;
		len -= count ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* msadpcm_write_s */

static sf_count_t
msadpcm_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = ptr [total + k] >> 16 ;
		count = msadpcm_write_block (psf, pms, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;
	return total ;
} /* msadpcm_write_i */

static sf_count_t
msadpcm_write_f	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = msadpcm_write_block (psf, pms, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;
	return total ;
} /* msadpcm_write_f */

static sf_count_t
msadpcm_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	MSADPCM_PRIVATE *pms ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	if (! psf->fdata)
		return 0 ;
	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrint (normfact * ptr [total + k]) ;
		count = msadpcm_write_block (psf, pms, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;
	return total ;
} /* msadpcm_write_d */

/*========================================================================================
*/

static int
msadpcm_close	(SF_PRIVATE *psf)
{	MSADPCM_PRIVATE *pms ;

	if (! psf->fdata)
		return 0 ;

	pms = (MSADPCM_PRIVATE*) psf->fdata ;

	if (psf->mode == SFM_WRITE)
	{	/*  Now we know static int for certain the length of the file we can
		**  re-write the header.
		*/

		if (pms->samplecount && pms->samplecount < pms->samplesperblock)
			msadpcm_encode_block (psf, pms) ;

		if (psf->write_header)
			psf->write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* msadpcm_close */

/*========================================================================================
** Static functions.
*/

/*----------------------------------------------------------------------------------------
**	Choosing the block predictor.
**	Each block requires a predictor and an idelta for each channel.
**	The predictor is in the range [0..6] which is an indx into the	two AdaptCoeff tables.
**	The predictor is chosen by trying all of the possible predictors on a small set of
**	samples at the beginning of the block. The predictor with the smallest average
**	abs (idelta) is chosen as the best predictor for this block.
**	The value of idelta is chosen to to give a 4 bit code value of +/- 4 (approx. half the
**	max. code value). If the average abs (idelta) is zero, the sixth predictor is chosen.
**	If the value of idelta is less then 16 it is set to 16.
**
**	Microsoft uses an IDELTA_COUNT (number of sample pairs used to choose best predictor)
**	value of 3. The best possible results would be obtained by using all the samples to
**	choose the predictor.
*/

#define		IDELTA_COUNT	3

static	void
choose_predictor (unsigned int channels, short *data, int *block_pred, int *idelta)
{	unsigned int	chan, k, bpred, idelta_sum, best_bpred, best_idelta ;

	for (chan = 0 ; chan < channels ; chan++)
	{	best_bpred = best_idelta = 0 ;

		for (bpred = 0 ; bpred < 7 ; bpred++)
		{	idelta_sum = 0 ;
			for (k = 2 ; k < 2 + IDELTA_COUNT ; k++)
				idelta_sum += abs (data [k * channels] - ((data [(k - 1) * channels] * AdaptCoeff1 [bpred] + data [(k - 2) * channels] * AdaptCoeff2 [bpred]) >> 8)) ;
			idelta_sum /= (4 * IDELTA_COUNT) ;

			if (bpred == 0 || idelta_sum < best_idelta)
			{	best_bpred = bpred ;
				best_idelta = idelta_sum ;
				} ;

			if (! idelta_sum)
			{	best_bpred = bpred ;
				best_idelta = 16 ;
				break ;
				} ;

			} ; /* for bpred ... */
		if (best_idelta < 16)
			best_idelta = 16 ;

		block_pred [chan]	= best_bpred ;
		idelta [chan]		= best_idelta ;
		} ;

	return ;
} /* choose_predictor */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: a98908a3-5305-4935-872b-77d6a86c330f
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
**	Some of the information used to read NIST files was gleaned from
**	reading the code of Bill Schottstaedt's sndlib library
**		ftp://ccrma-ftp.stanford.edu/pub/Lisp/sndlib.tar.gz
**	However, no code from that package was used.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
*/

#define	NIST_HEADER_LENGTH	1024

/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int	nist_close	(SF_PRIVATE *psf) ;
static	int nist_write_header	(SF_PRIVATE *psf, int calc_length) ;
static	int nist_read_header	(SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
*/

int
nist_open	(SF_PRIVATE *psf)
{	int subformat, error ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = nist_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_NIST)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;
		if (psf->endian == 0 || psf->endian == SF_ENDIAN_CPU)
			psf->endian = (CPU_IS_BIG_ENDIAN) ? SF_ENDIAN_BIG : SF_ENDIAN_LITTLE ;

		psf->blockwidth = psf->bytewidth * psf->sf.channels ;

		if ((error = nist_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = nist_write_header ;
		} ;

	psf->close = nist_close ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_ULAW :
				error = ulaw_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :
				error = alaw_init (psf) ;
				break ;

		default : error = SFE_UNIMPLEMENTED ;
				break ;
		} ;

	return error ;
} /* nist_open */

/*------------------------------------------------------------------------------
*/

static char bad_header [] =
{	'N', 'I', 'S', 'T', '_', '1', 'A', 0x0d, 0x0a,
	' ', ' ', ' ', '1', '0', '2', '4', 0x0d, 0x0a,
	0
} ;

	static int
nist_read_header (SF_PRIVATE *psf)
{	char	*psf_header ;
	int		bitwidth = 0, bytes = 0, count, encoding ;
	char 	str [64], *cptr ;
	long	samples ;

	psf->sf.format = SF_FORMAT_NIST ;

	psf_header = (char*) psf->buffer ;

	if (sizeof (psf->header) <= NIST_HEADER_LENGTH)
		return SFE_INTERNAL ;

	/* Go to start of file and read in the whole header. */
	psf_binheader_readf (psf, "pb", 0, psf_header, NIST_HEADER_LENGTH) ;

	/* Header is a string, so make sure it is null terminated. */
	psf_header [NIST_HEADER_LENGTH] = 0 ;

	/* Now trim the header after the end marker. */
	if ((cptr = strstr (psf_header, "end_head")))
	{	cptr += strlen ("end_head") + 1 ;
		cptr [0] = 0 ;
		} ;

	if (strstr (psf_header, bad_header) == psf_header)
		return SFE_NIST_CRLF_CONVERISON ;

	/* Make sure its a NIST file. */
	if (strstr (psf_header, "NIST_1A\n   1024\n") != psf_header)
	{	psf_log_printf (psf, "Not a NIST file.\n") ;
		return SFE_NIST_BAD_HEADER ;
		} ;

	/* Determine sample encoding, start by assuming PCM. */
	encoding = SF_FORMAT_PCM_U8 ;
	if ((cptr = strstr (psf_header, "sample_coding -s")))
	{	sscanf (cptr, "sample_coding -s%d %63s", &count, str) ;

		if (strcmp (str, "pcm") == 0)
			encoding = SF_FORMAT_PCM_U8 ;
		else if (strcmp (str, "alaw") == 0)
			encoding = SF_FORMAT_ALAW ;
		else if ((strcmp (str, "ulaw") == 0) || (strcmp (str, "mu-law") == 0))
			encoding = SF_FORMAT_ULAW ;
		else
		{	psf_log_printf (psf, "*** Unknown encoding : %s\n", str) ;
			encoding = 0 ;
			} ;
		} ;

	if ((cptr = strstr (psf_header, "channel_count -i ")))
		sscanf (cptr, "channel_count -i %d", &(psf->sf.channels)) ;

	if ((cptr = strstr (psf_header, "sample_rate -i ")))
		sscanf (cptr, "sample_rate -i %d", &(psf->sf.samplerate)) ;

	if ((cptr = strstr (psf_header, "sample_count -i ")))
	{	sscanf (psf_header, "sample_count -i %ld", &samples) ;
		psf->sf.frames = samples ;
		} ;

	if ((cptr = strstr (psf_header, "sample_n_bytes -i ")))
		sscanf (cptr, "sample_n_bytes -i %d", &(psf->bytewidth)) ;

	/* Default endian-ness (for 8 bit, u-law, A-law. */
	psf->endian = (CPU_IS_BIG_ENDIAN) ? SF_ENDIAN_BIG : SF_ENDIAN_LITTLE ;

	/* This is where we figure out endian-ness. */
	if ((cptr = strstr (psf_header, "sample_byte_format -s")))
	{	sscanf (cptr, "sample_byte_format -s%d %8s", &bytes, str) ;
		if (bytes > 1)
		{	if (psf->bytewidth == 0)
				psf->bytewidth = bytes ;
			else if (psf->bytewidth != bytes)
			{	psf_log_printf (psf, "psf->bytewidth (%d) != bytes (%d)\n", psf->bytewidth, bytes) ;
				return SFE_NIST_BAD_ENCODING ;
				} ;

			if (strstr (str, "01") == str)
				psf->endian = SF_ENDIAN_LITTLE ;
			else if (strstr (str, "10"))
				psf->endian = SF_ENDIAN_BIG ;
			else
			{	psf_log_printf (psf, "Weird endian-ness : %s\n", str) ;
				return SFE_NIST_BAD_ENCODING ;
				} ;
			} ;

		psf->sf.format |= psf->endian ;
		} ;

	if ((cptr = strstr (psf_header, "sample_sig_bits -i ")))
		sscanf (cptr, "sample_sig_bits -i %d", &bitwidth) ;

	if (strstr (psf_header, "channels_interleaved -s5 FALSE"))
	{	psf_log_printf (psf, "Non-interleaved data unsupported.\n", str) ;
		return SFE_NIST_BAD_ENCODING ;
		} ;

 	psf->dataoffset = NIST_HEADER_LENGTH ;
	psf->blockwidth = psf->sf.channels * psf->bytewidth ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	psf->close = nist_close ;

	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	if (encoding == SF_FORMAT_PCM_U8)
	{	switch (psf->bytewidth)
		{	case 1 :
					psf->sf.format |= SF_FORMAT_PCM_S8 ;
					break ;

			case 2 :
					psf->sf.format |= SF_FORMAT_PCM_16 ;
					break ;

			case 3 :
					psf->sf.format |= SF_FORMAT_PCM_24 ;
					break ;

			case 4 :
					psf->sf.format |= SF_FORMAT_PCM_32 ;
					break ;

			default : break ;
			} ;
		}
	else if (encoding != 0)
		psf->sf.format |= encoding ;
	else
		return SFE_UNIMPLEMENTED ;

	return 0 ;
} /* nist_read_header */

static int
nist_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		nist_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* nist_close */

/*=========================================================================
*/

static int
nist_write_header (SF_PRIVATE *psf, int calc_length)
{	const char	*end_str ;
	long		samples ;
	sf_count_t	current ;

	current = psf_ftell (psf) ;

	/* Prevent compiler warning. */
	calc_length = calc_length ;

	if (psf->endian == SF_ENDIAN_BIG)
		end_str = "10" ;
	else if (psf->endian == SF_ENDIAN_LITTLE)
		end_str = "01" ;
	else
		end_str = "error" ;

	/* Clear the whole header. */
	memset (psf->header, 0, sizeof (psf->header)) ;
	psf->headindex = 0 ;

	psf_fseek (psf, 0, SEEK_SET) ;

	psf_asciiheader_printf (psf, "NIST_1A\n   1024\n") ;
	psf_asciiheader_printf (psf, "channel_count -i %d\n", psf->sf.channels) ;
	psf_asciiheader_printf (psf, "sample_rate -i %d\n", psf->sf.samplerate) ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
				psf_asciiheader_printf (psf, "sample_coding -s3 pcm\n") ;
				psf_asciiheader_printf (psf, "sample_n_bytes -i 1\n"
											"sample_sig_bits -i 8\n") ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				psf_asciiheader_printf (psf, "sample_n_bytes -i %d\n", psf->bytewidth) ;
				psf_asciiheader_printf (psf, "sample_sig_bits -i %d\n", psf->bytewidth * 8) ;
				psf_asciiheader_printf (psf, "sample_coding -s3 pcm\n"
								"sample_byte_format -s%d %s\n", psf->bytewidth, end_str) ;
				break ;

		case SF_FORMAT_ALAW :
				psf_asciiheader_printf (psf, "sample_coding -s4 alaw\n") ;
				psf_asciiheader_printf (psf, "sample_n_bytes -s1 1\n") ;
				break ;

		case SF_FORMAT_ULAW :
				psf_asciiheader_printf (psf, "sample_coding -s4 ulaw\n") ;
				psf_asciiheader_printf (psf, "sample_n_bytes -s1 1\n") ;
				break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	psf->dataoffset = NIST_HEADER_LENGTH ;

	/* Fix this */
	samples = psf->sf.frames ;
	psf_asciiheader_printf (psf, "sample_count -i %ld\n", samples) ;
	psf_asciiheader_printf (psf, "end_head\n") ;

	/* Zero fill to dataoffset. */
	psf_binheader_writef (psf, "z", (size_t) (NIST_HEADER_LENGTH - psf->headindex)) ;

	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* nist_write_header */


/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: b45ed85d-9e22-4ad9-b78c-4b58b67152a8
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
ogg_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* ogg_open */

#else

#define	SFE_OGG_NOT_OGG	666

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define ALAW_MARKER		MAKE_MARKER ('A', 'L', 'a', 'w')
#define SOUN_MARKER		MAKE_MARKER ('S', 'o', 'u', 'n')
#define DFIL_MARKER		MAKE_MARKER ('d', 'F', 'i', 'l')

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int	ogg_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
ogg_open (SF_PRIVATE *psf)
{	int	subformat, error = 0 ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		return SFE_UNIMPLEMENTED ;

	if ((error = ogg_read_header (psf)))
		return error ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_OGG)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	return error ;
} /* ogg_open */

/*------------------------------------------------------------------------------
*/

static int
ogg_read_header (SF_PRIVATE *psf)
{	int marker ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "pm", 0, &marker) ;
	if (marker != ALAW_MARKER)
		return SFE_OGG_NOT_OGG ;

	psf_binheader_readf (psf, "m", &marker) ;
	if (marker != SOUN_MARKER)
		return SFE_OGG_NOT_OGG ;

	psf_binheader_readf (psf, "m", &marker) ;
	if (marker != DFIL_MARKER)
		return SFE_OGG_NOT_OGG ;

	psf_log_printf (psf, "Read only : Psion Palmtop Alaw (.wve)\n"
			"  Sample Rate : 8000\n"
			"  Channels    : 1\n"
			"  Encoding    : A-law\n") ;

	psf->dataoffset = 0x20 ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	psf->sf.format		= SF_FORMAT_OGG | SF_FORMAT_ALAW ;
	psf->sf.samplerate	= 8000 ;
	psf->sf.frames		= psf->datalength ;
	psf->sf.channels	= 1 ;

	return alaw_init (psf) ;
} /* ogg_read_header */

/*------------------------------------------------------------------------------
*/

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 9ff1fe9c-629e-4e9c-9ef5-3d0eb1e427a0
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>


/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define FAP_MARKER	(MAKE_MARKER ('f', 'a', 'p', ' '))
#define PAF_MARKER	(MAKE_MARKER (' ', 'p', 'a', 'f'))

/*------------------------------------------------------------------------------
** Other defines.
*/

#define	PAF_HEADER_LENGTH 			2048

#define	PAF24_SAMPLES_PER_BLOCK		10
#define	PAF24_BLOCK_SIZE			32

/*------------------------------------------------------------------------------
** Typedefs.
*/

typedef	struct
{	int	version ;
	int	endianness ;
    int	samplerate ;
    int	format ;
	int	channels ;
	int	source ;
} PAF_FMT ;

typedef struct
{	int				max_blocks, channels, samplesperblock, blocksize ;
	int				read_block, write_block, read_count, write_count ;
	sf_count_t		sample_count ;
	int				*samples ;
	unsigned char	*block ;
#if HAVE_FLEXIBLE_ARRAY
	int				data [] ; /* ISO C99 struct flexible array. */
#else
	int				data [1] ; /* This is a hack and may not work. */
#endif
} PAF24_PRIVATE ;

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int paf24_init (SF_PRIVATE *psf) ;

static int	paf_read_header	(SF_PRIVATE *psf) ;
static int	paf_write_header (SF_PRIVATE *psf, int calc_length) ;

static sf_count_t paf24_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t paf24_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t paf24_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t paf24_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t paf24_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t paf24_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t paf24_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t paf24_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t paf24_seek (SF_PRIVATE *psf, int mode, sf_count_t offset) ;

enum
{	PAF_PCM_16 = 0,
	PAF_PCM_24 = 1,
	PAF_PCM_S8 = 2
} ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
paf_open	(SF_PRIVATE *psf)
{	int		subformat, error, endian ;

 	psf->dataoffset = PAF_HEADER_LENGTH ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = paf_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_PAF)
			return	SFE_BAD_OPEN_FORMAT ;

		endian = psf->sf.format & SF_FORMAT_ENDMASK ;

		/* PAF is by default big endian. */
		psf->endian = SF_ENDIAN_BIG ;

		if (endian == SF_ENDIAN_LITTLE || (CPU_IS_LITTLE_ENDIAN && (endian == SF_ENDIAN_CPU)))
			psf->endian = SF_ENDIAN_LITTLE ;

		if ((error = paf_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = paf_write_header ;
		} ;

	switch (subformat)
	{	case SF_FORMAT_PCM_S8 :
					psf->bytewidth = 1 ;
					error = pcm_init (psf) ;
					break ;

		case SF_FORMAT_PCM_16 :
					psf->bytewidth = 2 ;
					error = pcm_init (psf) ;
					break ;

		case SF_FORMAT_PCM_24 :
					/* No bytewidth because of whacky 24 bit encoding. */
					error = paf24_init (psf) ;
					break ;

		default : return SFE_PAF_UNKNOWN_FORMAT ;
		} ;

	return error ;
} /* paf_open */

/*------------------------------------------------------------------------------
*/

static int
paf_read_header	(SF_PRIVATE *psf)
{	PAF_FMT		paf_fmt ;
	int			marker ;

	psf_binheader_readf (psf, "pm", 0, &marker) ;

	psf_log_printf (psf, "Signature   : '%M'\n", marker) ;

	if (marker == PAF_MARKER)
	{	psf_binheader_readf (psf, "E444444", &(paf_fmt.version), &(paf_fmt.endianness),
			&(paf_fmt.samplerate), &(paf_fmt.format), &(paf_fmt.channels), &(paf_fmt.source)) ;
		}
	else if (marker == FAP_MARKER)
	{	psf_binheader_readf (psf, "e444444", &(paf_fmt.version), &(paf_fmt.endianness),
			&(paf_fmt.samplerate), &(paf_fmt.format), &(paf_fmt.channels), &(paf_fmt.source)) ;
		}
	else
		return SFE_PAF_NO_MARKER ;

	psf_log_printf (psf, "Version     : %d\n", paf_fmt.version) ;

	if (paf_fmt.version != 0)
	{	psf_log_printf (psf, "*** Bad version number. should be zero.\n") ;
		return SFE_PAF_VERSION ;
		} ;

	psf_log_printf (psf, "Sample Rate : %d\n", paf_fmt.samplerate) ;
	psf_log_printf (psf, "Channels    : %d\n", paf_fmt.channels) ;

	psf_log_printf (psf, "Endianness  : %d => ", paf_fmt.endianness) ;
	if (paf_fmt.endianness)
	{	psf_log_printf (psf, "Little\n", paf_fmt.endianness) ;
		psf->endian = SF_ENDIAN_LITTLE ;
		}
	else
	{	psf_log_printf (psf, "Big\n", paf_fmt.endianness) ;
		psf->endian = SF_ENDIAN_BIG ;
		} ;

	if (psf->filelength < PAF_HEADER_LENGTH)
		return SFE_PAF_SHORT_HEADER ;

	psf->datalength = psf->filelength - psf->dataoffset ;

	psf_binheader_readf (psf, "p", (int) psf->dataoffset) ;

	psf->sf.samplerate	= paf_fmt.samplerate ;
	psf->sf.channels 	= paf_fmt.channels ;

	/* Only fill in type major. */
	psf->sf.format = SF_FORMAT_PAF ;

	psf_log_printf (psf, "Format      : %d => ", paf_fmt.format) ;

	/* PAF is by default big endian. */
	psf->sf.format |= paf_fmt.endianness ? SF_ENDIAN_LITTLE : SF_ENDIAN_BIG ;

	switch (paf_fmt.format)
	{	case PAF_PCM_S8 :
					psf_log_printf (psf, "8 bit linear PCM\n") ;
					psf->bytewidth = 1 ;

					psf->sf.format |= SF_FORMAT_PCM_S8 ;

					psf->blockwidth = psf->bytewidth * psf->sf.channels ;
					psf->sf.frames = psf->datalength / psf->blockwidth ;
					break ;

		case PAF_PCM_16 :
					psf_log_printf (psf, "16 bit linear PCM\n") ;
					psf->bytewidth = 2 ;

					psf->sf.format |= SF_FORMAT_PCM_16 ;

					psf->blockwidth = psf->bytewidth * psf->sf.channels ;
					psf->sf.frames = psf->datalength / psf->blockwidth ;
					break ;

		case PAF_PCM_24 :
					psf_log_printf (psf, "24 bit linear PCM\n") ;
					psf->bytewidth = 3 ;

					psf->sf.format |= SF_FORMAT_PCM_24 ;

					psf->blockwidth = 0 ;
					psf->sf.frames = PAF24_SAMPLES_PER_BLOCK * psf->datalength /
											(PAF24_BLOCK_SIZE * psf->sf.channels) ;
					break ;

		default :	psf_log_printf (psf, "Unknown\n") ;
					return SFE_PAF_UNKNOWN_FORMAT ;
					break ;
		} ;

	psf_log_printf (psf, "Source      : %d => ", paf_fmt.source) ;

	switch (paf_fmt.source)
	{	case 1 : psf_log_printf (psf, "Analog Recording\n") ;
					break ;
		case 2 : psf_log_printf (psf, "Digital Transfer\n") ;
					break ;
		case 3 : psf_log_printf (psf, "Multi-track Mixdown\n") ;
					break ;
		case 5 : psf_log_printf (psf, "Audio Resulting From DSP Processing\n") ;
					break ;
		default : psf_log_printf (psf, "Unknown\n") ;
					break ;
		} ;

	return 0 ;
} /* paf_read_header */

static int
paf_write_header (SF_PRIVATE *psf, int calc_length)
{	int			paf_format ;

	/* PAF header already written so no need to re-write. */
	if (psf_ftell (psf) >= PAF_HEADER_LENGTH)
		return 0 ;

	psf->dataoffset = PAF_HEADER_LENGTH ;

	psf->dataoffset = PAF_HEADER_LENGTH ;

	/* Prevent compiler warning. */
	calc_length = calc_length ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
					paf_format = PAF_PCM_S8 ;
					break ;

		case SF_FORMAT_PCM_16 :
					paf_format = PAF_PCM_16 ;
					break ;

		case SF_FORMAT_PCM_24 :
					paf_format = PAF_PCM_24 ;
					break ;

		default : return SFE_PAF_UNKNOWN_FORMAT ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	if (psf->endian == SF_ENDIAN_BIG)
	{	/* Marker, version, endianness, samplerate */
		psf_binheader_writef (psf, "Em444", PAF_MARKER, 0, 0, psf->sf.samplerate) ;
		/* format, channels, source */
		psf_binheader_writef (psf, "E444", paf_format, psf->sf.channels, 0) ;
		}
	else if (psf->endian == SF_ENDIAN_LITTLE)
	{	/* Marker, version, endianness, samplerate */
		psf_binheader_writef (psf, "em444", FAP_MARKER, 0, 1, psf->sf.samplerate) ;
		/* format, channels, source */
		psf_binheader_writef (psf, "e444", paf_format, psf->sf.channels, 0) ;
		} ;

	/* Zero fill to dataoffset. */
	psf_binheader_writef (psf, "z", (size_t) (psf->dataoffset - psf->headindex)) ;

	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	return psf->error ;
} /* paf_write_header */

/*===============================================================================
**	24 bit PAF files have a really weird encoding.
**  For a mono file, 10 samples (each being 3 bytes) are packed into a 32 byte
**	block. The 8 ints in this 32 byte block are then endian swapped (as ints)
**	if necessary before being written to disk.
**  For a stereo file, blocks of 10 samples from the same channel are encoded
**  into 32 bytes as for the mono case. The 32 byte blocks are then interleaved
**	on disk.
**	Reading has to reverse the above process :-).
**	Weird!!!
**
**	The code below attempts to gain efficiency while maintaining readability.
*/

static int paf24_read_block (SF_PRIVATE *psf, PAF24_PRIVATE *ppaf24) ;
static int paf24_write_block (SF_PRIVATE *psf, PAF24_PRIVATE *ppaf24) ;
static int paf24_close (SF_PRIVATE *psf) ;


static int
paf24_init (SF_PRIVATE *psf)
{	PAF24_PRIVATE	*ppaf24 ;
	int	paf24size, max_blocks ;

	paf24size = sizeof (PAF24_PRIVATE) + psf->sf.channels *
					(PAF24_BLOCK_SIZE + PAF24_SAMPLES_PER_BLOCK * sizeof (int)) ;

	/*
	**	Not exatly sure why this needs to be here but the tests
	**	fail without it.
	*/
	psf->last_op = 0 ;

	if (! (psf->fdata = malloc (paf24size)))
		return SFE_MALLOC_FAILED ;

	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;
	memset (ppaf24, 0, paf24size) ;

	ppaf24->channels	= psf->sf.channels ;
	ppaf24->samples		= ppaf24->data ;
	ppaf24->block		= (unsigned char*) (ppaf24->data + PAF24_SAMPLES_PER_BLOCK * ppaf24->channels) ;

	ppaf24->blocksize = PAF24_BLOCK_SIZE * ppaf24->channels ;
	ppaf24->samplesperblock = PAF24_SAMPLES_PER_BLOCK ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	paf24_read_block (psf, ppaf24) ;	/* Read first block. */

		psf->read_short		= paf24_read_s ;
		psf->read_int		= paf24_read_i ;
		psf->read_float		= paf24_read_f ;
		psf->read_double	= paf24_read_d ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	psf->write_short	= paf24_write_s ;
		psf->write_int		= paf24_write_i ;
		psf->write_float	= paf24_write_f ;
		psf->write_double	= paf24_write_d ;
		} ;

	psf->seek	= paf24_seek ;
	psf->close	= paf24_close ;

	psf->filelength = psf_get_filelen (psf) ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	if (psf->datalength % PAF24_BLOCK_SIZE)
	{	if (psf->mode == SFM_READ)
			psf_log_printf (psf, "*** Warning : file seems to be truncated.\n") ;
		max_blocks = psf->datalength / ppaf24->blocksize + 1 ;
		}
	else
		max_blocks = psf->datalength / ppaf24->blocksize ;

	ppaf24->read_block = 0 ;
	if (psf->mode == SFM_RDWR)
		ppaf24->write_block = max_blocks ;
	else
		ppaf24->write_block = 0 ;

	psf->sf.frames = ppaf24->samplesperblock * max_blocks ;
	ppaf24->sample_count = psf->sf.frames ;

	return 0 ;
} /* paf24_init */

static sf_count_t
paf24_seek (SF_PRIVATE *psf, int mode, sf_count_t offset)
{	PAF24_PRIVATE	*ppaf24 ;
	int				newblock, newsample ;

	if (psf->fdata == NULL)
	{	psf->error = SFE_INTERNAL ;
		return SF_SEEK_ERROR ;
		} ;

	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	if (mode == SFM_READ && ppaf24->write_count > 0)
		paf24_write_block (psf, ppaf24) ;

	newblock	= offset / ppaf24->samplesperblock ;
	newsample	= offset % ppaf24->samplesperblock ;

	switch (mode)
	{	case SFM_READ :
				if (offset > ppaf24->read_block * ppaf24->samplesperblock + ppaf24->read_count)
				{	psf->error = SFE_BAD_SEEK ;
					return SF_SEEK_ERROR ;
					} ;

				if (psf->last_op == SFM_WRITE && ppaf24->write_count)
					paf24_write_block (psf, ppaf24) ;

				psf_fseek (psf, psf->dataoffset + newblock * ppaf24->blocksize, SEEK_SET) ;
				ppaf24->read_block = newblock ;
				paf24_read_block (psf, ppaf24) ;
				ppaf24->read_count = newsample ;
				break ;

		case SFM_WRITE :
				if (offset > ppaf24->sample_count)
				{	psf->error = SFE_BAD_SEEK ;
					return SF_SEEK_ERROR ;
					} ;

				if (psf->last_op == SFM_WRITE && ppaf24->write_count)
					paf24_write_block (psf, ppaf24) ;

				psf_fseek (psf, psf->dataoffset + newblock * ppaf24->blocksize, SEEK_SET) ;
				ppaf24->write_block = newblock ;
				paf24_read_block (psf, ppaf24) ;
				ppaf24->write_count = newsample ;
				break ;

		default :
				psf->error = SFE_BAD_SEEK ;
				return SF_SEEK_ERROR ;
		} ;

	return newblock * ppaf24->samplesperblock + newsample ;
} /* paf24_seek */

static int
paf24_close (SF_PRIVATE *psf)
{	PAF24_PRIVATE *ppaf24 ;

	if (psf->fdata == NULL)
		return 0 ;

	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (ppaf24->write_count > 0)
			paf24_write_block (psf, ppaf24) ;
		} ;

	return 0 ;
} /* paf24_close */

/*---------------------------------------------------------------------------
*/
static int
paf24_read_block (SF_PRIVATE *psf, PAF24_PRIVATE *ppaf24)
{	int				k, channel ;
	unsigned char	*cptr ;

	ppaf24->read_block ++ ;
	ppaf24->read_count = 0 ;

	if (ppaf24->read_block * ppaf24->samplesperblock > ppaf24->sample_count)
	{	memset (ppaf24->samples, 0, ppaf24->samplesperblock * ppaf24->channels) ;
		return 1 ;
		} ;

	/* Read the block. */
	if ((k = psf_fread (ppaf24->block, 1, ppaf24->blocksize, psf)) != ppaf24->blocksize)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, ppaf24->blocksize) ;


	if (CPU_IS_LITTLE_ENDIAN)
	{	/* Do endian swapping if necessary. */
		if (psf->endian == SF_ENDIAN_BIG)
			endswap_int_array 	(ppaf24->data, 8 * ppaf24->channels) ;

		/* Unpack block. */
		for (k = 0 ; k < PAF24_SAMPLES_PER_BLOCK * ppaf24->channels ; k++)
		{	channel = k % ppaf24->channels ;
			cptr = ppaf24->block + PAF24_BLOCK_SIZE * channel + 3 * (k / ppaf24->channels) ;
			ppaf24->samples [k] = (cptr [0] << 8) | (cptr [1] << 16) | (cptr [2] << 24) ;
			} ;
		}
	else
	{	/* Do endian swapping if necessary. */
		if (psf->endian == SF_ENDIAN_BIG)
			endswap_int_array 	(ppaf24->data, 8 * ppaf24->channels) ;

		/* Unpack block. */
		for (k = 0 ; k < PAF24_SAMPLES_PER_BLOCK * ppaf24->channels ; k++)
		{	channel = k % ppaf24->channels ;
			cptr = ppaf24->block + PAF24_BLOCK_SIZE * channel + 3 * (k / ppaf24->channels) ;
			ppaf24->samples [k] = (cptr [0] << 8) | (cptr [1] << 16) | (cptr [2] << 24) ;
			} ;
		} ;

	return 1 ;
} /* paf24_read_block */

static int
paf24_read (SF_PRIVATE *psf, PAF24_PRIVATE *ppaf24, int *ptr, int len)
{	int	count, total = 0 ;

	while (total < len)
	{	if (ppaf24->read_block * ppaf24->samplesperblock >= ppaf24->sample_count)
		{	memset (&(ptr [total]), 0, (len - total) * sizeof (int)) ;
			return total ;
			} ;

		if (ppaf24->read_count >= ppaf24->samplesperblock)
			paf24_read_block (psf, ppaf24) ;

		count = (ppaf24->samplesperblock - ppaf24->read_count) * ppaf24->channels ;
		count = (len - total > count) ? count : len - total ;

		memcpy (&(ptr [total]), &(ppaf24->samples [ppaf24->read_count * ppaf24->channels]), count * sizeof (int)) ;
		total += count ;
		ppaf24->read_count += count / ppaf24->channels ;
		} ;

	return total ;
} /* paf24_read */

static sf_count_t
paf24_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				*iptr ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = paf24_read (psf, ppaf24, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = iptr [k] >> 16 ;
		total += count ;
		len -= readcount ;
		} ;
	return total ;
} /* paf24_read_s */

static sf_count_t
paf24_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	PAF24_PRIVATE *ppaf24 ;
	int				total ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	total = paf24_read (psf, ppaf24, ptr, len) ;

	return total ;
} /* paf24_read_i */

static sf_count_t
paf24_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				*iptr ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;
	float			normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 / 0x80000000) : (1.0 / 0x100) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = paf24_read (psf, ppaf24, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * iptr [k] ;
		total += count ;
		len -= readcount ;
		} ;
	return total ;
} /* paf24_read_f */

static sf_count_t
paf24_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				*iptr ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;
	double 			normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 / 0x80000000) : (1.0 / 0x100) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = paf24_read (psf, ppaf24, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * iptr [k] ;
		total += count ;
		len -= readcount ;
		} ;
	return total ;
} /* paf24_read_d */

/*---------------------------------------------------------------------------
*/

static int
paf24_write_block (SF_PRIVATE *psf, PAF24_PRIVATE *ppaf24)
{	int				k, nextsample, channel ;
	unsigned char	*cptr ;

	/* First pack block. */

	if (CPU_IS_LITTLE_ENDIAN)
	{	for (k = 0 ; k < PAF24_SAMPLES_PER_BLOCK * ppaf24->channels ; k++)
		{	channel = k % ppaf24->channels ;
			cptr = ppaf24->block + PAF24_BLOCK_SIZE * channel + 3 * (k / ppaf24->channels) ;
			nextsample = ppaf24->samples [k] >> 8 ;
			cptr [0] = nextsample ;
			cptr [1] = nextsample >> 8 ;
			cptr [2] = nextsample >> 16 ;
			} ;

		/* Do endian swapping if necessary. */
		if (psf->endian == SF_ENDIAN_BIG)
			endswap_int_array (ppaf24->data, 8 * ppaf24->channels) ;
		}
	else if (CPU_IS_BIG_ENDIAN)
	{	/* This is correct. */
		for (k = 0 ; k < PAF24_SAMPLES_PER_BLOCK * ppaf24->channels ; k++)
		{	channel = k % ppaf24->channels ;
			cptr = ppaf24->block + PAF24_BLOCK_SIZE * channel + 3 * (k / ppaf24->channels) ;
			nextsample = ppaf24->samples [k] >> 8 ;
			cptr [0] = nextsample ;
			cptr [1] = nextsample >> 8 ;
			cptr [2] = nextsample >> 16 ;
			} ;
		if (psf->endian == SF_ENDIAN_BIG)
			endswap_int_array (ppaf24->data, 8 * ppaf24->channels) ;
		} ;

	/* Write block to disk. */
	if ((k = psf_fwrite (ppaf24->block, 1, ppaf24->blocksize, psf)) != ppaf24->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, ppaf24->blocksize) ;

	if (ppaf24->sample_count < ppaf24->write_block * ppaf24->samplesperblock + ppaf24->write_count)
		ppaf24->sample_count = ppaf24->write_block * ppaf24->samplesperblock + ppaf24->write_count ;

	if (ppaf24->write_count == ppaf24->samplesperblock)
	{	ppaf24->write_block ++ ;
		ppaf24->write_count = 0 ;
		} ;

	return 1 ;
} /* paf24_write_block */

static int
paf24_write (SF_PRIVATE *psf, PAF24_PRIVATE *ppaf24, int *ptr, int len)
{	int		count, total = 0 ;

	while (total < len)
	{	count = (ppaf24->samplesperblock - ppaf24->write_count) * ppaf24->channels ;

		if (count > len - total)
			count = len - total ;

		memcpy (&(ppaf24->samples [ppaf24->write_count * ppaf24->channels]), &(ptr [total]), count * sizeof (int)) ;
		total += count ;
		ppaf24->write_count += count / ppaf24->channels ;

		if (ppaf24->write_count >= ppaf24->samplesperblock)
			paf24_write_block (psf, ppaf24) ;
		} ;

	return total ;
} /* paf24_write */

static sf_count_t
paf24_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				*iptr ;
	int				k, bufferlen, writecount = 0, count ;
	sf_count_t		total = 0 ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = ptr [total + k] << 16 ;
		count = paf24_write (psf, ppaf24, iptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;
	return total ;
} /* paf24_write_s */

static sf_count_t
paf24_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				writecount, count ;
	sf_count_t		total = 0 ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = paf24_write (psf, ppaf24, ptr, writecount) ;

		total += count ;
		len -= count ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* paf24_write_i */

static sf_count_t
paf24_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				*iptr ;
	int				k, bufferlen, writecount = 0, count ;
	sf_count_t		total = 0 ;
	float			normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : (1.0 / 0x100) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = paf24_write (psf, ppaf24, iptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* paf24_write_f */

static sf_count_t
paf24_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	PAF24_PRIVATE 	*ppaf24 ;
	int				*iptr ;
	int				k, bufferlen, writecount = 0, count ;
	sf_count_t		total = 0 ;
	double			normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	ppaf24 = (PAF24_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : (1.0 / 0x100) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = lrint (normfact * ptr [total+k]) ;
		count = paf24_write (psf, ppaf24, iptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* paf24_write_d */


/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 477a5308-451e-4bbd-bab4-fab6caa4e884
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


/* Need to be able to handle 3 byte (24 bit) integers. So defined a
** type and use SIZEOF_TRIBYTE instead of (tribyte).
*/

typedef	void	tribyte ;

#define	SIZEOF_TRIBYTE	3

static sf_count_t	pcm_read_sc2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2s	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;

static sf_count_t	pcm_read_sc2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2i	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;

static sf_count_t	pcm_read_sc2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2f	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;

static sf_count_t	pcm_read_sc2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2d	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;


static sf_count_t	pcm_write_s2sc	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2uc	(SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2bes (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2les (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2bet (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2let (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2bei (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2lei (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;

static sf_count_t	pcm_write_i2sc	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2uc	(SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2bes (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2les (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2bet (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2let (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2bei (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2lei (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;

static sf_count_t	pcm_write_f2sc	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2uc	(SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2bes (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2les (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2bet (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2let (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2bei (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2lei (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;

static sf_count_t	pcm_write_d2sc	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2uc	(SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2bes (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2les (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2bet (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2let (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2bei (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2lei (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static void sc2s_array	(signed char *src, int count, short *dest) ;
static void uc2s_array	(unsigned char *src, int count, short *dest) ;
static void bet2s_array (tribyte *src, int count, short *dest) ;
static void let2s_array (tribyte *src, int count, short *dest) ;
static void bei2s_array (int *src, int count, short *dest) ;
static void lei2s_array (int *src, int count, short *dest) ;

static void sc2i_array	(signed char *src, int count, int *dest) ;
static void uc2i_array	(unsigned char *src, int count, int *dest) ;
static void bes2i_array (short *src, int count, int *dest) ;
static void les2i_array (short *src, int count, int *dest) ;
static void bet2i_array (tribyte *src, int count, int *dest) ;
static void let2i_array (tribyte *src, int count, int *dest) ;

static void sc2f_array	(signed char *src, int count, float *dest, float normfact) ;
static void uc2f_array	(unsigned char *src, int count, float *dest, float normfact) ;
static void bes2f_array (short *src, int count, float *dest, float normfact) ;
static void les2f_array (short *src, int count, float *dest, float normfact) ;
static void bet2f_array (tribyte *src, int count, float *dest, float normfact) ;
static void let2f_array (tribyte *src, int count, float *dest, float normfact) ;
static void bei2f_array (int *src, int count, float *dest, float normfact) ;
static void lei2f_array (int *src, int count, float *dest, float normfact) ;

static void sc2d_array	(signed char *src, int count, double *dest, double normfact) ;
static void uc2d_array	(unsigned char *src, int count, double *dest, double normfact) ;
static void bes2d_array (short *src, int count, double *dest, double normfact) ;
static void les2d_array (short *src, int count, double *dest, double normfact) ;
static void bet2d_array (tribyte *src, int count, double *dest, double normfact) ;
static void let2d_array (tribyte *src, int count, double *dest, double normfact) ;
static void bei2d_array (int *src, int count, double *dest, double normfact) ;
static void lei2d_array (int *src, int count, double *dest, double normfact) ;

static void s2sc_array	(short *src, signed char *dest, int count) ;
static void s2uc_array	(short *src, unsigned char *dest, int count) ;
static void s2bet_array (short *src, tribyte *dest, int count) ;
static void s2let_array (short *src, tribyte *dest, int count) ;
static void s2bei_array (short *src, int *dest, int count) ;
static void s2lei_array (short *src, int *dest, int count) ;

static void i2sc_array	(int *src, signed char *dest, int count) ;
static void i2uc_array	(int *src, unsigned char *dest, int count) ;
static void i2bes_array (int *src, short *dest, int count) ;
static void i2les_array (int *src, short *dest, int count) ;
static void i2bet_array (int *src, tribyte *dest, int count) ;
static void i2let_array (int *src, tribyte *dest, int count) ;

/*-----------------------------------------------------------------------------------------------
*/

enum
{	/* Char type for 8 bit files. */
	SF_CHARS_SIGNED		= 200,
	SF_CHARS_UNSIGNED	= 201
} ;

/*-----------------------------------------------------------------------------------------------
*/

int
pcm_init (SF_PRIVATE *psf)
{	int chars = 0 ;

	if (psf->bytewidth == 0 || psf->sf.channels == 0)
		return SFE_INTERNAL ;


	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_S8)
		chars = SF_CHARS_SIGNED ;
	else if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_U8)
		chars = SF_CHARS_UNSIGNED ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	switch (psf->bytewidth * 0x10000 + psf->endian + chars)
		{	case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_SIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_SIGNED) :
					psf->read_short		= pcm_read_sc2s ;
					psf->read_int		= pcm_read_sc2i ;
					psf->read_float		= pcm_read_sc2f ;
					psf->read_double	= pcm_read_sc2d ;
					break ;
			case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_UNSIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_UNSIGNED) :
					psf->read_short		= pcm_read_uc2s ;
					psf->read_int		= pcm_read_uc2i ;
					psf->read_float		= pcm_read_uc2f ;
					psf->read_double	= pcm_read_uc2d ;
					break ;

			case (2 * 0x10000 + SF_ENDIAN_BIG) :
					psf->read_short		= pcm_read_bes2s ;
					psf->read_int		= pcm_read_bes2i ;
					psf->read_float		= pcm_read_bes2f ;
					psf->read_double	= pcm_read_bes2d ;
					break ;
			case (3 * 0x10000 + SF_ENDIAN_BIG) :
					psf->read_short		= pcm_read_bet2s ;
					psf->read_int		= pcm_read_bet2i ;
					psf->read_float		= pcm_read_bet2f ;
					psf->read_double	= pcm_read_bet2d ;
					break ;
			case (4 * 0x10000 + SF_ENDIAN_BIG) :
					psf->read_short		= pcm_read_bei2s ;
					psf->read_int		= pcm_read_bei2i ;
					psf->read_float		= pcm_read_bei2f ;
					psf->read_double	= pcm_read_bei2d ;
					break ;

			case (2 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->read_short		= pcm_read_les2s ;
					psf->read_int		= pcm_read_les2i ;
					psf->read_float		= pcm_read_les2f ;
					psf->read_double	= pcm_read_les2d ;
					break ;
			case (3 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->read_short		= pcm_read_let2s ;
					psf->read_int		= pcm_read_let2i ;
					psf->read_float		= pcm_read_let2f ;
					psf->read_double	= pcm_read_let2d ;
					break ;
			case (4 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->read_short		= pcm_read_lei2s ;
					psf->read_int		= pcm_read_lei2i ;
					psf->read_float		= pcm_read_lei2f ;
					psf->read_double	= pcm_read_lei2d ;
					break ;
			default :
				psf_log_printf (psf, "pcm.c returning SFE_UNIMPLEMENTED\n") ;
				return SFE_UNIMPLEMENTED ;
			} ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	switch (psf->bytewidth * 0x10000 + psf->endian + chars)
		{	case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_SIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_SIGNED) :
					psf->write_short	= pcm_write_s2sc ;
					psf->write_int		= pcm_write_i2sc ;
					psf->write_float	= pcm_write_f2sc ;
					psf->write_double	= pcm_write_d2sc ;
					break ;
			case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_UNSIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_UNSIGNED) :
					psf->write_short	= pcm_write_s2uc ;
					psf->write_int		= pcm_write_i2uc ;
					psf->write_float	= pcm_write_f2uc ;
					psf->write_double	= pcm_write_d2uc ;
					break ;

			case (2 * 0x10000 + SF_ENDIAN_BIG) :
					psf->write_short	= pcm_write_s2bes ;
					psf->write_int		= pcm_write_i2bes ;
					psf->write_float	= pcm_write_f2bes ;
					psf->write_double	= pcm_write_d2bes ;
					break ;

			case (3 * 0x10000 + SF_ENDIAN_BIG) :
					psf->write_short	= pcm_write_s2bet ;
					psf->write_int		= pcm_write_i2bet ;
					psf->write_float	= pcm_write_f2bet ;
					psf->write_double	= pcm_write_d2bet ;
					break ;

			case (4 * 0x10000 + SF_ENDIAN_BIG) :
					psf->write_short	= pcm_write_s2bei ;
					psf->write_int		= pcm_write_i2bei ;
					psf->write_float	= pcm_write_f2bei ;
					psf->write_double	= pcm_write_d2bei ;
					break ;

			case (2 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->write_short	= pcm_write_s2les ;
					psf->write_int		= pcm_write_i2les ;
					psf->write_float	= pcm_write_f2les ;
					psf->write_double	= pcm_write_d2les ;
					break ;

			case (3 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->write_short	= pcm_write_s2let ;
					psf->write_int		= pcm_write_i2let ;
					psf->write_float	= pcm_write_f2let ;
					psf->write_double	= pcm_write_d2let ;
					break ;

			case (4 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->write_short	= pcm_write_s2lei ;
					psf->write_int		= pcm_write_i2lei ;
					psf->write_float	= pcm_write_f2lei ;
					psf->write_double	= pcm_write_d2lei ;
					break ;

			default :
				psf_log_printf (psf, "pcm.c returning SFE_UNIMPLEMENTED\n") ;
				return SFE_UNIMPLEMENTED ;
			} ;

		} ;

	if (psf->filelength > psf->dataoffset)
	{	psf->datalength = (psf->dataend > 0) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
		}
	else
		psf->datalength = 0 ;

	psf->sf.frames = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* pcm_init */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2s_array ((signed char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2s */

static sf_count_t
pcm_read_uc2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2s_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2s */

static sf_count_t
pcm_read_bes2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (short), len, psf) ;
	if (CPU_IS_LITTLE_ENDIAN)
		endswap_short_array (ptr, len) ;

	return total ;
} /* pcm_read_bes2s */

static sf_count_t
pcm_read_les2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (short), len, psf) ;
	if (CPU_IS_BIG_ENDIAN)
		endswap_short_array (ptr, len) ;

	return total ;
} /* pcm_read_les2s */

static sf_count_t
pcm_read_bet2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2s_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2s */

static sf_count_t
pcm_read_let2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2s_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2s */

static sf_count_t
pcm_read_bei2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		bei2s_array ((int*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bei2s */

static sf_count_t
pcm_read_lei2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		lei2s_array ((int*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_lei2s */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2i_array ((signed char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2i */

static sf_count_t
pcm_read_uc2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2i_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2i */

static sf_count_t
pcm_read_bes2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		bes2i_array ((short*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bes2i */

static sf_count_t
pcm_read_les2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		les2i_array ((short*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_les2i */

static sf_count_t
pcm_read_bet2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2i_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2i */

static sf_count_t
pcm_read_let2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2i_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2i */

static sf_count_t
pcm_read_bei2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (int), len, psf) ;
	if (CPU_IS_LITTLE_ENDIAN)
		endswap_int_array	(ptr, len) ;

	return total ;
} /* pcm_read_bei2i */

static sf_count_t
pcm_read_lei2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (int), len, psf) ;
	if (CPU_IS_BIG_ENDIAN)
		endswap_int_array	(ptr, len) ;

	return total ;
} /* pcm_read_lei2i */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2f_array ((signed char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2f */

static sf_count_t
pcm_read_uc2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2f_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2f */

static sf_count_t
pcm_read_bes2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		bes2f_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bes2f */

static sf_count_t
pcm_read_les2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		les2f_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_les2f */

static sf_count_t
pcm_read_bet2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	/* Special normfactor because tribyte value is read into an int. */
	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2f_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2f */

static sf_count_t
pcm_read_let2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	/* Special normfactor because tribyte value is read into an int. */
	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2f_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2f */

static sf_count_t
pcm_read_bei2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		bei2f_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bei2f */

static sf_count_t
pcm_read_lei2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		lei2f_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_lei2f */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2d_array ((signed char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2d */

static sf_count_t
pcm_read_uc2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2d_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2d */

static sf_count_t
pcm_read_bes2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		bes2d_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bes2d */

static sf_count_t
pcm_read_les2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		les2d_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_les2d */

static sf_count_t
pcm_read_bet2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2d_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2d */

static sf_count_t
pcm_read_let2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	/* Special normfactor because tribyte value is read into an int. */
	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2d_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2d */

static sf_count_t
pcm_read_bei2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		bei2d_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bei2d */

static sf_count_t
pcm_read_lei2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		lei2d_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_lei2d */

/*===============================================================================================
**-----------------------------------------------------------------------------------------------
**===============================================================================================
*/

static sf_count_t
pcm_write_s2sc	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2sc_array (ptr + total, (signed char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2sc */

static sf_count_t
pcm_write_s2uc	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2uc_array (ptr + total, (unsigned char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (unsigned char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2uc */

static sf_count_t
pcm_write_s2bes	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{
	if (CPU_IS_BIG_ENDIAN)
		return psf_fwrite (ptr, sizeof (short), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (short) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_short_copy ((short*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_s2bes */

static sf_count_t
pcm_write_s2les	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{
	if (CPU_IS_LITTLE_ENDIAN)
		return psf_fwrite (ptr, sizeof (short), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (short) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_short_copy ((short*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_s2les */

static sf_count_t
pcm_write_s2bet	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2bet_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2bet */

static sf_count_t
pcm_write_s2let	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2let_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2let */

static sf_count_t
pcm_write_s2bei	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2bei_array (ptr + total, (int*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2bei */

static sf_count_t
pcm_write_s2lei	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2lei_array (ptr + total, (int*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2lei */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_write_i2sc	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2sc_array (ptr + total, (signed char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2sc */

static sf_count_t
pcm_write_i2uc	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2uc_array (ptr + total, (unsigned char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2uc */

static sf_count_t
pcm_write_i2bes	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2bes_array (ptr + total, (short*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2bes */

static sf_count_t
pcm_write_i2les	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2les_array (ptr + total, (short*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2les */

static sf_count_t
pcm_write_i2bet	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2bet_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2bet */

static sf_count_t
pcm_write_i2let	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2let_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2les */

static sf_count_t
pcm_write_i2bei	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{
	if (CPU_IS_BIG_ENDIAN)
		return psf_fwrite (ptr, sizeof (int), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (int) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_int_copy ((int*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_i2bei */

static sf_count_t
pcm_write_i2lei	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{
	if (CPU_IS_LITTLE_ENDIAN)
		return psf_fwrite (ptr, sizeof (int), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (int) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_int_copy ((int*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_i2lei */

/*------------------------------------------------------------------------------
**==============================================================================
**------------------------------------------------------------------------------
*/

static void
f2sc_array (float *src, signed char *dest, int count, int normalize)
{	float normfact ;

	normfact = normalize ? (1.0 * 0x7F) : 1.0 ;

	while (count)
	{	count -- ;
		dest [count] = lrintf (src [count] * normfact) ;
		} ;
} /* f2sc_array */

static void
f2sc_clip_array (float *src, signed char *dest, int count, int normalize)
{	float	normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x1000000) ;

	while (count)
	{	count -- ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	dest [count] = 127 ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	dest [count] = -128 ;
			continue ;
			} ;

		dest [count] = lrintf (scaled_value) >> 24 ;
		} ;
} /* f2sc_clip_array */

static sf_count_t
pcm_write_f2sc	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, signed char *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2sc_clip_array : f2sc_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (signed char*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2sc */

/*==============================================================================
*/

static	void
f2uc_array	(float *src, unsigned char *dest, int count, int normalize)
{	float normfact ;

	normfact = normalize ? (1.0 * 0x7F) : 1.0 ;

	while (count)
	{	count -- ;
		dest [count] = lrintf (src [count] * normfact) + 128 ;
		} ;
} /* f2uc_array */

static	void
f2uc_clip_array	(float *src, unsigned char *dest, int count, int normalize)
{	float	normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x1000000) ;

	while (count)
	{	count -- ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	dest [count] = 0xFF ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	dest [count] = 0 ;
			continue ;
			} ;

		dest [count] = (lrintf (scaled_value) >> 24) + 128 ;
		} ;
} /* f2uc_clip_array */

static sf_count_t
pcm_write_f2uc	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, unsigned char *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2uc_clip_array : f2uc_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (unsigned char*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (unsigned char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2uc */

/*==============================================================================
*/

static void
f2bes_array (float *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float 			normfact ;
	short			value ;

	normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [1] = value ;
		ucptr [0] = value >> 8 ;
		} ;
} /* f2bes_array */

static void
f2bes_clip_array (float *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x10000) ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [1] = 0xFF ;
			ucptr [0] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [1] = 0x00 ;
			ucptr [0] = 0x80 ;
			continue ;
			} ;

		value = lrintf (scaled_value) ;
		ucptr [1] = value >> 16 ;
		ucptr [0] = value >> 24 ;
		} ;
} /* f2bes_clip_array */

static sf_count_t
pcm_write_f2bes	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, short *t, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2bes_clip_array : f2bes_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (short*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2bes */

/*==============================================================================
*/

static void
f2les_array (float *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact ;
	int				value ;

	normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		} ;
} /* f2les_array */

static void
f2les_clip_array (float *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x10000) ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0xFF ;
			ucptr [1] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x00 ;
			ucptr [1] = 0x80 ;
			continue ;
			} ;

		value = lrintf (scaled_value) ;
		ucptr [0] = value >> 16 ;
		ucptr [1] = value >> 24 ;
		} ;
} /* f2les_clip_array */

static sf_count_t
pcm_write_f2les	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, short *t, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2les_clip_array : f2les_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (short*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2les */

/*==============================================================================
*/

static void
f2let_array (float *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float 			normfact ;
	int				value ;

	normfact = normalize ? (1.0 * 0x7FFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		} ;
} /* f2let_array */

static void
f2let_clip_array (float *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x100) ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0xFF ;
			ucptr [1] = 0xFF ;
			ucptr [2] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x00 ;
			ucptr [1] = 0x00 ;
			ucptr [2] = 0x80 ;
			continue ;
			} ;

		value = lrintf (scaled_value) ;
		ucptr [0] = value >> 8 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 24 ;
		} ;
} /* f2let_clip_array */

static sf_count_t
pcm_write_f2let	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, tribyte *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2let_clip_array : f2let_array ;
	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (tribyte*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2let */

/*==============================================================================
*/

static void
f2bet_array (float *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float 			normfact ;
	int				value ;

	normfact = normalize ? (1.0 * 0x7FFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value >> 16 ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value ;
		} ;
} /* f2bet_array */

static void
f2bet_clip_array (float *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x100) ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0x7F ;
			ucptr [1] = 0xFF ;
			ucptr [2] = 0xFF ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x80 ;
			ucptr [1] = 0x00 ;
			ucptr [2] = 0x00 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		} ;
} /* f2bet_clip_array */

static sf_count_t
pcm_write_f2bet	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, tribyte *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2bet_clip_array : f2bet_array ;
	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (tribyte*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2bet */

/*==============================================================================
*/

static void
f2bei_array (float *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact ;
	int				value ;

	normfact = normalize ? (1.0 * 0x7FFFFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		ucptr [3] = value ;
		} ;
} /* f2bei_array */

static void
f2bei_clip_array (float *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= 1.0 * 0x7FFFFFFF)
		{	ucptr [0] = 0x7F ;
			ucptr [1] = 0xFF ;
			ucptr [2] = 0xFF ;
			ucptr [3] = 0xFF ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x80 ;
			ucptr [1] = 0x00 ;
			ucptr [2] = 0x00 ;
			ucptr [3] = 0x00 ;
			continue ;
			} ;

		value = lrintf (scaled_value) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		ucptr [3] = value ;
		} ;
} /* f2bei_clip_array */

static sf_count_t
pcm_write_f2bei	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, int *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2bei_clip_array : f2bei_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (int*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2bei */

/*==============================================================================
*/

static void
f2lei_array (float *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact ;
	int				value ;

	normfact = normalize ? (1.0 * 0x7FFFFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		ucptr [3] = value >> 24 ;
		} ;
} /* f2lei_array */

static void
f2lei_clip_array (float *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	float			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0xFF ;
			ucptr [1] = 0xFF ;
			ucptr [2] = 0xFF ;
			ucptr [3] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x00 ;
			ucptr [1] = 0x00 ;
			ucptr [2] = 0x00 ;
			ucptr [3] = 0x80 ;
			continue ;
			} ;

		value = lrintf (scaled_value) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		ucptr [3] = value >> 24 ;
		} ;
} /* f2lei_clip_array */

static sf_count_t
pcm_write_f2lei	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	void		(*convert) (float *, int *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? f2lei_clip_array : f2lei_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (int*) (psf->buffer), writecount, psf->norm_float) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2lei */

/*==============================================================================
*/

static	void
d2sc_array	(double *src, signed char *dest, int count, int normalize)
{	double	normfact ;

	normfact = normalize ? (1.0 * 0x7F) : 1.0 ;

	while (count)
	{	count -- ;
		dest [count] = lrint (src [count] * normfact) ;
		} ;
} /* d2sc_array */

static	void
d2sc_clip_array	(double *src, signed char *dest, int count, int normalize)
{	double	normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x1000000) ;

	while (count)
	{	count -- ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	dest [count] = 127 ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	dest [count] = -128 ;
			continue ;
			} ;

		dest [count] = lrintf (scaled_value) >> 24 ;
		} ;
} /* d2sc_clip_array */

static sf_count_t
pcm_write_d2sc	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, signed char *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2sc_clip_array : d2sc_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (signed char*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2sc */

/*==============================================================================
*/

static	void
d2uc_array	(double *src, unsigned char *dest, int count, int normalize)
{	double normfact ;

	normfact = normalize ? (1.0 * 0x7F) : 1.0 ;

	while (count)
	{	count -- ;
		dest [count] = lrint (src [count] * normfact) + 128 ;
		} ;
} /* d2uc_array */

static	void
d2uc_clip_array	(double *src, unsigned char *dest, int count, int normalize)
{	double	normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x1000000) ;

	while (count)
	{	count -- ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	dest [count] = 255 ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	dest [count] = 0 ;
			continue ;
			} ;

		dest [count] = (lrint (src [count] * normfact) >> 24) + 128 ;
		} ;
} /* d2uc_clip_array */


static sf_count_t
pcm_write_d2uc	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, unsigned char *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2uc_clip_array : d2uc_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (unsigned char*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (unsigned char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2uc */

/*==============================================================================
*/

static void
d2bes_array (double *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	short			value ;
	double			normfact ;

	normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrint (src [count] * normfact) ;
		ucptr [1] = value ;
		ucptr [0] = value >> 8 ;
		} ;
} /* d2bes_array */

static void
d2bes_clip_array (double *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	double			normfact, scaled_value ;
	int				value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x10000) ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [1] = 0xFF ;
			ucptr [0] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [1] = 0x00 ;
			ucptr [0] = 0x80 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [1] = value >> 16 ;
		ucptr [0] = value >> 24 ;
		} ;
} /* d2bes_clip_array */

static sf_count_t
pcm_write_d2bes	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, short *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2bes_clip_array : d2bes_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (short*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2bes */

/*==============================================================================
*/

static void
d2les_array (double *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	short			value ;
	double			normfact ;

	normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		} ;
} /* d2les_array */

static void
d2les_clip_array (double *src, short *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x10000) ;
	ucptr = ((unsigned char*) dest) + 2 * count ;

	while (count)
	{	count -- ;
		ucptr -= 2 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0xFF ;
			ucptr [1] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x00 ;
			ucptr [1] = 0x80 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [0] = value >> 16 ;
		ucptr [1] = value >> 24 ;
		} ;
} /* d2les_clip_array */

static sf_count_t
pcm_write_d2les	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, short *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2les_clip_array : d2les_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (short*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2les */

/*==============================================================================
*/

static void
d2let_array (double *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact ;

	normfact = normalize ? (1.0 * 0x7FFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		} ;
} /* d2let_array */

static void
d2let_clip_array (double *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x100) ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0xFF ;
			ucptr [1] = 0xFF ;
			ucptr [2] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x00 ;
			ucptr [1] = 0x00 ;
			ucptr [2] = 0x80 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [0] = value >> 8 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 24 ;
		} ;
} /* d2let_clip_array */

static sf_count_t
pcm_write_d2let	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, tribyte *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2let_clip_array : d2let_array ;
	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (tribyte*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2let */

/*==============================================================================
*/

static void
d2bet_array (double *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact ;

	normfact = normalize ? (1.0 * 0x7FFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrint (src [count] * normfact) ;
		ucptr [2] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [0] = value >> 16 ;
		} ;
} /* d2bet_array */

static void
d2bet_clip_array (double *src, tribyte *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : (1.0 * 0x100) ;
	ucptr = ((unsigned char*) dest) + 3 * count ;

	while (count)
	{	count -- ;
		ucptr -= 3 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [2] = 0xFF ;
			ucptr [1] = 0xFF ;
			ucptr [0] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [2] = 0x00 ;
			ucptr [1] = 0x00 ;
			ucptr [0] = 0x80 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [2] = value >> 8 ;
		ucptr [1] = value >> 16 ;
		ucptr [0] = value >> 24 ;
		} ;
} /* d2bet_clip_array */

static sf_count_t
pcm_write_d2bet	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, tribyte *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2bet_clip_array : d2bet_array ;
	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (tribyte*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2bet */

/*==============================================================================
*/

static void
d2bei_array (double *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact ;

	normfact = normalize ? (1.0 * 0x7FFFFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		ucptr [3] = value ;
		} ;
} /* d2bei_array */

static void
d2bei_clip_array (double *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [3] = 0xFF ;
			ucptr [2] = 0xFF ;
			ucptr [1] = 0xFF ;
			ucptr [0] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [3] = 0x00 ;
			ucptr [2] = 0x00 ;
			ucptr [1] = 0x00 ;
			ucptr [0] = 0x80 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		ucptr [3] = value ;
		} ;
} /* d2bei_clip_array */

static sf_count_t
pcm_write_d2bei	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, int *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2bei_clip_array : d2bei_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (int*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2bei */

/*==============================================================================
*/

static void
d2lei_array (double *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact ;

	normfact = normalize ? (1.0 * 0x7FFFFFFF) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		ucptr [3] = value >> 24 ;
		} ;
} /* d2lei_array */

static void
d2lei_clip_array (double *src, int *dest, int count, int normalize)
{	unsigned char	*ucptr ;
	int				value ;
	double			normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : 1.0 ;
	ucptr = ((unsigned char*) dest) + 4 * count ;

	while (count)
	{	count -- ;
		ucptr -= 4 ;
		scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	ucptr [0] = 0xFF ;
			ucptr [1] = 0xFF ;
			ucptr [2] = 0xFF ;
			ucptr [3] = 0x7F ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	ucptr [0] = 0x00 ;
			ucptr [1] = 0x00 ;
			ucptr [2] = 0x00 ;
			ucptr [3] = 0x80 ;
			continue ;
			} ;

		value = lrint (scaled_value) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		ucptr [3] = value >> 24 ;
		} ;
} /* d2lei_clip_array */

static sf_count_t
pcm_write_d2lei	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	void		(*convert) (double *, int *, int, int) ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	convert = (psf->add_clipping) ? d2lei_clip_array : d2lei_array ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		convert (ptr + total, (int*) (psf->buffer), writecount, psf->norm_double) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2lei */

/*==============================================================================
*/

static	void
sc2s_array	(signed char *src, int count, short *dest)
{	while (count)
	{	count -- ;
		dest [count] = src [count] << 8 ;
		} ;
} /* sc2s_array */

static	void
uc2s_array	(unsigned char *src, int count, short *dest)
{	while (count)
	{	count -- ;
		dest [count] = (((short) src [count]) - 0x80) << 8 ;
		} ;
} /* uc2s_array */

static void
let2s_array (tribyte *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = LET2H_SHORT_PTR (ucptr) ;
		} ;
} /* let2s_array */

static void
bet2s_array (tribyte *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = BET2H_SHORT_PTR (ucptr) ;
		} ;
} /* bet2s_array */

static void
lei2s_array (int *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		dest [count] = LEI2H_SHORT_PTR (ucptr) ;
		} ;
} /* lei2s_array */

static void
bei2s_array (int *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		dest [count] = BEI2H_SHORT_PTR (ucptr) ;
		} ;
} /* bei2s_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
sc2i_array	(signed char *src, int count, int *dest)
{	while (count)
	{	count -- ;
		dest [count] = ((int) src [count]) << 24 ;
		} ;
} /* sc2i_array */

static	void
uc2i_array	(unsigned char *src, int count, int *dest)
{	while (count)
	{	count -- ;
		dest [count] = (((int) src [count]) - 128) << 24 ;
		} ;
} /* uc2i_array */

static void
bes2i_array (short *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		dest [count] = BES2H_INT_PTR (ucptr) ;
		} ;
} /* bes2i_array */

static void
les2i_array (short *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		dest [count] = LES2H_INT_PTR (ucptr) ;
		} ;
} /* les2i_array */

static void
bet2i_array (tribyte *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = BET2H_INT_PTR (ucptr) ;
		} ;
} /* bet2i_array */

static void
let2i_array (tribyte *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = LET2H_INT_PTR (ucptr) ;
		} ;
} /* let2i_array */

/*-----------------------------------------------------------------------------------------------
*/


static	void
sc2f_array	(signed char *src, int count, float *dest, float normfact)
{	while (count)
	{	count -- ;
		dest [count] = ((float) src [count]) * normfact ;
		} ;
} /* sc2f_array */

static	void
uc2f_array	(unsigned char *src, int count, float *dest, float normfact)
{	while (count)
	{	count -- ;
		dest [count] = (((float) src [count]) - 128.0) * normfact ;
		} ;
} /* uc2f_array */

static void
les2f_array (short *src, int count, float *dest, float normfact)
{	short	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LES2H_SHORT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* les2f_array */

static void
bes2f_array (short *src, int count, float *dest, float normfact)
{	short			value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BES2H_SHORT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* bes2f_array */

static void
let2f_array (tribyte *src, int count, float *dest, float normfact)
{	unsigned char	*ucptr ;
	int 			value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = LET2H_INT_PTR (ucptr) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* let2f_array */

static void
bet2f_array (tribyte *src, int count, float *dest, float normfact)
{	unsigned char	*ucptr ;
	int 	value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = BET2H_INT_PTR (ucptr) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* bet2f_array */

static void
lei2f_array (int *src, int count, float *dest, float normfact)
{	int 			value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LEI2H_INT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* lei2f_array */

static void
bei2f_array (int *src, int count, float *dest, float normfact)
{	int 			value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BEI2H_INT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* bei2f_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
sc2d_array	(signed char *src, int count, double *dest, double normfact)
{	while (count)
	{	count -- ;
		dest [count] = ((double) src [count]) * normfact ;
		} ;
} /* sc2d_array */

static	void
uc2d_array	(unsigned char *src, int count, double *dest, double normfact)
{	while (count)
	{	count -- ;
		dest [count] = (((double) src [count]) - 128.0) * normfact ;
		} ;
} /* uc2d_array */

static void
les2d_array (short *src, int count, double *dest, double normfact)
{	short	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LES2H_SHORT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* les2d_array */

static void
bes2d_array (short *src, int count, double *dest, double normfact)
{	short	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BES2H_SHORT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* bes2d_array */

static void
let2d_array (tribyte *src, int count, double *dest, double normfact)
{	unsigned char	*ucptr ;
	int 	value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = LET2H_INT_PTR (ucptr) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* let2d_array */

static void
bet2d_array (tribyte *src, int count, double *dest, double normfact)
{	unsigned char	*ucptr ;
	int 	value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = (ucptr [0] << 24) | (ucptr [1] << 16) | (ucptr [2] << 8) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* bet2d_array */

static void
lei2d_array (int *src, int count, double *dest, double normfact)
{	int 	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LEI2H_INT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* lei2d_array */

static void
bei2d_array (int *src, int count, double *dest, double normfact)
{	int 	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BEI2H_INT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* bei2d_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
s2sc_array	(short *src, signed char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] >> 8 ;
		} ;
} /* s2sc_array */

static	void
s2uc_array	(short *src, unsigned char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = (src [count] >> 8) + 0x80 ;
		} ;
} /* s2uc_array */

static void
s2let_array (short *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		ucptr [0] = 0 ;
		ucptr [1] = src [count] ;
		ucptr [2] = src [count] >> 8 ;
		} ;
} /* s2let_array */

static void
s2bet_array (short *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		ucptr [2] = 0 ;
		ucptr [1] = src [count] ;
		ucptr [0] = src [count] >> 8 ;
		} ;
} /* s2bet_array */

static void
s2lei_array (short *src, int *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		ucptr [0] = 0 ;
		ucptr [1] = 0 ;
		ucptr [2] = src [count] ;
		ucptr [3] = src [count] >> 8 ;
		} ;
} /* s2lei_array */

static void
s2bei_array (short *src, int *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		ucptr [0] = src [count] >> 8 ;
		ucptr [1] = src [count] ;
		ucptr [2] = 0 ;
		ucptr [3] = 0 ;
		} ;
} /* s2bei_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
i2sc_array	(int *src, signed char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = (src [count] >> 24) ;
		} ;
} /* i2sc_array */

static	void
i2uc_array	(int *src, unsigned char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = ((src [count] >> 24) + 128) ;
		} ;
} /* i2uc_array */

static void
i2bes_array (int *src, short *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		ucptr [0] = src [count] >> 24 ;
		ucptr [1] = src [count] >> 16 ;
		} ;
} /* i2bes_array */

static void
i2les_array (int *src, short *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		ucptr [0] = src [count] >> 16 ;
		ucptr [1] = src [count] >> 24 ;
		} ;
} /* i2les_array */

static void
i2let_array (int *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = src [count] >> 8 ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		} ;
} /* i2let_array */

static void
i2bet_array (int *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = src [count] >> 8 ;
		ucptr [2] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [0] = value >> 16 ;
		} ;
} /* i2bet_array */

/*-----------------------------------------------------------------------------------------------
*/

/*-----------------------------------------------------------------------------------------------
*/

/*==============================================================================
*/
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: d8bc7c0e-1e2f-4ff3-a28f-10ce1fbade3b
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include	<stdio.h>
#include	<assert.h>



/*	4.2.0 .. 4.2.3	PREPROCESSING SECTION
 *  
 *  	After A-law to linear conversion (or directly from the
 *   	Ato D converter) the following scaling is assumed for
 * 	input to the RPE-LTP algorithm:
 *
 *      in:  0.1.....................12
 *	     S.v.v.v.v.v.v.v.v.v.v.v.v.*.*.*
 *
 *	Where S is the sign bit, v a valid bit, and * a "don't care" bit.
 * 	The original signal is called sop[..]
 *
 *      out:   0.1................... 12 
 *	     S.S.v.v.v.v.v.v.v.v.v.v.v.v.0.0
 */


void Gsm_Preprocess (
	struct gsm_state * S,
	word		 * s,
	word 		 * so )		/* [0..159] 	IN/OUT	*/
{

	word       z1 = S->z1;
	longword L_z2 = S->L_z2;
	word 	   mp = S->mp;

	word 	   	s1;
	longword      L_s2;

	longword      L_temp;

	word		msp, lsp;
	word		SO;

	register int		k = 160;

	while (k--) {

	/*  4.2.1   Downscaling of the input signal
	 */
		SO = SASR_W( *s, 3 ) << 2;
		s++;

		assert (SO >= -0x4000);	/* downscaled by     */
		assert (SO <=  0x3FFC);	/* previous routine. */


	/*  4.2.2   Offset compensation
	 * 
	 *  This part implements a high-pass filter and requires extended
	 *  arithmetic precision for the recursive part of this filter.
	 *  The input of this procedure is the array so[0...159] and the
	 *  output the array sof[ 0...159 ].
	 */
		/*   Compute the non-recursive part
		 */

		s1 = SO - z1;			/* s1 = gsm_sub( *so, z1 ); */
		z1 = SO;

		assert(s1 != MIN_WORD);

		/*   Compute the recursive part
		 */
		L_s2 = s1;
		L_s2 <<= 15;

		/*   Execution of a 31 bv 16 bits multiplication
		 */

		msp = SASR_L( L_z2, 15 );
		lsp = L_z2-((longword)msp<<15); /* gsm_L_sub(L_z2,(msp<<15)); */

		L_s2  += GSM_MULT_R( lsp, 32735 );
		L_temp = (longword)msp * 32735; /* GSM_L_MULT(msp,32735) >> 1;*/
		L_z2   = GSM_L_ADD( L_temp, L_s2 );

		/*    Compute sof[k] with rounding
		 */
		L_temp = GSM_L_ADD( L_z2, 16384 );

	/*   4.2.3  Preemphasis
	 */

		msp   = GSM_MULT_R( mp, -28180 );
		mp    = SASR_L( L_temp, 15 );
		*so++ = GSM_ADD( mp, msp );
	}

	S->z1   = z1;
	S->L_z2 = L_z2;
	S->mp   = mp;
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: b760b0d9-3a05-4da3-9dc9-441ffb905d87
*/

/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define PVF1_MARKER		(MAKE_MARKER ('P', 'V', 'F', '1'))

/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int		pvf_close		(SF_PRIVATE *psf) ;

static int		pvf_write_header (SF_PRIVATE *psf, int calc_length) ;
static int		pvf_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
pvf_open	(SF_PRIVATE *psf)
{	int		subformat ;
	int		error = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = pvf_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_PVF)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = SF_ENDIAN_BIG ;

		if (pvf_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = pvf_write_header ;
		} ;

	psf->close = pvf_close ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case SF_FORMAT_PCM_S8 :	/* 8-bit linear PCM. */
		case SF_FORMAT_PCM_16 :	/* 16-bit linear PCM. */
		case SF_FORMAT_PCM_32 :	/* 32-bit linear PCM. */
				error = pcm_init (psf) ;
				break ;

		default :	break ;
		} ;

	return error ;
} /* pvf_open */

/*------------------------------------------------------------------------------
*/

static int
pvf_close	(SF_PRIVATE *psf)
{
	psf = psf ;

	return 0 ;
} /* pvf_close */

static int
pvf_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;

	if (psf->pipeoffset > 0)
		return 0 ;

	calc_length = calc_length ; /* Avoid a compiler warning. */

	current = psf_ftell (psf) ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	if (psf->is_pipe == SF_FALSE)
		psf_fseek (psf, 0, SEEK_SET) ;

	LSF_SNPRINTF ((char*) psf->header, sizeof (psf->header), "PVF1\n%d %d %d\n",
		psf->sf.channels, psf->sf.samplerate, psf->bytewidth * 8) ;

	psf->headindex = strlen ((char*) psf->header) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* pvf_write_header */

static int
pvf_read_header (SF_PRIVATE *psf)
{	char	buffer [32] ;
	int		marker, channels, samplerate, bitwidth ;

	psf_binheader_readf (psf, "pmj", 0, &marker, 1) ;
	psf_log_printf (psf, "%M\n", marker) ;

	if (marker != PVF1_MARKER)
		return SFE_PVF_NO_PVF1 ;

	/* Grab characters up until a newline which is replaced by an EOS. */
	psf_binheader_readf (psf, "G", buffer, sizeof (buffer)) ;

	if (sscanf (buffer, "%d %d %d", &channels, &samplerate, &bitwidth) != 3)
		return SFE_PVF_BAD_HEADER ;

	psf_log_printf (psf, " Channels    : %d\n Sample rate : %d\n Bit width   : %d\n",
				channels, samplerate, bitwidth) ;

	psf->sf.channels = channels ;
	psf->sf.samplerate = samplerate ;

	switch (bitwidth)
	{	case 8 :
				psf->sf.format = SF_FORMAT_PVF | SF_FORMAT_PCM_S8 ;
				psf->bytewidth = 1 ;
				break ;

		case 16 :
				psf->sf.format = SF_FORMAT_PVF | SF_FORMAT_PCM_16 ;
				psf->bytewidth = 2 ;
				break ;
		case 32 :
				psf->sf.format = SF_FORMAT_PVF | SF_FORMAT_PCM_32 ;
				psf->bytewidth = 4 ;
				break ;

		default :
				return SFE_PVF_BAD_BITWIDTH ;
		} ;

	psf->dataoffset = psf_ftell (psf) ;
	psf_log_printf (psf, " Data Offset : %D\n", psf->dataoffset) ;

	psf->endian = SF_ENDIAN_BIG ;

	psf->datalength = psf->filelength - psf->dataoffset ;
	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	psf->close = pvf_close ;

	if (! psf->sf.frames && psf->blockwidth)
		psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;

	return 0 ;
} /* pvf_read_header */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 20a26761-8bc1-41d7-b1f3-9793bf3d9864
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>


/*------------------------------------------------------------------------------
** Public function.
*/

int
raw_open	(SF_PRIVATE *psf)
{	int	subformat, error = SFE_NO_ERROR ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;

	if (CPU_IS_BIG_ENDIAN && (psf->endian == 0 || psf->endian == SF_ENDIAN_CPU))
		psf->endian = SF_ENDIAN_BIG ;
	else if (CPU_IS_LITTLE_ENDIAN && (psf->endian == 0 || psf->endian == SF_ENDIAN_CPU))
		psf->endian = SF_ENDIAN_LITTLE ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;
	psf->dataoffset = 0 ;
	psf->datalength = psf->filelength ;
	switch (subformat)
	{	case SF_FORMAT_PCM_S8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_U8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_ULAW :
				error = ulaw_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :
				error = alaw_init (psf) ;
				break ;

		case SF_FORMAT_GSM610 :
				error = gsm610_init (psf) ;
				break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :
				error = double64_init (psf) ;
				break ;

		case SF_FORMAT_DWVW_12 :
				error = dwvw_init (psf, 12) ;
				break ;

		case SF_FORMAT_DWVW_16 :
				error = dwvw_init (psf, 16) ;
				break ;

		case SF_FORMAT_DWVW_24 :
				error = dwvw_init (psf, 24) ;
				break ;

		case SF_FORMAT_VOX_ADPCM :
				error = vox_adpcm_init (psf) ;
				break ;
		/* Lite remove end */

		default : return SFE_BAD_OPEN_FORMAT ;
		} ;

	return error ;
} /* raw_open */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: f0066de7-d6ce-4f36-a1e0-e475c07d4e1a
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>



/*  4.2.13 .. 4.2.17  RPE ENCODING SECTION
 */

/* 4.2.13 */

static void Weighting_filter (
	register word	* e,		/* signal [-5..0.39.44]	IN  */
	word		* x		/* signal [0..39]	OUT */
)
/*
 *  The coefficients of the weighting filter are stored in a table
 *  (see table 4.4).  The following scaling is used:
 *
 *	H[0..10] = integer( real_H[ 0..10] * 8192 ); 
 */
{
	/* word			wt[ 50 ]; */

	register longword	L_result;
	register int		k /* , i */ ;

	/*  Initialization of a temporary working array wt[0...49]
	 */

	/* for (k =  0; k <=  4; k++) wt[k] = 0;
	 * for (k =  5; k <= 44; k++) wt[k] = *e++;
	 * for (k = 45; k <= 49; k++) wt[k] = 0;
	 *
	 *  (e[-5..-1] and e[40..44] are allocated by the caller,
	 *  are initially zero and are not written anywhere.)
	 */
	e -= 5;

	/*  Compute the signal x[0..39]
	 */ 
	for (k = 0; k <= 39; k++) {

		L_result = 8192 >> 1;

		/* for (i = 0; i <= 10; i++) {
		 *	L_temp   = GSM_L_MULT( wt[k+i], gsm_H[i] );
		 *	L_result = GSM_L_ADD( L_result, L_temp );
		 * }
		 */

#undef	rpeSTEP
#define	rpeSTEP( i, H )	(e[ k + i ] * (longword)H)

		/*  Every one of these multiplications is done twice --
		 *  but I don't see an elegant way to optimize this. 
		 *  Do you?
		 */

#ifdef	STUPID_COMPILER
		L_result += rpeSTEP(	0, 	-134 ) ;
		L_result += rpeSTEP(	1, 	-374 )  ;
	               /* + rpeSTEP(	2, 	0    )  */
		L_result += rpeSTEP(	3, 	2054 ) ;
		L_result += rpeSTEP(	4, 	5741 ) ;
		L_result += rpeSTEP(	5, 	8192 ) ;
		L_result += rpeSTEP(	6, 	5741 ) ;
		L_result += rpeSTEP(	7, 	2054 ) ;
	 	       /* + rpeSTEP(	8, 	0    )  */
		L_result += rpeSTEP(	9, 	-374 ) ;
		L_result += rpeSTEP(	10, 	-134 ) ;
#else
		L_result +=
		  rpeSTEP(	0, 	-134 ) 
		+ rpeSTEP(	1, 	-374 ) 
	     /* + rpeSTEP(	2, 	0    )  */
		+ rpeSTEP(	3, 	2054 ) 
		+ rpeSTEP(	4, 	5741 ) 
		+ rpeSTEP(	5, 	8192 ) 
		+ rpeSTEP(	6, 	5741 ) 
		+ rpeSTEP(	7, 	2054 ) 
	     /* + rpeSTEP(	8, 	0    )  */
		+ rpeSTEP(	9, 	-374 ) 
		+ rpeSTEP(10, 	-134 )
		;
#endif

		/* L_result = GSM_L_ADD( L_result, L_result ); (* scaling(x2) *)
		 * L_result = GSM_L_ADD( L_result, L_result ); (* scaling(x4) *)
		 *
		 * x[k] = SASR( L_result, 16 );
		 */

		/* 2 adds vs. >>16 => 14, minus one shift to compensate for
		 * those we lost when replacing L_MULT by '*'.
		 */

		L_result = SASR_L( L_result, 13 );
		x[k] =  (  L_result < MIN_WORD ? MIN_WORD
			: (L_result > MAX_WORD ? MAX_WORD : L_result ));
	}
}

/* 4.2.14 */

static void RPE_grid_selection (
	word		* x,		/* [0..39]		IN  */ 
	word		* xM,		/* [0..12]		OUT */
	word		* Mc_out	/*			OUT */
)
/*
 *  The signal x[0..39] is used to select the RPE grid which is
 *  represented by Mc.
 */
{
	/* register word	temp1;	*/
	register int		/* m, */  i;
	register longword	L_result, L_temp;
	longword		EM;	/* xxx should be L_EM? */
	word			Mc;

	longword		L_common_0_3;

	EM = 0;
	Mc = 0;

	/* for (m = 0; m <= 3; m++) {
	 *	L_result = 0;
	 *
	 *
	 *	for (i = 0; i <= 12; i++) {
	 *
	 *		temp1    = SASR_W( x[m + 3*i], 2 );
	 *
	 *		assert(temp1 != MIN_WORD);
	 *
	 *		L_temp   = GSM_L_MULT( temp1, temp1 );
	 *		L_result = GSM_L_ADD( L_temp, L_result );
	 *	}
	 * 
	 *	if (L_result > EM) {
	 *		Mc = m;
	 *		EM = L_result;
	 *	}
	 * }
	 */

#undef	rpeSTEP
#define	rpeSTEP( m, i )		L_temp = SASR_W( x[m + 3 * i], 2 );	\
				L_result += L_temp * L_temp;

	/* common part of 0 and 3 */

	L_result = 0;
	rpeSTEP( 0, 1 ); rpeSTEP( 0, 2 ); rpeSTEP( 0, 3 ); rpeSTEP( 0, 4 );
	rpeSTEP( 0, 5 ); rpeSTEP( 0, 6 ); rpeSTEP( 0, 7 ); rpeSTEP( 0, 8 );
	rpeSTEP( 0, 9 ); rpeSTEP( 0, 10); rpeSTEP( 0, 11); rpeSTEP( 0, 12);
	L_common_0_3 = L_result;

	/* i = 0 */

	rpeSTEP( 0, 0 );
	L_result <<= 1;	/* implicit in L_MULT */
	EM = L_result;

	/* i = 1 */

	L_result = 0;
	rpeSTEP( 1, 0 );
	rpeSTEP( 1, 1 ); rpeSTEP( 1, 2 ); rpeSTEP( 1, 3 ); rpeSTEP( 1, 4 );
	rpeSTEP( 1, 5 ); rpeSTEP( 1, 6 ); rpeSTEP( 1, 7 ); rpeSTEP( 1, 8 );
	rpeSTEP( 1, 9 ); rpeSTEP( 1, 10); rpeSTEP( 1, 11); rpeSTEP( 1, 12);
	L_result <<= 1;
	if (L_result > EM) {
		Mc = 1;
	 	EM = L_result;
	}

	/* i = 2 */

	L_result = 0;
	rpeSTEP( 2, 0 );
	rpeSTEP( 2, 1 ); rpeSTEP( 2, 2 ); rpeSTEP( 2, 3 ); rpeSTEP( 2, 4 );
	rpeSTEP( 2, 5 ); rpeSTEP( 2, 6 ); rpeSTEP( 2, 7 ); rpeSTEP( 2, 8 );
	rpeSTEP( 2, 9 ); rpeSTEP( 2, 10); rpeSTEP( 2, 11); rpeSTEP( 2, 12);
	L_result <<= 1;
	if (L_result > EM) {
		Mc = 2;
	 	EM = L_result;
	}

	/* i = 3 */

	L_result = L_common_0_3;
	rpeSTEP( 3, 12 );
	L_result <<= 1;
	if (L_result > EM) {
		Mc = 3;
	 	EM = L_result;
	}

	/**/

	/*  Down-sampling by a factor 3 to get the selected xM[0..12]
	 *  RPE sequence.
	 */
	for (i = 0; i <= 12; i ++) xM[i] = x[Mc + 3*i];
	*Mc_out = Mc;
}

/* 4.12.15 */

static void APCM_quantization_xmaxc_to_exp_mant (
	word		xmaxc,		/* IN 	*/
	word		* expon_out,	/* OUT	*/
	word		* mant_out )	/* OUT  */
{
	word	expon, mant;

	/* Compute expononent and mantissa of the decoded version of xmaxc
	 */

	expon = 0;
	if (xmaxc > 15) expon = SASR_W(xmaxc, 3) - 1;
	mant = xmaxc - (expon << 3);

	if (mant == 0) {
		expon  = -4;
		mant = 7;
	}
	else {
		while (mant <= 7) {
			mant = mant << 1 | 1;
			expon--;
		}
		mant -= 8;
	}

	assert( expon  >= -4 && expon <= 6 );
	assert( mant >= 0 && mant <= 7 );

	*expon_out  = expon;
	*mant_out = mant;
}

static void APCM_quantization (
	word		* xM,		/* [0..12]		IN	*/
	word		* xMc,		/* [0..12]		OUT	*/
	word		* mant_out,	/* 			OUT	*/
	word		* expon_out,	/*			OUT	*/
	word		* xmaxc_out	/*			OUT	*/
)
{
	int	i, itest;

	word	xmax, xmaxc, temp, temp1, temp2;
	word	expon, mant;


	/*  Find the maximum absolute value xmax of xM[0..12].
	 */

	xmax = 0;
	for (i = 0; i <= 12; i++) {
		temp = xM[i];
		temp = GSM_ABS(temp);
		if (temp > xmax) xmax = temp;
	}

	/*  Qantizing and coding of xmax to get xmaxc.
	 */

	expon   = 0;
	temp  = SASR_W( xmax, 9 );
	itest = 0;

	for (i = 0; i <= 5; i++) {

		itest |= (temp <= 0);
		temp = SASR_W( temp, 1 );

		assert(expon <= 5);
		if (itest == 0) expon++;		/* expon = add (expon, 1) */
	}

	assert(expon <= 6 && expon >= 0);
	temp = expon + 5;

	assert(temp <= 11 && temp >= 0);
	xmaxc = gsm_add( SASR_W(xmax, temp), (word) (expon << 3) );

	/*   Quantizing and coding of the xM[0..12] RPE sequence
	 *   to get the xMc[0..12]
	 */

	APCM_quantization_xmaxc_to_exp_mant( xmaxc, &expon, &mant );

	/*  This computation uses the fact that the decoded version of xmaxc
	 *  can be calculated by using the expononent and the mantissa part of
	 *  xmaxc (logarithmic table).
	 *  So, this method avoids any division and uses only a scaling
	 *  of the RPE samples by a function of the expononent.  A direct 
	 *  multiplication by the inverse of the mantissa (NRFAC[0..7]
	 *  found in table 4.5) gives the 3 bit coded version xMc[0..12]
	 *  of the RPE samples.
	 */


	/* Direct computation of xMc[0..12] using table 4.5
	 */

	assert( expon <= 4096 && expon >= -4096);
	assert( mant >= 0 && mant <= 7 ); 

	temp1 = 6 - expon;		/* normalization by the expononent */
	temp2 = gsm_NRFAC[ mant ];  	/* inverse mantissa 		 */

	for (i = 0; i <= 12; i++) {

		assert(temp1 >= 0 && temp1 < 16);

		temp = xM[i] << temp1;
		temp = GSM_MULT( temp, temp2 );
		temp = SASR_W(temp, 12);
		xMc[i] = temp + 4;		/* see note below */
	}

	/*  NOTE: This equation is used to make all the xMc[i] positive.
	 */

	*mant_out  = mant;
	*expon_out   = expon;
	*xmaxc_out = xmaxc;
}

/* 4.2.16 */

static void APCM_inverse_quantization (
	register word	* xMc,	/* [0..12]			IN 	*/
	word		mant,
	word		expon,
	register word	* xMp)	/* [0..12]			OUT 	*/
/* 
 *  This part is for decoding the RPE sequence of coded xMc[0..12]
 *  samples to obtain the xMp[0..12] array.  Table 4.6 is used to get
 *  the mantissa of xmaxc (FAC[0..7]).
 */
{
	int	i;
	word	temp, temp1, temp2, temp3;

	assert( mant >= 0 && mant <= 7 ); 

	temp1 = gsm_FAC[ mant ];	/* see 4.2-15 for mant */
	temp2 = gsm_sub( 6, expon );	/* see 4.2-15 for exp  */
	temp3 = gsm_asl( 1, gsm_sub( temp2, 1 ));

	for (i = 13; i--;) {

		assert( *xMc <= 7 && *xMc >= 0 ); 	/* 3 bit unsigned */

		/* temp = gsm_sub( *xMc++ << 1, 7 ); */
		temp = (*xMc++ << 1) - 7;	        /* restore sign   */
		assert( temp <= 7 && temp >= -7 ); 	/* 4 bit signed   */

		temp <<= 12;				/* 16 bit signed  */
		temp = GSM_MULT_R( temp1, temp );
		temp = GSM_ADD( temp, temp3 );
		*xMp++ = gsm_asr( temp, temp2 );
	}
}

/* 4.2.17 */

static void RPE_grid_positioning (
	word		Mc,		/* grid position	IN	*/
	register word	* xMp,		/* [0..12]		IN	*/
	register word	* ep		/* [0..39]		OUT	*/
)
/*
 *  This procedure computes the reconstructed long term residual signal
 *  ep[0..39] for the LTP analysis filter.  The inputs are the Mc
 *  which is the grid position selection and the xMp[0..12] decoded
 *  RPE samples which are upsampled by a factor of 3 by inserting zero
 *  values.
 */
{
	int	i = 13;

	assert(0 <= Mc && Mc <= 3);

        switch (Mc) {
                case 3: *ep++ = 0;
                case 2:  do {
                                *ep++ = 0;
                case 1:         *ep++ = 0;
                case 0:         *ep++ = *xMp++;
                         } while (--i);
        }
        while (++Mc < 4) *ep++ = 0;

	/*

	int i, k;
	for (k = 0; k <= 39; k++) ep[k] = 0;
	for (i = 0; i <= 12; i++) {
		ep[ Mc + (3*i) ] = xMp[i];
	}
	*/
}

/* 4.2.18 */

/*  This procedure adds the reconstructed long term residual signal
 *  ep[0..39] to the estimated signal dpp[0..39] from the long term
 *  analysis filter to compute the reconstructed short term residual
 *  signal dp[-40..-1]; also the reconstructed short term residual
 *  array dp[-120..-41] is updated.
 */

#if 0	/* Has been inlined in code.c */
void Gsm_Update_of_reconstructed_short_time_residual_signal (
	word	* dpp,		/* [0...39]	IN	*/
	word	* ep,		/* [0...39]	IN	*/
	word	* dp)		/* [-120...-1]  IN/OUT 	*/
{
	int 		k;

	for (k = 0; k <= 79; k++) 
		dp[ -120 + k ] = dp[ -80 + k ];

	for (k = 0; k <= 39; k++)
		dp[ -40 + k ] = gsm_add( ep[k], dpp[k] );
}
#endif	/* Has been inlined in code.c */

void Gsm_RPE_Encoding (
	/*-struct gsm_state * S,-*/

	word	* e,		/* -5..-1][0..39][40..44	IN/OUT  */
	word	* xmaxc,	/* 				OUT */
	word	* Mc,		/* 			  	OUT */
	word	* xMc)		/* [0..12]			OUT */
{
	word	x[40];
	word	xM[13], xMp[13];
	word	mant, expon;

	Weighting_filter(e, x);
	RPE_grid_selection(x, xM, Mc);

	APCM_quantization(	xM, xMc, &mant, &expon, xmaxc);
	APCM_inverse_quantization(  xMc,  mant,  expon, xMp);

	RPE_grid_positioning( *Mc, xMp, e );

}

void Gsm_RPE_Decoding (
	/*-struct gsm_state	* S,-*/

	word 		xmaxcr,
	word		Mcr,
	word		* xMcr,  /* [0..12], 3 bits 		IN	*/
	word		* erp	 /* [0..39]			OUT 	*/
)
{
	word	expon, mant;
	word	xMp[ 13 ];

	APCM_quantization_xmaxc_to_exp_mant( xmaxcr, &expon, &mant );
	APCM_inverse_quantization( xMcr, mant, expon, xMp );
	RPE_grid_positioning( Mcr, xMp, erp );

}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 82005b9e-1560-4e94-9ddb-00cb14867295
*/

/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<stdarg.h>


#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
rx2_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* rx2_open */

#else

/*------------------------------------------------------------------------------
 * Macros to handle big/little endian issues.
*/

#define	CAT_MARKER	(MAKE_MARKER ('C', 'A', 'T', ' '))
#define	GLOB_MARKER (MAKE_MARKER ('G', 'L', 'O', 'B'))

#define	RECY_MARKER (MAKE_MARKER ('R', 'E', 'C', 'Y'))

#define	SLCL_MARKER (MAKE_MARKER ('S', 'L', 'C', 'L'))
#define	SLCE_MARKER (MAKE_MARKER ('S', 'L', 'C', 'E'))

#define	DEVL_MARKER	(MAKE_MARKER ('D', 'E', 'V', 'L'))
#define	TRSH_MARKER	(MAKE_MARKER ('T', 'R', 'S', 'H'))

#define	EQ_MARKER	(MAKE_MARKER ('E', 'Q', ' ', ' '))
#define	COMP_MARKER (MAKE_MARKER ('C', 'O', 'M', 'P'))

#define	SINF_MARKER (MAKE_MARKER ('S', 'I', 'N', 'F'))
#define	SDAT_MARKER (MAKE_MARKER ('S', 'D', 'A', 'T'))

/*------------------------------------------------------------------------------
 * Typedefs for file chunks.
*/


/*------------------------------------------------------------------------------
 * Private static functions.
*/
static int	rx2_close	(SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public functions.
*/

int
rx2_open	(SF_PRIVATE *psf)
{	static char *marker_type [4] =
	{	"Original Enabled", "Enabled Hidden",
		"Additional/PencilTool", "Disabled"
		} ;

	int error, marker, length, glob_offset, slce_count, frames ;

	int sdat_length = 0, slce_total = 0 ;

	int n_channels ;


	/* So far only doing read. */

	psf_binheader_readf (psf, "Epm4", 0, &marker, &length) ;

	if (marker != CAT_MARKER)
	{	psf_log_printf (psf, "length : %d\n", length) ;
		return -1000 ;
		} ;

	if (length != psf->filelength - 8)
		psf_log_printf (psf, "%M : %d (should be %d)\n", marker, length, psf->filelength - 8) ;
	else
		psf_log_printf (psf, "%M : %d\n", marker, length) ;

	/* 'REX2' marker */
	psf_binheader_readf (psf, "m", &marker) ;
	psf_log_printf (psf, "%M", marker) ;

	/* 'HEAD' marker */
	psf_binheader_readf (psf, "m", &marker) ;
	psf_log_printf (psf, "%M\n", marker) ;

	/* Grab 'GLOB' offset. */
	psf_binheader_readf (psf, "E4", &glob_offset) ;
	glob_offset += 0x14 ;	/* Add the current file offset. */

	/* Jump to offset 0x30 */
	psf_binheader_readf (psf, "p", 0x30) ;

	/* Get name length */
	length = 0 ;
	psf_binheader_readf (psf, "1", &length) ;
	if (length >= SIGNED_SIZEOF (psf->buffer))
	{	psf_log_printf (psf, "  Text : %d *** Error : Too sf_count_t!\n") ;
		return -1001 ;
		}

	memset (psf->buffer, 0, SIGNED_SIZEOF (psf->buffer)) ;
	psf_binheader_readf (psf, "b", psf->buffer, length) ;
	psf_log_printf (psf, " Text : \"%s\"\n", psf->buffer) ;

	/* Jump to GLOB offset position. */
	if (glob_offset & 1)
		glob_offset ++ ;

	psf_binheader_readf (psf, "p", glob_offset) ;

	slce_count = 0 ;
	/* GLOB */
	while (1)
	{	psf_binheader_readf (psf, "m", &marker) ;

		if (marker != SLCE_MARKER && slce_count > 0)
		{	psf_log_printf (psf, "   SLCE count : %d\n", slce_count) ;
			slce_count = 0 ;
			}
		switch (marker)
		{	case GLOB_MARKER:
					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, " %M : %d\n", marker, length) ;
					psf_binheader_readf (psf, "j", length) ;
					break ;

			case RECY_MARKER:
					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, " %M : %d\n", marker, length) ;
					psf_binheader_readf (psf, "j", (length+1) & 0xFFFFFFFE) ; /* ?????? */
					break ;

			case CAT_MARKER:
					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, " %M : %d\n", marker, length) ;
					/*-psf_binheader_readf (psf, "j", length) ;-*/
					break ;

			case DEVL_MARKER:
					psf_binheader_readf (psf, "mE4", &marker, &length) ;
					psf_log_printf (psf, "  DEVL%M : %d\n", marker, length) ;
					if (length & 1)
						length ++ ;
					psf_binheader_readf (psf, "j", length) ;
					break ;

			case EQ_MARKER:
			case COMP_MARKER:
					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, "   %M : %d\n", marker, length) ;
					/* This is weird!!!! why make this (length - 1) */
					if (length & 1)
						length ++ ;
					psf_binheader_readf (psf, "j", length) ;
					break ;

			case SLCL_MARKER:
					psf_log_printf (psf, "  %M\n    (Offset, Next Offset, Type)\n", marker) ;
					slce_count = 0 ;
					break ;

			case SLCE_MARKER:
					{	int len [4], indx ;

						psf_binheader_readf (psf, "E4444", &len [0], &len [1], &len [2], &len [3]) ;

						indx = ((len [3] & 0x0000FFFF) >> 8) & 3 ;

						if (len [2] == 1)
						{	if (indx != 1)
								indx = 3 ;	/* 2 cases, where next slice offset = 1 -> disabled & enabled/hidden */

							psf_log_printf (psf, "   %M : (%6d, ?: 0x%X, %s)\n", marker, len [1], (len [3] & 0xFFFF0000) >> 16, marker_type [indx]) ;
							}
						else
						{	slce_total += len [2] ;

							psf_log_printf (psf, "   %M : (%6d, SLCE_next_ofs:%d, ?: 0x%X, %s)\n", marker, len [1], len [2], (len [3] & 0xFFFF0000) >> 16, marker_type [indx]) ;
							} ;

						slce_count ++ ;
						} ;
					break ;

			case SINF_MARKER:
					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, " %M : %d\n", marker, length) ;

					psf_binheader_readf (psf, "E2", &n_channels) ;
					n_channels = (n_channels & 0x0000FF00) >> 8 ;
					psf_log_printf (psf, "  Channels    : %d\n", n_channels) ;

					psf_binheader_readf (psf, "E44", &psf->sf.samplerate, &frames) ;
					psf->sf.frames = frames ;
					psf_log_printf (psf, "  Sample Rate : %d\n", psf->sf.samplerate) ;
					psf_log_printf (psf, "  Frames      : %D\n", psf->sf.frames) ;

					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, "  ??????????? : %d\n", length) ;

					psf_binheader_readf (psf, "E4", &length) ;
					psf_log_printf (psf, "  ??????????? : %d\n", length) ;
					break ;

			case SDAT_MARKER:
					psf_binheader_readf (psf, "E4", &length) ;

				sdat_length = length ;

					/* Get the current offset. */
					psf->dataoffset = psf_binheader_readf (psf, NULL) ;

					if (psf->dataoffset + length != psf->filelength)
						psf_log_printf (psf, " %M : %d (should be %d)\n", marker, length, psf->dataoffset + psf->filelength) ;
					else
						psf_log_printf (psf, " %M : %d\n", marker, length) ;
					break ;

			default :
					psf_log_printf (psf, "Unknown marker : 0x%X %M", marker, marker) ;
					return -1003 ;
					break ;
			} ;

		/* SDAT always last marker in file. */
		if (marker == SDAT_MARKER)
			break ;
		} ;

	puts (psf->logbuffer) ;
	puts ("-----------------------------------") ;

	printf ("SDAT length  : %d\n", sdat_length) ;
	printf ("SLCE count   : %d\n", slce_count) ;

	/* Hack for zero slice count. */
	if (slce_count == 0 && slce_total == 1)
		slce_total = frames ;

	printf ("SLCE samples : %d\n", slce_total) ;

	/* Two bytes per sample. */
	printf ("Comp Ratio   : %f:1\n", (2.0 * slce_total * n_channels) / sdat_length) ;

	puts (" ") ;

	psf->logbuffer [0] = 0 ;

	/* OK, have the header although not too sure what it all means. */

	psf->endian = SF_ENDIAN_BIG ;

	psf->datalength = psf->filelength - psf->dataoffset ;

 	if (psf_fseek (psf, psf->dataoffset, SEEK_SET))
		return SFE_BAD_SEEK ;

	psf->sf.format = (SF_FORMAT_REX2 | SF_FORMAT_DWVW_12) ;

	psf->sf.channels	= 1 ;
	psf->bytewidth		= 2 ;
	psf->blockwidth		= psf->sf.channels * psf->bytewidth ;

	if ((error = dwvw_init (psf, 16)))
		return error ;

	psf->close = rx2_close ;

	if (! psf->sf.frames && psf->blockwidth)
		psf->sf.frames = psf->datalength / psf->blockwidth ;

	/* All done. */

	return 0 ;
} /* rx2_open */

/*------------------------------------------------------------------------------
*/

static int
rx2_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE)
	{	/*  Now we know for certain the length of the file we can re-write
		**	correct values for the FORM, 8SVX and BODY chunks.
		*/

		} ;

	return 0 ;
} /* rx2_close */

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 7366e813-9fee-4d1f-881e-e4a691469370
*/
/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<string.h>



#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
sd2_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* sd2_open */

#else

/*------------------------------------------------------------------------------
 * Macros to handle big/little endian issues.
*/

#define	Sd2f_MARKER		MAKE_MARKER ('S', 'd', '2', 'f')

/*------------------------------------------------------------------------------
 * Typedefs for file chunks.
*/



/*------------------------------------------------------------------------------
 * Private static functions.
*/

static int	sd2_close	(SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public functions.
*/

int
sd2_open	(SF_PRIVATE *psf)
{	int		marker, software, rsrc_offset, len ;
	int 	rsrc_data_offset, rsrc_map_offset, rsrc_data_length, rsrc_map_length ;
	char	slen ;
	float	srate ;

	/* Read only so far. */

	psf_binheader_readf (psf, "Epmmj", 0x41, &marker, &software, 14) ;

	if (marker != Sd2f_MARKER)
	{	printf ("Whoops!!!\n") ;
		puts (psf->logbuffer) ;
		return SFE_INTERNAL ;
		} ;

	psf_log_printf (psf, "Marker   : %M\n"
						 "Software : %M\n",
			marker, software) ;

	/* This seems to be a constant for binhex files. */
	psf->dataoffset = 0x80 ;

	/* All SD2 files are big endian. */
	psf->endian= SF_ENDIAN_BIG ;

	/*
	**	Resource header info from:
	**	http://developer.apple.com/techpubs/mac/MoreToolbox/MoreToolbox-99.html
	*/

	rsrc_offset = psf->datalength + psf->dataoffset ;
	if (rsrc_offset & 0x7F)
		rsrc_offset = rsrc_offset - (rsrc_offset & 0x7F) + psf->dataoffset ;

	psf_log_printf (psf, "Resource offset : 0x%X\n", rsrc_offset) ;

	/* Jump to the rsrc_offset fork section. */
	psf_binheader_readf (psf, "Ep", rsrc_offset) ;

	psf_binheader_readf (psf, "E4444", &rsrc_data_offset, &rsrc_map_offset, &rsrc_data_length, &rsrc_map_length) ;

	rsrc_data_offset += rsrc_offset ;
	rsrc_map_offset	+= rsrc_offset ;

	psf_log_printf (psf, " data offset : 0x%X\n"
						 " map  offset : 0x%X\n"
						 " data length : 0x%X\n"
						 " map  length : 0x%X\n",

			rsrc_data_offset, rsrc_map_offset, rsrc_data_length, rsrc_map_length) ;

	if (rsrc_data_offset + rsrc_data_length > rsrc_map_offset || rsrc_map_offset + rsrc_map_length > psf->filelength)
	{	puts ("##############################") ;
		puts (psf->logbuffer) ;
		puts ("##############################") ;
		return SFE_INTERNAL ;
		} ;

	memset (psf->buffer, 0, sizeof (psf->buffer)) ;

	psf_binheader_readf (psf, "Ep41", rsrc_data_offset, &len, &slen) ;
	if (slen + 1 == len)
	{	psf_binheader_readf (psf, "Eb", psf->buffer, len - 1) ;
		((char*) psf->buffer) [len - 1] = 0 ;
		if (sscanf ((char*) psf->buffer, "%d", &len) == 1)
			psf->bytewidth = len ;
		} ;

	psf_binheader_readf (psf, "E41", &len, &slen) ;
	if (slen + 1 == len)
	{	psf_binheader_readf (psf, "Eb", psf->buffer, len - 1) ;
		((char*) psf->buffer) [len - 1] = 0 ;
		if (sscanf ((char*) psf->buffer, "%f", &srate) == 1)
			psf->sf.samplerate = srate ;
		} ;

	psf_binheader_readf (psf, "E41", &len, &slen) ;
	if (slen + 1 == len)
	{	psf_binheader_readf (psf, "Eb", psf->buffer, len - 1) ;
		((char*) psf->buffer) [len - 1] = 0 ;
		if (sscanf ((char*) psf->buffer, "%d", &len) == 1)
			psf->sf.channels = len ;
		} ;

	psf_log_printf (psf, "  byte width  : %d\n", psf->bytewidth) ;
	psf_log_printf (psf, "  sample rate : %d\n", psf->sf.samplerate) ;
	psf_log_printf (psf, "  channels    : %d\n", psf->sf.channels) ;

	if (psf->bytewidth == 2)
	{	psf->sf.format = SF_FORMAT_SD2 | SF_FORMAT_PCM_16 ;

		psf->blockwidth = psf->bytewidth * psf->sf.channels ;

		psf->sf.frames = psf->datalength / psf->blockwidth ;
		} ;

	pcm_init (psf) ;

	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	psf->close = sd2_close ;

	return 0 ;
} /* sd2_open */

/*------------------------------------------------------------------------------
*/

static int
sd2_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE)
	{	/*  Now we know for certain the audio_length of the file we can re-write
		**	correct values for the FORM, 8SVX and BODY chunks.
		*/

		} ;

	return 0 ;
} /* sd2_close */

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 1ee183e5-6b9f-4c2c-bd0a-24f35595cefc
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>


/*------------------------------------------------------------------------------
*/

#define	SDS_DATA_OFFSET				0x15
#define SDS_BLOCK_SIZE				127

#define SDS_AUDIO_BYTES_PER_BLOCK	120

#define SDS_3BYTE_TO_INT_DECODE(x) (((x) & 0x7F) | (((x) & 0x7F00) >> 1) | (((x) & 0x7F0000) >> 2))
#define SDS_INT_TO_3BYTE_ENCODE(x) (((x) & 0x7F) | (((x) << 1) & 0x7F00) | (((x) << 2) & 0x7F0000))

/*------------------------------------------------------------------------------
** Typedefs.
*/

typedef struct tag_SDS_PRIVATE
{	int bitwidth, frames ;
	int	samplesperblock, total_blocks ;

	int (*reader) (SF_PRIVATE *psf, struct tag_SDS_PRIVATE *psds) ;
	int (*writer) (SF_PRIVATE *psf, struct tag_SDS_PRIVATE *psds) ;

	int read_block, read_count ;
	unsigned char read_data [SDS_BLOCK_SIZE] ;
	int	read_samples [SDS_BLOCK_SIZE / 2] ; /* Maximum samples per block */

	int write_block, write_count ;
	unsigned char write_data [SDS_BLOCK_SIZE] ;
	int	write_samples [SDS_BLOCK_SIZE / 2] ; /* Maximum samples per block */
} SDS_PRIVATE ;

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int	sds_close	(SF_PRIVATE *psf) ;

static int	sds_write_header (SF_PRIVATE *psf, int calc_length) ;
static int	sds_read_header (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;

static int	sds_init (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;

static sf_count_t sds_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t sds_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t sds_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t sds_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t sds_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t sds_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t sds_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t sds_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t sds_seek (SF_PRIVATE *psf, int mode, sf_count_t offset) ;

static int sds_2byte_read (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;
static int sds_3byte_read (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;
static int sds_4byte_read (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;

static int sds_read (SF_PRIVATE *psf, SDS_PRIVATE *psds, int *iptr, int readcount) ;

static int sds_2byte_write (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;
static int sds_3byte_write (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;
static int sds_4byte_write (SF_PRIVATE *psf, SDS_PRIVATE *psds) ;

static int sds_write (SF_PRIVATE *psf, SDS_PRIVATE *psds, int *iptr, int writecount) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
sds_open	(SF_PRIVATE *psf)
{	SDS_PRIVATE	*psds ;
	int			subformat, error = 0 ;

	/* Hmmmm, need this here to pass update_header_test. */
	psf->sf.frames = 0 ;

	if (! (psds = calloc (1, sizeof (SDS_PRIVATE))))
		return SFE_MALLOC_FAILED ;
	psf->fdata = psds ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = sds_read_header (psf, psds)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_SDS)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (sds_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = sds_write_header ;

		psf_fseek (psf, SDS_DATA_OFFSET, SEEK_SET) ;
		} ;

	if ((error = sds_init (psf, psds)) != 0)
		return error ;

	psf->seek = sds_seek ;
	psf->close = sds_close ;

	psf->blockwidth = 0 ;

	return error ;
} /* sds_open */

/*------------------------------------------------------------------------------
*/

static int
sds_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	SDS_PRIVATE *psds ;

		if ((psds = (SDS_PRIVATE *) psf->fdata) == NULL)
		{	psf_log_printf (psf, "*** Bad psf->fdata ptr.\n") ;
			return SFE_INTERNAL ;
			} ;

		if (psds->write_count > 0)
		{	memset (&(psds->write_data [psds->write_count]), 0, (psds->samplesperblock - psds->write_count) * sizeof (int)) ;
			psds->writer (psf, psds) ;
			} ;

		sds_write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* sds_close */

static int
sds_init (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{
	if (psds->bitwidth < 8 || psds->bitwidth > 28)
		return (psf->error = SFE_SDS_BAD_BIT_WIDTH) ;

	if (psds->bitwidth < 14)
	{	psds->reader = sds_2byte_read ;
		psds->writer = sds_2byte_write ;
		psds->samplesperblock = SDS_AUDIO_BYTES_PER_BLOCK / 2 ;
		}
	else if (psds->bitwidth < 21)
	{	psds->reader = sds_3byte_read ;
		psds->writer = sds_3byte_write ;
		psds->samplesperblock = SDS_AUDIO_BYTES_PER_BLOCK / 3 ;
		}
	else
	{	psds->reader = sds_4byte_read ;
		psds->writer = sds_4byte_write ;
		psds->samplesperblock = SDS_AUDIO_BYTES_PER_BLOCK / 4 ;
		} ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	psf->read_short		= sds_read_s ;
		psf->read_int		= sds_read_i ;
		psf->read_float		= sds_read_f ;
		psf->read_double	= sds_read_d ;

		/* Read first block. */
		psds->reader (psf, psds) ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	psf->write_short	= sds_write_s ;
		psf->write_int		= sds_write_i ;
		psf->write_float	= sds_write_f ;
		psf->write_double	= sds_write_d ;
		} ;

	return 0 ;
} /* sds_init */

static int
sds_read_header (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char	channel, bitwidth, loop_type, byte ;
	unsigned short	sample_no, marker ;
	unsigned int	samp_period, data_length, sustain_loop_start, sustain_loop_end ;
	int		bytesread, blockcount ;

	/* Set position to start of file to begin reading header. */
	bytesread = psf_binheader_readf (psf, "pE211", 0, &marker, &channel, &byte) ;

	if (marker != 0xF07E || byte != 0x01)
		return SFE_SDS_NOT_SDS ;

	psf_log_printf (psf, "Midi Sample Dump Standard (.sds)\nF07E\n Midi Channel  : %d\n", channel) ;

	bytesread += psf_binheader_readf (psf, "e213", &sample_no, &bitwidth, &samp_period) ;

	sample_no = SDS_3BYTE_TO_INT_DECODE (sample_no) ;
	samp_period = SDS_3BYTE_TO_INT_DECODE (samp_period) ;

	psds->bitwidth = bitwidth ;

	psf->sf.samplerate = 1000000000 / samp_period ;

	psf_log_printf (psf, 	" Sample Number : %d\n"
							" Bit Width     : %d\n"
							" Sample Rate   : %d\n",
			sample_no, psds->bitwidth, psf->sf.samplerate) ;

	bytesread += psf_binheader_readf (psf, "e3331", &data_length, &sustain_loop_start, &sustain_loop_end, &loop_type) ;

	data_length = SDS_3BYTE_TO_INT_DECODE (data_length) ;

	sustain_loop_start = SDS_3BYTE_TO_INT_DECODE (sustain_loop_start) ;
	sustain_loop_end = SDS_3BYTE_TO_INT_DECODE (sustain_loop_end) ;

	psf_log_printf (psf, 	" Sustain Loop\n"
							"     Start     : %d\n"
							"     End       : %d\n"
							"     Loop Type : %d\n",
			sustain_loop_start, sustain_loop_end, loop_type) ;

	psf->dataoffset = SDS_DATA_OFFSET ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	if (data_length != psf->filelength - psf->dataoffset)
	{	psf_log_printf (psf, " Datalength     : %d (truncated data??? %d)\n", data_length, psf->filelength - psf->dataoffset) ;
		data_length = psf->filelength - psf->dataoffset ;
		}
	else
		psf_log_printf (psf, " Datalength     : %d\n", data_length) ;

	bytesread += psf_binheader_readf (psf, "1", &byte) ;
	if (byte != 0xF7)
		psf_log_printf (psf, "bad end : %X\n", byte & 0xFF) ;

	for (blockcount = 0 ; bytesread < psf->filelength ; blockcount++)
	{
		bytesread += psf_fread (&marker, 1, 2, psf) ;

		if (marker == 0)
			break ;

		psf_fseek (psf, SDS_BLOCK_SIZE - 2, SEEK_CUR) ;
		bytesread += SDS_BLOCK_SIZE - 2 ;
		} ;

	psf_log_printf (psf, "\nBlocks         : %d\n", blockcount) ;
	psds->total_blocks = blockcount ;

	psds->samplesperblock = SDS_AUDIO_BYTES_PER_BLOCK / ((psds->bitwidth + 6) / 7) ;
	psf_log_printf (psf, "Samples/Block  : %d\n", psds->samplesperblock) ;

	psf_log_printf (psf, "Frames         : %d\n", blockcount * psds->samplesperblock) ;

	psf->sf.frames = blockcount * psds->samplesperblock ;
	psds->frames = blockcount * psds->samplesperblock ;

	/* Always Mono */
	psf->sf.channels = 1 ;
	psf->sf.sections = 1 ;

	/*
	** Lie to the user about PCM bit width. Always round up to
	** the next multiple of 8.
	*/
	switch ((psds->bitwidth + 7) / 8)
	{	case 1 :
			psf->sf.format = SF_FORMAT_SDS | SF_FORMAT_PCM_S8 ;
			break ;

		case 2 :
			psf->sf.format = SF_FORMAT_SDS | SF_FORMAT_PCM_16 ;
			break ;

		case 3 :
			psf->sf.format = SF_FORMAT_SDS | SF_FORMAT_PCM_24 ;
			break ;

		case 4 :
			psf->sf.format = SF_FORMAT_SDS | SF_FORMAT_PCM_32 ;
			break ;

		default :
			psf_log_printf (psf, "*** Weird byte width (%d)\n", (psds->bitwidth + 7) / 8) ;
			return SFE_SDS_BAD_BIT_WIDTH ;
		} ;

	psf_fseek (psf, SDS_DATA_OFFSET, SEEK_SET) ;

	return 0 ;
} /* sds_read_header */

static int
sds_write_header (SF_PRIVATE *psf, int calc_length)
{	SDS_PRIVATE *psds ;
	sf_count_t	current ;
	int samp_period, data_length, sustain_loop_start, sustain_loop_end ;
	unsigned char loop_type = 0 ;

	if ((psds = (SDS_PRIVATE *) psf->fdata) == NULL)
	{	psf_log_printf (psf, "*** Bad psf->fdata ptr.\n") ;
		return SFE_INTERNAL ;
		} ;

	if (psf->pipeoffset > 0)
		return 0 ;

	current = psf_ftell (psf) ;

	if (calc_length)
		psf->sf.frames = psds->total_blocks * psds->samplesperblock + psds->write_count ;

	if (psds->write_count > 0)
	{	int current_count = psds->write_count ;
		int current_block = psds->write_block ;

		psds->writer (psf, psds) ;

		psf_fseek (psf, -1 * SDS_BLOCK_SIZE, SEEK_CUR) ;

		psds->write_count = current_count ;
		psds->write_block = current_block ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	if (psf->is_pipe == SF_FALSE)
		psf_fseek (psf, 0, SEEK_SET) ;

	psf_binheader_writef (psf, "E211", 0xF07E, 0, 1) ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
				psds->bitwidth = 8 ;
				break ;
		case SF_FORMAT_PCM_16 :
				psds->bitwidth = 16 ;
				break ;
		case SF_FORMAT_PCM_24 :
				psds->bitwidth = 24 ;
				break ;
		default:
			return SFE_SDS_BAD_BIT_WIDTH ;
		} ;

	samp_period = SDS_INT_TO_3BYTE_ENCODE (1000000000 / psf->sf.samplerate) ;

	psf_binheader_writef (psf, "e213", 0, psds->bitwidth, samp_period) ;

	data_length			= SDS_INT_TO_3BYTE_ENCODE (psds->total_blocks * SDS_BLOCK_SIZE) ;
	sustain_loop_start	= SDS_INT_TO_3BYTE_ENCODE (0) ;
	sustain_loop_end	= SDS_INT_TO_3BYTE_ENCODE (psf->sf.frames) ;

	psf_binheader_writef (psf, "e33311", data_length, sustain_loop_start, sustain_loop_end, loop_type, 0xF7) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;
	psf->datalength = psds->write_block * SDS_BLOCK_SIZE ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* sds_write_header */


/*------------------------------------------------------------------------------
*/

static int
sds_2byte_read (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char *ucptr, checksum ;
	unsigned int sample ;
	int 	k ;

	psds->read_block ++ ;
	psds->read_count = 0 ;

	if (psds->read_block * psds->samplesperblock > psds->frames)
	{	memset (psds->read_samples, 0, psds->samplesperblock * sizeof (int)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (psds->read_data, 1, SDS_BLOCK_SIZE, psf)) != SDS_BLOCK_SIZE)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, SDS_BLOCK_SIZE) ;

	if (psds->read_data [0] != 0xF0)
	{	printf ("Error A : %02X\n", psds->read_data [0] & 0xFF) ;
		} ;

	checksum = psds->read_data [1] ;
	if (checksum != 0x7E)
	{	printf ("Error 1 : %02X\n", checksum & 0xFF) ;
		}

	for (k = 2 ; k < SDS_BLOCK_SIZE - 3 ; k ++)
		checksum ^= psds->read_data [k] ;

	checksum &= 0x7F ;

	if (checksum != psds->read_data [SDS_BLOCK_SIZE - 2])
	{	psf_log_printf (psf, "Block %d : checksum is %02X should be %02X\n", psds->read_data [4], checksum, psds->read_data [SDS_BLOCK_SIZE - 2]) ;
		} ;

	ucptr = psds->read_data + 5 ;
	for (k = 0 ; k < 120 ; k += 2)
	{	sample = (ucptr [k] << 25) + (ucptr [k + 1] << 18) ;
		psds->read_samples [k / 2] = (int) (sample - 0x80000000) ;
		} ;

	return 1 ;
} /* sds_2byte_read */

static int
sds_3byte_read (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char *ucptr, checksum ;
	unsigned int sample ;
	int 	k ;

	psds->read_block ++ ;
	psds->read_count = 0 ;

	if (psds->read_block * psds->samplesperblock > psds->frames)
	{	memset (psds->read_samples, 0, psds->samplesperblock * sizeof (int)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (psds->read_data, 1, SDS_BLOCK_SIZE, psf)) != SDS_BLOCK_SIZE)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, SDS_BLOCK_SIZE) ;

	if (psds->read_data [0] != 0xF0)
	{	printf ("Error A : %02X\n", psds->read_data [0] & 0xFF) ;
		} ;

	checksum = psds->read_data [1] ;
	if (checksum != 0x7E)
	{	printf ("Error 1 : %02X\n", checksum & 0xFF) ;
		}

	for (k = 2 ; k < SDS_BLOCK_SIZE - 3 ; k ++)
		checksum ^= psds->read_data [k] ;

	checksum &= 0x7F ;

	if (checksum != psds->read_data [SDS_BLOCK_SIZE - 2])
	{	psf_log_printf (psf, "Block %d : checksum is %02X should be %02X\n", psds->read_data [4], checksum, psds->read_data [SDS_BLOCK_SIZE - 2]) ;
		} ;

	ucptr = psds->read_data + 5 ;
	for (k = 0 ; k < 120 ; k += 3)
	{	sample = (ucptr [k] << 25) + (ucptr [k + 1] << 18) + (ucptr [k + 2] << 11) ;
		psds->read_samples [k / 3] = (int) (sample - 0x80000000) ;
		} ;

	return 1 ;
} /* sds_3byte_read */

static int
sds_4byte_read (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char *ucptr, checksum ;
	unsigned int sample ;
	int 	k ;

	psds->read_block ++ ;
	psds->read_count = 0 ;

	if (psds->read_block * psds->samplesperblock > psds->frames)
	{	memset (psds->read_samples, 0, psds->samplesperblock * sizeof (int)) ;
		return 1 ;
		} ;

	if ((k = psf_fread (psds->read_data, 1, SDS_BLOCK_SIZE, psf)) != SDS_BLOCK_SIZE)
		psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, SDS_BLOCK_SIZE) ;

	if (psds->read_data [0] != 0xF0)
	{	printf ("Error A : %02X\n", psds->read_data [0] & 0xFF) ;
		} ;

	checksum = psds->read_data [1] ;
	if (checksum != 0x7E)
	{	printf ("Error 1 : %02X\n", checksum & 0xFF) ;
		}

	for (k = 2 ; k < SDS_BLOCK_SIZE - 3 ; k ++)
		checksum ^= psds->read_data [k] ;

	checksum &= 0x7F ;

	if (checksum != psds->read_data [SDS_BLOCK_SIZE - 2])
	{	psf_log_printf (psf, "Block %d : checksum is %02X should be %02X\n", psds->read_data [4], checksum, psds->read_data [SDS_BLOCK_SIZE - 2]) ;
		} ;

	ucptr = psds->read_data + 5 ;
	for (k = 0 ; k < 120 ; k += 4)
	{	sample = (ucptr [k] << 25) + (ucptr [k + 1] << 18) + (ucptr [k + 2] << 11) + (ucptr [k + 3] << 4) ;
		psds->read_samples [k / 4] = (int) (sample - 0x80000000) ;
		} ;

	return 1 ;
} /* sds_4byte_read */


static sf_count_t
sds_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	SDS_PRIVATE	*psds ;
	int			*iptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = sds_read (psf, psds, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = iptr [k] >> 16 ;
		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* sds_read_s */

static sf_count_t
sds_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	SDS_PRIVATE *psds ;
	int			total ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	total = sds_read (psf, psds, ptr, len) ;

	return total ;
} /* sds_read_i */

static sf_count_t
sds_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	SDS_PRIVATE	*psds ;
	int			*iptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	if (psf->norm_float == SF_TRUE)
		normfact = 1.0 / 0x80000000 ;
	else
		normfact = 1.0 / (1 << psds->bitwidth) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = sds_read (psf, psds, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * iptr [k] ;
		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* sds_read_f */

static sf_count_t
sds_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	SDS_PRIVATE	*psds ;
	int			*iptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	if (psf->norm_double == SF_TRUE)
		normfact = 1.0 / 0x80000000 ;
	else
		normfact = 1.0 / (1 << psds->bitwidth) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = sds_read (psf, psds, iptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * iptr [k] ;
		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* sds_read_d */

static int
sds_read (SF_PRIVATE *psf, SDS_PRIVATE *psds, int *ptr, int len)
{	int	count, total = 0 ;

	while (total < len)
	{	if (psds->read_block * psds->samplesperblock >= psds->frames)
		{	memset (&(ptr [total]), 0, (len - total) * sizeof (int)) ;
			return total ;
			} ;

		if (psds->read_count >= psds->samplesperblock)
			psds->reader (psf, psds) ;

		count = (psds->samplesperblock - psds->read_count) ;
		count = (len - total > count) ? count : len - total ;

		memcpy (&(ptr [total]), &(psds->read_samples [psds->read_count]), count * sizeof (int)) ;
		total += count ;
		psds->read_count += count ;
		} ;

	return total ;
} /* sds_read */

/*==============================================================================
*/

static sf_count_t
sds_seek (SF_PRIVATE *psf, int mode, sf_count_t seek_from_start)
{	SDS_PRIVATE	*psds ;
	sf_count_t	file_offset ;
	int			newblock, newsample ;

	if ((psds = psf->fdata) == NULL)
	{	psf->error = SFE_INTERNAL ;
		return SF_SEEK_ERROR ;
		} ;

	if (psf->datalength < 0 || psf->dataoffset < 0)
	{	psf->error = SFE_BAD_SEEK ;
		return SF_SEEK_ERROR ;
		} ;

	if (seek_from_start < 0 || seek_from_start > psf->sf.frames)
	{	psf->error = SFE_BAD_SEEK ;
		return SF_SEEK_ERROR ;
		} ;

	if (mode == SFM_READ && psds->write_count > 0)
		psds->writer (psf, psds) ;

	newblock = seek_from_start / psds->samplesperblock ;
	newsample = seek_from_start % psds->samplesperblock ;

	switch (mode)
	{	case SFM_READ :
			if (newblock > psds->total_blocks)
			{	psf->error = SFE_BAD_SEEK ;
				return SF_SEEK_ERROR ;
				} ;

			file_offset = psf->dataoffset + newblock * SDS_BLOCK_SIZE ;

			if (psf_fseek (psf, file_offset, SEEK_SET) != file_offset)
			{	psf->error = SFE_SEEK_FAILED ;
				return SF_SEEK_ERROR ;
				} ;

			psds->read_block = newblock ;
			psds->reader (psf, psds) ;
			psds->read_count = newsample ;
			break ;

		case SFM_WRITE :
			if (newblock > psds->total_blocks)
			{	psf->error = SFE_BAD_SEEK ;
				return SF_SEEK_ERROR ;
				} ;

			file_offset = psf->dataoffset + newblock * SDS_BLOCK_SIZE ;

			if (psf_fseek (psf, file_offset, SEEK_SET) != file_offset)
			{	psf->error = SFE_SEEK_FAILED ;
				return SF_SEEK_ERROR ;
				} ;

			psds->write_block = newblock ;
			psds->reader (psf, psds) ;
			psds->write_count = newsample ;
			break ;

		default :
			psf->error = SFE_BAD_SEEK ;
			return SF_SEEK_ERROR ;
			break ;
		} ;

	return seek_from_start ;
} /* sds_seek */

/*==============================================================================
*/

static int
sds_2byte_write (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char *ucptr, checksum ;
	unsigned int sample ;
	int 	k ;

	psds->write_data [0] = 0xF0 ;
	psds->write_data [1] = 0x7E ;
	psds->write_data [2] = 0 ;							/* Channel number */
	psds->write_data [3] = psds->write_block & 0x7F ;	/* Packet number */

	ucptr = psds->write_data + 5 ;
	for (k = 0 ; k < 120 ; k += 2)
	{	sample = psds->write_samples [k / 2] ;
		sample += 0x80000000 ;
		ucptr [k] = (sample >> 25) & 0x7F ;
		ucptr [k + 1] = (sample >> 18) & 0x7F ;
		} ;

	checksum = psds->write_data [1] ;
	for (k = 2 ; k < SDS_BLOCK_SIZE - 3 ; k ++)
		checksum ^= psds->write_data [k] ;
	checksum &= 0x7F ;

	psds->write_data [SDS_BLOCK_SIZE - 2] = checksum ;
	psds->write_data [SDS_BLOCK_SIZE - 1] = 0xF7 ;

	if ((k = psf_fwrite (psds->write_data, 1, SDS_BLOCK_SIZE, psf)) != SDS_BLOCK_SIZE)
		psf_log_printf (psf, "*** Warning : psf_fwrite (%d != %d).\n", k, SDS_BLOCK_SIZE) ;

	psds->write_block ++ ;
	psds->write_count = 0 ;

	if (psds->write_block > psds->total_blocks)
		psds->total_blocks = psds->write_block ;
	psds->frames = psds->total_blocks * psds->samplesperblock ;

	return 1 ;
} /* sds_2byte_write */

static int
sds_3byte_write (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char *ucptr, checksum ;
	unsigned int sample ;
	int 	k ;

	psds->write_data [0] = 0xF0 ;
	psds->write_data [1] = 0x7E ;
	psds->write_data [2] = 0 ;							/* Channel number */
	psds->write_data [3] = psds->write_block & 0x7F ;	/* Packet number */

	ucptr = psds->write_data + 5 ;
	for (k = 0 ; k < 120 ; k += 3)
	{	sample = psds->write_samples [k / 3] ;
		sample += 0x80000000 ;
		ucptr [k] = (sample >> 25) & 0x7F ;
		ucptr [k + 1] = (sample >> 18) & 0x7F ;
		ucptr [k + 2] = (sample >> 11) & 0x7F ;
		} ;

	checksum = psds->write_data [1] ;
	for (k = 2 ; k < SDS_BLOCK_SIZE - 3 ; k ++)
		checksum ^= psds->write_data [k] ;
	checksum &= 0x7F ;

	psds->write_data [SDS_BLOCK_SIZE - 2] = checksum ;
	psds->write_data [SDS_BLOCK_SIZE - 1] = 0xF7 ;

	if ((k = psf_fwrite (psds->write_data, 1, SDS_BLOCK_SIZE, psf)) != SDS_BLOCK_SIZE)
		psf_log_printf (psf, "*** Warning : psf_fwrite (%d != %d).\n", k, SDS_BLOCK_SIZE) ;

	psds->write_block ++ ;
	psds->write_count = 0 ;

	if (psds->write_block > psds->total_blocks)
		psds->total_blocks = psds->write_block ;
	psds->frames = psds->total_blocks * psds->samplesperblock ;

	return 1 ;
} /* sds_3byte_write */

static int
sds_4byte_write (SF_PRIVATE *psf, SDS_PRIVATE *psds)
{	unsigned char *ucptr, checksum ;
	unsigned int sample ;
	int 	k ;

	psds->write_data [0] = 0xF0 ;
	psds->write_data [1] = 0x7E ;
	psds->write_data [2] = 0 ;							/* Channel number */
	psds->write_data [3] = psds->write_block & 0x7F ;	/* Packet number */

	ucptr = psds->write_data + 5 ;
	for (k = 0 ; k < 120 ; k += 4)
	{	sample = psds->write_samples [k / 4] ;
		sample += 0x80000000 ;
		ucptr [k] = (sample >> 25) & 0x7F ;
		ucptr [k + 1] = (sample >> 18) & 0x7F ;
		ucptr [k + 2] = (sample >> 11) & 0x7F ;
		ucptr [k + 3] = (sample >> 4) & 0x7F ;
		} ;

	checksum = psds->write_data [1] ;
	for (k = 2 ; k < SDS_BLOCK_SIZE - 3 ; k ++)
		checksum ^= psds->write_data [k] ;
	checksum &= 0x7F ;

	psds->write_data [SDS_BLOCK_SIZE - 2] = checksum ;
	psds->write_data [SDS_BLOCK_SIZE - 1] = 0xF7 ;

	if ((k = psf_fwrite (psds->write_data, 1, SDS_BLOCK_SIZE, psf)) != SDS_BLOCK_SIZE)
		psf_log_printf (psf, "*** Warning : psf_fwrite (%d != %d).\n", k, SDS_BLOCK_SIZE) ;

	psds->write_block ++ ;
	psds->write_count = 0 ;

	if (psds->write_block > psds->total_blocks)
		psds->total_blocks = psds->write_block ;
	psds->frames = psds->total_blocks * psds->samplesperblock ;

	return 1 ;
} /* sds_4byte_write */

static sf_count_t
sds_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	SDS_PRIVATE	*psds ;
	int			*iptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = ptr [total + k] << 16 ;
		count = sds_write (psf, psds, iptr, writecount) ;
		total += count ;
		len -= writecount ;
		} ;

	return total ;
} /* sds_write_s */

static sf_count_t
sds_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	SDS_PRIVATE *psds ;
	int			total ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	total = sds_write (psf, psds, ptr, len) ;

	return total ;
} /* sds_write_i */

static sf_count_t
sds_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	SDS_PRIVATE	*psds ;
	int			*iptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	if (psf->norm_float == SF_TRUE)
		normfact = 1.0 * 0x80000000 ;
	else
		normfact = 1.0 * (1 << psds->bitwidth) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = normfact * ptr [total + k] ;
		count = sds_write (psf, psds, iptr, writecount) ;
		total += count ;
		len -= writecount ;
		} ;

	return total ;
} /* sds_write_f */

static sf_count_t
sds_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	SDS_PRIVATE	*psds ;
	int			*iptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if (psf->fdata == NULL)
		return 0 ;
	psds = (SDS_PRIVATE*) psf->fdata ;

	if (psf->norm_double == SF_TRUE)
		normfact = 1.0 * 0x80000000 ;
	else
		normfact = 1.0 * (1 << psds->bitwidth) ;

	iptr = (int*) psf->buffer ;
	bufferlen = sizeof (psf->buffer) / sizeof (int) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : len ;
		for (k = 0 ; k < writecount ; k++)
			iptr [k] = normfact * ptr [total + k] ;
		count = sds_write (psf, psds, iptr, writecount) ;
		total += count ;
		len -= writecount ;
		} ;

	return total ;
} /* sds_write_d */

static int
sds_write (SF_PRIVATE *psf, SDS_PRIVATE *psds, int *ptr, int len)
{	int	count, total = 0 ;

	while (total < len)
	{	count = psds->samplesperblock - psds->write_count ;
		if (count > len - total)
			count = len - total ;

		memcpy (&(psds->write_samples [psds->write_count]), &(ptr [total]), count * sizeof (int)) ;
		total += count ;
		psds->write_count += count ;

		if (psds->write_count >= psds->samplesperblock)
			psds->writer (psf, psds) ;
		} ;

	return total ;
} /* sds_write */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: d5d26aa3-368c-4ca6-bb85-377e5a2578cc
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


/*-----------------------------------------------------------------------------------------------
** Generic functions for performing endian swapping on integer arrays.
*/

void
endswap_short_array (short *ptr, int len)
{

#if 0
	unsigned char *ucptr, temp ;

	ucptr = ((unsigned char *) ptr) + 2 * len ;
	while (len > 0)
	{	len -- ;
		ucptr -= 2 ;
		temp = ucptr [0] ;
		ucptr [0] = ucptr [1] ;
		ucptr [1] = temp ;
		} ;
#else
	short	temp ;
	while (len > 0)
	{	len -- ;
		temp = ptr [len] ;
		ptr [len] = ENDSWAP_SHORT (temp) ;
		}
#endif
} /* endswap_short_array */

void
endswap_int_array (int *ptr, int len)
{
#if 0
	unsigned char *ucptr, temp ;

	ucptr = ((unsigned char *) ptr) + 4 * len ;
	while (len > 0)
	{	len -- ;
		ucptr -= 4 ;

		temp = ucptr [0] ;
		ucptr [0] = ucptr [3] ;
		ucptr [3] = temp ;

		temp = ucptr [1] ;
		ucptr [1] = ucptr [2] ;
		ucptr [2] = temp ;
		} ;
#else
	int temp ;

	while (len > 0)
	{	len -- ;
		temp = ptr [len] ;
		ptr [len] = ENDSWAP_INT (temp) ;
		} ;
#endif
} /* endswap_int_array */

/*	This function assumes that sizeof (long) == 8, but works correctly even
**	is sizeof (long) == 4.
*/
void
endswap_long_array (long *ptr, int len)
{	unsigned char *ucptr, temp ;

	ucptr = (unsigned char *) ptr + 8 * len ;
	while (len > 0)
	{	len -- ;
		ucptr -= 8 ;

		temp = ucptr [0] ;
		ucptr [0] = ucptr [7] ;
		ucptr [7] = temp ;

		temp = ucptr [1] ;
		ucptr [1] = ucptr [6] ;
		ucptr [6] = temp ;

		temp = ucptr [2] ;
		ucptr [2] = ucptr [5] ;
		ucptr [5] = temp ;

		temp = ucptr [3] ;
		ucptr [3] = ucptr [4] ;
		ucptr [4] = temp ;
		} ;
} /* endswap_long_array */

/*========================================================================================
*/

void
endswap_short_copy (short *dest, short *src, int len)
{
#if	0
	char *psrc, *pdest ;

	psrc = ((char *) src) + 2 * len ;
	pdest = ((char *) dest) + 2 * len ;
	while (len > 0)
	{	len -- ;
		psrc -= 2 ;
		pdest -= 2 ;

		pdest [0] = psrc [1] ;
		pdest [1] = psrc [0] ;
		} ;
#else
	while (len > 0)
	{	len -- ;
		dest [len] = ENDSWAP_SHORT (src [len]) ;
		} ;
#endif
} /* endswap_short_copy */

void
endswap_int_copy (int *dest, int *src, int len)
{
#if	0
	char *psrc, *pdest ;

	psrc = ((char *) src) + 4 * len ;
	pdest = ((char *) dest) + 4 * len ;
	while (len > 0)
	{	len -- ;
		psrc -= 4 ;
		pdest -= 4 ;

		pdest [0] = psrc [3] ;
		pdest [1] = psrc [2] ;
		pdest [2] = psrc [1] ;
		pdest [3] = psrc [0] ;
		} ;
#else
	while (len > 0)
	{	len -- ;
		dest [len] = ENDSWAP_INT (src [len]) ;
		} ;
#endif
} /* endswap_int_copy */

/*	This function assumes that sizeof (long) == 8, but works correctly even
**	is sizeof (long) == 4.
*/
void
endswap_long_copy (long *dest, long *src, int len)
{	char *psrc, *pdest ;

	psrc = (char *) src + 8 * len ;
	pdest = (char *) dest + 8 * len ;
	while (len > 0)
	{	len -- ;
		psrc -= 8 ;
		pdest -= 8 ;

		pdest [0] = psrc [7] ;
		pdest [1] = psrc [6] ;
		pdest [2] = psrc [5] ;
		pdest [3] = psrc [4] ;
		pdest [4] = psrc [3] ;
		pdest [5] = psrc [2] ;
		pdest [6] = psrc [1] ;
		pdest [7] = psrc [0] ;
		} ;
} /* endswap_long_copy */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 4cf6cc94-da4e-4ef5-aa4c-03dc6cd16810
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>



/*
 *  SHORT TERM ANALYSIS FILTERING SECTION
 */

/* 4.2.8 */

static void Decoding_of_the_coded_Log_Area_Ratios (
	word 	* LARc,		/* coded log area ratio	[0..7] 	IN	*/
	word	* LARpp)	/* out: decoded ..			*/
{
	register word	temp1 /* , temp2 */;

	/*  This procedure requires for efficient implementation
	 *  two tables.
 	 *
	 *  INVA[1..8] = integer( (32768 * 8) / real_A[1..8])
	 *  MIC[1..8]  = minimum value of the LARc[1..8]
	 */

	/*  Compute the LARpp[1..8]
	 */

	/* 	for (i = 1; i <= 8; i++, B++, MIC++, INVA++, LARc++, LARpp++) {
	 *
	 *		temp1  = GSM_ADD( *LARc, *MIC ) << 10;
	 *		temp2  = *B << 1;
	 *		temp1  = GSM_SUB( temp1, temp2 );
	 *
	 *		assert(*INVA != MIN_WORD);
	 *
	 *		temp1  = GSM_MULT_R( *INVA, temp1 );
	 *		*LARpp = GSM_ADD( temp1, temp1 );
	 *	}
	 */

#undef	short_termSTEP
#define	short_termSTEP( B, MIC, INVA )	\
		temp1    = GSM_ADD( *LARc++, MIC ) << 10;	\
		temp1    = GSM_SUB( temp1, B << 1 );		\
		temp1    = GSM_MULT_R( INVA, temp1 );		\
		*LARpp++ = GSM_ADD( temp1, temp1 );

	short_termSTEP(      0,  -32,  13107 );
	short_termSTEP(      0,  -32,  13107 );
	short_termSTEP(   2048,  -16,  13107 );
	short_termSTEP(  -2560,  -16,  13107 );

	short_termSTEP(     94,   -8,  19223 );
	short_termSTEP(  -1792,   -8,  17476 );
	short_termSTEP(   -341,   -4,  31454 );
	short_termSTEP(  -1144,   -4,  29708 );

	/* NOTE: the addition of *MIC is used to restore
	 * 	 the sign of *LARc.
	 */
}

/* 4.2.9 */
/* Computation of the quantized reflection coefficients 
 */

/* 4.2.9.1  Interpolation of the LARpp[1..8] to get the LARp[1..8]
 */

/*
 *  Within each frame of 160 analyzed speech samples the short term
 *  analysis and synthesis filters operate with four different sets of
 *  coefficients, derived from the previous set of decoded LARs(LARpp(j-1))
 *  and the actual set of decoded LARs (LARpp(j))
 *
 * (Initial value: LARpp(j-1)[1..8] = 0.)
 */

static void Coefficients_0_12 (
	register word * LARpp_j_1,
	register word * LARpp_j,
	register word * LARp)
{
	register int 	i;

	for (i = 1; i <= 8; i++, LARp++, LARpp_j_1++, LARpp_j++) {
		*LARp = GSM_ADD( SASR_W( *LARpp_j_1, 2 ), SASR_W( *LARpp_j, 2 ));
		*LARp = GSM_ADD( *LARp,  SASR_W( *LARpp_j_1, 1));
	}
}

static void Coefficients_13_26 (
	register word * LARpp_j_1,
	register word * LARpp_j,
	register word * LARp)
{
	register int i;
	for (i = 1; i <= 8; i++, LARpp_j_1++, LARpp_j++, LARp++) {
		*LARp = GSM_ADD( SASR_W( *LARpp_j_1, 1), SASR_W( *LARpp_j, 1 ));
	}
}

static void Coefficients_27_39 (
	register word * LARpp_j_1,
	register word * LARpp_j,
	register word * LARp)
{
	register int i;

	for (i = 1; i <= 8; i++, LARpp_j_1++, LARpp_j++, LARp++) {
		*LARp = GSM_ADD( SASR_W( *LARpp_j_1, 2 ), SASR_W( *LARpp_j, 2 ));
		*LARp = GSM_ADD( *LARp, SASR_W( *LARpp_j, 1 ));
	}
}


static void Coefficients_40_159 (
	register word * LARpp_j,
	register word * LARp)
{
	register int i;

	for (i = 1; i <= 8; i++, LARp++, LARpp_j++)
		*LARp = *LARpp_j;
}

/* 4.2.9.2 */

static void LARp_to_rp (
	register word * LARp)	/* [0..7] IN/OUT  */
/*
 *  The input of this procedure is the interpolated LARp[0..7] array.
 *  The reflection coefficients, rp[i], are used in the analysis
 *  filter and in the synthesis filter.
 */
{
	register int 		i;
	register word		temp;

	for (i = 1; i <= 8; i++, LARp++) {

		/* temp = GSM_ABS( *LARp );
	         *
		 * if (temp < 11059) temp <<= 1;
		 * else if (temp < 20070) temp += 11059;
		 * else temp = GSM_ADD( temp >> 2, 26112 );
		 *
		 * *LARp = *LARp < 0 ? -temp : temp;
		 */

		if (*LARp < 0) {
			temp = *LARp == MIN_WORD ? MAX_WORD : -(*LARp);
			*LARp = - ((temp < 11059) ? temp << 1
				: ((temp < 20070) ? temp + 11059
				:  GSM_ADD( (word) (temp >> 2), (word) 26112 )));
		} else {
			temp  = *LARp;
			*LARp =    (temp < 11059) ? temp << 1
				: ((temp < 20070) ? temp + 11059
				:  GSM_ADD( (word) (temp >> 2), (word) 26112 ));
		}
	}
}


/* 4.2.10 */
static void Short_term_analysis_filtering (
	struct gsm_state * S,
	register word	* rp,	/* [0..7]	IN	*/
	register int 	k_n, 	/*   k_end - k_start	*/
	register word	* s	/* [0..n-1]	IN/OUT	*/
)
/*
 *  This procedure computes the short term residual signal d[..] to be fed
 *  to the RPE-LTP loop from the s[..] signal and from the local rp[..]
 *  array (quantized reflection coefficients).  As the call of this
 *  procedure can be done in many ways (see the interpolation of the LAR
 *  coefficient), it is assumed that the computation begins with index
 *  k_start (for arrays d[..] and s[..]) and stops with index k_end
 *  (k_start and k_end are defined in 4.2.9.1).  This procedure also
 *  needs to keep the array u[0..7] in memory for each call.
 */
{
	register word		* u = S->u;
	register int		i;
	register word		di, zzz, ui, sav, rpi;

	for (; k_n--; s++) {

		di = sav = *s;

		for (i = 0; i < 8; i++) {		/* YYY */

			ui    = u[i];
			rpi   = rp[i];
			u[i]  = sav;

			zzz   = GSM_MULT_R(rpi, di);
			sav   = GSM_ADD(   ui,  zzz);

			zzz   = GSM_MULT_R(rpi, ui);
			di    = GSM_ADD(   di,  zzz );
		}

		*s = di;
	}
}

#if defined(USE_FLOAT_MUL) && defined(FAST)

static void Fast_Short_term_analysis_filtering (
	struct gsm_state * S,
	register word	* rp,	/* [0..7]	IN	*/
	register int 	k_n, 	/*   k_end - k_start	*/
	register word	* s	/* [0..n-1]	IN/OUT	*/
)
{
	register word		* u = S->u;
	register int		i;

	float 	  uf[8],
		 rpf[8];

	register float scalef = 3.0517578125e-5;
	register float		sav, di, temp;

	for (i = 0; i < 8; ++i) {
		uf[i]  = u[i];
		rpf[i] = rp[i] * scalef;
	}
	for (; k_n--; s++) {
		sav = di = *s;
		for (i = 0; i < 8; ++i) {
			register float rpfi = rpf[i];
			register float ufi  = uf[i];

			uf[i] = sav;
			temp  = rpfi * di + ufi;
			di   += rpfi * ufi;
			sav   = temp;
		}
		*s = di;
	}
	for (i = 0; i < 8; ++i) u[i] = uf[i];
}
#endif /* ! (defined (USE_FLOAT_MUL) && defined (FAST)) */

static void Short_term_synthesis_filtering (
	struct gsm_state * S,
	register word	* rrp,	/* [0..7]	IN	*/
	register int	k,	/* k_end - k_start	*/
	register word	* wt,	/* [0..k-1]	IN	*/
	register word	* sr	/* [0..k-1]	OUT	*/
)
{
	register word		* v = S->v;
	register int		i;
	register word		sri, tmp1, tmp2;

	while (k--) {
		sri = *wt++;
		for (i = 8; i--;) {

			/* sri = GSM_SUB( sri, gsm_mult_r( rrp[i], v[i] ) );
			 */
			tmp1 = rrp[i];
			tmp2 = v[i];
			tmp2 =  ( tmp1 == MIN_WORD && tmp2 == MIN_WORD
				? MAX_WORD
				: 0x0FFFF & (( (longword)tmp1 * (longword)tmp2
					     + 16384) >> 15)) ;

			sri  = GSM_SUB( sri, tmp2 );

			/* v[i+1] = GSM_ADD( v[i], gsm_mult_r( rrp[i], sri ) );
			 */
			tmp1  = ( tmp1 == MIN_WORD && sri == MIN_WORD
				? MAX_WORD
				: 0x0FFFF & (( (longword)tmp1 * (longword)sri
					     + 16384) >> 15)) ;

			v[i+1] = GSM_ADD( v[i], tmp1);
		}
		*sr++ = v[0] = sri;
	}
}


#if defined(FAST) && defined(USE_FLOAT_MUL)

static void Fast_Short_term_synthesis_filtering (
	struct gsm_state * S,
	register word	* rrp,	/* [0..7]	IN	*/
	register int	k,	/* k_end - k_start	*/
	register word	* wt,	/* [0..k-1]	IN	*/
	register word	* sr	/* [0..k-1]	OUT	*/
)
{
	register word		* v = S->v;
	register int		i;

	float va[9], rrpa[8];
	register float scalef = 3.0517578125e-5, temp;

	for (i = 0; i < 8; ++i) {
		va[i]   = v[i];
		rrpa[i] = (float)rrp[i] * scalef;
	}
	while (k--) {
		register float sri = *wt++;
		for (i = 8; i--;) {
			sri -= rrpa[i] * va[i];
			if     (sri < -32768.) sri = -32768.;
			else if (sri > 32767.) sri =  32767.;

			temp = va[i] + rrpa[i] * sri;
			if     (temp < -32768.) temp = -32768.;
			else if (temp > 32767.) temp =  32767.;
			va[i+1] = temp;
		}
		*sr++ = va[0] = sri;
	}
	for (i = 0; i < 9; ++i) v[i] = va[i];
}

#endif /* defined(FAST) && defined(USE_FLOAT_MUL) */

void Gsm_Short_Term_Analysis_Filter (

	struct gsm_state * S,

	word	* LARc,		/* coded log area ratio [0..7]  IN	*/
	word	* s		/* signal [0..159]		IN/OUT	*/
)
{
	word		* LARpp_j	= S->LARpp[ S->j      ];
	word		* LARpp_j_1	= S->LARpp[ S->j ^= 1 ];

	word		LARp[8];

#undef	FILTER
#if 	defined(FAST) && defined(USE_FLOAT_MUL)
# 	define	FILTER 	(* (S->fast			\
			   ? Fast_Short_term_analysis_filtering	\
		    	   : Short_term_analysis_filtering	))

#else
# 	define	FILTER	Short_term_analysis_filtering
#endif

	Decoding_of_the_coded_Log_Area_Ratios( LARc, LARpp_j );

	Coefficients_0_12(  LARpp_j_1, LARpp_j, LARp );
	LARp_to_rp( LARp );
	FILTER( S, LARp, 13, s);

	Coefficients_13_26( LARpp_j_1, LARpp_j, LARp);
	LARp_to_rp( LARp );
	FILTER( S, LARp, 14, s + 13);

	Coefficients_27_39( LARpp_j_1, LARpp_j, LARp);
	LARp_to_rp( LARp );
	FILTER( S, LARp, 13, s + 27);

	Coefficients_40_159( LARpp_j, LARp);
	LARp_to_rp( LARp );
	FILTER( S, LARp, 120, s + 40);
}

void Gsm_Short_Term_Synthesis_Filter (
	struct gsm_state * S,

	word	* LARcr,	/* received log area ratios [0..7] IN  */
	word	* wt,		/* received d [0..159]		   IN  */

	word	* s		/* signal   s [0..159]		  OUT  */
)
{
	word		* LARpp_j	= S->LARpp[ S->j     ];
	word		* LARpp_j_1	= S->LARpp[ S->j ^=1 ];

	word		LARp[8];

#undef	FILTER
#if 	defined(FAST) && defined(USE_FLOAT_MUL)

# 	define	FILTER 	(* (S->fast			\
			   ? Fast_Short_term_synthesis_filtering	\
		    	   : Short_term_synthesis_filtering	))
#else
#	define	FILTER	Short_term_synthesis_filtering
#endif

	Decoding_of_the_coded_Log_Area_Ratios( LARcr, LARpp_j );

	Coefficients_0_12( LARpp_j_1, LARpp_j, LARp );
	LARp_to_rp( LARp );
	FILTER( S, LARp, 13, wt, s );

	Coefficients_13_26( LARpp_j_1, LARpp_j, LARp);
	LARp_to_rp( LARp );
	FILTER( S, LARp, 14, wt + 13, s + 13 );

	Coefficients_27_39( LARpp_j_1, LARpp_j, LARp);
	LARp_to_rp( LARp );
	FILTER( S, LARp, 13, wt + 27, s + 27 );

	Coefficients_40_159( LARpp_j, LARp );
	LARp_to_rp( LARp );
	FILTER(S, LARp, 120, wt + 40, s + 40);
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 019ac7ba-c6dd-4540-abf0-8644b6c4a633
*/

/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>


#define		SNDFILE_MAGICK	0x1234C0DE

typedef struct
{	int 		error ;
	const char	*str ;
} ErrorStruct ;

static
ErrorStruct SndfileErrors [] =
{
	/* Public error values and their associated strings. */
	{	SF_ERR_NO_ERROR				, "No Error." },
	{	SF_ERR_UNRECOGNISED_FORMAT	, "File opened for read. Format not recognised." },
	{	SF_ERR_SYSTEM				, "System error." /* Often replaced. */ 	},

	/* Private error values and their associated strings. */
	{	SFE_BAD_FILE			, "File does not exist or is not a regular file (possibly a pipe?)." },
	{	SFE_BAD_FILE_READ		, "File exists but no data could be read." },
	{	SFE_OPEN_FAILED			, "Could not open file." },
	{	SFE_BAD_SNDFILE_PTR		, "Not a valid SNDFILE* pointer." },
	{	SFE_BAD_SF_INFO_PTR		, "NULL SF_INFO pointer passed to libsndfile." },
	{	SFE_BAD_SF_INCOMPLETE	, "SF_PRIVATE struct incomplete and end of header parsing." },
	{	SFE_BAD_FILE_PTR		, "Bad FILE pointer." },
	{	SFE_BAD_INT_PTR			, "Internal error, Bad pointer." },
	{	SFE_BAD_STAT_SIZE		, "Error : software was misconfigured at compile time (sizeof statbuf.st_size)." },

	{	SFE_MALLOC_FAILED		, "Internal malloc () failed." },
	{	SFE_UNIMPLEMENTED		, "File contains data in an unimplemented format." },
	{	SFE_BAD_READ_ALIGN		, "Attempt to read a non-integer number of channels." },
	{	SFE_BAD_WRITE_ALIGN 	, "Attempt to write a non-integer number of channels." },
	{	SFE_UNKNOWN_FORMAT		, "File contains data in an unknown format." },
	{	SFE_NOT_READMODE		, "Read attempted on file currently open for write." },
	{	SFE_NOT_WRITEMODE		, "Write attempted on file currently open for read." },
	{	SFE_BAD_MODE_RW			, "This file format does not support read/write mode." },
	{	SFE_BAD_SF_INFO			, "Internal error : SF_INFO struct incomplete." },
	{	SFE_BAD_OFFSET			, "Error : supplied offset beyond end of file." },
	{	SFE_NO_EMBED_SUPPORT	, "Error : embedding not supported for this file format." },
	{	SFE_NO_EMBEDDED_RDWR	, "Error : cannot open embedded file read/write." },
	{	SFE_NO_PIPE_WRITE		, "Error : this file format does not support pipe write." },
	{	SFE_BAD_RDWR_FORMAT		, "Attempted to open read only format for RDWR." },

	{	SFE_INTERLEAVE_MODE		, "Attempt to write to file with non-interleaved data." },
	{	SFE_INTERLEAVE_SEEK		, "Bad karma in seek during interleave read operation." },
	{	SFE_INTERLEAVE_READ		, "Bad karma in read during interleave read operation." },

	{	SFE_INTERNAL			, "Unspecified internal error." },
	{	SFE_LOG_OVERRUN			, "Log buffer has overrun. File probably broken." },
	{	SFE_BAD_CONTROL_CMD		, "Bad command passed to function sf_command()." },
	{	SFE_BAD_ENDIAN			, "Bad endian-ness. Try default endian-ness" },
	{	SFE_CHANNEL_COUNT		, "Too many channels specified." },

	{	SFE_BAD_SEEK			, "Internal psf_fseek() failed." },
	{	SFE_NOT_SEEKABLE		, "Seek attempted on unseekable file type." },
	{	SFE_AMBIGUOUS_SEEK		, "Error: combination of file open mode and seek command is ambiguous." },
	{	SFE_WRONG_SEEK			, "Error: invalid seek parameters." },
	{	SFE_SEEK_FAILED			, "Error: parameters OK, but psf_seek() failed." },

	{	SFE_BAD_OPEN_MODE		, "Error: bad mode parameter for file open." },
	{	SFE_OPEN_PIPE_RDWR		, "Error: attempt toopen a pipe in read/write mode." },
	{	SFE_RDWR_POSITION		, "Error on RDWR position (cryptic)." },

	{	SFE_STR_NO_SUPPORT		, "Error : File type does not support string data." },
	{	SFE_STR_MAX_DATA		, "Error : Maximum string data storage reached." },
	{	SFE_STR_MAX_COUNT		, "Error : Maximum string data count reached." },
	{	SFE_STR_BAD_TYPE		, "Error : Bad string data type." },
	{	SFE_STR_NO_ADD_END		, "Error : file type does not support strings added at end of file." },
	{	SFE_STR_BAD_STRING		, "Error : bad string." },
	{	SFE_STR_WEIRD			, "Error : Weird string error." },
	{	SFE_RDWR_BAD_HEADER		, "Error : Cannot open file in read/write mode due to string data in header." },

	{	SFE_WAV_NO_RIFF			, "Error in WAV file. No 'RIFF' chunk marker." },
	{	SFE_WAV_NO_WAVE			, "Error in WAV file. No 'WAVE' chunk marker." },
	{	SFE_WAV_NO_FMT			, "Error in WAV file. No 'fmt ' chunk marker." },
	{	SFE_WAV_FMT_SHORT		, "Error in WAV file. Short 'fmt ' chunk." },

	{	SFE_WAV_FMT_TOO_BIG		, "Error in WAV file. 'fmt ' chunk too large." },
	{	SFE_WAV_BAD_FACT		, "Error in WAV file. 'fact' chunk out of place." },
	{	SFE_WAV_BAD_PEAK		, "Error in WAV file. Bad 'PEAK' chunk." },
	{	SFE_WAV_PEAK_B4_FMT		, "Error in WAV file. 'PEAK' chunk found before 'fmt ' chunk." },

	{	SFE_WAV_BAD_FORMAT		, "Error in WAV file. Errors in 'fmt ' chunk." },
	{	SFE_WAV_BAD_BLOCKALIGN	, "Error in WAV file. Block alignment in 'fmt ' chunk is incorrect." },
	{	SFE_WAV_NO_DATA			, "Error in WAV file. No 'data' chunk marker." },
	{	SFE_WAV_UNKNOWN_CHUNK	, "Error in WAV file. File contains an unknown chunk marker." },
	{	SFE_WAV_WVPK_DATA		, "Error in WAV file. Data is in WAVPACK format." },

	{	SFE_WAV_ADPCM_NOT4BIT	, "Error in ADPCM WAV file. Invalid bit width." },
	{	SFE_WAV_ADPCM_CHANNELS	, "Error in ADPCM WAV file. Invalid number of channels." },
	{	SFE_WAV_GSM610_FORMAT	, "Error in GSM610 WAV file. Invalid format chunk." },

	{	SFE_AIFF_NO_FORM		, "Error in AIFF file, bad 'FORM' marker." },
	{	SFE_AIFF_AIFF_NO_FORM	, "Error in AIFF file, 'AIFF' marker without 'FORM'." },
	{	SFE_AIFF_COMM_NO_FORM	, "Error in AIFF file, 'COMM' marker without 'FORM'." },
	{	SFE_AIFF_SSND_NO_COMM	, "Error in AIFF file, 'SSND' marker without 'COMM'." },
	{	SFE_AIFF_UNKNOWN_CHUNK	, "Error in AIFF file, unknown chunk." },
	{	SFE_AIFF_COMM_CHUNK_SIZE, "Error in AIFF file, bad 'COMM' chunk size." },
	{	SFE_AIFF_BAD_COMM_CHUNK , "Error in AIFF file, bad 'COMM' chunk." },
	{	SFE_AIFF_PEAK_B4_COMM	, "Error in AIFF file. 'PEAK' chunk found before 'COMM' chunk." },
	{	SFE_AIFF_BAD_PEAK		, "Error in AIFF file. Bad 'PEAK' chunk." },
	{	SFE_AIFF_NO_SSND		, "Error in AIFF file, bad 'SSND' chunk." },
	{	SFE_AIFF_NO_DATA		, "Error in AIFF file, no sound data." },
	{	SFE_AIFF_RW_SSND_NOT_LAST, "Error in AIFF file, RDWR only possible if SSND chunk at end of file." },

	{	SFE_AU_UNKNOWN_FORMAT	, "Error in AU file, unknown format." },
	{	SFE_AU_NO_DOTSND		, "Error in AU file, missing '.snd' or 'dns.' marker." },
	{	SFE_AU_EMBED_BAD_LEN	, "Embedded AU file with unknown length." },

	{	SFE_RAW_READ_BAD_SPEC	, "Error while opening RAW file for read. Must specify format and channels.\n"
									"Possibly trying to open unsupported format."
									 },
	{	SFE_RAW_BAD_BITWIDTH	, "Error. RAW file bitwidth must be a multiple of 8." },
	{	SFE_RAW_BAD_FORMAT		, "Error. Bad format field in SF_INFO struct when openning a RAW file for read." },

	{	SFE_PAF_NO_MARKER		, "Error in PAF file, no marker." },
	{	SFE_PAF_VERSION			, "Error in PAF file, bad version." },
	{	SFE_PAF_UNKNOWN_FORMAT	, "Error in PAF file, unknown format." },
	{	SFE_PAF_SHORT_HEADER	, "Error in PAF file. File shorter than minimal header." },

	{	SFE_SVX_NO_FORM			, "Error in 8SVX / 16SV file, no 'FORM' marker." },
	{	SFE_SVX_NO_BODY			, "Error in 8SVX / 16SV file, no 'BODY' marker." },
	{	SFE_SVX_NO_DATA			, "Error in 8SVX / 16SV file, no sound data." },
	{	SFE_SVX_BAD_COMP		, "Error in 8SVX / 16SV file, unsupported compression format." },
	{	SFE_SVX_BAD_NAME_LENGTH	, "Error in 8SVX / 16SV file, NAME chunk too long." },

	{	SFE_NIST_BAD_HEADER		, "Error in NIST file, bad header." },
	{	SFE_NIST_CRLF_CONVERISON, "Error : NIST file damaged by Windows CR -> CRLF conversion process."	},
	{	SFE_NIST_BAD_ENCODING	, "Error in NIST file, unsupported compression format." },

	{	SFE_VOC_NO_CREATIVE		, "Error in VOC file, no 'Creative Voice File' marker." },
	{	SFE_VOC_BAD_FORMAT		, "Error in VOC file, bad format." },
	{	SFE_VOC_BAD_VERSION		, "Error in VOC file, bad version number." },
	{	SFE_VOC_BAD_MARKER		, "Error in VOC file, bad marker in file." },
	{	SFE_VOC_BAD_SECTIONS	, "Error in VOC file, incompatible VOC sections." },
	{	SFE_VOC_MULTI_SAMPLERATE, "Error in VOC file, more than one sample rate defined." },
	{	SFE_VOC_MULTI_SECTION	, "Unimplemented VOC file feature, file contains multiple sound sections." },
	{	SFE_VOC_MULTI_PARAM		, "Error in VOC file, file contains multiple bit or channel widths." },
	{	SFE_VOC_SECTION_COUNT	, "Error in VOC file, too many sections." },
	{	SFE_VOC_NO_PIPE			, "Error : not able to operate on VOC files over a pipe." },

	{	SFE_IRCAM_NO_MARKER		, "Error in IRCAM file, bad IRCAM marker." },
	{	SFE_IRCAM_BAD_CHANNELS	, "Error in IRCAM file, bad channel count." },
	{	SFE_IRCAM_UNKNOWN_FORMAT, "Error in IRCAM file, unknow encoding format." },

	{	SFE_W64_64_BIT			, "Error in W64 file, file contains 64 bit offset." },

	{	SFE_W64_NO_RIFF			, "Error in W64 file. No 'riff' chunk marker." },
	{	SFE_W64_NO_WAVE			, "Error in W64 file. No 'wave' chunk marker." },
	{	SFE_W64_NO_FMT			, "Error in W64 file. No 'fmt ' chunk marker." },
	{	SFE_W64_NO_DATA			, "Error in W64 file. No 'data' chunk marker." },

	{	SFE_W64_FMT_SHORT		, "Error in W64 file. Short 'fmt ' chunk." },
	{	SFE_W64_FMT_TOO_BIG		, "Error in W64 file. 'fmt ' chunk too large." },

	{	SFE_W64_ADPCM_NOT4BIT	, "Error in ADPCM W64 file. Invalid bit width." },
	{	SFE_W64_ADPCM_CHANNELS	, "Error in ADPCM W64 file. Invalid number of channels." },
	{	SFE_W64_GSM610_FORMAT	, "Error in GSM610 W64 file. Invalid format chunk." },

	{	SFE_MAT4_BAD_NAME		, "Error in MAT4 file. No variable name." },
	{	SFE_MAT4_NO_SAMPLERATE	, "Error in MAT4 file. No sample rate." },
	{	SFE_MAT4_ZERO_CHANNELS	, "Error in MAT4 file. Channel count is zero." },

	{	SFE_MAT5_BAD_ENDIAN		, "Error in MAT5 file. Not able to determine endian-ness." },
	{	SFE_MAT5_NO_BLOCK		, "Error in MAT5 file. Bad block structure." },
	{	SFE_MAT5_SAMPLE_RATE	, "Error in MAT5 file. Not able to determine sample rate." },
	{	SFE_MAT5_ZERO_CHANNELS	, "Error in MAT5 file. Channel count is zero." },

	{	SFE_PVF_NO_PVF1			, "Error in PVF file. No PVF1 marker." },
	{	SFE_PVF_BAD_HEADER		, "Error in PVF file. Bad header." },
	{	SFE_PVF_BAD_BITWIDTH	, "Error in PVF file. Bad bit width." },

	{	SFE_XI_BAD_HEADER		, "Error in XI file. Bad header." },
	{	SFE_XI_EXCESS_SAMPLES	, "Error in XI file. Excess samples in file." },
	{	SFE_XI_NO_PIPE			, "Error : not able to operate on XI files over a pipe." },

	{	SFE_HTK_NO_PIPE			, "Error : not able to operate on HTK files over a pipe." },

	{	SFE_SDS_NOT_SDS			, "Error : not an SDS file." },
	{	SFE_SDS_BAD_BIT_WIDTH	, "Error : bad bit width for SDS file." },

	{	SFE_DWVW_BAD_BITWIDTH	, "Error : Bad bit width for DWVW encoding. Must be 12, 16 or 24." },
	{	SFE_G72X_NOT_MONO		, "Error : G72x encoding does not support more than 1 channel." },

	{	SFE_MAX_ERROR			, "Maximum error number." },
	{	SFE_MAX_ERROR + 1		, NULL }
} ;

/*------------------------------------------------------------------------------
*/

static int 	format_from_extension (const char *filename) ;
static int	guess_file_type (SF_PRIVATE *psf, const char *filename) ;
static int	validate_sfinfo (SF_INFO *sfinfo) ;
static int	validate_psf (SF_PRIVATE *psf) ;
static void	save_header_info (SF_PRIVATE *psf) ;
static void	copy_filename (SF_PRIVATE *psf, const char *path) ;
static int	psf_close (SF_PRIVATE *psf) ;
static int	psf_open_file (SF_PRIVATE *psf, int mode, SF_INFO *sfinfo) ;

/*------------------------------------------------------------------------------
** Private (static) variables.
*/

static int	sf_errno = 0 ;
static char	sf_logbuffer [SF_BUFFER_LEN] = { 0 } ;
static char	sf_syserr [SF_SYSERR_LEN] = { 0 } ;

/*------------------------------------------------------------------------------
*/

#define	VALIDATE_SNDFILE_AND_ASSIGN_PSF(a,b,c)		\
		{	if (! (a))								\
			{	sf_errno = SFE_BAD_SNDFILE_PTR ;	\
				return 0 ;							\
				} ;									\
			(b) = (SF_PRIVATE*) (a) ;				\
			if (psf_filedes_valid (b) == 0)			\
			{	(b)->error = SFE_BAD_FILE_PTR ;		\
				return 0 ;							\
				} ;									\
			if ((b)->Magick != SNDFILE_MAGICK)		\
			{	(b)->error = SFE_BAD_SNDFILE_PTR ;	\
				return 0 ;							\
				} ;									\
			if (c) (b)->error = 0 ;					\
			}

/*------------------------------------------------------------------------------
**	Public functions.
*/

SNDFILE*
sf_open	(const char *path, int mode, SF_INFO *sfinfo)
{	SF_PRIVATE 	*psf ;
	int			error = 0 ;

	if ((psf = calloc (1, sizeof (SF_PRIVATE))) == NULL)
	{	sf_errno = SFE_MALLOC_FAILED ;
		return	NULL ;
		} ;

	memset (psf, 0, sizeof (SF_PRIVATE)) ;

	psf_log_printf (psf, "File : %s\n", path) ;

	copy_filename (psf, path) ;

	if (strcmp (path, "-") == 0)
		error = psf_set_stdio (psf, mode) ;
	else
		error = psf_fopen (psf, path, mode) ;

	if (error == 0)
		error = psf_open_file (psf, mode, sfinfo) ;

	if (error)
	{	sf_errno = error ;
		if (error == SFE_SYSTEM)
			LSF_SNPRINTF (sf_syserr, sizeof (sf_syserr), "%s", psf->syserr) ;
		LSF_SNPRINTF (sf_logbuffer, sizeof (sf_logbuffer), "%s", psf->logbuffer) ;
		psf_close (psf) ;
		return NULL ;
		} ;

	memcpy (sfinfo, &(psf->sf), sizeof (SF_INFO)) ;

	return (SNDFILE*) psf ;
} /* sf_open */

SNDFILE*
sf_open_fd	(int fd, int mode, SF_INFO *sfinfo, int close_desc)
{	SF_PRIVATE 	*psf ;
	int			error ;

	if ((psf = calloc (1, sizeof (SF_PRIVATE))) == NULL)
	{	sf_errno = SFE_MALLOC_FAILED ;
		return	NULL ;
		} ;

	psf_set_file (psf, fd) ;
	psf->is_pipe = psf_is_pipe (psf) ;
	psf->fileoffset = psf_ftell (psf) ;

	if (! close_desc)
		psf->do_not_close_descriptor = SF_TRUE ;

	error = psf_open_file (psf, mode, sfinfo) ;

	if (error)
	{	sf_errno = error ;
		if (error == SFE_SYSTEM)
			LSF_SNPRINTF (sf_syserr, sizeof (sf_syserr), "%s", psf->syserr) ;
		LSF_SNPRINTF (sf_logbuffer, sizeof (sf_logbuffer), "%s", psf->logbuffer) ;
		psf_close (psf) ;
		return NULL ;
		} ;

	memcpy (sfinfo, &(psf->sf), sizeof (SF_INFO)) ;

	return (SNDFILE*) psf ;
} /* sf_open_fd */

/*------------------------------------------------------------------------------
*/

int
sf_close	(SNDFILE *sndfile)
{	SF_PRIVATE	*psf ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	return psf_close (psf) ;
} /* sf_close */

/*==============================================================================
*/

const char*
sf_error_number	(int errnum)
{	static const char *bad_errnum =
		"No error defined for this error number. This is a bug in libsndfile." ;
	int	k ;

	if (errnum == SFE_MAX_ERROR)
		return SndfileErrors [0].str ;

	if (errnum < 0 || errnum > SFE_MAX_ERROR)
	{	/* This really shouldn't happen in release versions. */
		printf ("Not a valid error number (%d).\n", errnum) ;
		return bad_errnum ;
		} ;

	for (k = 0 ; SndfileErrors [k].str ; k++)
		if (errnum == SndfileErrors [k].error)
			return SndfileErrors [k].str ;

	return bad_errnum ;
} /* sf_error_number */

const char*
sf_strerror (SNDFILE *sndfile)
{	SF_PRIVATE 	*psf = NULL ;
	int errnum ;

	if (! sndfile)
	{	errnum = sf_errno ;
		if (errnum == SFE_SYSTEM && sf_syserr [0])
			return sf_syserr ;
		}
	else
	{	psf = (SF_PRIVATE *) sndfile ;

		if (psf->Magick != SNDFILE_MAGICK)
			return	"sf_strerror : Bad magic number." ;

		errnum = psf->error ;

		if (errnum == SFE_SYSTEM && psf->syserr [0])
			return psf->syserr ;
		} ;

	return sf_error_number (errnum) ;
} /* sf_strerror */

/*------------------------------------------------------------------------------
*/

int
sf_error (SNDFILE *sndfile)
{	SF_PRIVATE	*psf ;

	if (! sndfile)
	{	if (&sf_error != 0) // ge: added &
			return 1 ;
		return 0 ;
		} ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 0) ;

	if (psf->error)
		return 1 ;

	return 0 ;
} /* sf_error */

/*------------------------------------------------------------------------------
*/

int
sf_perror (SNDFILE *sndfile)
{	SF_PRIVATE 	*psf ;
	int 		errnum ;

	if (! sndfile)
	{	errnum = sf_errno ;
		}
	else
	{	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 0) ;
		errnum = psf->error ;
		} ;

	fprintf (stderr, "%s\n", sf_error_number (errnum)) ;
	return SFE_NO_ERROR ;
} /* sf_perror */


/*------------------------------------------------------------------------------
*/

int
sf_error_str (SNDFILE *sndfile, char *str, size_t maxlen)
{	SF_PRIVATE 	*psf ;
	int 		errnum ;

	if (! str)
		return SFE_INTERNAL ;

	if (! sndfile)
		errnum = sf_errno ;
	else
	{	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 0) ;
		errnum = psf->error ;
		} ;

	LSF_SNPRINTF (str, maxlen, "%s", sf_error_number (errnum)) ;

	return SFE_NO_ERROR ;
} /* sf_error_str */

/*==============================================================================
*/

int
sf_format_check	(const SF_INFO *info)
{	int	subformat, endian ;

	subformat = info->format & SF_FORMAT_SUBMASK ;
	endian = info->format & SF_FORMAT_ENDMASK ;

	/* This is the place where each file format can check if the suppiled
	** SF_INFO struct is valid.
	** Return 0 on failure, 1 ons success.
	*/

	if (info->channels < 1 || info->channels > 256)
		return 0 ;

	if (info->samplerate < 0)
		return 0 ;

	switch (info->format & SF_FORMAT_TYPEMASK)
	{	case SF_FORMAT_WAV :
		case SF_FORMAT_WAVEX :
				/* WAV is strictly little endian. */
				if (endian == SF_ENDIAN_BIG || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if ((subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM) && info->channels <= 2)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_AIFF :
				/* AIFF does allow both endian-nesses for PCM data.*/
				if (subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				/* Other encodings. Check for endian-ness. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_S8)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if ((subformat == SF_FORMAT_DWVW_12 || subformat == SF_FORMAT_DWVW_16 ||
							subformat == SF_FORMAT_DWVW_24) && info-> channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_IMA_ADPCM && (info->channels == 1 || info->channels == 2))
					return 1 ;
				break ;

		case SF_FORMAT_AU :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				if (subformat == SF_FORMAT_G721_32 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_G723_24 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_G723_40 && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_RAW :
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				if (subformat == SF_FORMAT_ALAW || subformat == SF_FORMAT_ULAW)
					return 1 ;
				if ((subformat == SF_FORMAT_DWVW_12 || subformat == SF_FORMAT_DWVW_16 ||
							subformat == SF_FORMAT_DWVW_24) && info-> channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_VOX_ADPCM && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_PAF :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				break ;

		case SF_FORMAT_SVX :
				/* SVX currently does not support more than one channel for write.
				** Read will allow more than one channel but only allow one here.
				*/
				if (info->channels != 1)
					return 0 ;
				/* Always big endian. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;

				if ((subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16) && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_NIST :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				break ;

		case SF_FORMAT_IRCAM :
				if (subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW || subformat == SF_FORMAT_FLOAT)
					return 1 ;
				break ;

		case SF_FORMAT_VOC :
				/* VOC is strictly little endian. */
				if (endian == SF_ENDIAN_BIG || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				break ;

		case SF_FORMAT_W64 :
				/* W64 is strictly little endian. */
				if (endian == SF_ENDIAN_BIG || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if ((subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM) && info->channels <= 2)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_MAT4 :
				if (subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_MAT5 :
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_PVF :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				break ;

		case SF_FORMAT_XI :
				if (info->channels != 1)
					return 0 ;
				if (subformat == SF_FORMAT_DPCM_8 || subformat == SF_FORMAT_DPCM_16)
					return 1 ;
				break ;

		case SF_FORMAT_HTK :
				/* HTK is strictly big endian. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (info->channels != 1)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_16)
					return 1 ;
				break ;

		case SF_FORMAT_SDS :
				/* SDS is strictly big endian. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (info->channels != 1)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_24)
					return 1 ;
				break ;

		case SF_FORMAT_AVR :
				/* SDS is strictly big endian. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (info->channels < 1 || info->channels > 2)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				break ;

		/*-
		case SF_FORMAT_SD2 :
				/+* SD2 is strictly big endian. *+/
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_16)
					return 1 ;
				break ;
		-*/

		default : break ;
		} ;

	return 0 ;
} /* sf_format_check */

/*------------------------------------------------------------------------------
*/

int
sf_command	(SNDFILE *sndfile, int command, void *data, int datasize)
{	SF_PRIVATE 		*psf = NULL ;

	/* This set of commands do not need the sndfile parameter. */
	switch (command)
	{	case SFC_GET_LIB_VERSION :
			if (data == NULL)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			if (ENABLE_EXPERIMENTAL_CODE)
				LSF_SNPRINTF (data, datasize, "%s-%s-exp", PACKAGE_NAME, PACKAGE_VERSION) ;
			else
				LSF_SNPRINTF (data, datasize, "%s-%s", PACKAGE_NAME, PACKAGE_VERSION) ;
			return 0 ;

		case SFC_GET_SIMPLE_FORMAT_COUNT :
			if (data == NULL || datasize != SIGNED_SIZEOF (int))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			*((int*) data) = psf_get_format_simple_count () ;
			return 0 ;

		case SFC_GET_SIMPLE_FORMAT :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_simple (data) ;

		case SFC_GET_FORMAT_MAJOR_COUNT :
			if (data == NULL || datasize != SIGNED_SIZEOF (int))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			*((int*) data) = psf_get_format_major_count () ;
			return 0 ;

		case SFC_GET_FORMAT_MAJOR :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_major (data) ;

		case SFC_GET_FORMAT_SUBTYPE_COUNT :
			if (data == NULL || datasize != SIGNED_SIZEOF (int))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			*((int*) data) = psf_get_format_subtype_count () ;
			return 0 ;

		case SFC_GET_FORMAT_SUBTYPE :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_subtype (data) ;

		case SFC_GET_FORMAT_INFO :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_info (data) ;
		} ;

	if (! sndfile && command == SFC_GET_LOG_INFO)
	{	if (data == NULL)
			return (psf->error = SFE_BAD_CONTROL_CMD) ;
		LSF_SNPRINTF (data, datasize, "%s", sf_logbuffer) ;
		return 0 ;
		} ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	switch (command)
	{	case SFC_SET_NORM_FLOAT :
			psf->norm_float = (datasize) ? SF_TRUE : SF_FALSE ;
			return psf->norm_float ;

		case SFC_SET_NORM_DOUBLE :
			psf->norm_double = (datasize) ? SF_TRUE : SF_FALSE ;
			return psf->norm_double ;

		case SFC_GET_NORM_FLOAT :
			return psf->norm_float ;

		case SFC_GET_NORM_DOUBLE :
			return psf->norm_double ;

		case SFC_SET_ADD_PEAK_CHUNK :
			{	int format = psf->sf.format & SF_FORMAT_TYPEMASK ;

				/* Only WAV and AIFF support the PEAK chunk. */
				if (format != SF_FORMAT_WAV && format != SF_FORMAT_WAVEX && format != SF_FORMAT_AIFF)
					return SF_FALSE ;

				format = psf->sf.format & SF_FORMAT_SUBMASK ;

				/* Only files containg the following data types support the PEAK chunk. */
				if (format != SF_FORMAT_FLOAT && format != SF_FORMAT_DOUBLE)
					return SF_FALSE ;

				} ;
			/* Can only do this is in SFM_WRITE mode. */
			if (psf->mode != SFM_WRITE)
				return SF_FALSE ;
			/* If data has already been written this must fail. */
			if (psf->have_written)
				return psf->has_peak ;
			/* Everything seems OK, so set psf->has_peak and re-write header. */
			psf->has_peak = (datasize) ? SF_TRUE : SF_FALSE ;
			if (psf->write_header)
				psf->write_header (psf, SF_TRUE) ;
			return psf->has_peak ;

		case SFC_GET_LOG_INFO :
			if (data == NULL)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			LSF_SNPRINTF (data, datasize, "%s", psf->logbuffer) ;
			break ;

		case SFC_CALC_SIGNAL_MAX :
			if (data == NULL || datasize != sizeof (double))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			*((double*) data) = psf_calc_signal_max (psf, SF_FALSE) ;
			break ;

		case SFC_CALC_NORM_SIGNAL_MAX :
			if (data == NULL || datasize != sizeof (double))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			*((double*) data) = psf_calc_signal_max (psf, SF_TRUE) ;
			break ;

		case SFC_CALC_MAX_ALL_CHANNELS :
			if (data == NULL || datasize != SIGNED_SIZEOF (double) * psf->sf.channels)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			return psf_calc_max_all_channels (psf, (double*) data, SF_FALSE) ;

		case SFC_CALC_NORM_MAX_ALL_CHANNELS :
			if (data == NULL || datasize != SIGNED_SIZEOF (double) * psf->sf.channels)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			return psf_calc_max_all_channels (psf, (double*) data, SF_TRUE) ;

		case SFC_UPDATE_HEADER_NOW :
			if (psf->write_header)
				psf->write_header (psf, SF_TRUE) ;
			break ;

		case SFC_SET_UPDATE_HEADER_AUTO :
			psf->auto_header = datasize ? SF_TRUE : SF_FALSE ;
			return psf->auto_header ;
			break ;

		case SFC_SET_ADD_DITHER_ON_WRITE :
		case SFC_SET_ADD_DITHER_ON_READ :
			/*
			** FIXME !
			** These are obsolete. Just return.
			** Remove some time after version 1.0.8.
			*/
			break ;

		case SFC_SET_DITHER_ON_WRITE :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_DITHER_INFO))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			memcpy (&psf->write_dither, data, sizeof (psf->write_dither)) ;
			if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
				dither_init (psf, SFM_WRITE) ;
			break ;

		case SFC_SET_DITHER_ON_READ :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_DITHER_INFO))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			memcpy (&psf->read_dither, data, sizeof (psf->read_dither)) ;
			if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
				dither_init (psf, SFM_READ) ;
			break ;

		case SFC_FILE_TRUNCATE :
			if (psf->mode != SFM_WRITE && psf->mode != SFM_RDWR)
				return SF_TRUE ;
			if (datasize != sizeof (sf_count_t))
				return SF_TRUE ;
			{	sf_count_t position ;

				position = *((sf_count_t*) data) ;

				if (sf_seek (sndfile, position, SEEK_SET) != position)
					return SF_TRUE ;

				psf->sf.frames = position ;

				position = psf_fseek (psf, 0, SEEK_CUR) ;

				return psf_ftruncate (psf, position) ;
				} ;
			break ;

		case SFC_SET_RAW_START_OFFSET :
			if (data == NULL || datasize != sizeof (sf_count_t))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;

			psf->dataoffset = *((sf_count_t*) data) ;
			sf_seek (sndfile, 0, SEEK_CUR) ;
			break ;

		case SFC_GET_EMBED_FILE_INFO :
			if (data == NULL || datasize != sizeof (SF_EMBED_FILE_INFO))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;

			((SF_EMBED_FILE_INFO*) data)->offset = psf->fileoffset ;
			((SF_EMBED_FILE_INFO*) data)->length = psf->filelength ;
			break ;

		/* Lite remove start */
		case SFC_TEST_IEEE_FLOAT_REPLACE :
			psf->ieee_replace = (datasize) ? SF_TRUE : SF_FALSE ;
			if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_FLOAT)
				float32_init (psf) ;
			else if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_DOUBLE)
				double64_init (psf) ;
			else
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			break ;
		/* Lite remove end */

		case SFC_SET_CLIPPING :
			psf->add_clipping = (datasize) ? SF_TRUE : SF_FALSE ;
			return psf->add_clipping ;

		case SFC_GET_CLIPPING :
			return psf->add_clipping ;

		default :
			/* Must be a file specific command. Pass it on. */
			if (psf->command)
				return psf->command (psf, command, data, datasize) ;

			psf_log_printf (psf, "*** sf_command : cmd = 0x%X\n", command) ;
			return (psf->error = SFE_BAD_CONTROL_CMD) ;
		} ;

	return 0 ;
} /* sf_command */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_seek	(SNDFILE *sndfile, sf_count_t offset, int whence)
{	SF_PRIVATE 	*psf ;
	sf_count_t	seek_from_start = 0, retval ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (! psf->sf.seekable)
	{	psf->error = SFE_NOT_SEEKABLE ;
		return	((sf_count_t) -1) ;
		} ;

	/* If the whence parameter has a mode ORed in, check to see that
	** it makes sense.
	*/
	if (((whence & SFM_MASK) == SFM_WRITE && psf->mode == SFM_READ) ||
			((whence & SFM_MASK) == SFM_WRITE && psf->mode == SFM_WRITE))
	{	psf->error = SFE_WRONG_SEEK ;
		return ((sf_count_t) -1) ;
		} ;

	/* Convert all SEEK_CUR and SEEK_END into seek_from_start to be
	** used with SEEK_SET.
	*/
	switch (whence)
	{	/* The SEEK_SET behaviour is independant of mode. */
		case SEEK_SET :
		case SEEK_SET | SFM_READ :
		case SEEK_SET | SFM_WRITE :
		case SEEK_SET | SFM_RDWR :
				seek_from_start = offset ;
				break ;

		/* The SEEK_CUR is a little more tricky. */
		case SEEK_CUR :
				if (offset == 0)
				{	if (psf->mode == SFM_READ)
						return psf->read_current ;
					if (psf->mode == SFM_WRITE)
						return psf->write_current ;
					} ;
				if (psf->mode == SFM_READ)
					seek_from_start = psf->read_current + offset ;
				else if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
					seek_from_start = psf->write_current + offset ;
				else
					psf->error = SFE_AMBIGUOUS_SEEK ;
				break ;

		case SEEK_CUR | SFM_READ :
				if (offset == 0)
					return psf->read_current ;
				seek_from_start = psf->read_current + offset ;
				break ;

		case SEEK_CUR | SFM_WRITE :
				if (offset == 0)
					return psf->write_current ;
				seek_from_start = psf->write_current + offset ;
				break ;

		/* The SEEK_END */
		case SEEK_END :
		case SEEK_END | SFM_READ :
		case SEEK_END | SFM_WRITE :
				seek_from_start = psf->sf.frames + offset ;
				break ;

		default :
				psf->error = SFE_BAD_SEEK ;
				break ;
		} ;

	if (psf->error)
		return ((sf_count_t) -1) ;

	if (seek_from_start < 0 || seek_from_start > psf->sf.frames)
	{	psf->error = SFE_BAD_SEEK ;
		return ((sf_count_t) -1) ;
		} ;

	if (psf->seek)
	{	int new_mode = (whence & SFM_MASK) ? (whence & SFM_MASK) : psf->mode ;

		retval = psf->seek (psf, new_mode, seek_from_start) ;

		switch (new_mode)
		{	case SFM_READ :
					psf->read_current = retval ;
					break ;
			case SFM_WRITE :
					psf->write_current = retval ;
					break ;
			case SFM_RDWR :
					psf->read_current = retval ;
					psf->write_current = retval ;
					new_mode = SFM_READ ;
					break ;
			} ;

		psf->last_op = new_mode ;

		return retval ;
		} ;

	psf->error = SFE_AMBIGUOUS_SEEK ;
	return ((sf_count_t) -1) ;
} /* sf_seek */

/*------------------------------------------------------------------------------
*/

const char*
sf_get_string (SNDFILE *sndfile, int str_type)
{	SF_PRIVATE 	*psf ;

	if ((psf = (SF_PRIVATE*) sndfile) == NULL)
		return NULL ;
	if (psf->Magick != SNDFILE_MAGICK)
		return NULL ;

	return psf_get_string (psf, str_type) ;
} /* sf_get_string */

int
sf_set_string (SNDFILE *sndfile, int str_type, const char* str)
{	SF_PRIVATE 	*psf ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	return psf_store_string (psf, str_type, str) ;
} /* sf_get_string */

/*==============================================================================
*/

sf_count_t
sf_read_raw		(SNDFILE *sndfile, void *ptr, sf_count_t bytes)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;
	int			bytewidth, blockwidth ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	bytewidth = (psf->bytewidth > 0) ? psf->bytewidth : 1 ;
	blockwidth = (psf->blockwidth > 0) ? psf->blockwidth : 1 ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return	0 ;
		} ;

	if (bytes < 0 || psf->read_current >= psf->datalength)
	{	psf_memset (ptr, 0, bytes) ;
		return 0 ;
		} ;

	if (bytes % (psf->sf.channels * bytewidth))
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	count = psf_fread (ptr, 1, bytes, psf) ;

	if (count < bytes)
		psf_memset (((char*) ptr) + count, 0, bytes - count) ;

	psf->read_current += count / blockwidth ;

	psf->last_op = SFM_READ ;

	return count ;
} /* sf_read_raw */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_short	(SNDFILE *sndfile, short *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (short)) ;
		return 0 ; /* End of file. */
		} ;

	if (! psf->read_short || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_short (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (short)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_short */

sf_count_t
sf_readf_short		(SNDFILE *sndfile, short *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (short)) ;
		return 0 ; /* End of file. */
		} ;

	if (! psf->read_short || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_short (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (short)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_short */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_int		(SNDFILE *sndfile, int *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (int)) ;
		return 0 ;
		} ;

	if (! psf->read_int || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_int (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (int)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_int */

sf_count_t
sf_readf_int	(SNDFILE *sndfile, int *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (int)) ;
		return 0 ;
		} ;

	if (! psf->read_int || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_int (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (int)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_int */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_float	(SNDFILE *sndfile, float *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (float)) ;
		return 0 ;
		} ;

	if (! psf->read_float || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_float (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (float)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_float */

sf_count_t
sf_readf_float	(SNDFILE *sndfile, float *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (float)) ;
		return 0 ;
		} ;

	if (! psf->read_float || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_float (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (float)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_float */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_double	(SNDFILE *sndfile, double *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (double)) ;
		return 0 ;
		} ;

	if (! psf->read_double || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_double (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (double)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_double */

sf_count_t
sf_readf_double	(SNDFILE *sndfile, double *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (double)) ;
		return 0 ;
		} ;

	if (! psf->read_double || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_double (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (double)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_double */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_raw	(SNDFILE *sndfile, void *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;
	int			bytewidth, blockwidth ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	bytewidth = (psf->bytewidth > 0) ? psf->bytewidth : 1 ;
	blockwidth = (psf->blockwidth > 0) ? psf->blockwidth : 1 ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % (psf->sf.channels * bytewidth))
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf_fwrite (ptr, 1, len, psf) ;

	psf->write_current += count / blockwidth ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	return count ;
} /* sf_write_raw */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_short	(SNDFILE *sndfile, short *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (! psf->write_short || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_short (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count ;
} /* sf_write_short */

sf_count_t
sf_writef_short	(SNDFILE *sndfile, short *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_short || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_short (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count / psf->sf.channels ;
} /* sf_writef_short */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_int	(SNDFILE *sndfile, int *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (! psf->write_int || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_int (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count ;
} /* sf_write_int */

sf_count_t
sf_writef_int	(SNDFILE *sndfile, int *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_int || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_int (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count / psf->sf.channels ;
} /* sf_writef_int */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_float	(SNDFILE *sndfile, float *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (! psf->write_float || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_float (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count ;
} /* sf_write_float */

sf_count_t
sf_writef_float	(SNDFILE *sndfile, float *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_float || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_float (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count / psf->sf.channels ;
} /* sf_writef_float */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_double	(SNDFILE *sndfile, double *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return	0 ;
		} ;

	if (! psf->write_double || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_double (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count ;
} /* sf_write_double */

sf_count_t
sf_writef_double	(SNDFILE *sndfile, double *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_double || psf->seek == NULL)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_double (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	return count / psf->sf.channels ;
} /* sf_writef_double */

/*=========================================================================
** Private functions.
*/

static int
format_from_extension (const char *filename)
{	char *cptr ;
	char buffer [16] ;

	if (filename == NULL)
		return 0 ;

	if ((cptr = strrchr (filename, '.')) == NULL)
		return 0 ;

	cptr ++ ;
	if (strlen (cptr) > sizeof (buffer) - 1)
		return 0 ;

	strncpy (buffer, cptr, sizeof (buffer)) ;
	buffer [sizeof (buffer) - 1] = 0 ;

	/* Convert everything in the buffer to lower case. */
	cptr = buffer ;
	while (*cptr)
	{	*cptr = tolower (*cptr) ;
		cptr ++ ;
		} ;

	cptr = buffer ;

	if (strcmp (cptr, "au") == 0)
		return SF_FORMAT_AU | SF_FORMAT_ULAW ;

	if (strcmp (cptr, "snd") == 0)
		return SF_FORMAT_AU | SF_FORMAT_ULAW ;

	if (strcmp (cptr, "vox") == 0)
		return SF_FORMAT_RAW | SF_FORMAT_VOX_ADPCM ;

	return 0 ;
} /* format_from_extension */

static int
guess_file_type (SF_PRIVATE *psf, const char *filename)
{	int 			buffer [3], format ;
	unsigned char	cptr [0x40] ;

	if (psf_binheader_readf (psf, "b", &buffer, SIGNED_SIZEOF (buffer)) != SIGNED_SIZEOF (buffer))
	{	psf->error = SFE_BAD_FILE_READ ;
		return 0 ;
		} ;

	if (buffer [0] == MAKE_MARKER ('R', 'I', 'F', 'F') && buffer [2] == MAKE_MARKER ('W', 'A', 'V', 'E'))
		return SF_FORMAT_WAV ;

	if (buffer [0] == MAKE_MARKER ('F', 'O', 'R', 'M'))
	{	if (buffer [2] == MAKE_MARKER ('A', 'I', 'F', 'F') || buffer [2] == MAKE_MARKER ('A', 'I', 'F', 'C'))
			return SF_FORMAT_AIFF ;
		if (buffer [2] == MAKE_MARKER ('8', 'S', 'V', 'X') || buffer [2] == MAKE_MARKER ('1', '6', 'S', 'V'))
			return SF_FORMAT_SVX ;
		return 0 ;
		} ;

	if (buffer [0] == MAKE_MARKER ('.', 's', 'n', 'd') || buffer [0] == MAKE_MARKER ('d', 'n', 's', '.'))
		return SF_FORMAT_AU ;

	if ((buffer [0] == MAKE_MARKER ('f', 'a', 'p', ' ') || buffer [0] == MAKE_MARKER (' ', 'p', 'a', 'f')))
		return SF_FORMAT_PAF ;

	if (buffer [0] == MAKE_MARKER ('N', 'I', 'S', 'T'))
		return SF_FORMAT_NIST ;

	if (buffer [0] == MAKE_MARKER ('C', 'r', 'e', 'a') && buffer [1] == MAKE_MARKER ('t', 'i', 'v', 'e'))
		return SF_FORMAT_VOC ;

	if ((buffer [0] & MAKE_MARKER (0xFF, 0xFF, 0xF8, 0xFF)) == MAKE_MARKER (0x64, 0xA3, 0x00, 0x00) ||
		(buffer [0] & MAKE_MARKER (0xFF, 0xF8, 0xFF, 0xFF)) == MAKE_MARKER (0x00, 0x00, 0xA3, 0x64))
		return SF_FORMAT_IRCAM ;

	if (buffer [0] == MAKE_MARKER ('r', 'i', 'f', 'f'))
		return SF_FORMAT_W64 ;

	if (buffer [0] == MAKE_MARKER (0, 0, 0x03, 0xE8) && buffer [1] == MAKE_MARKER (0, 0, 0, 1) &&
								buffer [2] == MAKE_MARKER (0, 0, 0, 1))
		return SF_FORMAT_MAT4 ;

	if (buffer [0] == MAKE_MARKER (0, 0, 0, 0) && buffer [1] == MAKE_MARKER (1, 0, 0, 0) &&
								buffer [2] == MAKE_MARKER (1, 0, 0, 0))
		return SF_FORMAT_MAT4 ;

	if (buffer [0] == MAKE_MARKER ('M', 'A', 'T', 'L') && buffer [1] == MAKE_MARKER ('A', 'B', ' ', '5'))
		return SF_FORMAT_MAT5 ;

	if (buffer [0] == MAKE_MARKER ('P', 'V', 'F', '1'))
		return SF_FORMAT_PVF ;

	if (buffer [0] == MAKE_MARKER ('E', 'x', 't', 'e') && buffer [1] == MAKE_MARKER ('n', 'd', 'e', 'd') &&
								buffer [2] == MAKE_MARKER (' ', 'I', 'n', 's'))
		return SF_FORMAT_XI ;

	if (ENABLE_EXPERIMENTAL_CODE && buffer [0] == MAKE_MARKER ('O', 'g', 'g', 'S'))
		return SF_FORMAT_OGG ;

	if (buffer [0] == MAKE_MARKER ('A', 'L', 'a', 'w') && buffer [1] == MAKE_MARKER ('S', 'o', 'u', 'n')
			&& buffer [2] == MAKE_MARKER ('d', 'F', 'i', 'l'))
		return SF_FORMAT_WVE ;

	if (buffer [0] == MAKE_MARKER ('D', 'i', 'a', 'm') && buffer [1] == MAKE_MARKER ('o', 'n', 'd', 'W')
			&& buffer [2] == MAKE_MARKER ('a', 'r', 'e', ' '))
		return SF_FORMAT_DWD ;

	if (buffer [0] == MAKE_MARKER ('L', 'M', '8', '9') || buffer [0] == MAKE_MARKER ('5', '3', 0, 0))
		return SF_FORMAT_TXW ;

	if ((buffer [0] & MAKE_MARKER (0xFF, 0xFF, 0x80, 0xFF)) == MAKE_MARKER (0xF0, 0x7E, 0, 0x01))
		return SF_FORMAT_SDS ;

	if (buffer [0] == MAKE_MARKER ('C', 'A', 'T', ' ') && buffer [2] == MAKE_MARKER ('R', 'E', 'X', '2'))
		return SF_FORMAT_REX2 ;

	if (buffer [0] == MAKE_MARKER (0x30, 0x26, 0xB2, 0x75) && buffer [1] == MAKE_MARKER (0x8E, 0x66, 0xCF, 0x11))
		return 0 /*-SF_FORMAT_WMA-*/ ;

	/* HMM (Hidden Markov Model) Tool Kit. */
	if (2 * BEI2H_INT (buffer [0]) + 12 == psf->filelength && buffer [2] == MAKE_MARKER (0, 2, 0, 0))
		return SF_FORMAT_HTK ;

	if (buffer [0] == MAKE_MARKER ('f', 'L', 'a', 'C'))
		return 0 /*-SF_FORMAT_FLAC-*/ ;

	/* Turtle Beach SMP 16-bit */
	if (buffer [0] == MAKE_MARKER ('S', 'O', 'U', 'N') && buffer [1] == MAKE_MARKER ('D', ' ', 'S', 'A'))
		return 0 ;

	if (buffer [0] == MAKE_MARKER ('S', 'Y', '8', '0') || buffer [0] == MAKE_MARKER ('S', 'Y', '8', '5'))
		return 0 ;

	if (buffer [0] == MAKE_MARKER ('a', 'j', 'k', 'g'))
		return 0 /*-SF_FORMAT_SHN-*/ ;

	if (buffer [0] == MAKE_MARKER ('2', 'B', 'I', 'T'))
		return SF_FORMAT_AVR ;

	if (OS_IS_MACOSX && (format = macos_guess_file_type (psf, filename)) != 0)
		return format ;

	/*	Detect wacky MacOS header stuff. This might be "Sound Designer II". */
	memcpy (cptr , buffer, sizeof (buffer)) ;
	if (cptr [0] == 0 && cptr [1] > 0 && psf->sf.seekable)
	{	psf_binheader_readf (psf, "pb", 0, &cptr, SIGNED_SIZEOF (cptr)) ;

		if (cptr [1] < (sizeof (cptr) - 3) && cptr [cptr [1] + 2] == 0 && strlen (((char*) cptr) + 2) == cptr [1])
		{	psf_log_printf (psf, "Weird MacOS Header.\n") ;
			psf_binheader_readf (psf, "em", &buffer) ;
			if (buffer [0] == MAKE_MARKER (0, 'S', 'd', '2'))
				return SF_FORMAT_SD2 ;
			} ;
		} ;

	/* This must be the last one. */
	if ((format = format_from_extension (filename)) != 0)
		return format ;

	/* Default to header-less RAW PCM file type. */
	return SF_FORMAT_RAW ;
} /* guess_file_type */


static int
validate_sfinfo (SF_INFO *sfinfo)
{	if (sfinfo->samplerate < 1)
		return 0 ;
	if (sfinfo->frames < 0)
		return 0 ;
	if (sfinfo->channels < 1)
		return 0 ;
	if ((sfinfo->format & SF_FORMAT_TYPEMASK) == 0)
		return 0 ;
	if ((sfinfo->format & SF_FORMAT_SUBMASK) == 0)
		return 0 ;
	if (sfinfo->sections < 1)
		return 0 ;
	return 1 ;
} /* validate_sfinfo */

static int
validate_psf (SF_PRIVATE *psf)
{
	if (psf->datalength < 0)
	{	psf_log_printf (psf, "Invalid SF_PRIVATE field : datalength == %D.\n", psf->datalength) ;
		return 0 ;
		} ;
	if (psf->dataoffset < 0)
	{	psf_log_printf (psf, "Invalid SF_PRIVATE field : dataoffset == %D.\n", psf->dataoffset) ;
		return 0 ;
		} ;
	if (psf->blockwidth && psf->blockwidth != psf->sf.channels * psf->bytewidth)
	{	psf_log_printf (psf, "Invalid SF_PRIVATE field : channels * bytewidth == %d.\n",
								psf->sf.channels * psf->bytewidth) ;
		return 0 ;
		} ;
	return 1 ;
} /* validate_psf */

static void
save_header_info (SF_PRIVATE *psf)
{	LSF_SNPRINTF (sf_logbuffer, sizeof (sf_logbuffer), "%s", psf->logbuffer) ;
} /* save_header_info */

static void
copy_filename (SF_PRIVATE *psf, const char *path)
{	const char *cptr ;

	if ((cptr = strrchr (path, '/')) || (cptr = strrchr (path, '\\')))
		cptr ++ ;
	else
		cptr = path ;

	memset (psf->filename, 0, SF_FILENAME_LEN) ;

	LSF_SNPRINTF (psf->filename, sizeof (psf->filename), "%s", cptr) ;

} /* copy_filename */

/*==============================================================================
*/

static int
psf_close (SF_PRIVATE *psf)
{	int			error ;

	if (psf->close)
		error = psf->close (psf) ;

	psf_fclose (psf) ;

	if (psf->fdata)
		free (psf->fdata) ;

	if (psf->interleave)
		free (psf->interleave) ;

	if (psf->dither)
		free (psf->dither) ;

	if (psf->pchunk)
		free (psf->pchunk) ;

	if (psf->format_desc)
	{	memset (psf->format_desc, 0, strlen (psf->format_desc)) ;
		free (psf->format_desc) ;
		} ;

	memset (psf, 0, sizeof (SF_PRIVATE)) ;
	free (psf) ;

	return 0 ;
} /* psf_close */

static int
psf_open_file (SF_PRIVATE *psf, int mode, SF_INFO *sfinfo)
{	int		error, format ;

	if (mode != SFM_READ && mode != SFM_WRITE && mode != SFM_RDWR)
		return SFE_BAD_OPEN_MODE ;

	if (sfinfo == NULL)
		return SFE_BAD_SF_INFO_PTR ;

	if (mode == SFM_READ)
	{	if ((sfinfo->format & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		{	if (sf_format_check (sfinfo) == 0)
				return SFE_RAW_BAD_FORMAT ;
			}
		else
			memset (sfinfo, 0, sizeof (SF_INFO)) ;
		} ;

	sf_errno = error = 0 ;
	sf_logbuffer [0] = 0 ;

	memcpy (&(psf->sf), sfinfo, sizeof (SF_INFO)) ;

	psf->Magick 		= SNDFILE_MAGICK ;
	psf->norm_float 	= SF_TRUE ;
	psf->norm_double	= SF_TRUE ;
	psf->mode 			= mode ;
	psf->dataoffset		= -1 ;
	psf->datalength		= -1 ;
	psf->read_current	= -1 ;
	psf->write_current	= -1 ;
	psf->auto_header 	= SF_FALSE ;
	psf->rwf_endian		= SF_ENDIAN_LITTLE ;
	psf->seek			= psf_default_seek ;

	psf->sf.sections = 1 ;

	psf->is_pipe = psf_is_pipe (psf) ;

	if (psf->is_pipe)
	{	psf->sf.seekable = SF_FALSE ;
		psf->filelength = SF_COUNT_MAX ;
		}
	else
	{	psf->sf.seekable = SF_TRUE ;

		/* File is open, so get the length. */
		psf->filelength = psf_get_filelen (psf) ;
		} ;

	if (psf->fileoffset > 0)
	{	switch (psf->mode)
		{	case SFM_READ :
				if (psf->filelength < 44)
				{	psf_log_printf (psf, "Short filelength: %D (fileoffset: %D)\n", psf->filelength, psf->fileoffset) ;
					return SFE_BAD_OFFSET ;
					} ;
				break ;

			case SFM_WRITE :
				psf->fileoffset = 0 ;
				psf_fseek (psf, 0, SEEK_END) ;
				psf->fileoffset = psf_ftell (psf) ;
				break ;

			case SFM_RDWR :
				return SFE_NO_EMBEDDED_RDWR ;
			} ;

		psf_log_printf (psf, "Embedded file offset : %D\n", psf->fileoffset) ;
		} ;

	if (psf->filelength == SF_COUNT_MAX)
		psf_log_printf (psf, "Length : unknown\n") ;
	else
		psf_log_printf (psf, "Length : %D\n", psf->filelength) ;

	if (mode == SFM_WRITE || (mode == SFM_RDWR && psf->filelength == 0))
	{	/* If the file is being opened for write or RDWR and the file is currently
		** empty, then the SF_INFO struct must contain valid data.
		*/
		if (sf_format_check (&(psf->sf)) == 0)
			return SFE_BAD_OPEN_FORMAT ;
		}
	else if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
	{	/* If type RAW has not been specified then need to figure out file type. */
		psf->sf.format = guess_file_type (psf, psf->filename) ;
		} ;

	/* Prevent unnecessary seeks */
	psf->last_op = psf->mode ;

	/* Set bytewidth if known. */
	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_ULAW :
		case SF_FORMAT_ALAW :
		case SF_FORMAT_DPCM_8 :
				psf->bytewidth = 1 ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_DPCM_16 :
				psf->bytewidth = 2 ;
				break ;

		case SF_FORMAT_PCM_24 :
				psf->bytewidth = 3 ;
				break ;

		case SF_FORMAT_PCM_32 :
		case SF_FORMAT_FLOAT :
				psf->bytewidth = 4 ;
				break ;

		case SF_FORMAT_DOUBLE :
				psf->bytewidth = 8 ;
				break ;
		} ;

	/* Call the initialisation function for the relevant file type. */
	switch (psf->sf.format & SF_FORMAT_TYPEMASK)
	{	case	SF_FORMAT_WAV :
		case	SF_FORMAT_WAVEX :
				error = wav_open (psf) ;
				break ;

		case	SF_FORMAT_AIFF :
				error = aiff_open (psf) ;
				break ;

		case	SF_FORMAT_AU :
				error = au_open (psf) ;
				break ;

		case	SF_FORMAT_AU | SF_FORMAT_ULAW :
				error = au_nh_open (psf) ;
				break ;

		case	SF_FORMAT_RAW :
				error = raw_open (psf) ;
				break ;

		case	SF_FORMAT_W64 :
				error = w64_open (psf) ;
				break ;

		/* Lite remove start */
		case	SF_FORMAT_PAF :
				error = paf_open (psf) ;
				break ;

		case	SF_FORMAT_SVX :
				error = svx_open (psf) ;
				break ;

		case	SF_FORMAT_NIST :
				error = nist_open (psf) ;
				break ;

		case	SF_FORMAT_IRCAM :
				error = ircam_open (psf) ;
				break ;

		case	SF_FORMAT_VOC :
				error = voc_open (psf) ;
				break ;

		case	SF_FORMAT_SDS :
				error = sds_open (psf) ;
				break ;

		case	SF_FORMAT_OGG :
				error = ogg_open (psf) ;
				break ;

		case	SF_FORMAT_TXW :
				error = txw_open (psf) ;
				break ;

		case	SF_FORMAT_WVE :
				error = wve_open (psf) ;
				break ;

		case	SF_FORMAT_DWD :
				error = dwd_open (psf) ;
				break ;

		case	SF_FORMAT_MAT4 :
				error = mat4_open (psf) ;
				break ;

		case	SF_FORMAT_MAT5 :
				error = mat5_open (psf) ;
				break ;

		case	SF_FORMAT_PVF :
				error = pvf_open (psf) ;
				break ;

		case	SF_FORMAT_XI :
				error = xi_open (psf) ;
				break ;

		case	SF_FORMAT_HTK :
				error = htk_open (psf) ;
				break ;

		case	SF_FORMAT_SD2 :
				error = sd2_open (psf) ;
				break ;

		case	SF_FORMAT_REX2 :
				error = rx2_open (psf) ;
				break ;

		case	SF_FORMAT_AVR :
				error = avr_open (psf) ;
				break ;

		/* Lite remove end */

		default :
				error = SFE_UNKNOWN_FORMAT ;
		} ;

	if (error)
		return error ;

	/* For now, check whether embedding is supported. */
	format = psf->sf.format & SF_FORMAT_TYPEMASK ;
	if (psf->fileoffset > 0 &&
			(format != SF_FORMAT_WAV) && (format != SF_FORMAT_WAVEX) &&
			(format != SF_FORMAT_AIFF) && (format != SF_FORMAT_AU)
			)
		return SFE_NO_EMBED_SUPPORT ;

	if (psf->fileoffset > 0)
		psf_log_printf (psf, "Embedded file length : %D\n", psf->filelength) ;

	if (mode == SFM_RDWR && sf_format_check (&(psf->sf)) == 0)
		return SFE_BAD_RDWR_FORMAT ;

	if (validate_sfinfo (&(psf->sf)) == 0)
	{	psf_log_SF_INFO (psf) ;
		save_header_info (psf) ;
		return SFE_BAD_SF_INFO ;
		} ;

	if (validate_psf (psf) == 0)
	{	save_header_info (psf) ;
		return SFE_INTERNAL ;
		} ;

	psf->read_current = 0 ;
	psf->write_current = (psf->mode == SFM_RDWR) ? psf->sf.frames : 0 ;

	memcpy (sfinfo, &(psf->sf), sizeof (SF_INFO)) ;

	return 0 ;
} /* psf_open_file */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: cd4f9e91-a8ec-4154-9bf6-fe4b8c69a615
*/
/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<string.h>
#include	<math.h>


#define STRINGS_DEBUG 0
#if STRINGS_DEBUG
static void hexdump (void *data, int len) ;
#endif

int
psf_store_string (SF_PRIVATE *psf, int str_type, const char *str)
{	static char lsf_name [] = PACKAGE "-" VERSION ;
	static char bracket_name [] = " (" PACKAGE "-" VERSION ")" ;
	int		k, str_len, len_remaining, str_flags ;

	if (str == NULL)
		return SFE_STR_BAD_STRING ;

	str_len = strlen (str) ;

	/* A few extra checks for write mode. */
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->str_flags & SF_STR_ALLOW_START) == 0)
			return SFE_STR_NO_SUPPORT ;
		if ((psf->str_flags & SF_STR_ALLOW_END) == 0)
			return SFE_STR_NO_SUPPORT ;
		/* Only allow zero length strings for software. */
		if (str_type != SF_STR_SOFTWARE && str_len == 0)
			return SFE_STR_BAD_STRING ;
		} ;

	/* Determine flags */
	str_flags = SF_STR_LOCATE_START ;
	if (psf->have_written)
	{	if ((psf->str_flags & SF_STR_ALLOW_END) == 0)
			return SFE_STR_NO_ADD_END ;
		str_flags = SF_STR_LOCATE_END ;
		} ;

	/* Find next free slot in table. */
	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
		if (psf->strings [k].type == 0)
			break ;

	/* More sanity checking. */
	if (k >= SF_MAX_STRINGS)
		return SFE_STR_MAX_COUNT ;

	if (k == 0 && psf->str_end != NULL)
	{	psf_log_printf (psf, "SFE_STR_WEIRD : k == 0 && psf->str_end != NULL\n") ;
		return SFE_STR_WEIRD ;
		} ;

	if (k != 0 && psf->str_end == NULL)
	{	psf_log_printf (psf, "SFE_STR_WEIRD : k != 0 && psf->str_end == NULL\n") ;
		return SFE_STR_WEIRD ;
		} ;

	/* Special case for the first string. */
	if (k == 0)
		psf->str_end = psf->str_storage ;


#if STRINGS_DEBUG
	psf_log_printf (psf, "str_storage      : %X\n", (int) psf->str_storage) ;
	psf_log_printf (psf, "str_end          : %X\n", (int) psf->str_end) ;
	psf_log_printf (psf, "sizeof (storage) : %d\n", SIGNED_SIZEOF (psf->str_storage)) ;
#endif

	len_remaining = SIGNED_SIZEOF (psf->str_storage) - (psf->str_end - psf->str_storage) ;

	if (len_remaining < str_len + 2)
		return SFE_STR_MAX_DATA ;

	switch (str_type)
	{	case SF_STR_SOFTWARE :
				/* In write mode, want to append libsndfile-version to string. */
				if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
				{	psf->strings [k].type = str_type ;
					psf->strings [k].str = psf->str_end ;
					psf->strings [k].flags = str_flags ;

					memcpy (psf->str_end, str, str_len + 1) ;
					psf->str_end += str_len ;

					/*
					** If the supplied string does not already contain a
					** libsndfile-X.Y.Z component, then add it.
					*/
					if (strstr (str, PACKAGE) == NULL && len_remaining > (int) (strlen (bracket_name) + str_len + 2))
					{	if (strlen (str) == 0)
							strncat (psf->str_end, lsf_name, len_remaining) ;
						else
							strncat (psf->str_end, bracket_name, len_remaining) ;
						psf->str_end += strlen (psf->str_end) ;
						} ;

					/* Plus one to catch string terminator. */
					psf->str_end += 1 ;
					break ;
					} ;

				/* Fall though if not write mode. */

		case SF_STR_TITLE :
		case SF_STR_COPYRIGHT :
		case SF_STR_ARTIST :
		case SF_STR_COMMENT :
		case SF_STR_DATE :
				psf->strings [k].type = str_type ;
				psf->strings [k].str = psf->str_end ;
				psf->strings [k].flags = str_flags ;

				/* Plus one to catch string terminator. */
				memcpy (psf->str_end, str, str_len + 1) ;
				psf->str_end += str_len + 1 ;
				break ;

		default :
			return SFE_STR_BAD_TYPE ;
		} ;

	psf->str_flags |= (psf->have_written) ? SF_STR_LOCATE_END : SF_STR_LOCATE_START ;

#if STRINGS_DEBUG
	hexdump (psf->str_storage, 300) ;
#endif

	return 0 ;
} /* psf_store_string */

const char*
psf_get_string (SF_PRIVATE *psf, int str_type)
{	int k ;

	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
		if (str_type == psf->strings [k].type)
			return psf->strings [k].str ;

	return NULL ;
} /* psf_get_string */

#if STRINGS_DEBUG

#include <ctype.h>
static void
hexdump (void *data, int len)
{	unsigned char *ptr ;
	int k ;

	ptr = data ;

	puts ("---------------------------------------------------------") ;
	while (len >= 16)
	{	for (k = 0 ; k < 16 ; k++)
			printf ("%02X ", ptr [k] & 0xFF) ;
		printf ("   ") ;
		for (k = 0 ; k < 16 ; k++)
			printf ("%c", isprint (ptr [k]) ? ptr [k] : '.') ;
		puts ("") ;
		ptr += 16 ;
		len -= 16 ;
		} ;
} /* hexdump */

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 04393aa1-9389-46fe-baf2-58a7bd544fd6
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>



/*------------------------------------------------------------------------------
 * Macros to handle big/little endian issues.
*/

#define FORM_MARKER	(MAKE_MARKER ('F', 'O', 'R', 'M'))
#define SVX8_MARKER	(MAKE_MARKER ('8', 'S', 'V', 'X'))
#define SV16_MARKER	(MAKE_MARKER ('1', '6', 'S', 'V'))
#define VHDR_MARKER	(MAKE_MARKER ('V', 'H', 'D', 'R'))
#define BODY_MARKER	(MAKE_MARKER ('B', 'O', 'D', 'Y'))

#define ATAK_MARKER	(MAKE_MARKER ('A', 'T', 'A', 'K'))
#define RLSE_MARKER	(MAKE_MARKER ('R', 'L', 'S', 'E'))

#define c_MARKER	(MAKE_MARKER ('(', 'c', ')', ' '))
#define NAME_MARKER	(MAKE_MARKER ('N', 'A', 'M', 'E'))
#define AUTH_MARKER	(MAKE_MARKER ('A', 'U', 'T', 'H'))
#define ANNO_MARKER	(MAKE_MARKER ('A', 'N', 'N', 'O'))
#define CHAN_MARKER	(MAKE_MARKER ('C', 'H', 'A', 'N'))

/*------------------------------------------------------------------------------
 * Typedefs for file chunks.
*/

typedef struct
{	unsigned int	oneShotHiSamples, repeatHiSamples, samplesPerHiCycle ;
	unsigned short	samplesPerSec ;
	unsigned char	octave, compression ;
	unsigned int	volume ;
} VHDR_CHUNK ;

enum {
	svxHAVE_FORM	= 0x01,

	HAVE_SVX	= 0x02,
	HAVE_VHDR	= 0x04,
	HAVE_BODY	= 0x08
} ;

/*------------------------------------------------------------------------------
 * Private static functions.
*/

static int	svx_close	(SF_PRIVATE *psf) ;
static int	svx_write_header (SF_PRIVATE *psf, int calc_length) ;
static int 	svx_read_header	(SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
svx_open	(SF_PRIVATE *psf)
{	int error, subformat ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = svx_read_header (psf)))
			return error ;

		psf->endian = SF_ENDIAN_BIG ;			/* All SVX files are big endian. */

		psf->blockwidth = psf->sf.channels * psf->bytewidth ;
		if (psf->blockwidth)
			psf->sf.frames = psf->datalength / psf->blockwidth ;

		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_SVX)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = psf->sf.format & SF_FORMAT_ENDMASK ;

		if (psf->endian == SF_ENDIAN_LITTLE || (CPU_IS_LITTLE_ENDIAN && psf->endian == SF_ENDIAN_CPU))
			return SFE_BAD_ENDIAN ;

		psf->endian = SF_ENDIAN_BIG ;			/* All SVX files are big endian. */

		error = svx_write_header (psf, SF_FALSE) ;
		if (error)
			return error ;

		psf->write_header = svx_write_header ;
		} ;

	psf->close = svx_close ;

	if ((error = pcm_init (psf)))
		return error ;

	return 0 ;
} /* svx_open */

/*------------------------------------------------------------------------------
*/

static int
svx_read_header	(SF_PRIVATE *psf)
{	VHDR_CHUNK		vhdr ;
	unsigned int	FORMsize, vhdrsize, dword, marker ;
	int				filetype = 0, parsestage = 0, done = 0 ;
	int 			bytecount = 0, channels ;

	psf_binheader_readf (psf, "p", 0) ;

	/* Set default number of channels. */
	psf->sf.channels = 1 ;

	psf->sf.format = SF_FORMAT_SVX ;

	while (! done)
	{	psf_binheader_readf (psf, "m", &marker) ;
		switch (marker)
		{	case FORM_MARKER :
					if (parsestage)
						return SFE_SVX_NO_FORM ;

					psf_binheader_readf (psf, "E4", &FORMsize) ;

					if (FORMsize != psf->filelength - 2 * sizeof (dword))
					{	dword = psf->filelength - 2 * sizeof (dword) ;
						psf_log_printf (psf, "FORM : %d (should be %d)\n", FORMsize, dword) ;
						FORMsize = dword ;
						}
					else
						psf_log_printf (psf, "FORM : %d\n", FORMsize) ;
					parsestage |= svxHAVE_FORM ;
					break ;

			case SVX8_MARKER :
			case SV16_MARKER :
					if (! (parsestage & svxHAVE_FORM))
						return SFE_SVX_NO_FORM ;
					filetype = marker ;
					psf_log_printf (psf, " %M\n", marker) ;
					parsestage |= HAVE_SVX ;
					break ;

			case VHDR_MARKER :
					if (! (parsestage & (svxHAVE_FORM | HAVE_SVX)))
						return SFE_SVX_NO_FORM ;

					psf_binheader_readf (psf, "E4", &vhdrsize) ;

					psf_log_printf (psf, " VHDR : %d\n", vhdrsize) ;

					psf_binheader_readf (psf, "E4442114", &(vhdr.oneShotHiSamples), &(vhdr.repeatHiSamples),
						&(vhdr.samplesPerHiCycle), &(vhdr.samplesPerSec), &(vhdr.octave), &(vhdr.compression),
						&(vhdr.volume)) ;

					psf_log_printf (psf, "  OneShotHiSamples  : %d\n", vhdr.oneShotHiSamples) ;
					psf_log_printf (psf, "  RepeatHiSamples   : %d\n", vhdr.repeatHiSamples) ;
					psf_log_printf (psf, "  samplesPerHiCycle : %d\n", vhdr.samplesPerHiCycle) ;
					psf_log_printf (psf, "  Sample Rate       : %d\n", vhdr.samplesPerSec) ;
					psf_log_printf (psf, "  Octave            : %d\n", vhdr.octave) ;

					psf_log_printf (psf, "  Compression       : %d => ", vhdr.compression) ;

					switch (vhdr.compression)
					{	case 0 : psf_log_printf (psf, "None.\n") ;
								break ;
						case 1 : psf_log_printf (psf, "Fibonacci delta\n") ;
								break ;
						case 2 : psf_log_printf (psf, "Exponential delta\n") ;
								break ;
						} ;

					psf_log_printf (psf, "  Volume            : %d\n", vhdr.volume) ;

					psf->sf.samplerate 	= vhdr.samplesPerSec ;

					if (filetype == SVX8_MARKER)
					{	psf->sf.format |= SF_FORMAT_PCM_S8 ;
						psf->bytewidth = 1 ;
						}
					else if (filetype == SV16_MARKER)
					{	psf->sf.format |= SF_FORMAT_PCM_16 ;
						psf->bytewidth = 2 ;
						} ;

					parsestage |= HAVE_VHDR ;
					break ;

			case BODY_MARKER :
					if (! (parsestage & HAVE_VHDR))
						return SFE_SVX_NO_BODY ;

					psf_binheader_readf (psf, "E4", &dword) ;
					psf->datalength = dword ;

					psf->dataoffset = psf_ftell (psf) ;

					if (psf->datalength > psf->filelength - psf->dataoffset)
					{	psf_log_printf (psf, " BODY : %D (should be %D)\n", psf->datalength, psf->filelength - psf->dataoffset) ;
						psf->datalength = psf->filelength - psf->dataoffset ;
						}
					else
						psf_log_printf (psf, " BODY : %D\n", psf->datalength) ;

					parsestage |= HAVE_BODY ;

					if (! psf->sf.seekable)
						break ;

					psf_fseek (psf, psf->datalength, SEEK_CUR) ;
					break ;

			case NAME_MARKER :
					if (! (parsestage & HAVE_SVX))
						return SFE_SVX_NO_FORM ;

					psf_binheader_readf (psf, "E4", &dword) ;

					psf_log_printf (psf, " %M : %d\n", marker, dword) ;

					if (strlen (psf->filename) != dword)
					{	if (dword > sizeof (psf->filename) - 1)
							return SFE_SVX_BAD_NAME_LENGTH ;

						psf_binheader_readf (psf, "b", psf->filename, dword) ;
						psf->filename [dword] = 0 ;
						}
					else
						psf_binheader_readf (psf, "j", dword) ;
					break ;

			case ANNO_MARKER :
					if (! (parsestage & HAVE_SVX))
						return SFE_SVX_NO_FORM ;

					psf_binheader_readf (psf, "E4", &dword) ;

					psf_log_printf (psf, " %M : %d\n", marker, dword) ;

					psf_binheader_readf (psf, "j", dword) ;
					break ;

			case CHAN_MARKER :
					if (! (parsestage & HAVE_SVX))
						return SFE_SVX_NO_FORM ;

					psf_binheader_readf (psf, "E4", &dword) ;

					psf_log_printf (psf, " %M : %d\n", marker, dword) ;

					bytecount += psf_binheader_readf (psf, "E4", &channels) ;

					psf_log_printf (psf, "  Channels : %d => %d\n", channels) ;

					psf_binheader_readf (psf, "j", dword - bytecount) ;
					break ;


			case AUTH_MARKER :
			case c_MARKER :
					if (! (parsestage & HAVE_SVX))
						return SFE_SVX_NO_FORM ;

					psf_binheader_readf (psf, "E4", &dword) ;

					psf_log_printf (psf, " %M : %d\n", marker, dword) ;

					psf_binheader_readf (psf, "j", dword) ;
					break ;

			default :
					if (isprint ((marker >> 24) & 0xFF) && isprint ((marker >> 16) & 0xFF)
						&& isprint ((marker >> 8) & 0xFF) && isprint (marker & 0xFF))
					{	psf_binheader_readf (psf, "E4", &dword) ;

						psf_log_printf (psf, "%M : %d (unknown marker)\n", marker, dword) ;

						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;
					if ((dword = psf_ftell (psf)) & 0x03)
					{	psf_log_printf (psf, "  Unknown chunk marker at position %d. Resynching.\n", dword - 4) ;

						psf_binheader_readf (psf, "j", -3) ;
						break ;
						} ;
					psf_log_printf (psf, "*** Unknown chunk marker : %X. Exiting parser.\n", marker) ;
					done = 1 ;
			} ;	/* switch (marker) */

		if (! psf->sf.seekable && (parsestage & HAVE_BODY))
			break ;

		if (psf_ftell (psf) >= psf->filelength - SIGNED_SIZEOF (dword))
			break ;
		} ; /* while (1) */

	if (vhdr.compression)
		return SFE_SVX_BAD_COMP ;

	if (psf->dataoffset <= 0)
		return SFE_SVX_NO_DATA ;

	return 0 ;
} /* svx_read_header */

static int
svx_close (SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		svx_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* svx_close */

static int
svx_write_header (SF_PRIVATE *psf, int calc_length)
{	static	char 	annotation	[] = "libsndfile by Erik de Castro Lopo\0\0\0" ;
	sf_count_t	current ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;

		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	/* FORM marker and FORM size. */
	psf_binheader_writef (psf, "Etm8", FORM_MARKER, (psf->filelength < 8) ?
			psf->filelength * 0 : psf->filelength - 8) ;

	psf_binheader_writef (psf, "m", (psf->bytewidth == 1) ? SVX8_MARKER : SV16_MARKER) ;

	/* VHDR chunk. */
	psf_binheader_writef (psf, "Em4", VHDR_MARKER, sizeof (VHDR_CHUNK)) ;
	/* VHDR : oneShotHiSamples, repeatHiSamples, samplesPerHiCycle */
	psf_binheader_writef (psf, "E444", psf->sf.frames, 0, 0) ;
	/* VHDR : samplesPerSec, octave, compression */
	psf_binheader_writef (psf, "E211", psf->sf.samplerate, 1, 0) ;
	/* VHDR : volume */
	psf_binheader_writef (psf, "E4", (psf->bytewidth == 1) ? 0xFF : 0xFFFF) ;

	/* Filename and annotation strings. */
	psf_binheader_writef (psf, "Emsms", NAME_MARKER, psf->filename, ANNO_MARKER, annotation) ;

	/* BODY marker and size. */
	psf_binheader_writef (psf, "Etm8", BODY_MARKER, (psf->datalength < 0) ?
			psf->datalength * 0 : psf->datalength) ;

	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* svx_write_header */


/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: a80ab6fb-7d75-4d32-a6b0-0061a3f05d95
*/
/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*  Most of these tables are inlined at their point of use.
 */

/*  4.4 TABLES USED IN THE FIXED POINT IMPLEMENTATION OF THE RPE-LTP
 *      CODER AND DECODER
 *
 *	(Most of them inlined, so watch out.)
 */

#define	GSM_TABLE_C

/*  Table 4.1  Quantization of the Log.-Area Ratios
 */
/* i 		     1      2      3        4      5      6        7       8 */
word gsm_A[8]   = {20480, 20480, 20480,  20480,  13964,  15360,   8534,  9036};
word gsm_B[8]   = {    0,     0,  2048,  -2560,     94,  -1792,   -341, -1144};
word gsm_MIC[8] = { -32,   -32,   -16,    -16,     -8,     -8,     -4,    -4 };
word gsm_MAC[8] = {  31,    31,    15,     15,      7,      7,      3,     3 };


/*  Table 4.2  Tabulation  of 1/A[1..8]
 */
word gsm_INVA[8]={ 13107, 13107,  13107, 13107,  19223, 17476,  31454, 29708 };


/*   Table 4.3a  Decision level of the LTP gain quantizer
 */
/*  bc		      0	        1	  2	     3			*/
word gsm_DLB[4] = {  6554,    16384,	26214,	   32767	};


/*   Table 4.3b   Quantization levels of the LTP gain quantizer
 */
/* bc		      0          1        2          3			*/
word gsm_QLB[4] = {  3277,    11469,	21299,	   32767	};


/*   Table 4.4	 Coefficients of the weighting filter
 */
/* i		    0      1   2    3   4      5      6     7   8   9    10  */
word gsm_H[11] = {-134, -374, 0, 2054, 5741, 8192, 5741, 2054, 0, -374, -134 };


/*   Table 4.5 	 Normalized inverse mantissa used to compute xM/xmax 
 */
/* i		 	0        1    2      3      4      5     6      7   */
word gsm_NRFAC[8] = { 29128, 26215, 23832, 21846, 20165, 18725, 17476, 16384 };


/*   Table 4.6	 Normalized direct mantissa used to compute xM/xmax
 */
/* i                  0      1       2      3      4      5      6      7   */
word gsm_FAC[8]	= { 18431, 20479, 22527, 24575, 26623, 28671, 30719, 32767 };
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 8957c531-e6b0-4097-9202-da7ca42729ca
*/

/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*===========================================================================
** Yamaha TX16 Sampler Files.
**
** This header parser was written using information from the SoX source code
** and trial and error experimentation. The code here however is all original.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
txw_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* txw_open */

#else

/*------------------------------------------------------------------------------
** Markers.
*/

#define TXW_DATA_OFFSET		32

#define	TXW_LOOPED		 	0x49
#define	TXW_NO_LOOP		 	0xC9

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int txw_read_header (SF_PRIVATE *psf) ;

static sf_count_t txw_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t txw_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t txw_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t txw_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t txw_seek (SF_PRIVATE *psf, int mode, sf_count_t offset) ;

/*------------------------------------------------------------------------------
** Public functions.
*/

/*
 * ftp://ftp.t0.or.at/pub/sound/tx16w/samples.yamaha
 * ftp://ftp.t0.or.at/pub/sound/tx16w/faq/tx16w.tec
 * http://www.t0.or.at/~mpakesch/tx16w/
 *
 * from tx16w.c sox 12.15: (7-Oct-98) (Mark Lakata and Leigh Smith)
 *  char filetype[6] "LM8953"
 *  nulls[10],
 *  dummy_aeg[6]
 *  format 0x49 = looped, 0xC9 = non-looped
 *  sample_rate 1 = 33 kHz, 2 = 50 kHz, 3 = 16 kHz
 *  atc_length[3] if sample rate 0, [2]&0xfe = 6: 33kHz, 0x10:50, 0xf6: 16,
 *					depending on [5] but to heck with it
 *  rpt_length[3] (these are for looped samples, attack and loop lengths)
 *  unused[2]
 */

typedef struct
{	unsigned char	format, srate, sr2, sr3 ;
	unsigned short	srhash ;
	unsigned int	attacklen, repeatlen ;
} TXW_HEADER ;

#define	ERROR_666	666

int
txw_open	(SF_PRIVATE *psf)
{	int error ;

	if (psf->mode != SFM_READ)
		return SFE_UNIMPLEMENTED ;

	if ((error = txw_read_header (psf)))
			return error ;

 	if (psf_fseek (psf, psf->dataoffset, SEEK_SET) != psf->dataoffset)
		return SFE_BAD_SEEK ;

	psf->read_short		= txw_read_s ;
	psf->read_int		= txw_read_i ;
	psf->read_float		= txw_read_f ;
	psf->read_double	= txw_read_d ;

	psf->seek = txw_seek ;

	return 0 ;
} /* txw_open */

/*------------------------------------------------------------------------------
*/

static int
txw_read_header	(SF_PRIVATE *psf)
{	TXW_HEADER txwh ;
	char	*strptr ;

	memset (&txwh, 0, sizeof (txwh)) ;
	memset (psf->buffer, 0, sizeof (psf->buffer)) ;
	psf_binheader_readf (psf, "pb", 0, psf->buffer, 16) ;

	if (memcmp (psf->buffer, "LM8953\0\0\0\0\0\0\0\0\0\0", 16) != 0)
		return ERROR_666 ;

	psf_log_printf (psf, "Read only : Yamaha TX-16 Sampler (.txw)\nLM8953\n") ;

	/* Jump 6 bytes (dummp_aeg), read format, read sample rate. */
	psf_binheader_readf (psf, "j11", 6, &txwh.format, &txwh.srate) ;

	/* 8 bytes (atc_length[3], rpt_length[3], unused[2]). */
	psf_binheader_readf (psf, "e33j", &txwh.attacklen, &txwh.repeatlen, 2) ;
	txwh.sr2 = (txwh.attacklen >> 16) & 0xFE ;
	txwh.sr3 = (txwh.repeatlen >> 16) & 0xFE ;
	txwh.attacklen &= 0x1FFFF ;
	txwh.repeatlen &= 0x1FFFF ;

	switch (txwh.format)
	{	case TXW_LOOPED :
				strptr = "looped" ;
				break ;

		case TXW_NO_LOOP :
				strptr = "non-looped" ;
				break ;

		default :
				psf_log_printf (psf, " Format      : 0x%02x => ?????\n", txwh.format) ;
				return ERROR_666 ;
		} ;

	psf_log_printf (psf, " Format      : 0x%02X => %s\n", txwh.format, strptr) ;

	strptr = NULL ;

	switch (txwh.srate)
	{	case 1 :
				psf->sf.samplerate = 33333 ;
				break ;

		case 2 :
				psf->sf.samplerate = 50000 ;
				break ;

		case 3 :
				psf->sf.samplerate = 16667 ;
				break ;

		default :
			/* This is ugly and braindead. */
			txwh.srhash = ((txwh.sr2 & 0xFE) << 8) | (txwh.sr3 & 0xFE) ;
			switch (txwh.srhash)
			{	case ((0x6 << 8) | 0x52) :
						psf->sf.samplerate = 33333 ;
						break ;

				case ((0x10 << 8) | 0x52) :
						psf->sf.samplerate = 50000 ;
						break ;

				case ((0xF6 << 8) | 0x52) :
						psf->sf.samplerate = 166667 ;
						break ;

				default :
						strptr = " Sample Rate : Unknown : forcing to 33333\n" ;
						psf->sf.samplerate = 33333 ;
						break ;
				} ;
		} ;


	if (strptr)
		psf_log_printf (psf, strptr) ;
	else if (txwh.srhash)
		psf_log_printf (psf, " Sample Rate : %d (0x%X) => %d\n", txwh.srate, txwh.srhash, psf->sf.samplerate) ;
	else
		psf_log_printf (psf, " Sample Rate : %d => %d\n", txwh.srate, psf->sf.samplerate) ;

	if (txwh.format == TXW_LOOPED)
	{	psf_log_printf (psf, " Attack Len  : %d\n", txwh.attacklen) ;
		psf_log_printf (psf, " Repeat Len  : %d\n", txwh.repeatlen) ;
		} ;

	psf->dataoffset = TXW_DATA_OFFSET ;
	psf->datalength = psf->filelength - TXW_DATA_OFFSET ;
	psf->sf.frames	= 2 * psf->datalength / 3 ;


	if (psf->datalength % 3 == 1)
		psf_log_printf (psf, "*** File seems to be truncated, %d extra bytes.\n",
			(int) (psf->datalength % 3)) ;

	if (txwh.attacklen + txwh.repeatlen > psf->sf.frames)
		psf_log_printf (psf, "*** File has been truncated.\n") ;

	psf->sf.format = SF_FORMAT_TXW | SF_FORMAT_PCM_16 ;
	psf->sf.channels = 1 ;
	psf->sf.sections = 1 ;
	psf->sf.seekable = SF_TRUE ;

	return 0 ;
} /* txw_read_header */

static sf_count_t
txw_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	unsigned char	*ucptr ;
	short			sample ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;

	bufferlen = sizeof (psf->buffer) / 3 ;
	bufferlen -= (bufferlen & 1) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = psf_fread (psf->buffer, 3, readcount, psf) ;

		ucptr = (unsigned char *) psf->buffer ;
		for (k = 0 ; k < readcount ; k += 2)
		{	sample = (ucptr [0] << 8) | (ucptr [1] & 0xF0) ;
			ptr [total + k] = sample ;
			sample = (ucptr [2] << 8) | ((ucptr [1] & 0xF) << 4) ;
			ptr [total + k + 1] = sample ;
			ucptr += 3 ;
			} ;

		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* txw_read_s */

static sf_count_t
txw_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	unsigned char	*ucptr ;
	short			sample ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;

	bufferlen = sizeof (psf->buffer) / 3 ;
	bufferlen -= (bufferlen & 1) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = psf_fread (psf->buffer, 3, readcount, psf) ;

		ucptr = (unsigned char *) psf->buffer ;
		for (k = 0 ; k < readcount ; k += 2)
		{	sample = (ucptr [0] << 8) | (ucptr [1] & 0xF0) ;
			ptr [total + k] = sample << 16 ;
			sample = (ucptr [2] << 8) | ((ucptr [1] & 0xF) << 4) ;
			ptr [total + k + 1] = sample << 16 ;
			ucptr += 3 ;
			} ;

		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* txw_read_i */

static sf_count_t
txw_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	unsigned char	*ucptr ;
	short			sample ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;
	float			normfact ;

	if (psf->norm_float == SF_TRUE)
		normfact = 1.0 / 0x8000 ;
	else
		normfact = 1.0 / 0x10 ;

	bufferlen = sizeof (psf->buffer) / 3 ;
	bufferlen -= (bufferlen & 1) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = psf_fread (psf->buffer, 3, readcount, psf) ;

		ucptr = (unsigned char *) psf->buffer ;
		for (k = 0 ; k < readcount ; k += 2)
		{	sample = (ucptr [0] << 8) | (ucptr [1] & 0xF0) ;
			ptr [total + k] = normfact * sample ;
			sample = (ucptr [2] << 8) | ((ucptr [1] & 0xF) << 4) ;
			ptr [total + k + 1] = normfact * sample ;
			ucptr += 3 ;
			} ;

		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* txw_read_f */

static sf_count_t
txw_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	unsigned char	*ucptr ;
	short			sample ;
	int				k, bufferlen, readcount, count ;
	sf_count_t		total = 0 ;
	double			normfact ;

	if (psf->norm_double == SF_TRUE)
		normfact = 1.0 / 0x8000 ;
	else
		normfact = 1.0 / 0x10 ;

	bufferlen = sizeof (psf->buffer) / 3 ;
	bufferlen -= (bufferlen & 1) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : len ;
		count = psf_fread (psf->buffer, 3, readcount, psf) ;

		ucptr = (unsigned char *) psf->buffer ;
		for (k = 0 ; k < readcount ; k += 2)
		{	sample = (ucptr [0] << 8) | (ucptr [1] & 0xF0) ;
			ptr [total + k] = normfact * sample ;
			sample = (ucptr [2] << 8) | ((ucptr [1] & 0xF) << 4) ;
			ptr [total + k + 1] = normfact * sample ;
			ucptr += 3 ;
			} ;

		total += count ;
		len -= readcount ;
		} ;

	return total ;
} /* txw_read_d */

static sf_count_t
txw_seek (SF_PRIVATE *psf, int mode, sf_count_t offset)
{	if (psf && mode)
		return offset ;

	return 0 ;
} /* txw_seek */

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 4d0ba7af-b1c5-46b4-a900-7c6f59fd9a89
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


static sf_count_t		ulaw_read_ulaw2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t		ulaw_read_ulaw2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t		ulaw_read_ulaw2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t		ulaw_read_ulaw2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t		ulaw_write_s2ulaw (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t		ulaw_write_i2ulaw (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t		ulaw_write_f2ulaw (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t		ulaw_write_d2ulaw (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static void	ulaw2s_array	(unsigned char *buffer, unsigned int count, short *ptr) ;
static void	ulaw2i_array	(unsigned char *buffer, unsigned int count, int *ptr) ;
static void	ulaw2f_array	(unsigned char *buffer, unsigned int count, float *ptr, float normfact) ;
static void	ulaw2d_array	(unsigned char *buffer, unsigned int count, double *ptr, double normfact) ;

static void	s2ulaw_array	(short *buffer, unsigned int count, unsigned char *ptr) ;
static void	i2ulaw_array	(int *buffer, unsigned int count, unsigned char *ptr) ;
static void	f2ulaw_array	(float *buffer, unsigned int count, unsigned char *ptr, float normfact) ;
static void	d2ulaw_array	(double *buffer, unsigned int count, unsigned char *ptr, double normfact) ;


int
ulaw_init (SF_PRIVATE *psf)
{
	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	psf->read_short		= ulaw_read_ulaw2s ;
		psf->read_int		= ulaw_read_ulaw2i ;
		psf->read_float		= ulaw_read_ulaw2f ;
		psf->read_double	= ulaw_read_ulaw2d ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	psf->write_short	= ulaw_write_s2ulaw ;
		psf->write_int		= ulaw_write_i2ulaw ;
		psf->write_float	= ulaw_write_f2ulaw ;
		psf->write_double	= ulaw_write_d2ulaw ;
		} ;

	psf->bytewidth = 1 ;
	psf->blockwidth = psf->sf.channels ;

	if (psf->filelength > psf->dataoffset)
		psf->datalength = (psf->dataend) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
	else
		psf->datalength = 0 ;

	psf->sf.frames = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* ulaw_init */

static sf_count_t
ulaw_read_ulaw2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		ulaw2s_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* ulaw_read_ulaw2s */

static sf_count_t
ulaw_read_ulaw2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		ulaw2i_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* ulaw_read_ulaw2i */

static sf_count_t
ulaw_read_ulaw2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		ulaw2f_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* ulaw_read_ulaw2f */

static sf_count_t
ulaw_read_ulaw2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double	normfact ;

	normfact = (psf->norm_double) ? 1.0 / ((double) 0x8000) : 1.0 ;
	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, 1, readcount, psf) ;
		ulaw2d_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		if (thisread < readcount)
			break ;
		len -= thisread ;
		} ;

	return total ;
} /* ulaw_read_ulaw2d */

/*=============================================================================================
*/

static sf_count_t
ulaw_write_s2ulaw	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2ulaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer)) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* ulaw_write_s2ulaw */

static sf_count_t
ulaw_write_i2ulaw	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2ulaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer)) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* ulaw_write_i2ulaw */

static sf_count_t
ulaw_write_f2ulaw	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2ulaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer), normfact) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* ulaw_write_f2ulaw */

static sf_count_t
ulaw_write_d2ulaw	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double	normfact ;

	normfact = (psf->norm_double) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2ulaw_array (ptr + total, writecount, (unsigned char*) (psf->buffer), normfact) ;
		thiswrite = psf_fwrite (psf->buffer, 1, writecount, psf) ;
		total += thiswrite ;
		if (thiswrite < writecount)
			break ;
		len -= thiswrite ;
		} ;

	return total ;
} /* ulaw_write_d2ulaw */

/*=============================================================================================
 *	Private static functions and data.
 */

static	short	ulaw_decode [128] =
{	-32124,	-31100,	-30076,	-29052,	-28028,	-27004,	-25980,	-24956,
	-23932,	-22908,	-21884,	-20860,	-19836,	-18812,	-17788,	-16764,
	-15996,	-15484,	-14972,	-14460,	-13948,	-13436,	-12924,	-12412,
	-11900,	-11388,	-10876,	-10364,	-9852,	-9340,	-8828,	-8316,
	-7932,	-7676,	-7420,	-7164,	-6908,	-6652,	-6396,	-6140,
	-5884,	-5628,	-5372,	-5116,	-4860,	-4604,	-4348,	-4092,
	-3900,	-3772,	-3644,	-3516,	-3388,	-3260,	-3132,	-3004,
	-2876,	-2748,	-2620,	-2492,	-2364,	-2236,	-2108,	-1980,
	-1884,	-1820,	-1756,	-1692,	-1628,	-1564,	-1500,	-1436,
	-1372,	-1308,	-1244,	-1180,	-1116,	-1052,	-988,	-924,
	-876,	-844,	-812,	-780,	-748,	-716,	-684,	-652,
	-620,	-588,	-556,	-524,	-492,	-460,	-428,	-396,
	-372,	-356,	-340,	-324,	-308,	-292,	-276,	-260,
	-244,	-228,	-212,	-196,	-180,	-164,	-148,	-132,
	-120,	-112,	-104,	-96,	-88,	-80,	-72,	-64,
	-56,	-48,	-40,	-32,	-24,	-16,	-8,		0,
} ;

static
unsigned char	ulaw_encode [8193] =
{	0xFF, 0xFE, 0xFE, 0xFD, 0xFD, 0xFC, 0xFC, 0xFB, 0xFB, 0xFA, 0xFA, 0xF9,
	0xF9, 0xF8, 0xF8, 0xF7, 0xF7, 0xF6, 0xF6, 0xF5, 0xF5, 0xF4, 0xF4, 0xF3,
	0xF3, 0xF2, 0xF2, 0xF1, 0xF1, 0xF0, 0xF0, 0xEF, 0xEF, 0xEF, 0xEF, 0xEE,
	0xEE, 0xEE, 0xEE, 0xED, 0xED, 0xED, 0xED, 0xEC, 0xEC, 0xEC, 0xEC, 0xEB,
	0xEB, 0xEB, 0xEB, 0xEA, 0xEA, 0xEA, 0xEA, 0xE9, 0xE9, 0xE9, 0xE9, 0xE8,
	0xE8, 0xE8, 0xE8, 0xE7, 0xE7, 0xE7, 0xE7, 0xE6, 0xE6, 0xE6, 0xE6, 0xE5,
	0xE5, 0xE5, 0xE5, 0xE4, 0xE4, 0xE4, 0xE4, 0xE3, 0xE3, 0xE3, 0xE3, 0xE2,
	0xE2, 0xE2, 0xE2, 0xE1, 0xE1, 0xE1, 0xE1, 0xE0, 0xE0, 0xE0, 0xE0, 0xDF,
	0xDF, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF, 0xDE, 0xDE, 0xDE, 0xDE, 0xDE,
	0xDE, 0xDE, 0xDE, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDC,
	0xDC, 0xDC, 0xDC, 0xDC, 0xDC, 0xDC, 0xDC, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB,
	0xDB, 0xDB, 0xDB, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xDA, 0xD9,
	0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD9, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8,
	0xD8, 0xD8, 0xD8, 0xD7, 0xD7, 0xD7, 0xD7, 0xD7, 0xD7, 0xD7, 0xD7, 0xD6,
	0xD6, 0xD6, 0xD6, 0xD6, 0xD6, 0xD6, 0xD6, 0xD5, 0xD5, 0xD5, 0xD5, 0xD5,
	0xD5, 0xD5, 0xD5, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD3,
	0xD3, 0xD3, 0xD3, 0xD3, 0xD3, 0xD3, 0xD3, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2,
	0xD2, 0xD2, 0xD2, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD1, 0xD0,
	0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF,
	0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCE,
	0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE, 0xCE,
	0xCE, 0xCE, 0xCE, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD,
	0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCB,
	0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB,
	0xCB, 0xCB, 0xCB, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA,
	0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xCA, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9,
	0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9, 0xC8,
	0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8,
	0xC8, 0xC8, 0xC8, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7,
	0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC7, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6,
	0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC5,
	0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5,
	0xC5, 0xC5, 0xC5, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4,
	0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC4, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
	0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC2,
	0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2,
	0xC2, 0xC2, 0xC2, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1,
	0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xBF,
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
	0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
	0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
	0xBE, 0xBE, 0xBE, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD,
	0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD,
	0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBC,
	0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC,
	0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC,
	0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
	0xBB, 0xBB, 0xBB, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA,
	0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA,
	0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xB8, 0xB8, 0xB8, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7,
	0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7,
	0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB6,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
	0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
	0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
	0xB5, 0xB5, 0xB5, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB3,
	0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
	0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
	0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
	0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
	0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
	0xB2, 0xB2, 0xB2, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB0,
	0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0,
	0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0,
	0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
	0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
	0xAE, 0xAE, 0xAE, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
	0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
	0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
	0xAB, 0xAB, 0xAB, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
	0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
	0xA8, 0xA8, 0xA8, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
	0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
	0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
	0xA5, 0xA5, 0xA5, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
	0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
	0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
	0xA2, 0xA2, 0xA2, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
	0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
	0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E,
	0x9E, 0x9E, 0x9E, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D,
	0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C,
	0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B,
	0x9B, 0x9B, 0x9B, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A,
	0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x9A, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x98, 0x98, 0x98, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
	0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
	0x8E, 0x8E, 0x8E, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
	0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
	0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
	0x8B, 0x8B, 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00
} ;

static void
ulaw2s_array	(unsigned char *buffer, unsigned int count, short *ptr)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -1 * ulaw_decode [((int) buffer [count]) & 0x7F] ;
		else
			ptr [count] = ulaw_decode [((int) buffer [count]) & 0x7F] ;
		} ;
} /* ulaw2s_array */

static void
ulaw2i_array	(unsigned char *buffer, unsigned int count, int *ptr)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -1 * ulaw_decode [buffer [count] & 0x7F] << 16 ;
		else
			ptr [count] = ulaw_decode [buffer [count] & 0x7F] << 16 ;
		} ;
} /* ulaw2i_array */

static void
ulaw2f_array	(unsigned char *buffer, unsigned int count, float *ptr, float normfact)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -normfact * ulaw_decode [((int) buffer [count]) & 0x7F] ;
		else
			ptr [count] = normfact * ulaw_decode [((int) buffer [count]) & 0x7F] ;
		} ;
} /* ulaw2f_array */

static void
ulaw2d_array	(unsigned char *buffer, unsigned int count, double *ptr, double normfact)
{	while (count)
	{	count -- ;
		if (buffer [count] & 0x80)
			ptr [count] = -normfact * ulaw_decode [((int) buffer [count]) & 0x7F] ;
		else
			ptr [count] = normfact * ulaw_decode [((int) buffer [count]) & 0x7F] ;
		} ;
} /* ulaw2d_array */

static void
s2ulaw_array	(short *ptr, unsigned int count, unsigned char *buffer)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = ulaw_encode [ptr [count] / 4] ;
		else
			buffer [count] = 0x7F & ulaw_encode [ptr [count] / -4] ;
		} ;
} /* s2ulaw_array */

static void
i2ulaw_array	(int *ptr, unsigned int count, unsigned char *buffer)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = ulaw_encode [ptr [count] >> (16 + 2)] ;
		else
			buffer [count] = 0x7F & ulaw_encode [-ptr [count] >> (16 + 2)] ;
		} ;
} /* i2ulaw_array */

static void
f2ulaw_array	(float *ptr, unsigned int count, unsigned char *buffer, float normfact)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = ulaw_encode [(lrintf (normfact * ptr [count])) / 4] ;
		else
			buffer [count] = 0x7F & ulaw_encode [(lrintf (normfact * ptr [count])) / -4] ;
		} ;
} /* f2ulaw_array */

static void
d2ulaw_array	(double *ptr, unsigned int count, unsigned char *buffer, double normfact)
{	while (count)
	{	count -- ;
		if (ptr [count] >= 0)
			buffer [count] = ulaw_encode [(lrint (normfact * ptr [count])) / 4] ;
		else
			buffer [count] = 0x7F & ulaw_encode [(lrint (normfact * ptr [count])) / -4] ;
		} ;
} /* d2ulaw_array */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 655cc790-f058-45e8-89c9-86967cccc37e
*/
/*
** Copyright (C) 2001-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*	RANT:
**	The VOC file format is the most brain damaged format I have yet had to deal
**	with. No one programmer could have bee stupid enough to put this together.
**	Instead it looks like a series of manic, dyslexic assembly language programmers
**	hacked it to fit their needs.
**	Utterly woeful.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*------------------------------------------------------------------------------
 * Typedefs for file chunks.
*/

#define	VOC_MAX_SECTIONS	200

enum
{	VOC_TERMINATOR		= 0,
	VOC_SOUND_DATA		= 1,
	VOC_SOUND_CONTINUE	= 2,
	VOC_SILENCE			= 3,
	VOC_MARKER			= 4,
	VOC_ASCII			= 5,
	VOC_REPEAT			= 6,
	VOC_END_REPEAT		= 7,
	VOC_EXTENDED		= 8,
	VOC_EXTENDED_II		= 9
} ;

typedef struct
{	int 	samples ;
	int		offset ;	/* Offset of zero => silence. */
} SND_DATA_BLOCKS ;

typedef struct
{	unsigned int 	sections, section_types ;
	int				samplerate, channels, bitwidth ;
	SND_DATA_BLOCKS	blocks [VOC_MAX_SECTIONS] ;
} VOC_DATA ;

/*------------------------------------------------------------------------------
 * Private static functions.
*/

static	int	voc_close	(SF_PRIVATE *psf) ;
static	int voc_write_header (SF_PRIVATE *psf, int calc_length) ;
static	int voc_read_header	(SF_PRIVATE *psf) ;

static const char* voc_encoding2str (int encoding) ;

#if 0

/*	These functions would be required for files with more than one VOC_SOUND_DATA
**	segment. Not sure whether to bother implementing this.
*/

static int	voc_multi_init (SF_PRIVATE *psf, VOC_DATA *pvoc) ;

static int	voc_multi_read_uc2s		(SF_PRIVATE *psf, short *ptr, int len) ;
static int	voc_multi_read_les2s	(SF_PRIVATE *psf, short *ptr, int len) ;

static int	voc_multi_read_uc2i		(SF_PRIVATE *psf, int *ptr, int len) ;
static int	voc_multi_read_les2i	(SF_PRIVATE *psf, int *ptr, int len) ;

static int	voc_multi_read_uc2f		(SF_PRIVATE *psf, float *ptr, int len) ;
static int	voc_multi_read_les2f	(SF_PRIVATE *psf, float *ptr, int len) ;

static int	voc_multi_read_uc2d		(SF_PRIVATE *psf, double *ptr, int len) ;
static int	voc_multi_read_les2d	(SF_PRIVATE *psf, double *ptr, int len) ;
#endif

/*------------------------------------------------------------------------------
** Public function.
*/

int
voc_open	(SF_PRIVATE *psf)
{	int subformat, error = 0 ;

	if (psf->is_pipe)
		return SFE_VOC_NO_PIPE ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = voc_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_VOC)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = SF_ENDIAN_LITTLE ;

		if ((error = voc_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = voc_write_header ;
		} ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	psf->close = voc_close ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :
				error = alaw_init (psf) ;
				break ;

		case SF_FORMAT_ULAW :
				error = ulaw_init (psf) ;
				break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	return error ;
} /* voc_open */

/*------------------------------------------------------------------------------
*/

static int
voc_read_header	(SF_PRIVATE *psf)
{	VOC_DATA	*pvoc ;
	char	creative [20] ;
	unsigned char block_type, rate_byte ;
	short	version, checksum, encoding, dataoffset ;
	int		offset ;

	/* Set position to start of file to begin reading header. */
	offset = psf_binheader_readf (psf, "pb", 0, creative, SIGNED_SIZEOF (creative)) ;

	if (creative [sizeof (creative) - 1] != 0x1A)
		return SFE_VOC_NO_CREATIVE ;

	/* Terminate the string. */
	creative [sizeof (creative) - 1] = 0 ;

	if (strcmp ("Creative Voice File", creative))
		return SFE_VOC_NO_CREATIVE ;

	psf_log_printf (psf, "%s\n", creative) ;

	offset += psf_binheader_readf (psf, "e222", &dataoffset, &version, &checksum) ;

	psf->dataoffset = dataoffset ;

	psf_log_printf (psf, 	"dataoffset : %d\n"
							"version    : 0x%X\n"
							"checksum   : 0x%X\n", psf->dataoffset, version, checksum) ;

	if (version != 0x010A && version != 0x0114)
		return SFE_VOC_BAD_VERSION ;

	if (! (psf->fdata = malloc (sizeof (VOC_DATA))))
		return SFE_MALLOC_FAILED ;

	pvoc = (VOC_DATA*) psf->fdata ;

	memset (pvoc, 0, sizeof (VOC_DATA)) ;

	/* Set the default encoding now. */
	psf->sf.format = SF_FORMAT_VOC ; /* Major format */
	encoding = SF_FORMAT_PCM_U8 ; /* Minor format */
	psf->endian = SF_ENDIAN_LITTLE ;

	while (1)
	{	offset += psf_binheader_readf (psf, "1", &block_type) ;

		switch (block_type)
		{	case VOC_ASCII :
					{	int size ;

						offset += psf_binheader_readf (psf, "e3", &size) ;

						psf_log_printf (psf, " ASCII : %d\n", size) ;

						offset += psf_binheader_readf (psf, "b", psf->header, size) ;
						psf->header [size] = 0 ;
						psf_log_printf (psf, "  text : %s\n", psf->header) ;
						} ;
					continue ;

			case VOC_SOUND_DATA :
			case VOC_EXTENDED :
			case VOC_EXTENDED_II :
					break ;

			default : psf_log_printf (psf, "*** Weird block marker (%d)\n", block_type) ;
			} ;

		break ;
		} ;

	if (block_type == VOC_SOUND_DATA)
	{	unsigned char compression ;
		int 	size ;

		offset += psf_binheader_readf (psf, "e311", &size, &rate_byte, &compression) ;

		psf->sf.samplerate = 1000000 / (256 - (rate_byte & 0xFF)) ;

		psf_log_printf (psf, " Sound Data : %d\n  sr   : %d => %dHz\n  comp : %d\n",
								size, rate_byte, psf->sf.samplerate, compression) ;

		if (offset + size - 1 > psf->filelength)
		{	psf_log_printf (psf, "Seems to be a truncated file.\n") ;
			psf_log_printf (psf, "offset: %d    size: %d    sum: %d    filelength: %D\n", offset, size, offset + size, psf->filelength) ;
			return SFE_VOC_BAD_SECTIONS ;
			}
		else if (offset + size - 1 < psf->filelength)
		{	psf_log_printf (psf, "Seems to be a multi-segment file (#1).\n") ;
			psf_log_printf (psf, "offset: %d    size: %d    sum: %d    filelength: %D\n", offset, size, offset + size, psf->filelength) ;
			return SFE_VOC_BAD_SECTIONS ;
			} ;

		psf->dataoffset = offset ;
		psf->dataend	= psf->filelength - 1 ;

		psf->sf.channels = 1 ;
		psf->bytewidth = 1 ;

		psf->sf.format = SF_FORMAT_VOC | SF_FORMAT_PCM_U8 ;

		return 0 ;
		} ;

	if (block_type == VOC_EXTENDED)
	{	unsigned char pack, stereo, compression ;
		unsigned short rate_short ;
		int		size ;

		offset += psf_binheader_readf (psf, "e3211", &size, &rate_short, &pack, &stereo) ;

		psf_log_printf (psf, " Extended : %d\n", size) ;
		if (size == 4)
			psf_log_printf (psf, "  size   : 4\n") ;
		else
			psf_log_printf (psf, "  size   : %d (should be 4)\n", size) ;

		psf_log_printf (psf,	"  pack   : %d\n"
								"  stereo : %s\n", pack, (stereo ? "yes" : "no")) ;

		if (stereo)
		{	psf->sf.channels = 2 ;
			psf->sf.samplerate = 128000000 / (65536 - rate_short) ;
			}
		else
		{	psf->sf.channels = 1 ;
			psf->sf.samplerate = 256000000 / (65536 - rate_short) ;
			} ;

		psf_log_printf (psf, "  sr     : %d => %dHz\n", (rate_short & 0xFFFF), psf->sf.samplerate) ;

		offset += psf_binheader_readf (psf, "1", &block_type) ;

		if (block_type != VOC_SOUND_DATA)
		{	psf_log_printf (psf, "*** Expecting VOC_SOUND_DATA section.\n") ;
			return SFE_VOC_BAD_FORMAT ;
			} ;

		offset += psf_binheader_readf (psf, "e311", &size, &rate_byte, &compression) ;

		psf_log_printf (psf,	" Sound Data : %d\n"
								"  sr     : %d\n"
								"  comp   : %d\n", size, rate_byte, compression) ;


		if (offset + size - 1 > psf->filelength)
		{	psf_log_printf (psf, "Seems to be a truncated file.\n") ;
			psf_log_printf (psf, "offset: %d    size: %d    sum: %d    filelength: %D\n", offset, size, offset + size, psf->filelength) ;
			return SFE_VOC_BAD_SECTIONS ;
			}
		else if (offset + size - 1 < psf->filelength)
		{	psf_log_printf (psf, "Seems to be a multi-segment file (#2).\n") ;
			psf_log_printf (psf, "offset: %d    size: %d    sum: %d    filelength: %D\n", offset, size, offset + size, psf->filelength) ;
			return SFE_VOC_BAD_SECTIONS ;
			} ;

		psf->dataoffset = offset ;
		psf->dataend = psf->filelength - 1 ;

		psf->bytewidth = 1 ;

		psf->sf.format = SF_FORMAT_VOC | SF_FORMAT_PCM_U8 ;

		return 0 ;
		}

	if (block_type == VOC_EXTENDED_II)
	{	unsigned char bitwidth, channels ;
		int size, fourbytes ;

		offset += psf_binheader_readf (psf, "e341124", &size, &psf->sf.samplerate,
								&bitwidth, &channels, &encoding, &fourbytes) ;

		if (size * 2 == psf->filelength - 39)
		{	int temp_size = psf->filelength - 31 ;

			psf_log_printf (psf, " Extended II : %d (SoX bug: should be %d)\n", size, temp_size) ;
			size = temp_size ;
			}
		else
			psf_log_printf (psf, " Extended II : %d\n", size) ;

		psf_log_printf (psf,	"  sample rate : %d\n"
								"  bit width   : %d\n"
								"  channels    : %d\n", psf->sf.samplerate, bitwidth, channels) ;

		if (bitwidth == 16 && encoding == 0)
		{	encoding = 4 ;
			psf_log_printf (psf, "  encoding    : 0 (SoX bug: should be 4 for 16 bit signed PCM)\n") ;
			}
		else
			psf_log_printf (psf, "  encoding    : %d => %s\n", encoding, voc_encoding2str (encoding)) ;


		psf_log_printf (psf, "  fourbytes   : %X\n", fourbytes) ;

		psf->sf.channels = channels ;

		psf->dataoffset = offset ;
		psf->dataend	= psf->filelength - 1 ;

		if (size + 31 == psf->filelength + 1)
		{	/* Hack for reading files produced using
			** sf_command (SFC_UPDATE_HEADER_NOW).
			*/
			psf_log_printf (psf, "Missing zero byte at end of file.\n") ;
			size = psf->filelength - 30 ;
			psf->dataend = 0 ;
			}
		else if (size + 31 > psf->filelength)
		{	psf_log_printf (psf, "Seems to be a truncated file.\n") ;
			size = psf->filelength - 31 ;
			}
		else if (size + 31 < psf->filelength)
			psf_log_printf (psf, "Seems to be a multi-segment file (#3).\n") ;

		switch (encoding)
		{	case 0 :
					psf->sf.format = SF_FORMAT_VOC | SF_FORMAT_PCM_U8 ;
					psf->bytewidth = 1 ;
					break ;

			case 4 :
					psf->sf.format = SF_FORMAT_VOC | SF_FORMAT_PCM_16 ;
					psf->bytewidth = 2 ;
					break ;

			case 6 :
					psf->sf.format = SF_FORMAT_VOC | SF_FORMAT_ALAW ;
					psf->bytewidth = 1 ;
					break ;

			case 7 :
					psf->sf.format = SF_FORMAT_VOC | SF_FORMAT_ULAW ;
					psf->bytewidth = 1 ;
					break ;

			default : /* Unknown */
					return SFE_UNKNOWN_FORMAT ;
					break ;
			} ;

		} ;

	return 0 ;
} /* voc_read_header */

/*====================================================================================
*/

static int
voc_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int			rate_const, subformat ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;
	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	/* VOC marker and 0x1A byte. */
	psf_binheader_writef (psf, "eb1", "Creative Voice File", 19, 0x1A) ;

	/* Data offset, version and other. */
	psf_binheader_writef (psf, "e222", 26, 0x0114, 0x111F) ;

	/*	Use same logic as SOX.
	**	If the file is mono 8 bit data, use VOC_SOUND_DATA.
	**	If the file is mono 16 bit data, use VOC_EXTENED.
	**	Otherwise use VOC_EXTENED_2.
	*/

	if (subformat == SF_FORMAT_PCM_U8 && psf->sf.channels == 1)
	{	/* samplerate = 1000000 / (256 - rate_const) ; */
		rate_const = 256 - 1000000 / psf->sf.samplerate ;

		/* First type marker, length, rate_const and compression */
		psf_binheader_writef (psf, "e1311", VOC_SOUND_DATA, (int) (psf->datalength + 1), rate_const, 0) ;
		}
	else if (subformat == SF_FORMAT_PCM_U8 && psf->sf.channels == 2)
	{	/* sample_rate = 128000000 / (65536 - rate_short) ; */
		rate_const = 65536 - 128000000 / psf->sf.samplerate ;

		/* First write the VOC_EXTENDED section
		** 		marker, length, rate_const and compression
		*/
		psf_binheader_writef (psf, "e13211", VOC_EXTENDED, 4, rate_const, 0, 1) ;

		/* samplerate = 1000000 / (256 - rate_const) ; */
		rate_const = 256 - 1000000 / psf->sf.samplerate ;

		/*	Now write the VOC_SOUND_DATA section
		** 		marker, length, rate_const and compression
		*/
		psf_binheader_writef (psf, "e1311", VOC_SOUND_DATA, (int) (psf->datalength + 1), rate_const, 0) ;
		}
	else
	{	int length ;

		if (psf->sf.channels < 1 || psf->sf.channels > 2)
			return SFE_CHANNEL_COUNT ;

		switch (subformat)
		{	case SF_FORMAT_PCM_U8 :
					psf->bytewidth = 1 ;
					length = psf->sf.frames * psf->sf.channels * psf->bytewidth + 12 ;
					/* Marker, length, sample rate, bitwidth, stereo flag, encoding and fourt zero bytes. */
					psf_binheader_writef (psf, "e1341124", VOC_EXTENDED_II, length, psf->sf.samplerate, 16, psf->sf.channels, 4, 0) ;
					break ;

			case SF_FORMAT_PCM_16 :
					psf->bytewidth = 2 ;
					length = psf->sf.frames * psf->sf.channels * psf->bytewidth + 12 ;
					/* Marker, length, sample rate, bitwidth, stereo flag, encoding and fourt zero bytes. */
					psf_binheader_writef (psf, "e1341124", VOC_EXTENDED_II, length, psf->sf.samplerate, 16, psf->sf.channels, 4, 0) ;
					break ;

			case SF_FORMAT_ALAW :
					psf->bytewidth = 1 ;
					length = psf->sf.frames * psf->sf.channels * psf->bytewidth + 12 ;
					psf_binheader_writef (psf, "e1341124", VOC_EXTENDED_II, length, psf->sf.samplerate, 8, psf->sf.channels, 6, 0) ;
					break ;

			case SF_FORMAT_ULAW :
					psf->bytewidth = 1 ;
					length = psf->sf.frames * psf->sf.channels * psf->bytewidth + 12 ;
					psf_binheader_writef (psf, "e1341124", VOC_EXTENDED_II, length, psf->sf.samplerate, 8, psf->sf.channels, 7, 0) ;
					break ;

			default : return SFE_UNIMPLEMENTED ;
			} ;
		} ;

	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* voc_write_header */

static int
voc_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	/*  Now we know for certain the length of the file we can re-write
		**	correct values for the FORM, 8SVX and BODY chunks.
		*/
		unsigned byte = VOC_TERMINATOR ;


		psf_fseek (psf, 0, SEEK_END) ;

		/* Write terminator */
		psf_fwrite (&byte, 1, 1, psf) ;

		voc_write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* voc_close */

static const 	char*
voc_encoding2str (int encoding)
{
	switch (encoding)
	{	case 0 :	return "8 bit unsigned PCM" ;
		case 4 :	return "16 bit signed PCM" ;
		case 6 :	return "A-law" ;
		case 7 :	return "u-law" ;
		default :	break ;
		}
	return "*** Unknown ***" ;
} /* voc_encoding2str */

/*====================================================================================
*/

#if 0
static int
voc_multi_init (SF_PRIVATE *psf, VOC_DATA *pvoc)
{
	psf->sf.frames = 0 ;

	if (pvoc->bitwidth == 8)
	{	psf->read_short		= voc_multi_read_uc2s ;
		psf->read_int		= voc_multi_read_uc2i ;
		psf->read_float		= voc_multi_read_uc2f ;
		psf->read_double	= voc_multi_read_uc2d ;
		return 0 ;
		} ;

	if (pvoc->bitwidth == 16)
	{	psf->read_short		= voc_multi_read_les2s ;
		psf->read_int		= voc_multi_read_les2i ;
		psf->read_float		= voc_multi_read_les2f ;
		psf->read_double	= voc_multi_read_les2d ;
		return 0 ;
		} ;

	psf_log_printf (psf, "Error : bitwith != 8 && bitwidth != 16.\n") ;

	return SFE_UNIMPLEMENTED ;
} /* voc_multi_read_int */

/*------------------------------------------------------------------------------------
*/

static int
voc_multi_read_uc2s (SF_PRIVATE *psf, short *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_uc2s */

static int
voc_multi_read_les2s (SF_PRIVATE *psf, short *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_les2s */


static int
voc_multi_read_uc2i (SF_PRIVATE *psf, int *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_uc2i */

static int
voc_multi_read_les2i (SF_PRIVATE *psf, int *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_les2i */


static int
voc_multi_read_uc2f (SF_PRIVATE *psf, float *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_uc2f */

static int
voc_multi_read_les2f (SF_PRIVATE *psf, float *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_les2f */


static int
voc_multi_read_uc2d (SF_PRIVATE *psf, double *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_uc2d */

static int
voc_multi_read_les2d (SF_PRIVATE *psf, double *ptr, int len)
{

	return 0 ;
} /* voc_multi_read_les2d */

#endif

/*------------------------------------------------------------------------------------

Creative Voice (VOC) file format
--------------------------------

~From: galt@dsd.es.com

(byte numbers are hex!)

    HEADER (bytes 00-19)
    Series of DATA BLOCKS (bytes 1A+) [Must end w/ Terminator Block]

- ---------------------------------------------------------------

HEADER:
=======
     byte #     Description
     ------     ------------------------------------------
     00-12      "Creative Voice File"
     13         1A (eof to abort printing of file)
     14-15      Offset of first datablock in .voc file (std 1A 00
                in Intel Notation)
     16-17      Version number (minor,major) (VOC-HDR puts 0A 01)
     18-19      1's Comp of Ver. # + 1234h (VOC-HDR puts 29 11)

- ---------------------------------------------------------------

DATA BLOCK:
===========

   Data Block:  TYPE(1-byte), SIZE(3-bytes), INFO(0+ bytes)
   NOTE: Terminator Block is an exception -- it has only the TYPE byte.

      TYPE   Description     Size (3-byte int)   Info
      ----   -----------     -----------------   -----------------------
      00     Terminator      (NONE)              (NONE)
      01     Sound data      2+length of data    *
      02     Sound continue  length of data      Voice Data
      03     Silence         3                   **
      04     Marker          2                   Marker# (2 bytes)
      05     ASCII           length of string    null terminated string
      06     Repeat          2                   Count# (2 bytes)
      07     End repeat      0                   (NONE)
      08     Extended        4                   ***

      *Sound Info Format:
       ---------------------
       00   Sample Rate
       01   Compression Type
       02+  Voice Data

      **Silence Info Format:
      ----------------------------
      00-01  Length of silence - 1
      02     Sample Rate


    ***Extended Info Format:
       ---------------------
       00-01  Time Constant: Mono: 65536 - (256000000/sample_rate)
                             Stereo: 65536 - (25600000/(2*sample_rate))
       02     Pack
       03     Mode: 0 = mono
                    1 = stereo


  Marker#           -- Driver keeps the most recent marker in a status byte
  Count#            -- Number of repetitions + 1
                         Count# may be 1 to FFFE for 0 - FFFD repetitions
                         or FFFF for endless repetitions
  Sample Rate       -- SR byte = 256-(1000000/sample_rate)
  Length of silence -- in units of sampling cycle
  Compression Type  -- of voice data
                         8-bits    = 0
                         4-bits    = 1
                         2.6-bits  = 2
                         2-bits    = 3
                         Multi DAC = 3+(# of channels) [interesting--
                                       this isn't in the developer's manual]


---------------------------------------------------------------------------------
Addendum submitted by Votis Kokavessis:

After some experimenting with .VOC files I found out that there is a Data Block
Type 9, which is not covered in the VOC.TXT file. Here is what I was able to discover
about this block type:


TYPE: 09
SIZE: 12 + length of data
INFO: 12 (twelve) bytes

INFO STRUCTURE:

Bytes 0-1: (Word) Sample Rate (e.g. 44100)
Bytes 2-3: zero (could be that bytes 0-3 are a DWord for Sample Rate)
Byte 4: Sample Size in bits (e.g. 16)
Byte 5: Number of channels (e.g. 1 for mono, 2 for stereo)
Byte 6: Unknown (equal to 4 in all files I examined)
Bytes 7-11: zero


-------------------------------------------------------------------------------------*/

/*=====================================================================================
**=====================================================================================
**=====================================================================================
**=====================================================================================
*/

/*------------------------------------------------------------------------
The following is taken from the Audio File Formats FAQ dated 2-Jan-1995
and submitted by Guido van Rossum <guido@cwi.nl>.
--------------------------------------------------------------------------
Creative Voice (VOC) file format
--------------------------------

From: galt@dsd.es.com

(byte numbers are hex!)

    HEADER (bytes 00-19)
    Series of DATA BLOCKS (bytes 1A+) [Must end w/ Terminator Block]

- ---------------------------------------------------------------

HEADER:
-------
     byte #     Description
     ------     ------------------------------------------
     00-12      "Creative Voice File"
     13         1A (eof to abort printing of file)
     14-15      Offset of first datablock in .voc file (std 1A 00
                in Intel Notation)
     16-17      Version number (minor,major) (VOC-HDR puts 0A 01)
     18-19      2's Comp of Ver. # + 1234h (VOC-HDR puts 29 11)

- ---------------------------------------------------------------

DATA BLOCK:
-----------

   Data Block:  TYPE(1-byte), SIZE(3-bytes), INFO(0+ bytes)
   NOTE: Terminator Block is an exception -- it has only the TYPE byte.

      TYPE   Description     Size (3-byte int)   Info
      ----   -----------     -----------------   -----------------------
      00     Terminator      (NONE)              (NONE)
      01     Sound data      2+length of data    *
      02     Sound continue  length of data      Voice Data
      03     Silence         3                   **
      04     Marker          2                   Marker# (2 bytes)
      05     ASCII           length of string    null terminated string
      06     Repeat          2                   Count# (2 bytes)
      07     End repeat      0                   (NONE)
      08     Extended        4                   ***

      *Sound Info Format:       **Silence Info Format:
       ---------------------      ----------------------------
       00   Sample Rate           00-01  Length of silence - 1
       01   Compression Type      02     Sample Rate
       02+  Voice Data

    ***Extended Info Format:
       ---------------------
       00-01  Time Constant: Mono: 65536 - (256000000/sample_rate)
                             Stereo: 65536 - (25600000/(2*sample_rate))
       02     Pack
       03     Mode: 0 = mono
                    1 = stereo


  Marker#           -- Driver keeps the most recent marker in a status byte
  Count#            -- Number of repetitions + 1
                         Count# may be 1 to FFFE for 0 - FFFD repetitions
                         or FFFF for endless repetitions
  Sample Rate       -- SR byte = 256-(1000000/sample_rate)
  Length of silence -- in units of sampling cycle
  Compression Type  -- of voice data
                         8-bits    = 0
                         4-bits    = 1
                         2.6-bits  = 2
                         2-bits    = 3
                         Multi DAC = 3+(# of channels) [interesting--
                                       this isn't in the developer's manual]

Detailed description of new data blocks (VOC files version 1.20 and above):

        (Source is fax from Barry Boone at Creative Labs, 405/742-6622)

BLOCK 8 - digitized sound attribute extension, must preceed block 1.
          Used to define stereo, 8 bit audio
        BYTE bBlockID;       // = 8
        BYTE nBlockLen[3];   // 3 byte length
        WORD wTimeConstant;  // time constant = same as block 1
        BYTE bPackMethod;    // same as in block 1
        BYTE bVoiceMode;     // 0-mono, 1-stereo

        Data is stored left, right

BLOCK 9 - data block that supersedes blocks 1 and 8.
          Used for stereo, 16 bit.

        BYTE bBlockID;          // = 9
        BYTE nBlockLen[3];      // length 12 plus length of sound
        DWORD dwSamplesPerSec;  // samples per second, not time const.
        BYTE bBitsPerSample;    // e.g., 8 or 16
        BYTE bChannels;         // 1 for mono, 2 for stereo
        WORD wFormat;           // see below
        BYTE reserved[4];       // pad to make block w/o data
                                // have a size of 16 bytes

        Valid values of wFormat are:

                0x0000  8-bit unsigned PCM
                0x0001  Creative 8-bit to 4-bit ADPCM
                0x0002  Creative 8-bit to 3-bit ADPCM
                0x0003  Creative 8-bit to 2-bit ADPCM
                0x0004  16-bit signed PCM
                0x0006  CCITT a-Law
                0x0007  CCITT u-Law
                0x02000 Creative 16-bit to 4-bit ADPCM

        Data is stored left, right

------------------------------------------------------------------------*/
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 40a50167-a81c-463a-9e1d-3282ff84e09d
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
**	This is the OKI / Dialogic ADPCM encoder/decoder. It converts from
**	12 bit linear sample data to a 4 bit ADPCM.
**
**	Implemented from the description found here:
**
**		http://www.comptek.ru:8100/telephony/tnotes/tt1-13.html
**
**	and compared against the encoder/decoder found here:
**
**		http://ibiblio.org/pub/linux/apps/sound/convert/vox.tar.gz
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>


#define		VOX_DATA_LEN	2048
#define		PCM_DATA_LEN	(VOX_DATA_LEN *2)

typedef struct
{	short last ;
	short step_index ;

	int		vox_bytes, pcm_samples ;

	unsigned char	vox_data [VOX_DATA_LEN] ;
	short 			pcm_data [PCM_DATA_LEN] ;
} VOX_ADPCM_PRIVATE ;

static int vox_adpcm_encode_block (VOX_ADPCM_PRIVATE *pvox) ;
static int vox_adpcm_decode_block (VOX_ADPCM_PRIVATE *pvox) ;

static short vox_adpcm_decode (char code, VOX_ADPCM_PRIVATE *pvox) ;
static char vox_adpcm_encode (short samp, VOX_ADPCM_PRIVATE *pvox) ;

static sf_count_t vox_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t vox_read_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t vox_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t vox_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t vox_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t vox_write_i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t vox_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t vox_write_d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static int vox_read_block (SF_PRIVATE *psf, VOX_ADPCM_PRIVATE *pvox, short *ptr, int len) ;
static int vox_write_block (SF_PRIVATE *psf, VOX_ADPCM_PRIVATE *pvox, short *ptr, int len) ;

/*============================================================================================
** Predefined OKI ADPCM encoder/decoder tables.
*/

static short stepvox_adpcm_size_table [49] =
{	16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60,
	66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209,
	230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
	724, 796, 876, 963, 1060, 1166, 1282, 1408, 1552
} ; /* stepvox_adpcm_size_table */

static short stepvox_adpcm_adjust_table [8] =
{	-1, -1, -1, -1, 2, 4, 6, 8
} ; /* stepvox_adpcm_adjust_table */

/*------------------------------------------------------------------------------
*/

int
vox_adpcm_init (SF_PRIVATE *psf)
{	VOX_ADPCM_PRIVATE *pvox = NULL ;

	if (psf->mode == SFM_RDWR)
		return SFE_BAD_MODE_RW ;

	if (psf->mode == SFM_WRITE && psf->sf.channels != 1)
		return SFE_CHANNEL_COUNT ;

	if ((pvox = malloc (sizeof (VOX_ADPCM_PRIVATE))) == NULL)
		return SFE_MALLOC_FAILED ;

	psf->fdata = (void*) pvox ;
	memset (pvox, 0, sizeof (VOX_ADPCM_PRIVATE)) ;

	if (psf->mode == SFM_WRITE)
	{	psf->write_short	= vox_write_s ;
		psf->write_int		= vox_write_i ;
		psf->write_float	= vox_write_f ;
		psf->write_double	= vox_write_d ;
		}
	else
	{	psf_log_printf (psf, "Header-less OKI Dialogic ADPCM encoded file.\n") ;
		psf_log_printf (psf, "Setting up for 8kHz, mono, Vox ADPCM.\n") ;

		psf->read_short		= vox_read_s ;
		psf->read_int		= vox_read_i ;
		psf->read_float		= vox_read_f ;
		psf->read_double	= vox_read_d ;
		} ;

	/* Standard sample rate chennels etc. */
	if (psf->sf.samplerate < 1)
		psf->sf.samplerate	= 8000 ;
	psf->sf.channels	= 1 ;

	psf->sf.frames = psf->filelength * 2 ;

	psf->sf.seekable = SF_FALSE ;

	/* Seek back to start of data. */
	if (psf_fseek (psf, 0 , SEEK_SET) == -1)
		return SFE_BAD_SEEK ;

	return 0 ;
} /* vox_adpcm_init */

/*------------------------------------------------------------------------------
*/

static char
vox_adpcm_encode (short samp, VOX_ADPCM_PRIVATE *pvox)
{	short code ;
	short diff, error, stepsize ;

	stepsize = stepvox_adpcm_size_table [pvox->step_index] ;
	code = 0 ;

	diff = samp - pvox->last ;
	if (diff < 0)
	{	code = 0x08 ;
		error = -diff ;
		}
	else
		error = diff ;

	if (error >= stepsize)
	{	code = code | 0x04 ;
		error -= stepsize ;
		} ;

	if (error >= stepsize / 2)
	{	code = code | 0x02 ;
		error -= stepsize / 2 ;
		} ;

	if (error >= stepsize / 4)
		code = code | 0x01 ;

	/*
	** To close the feedback loop, the deocder is used to set the
	** estimate of last sample and in doing so, also set the step_index.
	*/
	pvox->last = vox_adpcm_decode (code, pvox) ;

	return code ;
} /* vox_adpcm_encode */

static short
vox_adpcm_decode (char code, VOX_ADPCM_PRIVATE *pvox)
{	short diff, error, stepsize, samp ;

	stepsize = stepvox_adpcm_size_table [pvox->step_index] ;

	error = stepsize / 8 ;

	if (code & 0x01)
		error += stepsize / 4 ;

	if (code & 0x02)
		error += stepsize / 2 ;

	if (code & 0x04)
		error += stepsize ;

	diff = (code & 0x08) ? -error : error ;
	samp = pvox->last + diff ;

	/*
	**  Apply clipping.
	*/
	if (samp > 2048)
		samp = 2048 ;
	if (samp < -2048)
		samp = -2048 ;

	pvox->last = samp ;
	pvox->step_index += stepvox_adpcm_adjust_table [code & 0x7] ;

	if (pvox->step_index < 0)
		pvox->step_index = 0 ;
	if (pvox->step_index > 48)
		pvox->step_index = 48 ;

	return samp ;
} /* vox_adpcm_decode */

static int
vox_adpcm_encode_block (VOX_ADPCM_PRIVATE *pvox)
{	unsigned char code ;
	int j, k ;

	/* If data_count is odd, add an extra zero valued sample. */
	if (pvox->pcm_samples & 1)
		pvox->pcm_data [pvox->pcm_samples++] = 0 ;

	for (j = k = 0 ; k < pvox->pcm_samples ; j++)
	{	code = vox_adpcm_encode (pvox->pcm_data [k++] / 16, pvox) << 4 ;
		code |= vox_adpcm_encode (pvox->pcm_data [k++] / 16, pvox) ;
		pvox->vox_data [j] = code ;
		} ;

	pvox->vox_bytes = j ;

	return 0 ;
} /* vox_adpcm_encode_block */

static int
vox_adpcm_decode_block (VOX_ADPCM_PRIVATE *pvox)
{	unsigned char code ;
	int j, k ;

	for (j = k = 0 ; j < pvox->vox_bytes ; j++)
	{	code = pvox->vox_data [j] ;
		pvox->pcm_data [k++] = 16 * vox_adpcm_decode ((code >> 4) & 0x0f, pvox) ;
		pvox->pcm_data [k++] = 16 * vox_adpcm_decode (code & 0x0f, pvox) ;
		} ;

	pvox->pcm_samples = k ;

	return 0 ;
} /* vox_adpcm_decode_block */

/*==============================================================================
*/

static int
vox_read_block (SF_PRIVATE *psf, VOX_ADPCM_PRIVATE *pvox, short *ptr, int len)
{	int	indx = 0, k ;

	while (indx < len)
	{	pvox->vox_bytes = (len - indx > PCM_DATA_LEN) ? VOX_DATA_LEN : (len - indx + 1) / 2 ;

		if ((k = psf_fread (pvox->vox_data, 1, pvox->vox_bytes, psf)) != pvox->vox_bytes)
		{	if (psf_ftell (psf) + k != psf->filelength)
				psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, pvox->vox_bytes) ;
			if (k == 0)
				break ;
			} ;

		pvox->vox_bytes = k ;

		vox_adpcm_decode_block (pvox) ;

		memcpy (&(ptr [indx]), pvox->pcm_data, pvox->pcm_samples * sizeof (short)) ;
		indx += pvox->pcm_samples ;
		} ;

	return indx ;
} /* vox_read_block */


static sf_count_t
vox_read_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE 	*pvox ;
	int			readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	while (len > 0)
	{	readcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = vox_read_block (psf, pvox, ptr, readcount) ;

		total += count ;
		len -= count ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* vox_read_s */

static sf_count_t
vox_read_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE *pvox ;
	short		*sptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		count = vox_read_block (psf, pvox, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = ((int) sptr [k]) << 16 ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* vox_read_i */

static sf_count_t
vox_read_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE *pvox ;
	short		*sptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		count = vox_read_block (psf, pvox, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (float) (sptr [k]) ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* vox_read_f */

static sf_count_t
vox_read_d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE *pvox ;
	short		*sptr ;
	int			k, bufferlen, readcount, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		count = vox_read_block (psf, pvox, sptr, readcount) ;
		for (k = 0 ; k < readcount ; k++)
			ptr [total + k] = normfact * (double) (sptr [k]) ;
		total += count ;
		len -= readcount ;
		if (count != readcount)
			break ;
		} ;

	return total ;
} /* vox_read_d */

/*------------------------------------------------------------------------------
*/

static int
vox_write_block (SF_PRIVATE *psf, VOX_ADPCM_PRIVATE *pvox, short *ptr, int len)
{	int	indx = 0, k ;

	while (indx < len)
	{	pvox->pcm_samples = (len - indx > PCM_DATA_LEN) ? PCM_DATA_LEN : len - indx ;

		memcpy (pvox->pcm_data, &(ptr [indx]), pvox->pcm_samples * sizeof (short)) ;

		vox_adpcm_encode_block (pvox) ;

		if ((k = psf_fwrite (pvox->vox_data, 1, pvox->vox_bytes, psf)) != pvox->vox_bytes)
			psf_log_printf (psf, "*** Warning : short read (%d != %d).\n", k, pvox->vox_bytes) ;

		indx += pvox->pcm_samples ;
		} ;

	return indx ;
} /* vox_write_block */

static sf_count_t
vox_write_s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE 	*pvox ;
	int			writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	while (len)
	{	writecount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		count = vox_write_block (psf, pvox, ptr, writecount) ;

		total += count ;
		len -= count ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* vox_write_s */

static sf_count_t
vox_write_i	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE *pvox ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = ptr [total + k] >> 16 ;
		count = vox_write_block (psf, pvox, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* vox_write_i */

static sf_count_t
vox_write_f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE *pvox ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrintf (normfact * ptr [total + k]) ;
		count = vox_write_block (psf, pvox, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* vox_write_f */

static sf_count_t
vox_write_d	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	VOX_ADPCM_PRIVATE *pvox ;
	short		*sptr ;
	int			k, bufferlen, writecount, count ;
	sf_count_t	total = 0 ;
	double 		normfact ;

	if (! psf->fdata)
		return 0 ;
	pvox = (VOX_ADPCM_PRIVATE*) psf->fdata ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	sptr = (short*) psf->buffer ;
	bufferlen = SF_BUFFER_LEN / sizeof (short) ;
	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		for (k = 0 ; k < writecount ; k++)
			sptr [k] = lrint (normfact * ptr [total + k]) ;
		count = vox_write_block (psf, pvox, sptr, writecount) ;
		total += count ;
		len -= writecount ;
		if (count != writecount)
			break ;
		} ;

	return total ;
} /* vox_write_d */


/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: e15e97fe-ff9d-4b46-a489-7059fb2d0b1e
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<time.h>


/*------------------------------------------------------------------------------
** W64 files use 16 byte markers as opposed to the four byte marker of
** WAV files.
** For comparison purposes, an integer is required, so make an integer
** hash for the 16 bytes using MAKE_HASH16 macro, but also create a 16
** byte array containing the complete 16 bytes required when writing the
** header.
*/

#define MAKE_HASH16(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc,xd,xe,xf)	\
			(	(x0)			^ ((x1) << 1)	^ ((x2) << 2)	^ ((x3) << 3) ^	\
				((x4) << 4) 	^ ((x5) << 5)	^ ((x6) << 6)	^ ((x7) << 7) ^	\
				((x8) << 8) 	^ ((x9) << 9)	^ ((xa) << 10)	^ ((xb) << 11) ^ \
				((xc) << 12) 	^ ((xd) << 13)	^ ((xe) << 14)	^ ((xf) << 15)	)

#define MAKE_MARKER16(name,x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc,xd,xe,xf)	\
			static unsigned char name [16] = { (x0), (x1), (x2), (x3), (x4), (x5), \
				(x6), (x7), (x8), (x9), (xa), (xb), (xc), (xd), (xe), (xf) }

#define	riff_HASH16 MAKE_HASH16 ('r', 'i', 'f', 'f', 0x2E, 0x91, 0xCF, 0x11, 0xA5, \
								0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00)

#define	wave_HASH16 	MAKE_HASH16 ('w', 'a', 'v', 'e', 0xF3, 0xAC, 0xD3, 0x11, \
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A)

#define	fmt_HASH16 		MAKE_HASH16 ('f', 'm', 't', ' ', 0xF3, 0xAC, 0xD3, 0x11, \
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A)

#define	fact_HASH16 	MAKE_HASH16 ('f', 'a', 'c', 't', 0xF3, 0xAC, 0xD3, 0x11, \
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A)

#define	data_HASH16 	MAKE_HASH16 ('d', 'a', 't', 'a', 0xF3, 0xAC, 0xD3, 0x11, \
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A)

#define	ACID_HASH16 	MAKE_HASH16 (0x6D, 0x07, 0x1C, 0xEA, 0xA3, 0xEF, 0x78, 0x4C, \
								0x90, 0x57, 0x7F, 0x79, 0xEE, 0x25, 0x2A, 0xAE)

MAKE_MARKER16 (riff_MARKER16, 'r', 'i', 'f', 'f', 0x2E, 0x91, 0xCF, 0x11,
								0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00) ;


MAKE_MARKER16 (wave_MARKER16, 'w', 'a', 'v', 'e', 0xF3, 0xAC, 0xD3, 0x11,
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A) ;

MAKE_MARKER16 (fmt_MARKER16, 'f', 'm', 't', ' ', 0xF3, 0xAC, 0xD3, 0x11,
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A) ;

MAKE_MARKER16 (fact_MARKER16, 'f', 'a', 'c', 't', 0xF3, 0xAC, 0xD3, 0x11,
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A) ;

MAKE_MARKER16 (data_MARKER16, 'd', 'a', 't', 'a', 0xF3, 0xAC, 0xD3, 0x11,
								0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A) ;

enum
{	HAVE_riff	= 0x01,
	HAVE_wave	= 0x02,
	w64HAVE_fmt	= 0x04,
	w64HAVE_fact	= 0x08,
	w64HAVE_data	= 0x20
} ;

/*------------------------------------------------------------------------------
 * Private static functions.
 */

static int	w64_read_header	(SF_PRIVATE *psf, int *blockalign, int *framesperblock) ;
static int	w64_write_header (SF_PRIVATE *psf, int calc_length) ;
static int	w64_close (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
w64_open	(SF_PRIVATE *psf)
{	int	subformat, error, blockalign = 0, framesperblock = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR &&psf->filelength > 0))
	{	if ((error = w64_read_header (psf, &blockalign, &framesperblock)))
			return error ;
		} ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_W64)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		psf->endian = SF_ENDIAN_LITTLE ;		/* All W64 files are little endian. */

		psf->blockwidth = psf->bytewidth * psf->sf.channels ;

		if (subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM)
		{	blockalign = wav_w64_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
			framesperblock = -1 ;

			/* FIXME : This block must go */
			psf->filelength = SF_COUNT_MAX ;
			psf->datalength = psf->filelength ;
			if (psf->sf.frames <= 0)
				psf->sf.frames = (psf->blockwidth) ? psf->filelength / psf->blockwidth : psf->filelength ;
			/* EMXIF : This block must go */
			} ;

		if ((error = w64_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = w64_write_header ;
		} ;

	psf->close = w64_close ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
					error = pcm_init (psf) ;
					break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
					error = pcm_init (psf) ;
					break ;

		case SF_FORMAT_ULAW :
					error = ulaw_init (psf) ;
					break ;

		case SF_FORMAT_ALAW :
					error = alaw_init (psf) ;
					break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :
					error = float32_init (psf) ;
					break ;

		case SF_FORMAT_DOUBLE :
					error = double64_init (psf) ;
					break ;

		case SF_FORMAT_IMA_ADPCM :
					error = wav_w64_ima_init (psf, blockalign, framesperblock) ;
					break ;

		case SF_FORMAT_MS_ADPCM :
					error = wav_w64_msadpcm_init (psf, blockalign, framesperblock) ;
					break ;
		/* Lite remove end */

		case SF_FORMAT_GSM610 :
					error = gsm610_init (psf) ;
					break ;

		default : 	return SFE_UNIMPLEMENTED ;
		} ;

	return error ;
} /* w64_open */

/*=========================================================================
** Private functions.
*/

static int
w64_read_header	(SF_PRIVATE *psf, int *blockalign, int *framesperblock)
{	WAV_FMT 	wav_fmt ;
	int			dword = 0, marker, format = 0 ;
	sf_count_t	chunk_size, bytesread = 0 ;
	int			parsestage = 0, error, done = 0 ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "p", 0) ;

	while (! done)
	{	/* Read the 4 byte marker and jump 12 bytes. */
		bytesread += psf_binheader_readf (psf, "h", &marker) ;
		chunk_size = 0 ;

		switch (marker)
		{	case riff_HASH16 :
					if (parsestage)
						return SFE_W64_NO_RIFF ;

					bytesread += psf_binheader_readf (psf, "e8", &chunk_size) ;

					if (psf->filelength < chunk_size)
						psf_log_printf (psf, "riff : %D (should be %D)\n", chunk_size, psf->filelength) ;
					else
						psf_log_printf (psf, "riff : %D\n", chunk_size) ;

					parsestage |= HAVE_riff ;
					break ;

			case ACID_HASH16:
					psf_log_printf (psf, "Looks like an ACID file. Exiting.\n") ;
					return SFE_UNIMPLEMENTED ;

			case wave_HASH16 :
					if ((parsestage & HAVE_riff) != HAVE_riff)
						return SFE_W64_NO_WAVE ;
					psf_log_printf (psf, "wave\n") ;
					parsestage |= HAVE_wave ;
					break ;

			case fmt_HASH16 :
					if ((parsestage & (HAVE_riff | HAVE_wave)) != (HAVE_riff | HAVE_wave))
						return SFE_W64_NO_FMT ;

					bytesread += psf_binheader_readf (psf, "e8", &chunk_size) ;
					psf_log_printf (psf, " fmt : %D\n", chunk_size) ;

					/* size of 16 byte marker and 8 byte chunk_size value. */
					chunk_size -= 24 ;

					if ((error = wav_w64_read_fmt_chunk (psf, &wav_fmt, (int) chunk_size)))
						return error ;

					if (chunk_size % 8)
						psf_binheader_readf (psf, "j", 8 - (chunk_size % 8)) ;

					format		= wav_fmt.format ;
					parsestage |= w64HAVE_fmt ;
					break ;

			case fact_HASH16:
					{	sf_count_t frames ;

						psf_binheader_readf (psf, "e88", &chunk_size, &frames) ;
						psf_log_printf (psf, "   fact : %D\n     frames : %D\n",
										chunk_size, frames) ;
						} ;
					break ;


			case data_HASH16 :
					if ((parsestage & (HAVE_riff | HAVE_wave | w64HAVE_fmt)) != (HAVE_riff | HAVE_wave | w64HAVE_fmt))
						return SFE_W64_NO_DATA ;

					psf_binheader_readf (psf, "e8", &chunk_size) ;

					psf->dataoffset = psf_ftell (psf) ;

					psf->datalength = chunk_size - 24 ;

					if (chunk_size % 8)
						chunk_size += 8 - (chunk_size % 8) ;

					psf_log_printf (psf, "data : %D\n", chunk_size) ;

					parsestage |= w64HAVE_data ;

					if (! psf->sf.seekable)
						break ;

					/* Seek past data and continue reading header. */
					psf_fseek (psf, chunk_size, SEEK_CUR) ;
					break ;

			default :
					if (psf_ftell (psf) & 0x0F)
					{	psf_log_printf (psf, "  Unknown chunk marker at position %d. Resynching.\n", dword - 4) ;
						psf_binheader_readf (psf, "j", -3) ;
						break ;
						} ;
					psf_log_printf (psf, "*** Unknown chunk marker : %X. Exiting parser.\n", marker) ;
					done = SF_TRUE ;
					break ;
			} ;	/* switch (dword) */

		if (psf->sf.seekable == 0 && (parsestage & w64HAVE_data))
			break ;

		if (psf_ftell (psf) >= (psf->filelength - (2 * SIGNED_SIZEOF (dword))))
			break ;

		if (psf->logindex >= SIGNED_SIZEOF (psf->logbuffer) - 2)
			return SFE_LOG_OVERRUN ;
		} ; /* while (1) */

	if (! psf->dataoffset)
		return SFE_W64_NO_DATA ;

	psf->endian = SF_ENDIAN_LITTLE ;		/* All WAV files are little endian. */

	if (psf_ftell (psf) != psf->dataoffset)
		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	psf->close = w64_close ;

	if (psf->blockwidth)
	{	if (psf->filelength - psf->dataoffset < psf->datalength)
			psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;
		else
			psf->sf.frames = psf->datalength / psf->blockwidth ;
		} ;

	switch (format)
	{	case WAVE_FORMAT_PCM :
		case WAVE_FORMAT_EXTENSIBLE :
					/* extensible might be FLOAT, MULAW, etc as well! */
					psf->sf.format = SF_FORMAT_W64 | u_bitwidth_to_subformat (psf->bytewidth * 8) ;
					break ;

		case WAVE_FORMAT_MULAW :
					psf->sf.format = (SF_FORMAT_W64 | SF_FORMAT_ULAW) ;
					break ;

		case WAVE_FORMAT_ALAW :
					psf->sf.format = (SF_FORMAT_W64 | SF_FORMAT_ALAW) ;
					break ;

		case WAVE_FORMAT_MS_ADPCM :
					psf->sf.format = (SF_FORMAT_W64 | SF_FORMAT_MS_ADPCM) ;
					*blockalign = wav_fmt.msadpcm.blockalign ;
					*framesperblock = wav_fmt.msadpcm.samplesperblock ;
					break ;

		case WAVE_FORMAT_IMA_ADPCM :
					psf->sf.format = (SF_FORMAT_W64 | SF_FORMAT_IMA_ADPCM) ;
					*blockalign = wav_fmt.ima.blockalign ;
					*framesperblock = wav_fmt.ima.samplesperblock ;
					break ;

		case WAVE_FORMAT_GSM610 :
					psf->sf.format = (SF_FORMAT_W64 | SF_FORMAT_GSM610) ;
					break ;

		case WAVE_FORMAT_IEEE_FLOAT :
					psf->sf.format = SF_FORMAT_W64 ;
					psf->sf.format |= (psf->bytewidth == 8) ? SF_FORMAT_DOUBLE : SF_FORMAT_FLOAT ;
					break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	return 0 ;
} /* w64_read_header */

static int
w64_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t 	fmt_size, current ;
	size_t		fmt_pad  = 0 ;
	int 		subformat, add_fact_chunk = SF_FALSE ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		if (psf->bytewidth)
			psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	/* riff marker, length, wave and 'fmt ' markers. */
	psf_binheader_writef (psf, "eh8hh", riff_MARKER16, psf->filelength - 8, wave_MARKER16, fmt_MARKER16) ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	switch (subformat)
	{	case	SF_FORMAT_PCM_U8 :
		case	SF_FORMAT_PCM_16 :
		case	SF_FORMAT_PCM_24 :
		case	SF_FORMAT_PCM_32 :
					fmt_size = 24 + 2 + 2 + 4 + 4 + 2 + 2 ;
					fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
					fmt_size += fmt_pad ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e8224", fmt_size, WAVE_FORMAT_PCM, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, psf->bytewidth * 8) ;
					break ;

		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
					fmt_size = 24 + 2 + 2 + 4 + 4 + 2 + 2 ;
					fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
					fmt_size += fmt_pad ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e8224", fmt_size, WAVE_FORMAT_IEEE_FLOAT, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, psf->bytewidth * 8) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_ULAW :
					fmt_size = 24 + 2 + 2 + 4 + 4 + 2 + 2 ;
					fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
					fmt_size += fmt_pad ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e8224", fmt_size, WAVE_FORMAT_MULAW, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, 8) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_ALAW :
					fmt_size = 24 + 2 + 2 + 4 + 4 + 2 + 2 ;
					fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
					fmt_size += fmt_pad ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e8224", fmt_size, WAVE_FORMAT_ALAW, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, 8) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		/* Lite remove start */
		case SF_FORMAT_IMA_ADPCM :
					{	int		blockalign, framesperblock, bytespersec ;

						blockalign		= wav_w64_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
						framesperblock	= 2 * (blockalign - 4 * psf->sf.channels) / psf->sf.channels + 1 ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						fmt_size = 24 + 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;
						fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
						fmt_size += fmt_pad ;

						/* fmt : size, WAV format type, channels. */
						psf_binheader_writef (psf, "e822", fmt_size, WAVE_FORMAT_IMA_ADPCM, psf->sf.channels) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "e44", psf->sf.samplerate, bytespersec) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "e2222", blockalign, 4, 2, framesperblock) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_MS_ADPCM :
					{	int blockalign, framesperblock, bytespersec, extrabytes ;

						blockalign		= wav_w64_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
						framesperblock	= 2 + 2 * (blockalign - 7 * psf->sf.channels) / psf->sf.channels ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						extrabytes	= 2 + 2 + MSADPCM_ADAPT_COEFF_COUNT * (2 + 2) ;
						fmt_size	= 24 + 2 + 2 + 4 + 4 + 2 + 2 + 2 + extrabytes ;
						fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
						fmt_size += fmt_pad ;

						/* fmt : size, W64 format type, channels. */
						psf_binheader_writef (psf, "e822", fmt_size, WAVE_FORMAT_MS_ADPCM, psf->sf.channels) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "e44", psf->sf.samplerate, bytespersec) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "e22222", blockalign, 4, extrabytes, framesperblock, 7) ;

						msadpcm_write_adapt_coeffs (psf) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;
		/* Lite remove end */

		case SF_FORMAT_GSM610 :
					{	int bytespersec ;

						bytespersec = (psf->sf.samplerate * WAV_W64_GSM610_BLOCKSIZE) / WAV_W64_GSM610_SAMPLES ;

						/* fmt chunk. */
						fmt_size = 24 + 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;
						fmt_pad = (size_t) (8 - (fmt_size & 0x7)) ;
						fmt_size += fmt_pad ;

						/* fmt : size, WAV format type, channels. */
						psf_binheader_writef (psf, "e822", fmt_size, WAVE_FORMAT_GSM610, psf->sf.channels) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "e44", psf->sf.samplerate, bytespersec) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "e2222", WAV_W64_GSM610_BLOCKSIZE, 0, 2, WAV_W64_GSM610_SAMPLES) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;

		default : 	return SFE_UNIMPLEMENTED ;
		} ;

	/* Pad to 8 bytes with zeros. */
	if (fmt_pad > 0)
		psf_binheader_writef (psf, "z", fmt_pad) ;

	if (add_fact_chunk)
		psf_binheader_writef (psf, "eh88", fact_MARKER16, 16 + 8 + 8, psf->sf.frames) ;

	psf_binheader_writef (psf, "eh8", data_MARKER16, psf->datalength + 24) ;
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* w64_write_header */

static int
w64_close (SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		w64_write_header (psf, SF_TRUE) ;

	return 0 ;
} /* w64_close */


/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 9aa4e141-538a-4dd9-99c9-b3f0f2dd4f4a
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
** Copyright (C) 2004 David Viens <davidv@plogue.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<time.h>


/*------------------------------------------------------------------------------
 * Macros to handle big/little endian issues.
 */

#define RIFF_MARKER	 (MAKE_MARKER ('R', 'I', 'F', 'F'))
#define WAVE_MARKER	 (MAKE_MARKER ('W', 'A', 'V', 'E'))
#define fmt_MARKER	 (MAKE_MARKER ('f', 'm', 't', ' '))
#define data_MARKER	 (MAKE_MARKER ('d', 'a', 't', 'a'))
#define fact_MARKER	 (MAKE_MARKER ('f', 'a', 'c', 't'))
#define PEAK_MARKER	 (MAKE_MARKER ('P', 'E', 'A', 'K'))

#define cue_MARKER	 (MAKE_MARKER ('c', 'u', 'e', ' '))
#define LIST_MARKER	 (MAKE_MARKER ('L', 'I', 'S', 'T'))
#define slnt_MARKER	 (MAKE_MARKER ('s', 'l', 'n', 't'))
#define wavl_MARKER	 (MAKE_MARKER ('w', 'a', 'v', 'l'))
#define INFO_MARKER	 (MAKE_MARKER ('I', 'N', 'F', 'O'))
#define plst_MARKER	 (MAKE_MARKER ('p', 'l', 's', 't'))
#define adtl_MARKER	 (MAKE_MARKER ('a', 'd', 't', 'l'))
#define labl_MARKER	 (MAKE_MARKER ('l', 'a', 'b', 'l'))
#define ltxt_MARKER	 (MAKE_MARKER ('l', 't', 'x', 't'))
#define note_MARKER	 (MAKE_MARKER ('n', 'o', 't', 'e'))
#define smpl_MARKER	 (MAKE_MARKER ('s', 'm', 'p', 'l'))
#define bext_MARKER	 (MAKE_MARKER ('b', 'e', 'x', 't'))
#define MEXT_MARKER	 (MAKE_MARKER ('M', 'E', 'X', 'T'))
#define DISP_MARKER	 (MAKE_MARKER ('D', 'I', 'S', 'P'))
#define acid_MARKER	 (MAKE_MARKER ('a', 'c', 'i', 'd'))
#define strc_MARKER	 (MAKE_MARKER ('s', 't', 'r', 'c'))
#define PAD_MARKER	 (MAKE_MARKER ('P', 'A', 'D', ' '))
#define afsp_MARKER	 (MAKE_MARKER ('a', 'f', 's', 'p'))
#define clm_MARKER	 (MAKE_MARKER ('c', 'l', 'm', ' '))

#define ISFT_MARKER	 (MAKE_MARKER ('I', 'S', 'F', 'T'))
#define ICRD_MARKER	 (MAKE_MARKER ('I', 'C', 'R', 'D'))
#define ICOP_MARKER	 (MAKE_MARKER ('I', 'C', 'O', 'P'))
#define IARL_MARKER	 (MAKE_MARKER ('I', 'A', 'R', 'L'))
#define IART_MARKER	 (MAKE_MARKER ('I', 'A', 'R', 'T'))
#define INAM_MARKER	 (MAKE_MARKER ('I', 'N', 'A', 'M'))
#define IENG_MARKER	 (MAKE_MARKER ('I', 'E', 'N', 'G'))
#define IART_MARKER	 (MAKE_MARKER ('I', 'A', 'R', 'T'))
#define ICOP_MARKER	 (MAKE_MARKER ('I', 'C', 'O', 'P'))
#define IPRD_MARKER	 (MAKE_MARKER ('I', 'P', 'R', 'D'))
#define ISRC_MARKER	 (MAKE_MARKER ('I', 'S', 'R', 'C'))
#define ISBJ_MARKER	 (MAKE_MARKER ('I', 'S', 'B', 'J'))
#define ICMT_MARKER	 (MAKE_MARKER ('I', 'C', 'M', 'T'))

/* Weird WAVPACK marker which can show up at the start of the DATA section. */
#define wvpk_MARKER (MAKE_MARKER ('w', 'v', 'p', 'k'))
#define OggS_MARKER (MAKE_MARKER ('O', 'g', 'g', 'S'))

enum
{	HAVE_RIFF	= 0x01,
	HAVE_WAVE	= 0x02,
	wavHAVE_fmt	= 0x04,
	wavHAVE_fact	= 0x08,
	HAVE_PEAK	= 0x10,
	wavHAVE_data	= 0x20,
	HAVE_other  = 0x80000000
} ;



/*  known WAVEFORMATEXTENSIBLE GUIDS  */
static const EXT_SUBFORMAT MSGUID_SUBTYPE_PCM =
{	0x00000001, 0x0000, 0x0010, {	0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
} ;

static const EXT_SUBFORMAT MSGUID_SUBTYPE_MS_ADPCM =
{	0x00000002, 0x0000, 0x0010, {	0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
} ;

static const EXT_SUBFORMAT MSGUID_SUBTYPE_IEEE_FLOAT =
{	0x00000003, 0x0000, 0x0010, {	0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
} ;

static const EXT_SUBFORMAT MSGUID_SUBTYPE_ALAW =
{	0x00000006, 0x0000, 0x0010, {	0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
} ;

static const EXT_SUBFORMAT MSGUID_SUBTYPE_MULAW =
{	0x00000007, 0x0000, 0x0010, {	0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
} ;

#if 0
/* maybe interesting one day to read the following through sf_read_raw */
/* http://www.bath.ac.uk/~masrwd/pvocex/pvocex.html */
static const EXT_SUBFORMAT MSGUID_SUBTYPE_PVOCEX =
{	0x8312B9C2, 0x2E6E, 0x11d4, {	0xA8, 0x24, 0xDE, 0x5B, 0x96, 0xC3, 0xAB, 0x21 }
} ;
#endif

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int	wav_read_header	 (SF_PRIVATE *psf, int *blockalign, int *framesperblock) ;
static int	wav_write_header (SF_PRIVATE *psf, int calc_length) ;

static void wavex_write_guid (SF_PRIVATE *psf, const EXT_SUBFORMAT * subformat) ;
static int	wavex_write_header (SF_PRIVATE *psf, int calc_length) ;

static int	wav_write_tailer (SF_PRIVATE *psf) ;
static void wav_write_strings (SF_PRIVATE *psf, int location) ;
static int	wav_command (SF_PRIVATE *psf, int command, void *data, int datasize) ;
static int	wav_close (SF_PRIVATE *psf) ;

static int 	wav_subchunk_parse	 (SF_PRIVATE *psf, int chunk) ;
static int	wav_read_smpl_chunk (SF_PRIVATE *psf, unsigned int chunklen) ;
static int	wav_read_acid_chunk (SF_PRIVATE *psf, unsigned int chunklen) ;

static int wavex_write_guid_equal (const EXT_SUBFORMAT * first, const EXT_SUBFORMAT * second) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
wav_open	 (SF_PRIVATE *psf)
{	int	format, subformat, error, blockalign = 0, framesperblock = 0 ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = wav_read_header (psf, &blockalign, &framesperblock)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		format = psf->sf.format & SF_FORMAT_TYPEMASK ;
		if (format != SF_FORMAT_WAV && format != SF_FORMAT_WAVEX)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = SF_ENDIAN_LITTLE ;	/* All WAV files are little endian. */

		psf->blockwidth = psf->bytewidth * psf->sf.channels ;

		if (psf->mode != SFM_RDWR || psf->filelength < 44)
		{	psf->filelength = 0 ;
			psf->datalength = 0 ;
			psf->dataoffset = 0 ;
			psf->sf.frames = 0 ;
			} ;

		if (subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM)
		{	blockalign = wav_w64_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
			framesperblock = -1 ; /* Corrected later. */
			} ;

		psf->str_flags = SF_STR_ALLOW_START | SF_STR_ALLOW_END ;

		/* By default, add the peak chunk to floating point files. Default behaviour
		** can be switched off using sf_command (SFC_SET_PEAK_CHUNK, SF_FALSE).
		*/
		if (psf->mode == SFM_WRITE && (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE))
		{	psf->pchunk = calloc (1, sizeof (PEAK_CHUNK) * psf->sf.channels * sizeof (PEAK_POS)) ;
			if (psf->pchunk == NULL)
				return SFE_MALLOC_FAILED ;
			psf->has_peak = SF_TRUE ;
			psf->peak_loc = SF_PEAK_START ;
			} ;

		psf->write_header = (format == SF_FORMAT_WAV) ? wav_write_header : wavex_write_header ;
		} ;

	psf->close = wav_close ;
	psf->command = wav_command ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
					error = pcm_init (psf) ;
					break ;

		case SF_FORMAT_ULAW :
					error = ulaw_init (psf) ;
					break ;

		case SF_FORMAT_ALAW :
					error = alaw_init (psf) ;
					break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :
					error = float32_init (psf) ;
					break ;

		case SF_FORMAT_DOUBLE :
					error = double64_init (psf) ;
					break ;

		case SF_FORMAT_IMA_ADPCM :
					error = wav_w64_ima_init (psf, blockalign, framesperblock) ;
					break ;

		case SF_FORMAT_MS_ADPCM :
					error = wav_w64_msadpcm_init (psf, blockalign, framesperblock) ;
					break ;
		/* Lite remove end */

		case SF_FORMAT_GSM610 :
					error = gsm610_init (psf) ;
					break ;

		default : 	return SFE_UNIMPLEMENTED ;
		} ;

	if (psf->mode == SFM_WRITE || (psf->mode == SFM_RDWR && psf->filelength == 0))
		return psf->write_header (psf, SF_FALSE) ;

	return error ;
} /* wav_open */

/*=========================================================================
** Private functions.
*/

static int
wav_read_header	 (SF_PRIVATE *psf, int *blockalign, int *framesperblock)
{	WAV_FMT		wav_fmt ;
	FACT_CHUNK	fact_chunk ;
	int			dword, marker, RIFFsize, done = 0 ;
	int			parsestage = 0, error, format = 0 ;
	char		*cptr ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "p", 0) ;

	while (! done)
	{	psf_binheader_readf (psf, "m", &marker) ;

		switch (marker)
		{	case RIFF_MARKER :
					if (parsestage)
						return SFE_WAV_NO_RIFF ;

					parsestage |= HAVE_RIFF ;

					psf_binheader_readf (psf, "e4", &RIFFsize) ;

					if (psf->fileoffset > 0 && psf->filelength > RIFFsize + 8)
					{	/* Set file length. */
						psf->filelength = RIFFsize + 8 ;
						psf_log_printf (psf, "RIFF : %u\n", RIFFsize) ;
						}
					else if (psf->filelength < RIFFsize + 2 * SIGNED_SIZEOF (dword))
					{	psf_log_printf (psf, "RIFF : %u (should be %D)\n", RIFFsize, psf->filelength - 2 * SIGNED_SIZEOF (dword)) ;
						RIFFsize = dword ;
						}
					else
						psf_log_printf (psf, "RIFF : %u\n", RIFFsize) ;

					break ;

			case WAVE_MARKER :
					if ((parsestage & HAVE_RIFF) != HAVE_RIFF)
						return SFE_WAV_NO_WAVE ;
					parsestage |= HAVE_WAVE ;

					psf_log_printf (psf, "WAVE\n") ;
					break ;

			case fmt_MARKER :
					if ((parsestage & (HAVE_RIFF | HAVE_WAVE)) != (HAVE_RIFF | HAVE_WAVE))
						return SFE_WAV_NO_FMT ;

					/* If this file has a SECOND fmt chunk, I don't want to know about it. */
					if (parsestage & wavHAVE_fmt)
						break ;

					parsestage |= wavHAVE_fmt ;

					psf_binheader_readf (psf, "e4", &dword) ;
					psf_log_printf (psf, "fmt  : %d\n", dword) ;

					if ((error = wav_w64_read_fmt_chunk (psf, &wav_fmt, dword)))
						return error ;

					format = wav_fmt.format ;
					break ;

			case data_MARKER :
					if ((parsestage & (HAVE_RIFF | HAVE_WAVE | wavHAVE_fmt)) != (HAVE_RIFF | HAVE_WAVE | wavHAVE_fmt))
						return SFE_WAV_NO_DATA ;

					if (psf->mode == SFM_RDWR && (parsestage & HAVE_other) != 0)
						return SFE_RDWR_BAD_HEADER ;

					parsestage |= wavHAVE_data ;

					psf_binheader_readf (psf, "e4", &dword) ;

					psf->datalength = dword ;
					psf->dataoffset = psf_ftell (psf) ;

					if (dword == 0 && RIFFsize == 8 && psf->filelength > 44)
					{	psf_log_printf (psf, "*** Looks like a WAV file which wasn't closed properly. Fixing it.\n") ;
						psf->datalength = dword = psf->filelength - psf->dataoffset ;
						} ;

					if (psf->datalength > psf->filelength - psf->dataoffset)
					{	psf_log_printf (psf, "data : %D (should be %D)\n", psf->datalength, psf->filelength - psf->dataoffset) ;
						psf->datalength = psf->filelength - psf->dataoffset ;
						}
					else
						psf_log_printf (psf, "data : %D\n", psf->datalength) ;

					/* Only set dataend if there really is data at the end. */
					if (psf->datalength + psf->dataoffset < psf->filelength)
						psf->dataend = psf->datalength + psf->dataoffset ;

					if (format == WAVE_FORMAT_MS_ADPCM && psf->datalength % 2)
					{	psf->datalength ++ ;
						psf_log_printf (psf, "*** Data length odd. Increasing it by 1.\n") ;
						} ;

					if (! psf->sf.seekable)
						break ;

					/* Seek past data and continue reading header. */
					psf_fseek (psf, psf->datalength, SEEK_CUR) ;

					dword = psf_ftell (psf) ;
					if (dword != (sf_count_t) (psf->dataoffset + psf->datalength))
						psf_log_printf (psf, "*** psf_fseek past end error ***\n", dword, psf->dataoffset + psf->datalength) ;
					break ;

			case fact_MARKER :
					if ((parsestage & (HAVE_RIFF | HAVE_WAVE)) != (HAVE_RIFF | HAVE_WAVE))
						return SFE_WAV_BAD_FACT ;

					parsestage |= wavHAVE_fact ;

					if ((parsestage & wavHAVE_fmt) != wavHAVE_fmt)
						psf_log_printf (psf, "*** Should have 'fmt ' chunk before 'fact'\n") ;

					psf_binheader_readf (psf, "e44", &dword, & (fact_chunk.frames)) ;

					if (dword > SIGNED_SIZEOF (fact_chunk))
						psf_binheader_readf (psf, "j", (int) (dword - SIGNED_SIZEOF (fact_chunk))) ;

					if (dword)
						psf_log_printf (psf, "%M : %d\n", marker, dword) ;
					else
						psf_log_printf (psf, "%M : %d (should not be zero)\n", marker, dword) ;

					psf_log_printf (psf, "  frames  : %d\n", fact_chunk.frames) ;
					break ;

			case PEAK_MARKER :
					if ((parsestage & (HAVE_RIFF | HAVE_WAVE | wavHAVE_fmt)) != (HAVE_RIFF | HAVE_WAVE | wavHAVE_fmt))
						return SFE_WAV_PEAK_B4_FMT ;

					parsestage |= HAVE_PEAK ;

					psf_binheader_readf (psf, "e4", &dword) ;

					psf_log_printf (psf, "%M : %d\n", marker, dword) ;
					if (dword != SIGNED_SIZEOF (PEAK_CHUNK) + psf->sf.channels * SIGNED_SIZEOF (PEAK_POS))
					{	psf_binheader_readf (psf, "j", dword) ;
						psf_log_printf (psf, "*** File PEAK chunk size doesn't fit with number of channels.\n") ;
						return SFE_WAV_BAD_PEAK ;
						} ;

					psf->pchunk = calloc (1, sizeof (PEAK_CHUNK) * psf->sf.channels * sizeof (PEAK_POS)) ;
					if (psf->pchunk == NULL)
						return SFE_MALLOC_FAILED ;

					/* read in rest of PEAK chunk. */
					psf_binheader_readf (psf, "e44", & (psf->pchunk->version), & (psf->pchunk->timestamp)) ;

					if (psf->pchunk->version != 1)
						psf_log_printf (psf, "  version    : %d *** (should be version 1)\n", psf->pchunk->version) ;
					else
						psf_log_printf (psf, "  version    : %d\n", psf->pchunk->version) ;

					psf_log_printf (psf, "  time stamp : %d\n", psf->pchunk->timestamp) ;
					psf_log_printf (psf, "    Ch   Position       Value\n") ;

					cptr = (char *) psf->buffer ;
					for (dword = 0 ; dword < psf->sf.channels ; dword++)
					{	psf_binheader_readf (psf, "ef4", & (psf->pchunk->peaks [dword].value),
														& (psf->pchunk->peaks [dword].position)) ;

						LSF_SNPRINTF (cptr, sizeof (psf->buffer), "    %2d   %-12d   %g\n",
								dword, psf->pchunk->peaks [dword].position, psf->pchunk->peaks [dword].value) ;
						cptr [sizeof (psf->buffer) - 1] = 0 ;
						psf_log_printf (psf, cptr) ;
						} ;

					psf->has_peak = SF_TRUE ; /* Found PEAK chunk. */
					psf->peak_loc = ((parsestage & wavHAVE_data) == 0) ? SF_PEAK_START : SF_PEAK_END ;
					break ;

			case cue_MARKER :
					parsestage |= HAVE_other ;

					{	int bytesread, cue_count ;
						int id, position, chunk_id, chunk_start, block_start, offset ;

						bytesread = psf_binheader_readf (psf, "e44", &dword, &cue_count) ;
						bytesread -= 4 ; /* Remove bytes for first dword. */
						psf_log_printf (psf, "%M : %u\n  Count : %d\n", marker, dword, cue_count) ;

						while (cue_count)
						{	bytesread += psf_binheader_readf (psf, "e444444", &id, &position,
									&chunk_id, &chunk_start, &block_start, &offset) ;
							psf_log_printf (psf, "   Cue ID : %2d"
												 "  Pos : %5u  Chunk : %M"
												 "  Chk Start : %d  Blk Start : %d"
												 "  Offset : %5d\n",
									id, position, chunk_id, chunk_start, block_start, offset) ;
							cue_count -- ;
							} ;

						if (bytesread != dword)
						{	psf_log_printf (psf, "**** Chunk size weirdness (%d != %d)\n", dword, bytesread) ;
							psf_binheader_readf (psf, "j", dword - bytesread) ;
							} ;
						} ;
					break ;

			case smpl_MARKER :
					parsestage |= HAVE_other ;

					psf_binheader_readf (psf, "e4", &dword) ;
					psf_log_printf (psf, "smpl : %u\n", dword) ;

					if ((error = wav_read_smpl_chunk (psf, dword)))
						return error ;
					break ;

			case acid_MARKER :
					parsestage |= HAVE_other ;

					psf_binheader_readf (psf, "e4", &dword) ;
					psf_log_printf (psf, "acid : %u\n", dword) ;

					if ((error = wav_read_acid_chunk (psf, dword)))
						return error ;
					break ;

			case INFO_MARKER :
			case LIST_MARKER :
					parsestage |= HAVE_other ;

					if ((error = wav_subchunk_parse (psf, marker)) != 0)
						return error ;
					break ;

			case strc_MARKER : /* Multiple of 32 bytes. */

			case afsp_MARKER :
			case bext_MARKER :
			case clm_MARKER :
			case plst_MARKER :
			case DISP_MARKER :
			case MEXT_MARKER :
			case PAD_MARKER :
					parsestage |= HAVE_other ;

					psf_binheader_readf (psf, "e4", &dword) ;
					psf_log_printf (psf, "%M : %u\n", marker, dword) ;
					dword += (dword & 1) ;
					psf_binheader_readf (psf, "j", dword) ;
					break ;

			default :
					if (isprint ((marker >> 24) & 0xFF) && isprint ((marker >> 16) & 0xFF)
						&& isprint ((marker >> 8) & 0xFF) && isprint (marker & 0xFF))
					{	psf_binheader_readf (psf, "e4", &dword) ;
						psf_log_printf (psf, "*** %M : %d (unknown marker)\n", marker, dword) ;
						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;
					if (psf_ftell (psf) & 0x03)
					{	psf_log_printf (psf, "  Unknown chunk marker at position %d. Resynching.\n", dword - 4) ;
						psf_binheader_readf (psf, "j", -3) ;
						break ;
						} ;
					psf_log_printf (psf, "*** Unknown chunk marker : %X. Exiting parser.\n", marker) ;
					done = SF_TRUE ;
					break ;
			} ;	/* switch (dword) */

		if (! psf->sf.seekable && (parsestage & wavHAVE_data))
			break ;

		if (psf_ftell (psf) >= psf->filelength - SIGNED_SIZEOF (dword))
		{	psf_log_printf (psf, "End\n") ;
			break ;
			} ;

		if (psf->logindex >= SIGNED_SIZEOF (psf->logbuffer) - 2)
			return SFE_LOG_OVERRUN ;
		} ; /* while (1) */

	if (! psf->dataoffset)
		return SFE_WAV_NO_DATA ;

	psf->endian = SF_ENDIAN_LITTLE ;		/* All WAV files are little endian. */

	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	if (psf->is_pipe == 0)
	{	/*
		** Check for 'wvpk' at the start of the DATA section. Not able to
		** handle this.
		*/
		psf_binheader_readf (psf, "e4", &marker) ;
		if (marker == wvpk_MARKER || marker == OggS_MARKER)
			return SFE_WAV_WVPK_DATA ;
		} ;

	/* Seek to start of DATA section. */
	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	psf->close = wav_close ;

	if (psf->blockwidth)
	{	if (psf->filelength - psf->dataoffset < psf->datalength)
			psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;
		else
			psf->sf.frames = psf->datalength / psf->blockwidth ;
		} ;

	switch (format)
	{

		case WAVE_FORMAT_EXTENSIBLE :
			/* compare GUIDs for known ones */
			if (wavex_write_guid_equal (&wav_fmt.ext.esf, &MSGUID_SUBTYPE_PCM))
				psf->sf.format = SF_FORMAT_WAVEX | u_bitwidth_to_subformat (psf->bytewidth * 8) ;
			else
			if (wavex_write_guid_equal (&wav_fmt.ext.esf, &MSGUID_SUBTYPE_MS_ADPCM))
			{	psf->sf.format = (SF_FORMAT_WAVEX | SF_FORMAT_MS_ADPCM) ;
				*blockalign = wav_fmt.msadpcm.blockalign ;
				*framesperblock = wav_fmt.msadpcm.samplesperblock ;
				}
			else if (wavex_write_guid_equal (&wav_fmt.ext.esf, &MSGUID_SUBTYPE_IEEE_FLOAT))
				psf->sf.format = SF_FORMAT_WAVEX | ((psf->bytewidth == 8) ? SF_FORMAT_DOUBLE : SF_FORMAT_FLOAT) ;
			else if (wavex_write_guid_equal (&wav_fmt.ext.esf, &MSGUID_SUBTYPE_ALAW))
				psf->sf.format = (SF_FORMAT_WAVEX | SF_FORMAT_ALAW) ;
			else if (wavex_write_guid_equal (&wav_fmt.ext.esf, &MSGUID_SUBTYPE_MULAW))
				psf->sf.format = (SF_FORMAT_WAVEX | SF_FORMAT_ULAW) ;
			else
				return SFE_UNIMPLEMENTED ;
			break ;

		case WAVE_FORMAT_PCM :
					psf->sf.format = SF_FORMAT_WAV | u_bitwidth_to_subformat (psf->bytewidth * 8) ;
					break ;

		case WAVE_FORMAT_MULAW :
		case IBM_FORMAT_MULAW :
					psf->sf.format = (SF_FORMAT_WAV | SF_FORMAT_ULAW) ;
					break ;

		case WAVE_FORMAT_ALAW :
		case IBM_FORMAT_ALAW :
					psf->sf.format = (SF_FORMAT_WAV | SF_FORMAT_ALAW) ;
					break ;

		case WAVE_FORMAT_MS_ADPCM :
					psf->sf.format = (SF_FORMAT_WAV | SF_FORMAT_MS_ADPCM) ;
					*blockalign = wav_fmt.msadpcm.blockalign ;
					*framesperblock = wav_fmt.msadpcm.samplesperblock ;
					break ;

		case WAVE_FORMAT_IMA_ADPCM :
					psf->sf.format = (SF_FORMAT_WAV | SF_FORMAT_IMA_ADPCM) ;
					*blockalign = wav_fmt.ima.blockalign ;
					*framesperblock = wav_fmt.ima.samplesperblock ;
					break ;

		case WAVE_FORMAT_GSM610 :
					psf->sf.format = (SF_FORMAT_WAV | SF_FORMAT_GSM610) ;
					break ;

		case WAVE_FORMAT_IEEE_FLOAT :
					psf->sf.format = SF_FORMAT_WAV ;
					psf->sf.format |= (psf->bytewidth == 8) ? SF_FORMAT_DOUBLE : SF_FORMAT_FLOAT ;
					break ;

		default : return SFE_UNIMPLEMENTED ;
		} ;

	return 0 ;
} /* wav_read_header */

static int
wav_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int 		fmt_size, k, subformat, add_fact_chunk = SF_FALSE ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;

		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		if (psf->bytewidth > 0)
			psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	/* RIFF marker, length, WAVE and 'fmt ' markers. */
	if (psf->filelength < 8)
		psf_binheader_writef (psf, "etm8", RIFF_MARKER, 8) ;
	else
		psf_binheader_writef (psf, "etm8", RIFF_MARKER, psf->filelength - 8) ;

	/* WAVE and 'fmt ' markers. */
	psf_binheader_writef (psf, "emm", WAVE_MARKER, fmt_MARKER) ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e4224", fmt_size, WAVE_FORMAT_PCM, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, psf->bytewidth * 8) ;
					break ;

		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e4224", fmt_size, WAVE_FORMAT_IEEE_FLOAT, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, psf->bytewidth * 8) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_ULAW :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e4224", fmt_size, WAVE_FORMAT_MULAW, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, 8) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_ALAW :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "e4224", fmt_size, WAVE_FORMAT_ALAW, psf->sf.channels, psf->sf.samplerate) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, 8) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		/* Lite remove start */
		case SF_FORMAT_IMA_ADPCM :
					{	int blockalign, framesperblock, bytespersec ;

						blockalign		= wav_w64_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
						framesperblock	= 2 * (blockalign - 4 * psf->sf.channels) / psf->sf.channels + 1 ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;

						/* fmt : size, WAV format type, channels, samplerate, bytespersec */
						psf_binheader_writef (psf, "e42244", fmt_size, WAVE_FORMAT_IMA_ADPCM,
									psf->sf.channels, psf->sf.samplerate, bytespersec) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "e2222", blockalign, 4, 2, framesperblock) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_MS_ADPCM :
					{	int	blockalign, framesperblock, bytespersec, extrabytes ;

						blockalign		= wav_w64_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
						framesperblock	= 2 + 2 * (blockalign - 7 * psf->sf.channels) / psf->sf.channels ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						extrabytes	= 2 + 2 + MSADPCM_ADAPT_COEFF_COUNT * (2 + 2) ;
						fmt_size	= 2 + 2 + 4 + 4 + 2 + 2 + 2 + extrabytes ;

						/* fmt : size, WAV format type, channels. */
						psf_binheader_writef (psf, "e422", fmt_size, WAVE_FORMAT_MS_ADPCM, psf->sf.channels) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "e44", psf->sf.samplerate, bytespersec) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "e22222", blockalign, 4, extrabytes, framesperblock, 7) ;

						msadpcm_write_adapt_coeffs (psf) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;
		/* Lite remove end */

		case SF_FORMAT_GSM610 :
					{	int	blockalign, framesperblock, bytespersec ;

						blockalign		= WAV_W64_GSM610_BLOCKSIZE ;
						framesperblock	= WAV_W64_GSM610_SAMPLES ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;

						/* fmt : size, WAV format type, channels. */
						psf_binheader_writef (psf, "e422", fmt_size, WAVE_FORMAT_GSM610, psf->sf.channels) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "e44", psf->sf.samplerate, bytespersec) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "e2222", blockalign, 0, 2, framesperblock) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;

		default : 	return SFE_UNIMPLEMENTED ;
		} ;

	if (add_fact_chunk)
		psf_binheader_writef (psf, "etm48", fact_MARKER, 4, psf->sf.frames) ;

	if (psf->str_flags & SF_STR_LOCATE_START)
		wav_write_strings (psf, SF_STR_LOCATE_START) ;

	if (psf->has_peak && psf->peak_loc == SF_PEAK_START)
	{	psf_binheader_writef (psf, "em4", PEAK_MARKER,
			sizeof (PEAK_CHUNK) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "e44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "ef4", psf->pchunk->peaks [k].value, psf->pchunk->peaks [k].position) ;
		} ;

	psf_binheader_writef (psf, "etm8", data_MARKER, psf->datalength) ;
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;
	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current < psf->dataoffset)
		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
	else if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* wav_write_header */



static int
wavex_write_guid_equal (const EXT_SUBFORMAT * first, const EXT_SUBFORMAT * second)
{	return !memcmp (first, second, sizeof (EXT_SUBFORMAT)) ;
} /* wavex_write_guid_equal */


static void
wavex_write_guid (SF_PRIVATE *psf, const EXT_SUBFORMAT * subformat)
{
	psf_binheader_writef (psf, "e422b", subformat->esf_field1,
					subformat->esf_field2, subformat->esf_field3,
					subformat->esf_field4, 8) ;
} /* wavex_write_guid */


static int
wavex_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;
	int 		fmt_size, k, subformat, add_fact_chunk = SF_FALSE ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;

		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		if (psf->bytewidth > 0)
			psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;


	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	/* RIFF marker, length, WAVE and 'fmt ' markers. */
	if (psf->filelength < 8)
		psf_binheader_writef (psf, "etm8", RIFF_MARKER, 8) ;
	else
		psf_binheader_writef (psf, "etm8", RIFF_MARKER, psf->filelength - 8) ;

	/* WAVE and 'fmt ' markers. */
	psf_binheader_writef (psf, "emm", WAVE_MARKER, fmt_MARKER) ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	/* initial section (same for all, it appears) */
	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
		case SF_FORMAT_ULAW :
		case SF_FORMAT_ALAW :
			fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 2 + 2 + 8 ;

			/* fmt : format, channels, samplerate */
			psf_binheader_writef (psf, "e4224", fmt_size, WAVE_FORMAT_EXTENSIBLE, psf->sf.channels, psf->sf.samplerate) ;
			/*  fmt : bytespersec */
			psf_binheader_writef (psf, "e4", psf->sf.samplerate * psf->bytewidth * psf->sf.channels) ;
			/*  fmt : blockalign, bitwidth */
			psf_binheader_writef (psf, "e22", psf->bytewidth * psf->sf.channels, psf->bytewidth * 8) ;

			/* cbSize 22 is sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX) */
			psf_binheader_writef (psf, "e2", 22) ;

			/* wValidBitsPerSample, for our use same as bitwidth as we use it fully */
			psf_binheader_writef (psf, "e2", psf->bytewidth * 8) ;

			if (psf->sf.channels == 2)
				psf_binheader_writef (psf, "e4", 0x1 | 0x2 ) ;	/* dwChannelMask front left and right */
			else
				psf_binheader_writef (psf, "e4", 0) ;			/* dwChannelMask = 0 when in doubt */
			break ;

		case SF_FORMAT_MS_ADPCM : /* todo, GUID exists might have different header as per wav_write_header */
		default : return SFE_UNIMPLEMENTED ;
		} ;

	/* GUI section, different for each */

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
			wavex_write_guid (psf, &MSGUID_SUBTYPE_PCM) ;
			break ;

		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
			wavex_write_guid (psf, &MSGUID_SUBTYPE_IEEE_FLOAT) ;
			add_fact_chunk = SF_TRUE ;
			break ;

		case SF_FORMAT_ULAW :
			wavex_write_guid (psf, &MSGUID_SUBTYPE_MULAW) ;
			add_fact_chunk = SF_TRUE ;
			break ;

		case SF_FORMAT_ALAW :
			wavex_write_guid (psf, &MSGUID_SUBTYPE_ALAW) ;
			add_fact_chunk = SF_TRUE ;
			break ;

		case SF_FORMAT_MS_ADPCM : /* todo, GUID exists */

		default : return SFE_UNIMPLEMENTED ;
		} ;


	if (add_fact_chunk)
		psf_binheader_writef (psf, "etm48", fact_MARKER, 4, psf->sf.frames) ;

	if (psf->str_flags & SF_STR_LOCATE_START)
		wav_write_strings (psf, SF_STR_LOCATE_START) ;

	if (psf->has_peak && psf->peak_loc == SF_PEAK_START)
	{	psf_binheader_writef (psf, "em4", PEAK_MARKER,
			sizeof (PEAK_CHUNK) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "e44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "ef4", psf->pchunk->peaks [k].value, psf->pchunk->peaks [k].position) ;
		} ;

	psf_binheader_writef (psf, "etm8", data_MARKER, psf->datalength) ;
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;
	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current < psf->dataoffset)
		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
	else if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* wavex_write_header */



static int
wav_write_tailer (SF_PRIVATE *psf)
{	int		k ;

	/* Reset the current header buffer length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	psf->dataend = psf_fseek (psf, 0, SEEK_END) ;

	/* Add a PEAK chunk if requested. */
	if (psf->has_peak && psf->peak_loc == SF_PEAK_END)
	{	psf_binheader_writef (psf, "em4", PEAK_MARKER,
			sizeof (PEAK_CHUNK) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "e44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "ef4", psf->pchunk->peaks [k].value, psf->pchunk->peaks [k].position) ;
		} ;

	if (psf->str_flags & SF_STR_LOCATE_END)
		wav_write_strings (psf, SF_STR_LOCATE_END) ;

	/* Write the tailer. */
	if (psf->headindex > 0)
		psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	return 0 ;
} /* wav_write_tailer */

static void
wav_write_strings (SF_PRIVATE *psf, int location)
{	int	k, prev_head_index, saved_head_index ;

	prev_head_index = psf->headindex + 4 ;

	psf_binheader_writef (psf, "em4m", LIST_MARKER, 0xBADBAD, INFO_MARKER) ;

	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
	{	if (psf->strings [k].type == 0)
			break ;
		if (psf->strings [k].flags != location)
			continue ;

		switch (psf->strings [k].type)
		{	case SF_STR_SOFTWARE :
				psf_binheader_writef (psf, "ems", ISFT_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_TITLE :
				psf_binheader_writef (psf, "ems", INAM_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_COPYRIGHT :
				psf_binheader_writef (psf, "ems", ICOP_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_ARTIST :
				psf_binheader_writef (psf, "ems", IART_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_COMMENT :
				psf_binheader_writef (psf, "ems", ICMT_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_DATE :
				psf_binheader_writef (psf, "ems", ICRD_MARKER, psf->strings [k].str) ;
				break ;
			} ;
		} ;

	saved_head_index = psf->headindex ;
	psf->headindex = prev_head_index ;
	psf_binheader_writef (psf, "e4", saved_head_index - prev_head_index - 4) ;
	psf->headindex = saved_head_index ;

} /* wav_write_strings */

static int
wav_close (SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	wav_write_tailer (psf) ;

		if ((psf->sf.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV)
			wav_write_header (psf, SF_TRUE) ;
		else
			wavex_write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* wav_close */

static int
wav_command (SF_PRIVATE *psf, int command, void *data, int datasize)
{
	/* Avoid compiler warnings. */
	psf = psf ;
	data = data ;
	datasize = datasize ;

	switch (command)
	{	default : break ;
		} ;

	return 0 ;
} /* wav_command */

static int
wav_subchunk_parse (SF_PRIVATE *psf, int chunk)
{	sf_count_t	current_pos ;
	char		*cptr ;
	int 		dword, bytesread, length ;

	current_pos = psf_fseek (psf, 0, SEEK_CUR) ;

	bytesread = psf_binheader_readf (psf, "e4", &length) ;

	if (current_pos + length > psf->filelength)
	{	psf_log_printf (psf, "%M : %d (should be %d)\n", chunk, length, (int) (psf->filelength - current_pos)) ;
		length = psf->filelength - current_pos ;
		}
	else
		psf_log_printf (psf, "%M : %d\n", chunk, length) ;


	while (bytesread < length)
	{	bytesread += psf_binheader_readf (psf, "m", &chunk) ;

		switch (chunk)
		{	case adtl_MARKER :
			case INFO_MARKER :
					/* These markers don't contain anything. */
					psf_log_printf (psf, "  %M\n", chunk) ;
					break ;

			case data_MARKER:
					psf_log_printf (psf, "  %M inside a LIST block??? Backing out.\n", chunk) ;
					/* Jump back four bytes and return to caller. */
					psf_binheader_readf (psf, "j", -4) ;
					return 0 ;

			case ISFT_MARKER :
			case ICOP_MARKER :
			case IARL_MARKER :
			case IART_MARKER :
			case ICMT_MARKER :
			case ICRD_MARKER :
			case IENG_MARKER :

			case INAM_MARKER :
			case IPRD_MARKER :
			case ISBJ_MARKER :
			case ISRC_MARKER :
					bytesread += psf_binheader_readf (psf, "e4", &dword) ;
					dword += (dword & 1) ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, "  *** %M : %d (too big)\n", chunk, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					bytesread += dword ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, "    %M : %s\n", chunk, cptr) ;
					break ;

			case labl_MARKER :
					{	int mark_id ;

						bytesread += psf_binheader_readf (psf, "e44", &dword, &mark_id) ;
						dword -= 4 ;
						dword += (dword & 1) ;
						if (dword > SIGNED_SIZEOF (psf->buffer))
						{	psf_log_printf (psf, "  *** %M : %d (too big)\n", chunk, dword) ;
							return SFE_INTERNAL ;
							} ;

						cptr = (char*) psf->buffer ;
						psf_binheader_readf (psf, "b", cptr, dword) ;
						bytesread += dword ;
						cptr [dword - 1] = 0 ;
						psf_log_printf (psf, "    %M : %d : %s\n", chunk, mark_id, cptr) ;
						} ;
					break ;


			case DISP_MARKER :
			case ltxt_MARKER :
			case note_MARKER :
					bytesread += psf_binheader_readf (psf, "e4", &dword) ;
					dword += (dword & 1) ;
					psf_binheader_readf (psf, "j", dword) ;
					bytesread += dword ;
					psf_log_printf (psf, "    %M : %d\n", chunk, dword) ;
					break ;

			default :
					psf_binheader_readf (psf, "e4", &dword) ;
					bytesread += sizeof (dword) ;
					dword += (dword & 1) ;
					psf_binheader_readf (psf, "j", dword) ;
					bytesread += dword ;
					psf_log_printf (psf, "    *** %M : %d\n", chunk, dword) ;
					if (dword > length)
						return 0 ;
					break ;
			} ;

		switch (chunk)
		{	case ISFT_MARKER :
					psf_store_string (psf, SF_STR_SOFTWARE, (char*) psf->buffer) ;
					break ;
			case ICOP_MARKER :
					psf_store_string (psf, SF_STR_COPYRIGHT, (char*) psf->buffer) ;
					break ;
			case INAM_MARKER :
					psf_store_string (psf, SF_STR_TITLE, (char*) psf->buffer) ;
					break ;
			case IART_MARKER :
					psf_store_string (psf, SF_STR_ARTIST, (char*) psf->buffer) ;
					break ;
			case ICMT_MARKER :
					psf_store_string (psf, SF_STR_COMMENT, (char*) psf->buffer) ;
					break ;
			case ICRD_MARKER :
					psf_store_string (psf, SF_STR_DATE, (char*) psf->buffer) ;
					break ;
			} ;

		if (psf->logindex >= SIGNED_SIZEOF (psf->logbuffer) - 2)
			return SFE_LOG_OVERRUN ;
		} ;

	current_pos = psf_fseek (psf, 0, SEEK_CUR) - current_pos ;

	if (current_pos - 4 != length)
		psf_log_printf (psf, "**** Bad chunk length %d sbould be %D\n", length, current_pos - 4) ;

	return 0 ;
} /* wav_subchunk_parse */

static int
wav_read_smpl_chunk (SF_PRIVATE *psf, unsigned int chunklen)
{	unsigned int bytesread = 0, dword, sampler_data, loop_count ;
	int k ;

	chunklen += (chunklen & 1) ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	psf_log_printf (psf, "  Manufacturer : %X\n", dword) ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	psf_log_printf (psf, "  Product      : %u\n", dword) ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	psf_log_printf (psf, "  Period       : %u nsec\n", dword) ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	psf_log_printf (psf, "  Midi Note    : %u\n", dword) ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	if (dword != 0)
	{	LSF_SNPRINTF ((char*) psf->buffer, sizeof (psf->buffer), "%f",
				 (1.0 * 0x80000000) / ((unsigned int) dword)) ;
		psf_log_printf (psf, "  Pitch Fract. : %s\n", (char*) psf->buffer) ;
		}
	else
		psf_log_printf (psf, "  Pitch Fract. : 0\n") ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	psf_log_printf (psf, "  SMPTE Format : %u\n", dword) ;

	bytesread += psf_binheader_readf (psf, "e4", &dword) ;
	LSF_SNPRINTF ((char*) psf->buffer, sizeof (psf->buffer), "%02d:%02d:%02d %02d",
		 (dword >> 24) & 0x7F, (dword >> 16) & 0x7F, (dword >> 8) & 0x7F, dword & 0x7F) ;
	psf_log_printf (psf, "  SMPTE Offset : %s\n", psf->buffer) ;

	bytesread += psf_binheader_readf (psf, "e4", &loop_count) ;
	psf_log_printf (psf, "  Loop Count   : %u\n", loop_count) ;

	/* Sampler Data holds the number of data bytes after the CUE chunks which
	** is not actually CUE data. Display value after CUE data.
	*/
	bytesread += psf_binheader_readf (psf, "e4", &sampler_data) ;

	while (loop_count > 0 && chunklen - bytesread >= 24)
	{
		bytesread += psf_binheader_readf (psf, "e4", &dword) ;
		psf_log_printf (psf, "    Cue ID : %2u", dword) ;

		bytesread += psf_binheader_readf (psf, "e4", &dword) ;
		psf_log_printf (psf, "  Type : %2u", dword) ;

		bytesread += psf_binheader_readf (psf, "e4", &dword) ;
		psf_log_printf (psf, "  Start : %5u", dword) ;

		bytesread += psf_binheader_readf (psf, "e4", &dword) ;
		psf_log_printf (psf, "  End : %5u", dword) ;

		bytesread += psf_binheader_readf (psf, "e4", &dword) ;
		psf_log_printf (psf, "  Fraction : %5u", dword) ;

		bytesread += psf_binheader_readf (psf, "e4", &dword) ;
		psf_log_printf (psf, "  Count : %5u\n", dword) ;

		loop_count -- ;
		} ;

	if (chunklen - bytesread == 0)
	{	if (sampler_data != 0)
			psf_log_printf (psf, "  Sampler Data : %u (should be 0)\n", sampler_data) ;
		else
			psf_log_printf (psf, "  Sampler Data : %u\n", sampler_data) ;
		}
	else
	{	if (sampler_data != chunklen - bytesread)
		{	psf_log_printf (psf, "  Sampler Data : %u (should have been %u)\n", sampler_data, chunklen - bytesread) ;
			sampler_data = chunklen - bytesread ;
			}
		else
			psf_log_printf (psf, "  Sampler Data : %u\n", sampler_data) ;

		psf_log_printf (psf, "      ") ;
		for (k = 0 ; k < (int) sampler_data ; k++)
		{	char ch ;

			if (k > 0 && (k % 20) == 0)
				psf_log_printf (psf, "\n      ") ;

			bytesread += psf_binheader_readf (psf, "1", &ch) ;
			psf_log_printf (psf, "%02X ", ch & 0xFF) ;
			} ;

		psf_log_printf (psf, "\n") ;
		} ;

	return 0 ;
} /* wav_read_smpl_chunk */

/*
** The acid chunk goes a little something like this:
**
** 4 bytes          'acid'
** 4 bytes (int)     length of chunk
** 4 bytes (int)     ???
** 2 bytes (short)      root note
** 2 bytes (short)      ???
** 4 bytes (float)      ???
** 4 bytes (int)     number of beats
** 2 bytes (short)      meter denominator
** 2 bytes (short)      meter numerator
** 4 bytes (float)      tempo
**
*/

static int
wav_read_acid_chunk (SF_PRIVATE *psf, unsigned int chunklen)
{	unsigned int bytesread = 0 ;
	int	beats ;
	short rootnote, q1, meter_denom, meter_numer ;
	float q2, q3, tempo ;

	chunklen += (chunklen & 1) ;

	bytesread += psf_binheader_readf (psf, "e224f", &rootnote, &q1, &q2, &q3) ;
	LSF_SNPRINTF ((char*) psf->buffer, sizeof (psf->buffer), "%f", q2) ;
	psf_log_printf (psf, "  Root note : %d\n  ????      : %d\n  ????      : %s\n  ????      : %d\n",
				rootnote, q1, psf->buffer, q3) ;

	bytesread += psf_binheader_readf (psf, "e422f", &beats, &meter_denom, &meter_numer, &tempo) ;
	LSF_SNPRINTF ((char*) psf->buffer, sizeof (psf->buffer), "%f", tempo) ;
	psf_log_printf (psf, "  Beats     : %d\n  Meter     : %d/%d\n  Tempo     : %s\n",
				beats, meter_denom, meter_numer, psf->buffer) ;

	psf_binheader_readf (psf, "j", chunklen - bytesread) ;

	return 0 ;
} /* wav_read_acid_chunk */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 9c551689-a1d8-4905-9f56-26a204374f18
*/
/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<time.h>


/*------------------------------------------------------------------------------
 * Private static functions.
 */

int
wav_w64_read_fmt_chunk (SF_PRIVATE *psf, WAV_FMT *wav_fmt, int structsize)
{	int	bytesread, k, bytespersec = 0 ;

	memset (wav_fmt, 0, sizeof (WAV_FMT)) ;

	if (structsize < 16)
		return SFE_WAV_FMT_SHORT ;
	if (structsize > SIGNED_SIZEOF (WAV_FMT))
		return SFE_WAV_FMT_TOO_BIG ;

	/* Read the minimal WAV file header here. */
	bytesread =
	psf_binheader_readf (psf, "e224422", &(wav_fmt->format), &(wav_fmt->min.channels),
			&(wav_fmt->min.samplerate), &(wav_fmt->min.bytespersec),
			&(wav_fmt->min.blockalign), &(wav_fmt->min.bitwidth)) ;

	psf_log_printf (psf, "  Format        : 0x%X => %s\n", wav_fmt->format, wav_w64_format_str (wav_fmt->format)) ;
	psf_log_printf (psf, "  Channels      : %d\n", wav_fmt->min.channels) ;
	psf_log_printf (psf, "  Sample Rate   : %d\n", wav_fmt->min.samplerate) ;
	psf_log_printf (psf, "  Block Align   : %d\n", wav_fmt->min.blockalign) ;

	if (wav_fmt->format == WAVE_FORMAT_PCM && wav_fmt->min.bitwidth == 24 &&
			wav_fmt->min.blockalign == 4 * wav_fmt->min.channels)
	{
		psf_log_printf (psf, "\nInvalid file generated by Syntrillium's Cooledit!\n"
				"Treating as WAVE_FORMAT_IEEE_FLOAT 32 bit floating point file.\n\n") ;
		psf_log_printf (psf, "  Bit Width     : 24 (should be 32)\n") ;
		wav_fmt->min.bitwidth = 32 ;
		wav_fmt->format = WAVE_FORMAT_IEEE_FLOAT ;
		}
	else if (wav_fmt->format == WAVE_FORMAT_GSM610 && wav_fmt->min.bitwidth != 0)
		psf_log_printf (psf, "  Bit Width     : %d (should be 0)\n", wav_fmt->min.bitwidth) ;
	else
		psf_log_printf (psf, "  Bit Width     : %d\n", wav_fmt->min.bitwidth) ;


	psf->sf.samplerate	= wav_fmt->min.samplerate ;
	psf->sf.frames 		= 0 ;					/* Correct this when reading data chunk. */
	psf->sf.channels	= wav_fmt->min.channels ;

	switch (wav_fmt->format)
	{	case WAVE_FORMAT_PCM :
		case WAVE_FORMAT_IEEE_FLOAT :
				bytespersec = wav_fmt->min.samplerate * wav_fmt->min.blockalign ;
				if (wav_fmt->min.bytespersec != (unsigned) bytespersec)
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d)\n", wav_fmt->min.bytespersec, bytespersec) ;
				else
					psf_log_printf (psf, "  Bytes/sec     : %d\n", wav_fmt->min.bytespersec) ;

				psf->bytewidth = BITWIDTH2BYTES (wav_fmt->min.bitwidth) ;
				break ;

		case WAVE_FORMAT_ALAW :
		case WAVE_FORMAT_MULAW :
				if (wav_fmt->min.bytespersec / wav_fmt->min.blockalign != wav_fmt->min.samplerate)
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d)\n", wav_fmt->min.bytespersec, wav_fmt->min.samplerate * wav_fmt->min.blockalign) ;
				else
					psf_log_printf (psf, "  Bytes/sec     : %d\n", wav_fmt->min.bytespersec) ;

				psf->bytewidth = 1 ;
				if (structsize >= 18)
				{	bytesread += psf_binheader_readf (psf, "e2", &(wav_fmt->size20.extrabytes)) ;
					psf_log_printf (psf, "  Extra Bytes   : %d\n", wav_fmt->size20.extrabytes) ;
					} ;
				break ;

		case WAVE_FORMAT_IMA_ADPCM :
				if (wav_fmt->min.bitwidth != 4)
					return SFE_WAV_ADPCM_NOT4BIT ;
				if (wav_fmt->min.channels < 1 || wav_fmt->min.channels > 2)
					return SFE_WAV_ADPCM_CHANNELS ;

				bytesread +=
				psf_binheader_readf (psf, "e22", &(wav_fmt->ima.extrabytes), &(wav_fmt->ima.samplesperblock)) ;

				bytespersec = (wav_fmt->ima.samplerate * wav_fmt->ima.blockalign) / wav_fmt->ima.samplesperblock ;
				if (wav_fmt->ima.bytespersec != (unsigned) bytespersec)
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d)\n", wav_fmt->ima.bytespersec, bytespersec) ;
				else
					psf_log_printf (psf, "  Bytes/sec     : %d\n", wav_fmt->ima.bytespersec) ;

				psf->bytewidth = 2 ;
				psf_log_printf (psf, "  Extra Bytes   : %d\n", wav_fmt->ima.extrabytes) ;
				psf_log_printf (psf, "  Samples/Block : %d\n", wav_fmt->ima.samplesperblock) ;
				break ;

		case WAVE_FORMAT_MS_ADPCM :
				if (wav_fmt->msadpcm.bitwidth != 4)
					return SFE_WAV_ADPCM_NOT4BIT ;
				if (wav_fmt->msadpcm.channels < 1 || wav_fmt->msadpcm.channels > 2)
					return SFE_WAV_ADPCM_CHANNELS ;

				bytesread +=
				psf_binheader_readf (psf, "e222", &(wav_fmt->msadpcm.extrabytes),
						&(wav_fmt->msadpcm.samplesperblock), &(wav_fmt->msadpcm.numcoeffs)) ;

				bytespersec = (wav_fmt->min.samplerate * wav_fmt->min.blockalign) / wav_fmt->msadpcm.samplesperblock ;
				if (wav_fmt->min.bytespersec == (unsigned) bytespersec)
					psf_log_printf (psf, "  Bytes/sec     : %d\n", wav_fmt->min.bytespersec) ;
				else if (wav_fmt->min.bytespersec == (wav_fmt->min.samplerate / wav_fmt->msadpcm.samplesperblock) * wav_fmt->min.blockalign)
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d (MS BUG!))\n", wav_fmt->min.bytespersec, bytespersec) ;
				else
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d)\n", wav_fmt->min.bytespersec, bytespersec) ;


				psf->bytewidth = 2 ;
				psf_log_printf (psf, "  Extra Bytes   : %d\n", wav_fmt->msadpcm.extrabytes) ;
				psf_log_printf (psf, "  Samples/Block : %d\n", wav_fmt->msadpcm.samplesperblock) ;
				if (wav_fmt->msadpcm.numcoeffs > SIGNED_SIZEOF (MS_ADPCM_WAV_FMT) / SIGNED_SIZEOF (int))
				{	psf_log_printf (psf, "  No. of Coeffs : %d ****\n", wav_fmt->msadpcm.numcoeffs) ;
					wav_fmt->msadpcm.numcoeffs = SIGNED_SIZEOF (MS_ADPCM_WAV_FMT) / SIGNED_SIZEOF (int) ;
					}
				else
					psf_log_printf (psf, "  No. of Coeffs : %d\n", wav_fmt->msadpcm.numcoeffs) ;

				psf_log_printf (psf, "    Index   Coeffs1   Coeffs2\n") ;
				for (k = 0 ; k < wav_fmt->msadpcm.numcoeffs ; k++)
				{	bytesread +=
					psf_binheader_readf (psf, "e22", &(wav_fmt->msadpcm.coeffs [k].coeff1), &(wav_fmt->msadpcm.coeffs [k].coeff2)) ;
					LSF_SNPRINTF ((char*) psf->buffer, sizeof (psf->buffer), "     %2d     %7d   %7d\n", k, wav_fmt->msadpcm.coeffs [k].coeff1, wav_fmt->msadpcm.coeffs [k].coeff2) ;
					psf_log_printf (psf, (char*) psf->buffer) ;
					} ;
				break ;

		case WAVE_FORMAT_GSM610 :
				if (wav_fmt->gsm610.channels != 1 || wav_fmt->gsm610.blockalign != 65)
					return SFE_WAV_GSM610_FORMAT ;

				bytesread +=
				psf_binheader_readf (psf, "e22", &(wav_fmt->gsm610.extrabytes), &(wav_fmt->gsm610.samplesperblock)) ;

				if (wav_fmt->gsm610.samplesperblock != 320)
					return SFE_WAV_GSM610_FORMAT ;

				bytespersec = (wav_fmt->gsm610.samplerate * wav_fmt->gsm610.blockalign) / wav_fmt->gsm610.samplesperblock ;
				if (wav_fmt->gsm610.bytespersec != (unsigned) bytespersec)
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d)\n", wav_fmt->gsm610.bytespersec, bytespersec) ;
				else
					psf_log_printf (psf, "  Bytes/sec     : %d\n", wav_fmt->gsm610.bytespersec) ;

				psf->bytewidth = 2 ;
				psf_log_printf (psf, "  Extra Bytes   : %d\n", wav_fmt->gsm610.extrabytes) ;
				psf_log_printf (psf, "  Samples/Block : %d\n", wav_fmt->gsm610.samplesperblock) ;
				break ;

		case WAVE_FORMAT_EXTENSIBLE :
				if (wav_fmt->ext.bytespersec / wav_fmt->ext.blockalign != wav_fmt->ext.samplerate)
					psf_log_printf (psf, "  Bytes/sec     : %d (should be %d)\n", wav_fmt->ext.bytespersec, wav_fmt->ext.samplerate * wav_fmt->ext.blockalign) ;
				else
					psf_log_printf (psf, "  Bytes/sec     : %d\n", wav_fmt->ext.bytespersec) ;

				bytesread +=
				psf_binheader_readf (psf, "e224", &(wav_fmt->ext.extrabytes), &(wav_fmt->ext.validbits),
						&(wav_fmt->ext.channelmask)) ;

				psf_log_printf (psf, "  Valid Bits    : %d\n", wav_fmt->ext.validbits) ;
				psf_log_printf (psf, "  Channel Mask  : 0x%X\n", wav_fmt->ext.channelmask) ;

				bytesread +=
				psf_binheader_readf (psf, "e422", &(wav_fmt->ext.esf.esf_field1), &(wav_fmt->ext.esf.esf_field2),
						&(wav_fmt->ext.esf.esf_field3)) ;

                /* compare the esf_fields with each known GUID? and print?*/
				psf_log_printf (psf, "  Subformat\n") ;
				psf_log_printf (psf, "    esf_field1 : 0x%X\n", wav_fmt->ext.esf.esf_field1) ;
				psf_log_printf (psf, "    esf_field2 : 0x%X\n", wav_fmt->ext.esf.esf_field2) ;
				psf_log_printf (psf, "    esf_field3 : 0x%X\n", wav_fmt->ext.esf.esf_field3) ;
				psf_log_printf (psf, "    esf_field4 : ") ;
				for (k = 0 ; k < 8 ; k++)
				{	bytesread += psf_binheader_readf (psf, "1", &(wav_fmt->ext.esf.esf_field4 [k])) ;
					psf_log_printf (psf, "0x%X ", wav_fmt->ext.esf.esf_field4 [k] & 0xFF) ;
					} ;
				psf_log_printf (psf, "\n") ;
				psf->bytewidth = BITWIDTH2BYTES (wav_fmt->ext.bitwidth) ;
				break ;

		default : break ;
		} ;

	if (bytesread > structsize)
	{	psf_log_printf (psf, "*** wav_w64_read_fmt_chunk (bytesread > structsize)\n") ;
		return SFE_W64_FMT_SHORT ;
		}
	else
		psf_binheader_readf (psf, "j", structsize - bytesread) ;

	psf->blockwidth = wav_fmt->min.channels * psf->bytewidth ;

	return 0 ;
} /* wav_w64_read_fmt_chunk */

/*==============================================================================
*/

typedef struct
{	int			ID ;
	const char	*name ;
} WAV_FORMAT_DESC ;

#define STR(x)			#x
#define FORMAT_TYPE(x)	{ x, STR (x) }

static WAV_FORMAT_DESC wave_descs [] =
{	FORMAT_TYPE	(WAVE_FORMAT_PCM),
	FORMAT_TYPE (WAVE_FORMAT_MS_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_IEEE_FLOAT),
	FORMAT_TYPE (WAVE_FORMAT_VSELP),
	FORMAT_TYPE (WAVE_FORMAT_IBM_CVSD),
	FORMAT_TYPE (WAVE_FORMAT_ALAW),
	FORMAT_TYPE (WAVE_FORMAT_MULAW),
	FORMAT_TYPE (WAVE_FORMAT_OKI_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_IMA_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_MEDIASPACE_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_SIERRA_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_G723_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_DIGISTD),
	FORMAT_TYPE (WAVE_FORMAT_DIGIFIX),
	FORMAT_TYPE (WAVE_FORMAT_DIALOGIC_OKI_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_MEDIAVISION_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_CU_CODEC),
	FORMAT_TYPE (WAVE_FORMAT_YAMAHA_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_SONARC),
	FORMAT_TYPE (WAVE_FORMAT_DSPGROUP_TRUESPEECH),
	FORMAT_TYPE (WAVE_FORMAT_ECHOSC1),
	FORMAT_TYPE (WAVE_FORMAT_AUDIOFILE_AF36),
	FORMAT_TYPE (WAVE_FORMAT_APTX),
	FORMAT_TYPE (WAVE_FORMAT_AUDIOFILE_AF10),
	FORMAT_TYPE (WAVE_FORMAT_PROSODY_1612),
	FORMAT_TYPE (WAVE_FORMAT_LRC),
	FORMAT_TYPE (WAVE_FORMAT_DOLBY_AC2),
	FORMAT_TYPE (WAVE_FORMAT_GSM610),
	FORMAT_TYPE (WAVE_FORMAT_MSNAUDIO),
	FORMAT_TYPE (WAVE_FORMAT_ANTEX_ADPCME),
	FORMAT_TYPE (WAVE_FORMAT_CONTROL_RES_VQLPC),
	FORMAT_TYPE (WAVE_FORMAT_DIGIREAL),
	FORMAT_TYPE (WAVE_FORMAT_DIGIADPCM),
	FORMAT_TYPE (WAVE_FORMAT_CONTROL_RES_CR10),
	FORMAT_TYPE (WAVE_FORMAT_NMS_VBXADPCM),
	FORMAT_TYPE (WAVE_FORMAT_ROLAND_RDAC),
	FORMAT_TYPE (WAVE_FORMAT_ECHOSC3),
	FORMAT_TYPE (WAVE_FORMAT_ROCKWELL_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_ROCKWELL_DIGITALK),
	FORMAT_TYPE (WAVE_FORMAT_XEBEC),
	FORMAT_TYPE (WAVE_FORMAT_G721_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_G728_CELP),
	FORMAT_TYPE (WAVE_FORMAT_MSG723),
	FORMAT_TYPE (WAVE_FORMAT_MPEG),
	FORMAT_TYPE (WAVE_FORMAT_RT24),
	FORMAT_TYPE (WAVE_FORMAT_PAC),
	FORMAT_TYPE (WAVE_FORMAT_MPEGLAYER3),
	FORMAT_TYPE (WAVE_FORMAT_LUCENT_G723),
	FORMAT_TYPE (WAVE_FORMAT_CIRRUS),
	FORMAT_TYPE (WAVE_FORMAT_ESPCM),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE),
	FORMAT_TYPE (WAVE_FORMAT_CANOPUS_ATRAC),
	FORMAT_TYPE (WAVE_FORMAT_G726_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_G722_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_DSAT),
	FORMAT_TYPE (WAVE_FORMAT_DSAT_DISPLAY),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_BYTE_ALIGNED),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_AC8),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_AC10),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_AC16),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_AC20),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_RT24),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_RT29),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_RT29HW),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_VR12),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_VR18),
	FORMAT_TYPE (WAVE_FORMAT_VOXWARE_TQ40),
	FORMAT_TYPE (WAVE_FORMAT_SOFTSOUND),
	FORMAT_TYPE (WAVE_FORMAT_VOXARE_TQ60),
	FORMAT_TYPE (WAVE_FORMAT_MSRT24),
	FORMAT_TYPE (WAVE_FORMAT_G729A),
	FORMAT_TYPE (WAVE_FORMAT_MVI_MV12),
	FORMAT_TYPE (WAVE_FORMAT_DF_G726),
	FORMAT_TYPE (WAVE_FORMAT_DF_GSM610),
	FORMAT_TYPE (WAVE_FORMAT_ONLIVE),
	FORMAT_TYPE (WAVE_FORMAT_SBC24),
	FORMAT_TYPE (WAVE_FORMAT_DOLBY_AC3_SPDIF),
	FORMAT_TYPE (WAVE_FORMAT_ZYXEL_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_PHILIPS_LPCBB),
	FORMAT_TYPE (WAVE_FORMAT_PACKED),
	FORMAT_TYPE (WAVE_FORMAT_RHETOREX_ADPCM),
	FORMAT_TYPE (IBM_FORMAT_MULAW),
	FORMAT_TYPE (IBM_FORMAT_ALAW),
	FORMAT_TYPE (IBM_FORMAT_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_VIVO_G723),
	FORMAT_TYPE (WAVE_FORMAT_VIVO_SIREN),
	FORMAT_TYPE (WAVE_FORMAT_DIGITAL_G723),
	FORMAT_TYPE (WAVE_FORMAT_CREATIVE_ADPCM),
	FORMAT_TYPE (WAVE_FORMAT_CREATIVE_FASTSPEECH8),
	FORMAT_TYPE (WAVE_FORMAT_CREATIVE_FASTSPEECH10),
	FORMAT_TYPE (WAVE_FORMAT_QUARTERDECK),
	FORMAT_TYPE (WAVE_FORMAT_FM_TOWNS_SND),
	FORMAT_TYPE (WAVE_FORMAT_BZV_DIGITAL),
	FORMAT_TYPE (WAVE_FORMAT_VME_VMPCM),
	FORMAT_TYPE (WAVE_FORMAT_OLIGSM),
	FORMAT_TYPE (WAVE_FORMAT_OLIADPCM),
	FORMAT_TYPE (WAVE_FORMAT_OLICELP),
	FORMAT_TYPE (WAVE_FORMAT_OLISBC),
	FORMAT_TYPE (WAVE_FORMAT_OLIOPR),
	FORMAT_TYPE (WAVE_FORMAT_LH_CODEC),
	FORMAT_TYPE (WAVE_FORMAT_NORRIS),
	FORMAT_TYPE (WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS),
	FORMAT_TYPE (WAVE_FORMAT_DVM),
	FORMAT_TYPE (WAVE_FORMAT_INTERWAV_VSC112),
	FORMAT_TYPE (WAVE_FORMAT_EXTENSIBLE),
} ;

char const*
wav_w64_format_str (int k)
{	int lower, upper, mid ;

	lower = -1 ;
	upper = sizeof (wave_descs) / sizeof (WAV_FORMAT_DESC) ;

	/* binary search */
	if ((wave_descs [0].ID <= k) & (k <= wave_descs [upper - 1].ID))
	{
		while (lower + 1 < upper)
		{	mid = (upper + lower) / 2 ;

			if (k == wave_descs [mid].ID)
				return wave_descs [mid].name ;
			if (k < wave_descs [mid].ID)
				upper = mid ;
			else
				lower = mid ;
			} ;
		} ;

	return "Unknown format" ;
} /* wav_w64_format_str */

int
wav_w64_srate2blocksize (int srate_chan_product)
{	if (srate_chan_product < 12000)
		return 256 ;
	if (srate_chan_product < 23000)
		return 512 ;
	if (srate_chan_product < 44000)
		return 1024 ;
	return 2048 ;
} /* srate2blocksize */
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 43c1b1dd-8abd-43da-a8cd-44da914b64a5
*/
/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>


#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
wve_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* wve_open */

#else

#define	SFE_WVE_NOT_WVE	666

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define ALAW_MARKER		MAKE_MARKER ('A', 'L', 'a', 'w')
#define SOUN_MARKER		MAKE_MARKER ('S', 'o', 'u', 'n')
#define DFIL_MARKER		MAKE_MARKER ('d', 'F', 'i', 'l')

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int	wve_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
wve_open (SF_PRIVATE *psf)
{	int	subformat, error = 0 ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		return SFE_UNIMPLEMENTED ;

	if ((error = wve_read_header (psf)))
		return error ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_WVE)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	return error ;
} /* wve_open */

/*------------------------------------------------------------------------------
*/

static int
wve_read_header (SF_PRIVATE *psf)
{	int marker ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "pm", 0, &marker) ;
	if (marker != ALAW_MARKER)
		return SFE_WVE_NOT_WVE ;

	psf_binheader_readf (psf, "m", &marker) ;
	if (marker != SOUN_MARKER)
		return SFE_WVE_NOT_WVE ;

	psf_binheader_readf (psf, "m", &marker) ;
	if (marker != DFIL_MARKER)
		return SFE_WVE_NOT_WVE ;

	psf_log_printf (psf, "Read only : Psion Palmtop Alaw (.wve)\n"
			"  Sample Rate : 8000\n"
			"  Channels    : 1\n"
			"  Encoding    : A-law\n") ;

	psf->dataoffset = 0x20 ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	psf->sf.format		= SF_FORMAT_WVE | SF_FORMAT_ALAW ;
	psf->sf.samplerate	= 8000 ;
	psf->sf.frames		= psf->datalength ;
	psf->sf.channels	= 1 ;

	return alaw_init (psf) ;
} /* wve_read_header */

/*------------------------------------------------------------------------------
*/

#endif
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: ba368cb5-523f-45e4-98c1-5b99a102f73f
*/
/*
** Copyright (C) 2003,2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>


#define	MAX_XI_SAMPLES	16

/*------------------------------------------------------------------------------
** Private static functions and tyepdefs.
*/

typedef struct
{	/* Warning, this filename is NOT nul terminated. */
	char	filename [22] ;
	char	software [20] ;
	char	sample_name [22] ;

	int		loop_begin, loop_end ;
	int		sample_flags ;

	/* Data for encoder and decoder. */
	short	last_16 ;
} XI_PRIVATE ;

static int	xi_close		(SF_PRIVATE *psf) ;
static int	xi_write_header (SF_PRIVATE *psf, int calc_length) ;
static int	xi_read_header	(SF_PRIVATE *psf) ;
static int	dpcm_init 		(SF_PRIVATE *psf) ;


static sf_count_t	dpcm_seek (SF_PRIVATE *psf, int mode, sf_count_t offset) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
xi_open	(SF_PRIVATE *psf)
{	XI_PRIVATE *pxi ;
	int		subformat, error = 0 ;

	if (psf->is_pipe)
		return SFE_XI_NO_PIPE ;

	if (psf->fdata)
		pxi = psf->fdata ;
	else if ((pxi = calloc (1, sizeof (XI_PRIVATE))) == NULL)
		return SFE_MALLOC_FAILED ;

	psf->fdata = pxi ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = xi_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_XI)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = SF_ENDIAN_LITTLE ;
		psf->sf.channels = 1 ; /* Always mono */
		psf->sf.samplerate = 44100 ; /* Always */

		/* Set up default instrument and software name. */
		memcpy (pxi->filename, "Default Name            ", sizeof (pxi->filename)) ;
		memcpy (pxi->software, PACKAGE "-" VERSION "               ", sizeof (pxi->software)) ;

		memset (pxi->sample_name, 0, sizeof (pxi->sample_name)) ;
		LSF_SNPRINTF (pxi->sample_name, sizeof (pxi->sample_name), "%s", "Sample #1") ;

		pxi->sample_flags = (subformat == SF_FORMAT_DPCM_16) ? 16 : 0 ;

		if (xi_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = xi_write_header ;
		} ;

	psf->close = xi_close ;
	psf->seek = dpcm_seek ;

	psf->sf.seekable = SF_FALSE ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case SF_FORMAT_DPCM_8 :		/* 8-bit differential PCM. */
		case SF_FORMAT_DPCM_16 :	/* 16-bit differential PCM. */
				error = dpcm_init (psf) ;
				break ;

		default : break ;
		} ;

	return error ;
} /* xi_open */

/*------------------------------------------------------------------------------
*/

static int
xi_close	(SF_PRIVATE *psf)
{
	psf = psf ;

	return 0 ;
} /* xi_close */

/*==============================================================================
*/

static sf_count_t dpcm_read_dsc2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dpcm_read_dsc2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dpcm_read_dsc2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dpcm_read_dsc2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t dpcm_write_s2dsc (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dpcm_write_i2dsc (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dpcm_write_f2dsc (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dpcm_write_d2dsc (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t dpcm_read_dles2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dpcm_read_dles2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dpcm_read_dles2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dpcm_read_dles2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t dpcm_write_s2dles (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t dpcm_write_i2dles (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t dpcm_write_f2dles (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t dpcm_write_d2dles (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static int
dpcm_init (SF_PRIVATE *psf)
{	if (psf->bytewidth == 0 || psf->sf.channels == 0)
		return SFE_INTERNAL ;

	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	switch (psf->bytewidth)
		{	case 1 :
					psf->read_short		= dpcm_read_dsc2s ;
					psf->read_int		= dpcm_read_dsc2i ;
					psf->read_float		= dpcm_read_dsc2f ;
					psf->read_double	= dpcm_read_dsc2d ;
					break ;
			case 2 :
					psf->read_short		= dpcm_read_dles2s ;
					psf->read_int		= dpcm_read_dles2i ;
					psf->read_float		= dpcm_read_dles2f ;
					psf->read_double	= dpcm_read_dles2d ;
					break ;
			default :
				psf_log_printf (psf, "dpcm_init() returning SFE_UNIMPLEMENTED\n") ;
				return SFE_UNIMPLEMENTED ;
			} ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	switch (psf->bytewidth)
		{	case 1 :
					psf->write_short	= dpcm_write_s2dsc ;
					psf->write_int		= dpcm_write_i2dsc ;
					psf->write_float	= dpcm_write_f2dsc ;
					psf->write_double	= dpcm_write_d2dsc ;
					break ;
			case 2 :
					psf->write_short	= dpcm_write_s2dles ;
					psf->write_int		= dpcm_write_i2dles ;
					psf->write_float	= dpcm_write_f2dles ;
					psf->write_double	= dpcm_write_d2dles ;
					break ;
			default :
				psf_log_printf (psf, "dpcm_init() returning SFE_UNIMPLEMENTED\n") ;
				return SFE_UNIMPLEMENTED ;
			} ;
		} ;

	psf->filelength = psf_get_filelen (psf) ;
	psf->datalength = (psf->dataend) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
	psf->sf.frames = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* dpcm_init */

/*==============================================================================
*/

static sf_count_t
dpcm_seek (SF_PRIVATE *psf, int mode, sf_count_t offset)
{	XI_PRIVATE	*pxi ;
	int			total, bufferlen, len ;

	if ((pxi = psf->fdata) == NULL)
		return SFE_INTERNAL ;

	if (psf->datalength < 0 || psf->dataoffset < 0)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	if (offset == 0)
	{	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		pxi->last_16 = 0 ;
		return 0 ;
		} ;

	if (offset < 0 || offset > psf->sf.frames)
	{	psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	if (mode != SFM_READ)
	{	/* What to do about write??? */
		psf->error = SFE_BAD_SEEK ;
		return	((sf_count_t) -1) ;
		} ;

	psf_fseek (psf, psf->dataoffset, SEEK_SET) ;

	if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_DPCM_16)
	{	total = offset ;
		bufferlen = sizeof (psf->buffer) / sizeof (short) ;
		while (total > 0)
		{	len = (total > bufferlen) ? bufferlen : total ;
			total -= dpcm_read_dles2s (psf, (short *) psf->buffer, len) ;
			} ;
		}
	else
	{	total = offset ;
		bufferlen = sizeof (psf->buffer) / sizeof (short) ;
		while (total > 0)
		{	len = (total > bufferlen) ? bufferlen : total ;
			total -= dpcm_read_dsc2s (psf, (short *) psf->buffer, len) ;
			} ;
		} ;

	return offset ;
} /* dpcm_seek */


static int
xi_write_header (SF_PRIVATE *psf, int calc_length)
{	XI_PRIVATE	*pxi ;
	sf_count_t	current ;
	const char	*string ;

	if ((pxi = psf->fdata) == NULL)
		return SFE_INTERNAL ;

	calc_length = calc_length ; /* Avoid a compiler warning. */

	current = psf_ftell (psf) ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	string = "Extended Instrument: " ;
	psf_binheader_writef (psf, "b", string, strlen (string)) ;
	psf_binheader_writef (psf, "b1", pxi->filename, sizeof (pxi->filename), 0x1A) ;

	/* Write software version and two byte XI version. */
	psf_binheader_writef (psf, "eb2", pxi->software, sizeof (pxi->software), (1 << 8) + 2) ;

	/*
	** Jump note numbers (96), volume envelope (48), pan envelope (48),
	** volume points (1), pan points (1)
	*/
	psf_binheader_writef (psf, "z", (size_t) (96 + 48 + 48 + 1 + 1)) ;

	/* Jump volume loop (3 bytes), pan loop (3), envelope flags (3), vibrato (3)
	** fade out (2), 22 unknown bytes, and then write sample_count (2 bytes).
	*/
	psf_binheader_writef (psf, "ez2z2", (size_t) (4 * 3), 0x1234, (size_t) 22, 1) ;

psf->sf.frames = 12 ;
pxi->loop_begin = 0 ;
pxi->loop_end = 0 ;

	psf_binheader_writef (psf, "et844", psf->sf.frames, pxi->loop_begin, pxi->loop_end) ;

	/* volume, fine tune, flags, pan, note, namelen */
	psf_binheader_writef (psf, "111111", 128, 0, pxi->sample_flags, 128, 0, strlen (pxi->sample_name)) ;

	psf_binheader_writef (psf, "b", pxi->sample_name, sizeof (pxi->sample_name)) ;





	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* xi_write_header */

static int
xi_read_header (SF_PRIVATE *psf)
{	char	buffer [64], name [32] ;
	short	version, fade_out, sample_count ;
	int		k, loop_begin, loop_end ;
	int 	sample_sizes [MAX_XI_SAMPLES] ;

	psf_binheader_readf (psf, "pb", 0, buffer, 21) ;

	memset (sample_sizes, 0, sizeof (sample_sizes)) ;

	buffer [20] = 0 ;
	if (strcmp (buffer, "Extended Instrument:") != 0)
		return SFE_XI_BAD_HEADER ;

	memset (buffer, 0, sizeof (buffer)) ;
	psf_binheader_readf (psf, "b", buffer, 23) ;

	if (buffer [22] != 0x1A)
		return SFE_XI_BAD_HEADER ;

	buffer [22] = 0 ;
	psf_log_printf (psf, "Extended Instrument : %s\n", buffer) ;

	psf_binheader_readf (psf, "be2", buffer, 20, &version) ;
	buffer [19] = 0 ;
	psf_log_printf (psf, "Software : %s\nVersion  : %d.%02d\n", buffer, version / 256, version % 256) ;

	/* Jump note numbers (96), volume envelope (48), pan envelope (48),
	** volume points (1), pan points (1)
	*/
	psf_binheader_readf (psf, "j", 96 + 48 + 48 + 1 + 1) ;

	psf_binheader_readf (psf, "b", buffer, 12) ;
	psf_log_printf (psf, "Volume Loop\n  sustain : %u\n  begin   : %u\n  end     : %u\n",
						buffer [0], buffer [1], buffer [2]) ;
	psf_log_printf (psf, "Pan Loop\n  sustain : %u\n  begin   : %u\n  end     : %u\n",
						buffer [3], buffer [4], buffer [5]) ;
	psf_log_printf (psf, "Envelope Flags\n  volume  : 0x%X\n  pan     : 0x%X\n",
				buffer [6] & 0xFF, buffer [7] & 0xFF) ;

	psf_log_printf (psf, "Vibrato\n  type    : %u\n  sweep   : %u\n  depth   : %u\n  rate    : %u\n",
				buffer [8], buffer [9], buffer [10], buffer [11]) ;

	/*
	** Read fade_out then jump reserved (2 bytes) and ???? (20 bytes) and
	** sample_count.
	*/
	psf_binheader_readf (psf, "e2j2", &fade_out, 2 + 20, &sample_count) ;
	psf_log_printf (psf, "Fade out  : %d\n", fade_out) ;

	/* XI file can contain up to 16 samples. */
	if (sample_count > MAX_XI_SAMPLES)
		return SFE_XI_EXCESS_SAMPLES ;

	/* Log all data for each sample. */
	for (k = 0 ; k < sample_count ; k++)
	{	psf_binheader_readf (psf, "e444", &(sample_sizes [k]), &loop_begin, &loop_end) ;

		/* Read 5 know bytes, 1 unknown byte and 22 name bytes. */
		psf_binheader_readf (psf, "bb", buffer, 6, name, 22) ;
		name [21] = 0 ;

		psf_log_printf (psf, "Sample #%d\n  name    : %s\n  size    : %d\n", k + 1, name, sample_sizes [k]) ;
		psf_log_printf (psf, "  loop\n    begin : %d\n    end   : %d\n", loop_begin, loop_end) ;

		psf_log_printf (psf, "  volume  : %u\n  f. tune : %d\n  flags   : 0x%02X ",
					buffer [0] & 0xFF, buffer [1] & 0xFF, buffer [2] & 0xFF) ;

		psf_log_printf (psf, " (") ;
		if (buffer [2] & 1)
			psf_log_printf (psf, " Loop") ;
		if (buffer [2] & 2)
			psf_log_printf (psf, " PingPong") ;
		psf_log_printf (psf, (buffer [2] & 16) ? " 16bit" : " 8bit") ;
		psf_log_printf (psf, " )\n") ;

		psf_log_printf (psf, "  pan     : %u\n  note    : %d\n  namelen : %d\n",
					buffer [3] & 0xFF, buffer [4], buffer [5]) ;

		if (k != 0)
			continue ;

		if (buffer [2] & 16)
		{	psf->sf.format = SF_FORMAT_XI | SF_FORMAT_DPCM_16 ;
			psf->bytewidth = 2 ;
			}
		else
		{	psf->sf.format = SF_FORMAT_XI | SF_FORMAT_DPCM_8 ;
			psf->bytewidth = 1 ;
			} ;
		} ;

	while (sample_count > 1 && sample_sizes [sample_count - 1] == 0)
		sample_count -- ;

	/* Currently, we can only handle 1 sample per file. */

	if (sample_count > 2)
	{	psf_log_printf (psf, "*** Sample count is less than 16 but more than 1.\n") ;
		psf_log_printf (psf, "  sample count : %d    sample_sizes [%d] : %d\n",
						sample_count, sample_count - 1, sample_sizes [sample_count - 1]) ;
		return SFE_XI_EXCESS_SAMPLES ;
		} ;

	psf->dataoffset = psf_fseek (psf, 0, SEEK_CUR) ;
	psf_log_printf (psf, "Data Offset : %D\n", psf->dataoffset) ;

	psf->datalength = sample_sizes [0] ;

	if (psf->dataoffset + psf->datalength > psf->filelength)
	{	psf_log_printf (psf, "*** File seems to be truncated. Should be at least %D bytes long.\n",
				psf->dataoffset + sample_sizes [0]) ;
		psf->datalength = psf->filelength - psf->dataoffset ;
		} ;

 	if (psf_fseek (psf, psf->dataoffset, SEEK_SET) != psf->dataoffset)
		return SFE_BAD_SEEK ;

	psf->close = xi_close ;

	psf->endian = SF_ENDIAN_LITTLE ;
	psf->sf.channels = 1 ; /* Always mono */
	psf->sf.samplerate = 44100 ; /* Always */

	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	if (! psf->sf.frames && psf->blockwidth)
		psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;

	return 0 ;
} /* xi_read_header */

/*==============================================================================
*/

static void dsc2s_array (XI_PRIVATE *pxi, signed char *src, int count, short *dest) ;
static void dsc2i_array (XI_PRIVATE *pxi, signed char *src, int count, int *dest) ;
static void dsc2f_array (XI_PRIVATE *pxi, signed char *src, int count, float *dest, float normfact) ;
static void dsc2d_array (XI_PRIVATE *pxi, signed char *src, int count, double *dest, double normfact) ;

static void dles2s_array (XI_PRIVATE *pxi, short *src, int count, short *dest) ;
static void dles2i_array (XI_PRIVATE *pxi, short *src, int count, int *dest) ;
static void dles2f_array (XI_PRIVATE *pxi, short *src, int count, float *dest, float normfact) ;
static void dles2d_array (XI_PRIVATE *pxi, short *src, int count, double *dest, double normfact) ;

static sf_count_t
dpcm_read_dsc2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		dsc2s_array (pxi, (signed char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dsc2s */

static sf_count_t
dpcm_read_dsc2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		dsc2i_array (pxi, (signed char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dsc2i */

static sf_count_t
dpcm_read_dsc2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		dsc2f_array (pxi, (signed char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dsc2f */

static sf_count_t
dpcm_read_dsc2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		dsc2d_array (pxi, (signed char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dsc2d */

/*------------------------------------------------------------------------------
*/

static sf_count_t
dpcm_read_dles2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		dles2s_array (pxi, (short*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dles2s */

static sf_count_t
dpcm_read_dles2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		dles2i_array (pxi, (short*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dles2i */

static sf_count_t
dpcm_read_dles2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		dles2f_array (pxi, (short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dles2f */

static sf_count_t
dpcm_read_dles2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		dles2d_array (pxi, (short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* dpcm_read_dles2d */

/*==============================================================================
*/

static void s2dsc_array (XI_PRIVATE *pxi, short *src, signed char *dest, int count) ;
static void i2dsc_array (XI_PRIVATE *pxi, int *src, signed char *dest, int count) ;
static void f2dsc_array (XI_PRIVATE *pxi, float *src, signed char *dest, int count, float normfact) ;
static void d2dsc_array (XI_PRIVATE *pxi, double *src, signed char *dest, int count, double normfact) ;

static void	s2dles_array (XI_PRIVATE *pxi, short *src, short *dest, int count) ;
static void i2dles_array (XI_PRIVATE *pxi, int *src, short *dest, int count) ;
static void f2dles_array (XI_PRIVATE *pxi, float *src, short *dest, int count, float normfact) ;
static void d2dles_array (XI_PRIVATE *pxi, double *src, short *dest, int count, double normfact) ;


static sf_count_t
dpcm_write_s2dsc (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2dsc_array (pxi, ptr + total, (signed char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_s2dsc */

static sf_count_t
dpcm_write_i2dsc (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2dsc_array (pxi, ptr + total, (signed char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_i2dsc */

static sf_count_t
dpcm_write_f2dsc (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7F) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2dsc_array (pxi, ptr + total, (signed char*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_f2dsc */

static sf_count_t
dpcm_write_d2dsc (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7F) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2dsc_array (pxi, ptr + total, (signed char*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_d2dsc */


static sf_count_t
dpcm_write_s2dles (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2dles_array (pxi, ptr + total, (short*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_s2dles */

static sf_count_t
dpcm_write_i2dles (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2dles_array (pxi, ptr + total, (short*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_i2dles */

static sf_count_t
dpcm_write_f2dles (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2dles_array (pxi, ptr + total, (short*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_f2dles */

static sf_count_t
dpcm_write_d2dles (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	XI_PRIVATE	*pxi ;
	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	if ((pxi = psf->fdata) == NULL)
		return 0 ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2dles_array (pxi, ptr + total, (short*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* dpcm_write_d2dles */


/*==============================================================================
*/

static void
dsc2s_array (XI_PRIVATE *pxi, signed char *src, int count, short *dest)
{	signed char	last_val ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += src [k] ;
		dest [k] = last_val << 8 ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* dsc2s_array */

static void
dsc2i_array (XI_PRIVATE *pxi, signed char *src, int count, int *dest)
{	signed char	last_val ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += src [k] ;
		dest [k] = last_val << 24 ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* dsc2i_array */

static void
dsc2f_array (XI_PRIVATE *pxi, signed char *src, int count, float *dest, float normfact)
{	signed char	last_val ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += src [k] ;
		dest [k] = last_val * normfact ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* dsc2f_array */

static void
dsc2d_array (XI_PRIVATE *pxi, signed char *src, int count, double *dest, double normfact)
{	signed char	last_val ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += src [k] ;
		dest [k] = last_val * normfact ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* dsc2d_array */

/*------------------------------------------------------------------------------
*/

static void
s2dsc_array (XI_PRIVATE *pxi, short *src, signed char *dest, int count)
{	signed char	last_val, current ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	current = src [k] >> 8 ;
		dest [k] = current - last_val ;
		last_val = current ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* s2dsc_array */

static void
i2dsc_array (XI_PRIVATE *pxi, int *src, signed char *dest, int count)
{	signed char	last_val, current ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	current = src [k] >> 24 ;
		dest [k] = current - last_val ;
		last_val = current ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* i2dsc_array */

static void
f2dsc_array (XI_PRIVATE *pxi, float *src, signed char *dest, int count, float normfact)
{	signed char	last_val, current ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	current = lrintf (src [k] * normfact) ;
		dest [k] = current - last_val ;
		last_val = current ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* f2dsc_array */

static void
d2dsc_array (XI_PRIVATE *pxi, double *src, signed char *dest, int count, double normfact)
{	signed char	last_val, current ;
	int			k ;

	last_val = pxi->last_16 >> 8 ;

	for (k = 0 ; k < count ; k++)
	{	current = lrint (src [k] * normfact) ;
		dest [k] = current - last_val ;
		last_val = current ;
		} ;

	pxi->last_16 = last_val << 8 ;
} /* d2dsc_array */

/*==============================================================================
*/

static void
dles2s_array (XI_PRIVATE *pxi, short *src, int count, short *dest)
{	short	last_val ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += LES2H_SHORT (src [k]) ;
		dest [k] = last_val ;
		} ;

	pxi->last_16 = last_val ;
} /* dles2s_array */

static void
dles2i_array (XI_PRIVATE *pxi, short *src, int count, int *dest)
{	short	last_val ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += LES2H_SHORT (src [k]) ;
		dest [k] = last_val << 16 ;
		} ;

	pxi->last_16 = last_val ;
} /* dles2i_array */

static void
dles2f_array (XI_PRIVATE *pxi, short *src, int count, float *dest, float normfact)
{	short	last_val ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += LES2H_SHORT (src [k]) ;
		dest [k] = last_val * normfact ;
		} ;

	pxi->last_16 = last_val ;
} /* dles2f_array */

static void
dles2d_array (XI_PRIVATE *pxi, short *src, int count, double *dest, double normfact)
{	short	last_val ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	last_val += LES2H_SHORT (src [k]) ;
		dest [k] = last_val * normfact ;
		} ;

	pxi->last_16 = last_val ;
} /* dles2d_array */

/*------------------------------------------------------------------------------
*/

static void
s2dles_array (XI_PRIVATE *pxi, short *src, short *dest, int count)
{	short	diff, last_val ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	diff = src [k] - last_val ;
		dest [k] = LES2H_SHORT (diff) ;
		last_val = src [k] ;
		} ;

	pxi->last_16 = last_val ;
} /* s2dles_array */

static void
i2dles_array (XI_PRIVATE *pxi, int *src, short *dest, int count)
{	short	diff, last_val ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	diff = (src [k] >> 16) - last_val ;
		dest [k] = LES2H_SHORT (diff) ;
		last_val = src [k] >> 16 ;
		} ;

	pxi->last_16 = last_val ;
} /* i2dles_array */

static void
f2dles_array (XI_PRIVATE *pxi, float *src, short *dest, int count, float normfact)
{	short	diff, last_val, current ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	current = lrintf (src [k] * normfact) ;
		diff = current - last_val ;
		dest [k] = LES2H_SHORT (diff) ;
		last_val = current ;
		} ;

	pxi->last_16 = last_val ;
} /* f2dles_array */

static void
d2dles_array (XI_PRIVATE *pxi, double *src, short *dest, int count, double normfact)
{	short	diff, last_val, current ;
	int		k ;

	last_val = pxi->last_16 ;

	for (k = 0 ; k < count ; k++)
	{	current = lrint (src [k] * normfact) ;
		diff = current - last_val ;
		dest [k] = LES2H_SHORT (diff) ;
		last_val = current ;
		} ;

	pxi->last_16 = last_val ;
} /* d2dles_array */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 1ab2dbe0-29af-4d80-9c6f-cb21b67521bc
*/
