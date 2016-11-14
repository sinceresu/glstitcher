#include <jni.h>
#include <string>

extern "C"
jstring
Java_com_xiaoyi_sujin_glstitch_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_MainActivity_init
        (JNIEnv * env, jobject, jstring curDirectory) {

    const char* strDir = (*env).GetStringUTFChars(curDirectory, NULL);
    env->ReleaseStringUTFChars(curDirectory, strDir);


}