#ifdef __ANDROID__

#include "chuck_android.h"
#include <jni.h>
#include <cassert>
#include <memory>
#include <functional>

static JavaVM *jvm_instance = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jvm_instance = vm;

    return JNI_VERSION_1_6;
}

JavaVM *ChuckAndroid::getJVM()
{
    return jvm_instance;
}

FILE *ChuckAndroid::getTemporaryFile()
{
    JavaVM *jvm = getJVM();
    if( !jvm )
    {
        return NULL;
    }
    JNIEnv* env = NULL;
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