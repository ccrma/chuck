//
//  util_path.h
//  chuck
//
//  Created by Spencer Salazar on 11/15/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __UTIL_PATH_H__
#define __UTIL_PATH_H__


#include <string>
#include <vector>
#include <map>


class Chuck_Path_Manager
{
public:
    
    static Chuck_Path_Manager * instance();
    
    int numPaths( const std::string & nspc = "" );
    void addPath( const std::string & p, const std::string & nspc = "" );
    const std::string &getPath( unsigned int i, const std::string & nspc = "" );
//    void removePath( unsigned int i, const std::string & nspc = "" );
    
    std::string resolveFilename( const std::string & filename, 
                                 const std::string & nspc = "",
                                 bool mustBeReadable = true, 
                                 bool mustBeWriteable = false );
        
private:
    
    std::map<std::string, std::vector<std::string> > m_paths;
    
    static Chuck_Path_Manager * s_instance;
    
    Chuck_Path_Manager() {};
    Chuck_Path_Manager(const Chuck_Path_Manager &) {};
    Chuck_Path_Manager& operator=(Chuck_Path_Manager const&cpm) {return *this;};
};

#endif // __UTIL_PATH_H__
