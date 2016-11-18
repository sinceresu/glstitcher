//
// Created by sujin on 2016/11/10.
//

#include "../StitchJNIWrapper.h"
#include "types.h"
#include "GLStitcher.h"
#include <time.h>

void RGB2YUVRevert(unsigned char *yuvBuf, unsigned char *rgbBuf, int w, int h, int stride)
{
    int i, j;
    int r,g,b,y,u,v;
    unsigned char *pYuvBuf = yuvBuf + stride * (h - 1);
    //uint8_t * line = img + stride * (h - 1);
    int fy;
    int stride_rgb = w * 4;
    for (int i = 0; i<h; i++) {
        for (int j = 0, k = 0; j < 3 * w; j += 3, k += 4) {
            y = pYuvBuf[j];
            u = pYuvBuf[j + 1];
            v = pYuvBuf[j + 2];

            y -= 16;
            u -= 128;
            v -= 128;

            fy = 1192*y;

            r = ((fy           + 1634*v + 512)>>10);
            g = ((fy - 401*u - 833*v + 512)>>10);
            b = ((fy + 2065*u + 512)>>10);

            rgbBuf[k + 2] = (b>255) ? 255 : ((b<0) ? 0 : b);
            rgbBuf[k + 1] = (g>255) ? 255 : ((g<0) ? 0 : g);
            rgbBuf[k] = (r>255) ? 255 : ((r<0) ? 0 : r);
            rgbBuf[k + 3] = 255;
         }
        pYuvBuf -= stride;
        rgbBuf += stride_rgb;
        //	fwrite(img+(stride*(h-i-1)),3,w,f);
    }
}

void RGBA2RGBARevert(unsigned char *inputBuf, unsigned char *outputBuf, int w, int h, int stride)
{
    int i, j;
    int r,g,b,y,u,v;
    unsigned char *pInputBuf = inputBuf + stride * (h - 1);
    //uint8_t * line = img + stride * (h - 1);
    int fy;
    int stride_rgb = w * 4;
    for (int i = 0; i<h; i++) {
        for (int j = 0, k = 0; j < 4 * w; j += 4, k += 4) {
            outputBuf[k + 2] = pInputBuf[j];
            outputBuf[k + 1] = pInputBuf[j + 1];
            outputBuf[k] = pInputBuf[j + 2];
            outputBuf[k + 3] = 255;
        }
        pInputBuf -= stride;
        outputBuf += stride_rgb;
        //	fwrite(img+(stride*(h-i-1)),3,w,f);
    }
}


int RGB2YUV(unsigned char *rgbBuf, unsigned char *yuvBuf, int buf_block)
{
    // rgbBuf's data order is interleaved
    // yuvBuf's data order is separated
    int i, j;
    int r,g,b,y,u,v;
    unsigned char *pyBuf = yuvBuf;
    for (i = 0, j = 0; i < buf_block; i += 4, j += 4) {
        b = rgbBuf[i];
        g = rgbBuf[i + 1];
        r = rgbBuf[i + 2];

        y = (( 263*r + 516*g + 100*b + 16896)>>10);
        u = ((-152*r - 298*g + 450*b + 131584)>>10);
        v =  ((450*r - 377*g - 73*b + 131584)>>10);

        pyBuf[j] = (y>255) ? 255 : ((y<0) ? 0 : y);
        pyBuf[j + 1] = (u>255) ? 255 : ((u<0) ? 0 : u);
        pyBuf[j + 2] = (v>255) ? 255 : ((v<0) ? 0 : v);
        pyBuf[j + 3] = 255;
    }
    return 0;
}

int YUV2RGB(unsigned char *yuvBuf, unsigned char *rgbBuf, int buf_block)
{
    // yuvBuf's data order is separated
    // rgbBuf's data order is interleaved
    int i, j;
    int r,g,b,y,u,v;

    int fy;

    unsigned char *prgbBuf = rgbBuf ;

    for(i = 0, j = 0; i < buf_block; i+=4, j+=4)
    {
        y = yuvBuf[i];
        u = yuvBuf[i + 1];
        v = yuvBuf[i + 2];

        y -= 16;
        u -= 128;
        v -= 128;

        fy = 1192*y;

        r = ((fy           + 1634*v + 512)>>10);
        g = ((fy - 401*u - 833*v + 512)>>10);
        b = ((fy + 2065*u + 512)>>10);

        rgbBuf[j] = (b>255) ? 255 : ((b<0) ? 0 : b);
        rgbBuf[j + 1] = (g>255) ? 255 : ((g<0) ? 0 : g);
        rgbBuf[j + 2] = (r>255) ? 255 : ((r<0) ? 0 : r);
        rgbBuf[j + 3] = 255;

    }

    return 0;
}


static jlong outputPxBufNumBytes = 0;			// number of bytes in output buffer
static jobject outputPxBuf = NULL;				// DirectByteBuffer object pointing to <outputPxBufData>
static unsigned char *outputPxBufData = NULL;	// pointer to data in DirectByteBuffer <outputPxBuf>
static jint outputFrameSize[] = { 0, 0 };		// width x height
static std::shared_ptr<GLStitcher> stitcher;
static ImageFormat_t input_format;
static ImageFormat_t output_format;
static bool isInitiated = false;
std::vector<uint8_t> front_buffer;
std::vector<uint8_t> back_buffer;
std::vector<uint8_t> stitched_buffer;
static clock_t process_time;
/**
 * Cleanup helper. Will release the output pixel buffer.
 */
void ogCleanupHelper(JNIEnv *env) {
    if (outputPxBuf && outputPxBufData) {	// buffer is already set, release it first
        env->DeleteGlobalRef(outputPxBuf);
        delete outputPxBufData;

        outputPxBuf = NULL;
        outputPxBufData = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_init
        (JNIEnv * env, jobject, jstring curDirectory)
{
    stitcher = std::make_shared<GLStitcher>();
    const char* strDir = env->GetStringUTFChars(curDirectory, NULL);
    stitcher->SetWorkDirectory(strDir);
    env->ReleaseStringUTFChars(curDirectory, strDir);
}
JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_prepareInput
        (JNIEnv *, jobject, jint width, jint height)
{
    input_format.frame_width = width;
    input_format.frame_height = height;
    stitcher->SetSrcImageFormat(input_format);

    front_buffer.resize(input_format.frame_width*input_format.frame_height * 4);
    back_buffer.resize(input_format.frame_width*input_format.frame_height * 4);

}

JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_prepareOutput
        (JNIEnv * env, jobject, jint width, jint height)
{
    output_format.frame_width = width;
    output_format.frame_height = height;

    stitcher->SetDstImageFormat(output_format);

    stitched_buffer.resize(output_format.frame_width * output_format.frame_height * 4);

    ogCleanupHelper(env);

    // get the output frame size
    outputFrameSize[0] = output_format.frame_width;
    outputFrameSize[1] = output_format.frame_height;

    // create the output buffer as NIO direct byte buffer
    outputPxBufNumBytes = width * height * 4;
    outputPxBufData = new unsigned char[outputPxBufNumBytes];
    outputPxBuf = env->NewDirectByteBuffer(outputPxBufData, outputPxBufNumBytes);
    outputPxBuf = env->NewGlobalRef(outputPxBuf);	// we will hold a reference on this global variable until cleanup is called

}


static const int TEST_TIME  = 200;

JNIEXPORT jobject JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_process
        (JNIEnv *env, jobject, jintArray frontPixels, jintArray backPixels)
{
    jint *pxFrontInts = env->GetIntArrayElements(frontPixels, 0);
    jint *pxBackInts = env->GetIntArrayElements(backPixels, 0);
/*    if (!isInitiated) {

        isInitiated = true;
    }*/

//    RGB2YUV((uint8_t*)pxFrontInts, front_buffer.data(), input_format.frame_width*input_format.frame_height * 4);
//    RGB2YUV((uint8_t*)pxBackInts, back_buffer.data(), input_format.frame_width*input_format.frame_height * 4);

    VideoFrame_t src_frame[2];
    src_frame[0].planes[0] = (uint8_t*)pxFrontInts;
    src_frame[0].strides[0] = input_format.frame_width * 4;
    src_frame[1].planes[0] = (uint8_t*)pxBackInts;
    src_frame[1].strides[0] = input_format.frame_width * 4;

    VideoFrame_t dst_frame;
    dst_frame.planes[0] = stitched_buffer.data();
    dst_frame.strides[0] = output_format.frame_width * 4;

    stitcher->StitchImage(src_frame, &dst_frame);

    clock_t start_time = clock();
    for (int i = 0; i < TEST_TIME; i++) {
        stitcher->StitchImage(src_frame, &dst_frame);
    }
    process_time = clock() - start_time;
    int milliDivide = CLOCKS_PER_SEC / 1000;
    process_time = process_time / milliDivide;
//    YUV2RGB(dst_frame.planes[0], outputPxBufData, output_format.frame_width * output_format.frame_height * 4);
    RGBA2RGBARevert(dst_frame.planes[0], outputPxBufData, output_format.frame_width, output_format.frame_height,
                  output_format.frame_width  * 4);

    return outputPxBuf;

}

JNIEXPORT void JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_cleanup
        (JNIEnv *env, jobject)
{
    ogCleanupHelper(env);
}


JNIEXPORT int JNICALL Java_com_xiaoyi_sujin_glstitch_StitchJNIWrapper_getProcessTime
        (JNIEnv *env, jobject)
{
    return process_time;
}