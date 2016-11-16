//
// Created by sujin on 2016/11/15.
//

#include "MemTransferAndroid.h"
#include <dlfcn.h>
#include <assert.h>

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


GraphicBufferFnCtor MemTransferAndroid::graBufCreate = NULL;
GraphicBufferFnDtor MemTransferAndroid::graBufDestroy = NULL;
GraphicBufferFnGetNativeBuffer MemTransferAndroid::graBufGetNativeBuffer = NULL;
GraphicBufferFnLock MemTransferAndroid::graBufLock = NULL;
GraphicBufferFnUnlock MemTransferAndroid::graBufUnlock = NULL;

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

    // load necessary Android GraphicBuffer functions
    void *dlUIhndl = dlopen("libui.so", RTLD_LAZY);
    if (!dlUIhndl) {
        return false;
    }

    graBufCreate = OG_DL_FUNC(dlUIhndl, "_ZN7android13GraphicBufferC1Ejjij", GraphicBufferFnCtor);

    graBufDestroy = OG_DL_FUNC(dlUIhndl, "_ZN7android13GraphicBufferD1Ev", GraphicBufferFnDtor);

    graBufGetNativeBuffer = OG_DL_FUNC(dlUIhndl, "_ZNK7android13GraphicBuffer15getNativeBufferEv", GraphicBufferFnGetNativeBuffer);

    graBufLock = OG_DL_FUNC(dlUIhndl, "_ZN7android13GraphicBuffer4lockEjPPv", GraphicBufferFnLock);

    graBufUnlock = OG_DL_FUNC(dlUIhndl, "_ZN7android13GraphicBuffer6unlockEv", GraphicBufferFnUnlock);

    dlclose(dlUIhndl);

    // all done

    return true;
}

MemTransferAndroid::~MemTransferAndroid() {
    // release in- and outputs
    releaseInput();
    releaseOutput();
}
#define OG_ANDROID_GRAPHIC_BUFFER_SIZE 1024

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

    // generate texture id

    glGenTextures(1, &front_tex);
    glBindTexture(GL_TEXTURE_2D, front_tex);



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    glGenerateMipmap(GL_TEXTURE_2D);

    int nativePxFmt = HAL_PIXEL_FORMAT_RGBA_8888;

    // create graphic buffer
    inputFrontGraBufHndl = malloc(OG_ANDROID_GRAPHIC_BUFFER_SIZE);
    graBufCreate(inputFrontGraBufHndl, inputW, inputH, nativePxFmt,
                 GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_SW_WRITE_OFTEN);  // is used as OpenGL texture and will be written often

    // get window buffer
    inputFrontNativeBuf = (struct ANativeWindowBuffer *)graBufGetNativeBuffer(inputFrontGraBufHndl);

    if (!inputFrontNativeBuf) {
        OG_LOGERR("MemTransferAndroid", "error getting native window buffer for input");
        return 0;
    }

    // create image for reading back the results
    EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };
    inputFrontImage = imageKHRCreate(eglGetDisplay(EGL_DEFAULT_DISPLAY),
                                EGL_NO_CONTEXT,
                                EGL_NATIVE_BUFFER_ANDROID,
                                (EGLClientBuffer)inputFrontNativeBuf,
                                eglImgAttrs);	// or NULL as last param?

    if (!inputFrontImage) {
        OG_LOGERR("MemTransferAndroid", "error creating image KHR for input");
        return 0;
    }
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputFrontImage);

    GLenum err = glGetError();


    glGenTextures(1, &back_tex);
    glBindTexture(GL_TEXTURE_2D, back_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);

//    glGenerateMipmap(GL_TEXTURE_2D);
    // create graphic buffer
    inputBackGraBufHndl = malloc(OG_ANDROID_GRAPHIC_BUFFER_SIZE);
    graBufCreate(inputBackGraBufHndl, inputW, inputH, nativePxFmt,
                 GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_SW_WRITE_OFTEN);  // is used as OpenGL texture and will be written often

    // get window buffer
    inputBackNativeBuf = (struct ANativeWindowBuffer *)graBufGetNativeBuffer(inputBackGraBufHndl);

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

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputBackImage);

    // done
    preparedInput = true;

    return true;
}

//bool MemTransferAndroid::prepareOutput(int outTexW, int outTexH) {
//    assert(outTexW > 0 && outTexH > 0);
//
//    if (outputW == outTexW && outputH == outTexH) {
//        return true; // no change
//    }
//
//    if (preparedOutput) {    // already prepared -- release buffers!
//        releaseOutput();
//    }
//
//    // set attributes
//    outputW = outTexW;
//    outputH = outTexH;
//
//    // generate texture id
//
//    glGenTextures(1, &output_tex);
//
//
//    if (output_tex == 0) {
//        return false;
//    }
//    glBindTexture(GL_TEXTURE_2D, output_tex);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputW, outputH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//
//    // create graphic buffer
//    outputGraBufHndl = malloc(OG_ANDROID_GRAPHIC_BUFFER_SIZE);
//    graBufCreate(outputGraBufHndl, outputW, outputH, HAL_PIXEL_FORMAT_RGBA_8888,
//                 GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_SW_READ_OFTEN);    // is render target and will be read often
//
//    // get window buffer
//    outputNativeBuf = (struct ANativeWindowBuffer *)graBufGetNativeBuffer(outputGraBufHndl);
//
//    if (!outputNativeBuf) {
//        OG_LOGERR("MemTransferAndroid", "error getting native window buffer for output");
//        return 0;
//    }
//
//    // create image for reading back the results
//    EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };
//    outputImage = imageKHRCreate(eglGetDisplay(EGL_DEFAULT_DISPLAY),
//                                 EGL_NO_CONTEXT,
//                                 EGL_NATIVE_BUFFER_ANDROID,
//                                 (EGLClientBuffer)outputNativeBuf,
//                                 eglImgAttrs);	// or NULL as last param?
//
//    if (!outputImage) {
//        OG_LOGERR("MemTransferAndroid", "error creating image KHR for output");
//        return 0;
//    }
//
//
//
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output_tex, 0);
//
//    // done
//    preparedOutput = true;
//
//    return true;
//}
//
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
        free(inputFrontImage);
        inputFrontImage = NULL;
    }

    // release android graphic buffer handle for input
    if (inputFrontGraBufHndl) {
        OG_LOGINF("MemTransferAndroid", "releasing graphic buffer handle for input");
        graBufDestroy(inputFrontGraBufHndl);
        free(inputFrontGraBufHndl);

        inputFrontGraBufHndl = NULL;
        inputFrontNativeBuf = NULL;  // reset weak-ref pointer to NULL
    }

    if (inputBackImage) {
        OG_LOGINF("MemTransferAndroid", "releasing input image");
        imageKHRDestroy(EGL_DEFAULT_DISPLAY, inputBackImage);
        free(inputBackImage);
        inputBackImage = NULL;
    }

    // release android graphic buffer handle for input
    if (inputBackGraBufHndl) {
        OG_LOGINF("MemTransferAndroid", "releasing graphic buffer handle for input");
        graBufDestroy(inputBackGraBufHndl);
        free(inputBackGraBufHndl);

        inputBackGraBufHndl = NULL;
        inputBackNativeBuf = NULL;  // reset weak-ref pointer to NULL
    }

}


//void MemTransferAndroid::releaseOutput() {
//    if (output_tex > 0) {
//        glDeleteTextures(1, &output_tex);
//        output_tex = 0;
//    }
//
//    if (outputImage) {
//        OG_LOGINF("MemTransferAndroid", "releasing output image");
//        imageKHRDestroy(EGL_DEFAULT_DISPLAY, outputImage);
//        free(outputImage);
//        outputImage = NULL;
//    }
//
//    // release android graphic buffer handle for output
//    if (outputGraBufHndl) {
//        OG_LOGINF("MemTransferAndroid", "releasing graphic buffer handle for output");
//        graBufDestroy(outputGraBufHndl);
//        free(outputGraBufHndl);
//
//        outputGraBufHndl = NULL;
//        outputNativeBuf = NULL;  // reset weak-ref pointer to NULL
//    }
//}

void MemTransferAndroid::toGPU(const unsigned char *frontBuf, const unsigned char *backBuf) {
    assert(preparedInput && front_tex > 0  && back_tex > 0 && frontBuf && backBuf);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, front_tex);

    // activate the image KHR for the input
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputFrontImage);

    // lock the graphics buffer at graphicsPtr
    unsigned char *graphicsPtr = (unsigned char *)lockInputBufferAndGetPtr(inputFrontGraBufHndl);

    // copy whole image from "buf" to "graphicsPtr"
    memcpy(graphicsPtr, frontBuf, inputW * inputH * 4);
    //  memset(graphicsPtr, 0, inputW * inputH * 4);
    // unlock the graphics buffer again
    unlockInputBuffer(inputFrontGraBufHndl);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, back_tex);
    // activate the image KHR for the input
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputBackImage);

    // lock the graphics buffer at graphicsPtr
    graphicsPtr = (unsigned char *)lockInputBufferAndGetPtr(inputBackGraBufHndl);

    // copy whole image from "buf" to "graphicsPtr"
    memcpy(graphicsPtr, backBuf, inputW * inputH * 4);

    // unlock the graphics buffer again
    unlockInputBuffer(inputBackGraBufHndl);


}

//void MemTransferAndroid::fromGPU(unsigned char *buf) {
//    assert(preparedOutput && output_tex > 0 && buf);
//
//    glBindTexture(GL_TEXTURE_2D, output_tex);
//
//    // activate the image KHR for the output
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, outputImage);
//
//
//    // lock the graphics buffer at graphicsPtr
//    const unsigned char *graphicsPtr = (const unsigned char *)lockOutputBufferAndGetPtr();
//
//    // copy whole image from "graphicsPtr" to "buf"
//    memcpy(buf, graphicsPtr, outputW * outputH * 4);
//
//    // unlock the graphics buffer again
//    unlockOutputBuffer();
//
//}


void *MemTransferAndroid::lockInputBufferAndGetPtr(void *inputGraBufHndl) {
    void *hndl;
    int usage;
    unsigned char *memPtr;

    hndl = inputGraBufHndl;
    usage = GRALLOC_USAGE_SW_WRITE_OFTEN;


    // lock and get pointer
    graBufLock(hndl, usage, &memPtr);

    // check for valid pointer
    if (!memPtr) {
        OG_LOGERR("MemTransferAndroid", "GraphicBuffer lock returned invalid pointer");
    }

    return (void *)memPtr;
}


void *MemTransferAndroid::lockOutputBufferAndGetPtr() {
    void *hndl;
    int usage;
    unsigned char *memPtr;

    hndl = outputGraBufHndl;
    usage = GRALLOC_USAGE_SW_READ_OFTEN;


    // lock and get pointer
    graBufLock(hndl, usage, &memPtr);

    // check for valid pointer
    if (!memPtr) {
        OG_LOGERR("MemTransferAndroid", "GraphicBuffer lock returned invalid pointer");
    }

    return (void *)memPtr;
}

void MemTransferAndroid::unlockInputBuffer(void *inputGraBufHndl) {
    graBufUnlock(inputGraBufHndl);
}

void MemTransferAndroid::unlockOutputBuffer() {
    graBufUnlock(outputGraBufHndl);
}
