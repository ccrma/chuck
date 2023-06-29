/*----------------------------------------------------------------------------
 ChucK Concurrent, On-the-fly Audio Programming Language
 Compiler and Virtual Machine

 Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
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

#ifdef __PLATFORM_WIN32__
  #ifndef __CHUNREAL_ENGINE__
    #include <windows.h> // for win32_tmpfile()
  #else
    // 1.5.0.0 (ge) | #chunreal
    // unreal engine on windows disallows including windows.h
    #include "Windows/MinWindows.h"
  #endif // #ifndef __CHUNREAL_ENGINE__
#else // not windows
  #include <unistd.h>
#endif // #ifdef __PLATFORM_WIN32__

#include <stdio.h>
#include <sys/stat.h>




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
#ifdef __PLATFORM_WIN32__
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
#endif // #ifdef __PLATFORM_WIN32__




//-----------------------------------------------------------------------------
// name: ck_configANSI_ESCcodes() | 1.5.0.5 (ge) added
// desc: do any platform-specific setup to enable ANSI escape codes
//-----------------------------------------------------------------------------
t_CKBOOL ck_configANSI_ESCcodes()
{
#ifndef __PLATFORM_WIN32__
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
#ifndef __PLATFORM_WIN32__
    return isatty( fd ) != 0;
#else
    // windows, until we can figure this out
    return TRUE;
#endif
}
//-----------------------------------------------------------------------------
// get a general sense if current outputting to tty
//-----------------------------------------------------------------------------
t_CKBOOL ck_isatty()
{
    // use 2 for stderr
    return ck_isatty( 2 );
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
#ifdef __PLATFORM_WIN32__
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




#ifdef __PLATFORM_WIN32__
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
#ifndef __PLATFORM_WIN32__
    return getline( lineptr, n, stream );
#else
    return win32_getline( lineptr, n, stream );
#endif
}




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
