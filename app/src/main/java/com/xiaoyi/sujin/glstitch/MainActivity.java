package com.xiaoyi.sujin.glstitch;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;


public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private Button startBtn = null;
    private StitchJNIWrapper stitchWrapper = null;


    private int frontImgId;
    private int backImgId;

    private int selectedTestImgId = -1;

    private ImageView imgView;
    private Bitmap frontImgBm;
    private Bitmap backImgBm;

    private BitmapDrawable origImgBmDr;

    private int inputW;					// input image width
    private int inputH;					// input image height
    private int outputW  = 1920;				// output image width
    private int outputH = 960;				// output image height

    private int[] inputFrontPixels;			// pixel data of <frontImgBm> as ARGB int values
    private int[] inputBackPixels;			// pixel data of <frontImgBm> as ARGB int values
    private ByteBuffer outputPixels;	// output pixel data as ARGB bytes values

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("stitch-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imgView = (ImageView)findViewById(R.id.img_view);

        // Example of a call to a native method
        startBtn = (Button) findViewById(R.id.btn_start);

        initTestImagesMap();

        startBtn.setOnClickListener(this);

        String outputStr = stringFromJNI();

    }
    @Override
    public void onClick(View v) {
        stitchWrapper = new StitchJNIWrapper();

        stitchWrapper.init("/stitch");

        loadAndDisplayTestImage();

    }

    private void loadAndDisplayTestImage() {

        // create a bitmap of the input image
        frontImgBm = BitmapFactory.decodeResource(getResources(), frontImgId);
        inputW = frontImgBm.getWidth();
        inputH = frontImgBm.getHeight();
        backImgBm = BitmapFactory.decodeResource(getResources(), backImgId);

        // set up the new input pixel buffer
        inputFrontPixels = new int[inputW * inputH];
        inputBackPixels = new int[inputW * inputH];

        // get the pixel data as ARGB int values
        frontImgBm.getPixels(inputFrontPixels, 0, inputW, 0, 0, inputW, inputH);
        backImgBm.getPixels(inputBackPixels, 0, inputW, 0, 0, inputW, inputH);

        // prepare ogles_gpgpu for the input image of size <inputW>x<inputH>
        prepareOG();

        Bitmap processedBm = Bitmap.createBitmap(
                outputW,
                outputH,
                Bitmap.Config.ARGB_8888);
        // set it to the bitmap
        outputPixels.rewind();
        processedBm.copyPixelsFromBuffer(outputPixels);
        outputPixels.rewind();

        // create drawable from it
        BitmapDrawable dispDrawable = new BitmapDrawable(getResources(), processedBm);

        // set it as drawable for the image view
        origImgBmDr = new BitmapDrawable(getResources(), frontImgBm);
        imgView.setImageDrawable(origImgBmDr);

        imgView.setImageDrawable(dispDrawable);

    }
    private void initTestImagesMap() {
        frontImgId = R.drawable.cam_front_1;
        backImgId = R.drawable.cam_back_1;
    }

    /**
     * Prepare ogles_gpgpu for the incoming image size
     */
    private void prepareOG() {
        // prepare for the input image size
        stitchWrapper.prepareInput(inputW, inputH);
        stitchWrapper.prepareOutput(outputW, outputH);


        // unfortunately, the first call to "process" will always create an empty result
        // when using Android platform optimizations.
        // the following is a workaround for this: make a fake first "process" call
        outputPixels = stitchWrapper.process(inputFrontPixels, inputBackPixels);

    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
