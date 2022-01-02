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
// desc: platform-specific utilities, e.g., tmpfile() for Android etc.
//
// author: Andriy Kunitsyn (kunitzin@gmail.com) original ChuckAndroid
//         Ge Wang (https://ccrma.stanford.edu/~ge/) added util_platforms.*
// date: Summer 2021
//-----------------------------------------------------------------------------
#include "util_platforms.h"




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
