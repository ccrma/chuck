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

//-----------------------------------------------------------------------------
// name: chuck_dl.cpp
// desc: chuck dynamic linking
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// mac code from apple
//
// date: spring 2004
//-----------------------------------------------------------------------------
#include "chuck_dl.h"
#ifndef __CKDL_NO_BBQ__
#include "chuck_bbq.h"
#endif
using namespace std;



//-----------------------------------------------------------------------------
// name: load()
// desc: load dynamic link library
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::load( const char * filename, const char * func, t_CKBOOL lazy )
{
    // open
    m_handle = dlopen( filename, lazy ? RTLD_LAZY : RTLD_NOW );

    // still not there
    if( !m_handle )
    {
        m_last_error = dlerror();
        return FALSE;
    }

    // save the filename
    m_filename = filename;
    m_done_query = FALSE;
    m_func = func;

    // if not lazy, do it
    if( !lazy && !this->query() )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load()
// desc: load dynamic link library
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::load( f_ck_query query_func, t_CKBOOL lazy )
{
    m_query_func = query_func;
    m_done_query = FALSE;
    m_func = "ck_query";
    
    // if not lazy, do it
    if( !lazy && !this->query() )
        return FALSE;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: good()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::good() const
{
    return ( m_handle != NULL || m_query_func != NULL );
}




//-----------------------------------------------------------------------------
// name: query()
// desc: ...
//-----------------------------------------------------------------------------
const Chuck_DL_Query * Chuck_DLL::query( )
{
    if( !m_handle && !m_query_func )
    {
        m_last_error = "dynamic link library not loaded for query...";
        return NULL;
    }

    // return if there already
    if( m_done_query )
        return &m_query;
    
    // get the address of the query function from the DLL
    if( !m_query_func )
        m_query_func = (f_ck_query)this->get_addr( m_func.c_str() );
    if( !m_query_func )
        m_query_func = (f_ck_query)this->get_addr( (string("_")+m_func).c_str() );
    if( !m_query_func )
    {
        m_last_error = string("no query function found in dll '") 
                       + m_filename + string("'");
        return NULL;
    }

    // get the address of the attach function from the DLL
    if( !m_attach_func )
        m_attach_func = (f_ck_attach)this->get_addr( "ck_attach" );
    if( !m_attach_func )
        m_attach_func = (f_ck_attach)this->get_addr( "_ck_attach" );

    // get the address of the detach function from the DLL
    if( !m_detach_func )
        m_detach_func = (f_ck_detach)this->get_addr( "ck_detach" );
    if( !m_detach_func )
        m_detach_func = (f_ck_detach)this->get_addr( "_ck_detach" );

    // do the query
    m_query.clear();
    if( !m_query_func( &m_query ) )
    {
        m_last_error = string("unsuccessful query in dll '") + m_filename
                       + string("'");
        return NULL;
    }
    
    // load the proto table
    Chuck_DL_Proto * proto;
    m_name2proto.clear();
    for( t_CKUINT i = 0; i < m_query.dll_exports.size(); i++ )
    {
        proto = &m_query.dll_exports[i];
        if( m_name2proto[proto->name] )
        {
            m_last_error = string("duplicate export name '") + proto->name
                           + string("'");
            return NULL;
        }
        
        // get the addr
        if( !proto->addr )
            proto->addr = (f_ck_func)this->get_addr( (char *)proto->name.c_str() );
        if( !proto->addr )
        {
            m_last_error = string("no addr associated with export '") + 
                           proto->name + string("'");
            return NULL;
        }

        // put in the lookup table
        m_name2proto[proto->name] = proto;
    }
    
    // load the proto table
    Chuck_UGen_Info * info;
    m_name2ugen.clear();
    for( t_CKUINT j = 0; j < m_query.ugen_exports.size(); j++ )
    {
        info = &m_query.ugen_exports[j];
        if( m_name2ugen[info->name] )
        {
            m_last_error = string("duplicate export ugen name '") + info->name
                           + string("'");
            return NULL;
        }
        
        // put in the lookup table
        m_name2ugen[info->name] = info;
    }

    m_done_query = TRUE;
    
    // call attach
    if( m_attach_func ) m_attach_func( 0, NULL );

    return &m_query;
}




//-----------------------------------------------------------------------------
// name: unload()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::unload()
{
    if( !m_handle && !m_query_func )
    {
        m_last_error = "cannot unload dynamic library - nothing open...";
        return FALSE;
    }

    if( m_detach_func ) m_detach_func( 0, NULL );

    if( m_handle )
    {
        dlclose( m_handle );
        m_handle = NULL;
    }
    else
        m_query_func = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_addr()
// desc: ...
//-----------------------------------------------------------------------------
void * Chuck_DLL::get_addr( const char * symbol )
{
    if( !m_handle )
    {
        m_last_error = "cannot find addr from dynamic library - nothing open...";
        return FALSE;
    }
    
    return dlsym( m_handle, symbol );
}




//-----------------------------------------------------------------------------
// name: proto()
// desc: ...
//-----------------------------------------------------------------------------
const Chuck_DL_Proto * Chuck_DLL::proto( const char * symbol )
{
    if( !m_handle && !m_query_func )
    {
        m_last_error = "cannot find proto from dynamic library - nothing open...";
        return NULL;
    }

    // get the proto from the hash
    Chuck_DL_Proto * proto = m_name2proto[symbol];
    if( !proto )
    {
        m_last_error = string("no prototype function '") + string(symbol)
                       + string("' found");
        return NULL;
    }
    
    return proto;
}




//-----------------------------------------------------------------------------
// name: last_error()
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_DLL::last_error() const
{
    return m_last_error.c_str();
}




//-----------------------------------------------------------------------------
// name: Chuck_DL_Query
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_Query::Chuck_DL_Query( )
{ add_export = __ck_addexport; add_param = __ck_addparam;
  ugen_add = __ck_ugen_add; ugen_func = __ck_ugen_func;
  ugen_ctrl = __ck_ugen_ctrl; set_name = __ck_setname;
  ugen_extends = __ck_ugen_extends;  // XXX - pld
  dll_name = "[noname]"; reserved = NULL;
  
#ifndef __CKDL_NO_BBQ__
  srate = Digitalio::sampling_rate() ; bufsize = Digitalio::buffer_size();
#else
  srate = 0; bufsize = 0;
#endif

  ugen_exports.resize( 128 );
  linepos = 0;
}




// add proto
extern "C" void CK_DLL_CALL __ck_addexport( Chuck_DL_Query * query, 
           const char * type, const char * name, f_ck_func addr,
           t_CKBOOL is_func )
{ query->dll_exports.push_back( Chuck_DL_Proto( type, name, (void *)addr, is_func ) ); }

// add param
extern "C" void CK_DLL_CALL __ck_addparam( Chuck_DL_Query * query,
           const char * type, const char * name )
{ if( query->dll_exports.size() ) 
    query->dll_exports[query->dll_exports.size()-1].add_param( type, name ); }

// add ugen
extern "C" void CK_DLL_CALL __ck_ugen_add( Chuck_DL_Query * query,
           const char * name, void * reserved )
{ query->ugen_exports.push_back( Chuck_UGen_Info( name ) );
  query->reserved = reserved; }

// set funcs
extern "C" void CK_DLL_CALL __ck_ugen_func( Chuck_DL_Query * query,
           f_ctor c, f_dtor d, f_tick t, f_pmsg p )
{ if( query->ugen_exports.size() )
    query->ugen_exports[query->ugen_exports.size()-1].set( c, d, t, p ); }

// add ctrl
extern "C" void CK_DLL_CALL __ck_ugen_ctrl( Chuck_DL_Query * query,
           f_ctrl c, f_cget g, const char * t, const char * n )
{ if( query->ugen_exports.size() )
    query->ugen_exports[query->ugen_exports.size()-1].add( c, g, t, n ); }

//XXX - pld  inherit functions from 'parent' ugen
extern "C" void CK_DLL_CALL __ck_ugen_extends( Chuck_DL_Query * query,
           const char * parent )
{
    if( query->ugen_exports.size() > 1 )
        for( int i= 0 ; i < query->ugen_exports.size() - 1 ; i++ )
            if( strcmp ( parent, query->ugen_exports[i].name.c_str() ) == 0 )
            {
                query->ugen_exports[query->ugen_exports.size()-1].inherit( 
                    &(query->ugen_exports[i]) );
                return;					     
            }
}


// set name
extern "C" void CK_DLL_CALL __ck_setname( Chuck_DL_Query * query,
           const char * name )
{ query->dll_name = name; }

// windows
#if defined(__WINDOWS_DS__)
extern "C"
{
#include <windows.h>

void *dlopen( const char *path, int mode)
{
    return (void *)LoadLibrary(path);
}

int dlclose( void *handle)
{
    FreeLibrary((HMODULE)handle);
    return 1;
}

void *dlsym( void * handle, const char *symbol )
{
    return (void *)GetProcAddress((HMODULE)handle, symbol);
}

const char * dlerror( void )
{
    int error = GetLastError();
    if( error == 0 ) return NULL;
    sprintf( dlerror_buffer, "%i", error );
    return dlerror_buffer;
}
}
#endif

// mac os x
#if defined(__MACOSX_CORE__) && MAC_OS_X_VERSION_MAX_ALLOWED <= 1030

extern "C"
{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "mach-o/dyld.h"

/*
 * debugging macros
 */
#if DEBUG > 0
#define DEBUG_PRINT(format) fprintf(stderr,(format));fflush(stderr)
#define DEBUG_PRINT1(format,arg1) fprintf(stderr,(format),(arg1));\
  fflush(stderr)
#define DEBUG_PRINT2(format,arg1,arg2) fprintf(stderr,(format),\
  (arg1),(arg2));fflush(stderr)
#define DEBUG_PRINT3(format,arg1,arg2,arg3) fprintf(stderr,(format),\
  (arg1),(arg2),(arg3));fflush(stderr)
#else
#define DEBUG_PRINT(format) /**/
#define DEBUG_PRINT1(format,arg1) /**/
#define DEBUG_PRINT2(format,arg1,arg2) /**/
#define DEBUG_PRINT3(format,arg1,arg2,arg3) /**/
#undef DEBUG
#endif

/*
 * The structure of a dlopen() handle.
 */
struct dlopen_handle {
    dev_t dev;		/* the path's device and inode number from stat(2) */
    ino_t ino; 
    int dlopen_mode;	/* current dlopen mode for this handle */
    int dlopen_count;	/* number of times dlopen() called on this handle */
    NSModule module;	/* the NSModule returned by NSLinkModule() */
    struct dlopen_handle *prev;
    struct dlopen_handle *next;
};
static struct dlopen_handle *dlopen_handles = NULL;
static const struct dlopen_handle main_program_handle = {NULL};
static char *dlerror_pointer = NULL;

/*
 * NSMakePrivateModulePublic() is not part of the public dyld API so we define
 * it here.  The internal dyld function pointer for
 * __dyld_NSMakePrivateModulePublic is returned so thats all that maters to get
 * the functionality need to implement the dlopen() interfaces.
 */
static
int
NSMakePrivateModulePublic(
NSModule module)
{
    static int (*p)(NSModule module) = NULL;

	if(p == NULL)
	    _dyld_func_lookup("__dyld_NSMakePrivateModulePublic",
			      (unsigned long *)&p);
	if(p == NULL){
#ifdef DEBUG
	    printf("_dyld_func_lookup of __dyld_NSMakePrivateModulePublic "
		   "failed\n");
#endif
	    return(FALSE);
	}
	return(p(module));
}

/*
 * helper routine: search for a named module in various locations
 */
static
int
_dl_search_paths(
const char *filename,
char *pathbuf,
struct stat *stat_buf)
{
    const char *pathspec;
    const char *element;
    const char *p;
    char *q;
    char *pathbuf_end;
    const char *envvars[] = {
        "$DYLD_LIBRARY_PATH",
        "$LD_LIBRARY_PATH",
        "/usr/lib:/lib",
        NULL };
    int envvar_index;

        pathbuf_end = pathbuf + PATH_MAX - 8;

	for(envvar_index = 0; envvars[envvar_index]; envvar_index++){
	    if(envvars[envvar_index][0] == '$'){
	        pathspec = getenv(envvars[envvar_index]+1);
	    }
	    else {
	        pathspec = envvars[envvar_index];
	    }

	    if(pathspec != NULL){
	        element = pathspec;
		while(*element){
	            /* extract path list element */
		    p = element;
		    q = pathbuf;
		    while(*p && *p != ':' && q < pathbuf_end) *q++ = *p++;
		    if(q == pathbuf){  /* empty element */
		        if(*p){
		            element = p+1;
			    continue;
			}
			break;
		    }
		    if (*p){
		        element = p+1;
		    }
		    else{
		        element = p;  /* this terminates the loop */
		    }

		    /* add slash if neccessary */
		    if(*(q-1) != '/' && q < pathbuf_end){
		        *q++ = '/';
		    }

		    /* append module name */
		    p = filename;
		    while(*p && q < pathbuf_end) *q++ = *p++;
		    *q++ = 0;

		    if(q >= pathbuf_end){
		        /* maybe add an error message here */
		        break;
		    }

		    if(stat(pathbuf, stat_buf) == 0){
		        return 0;
		    }
		}
	    }
	}

	/* we have searched everywhere, now we give up */
	return -1;
}

/*
 * dlopen() the MacOS X version of the FreeBSD dlopen() interface.
 */
void *
dlopen(
const char *path,
int mode)
{
    const char *module_path;
    void *retval;
    struct stat stat_buf;
    NSObjectFileImage objectFileImage;
    NSObjectFileImageReturnCode ofile_result_code;
    NSModule module;
    struct dlopen_handle *p;
    unsigned long options;
    NSSymbol NSSymbol;
    void (*init)(void);
    char pathbuf[PATH_MAX];

        DEBUG_PRINT2("libdl: dlopen(%s,0x%x) -> ", path, (unsigned int)mode);

	dlerror_pointer = NULL;
	/*
	 * A NULL path is to indicate the caller wants a handle for the
	 * main program.
 	 */
	if(path == NULL){
	    retval = (void *)&main_program_handle;
	    DEBUG_PRINT1("main / %p\n", retval);
	    return(retval);
	}

	/* see if the path exists and if so get the device and inode number */
	if(stat(path, &stat_buf) == -1){
	    dlerror_pointer = strerror(errno);

	    if(path[0] == '/'){
	        DEBUG_PRINT1("ERROR (stat): %s\n", dlerror_pointer);
	        return(NULL);
	    }

	    /* search for the module in various places */
	    if(_dl_search_paths(path, pathbuf, &stat_buf)){
	        /* dlerror_pointer is unmodified */
	        DEBUG_PRINT1("ERROR (stat): %s\n", dlerror_pointer);
	        return(NULL);
	    }
	    DEBUG_PRINT1("found %s -> ", pathbuf);
	    module_path = pathbuf;
	    dlerror_pointer = NULL;
	}
	else{
	    module_path = path;
	}

	/*
	 * If we don't want an unshared handle see if we already have a handle
	 * for this path.
	 */
	if((mode & RTLD_UNSHARED) != RTLD_UNSHARED){
	    p = dlopen_handles;
	    while(p != NULL){
		if(p->dev == stat_buf.st_dev && p->ino == stat_buf.st_ino){
		    /* skip unshared handles */
		    if((p->dlopen_mode & RTLD_UNSHARED) == RTLD_UNSHARED)
			continue;
		    /*
		     * We have already created a handle for this path.  The
		     * caller might be trying to promote an RTLD_LOCAL handle
		     * to a RTLD_GLOBAL.  Or just looking it up with
		     * RTLD_NOLOAD.
		     */
		    if((p->dlopen_mode & RTLD_LOCAL) == RTLD_LOCAL &&
		       (mode & RTLD_GLOBAL) == RTLD_GLOBAL){
			/* promote the handle */
			if(NSMakePrivateModulePublic(p->module) == TRUE){
			    p->dlopen_mode &= ~RTLD_LOCAL;
			    p->dlopen_mode |= RTLD_GLOBAL;
			    p->dlopen_count++;
			    DEBUG_PRINT1("%p\n", p);
			    return(p);
			}
			else{
			    dlerror_pointer = "can't promote handle from "
					      "RTLD_LOCAL to RTLD_GLOBAL";
			    DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
			    return(NULL);
			}
		    }
		    p->dlopen_count++;
		    DEBUG_PRINT1("%p\n", p);
		    return(p);
		}
		p = p->next;
	    }
	}
	
	/*
	 * We do not have a handle for this path if we were just trying to
	 * look it up return NULL to indicate we don't have it.
	 */
	if((mode & RTLD_NOLOAD) == RTLD_NOLOAD){
	    dlerror_pointer = "no existing handle for path RTLD_NOLOAD test";
	    DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
	    return(NULL);
	}

	/* try to create an object file image from this path */
	ofile_result_code = NSCreateObjectFileImageFromFile(module_path,
							    &objectFileImage);
	if(ofile_result_code != NSObjectFileImageSuccess){
	    switch(ofile_result_code){
	    case NSObjectFileImageFailure:
		dlerror_pointer = "object file setup failure";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    case NSObjectFileImageInappropriateFile:
		dlerror_pointer = "not a Mach-O MH_BUNDLE file type";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    case NSObjectFileImageArch:
		dlerror_pointer = "no object for this architecture";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    case NSObjectFileImageFormat:
		dlerror_pointer = "bad object file format";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    case NSObjectFileImageAccess:
		dlerror_pointer = "can't read object file";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    default:
		dlerror_pointer = "unknown error from "
				  "NSCreateObjectFileImageFromFile()";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    }
	}

	/* try to link in this object file image */
	options = NSLINKMODULE_OPTION_PRIVATE;
	if((mode & RTLD_NOW) == RTLD_NOW)
	    options |= NSLINKMODULE_OPTION_BINDNOW;
	module = NSLinkModule(objectFileImage, module_path, options);
	NSDestroyObjectFileImage(objectFileImage) ;
	if(module == NULL){
	    dlerror_pointer = "NSLinkModule() failed for dlopen()";
	    DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
	    return(NULL);
	}

	/*
	 * If the handle is to be global promote the handle.  It is done this
	 * way to avoid multiply defined symbols.
	 */
	if((mode & RTLD_GLOBAL) == RTLD_GLOBAL){
	    if(NSMakePrivateModulePublic(module) == FALSE){
		dlerror_pointer = "can't promote handle from RTLD_LOCAL to "
				  "RTLD_GLOBAL";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    }
	}

	p = (dlopen_handle *)malloc(sizeof(struct dlopen_handle));
	if(p == NULL){
	    dlerror_pointer = "can't allocate memory for the dlopen handle";
	    DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
	    return(NULL);
	}

	/* fill in the handle */
	p->dev = stat_buf.st_dev;
        p->ino = stat_buf.st_ino;
	if(mode & RTLD_GLOBAL)
	    p->dlopen_mode = RTLD_GLOBAL;
	else
	    p->dlopen_mode = RTLD_LOCAL;
	p->dlopen_mode |= (mode & RTLD_UNSHARED) |
			  (mode & RTLD_NODELETE) |
			  (mode & RTLD_LAZY_UNDEF);
	p->dlopen_count = 1;
	p->module = module;
	p->prev = NULL;
	p->next = dlopen_handles;
	if(dlopen_handles != NULL)
	    dlopen_handles->prev = p;
	dlopen_handles = p;

	/* call the init function if one exists */
	NSSymbol = NSLookupSymbolInModule(p->module, "__init");
	if(NSSymbol != NULL){
	    init = ( void(*)() )NSAddressOfSymbol(NSSymbol);
	    init();
	}
	
	DEBUG_PRINT1("%p\n", p);
	return(p);
}

/*
 * dlsym() the MacOS X version of the FreeBSD dlopen() interface.
 */
void *
dlsym(
void * handle,
const char *symbol)
{
    struct dlopen_handle *dlopen_handle, *p;
    NSSymbol NSSymbol;
    void *address;

        DEBUG_PRINT2("libdl: dlsym(%p,%s) -> ", handle, symbol);

	dlopen_handle = (struct dlopen_handle *)handle;

	/*
	 * If this is the handle for the main program do a global lookup.
	 */
	if(dlopen_handle == (struct dlopen_handle *)&main_program_handle){
	    if(NSIsSymbolNameDefined(symbol) == TRUE){
		NSSymbol = NSLookupAndBindSymbol(symbol);
		address = NSAddressOfSymbol(NSSymbol);
		dlerror_pointer = NULL;
		DEBUG_PRINT1("%p\n", address);
		return(address);
	    }
	    else{
		dlerror_pointer = "symbol not found";
		DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		return(NULL);
	    }
	}

	/*
	 * Find this handle and do a lookup in just this module.
	 */
	p = dlopen_handles;
	while(p != NULL){
	    if(dlopen_handle == p){
		NSSymbol = NSLookupSymbolInModule(p->module, symbol);
		if(NSSymbol != NULL){
		    address = NSAddressOfSymbol(NSSymbol);
		    dlerror_pointer = NULL;
		    DEBUG_PRINT1("%p\n", address);
		    return(address);
		}
		else{
		    dlerror_pointer = "symbol not found";
		    DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		    return(NULL);
		}
	    }
	    p = p->next;
	}

	dlerror_pointer = "bad handle passed to dlsym()";
	DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
	return(NULL);
}

/*
 * dlerror() the MacOS X version of the FreeBSD dlopen() interface.
 */
const char *
dlerror(
void)
{
    const char *p;

	p = (const char *)dlerror_pointer;
	dlerror_pointer = NULL;
	return(p);
}

/*
 * dlclose() the MacOS X version of the FreeBSD dlopen() interface.
 */
int
dlclose(
void * handle)
{
    struct dlopen_handle *p, *q;
    unsigned long options;
    NSSymbol NSSymbol;
    void (*fini)(void);

        DEBUG_PRINT1("libdl: dlclose(%p) -> ", handle);

	dlerror_pointer = NULL;
	q = (struct dlopen_handle *)handle;
	p = dlopen_handles;
	while(p != NULL){
	    if(p == q){
		/* if the dlopen() count is not zero we are done */
		p->dlopen_count--;
		if(p->dlopen_count != 0){
		    DEBUG_PRINT("OK");
		    return(0);
		}

		/* call the fini function if one exists */
		NSSymbol = NSLookupSymbolInModule(p->module, "__fini");
		if(NSSymbol != NULL){
		    fini = ( void(*)() )NSAddressOfSymbol(NSSymbol);
		    fini();
		}

		/* unlink the module for this handle */
		options = 0;
		if(p->dlopen_mode & RTLD_NODELETE)
		    options |= NSUNLINKMODULE_OPTION_KEEP_MEMORY_MAPPED;
		if(p->dlopen_mode & RTLD_LAZY_UNDEF)
		    options |= NSUNLINKMODULE_OPTION_RESET_LAZY_REFERENCES;
		if(NSUnLinkModule(p->module, options) == FALSE){
		    dlerror_pointer = "NSUnLinkModule() failed for dlclose()";
		    DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
		    return(-1);
		}
		if(p->prev != NULL)
		    p->prev->next = p->next;
		if(p->next != NULL)
		    p->next->prev = p->prev;
		if(dlopen_handles == p)
		    dlopen_handles = p->next;
		free(p);
		DEBUG_PRINT("OK");
		return(0);
	    }
	    p = p->next;
	}
	dlerror_pointer = "invalid handle passed to dlclose()";
	DEBUG_PRINT1("ERROR: %s\n", dlerror_pointer);
	return(-1);
}

}

#else

// do nothing, it's all in dlfcn

#endif
