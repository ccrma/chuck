#ifndef __CHUCK_ANDROID_H__
#define __CHUCK_ANDROID_H__
#ifdef __ANDROID__

#include <jni.h>
#include <stdio.h>

class ChuckAndroid {
public:
    static JavaVM *getJVM();
    static FILE *getTemporaryFile();
};

#endif // __ANDROID__
#endif // __CHUCK_ANDROID_H__