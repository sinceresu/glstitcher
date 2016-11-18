//
// Created by sujin on 2016/11/15.
//

#include "MemTransferAndroid.h"
#include <dlfcn.h>
#include <assert.h>
#include "GraphicBuffer.h"

#include "../common/common_includes.h"
#include "FormatConverterFactory.h"
#include "FormatConverter.h"

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

    _pInputFormatConverter = FormatConverterFactory::CreateFormatConverter(PIXELFORMAT_ARGB, PIXELFORMAT_ARGB);

    if (!_pInputFormatConverter)
        return false;
    _pInputFormatConverter->SetImageSize(inTexW, inTexH);


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

    _pOutputFormatConverter = FormatConverterFactory::CreateFormatConverter(PIXELFORMAT_ARGB, PIXELFORMAT_ARGB);

    if (!_pOutputFormatConverter)
        return false;
    _pOutputFormatConverter->SetImageSize(outTexW, outTexH);

    // generate texture id

    glGenTextures(1, &output_tex);


    if (output_tex == 0) {
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, output_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputW, outputH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int usage = GraphicBuffer::USAGE_HW_RENDER | GraphicBuffer::USAGE_SW_READ_OFTEN;

    // create graphic buffer
    _pStitchedGraphicBuffer = std::make_shared<GraphicBuffer>(outputW, outputH, PIXEL_FORMAT_RGBA_8888, usage);
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

    // lock the graphics buffer at graphicsPtr
    void* graphicsPtr;
    _pFrontGraphicBuffer->lock(GraphicBuffer::USAGE_SW_WRITE_OFTEN, &graphicsPtr);
    // copy whole image from "buf" to "graphicsPtr"
    int stride = _pFrontGraphicBuffer->getStride();
    VideoFrame_t graphic_frm = {
        PIXELFORMAT_ARGB,
        {
                (unsigned char *) graphicsPtr, NULL, NULL
        },
        {
                stride * 4, 0, 0
        }
    };
    _pInputFormatConverter->ConvertImage(frontFrm, &graphic_frm);
    // unlock the graphics buffer again
    _pFrontGraphicBuffer->unlock();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, back_tex);
    // activate the image KHR for the input
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, inputBackImage);

    // lock the graphics buffer at graphicsPtr
    _pBackGraphicBuffer->lock(GraphicBuffer::USAGE_SW_WRITE_OFTEN, &graphicsPtr);
    // copy whole image from "buf" to "graphicsPtr"
    stride = _pBackGraphicBuffer->getStride();
    graphic_frm = {
        PIXELFORMAT_ARGB,
        {
                (unsigned char *) graphicsPtr, NULL, NULL
        },
        {
                stride * 4, 0, 0
        }
    };
    _pInputFormatConverter->ConvertImage(backFrm, &graphic_frm);

    // unlock the graphics buffer again
    _pBackGraphicBuffer->unlock();


}

void MemTransferAndroid::fromGPU(VideoFrame_t *outputFrm) {
    assert(preparedOutput && output_tex > 0 && outputFrm);

    glBindTexture(GL_TEXTURE_2D, output_tex);

    // activate the image KHR for the output
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, outputImage);


    // lock the graphics buffer at graphicsPtr
     void* graphicsPtr;
    _pStitchedGraphicBuffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, &graphicsPtr);
    // copy whole image from "graphicsPtr" to "buf"
    int stride = _pStitchedGraphicBuffer->getStride();
    VideoFrame_t graphic_frm = {
        PIXELFORMAT_ARGB,
        {
                (unsigned char *)graphicsPtr, NULL, NULL
        },
        {
                stride * 4, 0, 0
        }
    };
   _pOutputFormatConverter->ConvertImage(&graphic_frm, outputFrm);

    // unlock the graphics buffer again
    _pStitchedGraphicBuffer->unlock();

}

