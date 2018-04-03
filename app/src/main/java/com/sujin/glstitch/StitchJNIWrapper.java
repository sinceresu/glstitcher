//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0 
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015 
// http://www.mkonrad.net
//
// See LICENSE file in project repository root for the license.
//

package com.sujin.glstitch;


import java.nio.ByteBuffer;

/**
 * The ogles_gppgu JNI wrapper class. Interface to the native functions for the
 * og_jni_wrapper.so library.
 */
public class StitchJNIWrapper {
	public final static int ORIENTATION_NONE 				= -1;
	public final static int ORIENTATION_STD 				= 0;
	public final static int ORIENTATION_STD_MIRRORED 		= 1;
	public final static int ORIENTATION_FLIPPED 			= 2;
	public final static int ORIENTATION_FLIPPED_MIRRORED	= 3;
	public final static int ORIENTATION_DIAGONAL			= 4;
	
	public final static int RENDER_DISP_MODE_INPUT			= 0;
	public final static int RENDER_DISP_MODE_OUTPUT			= 1;
	
	static {
		// load the static library ogles_gpgpu JNI wrapper
		System.loadLibrary("stitch-lib");
	}

    /**
     * Initialize ogles_gpgpu. Call this function at first to use ogles_gpgpu.
     * @param usePlatformOptimizations try to enable platform optimizations
     */
    public native void init(String curDirectory);

    /**
     * Prepare ogles_gpgpu for incoming images of size <inW> x <inH>. Do this
     * each time you change the input image size (and of course at the beginning
     * for the initial input image size).
     *
     * @param inW input frame width
     * @param inH input frame height
      */
    public native void prepareInput(int inW, int inH);

    /**
     * Prepare ogles_gpgpu for outcoming images of size <outW> x <outH>. Do this
     * each time you change the output image size (and of course at the beginning
     * for the initial output image size).
     *
     * @param outW output frame width
     * @param outH output frame height
     */
    public native void prepareOutput(int outW, int outH);

    /**
     * Cleanup the ogles_gpgpu resources. Call this only once when you quit using ogles_gpgpu.
     */
    public native void cleanup();
    
    /**
     * Set the raw input pixel data as ARGB integer array. The size of this array
     * must equal <inW> * <inH> (set via <prepare()>).
     * Executes the GPGPU processing tasks.
     *
     * @param pixels    pixel data with ARGB integers
     * @return reference to pixel data as ByteBuffer valid unit next call to this function
      */
    public native ByteBuffer process(int[] frontPixels, int[] backPixels);

	public native int getProcessTime();




}