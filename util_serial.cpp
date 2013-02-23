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
// file: util_serial.cpp
// desc: ...
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2012
//-----------------------------------------------------------------------------

#include "chuck_def.h"
#include "util_serial.h"
#include "chuck_errmsg.h"


#if defined(__MACOSX_CORE__)

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>

vector<string> SerialIOManager::availableSerialDevices()
{
    vector<string> devices;
    
    io_iterator_t serialIterator = NULL;
    kern_return_t kernResult;
    mach_port_t masterPort;
    CFMutableDictionaryRef classesToMatch = NULL;
    
    io_object_t serialObject;
    static char resultStr[256];
    CFStringRef calloutCFKeyName = CFStringCreateWithCString(kCFAllocatorDefault, kIOCalloutDeviceKey, kCFStringEncodingUTF8);
    CFStringRef dialinCFKeyName = CFStringCreateWithCString(kCFAllocatorDefault, kIODialinDeviceKey, kCFStringEncodingUTF8);

    if((kernResult = IOMasterPort(NULL, &masterPort)) != KERN_SUCCESS)
    {
        EM_log(CK_LOG_WARNING, "[SerialIOManager] IOMasterPort returned %d\n", kernResult);
        goto error;
    }
    if((classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue)) == NULL)
    {
        EM_log(CK_LOG_WARNING, "[SerialIOManager] IOServiceMatching returned NULL\n" );
        goto error;
    }
    CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDAllTypes));
    kernResult = IOServiceGetMatchingServices(masterPort, classesToMatch, &serialIterator);
    classesToMatch = NULL;
    if(kernResult != KERN_SUCCESS)
    {
        EM_log(CK_LOG_WARNING, "[SerialIOManager] IOServiceGetMatchingServices returned %d\n", kernResult);
        goto error;
    }
        
    while((serialObject = IOIteratorNext(serialIterator)))
    {
        CFTypeRef nameCFstring;
        nameCFstring = IORegistryEntryCreateCFProperty(serialObject,
                                                       calloutCFKeyName,
                                                       kCFAllocatorDefault, 0);
        if(nameCFstring)
        {
            CFStringGetCString((CFStringRef) nameCFstring, resultStr, sizeof(resultStr), kCFStringEncodingUTF8);
            CFRelease(nameCFstring);
            devices.push_back(string(resultStr));
        }
        
        // TODO: use tty name also? 
        // see: http://stuffthingsandjunk.blogspot.com/2009/03/devcu-vs-devtty-osx-serial-ports.html
//        nameCFstring = IORegistryEntryCreateCFProperty(serialObject,
//                                                       dialinCFKeyName,
//                                                       kCFAllocatorDefault, 0);
//        if(nameCFstring)
//        {
//            CFStringGetCString((CFStringRef) nameCFstring, resultStr, sizeof(resultStr), kCFStringEncodingUTF8);
//            CFRelease(nameCFstring);
//            devices.push_back(string(resultStr));
//        }
    }
    
    goto cleanup;
    
error:
    devices.clear();
    
cleanup:
    if(classesToMatch) { CFRelease(classesToMatch); classesToMatch = NULL; }
    if(calloutCFKeyName) { CFRelease(calloutCFKeyName); calloutCFKeyName = NULL; }
    if(dialinCFKeyName) { CFRelease(dialinCFKeyName); dialinCFKeyName = NULL; }
    
    return devices;
}

#elif defined(__LINUX_ALSA__) || defined(__LINUX_JACK__) || defined(__LINUX_OSS__)


#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>


vector<string> SerialIOManager::availableSerialDevices()
{
    vector<string> devices;
    const int buf_size = PATH_MAX;
    const char * serial_dir = "/dev/serial/by-id";
    char link_buf[buf_size];
    char path_buf[buf_size];

    DIR * dir = opendir(serial_dir);

    if(dir == NULL)
    {
        // either we cannot open the directory, or there were no devices
        EM_log(CK_LOG_INFO, "(SerialIOManager): unable to enumerate available devices");
        goto error;
    }

    dirent * dir_info;
    while((dir_info = readdir(dir)))
    {
        if(strcmp(dir_info->d_name, ".") == 0 ||
           strcmp(dir_info->d_name, "..") == 0)
            continue;
        snprintf(link_buf, buf_size, "%s/%s", serial_dir, dir_info->d_name);
        int link_size = readlink(link_buf, path_buf, buf_size-1);
        if(link_size <= 0)
        {
            EM_log(CK_LOG_INFO, "(SerialIOManager): readlink failed on '%s'", link_buf);
            goto error;
        }

        // readlink does not NULL-terminate (...)
        path_buf[link_size] = '\0';

        if(path_buf[0] != '/')
        {
            // normalize path
            snprintf(link_buf, buf_size, "%s/%s", serial_dir, path_buf);

            if(!realpath(link_buf, path_buf))
                goto error;
        }

        devices.push_back(string(path_buf));
    }

    goto cleanup;
    
error:
    devices.clear();
    
cleanup:
    return devices;
}

#elif defined(__WINDOWS_DS__)

vector<string> SerialIOManager::availableSerialDevices()
{
    vector<string> devices;

    return devices;
}


#endif /* __MACOSX_CORE__ */

