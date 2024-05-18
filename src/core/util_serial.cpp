/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

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
// desc: utility for serial I/O
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2012
//-----------------------------------------------------------------------------
#include "util_serial.h"
#include "chuck_errmsg.h"

#if defined(__PLATFORM_APPLE__) && !defined(__CHIP_MODE__)

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>

#include <Availability.h> // 1.5.0.7 (ge) added; __MAC_OS_X_VERSION_MIN_REQUIRED

using namespace std;

vector<string> SerialIOManager::availableSerialDevices()
{
    vector<string> devices;

    io_iterator_t serialIterator = 0; // NULL;
    kern_return_t kernResult;
    mach_port_t mainPort;
    CFMutableDictionaryRef classesToMatch = NULL;

    io_object_t serialObject;
    static char resultStr[256];
    CFStringRef calloutCFKeyName = CFStringCreateWithCString(kCFAllocatorDefault, kIOCalloutDeviceKey, kCFStringEncodingUTF8);
    CFStringRef dialinCFKeyName = CFStringCreateWithCString(kCFAllocatorDefault, kIODialinDeviceKey, kCFStringEncodingUTF8);

    // ge: changed NULL to 0 below to get rid of warning 1.3.2.0
    // ge: updated kIOMasterPort (deprecated as of macOS 12) to kIOMainPort 1.5.0.7
#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 120000 // before macOS 12
    if( (kernResult = IOMasterPort(0, &mainPort)) != KERN_SUCCESS )
#else
    if( (kernResult = IOMainPort(0, &mainPort)) != KERN_SUCCESS )
#endif
    {
        EM_log(CK_LOG_WARNING, "[SerialIOManager] IOMasterPort returned %d\n", kernResult);
        goto error;
    }

// curious about how the version macro above works?
// in src/core/ try `make mac` (uses OS target, e.g., 1090) vs. `make vanilla` (uses current version 130000)
// uncomment the next three lines to print the preprocessor value at compile time
//#define STRING2(x) #x
//#define STRING(x) STRING2(x)
//#pragma message ("__MAC_OS_X_VERSION_MIN_REQUIRED = " STRING(__MAC_OS_X_VERSION_MIN_REQUIRED))

    if((classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue)) == NULL)
    {
        EM_log(CK_LOG_WARNING, "[SerialIOManager] IOServiceMatching returned NULL\n" );
        goto error;
    }
    CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDAllTypes));
    kernResult = IOServiceGetMatchingServices(mainPort, classesToMatch, &serialIterator);
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

#elif defined(__PLATFORM_LINUX__)


#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

vector<string> SerialIOManager::availableSerialDevices()
{
    vector<string> devices;

    // two sizes | 1.5.0.1 (ge) updated, as pointed out by linux compilation warning
    const int link_buf_size = 4096+256;
    const int path_buf_size = 4096;
    // was PATH_MAX, which apparently isn't
    // https://stackoverflow.com/questions/9449241/where-is-path-max-defined-in-linux
    // https://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
    // https://news.ycombinator.com/item?id=14194681
    const char * serial_dir = "/dev/serial/by-id";
    char link_buf[link_buf_size];
    char path_buf[path_buf_size];

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
        snprintf(link_buf, link_buf_size, "%s/%s", serial_dir, dir_info->d_name);
        int link_size = readlink(link_buf, path_buf, path_buf_size-1);
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
            snprintf(link_buf, link_buf_size, "%s/%s", serial_dir, path_buf);

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

#elif defined(__PLATFORM_WINDOWS__)

#include <windows.h>

using namespace std;

vector<string> SerialIOManager::availableSerialDevices()
{
    vector<string> devices;

    HKEY hSERIALCOMM;
    DWORD dwMaxValueNameLen, dwMaxValueLen;
    char *name, *value;

    DWORD dwIndex = 0;
    DWORD dwType;
    DWORD dwValueNameSize;
    DWORD dwDataSize;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hSERIALCOMM) != ERROR_SUCCESS)
    {
        EM_error3("SerialIOManager: unable to open registry key");
        goto error;
    }

    //Get the max value name and max value lengths
    if(RegQueryInfoKey(hSERIALCOMM, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL) != ERROR_SUCCESS)
    {
        EM_error3("SerialIOManager: unable to query registry key info");
        goto error;
    }

    name = new char[dwMaxValueNameLen+1];
    value = new char[dwMaxValueLen+1];

    dwIndex = 0;
    dwValueNameSize = dwMaxValueNameLen+1;
    dwDataSize = dwMaxValueLen+1;
    while(RegEnumValue(hSERIALCOMM, dwIndex, name, &dwValueNameSize, NULL, &dwType, (unsigned char *) value, &dwDataSize) == ERROR_SUCCESS)
    {
        if(dwType == REG_SZ)
        {
            devices.push_back(std::string(value));
        }

        dwIndex++;
        dwValueNameSize = dwMaxValueNameLen+1;
        dwDataSize = dwMaxValueLen+1;
    }

    return devices;

error:

    return devices;
}


#else // ? e.g., iPhone


// for compatibility | 1.5.2.5 (ge & eito) added std::
std::vector<std::string> SerialIOManager::availableSerialDevices()
{
    // vector of devices
    std::vector<std::string> devices;
    // return empty list
    return devices;
}


#endif // __PLATFORM_APPLE__
