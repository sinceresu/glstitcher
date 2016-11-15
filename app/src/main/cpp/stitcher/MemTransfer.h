//
// Created by sujin on 2016/11/15.
//

#ifndef GLSTITCH_MEMTRANSFER_H
#define GLSTITCH_MEMTRANSFER_H
#include "esutil.h"

class MemTransfer {
    /**
    * Constructor
    */
    MemTransfer();

    /**
     * Deconstructor
     */
    virtual ~MemTransfer();
    /**
     * Prepare for input frames of size <inTexW>x<inTexH>. Return a texture id for the input frames.
     */
    virtual bool prepareInput(int inTexW, int inTexH, GLenum inputPxFormat = GL_RGBA, void *inputDataPtr = NULL);

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

    /**
     * Try to initialize platform optimizations. Returns true on success, else false.
     * Is only fully implemented in platform-specialized classes of MemTransfer.
     */
    static bool initPlatformOptimizations();
protected:
    bool initialized;       // is initialized?

    bool preparedInput;     // input is prepared?
    bool preparedOutput;    // output is prepared?

    GLuint front_tex;
    GLuint back_tex;

    GLuint output_tex;  // Texture handles

    int inputW;             // input texture width
    int inputH;             // input texture height
    int outputW;            // output texture width
    int outputH;            // output texture heights

};


#endif //GLSTITCH_MEMTRANSFER_H
