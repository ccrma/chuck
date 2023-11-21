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
// name: util_platforms.cpp
// desc: platform-specific utilities, e.g., for Android and various
//
// author: Andriy Kunitsyn (kunitzin@gmail.com) | original ChuckAndroid
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Summer 2021
//-----------------------------------------------------------------------------
#include "util_platforms.h"
#include "util_string.h"
#include "chuck_errmsg.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef __PLATFORM_WINDOWS__

  #ifndef __CHUNREAL_ENGINE__
    #include <windows.h> // for win32_tmpfile()
  #else
    // 1.5.0.0 (ge) | #chunreal
    // unreal engine on windows disallows including windows.h
    #include "Windows/MinWindows.h"
  #endif // #ifndef __CHUNREAL_ENGINE__
  #include <io.h> // for _isatty()

#else // not windows

  #include <unistd.h>
  #include <sys/ioctl.h>

#endif // #ifdef __PLATFORM_WINDOWS__



//-----------------------------------------------------------------------------
// name: ck_openFileAutoExt()
// desc: open a file by name, first as is, and if needed concatenateing ".ck"
//       this potentially modifies the contents of fname
//-----------------------------------------------------------------------------
FILE * ck_openFileAutoExt( std::string & filenameMutable,
                           const std::string & extension )
{
#ifdef __ANDROID__
    if( strncmp(filenameMutable.c_str(), "jar:", strlen("jar:")) == 0 )
    {
        int fd = 0;
        if( !ChuckAndroid::copyJARURLFileToTemporary(filenameMutable.c_str(), &fd) )
        {
            EM_error2( 0, "unable to download from JAR URL: %s", filenameMutable.c_str() );
            return NULL;
        }
        return fdopen(fd, "rb");
    }
#endif // __ANDROID__

    // try opening
    FILE * fd = fopen( filenameMutable.c_str(), "rb" );
    // file open?
    if( !fd )
    {
        // check if filename has extension, ignoring case
        if( !extension_matches( filenameMutable, extension, TRUE ) )
        {
            // concat
            std::string tryThis = filenameMutable + extension;
            // try opening again
            fd = fopen( tryThis.c_str(), "rb" );
            // if successful update filename
            filenameMutable = tryThis;
        }
    }

    return fd;
}




//-----------------------------------------------------------------------------
// name: win32_tmpfile()
// desc: replacement for broken tmpfile() on Windows Vista + 7
//-----------------------------------------------------------------------------
#ifdef __PLATFORM_WINDOWS__
FILE * win32_tmpfile()
{
    char tmp_path[MAX_PATH];
    char file_path[MAX_PATH];
    FILE * file = NULL;

#ifndef __CHUNREAL_ENGINE__
    if( GetTempPath(256, tmp_path) == 0 )
#else
    // 1.5.0.0 (ge) | #chunreal explicit call ASCII version
    if( GetTempPathA(256, tmp_path) == 0 )
#endif
        return NULL;

#ifndef __CHUNREAL_ENGINE__
    if( GetTempFileName(tmp_path, "mA", 0, file_path) == 0 )
#else
    // 1.5.0.0 (ge) | #chunreal explicit call ASCII version
    if( GetTempFileNameA(tmp_path, "mA", 0, file_path) == 0 )
#endif
        return NULL;

    file = fopen( file_path, "wb+D" );

    return file;
}
#endif // #ifdef __PLATFORM_WINDOWS__




//-----------------------------------------------------------------------------
// name: ck_configANSI_ESCcodes() | 1.5.0.5 (ge) added
// desc: do any platform-specific setup to enable ANSI escape codes
//-----------------------------------------------------------------------------
t_CKBOOL ck_configANSI_ESCcodes()
{
#ifndef __PLATFORM_WINDOWS__
#else
    // get stderr handle; do this first as chuck tends to write to stderr
    HANDLE winStderr = GetStdHandle( STD_ERROR_HANDLE );
    // check handle
    if( winStderr == NULL || winStderr == INVALID_HANDLE_VALUE ) return FALSE;
    // set windows console mode to process escape sequences + enable color terminal output
    SetConsoleMode( winStderr, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING );

    // get stdout handle
    HANDLE winStdout = GetStdHandle( STD_OUTPUT_HANDLE );
    // check handle
    if( winStdout == NULL || winStdout == INVALID_HANDLE_VALUE ) return FALSE;
    // set windows console mode to process escape sequences + enable color terminal output
    SetConsoleMode( winStdout, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING );
#endif

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ck_isatty() | 1.5.0.5 (ge) added
// desc: are we output to a TTY (teletype, character by character)
//       (vs say a file stream; helpful for determining if we should
//       suppress printing ANSI escapes codes, e.g., color codes
//       to the output stream
//-----------------------------------------------------------------------------
t_CKBOOL ck_isatty( int fd )
{
#if defined(__PLATFORM_WINDOWS__)
    return _isatty( fd ) != 0;
#else
    return isatty( fd ) != 0;
#endif
}
//-----------------------------------------------------------------------------
// get a general sense if currently outputting to TTY
//-----------------------------------------------------------------------------
t_CKBOOL ck_isatty()
{
    // let's test stderr, since much of chuck operates over it
#if defined(__PLATFORM_WINDOWS__)
    return ck_isatty( _fileno(stderr) );
#elif defined(__PLATFORM_EMSCRIPTEN__)
    // emscripten always seems to return TRUE for isatty()
    // but then ioctl() does not seem to be working / implemented
    // so returning FALSE for now | use chuck param TTY_WIDTH_HINT
    return FALSE;
#else
    return ck_isatty( fileno(stderr) );
#endif
}




// current dfeault
static t_CKUINT g_ttywidth_default = 80;
//-----------------------------------------------------------------------------
// name: ck_ttywidth() | 1.5.0.5 (ge) added
// desc: get TTY terminal width, or CK_DEFAULT_TTY_WIDTH if not TTY
//-----------------------------------------------------------------------------
t_CKUINT ck_ttywidth()
{
    // return default if not TTY
    if( !ck_isatty() ) return g_ttywidth_default;

#ifndef __PLATFORM_WINDOWS__
    // a struct to hold return value
    struct winsize w;
    // zero out, as ioctl() can return 0 but not modify w (et tu emscripten?)
    memset( &w, 0, sizeof(w) );
    // get the window size
    if( ioctl( fileno(stderr), TIOCGWINSZ, &w ) != 0 ) { goto error; }
    // return
    return w.ws_col;
#else
    // get windows handle to stderr
    HANDLE hStderr = GetStdHandle( STD_ERROR_HANDLE );
    // console buffer info
    CONSOLE_SCREEN_BUFFER_INFO info;
    // get screen buffer info
    if( !GetConsoleScreenBufferInfo( hStderr, &info ) ) { goto error; }
    // return width
    return info.dwSize.X;
#endif

error:
    return g_ttywidth_default;
}




//-----------------------------------------------------------------------------
// set default TTY width
//-----------------------------------------------------------------------------
void ck_ttywidth_setdefault( t_CKUINT width )
{
    g_ttywidth_default = width;
}
//-----------------------------------------------------------------------------
// get default TTY width
//-----------------------------------------------------------------------------
t_CKUINT ck_ttywidth_default()
{
    return g_ttywidth_default;
}




//-----------------------------------------------------------------------------
// name: android_tmpfile()
// desc: replacement for broken tmpfile() on Android
//-----------------------------------------------------------------------------
#ifdef __ANDROID__
FILE * android_tmpfile()
{
    return ChuckAndroid::getTemporaryFile();
}
#endif




//-----------------------------------------------------------------------------
// name: android_tmpfile()
// desc: replacement for broken tmpfile() on Android
//-----------------------------------------------------------------------------
FILE * ck_tmpfile()
{
    FILE * fd = NULL;
#ifdef __PLATFORM_WINDOWS__
    fd = win32_tmpfile();
#elif defined (__ANDROID__)
    fd = android_tmpfile();
#else
    fd = tmpfile();
#endif

    return fd;
}




//-----------------------------------------------------------------------------
// name: ck_isdir()
// desc: check if file is a directory
//-----------------------------------------------------------------------------
t_CKBOOL ck_isdir( const std::string & path )
{
    // shuttle
    struct stat fs;
    // stat the path and check flag (is path not there, also returns false)
    return (stat( path.c_str(), &fs )==0 && fs.st_mode & S_IFDIR);
}




#ifdef __PLATFORM_WINDOWS__
//-----------------------------------------------------------------------------
// name: win32_getline()
// desc: C getline using a FILE descriptor
// thanks be to random people on the internet
// https://stackoverflow.com/questions/735126/are-there-alternate-implementations-of-gnu-getline-interface/735472#735472
// using version from Song "from Malaysia": "Better Answer with no Bug Here:"
//-----------------------------------------------------------------------------
size_t win32_getline( char ** lineptr, size_t * n, FILE * stream )
{
    char * bufptr = NULL;
    char * p = bufptr;
    size_t size;
    int c;

    if( lineptr == NULL )
    {
        return -1;
    }
    if( stream == NULL )
    {
        return -1;
    }
    if( n == NULL )
    {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc( stream );
    if( c == EOF )
    {
        return -1;
    }
    if( bufptr == NULL )
    {
        bufptr = (char *)malloc( 128 );
        if( bufptr == NULL )
        {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while( c != EOF )
    {
        if( (p - bufptr) > (size - 1) )
        {
            size = size + 128;
            bufptr = (char *)realloc( bufptr, size );
            if( bufptr == NULL )
            {
                return -1;
            }
            p = bufptr + (size - 128);
        }
        *p++ = c;
        if( c == '\n' )
        {
            break;
        }
        c = fgetc( stream );
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif // __PLATFORM_WIN32





//-----------------------------------------------------------------------------
// name: ck_getline()
// desc: redirect for c edition of getline
//-----------------------------------------------------------------------------
size_t ck_getline( char ** lineptr, size_t * n, FILE * stream )
{
#ifndef __PLATFORM_WINDOWS__
    return getline( lineptr, n, stream );
#else
    return win32_getline( lineptr, n, stream );
#endif
}




//-----------------------------------------------------------------------------
// name: ck_usleep()
// desc: usleep in microseconds
//-----------------------------------------------------------------------------
void ck_usleep( t_CKUINT microseconds )
{
#if defined(__PLATFORM_WINDOWS__) && !defined(usleep)
  #ifndef __CHUNREAL_ENGINE__
    Sleep( microseconds / 1000 <= 0 ? 1 : microseconds / 1000 );
  #else
    // 1.5.0.0 (ge) | #chunreal
    FPlatformProcess::Sleep( microseconds/1000000.0f <= 0 ? .001 : microseconds/1000000.0f );
  #endif // #ifndef __UNREAL_ENGINE__
#else
    // call system usleep
    usleep( (useconds_t)microseconds );
#endif
}




//-----------------------------------------------------------------------------
#if defined(__PLATFORM_APPLE__) && !defined(__CHIP_MODE__)
//-----------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_time.h> // for mach_absolute_time | 1.5.0.5


//-----------------------------------------------------------------------------
// name: ck_darwin_version()
// desc: get darwin release version (also see ck_macOS_version())
// https://stackoverflow.com/questions/11072804/how-do-i-determine-the-os-version-at-runtime-in-os-x-or-ios-without-using-gesta
//-----------------------------------------------------------------------------
ck_OSVersion ck_darwin_version()
{
    // version
    ck_OSVersion ver;

    // get darwin release number
    char str[256];
    size_t size = sizeof(str);
    // get darwin kernel release version
    // note: sysctl() is supported on macOS; no longer in linux kernels
    int ret = sysctlbyname( "kern.osrelease", str, &size, NULL, 0 );

    // error
    if( ret ) return ver;

    // read it in
    sscanf( str, "%hd.%hd.%hd", &ver.major, &ver.minor, &ver.patch );

    // return version
    return ver;
}


//-----------------------------------------------------------------------------
// name: ck_macOS_version()
// desc: get macOS release version
// note: alternative for Gestalt() which is deprecated
// https://stackoverflow.com/questions/11072804/how-do-i-determine-the-os-version-at-runtime-in-os-x-or-ios-without-using-gesta
// https://en.wikipedia.org/wiki/Darwin_(operating_system)
// Darwin operating system version to macOS release version
// 22.x.x. macOS 13.x.x Ventura
// 21.x.x. macOS 12.x.x Monterey
// 20.x.x. macOS 11.x.x Big Sur
// 19.x.x. macOS 10.15.x Catalina
// 18.x.x. macOS 10.14.x Mojave
// 17.x.x. macOS 10.13.x High Sierra
// 16.x.x  macOS 10.12.x Sierra
// 15.x.x  OS X  10.11.x El Capitan
// 14.x.x  OS X  10.10.x Yosemite
// 13.x.x  OS X  10.9.x  Mavericks
// 12.x.x  OS X  10.8.x  Mountain Lion
// 11.x.x  OS X  10.7.x  Lion
// 10.x.x  OS X  10.6.x  Snow Leopard
//  9.x.x  OS X  10.5.x  Leopard
//  8.x.x  OS X  10.4.x  Tiger
//  7.x.x  OS X  10.3.x  Panther
//  6.x.x  OS X  10.2.x  Jaguar
//  5.x    OS X  10.1.x  Puma
//  4.x    OS X  10.0.x  Cheetah
//-----------------------------------------------------------------------------
ck_OSVersion ck_macOS_version()
{
    // version
    ck_OSVersion ver;

    // first try to get macOS release number directly
    // FYI this may not work in older versions, circa before 2018
    char str[256];
    size_t size = sizeof(str);
    // get darwin kernel release version
    // note: sysctl() is supported on macOS; no longer in linux kernels
    int ret = sysctlbyname( "kern.osproductversion", str, &size, NULL, 0 );

    // success
    if( ret == 0 )
    {
        // read it in
        sscanf( str, "%hd.%hd.%hd", &ver.major, &ver.minor, &ver.patch );
        // done
        return ver;
    }

    // fall back -- get darwin version and our own conversion
    // FYI darwin minor version could be off by 1 due to security patches
    ver = ck_darwin_version();

    // test
    if( ver.major < 4 ) // before 10.1.x Puma
    {
        // reset to 0
        ver.reset();
        // return 0 version
        return ver;
    }

    // convert to macOS release number
    if( ver.major <= 19 )
    {
        // still in OS X era
        ver.minor = ver.major - 4;
        ver.major = 10;
    }
    else
    {
        // hopefully this is future proof; depends on Apple release versioning
        ver.major = ver.major - 9;
    }

    // return version
    return ver;
}


//-----------------------------------------------------------------------------
// name: str()
// desc: get version as string
//-----------------------------------------------------------------------------
std::string ck_OSVersion::str()
{
    return ck_itoa(major) + "." + ck_itoa(minor) + "." + ck_itoa(patch);
}


//-----------------------------------------------------------------------------
// name: ck_macOS_uptime() | 1.5.0.5 (ge) added
// desc: return macOS uptime
// note: replacement for UpTime(), which is deprecated
// https://stackoverflow.com/questions/47168914/absolutetonanoseconds-uptime-deprecated
// https://opensource.apple.com/source/Libc/Libc-1158.1.2/gen/clock_gettime.c.auto.html
// clock_gettime_nsec_np(CLOCK_UPTIME_RAW) doesn't seem be the same as UpTime
// at least on apple silicon macOS 13 (Ventura)
//-----------------------------------------------------------------------------
AbsoluteTime ck_macOS_uptime()
{
    uint64_t mt = mach_absolute_time();
    AbsoluteTime at;
    at.hi = (uint32_t)(mt << 32);
    at.lo = (uint32_t)(mt & 0xffffffff);
    return at;
}


/*
static void testUpTime()
{
    uint64_t mt = mach_absolute_time();
    AbsoluteTime at = UpTime();
    uint64_t t = ((uint64_t)at.hi << 32) + at.lo;

    AbsoluteTime at2;
    at2.hi = mt >> 32;
    at2.lo = mt & 0xffffffff;
    uint64_t t2 = ((uint64_t)at2.hi << 32) + at2.lo;

    cerr << "TIME:" << clock_gettime_nsec_np(CLOCK_UPTIME_RAW) / 1000000 << " mach: " << mt << " UpTime: " << t2 << endl;
}
*/


#endif // #if defined(__PLATFORM_APPLE__) && !defined(__CHIP_MODE__)




//-----------------------------------------------------------------------------
#ifdef __ANDROID__
//-----------------------------------------------------------------------------
#include <jni.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

static JavaVM * jvm_instance = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM * vm, void * reserved )
{
    jvm_instance = vm;

    return JNI_VERSION_1_6;
}

JavaVM * ChuckAndroid::getJVM()
{
    return jvm_instance;
}

//-----------------------------------------------------------------------------
// name: class JNIEnvWrapper
// desc: Wrapper around JNIEnv. Serves three purposes:
//       - releases JNIEnv and Java objects in RAII fashion
//       - removes the need to get method IDs
//       - throws JNIEnvWrapperException exception on Java exceptions
//-----------------------------------------------------------------------------
class JNIEnvWrapper
{
public:
    JNIEnvWrapper(JavaVM * jvm);
    ~JNIEnvWrapper();

    bool IsValid() const;

    jclass FindClass(const char * class_signature);
    jobject ConstructNewObject(jclass constructed_class,
        const char * constructor_signature, jobject param_1);
    jobject CallObjectMethod(jobject callee, jclass callee_class,
        const char * method_name, const char * method_signature);
    jobject CallObjectMethod(jobject callee, jclass callee_class,
        const char * method_name, const char * method_signature,
        jobject param_1);
    jobject CallStaticObjectMethod(jclass callee_class,
        const char * method_name, const char * method_signature);
    jobject CallStaticObjectMethod(jclass callee_class,
        const char * method_name, const char * method_signature,
        jobject param_1);
    jobject CallStaticObjectMethod(jclass callee_class,
        const char * method_name, const char * method_signature,
        jobject param_1, jobject param_2, jobject param_3);
    jint CallIntMethod(jobject callee, jclass callee_class,
        const char * method_name, const char * method_signature,
        jobject param_1);
    void CallVoidMethod(jobject callee, jclass callee_class,
        const char * method_name, const char * method_signature);

    jstring NewStringUTF(const char * utf8_string);
    const char * GetStringUTFChars(jstring string);
    void ReleaseStringUTFChars(jstring string, const char * utf8_chars);
    jbyteArray NewByteArray(size_t size);
    jbyte * GetByteArrayElements(jbyteArray java_array);
    void ReleaseByteArrayElements(jbyteArray java_array, jbyte * native_array);

private:
    void ThrowOnJavaException();
    JavaVM * m_jvm = NULL;
    JNIEnv * m_env = NULL;
    bool m_needs_detaching = false;
};

class JNIEnvWrapperException {};

JNIEnvWrapper::JNIEnvWrapper(JavaVM * jvm)
    : m_jvm(jvm)
{
    int reason = jvm->GetEnv((void **)&m_env, JNI_VERSION_1_6);
    if( reason != JNI_OK && reason != JNI_EDETACHED )
    {
        m_env = NULL;
        return;
    }
    if( reason == JNI_EDETACHED )
    {
        if( jvm->AttachCurrentThread(&m_env, NULL) != 0 )
        {
            m_env = NULL;
            return;
        }
        m_needs_detaching = true;
    }
    if( m_env->PushLocalFrame(32) != 0 )
    {
        if( m_needs_detaching )
        {
            m_jvm->DetachCurrentThread();
        }
        m_env = NULL;
    }
}

JNIEnvWrapper::~JNIEnvWrapper()
{
    if( m_env == NULL )
    {
        return;
    }
    m_env->PopLocalFrame(NULL);
    if( m_needs_detaching )
    {
        m_jvm->DetachCurrentThread();
    }
}

bool JNIEnvWrapper::IsValid() const
{
    return m_env != NULL;
}

void JNIEnvWrapper::ThrowOnJavaException()
{
    if( m_env->ExceptionCheck() )
    {
        m_env->ExceptionDescribe();
        m_env->ExceptionClear();
        throw JNIEnvWrapperException();
    }
}

jclass JNIEnvWrapper::FindClass(const char * class_signature)
{
    jclass ret = m_env->FindClass(class_signature);
    ThrowOnJavaException();
    return ret;
}

jobject JNIEnvWrapper::ConstructNewObject(jclass constructed_class,
    const char * constructor_signature, jobject param_1)
{
    jmethodID method_id = m_env->GetMethodID(constructed_class,
        "<init>", constructor_signature);
    ThrowOnJavaException();
    jobject ret = m_env->NewObject(constructed_class, method_id,
        param_1);
    ThrowOnJavaException();
    return ret;
}

jobject JNIEnvWrapper::CallObjectMethod(jobject callee, jclass callee_class,
    const char * method_name, const char * method_signature)
{
    jmethodID method_id = m_env->GetMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    jobject ret = m_env->CallObjectMethod(callee, method_id);
    ThrowOnJavaException();
    return ret;
}

jobject JNIEnvWrapper::CallObjectMethod(jobject callee, jclass callee_class,
    const char * method_name, const char * method_signature,
    jobject param_1)
{
    jmethodID method_id = m_env->GetMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    jobject ret = m_env->CallObjectMethod(callee, method_id, param_1);
    ThrowOnJavaException();
    return ret;
}

jobject JNIEnvWrapper::CallStaticObjectMethod(jclass callee_class,
    const char * method_name, const char * method_signature)
{
    jmethodID method_id = m_env->GetStaticMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    jobject ret = m_env->CallStaticObjectMethod(callee_class, method_id);
    ThrowOnJavaException();

    return ret;
}

jobject JNIEnvWrapper::CallStaticObjectMethod(jclass callee_class,
    const char * method_name, const char * method_signature, jobject param_1)
{
    jmethodID method_id = m_env->GetStaticMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    jobject ret = m_env->CallStaticObjectMethod(callee_class,
        method_id, param_1);
    ThrowOnJavaException();

    return ret;
}

jobject JNIEnvWrapper::CallStaticObjectMethod(jclass callee_class,
    const char * method_name, const char * method_signature, jobject param_1,
    jobject param_2, jobject param_3)
{
    jmethodID method_id = m_env->GetStaticMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    jobject ret = m_env->CallStaticObjectMethod(callee_class, method_id,
        param_1, param_2, param_3);
    ThrowOnJavaException();

    return ret;
}

jint JNIEnvWrapper::CallIntMethod(jobject callee, jclass callee_class,
    const char * method_name, const char * method_signature, jobject param_1)
{
    jmethodID method_id = m_env->GetMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    jint ret = m_env->CallIntMethod(callee, method_id, param_1);
    ThrowOnJavaException();
    return ret;
}

void JNIEnvWrapper::CallVoidMethod(jobject callee, jclass callee_class,
    const char * method_name, const char * method_signature)
{
    jmethodID method_id = m_env->GetMethodID(callee_class,
        method_name, method_signature);
    ThrowOnJavaException();
    m_env->CallVoidMethod(callee, method_id);
    ThrowOnJavaException();
}

jstring JNIEnvWrapper::NewStringUTF(const char * utf8_string)
{
    jstring ret = m_env->NewStringUTF(utf8_string);
    ThrowOnJavaException();

    return ret;
}

const char * JNIEnvWrapper::GetStringUTFChars(jstring string)
{
    return m_env->GetStringUTFChars(string, NULL);
}

void JNIEnvWrapper::ReleaseStringUTFChars(jstring string, const char * utf8_chars)
{
    m_env->ReleaseStringUTFChars(string, utf8_chars);
}

jbyteArray JNIEnvWrapper::NewByteArray(size_t size)
{
    jbyteArray ret = m_env->NewByteArray(size);
    ThrowOnJavaException();

    return ret;
}

jbyte * JNIEnvWrapper::GetByteArrayElements(jbyteArray java_array)
{
    return m_env->GetByteArrayElements(java_array, NULL);
}

void JNIEnvWrapper::ReleaseByteArrayElements(jbyteArray java_array,
    jbyte * native_array)
{
    m_env->ReleaseByteArrayElements(java_array, native_array, 0);
}

static std::string GetTemporaryFilePath(JNIEnvWrapper& jni)
{
    // context = android.app.ActivityThread.currentActivityThread().getApplication()
    jclass thread_class = jni.FindClass("android/app/ActivityThread");
    jobject current_thread = jni.CallStaticObjectMethod(thread_class,
        "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject context = jni.CallObjectMethod(current_thread, thread_class,
        "getApplication", "()Landroid/app/Application;");

    // cache_dir = context.getCacheDir()
    jclass context_class = jni.FindClass("android/content/Context");
    jobject cache_dir = jni.CallObjectMethod(context, context_class,
        "getCacheDir", "()Ljava/io/File;");

    // temp_file = java.io.File.createTempFile("chuck_temp", "", cache_dir)
    jclass file_class = jni.FindClass("java/io/File");
    jobject temp_file = jni.CallStaticObjectMethod(file_class, "createTempFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/io/File;)Ljava/io/File;",
        jni.NewStringUTF("chuck_temp"), jni.NewStringUTF(""), cache_dir);

    // temp_file_path = temp_file.getPath()
    jstring temp_file_path = (jstring)jni.CallObjectMethod(temp_file,
        file_class, "getPath", "()Ljava/lang/String;");

    const char * path_chars = jni.GetStringUTFChars(temp_file_path);
    std::string ret = path_chars;
    jni.ReleaseStringUTFChars(temp_file_path, path_chars);

    return ret;
}

FILE * ChuckAndroid::getTemporaryFile() noexcept
{
    JavaVM * jvm = getJVM();
    if( !jvm )
    {
        return NULL;
    }
    JNIEnvWrapper jni(jvm);
    if( !jni.IsValid() )
    {
        return NULL;
    }

    try
    {
        const std::string temp_path = GetTemporaryFilePath(jni);
        return fopen(temp_path.c_str(), "wb+");
    }
    catch (JNIEnvWrapperException)
    {
        return NULL;
    }
}

bool ChuckAndroid::copyJARURLFileToTemporary(const char * jar_url, int * fd)
    noexcept
{
    JavaVM * jvm = getJVM();
    if( !jvm )
    {
        return false;
    }
    JNIEnvWrapper jni(jvm);
    if( !jni.IsValid() )
    {
        return false;
    }

    int ret = 0;
    try
    {
        const std::string temp_path = GetTemporaryFilePath(jni);
        ret = open(temp_path.c_str(), O_RDWR | O_CREAT, 0600);
        if( ret == -1 )
        {
            return false;
        }
        // do not pollute temp directory
        if( unlink(temp_path.c_str()) == -1 )
        {
            return false;
        }

        // url = new java.net.URL(jar_url)
        jclass url_class = jni.FindClass("java/net/URL");
        jobject url = jni.ConstructNewObject(url_class,
            "(Ljava/lang/String;)V", jni.NewStringUTF(jar_url));

        // connection = url.openConnection()
        jobject connection = jni.CallObjectMethod(url, url_class,
            "openConnection", "()Ljava/net/URLConnection;");

        // jar_file = (java.net.JarURLConnection)connection.getJarFile()
        jclass jar_url_connection_class = jni.FindClass(
            "java/net/JarURLConnection");
        jobject jar_file = jni.CallObjectMethod(connection,
            jar_url_connection_class, "getJarFile",
            "()Ljava/util/jar/JarFile;");

        // jar_entry = connection.getJarEntry()
        jobject jar_entry = jni.CallObjectMethod(connection,
            jar_url_connection_class, "getJarEntry",
            "()Ljava/util/jar/JarEntry;");

        // input_stream = jar_file.getInputStream(jar_entry)
        jclass jar_file_class = jni.FindClass("java/util/jar/JarFile");
        jobject input_stream = jni.CallObjectMethod(jar_file, jar_file_class,
            "getInputStream", "(Ljava/util/zip/ZipEntry;)Ljava/io/InputStream;",
            jar_entry);

        // buffer = new byte[8 * 1024]
        jbyteArray java_buffer = jni.NewByteArray(8 * 1024);
        // s = input_stream.read(buffer)
        jclass input_stream_class = jni.FindClass("java/io/InputStream");
        jint s = 0;
        while( true )
        {
            s = jni.CallIntMethod(input_stream, input_stream_class, "read",
                "([B)I", java_buffer);
            if( s < 0 )
            {
                break;
            }
            jbyte * native_buffer = jni.GetByteArrayElements(java_buffer);
            size_t written = 0;
            while( written < s )
            {
                ssize_t written_now = write(ret, native_buffer + written,
                    s - written);
                if( written_now < 0 )
                {
                    close(ret);
                    return false;
                }
                written += written_now;
            }
            jni.ReleaseByteArrayElements(java_buffer, native_buffer);
        }

        // jar_file.close()
        jni.CallVoidMethod(jar_file, jar_file_class, "close", "()V");
    }
    catch (JNIEnvWrapperException)
    {
        if( ret != -1 )
        {
            close(ret);
        }
        return false;
    }

    lseek(ret, 0, SEEK_SET);
    *fd = ret;
    return true;
}




#endif // __ANDROID__
