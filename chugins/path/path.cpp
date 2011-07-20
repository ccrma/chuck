

#include "chuck_dl.h"
#include "chuck_def.h"
#include "chuck_type.h"
#include "chuck_instr.h"

#include <stdio.h>


#ifndef __PLATFORM_WIN32__
static const char path_separator = '/';
#else
static const char path_separator = '\\';
#endif // __PLATFORM_WIN32__


CK_DLL_SFUN(path_basename);
CK_DLL_SFUN(path_dirname);
CK_DLL_SFUN(path_extension);


CK_DLL_QUERY(path)
{
    QUERY->setname(QUERY, "path");
    
    QUERY->begin_class(QUERY, "Path", "Object");
    
    QUERY->add_sfun(QUERY, path_basename, "string", "basename");
    QUERY->add_arg(QUERY, "string", "path");
    
    QUERY->add_sfun(QUERY, path_dirname, "string", "dirname");
    QUERY->add_arg(QUERY, "string", "path");
    
    QUERY->add_sfun(QUERY, path_extension, "string", "extension");
    QUERY->add_arg(QUERY, "string", "path");
    
    QUERY->end_class(QUERY);
    
    return TRUE;
}


CK_DLL_SFUN(path_basename)
{
    const char *path = GET_NEXT_STRING(ARGS)->str.c_str();
    const char *last_slash = strrchr(path, path_separator);
    
    std::string basename;
    
    if(last_slash == NULL)
    {
        basename = path;
    }
    else
    {
        basename = path;
        size_t dirname_length = (last_slash - path + 1) * sizeof(char);
        basename.erase(0, dirname_length);
    }
    
    Chuck_DL_Api::String str = API.object.create_string( basename );
    
    RETURN->v_string = ( Chuck_String * ) str; 
}

CK_DLL_SFUN(path_dirname)
{
    const char *path = GET_NEXT_STRING(ARGS)->str.c_str();
    const char *last_slash = strrchr(path, path_separator);
    
    std::string dirname;
    
    if(last_slash == NULL)
    {
        dirname = "";
    }
    else
    {
        dirname = path;
        size_t dirname_length = (last_slash - path + 1)*sizeof(char);
        dirname.erase(dirname_length, dirname.size() - dirname_length);
    }
    
    Chuck_DL_Api::String str = API.object.create_string( dirname );
    
    RETURN->v_string = ( Chuck_String * ) str; 
}

CK_DLL_SFUN(path_extension)
{
    const char *path = GET_NEXT_STRING(ARGS)->str.c_str();
    const char *last_dot = strrchr(path, '.');
    
    std::string extension;
    
    if(last_dot == NULL || last_dot == path)
    {
        extension = "";
    }
    else
    {
        extension = path;
        extension.erase(0, (last_dot - path + 1) * sizeof(char));
    }
    
    Chuck_DL_Api::String str = API.object.create_string( extension );
    
    RETURN->v_string = ( Chuck_String * ) str; 
}


