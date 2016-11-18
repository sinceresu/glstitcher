//
// Created by sujin on 2016/11/15.
//

#include "MemTransferAndroid.h"
#include <dlfcn.h>
#include <assert.h>
#include "GraphicBuffer.h"

#include "../common/common_includes.h"


enum {
    /* buffer is never read in software */
            GRALLOC_USAGE_SW_READ_NEVER   = 0x00000000,
    /* buffer is rarely read in software */
            GRALLOC_USAGE_SW_READ_RARELY  = 0x00000002,
    /* buffer is often read in software */
            GRALLOC_USAGE_SW_READ_OFTEN   = 0x00000003,
    /* mask for the software read values */
            GRALLOC_USAGE_SW_READ_MASK    = 0x0000000F,

    /* buffer is never written in software */
            GRALLOC_USAGE_SW_WRITE_NEVER  = 0x00000000,
    /* buffer is never written in software */
            GRALLOC_USAGE_SW_WRITE_RARELY = 0x00000020,
    /* buffer is never written in software */
            GRALLOC_USAGE_SW_WRITE_OFTEN  = 0x00000030,
    /* mask for the software write values */
            GRALLOC_USAGE_SW_WRITE_MASK   = 0x000000F0,

    /* buffer will be used as an OpenGL ES texture */
            GRALLOC_USAGE_HW_TEXTURE      = 0x00000100,
    /* buffer will be used as an OpenGL ES render target */
            GRALLOC_USAGE_HW_RENDER       = 0x00000200,
    /* buffer will be used by the 2D hardware blitter */
            GRALLOC_USAGE_HW_2D           = 0x00000400,
    /* buffer will be used with the framebuffer device */
            GRALLOC_USAGE_HW_FB           = 0x00001000,
    /* mask for the software usage bit-mask */
            GRALLOC_USAGE_HW_MASK         = 0x00001F00,
};

enum {
    HAL_PIXEL_FORMAT_RGBA_8888          = 1,
    HAL_PIXEL_FORMAT_RGBX_8888          = 2,
    HAL_PIXEL_FORMAT_RGB_888            = 3,
    HAL_PIXEL_FORMAT_RGB_565            = 4,
    HAL_PIXEL_FORMAT_BGRA_8888          = 5,
    HAL_PIXEL_FORMAT_RGBA_5551          = 6,
    HAL_PIXEL_FORMAT_RGBA_4444          = 7,
};

EGLExtFnCreateImage MemTransferAndroid::imageKHRCreate = NULL;
EGLExtFnDestroyImage MemTransferAndroid::imageKHRDestroy = NULL;

#define OG_DL_FUNC(hndl, fn, type) (type)dlsym(hndl, fn)
#define OG_DL_FUNC_CHECK(hndl, fn_ptr, fn) if (!fn_ptr) { OG_LOGERR("MemTransferAndroid", "could not dynamically link func '%s': %s", fn, dlerror()); dlclose(hndl); return false; }

bool MemTransferAndroid::initPlatformOptimizations() {
    // load necessary EGL extension functions
    void *dlEGLhndl = dlopen("libEGL.so", RTLD_LAZY);
    if (!dlEGLhndl) {
         return false;
    }

    imageKHRCreate = OG_DL_FUNC(dlEGLhndl, "eglCreateImageKHR", EGLExtFnCreateImage);

    imageKHRDestroy = OG_DL_FUNC(dlEGLhndl, "eglDestroyImageKHR", EGLExtFnDestroyImage);

    dlclose(dlEGLhndl);


    // all done

    return true;
}

MemTransferAndroid::~MemTransferAndroid() {
    // release in- and outputs
    releaseInput();
    releaseOutput();
}
#define OG_ANDROID_GRAPHIC_BUFFER_SIZE 2048

bool MemTransferAndroid::prepareInput(int inTexW, int inTexH) {
    assert(inTexW > 0 && inTexH > 0);

    if (inputW == inTexW && inputH == inTexH) {
        return true; // no change
    }

    if (preparedInput) {    // already prepared -- release buffers!
        releaseInput();
    }

    // set attributes
    inputW = inTexW;
    inputH = inTexH;


    int usage = GraphicBuffer::USAGE_HW_TEXTURE | GraphicBuffer::USAGE_SW_WRITE_OFTEN ;
    // create graphic buffer
    _pFrontGraphicBuffer = std::make_shared<GraphicBuffer>(inputW, inputH, PIXEL_FORMAT_RGBA_8888, usage);
    // get window buffer
    inputFrontNativeBuf = (struct ANativeWindowBuffer *)_pFrontGraphicBuffer->getNativeBuffer();

    if (!inputFrontNativeBuf) {
        OG_LOGERR("MemTransferAndroid", "error getting native window buffer for input");
        return 0;
    }

    // create image for reading back the results
    EGLint eglImgAttrs[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
    inputFrontImage = imageKHRCreate(eglGetDisplay(EGL_DEFAULT_DISPLAY),
                                EGL_NO_CONTEXT,
                                EGL_NATIVE_BUFFER_ANDROID,
                                (EGLClientBuffer)inputFrontNativeBuf,
                                eglImgAttrs);	// or NULL as last param?

    if (!inputFrontImage) {
        OG_LOGERR("MemTransferAndroid", "error creating image KHR for input");
        return 0;
    }

    glGenTextures(1, &front_tex);
    glBindTexture(GL_TEXTURE_2D, front_tex);



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputFrontImage);

    GLenum err = glGetError();
    // create graphic buffer

    _pBackGraphicBuffer = std::make_shared<GraphicBuffer>(inputW, inputH, PIXEL_FORMAT_RGBA_8888, usage);
    // get window buffer
    inputBackNativeBuf = (struct ANativeWindowBuffer *)_pBackGraphicBuffer->getNativeBuffer();

    if (!inputBackNativeBuf) {
        OG_LOGERR("MemTransferAndroid", "error getting native window buffer for input");
        return 0;
    }

    // create image for reading back the results
     inputBackImage = imageKHRCreate(eglGetDisplay(EGL_DEFAULT_DISPLAY),
                                     EGL_NO_CONTEXT,
                                     EGL_NATIVE_BUFFER_ANDROID,
                                     (EGLClientBuffer)inputBackNativeBuf,
                                     eglImgAttrs);	// or NULL as last param?

    if (!inputBackImage) {
        OG_LOGERR("MemTransferAndroid", "error creating image KHR for input");
        return 0;
    }

    glGenTextures(1, &back_tex);
    glBindTexture(GL_TEXTURE_2D, back_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputBackImage);

    // done
    preparedInput = true;

    return true;
}

bool MemTransferAndroid::prepareOutput(int outTexW, int outTexH) {
    assert(outTexW > 0 && outTexH > 0);

    if (outputW == outTexW && outputH == outTexH) {
        return true; // no change
    }

    if (preparedOutput) {    // already prepared -- release buffers!
        releaseOutput();
    }

    // set attributes
    outputW = outTexW;
    outputH = outTexH;

    // generate texture id

    glGenTextures(1, &output_tex);


    if (output_tex == 0) {
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, output_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, outputW, outputH, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int usage = GraphicBuffer::USAGE_HW_RENDER | GraphicBuffer::USAGE_SW_READ_OFTEN;

    // create graphic buffer
    _pStitchedGraphicBuffer = std::make_shared<GraphicBuffer>(outputW, outputH, PIXEL_FORMAT_RGB_888, usage);
    // get window buffer
    outputNativeBuf = (struct ANativeWindowBuffer *)_pStitchedGraphicBuffer->getNativeBuffer();

    if (!outputNativeBuf) {
        OG_LOGERR("MemTransferAndroid", "error getting native window buffer for output");
        return 0;
    }

    // create image for reading back the results
    EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };
    outputImage = imageKHRCreate(eglGetDisplay(EGL_DEFAULT_DISPLAY),
                                 EGL_NO_CONTEXT,
                                 EGL_NATIVE_BUFFER_ANDROID,
                                 (EGLClientBuffer)outputNativeBuf,
                                 eglImgAttrs);	// or NULL as last param?

    if (!outputImage) {
        OG_LOGERR("MemTransferAndroid", "error creating image KHR for output");
        return 0;
    }

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, outputImage);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output_tex, 0);

    // done
    preparedOutput = true;

    return true;
}

void MemTransferAndroid::releaseInput() {
    if (front_tex > 0) {
        glDeleteTextures(1, &front_tex);
        front_tex = 0;
    }

    if (back_tex > 0) {
        glDeleteTextures(1, &back_tex);
        back_tex = 0;
    }

    if (inputFrontImage) {
        OG_LOGINF("MemTransferAndroid", "releasing input image");
        imageKHRDestroy(EGL_DEFAULT_DISPLAY, inputFrontImage);
//        free(inputFrontImage);
        inputFrontImage = NULL;
    }
    if (inputFrontNativeBuf) {
        _pFrontGraphicBuffer.reset();
        inputFrontNativeBuf =  NULL;
    }

    if (inputBackImage) {
        OG_LOGINF("MemTransferAndroid", "releasing input image");
        imageKHRDestroy(EGL_DEFAULT_DISPLAY, inputBackImage);
//        free(inputBackImage);
        inputBackImage = NULL;
    }
    if (inputBackNativeBuf) {
        _pBackGraphicBuffer.reset();
        inputBackNativeBuf =  NULL;
    }

}


void MemTransferAndroid::releaseOutput() {
    if (output_tex > 0) {
        glDeleteTextures(1, &output_tex);
        output_tex = 0;
    }

    if (outputImage) {
        OG_LOGINF("MemTransferAndroid", "releasing output image");
        imageKHRDestroy(EGL_DEFAULT_DISPLAY, outputImage);
//        free(outputImage);
        outputImage = NULL;
    }
    if (outputNativeBuf) {
        _pStitchedGraphicBuffer.reset();
        outputNativeBuf =  NULL;
    }
    // release android graphic buffer handle for output

}

void MemTransferAndroid::toGPU(const VideoFrame_t *frontFrm, const VideoFrame_t *backFrm) {
    assert(preparedInput && front_tex > 0  && back_tex > 0 && frontFrm && backFrm);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, front_tex);

    // activate the image KHR for the input
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputFrontImage);

    // lock the graphics buffer at graphicsPtr
    unsigned char *graphicsPtr;
    void* temp;
    _pFrontGraphicBuffer->lock(GraphicBuffer::USAGE_SW_WRITE_OFTEN, &temp);
    graphicsPtr = (unsigned char *) temp;

    // copy whole image from "buf" to "graphicsPtr"
    int stride = _pFrontGraphicBuffer->getStride();
    unsigned char * writePtr = graphicsPtr;
    const unsigned char * readPtr  = frontFrm->planes[0];

    for (int row = 0; row < inputH; row++) {
        memcpy(writePtr, readPtr, inputW * 4);
        readPtr += inputW  * 4;
        writePtr += stride * 4;
    }
   // memcpy(graphicsPtr, frontBuf, stride * inputH * 3);
    //  memset(graphicsPtr, 0, inputW * inputH * 4);
    // unlock the graphics buffer again

    _pFrontGraphicBuffer->unlock();


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, back_tex);
    // activate the image KHR for the input
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputBackImage);

    // lock the graphics buffer at graphicsPtr
    _pBackGraphicBuffer->lock(GraphicBuffer::USAGE_SW_WRITE_OFTEN, &temp);
    graphicsPtr = (unsigned char *) temp;

    stride = _pBackGraphicBuffer->getStride();
    writePtr = graphicsPtr;
    readPtr  = backFrm->planes[0];

    for (int row = 0; row < inputH; row++) {
        memcpy(writePtr, readPtr, inputW * 4);
        readPtr += inputW  * 4;
        writePtr += stride * 4;
    }
    // copy whole image from "buf" to "graphicsPtr"
   // memcpy(graphicsPtr, backBuf, stride * inputH * 3);

    // unlock the graphics buffer again
    _pBackGraphicBuffer->unlock();


}

void MemTransferAndroid::fromGPU(VideoFrame_t *outputFrm) {
    assert(preparedOutput && output_tex > 0 && outputFrm);

    glBindTexture(GL_TEXTURE_2D, output_tex);

    // activate the image KHR for the output
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, outputImage);


    // lock the graphics buffer at graphicsPtr
    const unsigned char *graphicsPtr;
    void* temp;
    _pStitchedGraphicBuffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, &temp);
    graphicsPtr = (const unsigned char *) temp;

    // copy whole image from "graphicsPtr" to "buf"
    const unsigned char * readPtr = graphicsPtr;
    unsigned char *  writePtr  = outputFrm->planes[0];

    int stride = _pStitchedGraphicBuffer->getStride();
    for (int row = 0; row < outputH; row++) {
        memcpy(writePtr, readPtr, outputW * 3);
        readPtr += stride * 3;
        writePtr +=  outputW  * 3;
    }

    // unlock the graphics buffer again
    _pStitchedGraphicBuffer->unlock();

}

