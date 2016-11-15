//
// Created by sujin on 2016/11/15.
//

#ifndef GLSTITCH_MEMTRANSFER_H
#define GLSTITCH_MEMTRANSFER_H
#include "esutil.h"

class MemTransfer {
public:
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
