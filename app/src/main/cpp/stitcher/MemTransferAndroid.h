//
// Created by sujin on 2016/11/15.
//

#ifndef GLSTITCH_MEMTRANSFERANDROID_H
#define GLSTITCH_MEMTRANSFERANDROID_H

#include "MemTransfer.h"

#include <EGL/egl.h>
#ifndef EGL_EGLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLES2/gl2ext.h>
#include <memory>

class GraphicBuffer;
// constructor
typedef void (*GraphicBufferFnCtor)(void *graphicBufHndl, uint32_t w, uint32_t h, uint32_t format, uint32_t usage);

// deconstructor
typedef void (*GraphicBufferFnDtor)(void *graphicBufHndl);

// getNativeBuffer
typedef void* (*GraphicBufferFnGetNativeBuffer)(void *graphicBufHndl);

// lock
typedef int (*GraphicBufferFnLock)(void *graphicBufHndl, uint32_t usage, unsigned char **addr);

// unlock
typedef int (*GraphicBufferFnUnlock)(void *graphicBufHndl);

/**
 * typedefs to EGL extension functions for ImageKHR extension
 */

// create ImageKHR
typedef EGLImageKHR (*EGLExtFnCreateImage)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attribList);

// destroy ImageKHR
typedef EGLBoolean (*EGLExtFnDestroyImage)(EGLDisplay dpy, EGLImageKHR image);


class MemTransferAndroid : public MemTransfer{
public:
    /**
     * Try to initialize platform optimizations. Returns true on success, else false.
     */
    static bool initPlatformOptimizations();

    /**
    * Constructor
    */
    MemTransferAndroid() :  MemTransfer(),
                            inputFrontNativeBuf(NULL),
                            outputNativeBuf(NULL),
                            inputFrontImage(NULL),
                            outputImage(NULL)
    { }

    /**
     * Deconstructor
     */
    virtual ~MemTransferAndroid();
    /**
      * Prepare for input frames of size <inTexW>x<inTexH>. Return a texture id for the input frames.
      */
    virtual bool prepareInput(int inTexW, int inTexH);

    /**
     * Prepare for output frames of size <outTexW>x<outTexH>. Return a texture id for the output frames.
     */
    virtual bool prepareOutput(int outTexW, int outTexH);

    /**
     * Delete input texture.
     */
    virtual void releaseInput();
    /**
      * Delete output texture.
      */
    virtual void releaseOutput();


    /**
     * Map data in <buf> to GPU.
     */
    virtual void toGPU(const unsigned char *frontBuf, const unsigned char *backBuf);

    /**
     * Map data from GPU to <buf>
     */
    virtual void fromGPU(unsigned char *buf);

private:

    static EGLExtFnCreateImage  imageKHRCreate;     // function pointer to EGLExtFnCreateImage
    static EGLExtFnDestroyImage  imageKHRDestroy;   // function pointer to EGLExtFnDestroyImage



    struct ANativeWindowBuffer *inputFrontNativeBuf;     // pointer to native window buffer for input (weak ref - do not free()!)
    EGLImageKHR inputFrontImage;     // ImageKHR handle for input

    struct ANativeWindowBuffer *inputBackNativeBuf;     // pointer to native window buffer for input (weak ref - do not free()!)
    EGLImageKHR inputBackImage;     // ImageKHR handle for input


    struct ANativeWindowBuffer *outputNativeBuf;	// pointer to native window buffer for output (weak ref - do not free()!)

    EGLImageKHR outputImage;    // ImageKHR handle for output

    std::shared_ptr<GraphicBuffer> _pFrontGraphicBuffer;
    std::shared_ptr<GraphicBuffer> _pBackGraphicBuffer;

    std::shared_ptr<GraphicBuffer> _pStitchedGraphicBuffer;

};


#endif //GLSTITCH_MEMTRANSFERANDROID_H
