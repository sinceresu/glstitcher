/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_xiaoyi_sujin_glstitch_StitchJNIWrapper */

#ifndef _Included_com_xiaoyi_sujin_glstitch_StitchJNIWrapper
#define _Included_com_xiaoyi_sujin_glstitch_StitchJNIWrapper
#ifdef __cplusplus
extern "C" {
#endif
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_NONE
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_NONE -1L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_STD
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_STD 0L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_STD_MIRRORED
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_STD_MIRRORED 1L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_FLIPPED
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_FLIPPED 2L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_FLIPPED_MIRRORED
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_FLIPPED_MIRRORED 3L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_DIAGONAL
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_ORIENTATION_DIAGONAL 4L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_RENDER_DISP_MODE_INPUT
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_RENDER_DISP_MODE_INPUT 0L
#undef com_xiaoyi_sujin_glstitch_StitchJNIWrapper_RENDER_DISP_MODE_OUTPUT
#define com_xiaoyi_sujin_glstitch_StitchJNIWrapper_RENDER_DISP_MODE_OUTPUT 1L
/*
 * Class:     com_xiaoyi_sujin_glstitch_StitchJNIWrapper
 * Method:    init
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_init
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_xiaoyi_sujin_glstitch_StitchJNIWrapper
 * Method:    prepareInput
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_prepareInput
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_xiaoyi_sujin_glstitch_StitchJNIWrapper
 * Method:    prepareOutput
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_prepareOutput
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_xiaoyi_sujin_glstitch_StitchJNIWrapper
 * Method:    cleanup
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_cleanup
  (JNIEnv *, jobject);

/*
 * Class:     com_xiaoyi_sujin_glstitch_StitchJNIWrapper
 * Method:    process
 * Signature: ([I[I)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_process
  (JNIEnv *, jobject, jintArray, jintArray);

JNIEXPORT int JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_getProcessTime
        (JNIEnv *env, jobject);
#ifdef __cplusplus
}
#endif
#endif