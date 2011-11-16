//
//  util_path.cpp
//  chuck
//
//  Created by Spencer Salazar on 11/15/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//


#include "util_path.h"
#include <sys/stat.h> // TODO: Windows



Chuck_Path_Manager * Chuck_Path_Manager::s_instance = NULL;

Chuck_Path_Manager * Chuck_Path_Manager::instance()
{
    if( s_instance == NULL )
    {
        s_instance = new Chuck_Path_Manager;
    }
    
    return s_instance;
}


int Chuck_Path_Manager::numPaths( const std::string & nspc )
{
    return m_paths[nspc].size();
}


void Chuck_Path_Manager::addPath( const std::string & p, const std::string & nspc )
{
    if(m_paths.count(nspc) == 0)
    {
        m_paths[nspc] = std::vector<std::string>();
    }
    
    m_paths[nspc].push_back(p);
}


const std::string &Chuck_Path_Manager::getPath( unsigned int i, const std::string & nspc )
{
    return m_paths[nspc][i];
}


static bool is_absolute( const std::string & p )
{
#if defined(__WINDOWS_ASIO__) || defined(__WINDOWS_DS__)
#error is_absolute not implemented for Windows
#else
    if( p.size() >= 1 && p[0] == '/' )
        return true;
    return false;
#endif
}


static bool exists( const std::string & p )
{
#if defined(__WINDOWS_ASIO__) || defined(__WINDOWS_DS__)
#error exists not implemented for Windows
#else
    struct stat s;
    
    if( stat( p.c_str(), &s ) != 0 )
    {
        return false;
    }
    
    return true;
#endif
}


static std::string make_path( const std::string & base, const std::string & file )
{
#if defined(__WINDOWS_ASIO__) || defined(__WINDOWS_DS__)
#error make_path not implemented for Windows
#else
    
    return base + "/" + file;
#endif
}


//static bool is_readable( std::filepath & p )
//{
//    struct stat s;
//    if( stat( p.c_str(), &s ) != 0 )
//    {
//        return false;
//    }
//    
//    return false;
//}
//
//static bool is_writeable( std::filepath & p )
//{
//    
//}


std::string Chuck_Path_Manager::resolveFilename( const std::string & filename, 
                                                 const std::string & nspc, 
                                                 bool mustBeReadable, 
                                                 bool mustBeWriteable )
{
    if( is_absolute( filename ) )
        return filename;
    
    std::string path;
    
    for( std::vector<std::string>::iterator i = m_paths[nspc].begin();
         i != m_paths[nspc].end(); i++ )
    {
        path = make_path( *i, filename );
        if( exists( path ) )
            return path;
    }
    
    return filename;
}



