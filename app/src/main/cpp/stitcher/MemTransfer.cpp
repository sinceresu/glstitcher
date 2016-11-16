//
// Created by sujin on 2016/11/15.
//

#include "MemTransfer.h"
#include <assert.h>

bool MemTransfer::initPlatformOptimizations() {
    // always return false here. this method is only fully implemented
    // in platform-specialized classes of MemTransfer.
    return false;
}

MemTransfer::MemTransfer() {
    // set defaults
    inputW = inputH = outputW = outputH = 0;
    front_tex = 0;
    back_tex = 0;
    output_tex = 0;
    initialized = false;
    preparedInput = false;
    preparedOutput = false;
}

MemTransfer::~MemTransfer() {
    // release in- and output
    releaseInput();
    releaseOutput();
}

bool MemTransfer::prepareInput(int inTexW, int inTexH) {
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
//    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,
//                   inputW, inputH
//    );

    //static const GLint swizzles[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ONE };
    //glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);

//    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &back_tex);

    glBindTexture(GL_TEXTURE_2D, back_tex);
//    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,
//                   inputW, inputH
//    );

    //static const GLint swizzles[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ONE };
    //glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);

//    glGenerateMipmap(GL_TEXTURE_2D);
    // done
    preparedInput = true;

    return true;
}

bool MemTransfer::prepareOutput(int outTexW, int outTexH) {
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputW, outputH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output_tex, 0);

   // done
    preparedOutput = true;

    return true;
}

void MemTransfer::releaseInput() {
    if (front_tex > 0) {
        glDeleteTextures(1, &front_tex);
        front_tex = 0;
    }
    if (back_tex > 0) {
        glDeleteTextures(1, &back_tex);
        back_tex = 0;
    }
}

void MemTransfer::releaseOutput() {
    if (output_tex > 0) {
        glDeleteTextures(1, &output_tex);
        output_tex = 0;
    }
}

void MemTransfer::toGPU(const unsigned char *frontBuf, const unsigned char *backBuf) {
    assert(preparedInput && front_tex > 0  && back_tex > 0 && frontBuf && backBuf);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, front_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inputW, inputH, 0, GL_RGB, GL_UNSIGNED_BYTE, frontBuf);
/*
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0, 0,
                    inputW, inputH,
                    GL_RGBA, GL_UNSIGNED_BYTE,
                    frontBuf);*/


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, back_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inputW, inputH, 0, GL_RGB, GL_UNSIGNED_BYTE, backBuf);
//    glTexSubImage2D(GL_TEXTURE_2D,
//                    0,
//                    0, 0,
//                    inputW, inputH,
//                    GL_RGBA, GL_UNSIGNED_BYTE,
//                    backBuf);


}

void MemTransfer::fromGPU(unsigned char *buf) {
    assert(preparedOutput && output_tex > 0 && buf);

    glBindTexture(GL_TEXTURE_2D, output_tex);

    // default (and slow) way using glReadPixels:
    glReadPixels(0, 0, outputW, outputH, GL_RGBA, GL_UNSIGNED_BYTE, buf);

}
