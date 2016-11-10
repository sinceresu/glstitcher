//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0 
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015 
// http://www.mkonrad.net
//
// See LICENSE file in project repository root for the license.
//

package com.xiaoyi.sujin.glstitch;


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
    public native void init(boolean usePlatformOptimizations);

    /**
     * Prepare ogles_gpgpu for incoming images of size <inW> x <inH>. Do this
     * each time you change the input image size (and of course at the beginning
     * for the initial input image size).
     *
     * @param inW input frame width
     * @param inH input frame height
     * @param prepareDataInput set to true if you later want to copy data to ogles_gpgpu
     *						   by using setInputPixels(). set to false if you submit
     *						   input data by texture via setInputTexture().
     */
    public native void prepareInput(int inW, int inH);


    public native void prepareOutput(int outW, int outH);


    /**
     * Cleanup the ogles_gpgpu resources. Call this only once when you quit using ogles_gpgpu.
     */
    public native void cleanup();
    
    /**
     * Set the raw input pixel data as ARGB integer array. The size of this array
     * must equal <inW> * <inH> (set via <prepare()>).
     * 
     * @param pixels    pixel data with ARGB integers
     */
    public native void setInputPixels(int[] pixels);


    /**
     * Executes the GPGPU processing tasks.
     */
    public native void process();

    /**
     * Return the input pixel data as ARGB ByteBuffer. The size of this byte buffer
     * equals output frame width * output frame height * 4 (4 channel ARGB data).
     * 
     * Note: The returned ByteBuffer is only a reference to the actual image data
     * on the native side! It is only valid until the next call to this function!
     * 
     * @return reference to pixel data as ByteBuffer valid unit next call to this function
     */
    public native ByteBuffer getOutputPixels();


}