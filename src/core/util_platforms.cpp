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
#include <cassert>
#include <memory>
#include <functional>

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

FILE * ChuckAndroid::getTemporaryFile()
{
    JavaVM * jvm = getJVM();
    if( !jvm )
    {
        return NULL;
    }
    JNIEnv * env = NULL;
    bool needs_detaching = false;
    int reason = jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if( reason != JNI_OK && reason != JNI_EDETACHED )
    {
        return NULL;
    }
    if( reason == JNI_EDETACHED )
    {
        if( jvm->AttachCurrentThread(&env, NULL) != 0 )
        {
            return NULL;
        }
        needs_detaching = true;
    }

    std::unique_ptr<JavaVM, std::function<void(JavaVM *)>> guard(jvm,
        [needs_detaching](JavaVM *jvm)
        {
            if (needs_detaching)
            {
                jvm->DetachCurrentThread();
            }
        });

    assert( env != NULL );
    #define EXCEPTION_CHECK \
        if (env->ExceptionCheck()) \
        { \
            env->ExceptionDescribe(); \
            env->ExceptionClear(); \
            return NULL; \
        }

    // context = android.app.ActivityThread.currentActivityThread().getApplication()
    jclass thread_class = env->FindClass("android/app/ActivityThread");
    EXCEPTION_CHECK;
    jmethodID get_thread = env->GetStaticMethodID(
        thread_class, "currentActivityThread", "()Landroid/app/ActivityThread;");
    EXCEPTION_CHECK;
    jobject current_thread = env->CallStaticObjectMethod(thread_class, get_thread);
    EXCEPTION_CHECK;
    jmethodID get_application = env->GetMethodID(
        thread_class, "getApplication", "()Landroid/app/Application;");
    EXCEPTION_CHECK;
    jobject context = env->CallObjectMethod(current_thread, get_application);
    EXCEPTION_CHECK;

    // cache_dir = context.getCacheDir()
    jclass context_class = env->FindClass("android/content/Context");
    EXCEPTION_CHECK;
    jmethodID get_cache_dir = env->GetMethodID(context_class, "getCacheDir", "()Ljava/io/File;");
    EXCEPTION_CHECK;
    jobject cache_dir = env->CallObjectMethod(context, get_cache_dir);
    EXCEPTION_CHECK;

    // temp_file = cache_dir.createTempFile("chuck_temp", "", cache_dir)
    jclass file_class = env->FindClass("java/io/File");
    EXCEPTION_CHECK;
    jmethodID create_temp_file = env->GetStaticMethodID(file_class, "createTempFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/io/File;)Ljava/io/File;");
    EXCEPTION_CHECK;
    jobject temp_file = env->CallStaticObjectMethod(file_class, create_temp_file,
        env->NewStringUTF("chuck_temp"), env->NewStringUTF(""), cache_dir);
    EXCEPTION_CHECK;

    // temp_file_path = temp_file.getPath()
    jmethodID get_path = env->GetMethodID(file_class, "getPath", "()Ljava/lang/String;");
    EXCEPTION_CHECK;
    jstring temp_file_path = (jstring)env->CallObjectMethod(temp_file, get_path);
    EXCEPTION_CHECK;

    const char *path_chars = env->GetStringUTFChars(temp_file_path, NULL);
    FILE *ret = fopen(path_chars, "wb+D");
    env->ReleaseStringUTFChars(temp_file_path, path_chars);

    return ret;
}

#endif // __ANDROID__
