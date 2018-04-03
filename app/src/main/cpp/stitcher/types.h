#ifndef __TYPES_H_
#define  __TYPES_H_

#include "common.h"

typedef enum {
	MEDIAFRAME_ENDOFSTREAM  = 0X01,				//SET WHEN LAST PACKET FOR THIS STREAM
	MEDIAFRAME_TIMESTAMP	 = 0X02,			//
	MEDIAFRAME_DISCONTINUITY = 0X04,
	MEDIAFRAME_KEY_FRAME	 = 0X08,			//
	MEDIAFRAME_STOP_FRAME	 = 0X20,			//
	MEDIAFRAME_SKIP_FRAME	 = 0X40,			//
} MediaFrameFlags_t;

enum eStitchedVideoSize
{
	VIDEOSIZE_3840X1920 = 0,
	VIDEOSIZE_1920X960 = 1,
};


#define MAX_VIDEO_PLANES 4
typedef struct VideoFrame_t
{
	ePixelFormat_t pixel_format;
	uint8_t * planes[MAX_VIDEO_PLANES];
	int strides[MAX_VIDEO_PLANES]; // decoded bitmap stride
	int flags;
	int64_t pts;
     /**
     * for some private data of the user
     */
	void *opaque;
} VideoFrame_t;


#define MAX_AUDIO_CHANNELS 8

typedef struct AudioFrame_t
{
	eSampleFormat_t format;
	uint8_t * data[MAX_AUDIO_CHANNELS];
	size_t samples;
	int flags;
	int64_t pts;
} AudioFrame_t;


// Audio Format
typedef struct ContainerFormat_t
{
	char	type[64];       // container type string
	int64_t size;			// file  size in bytes
	int64_t duration;		// file  duration in ms
	int64_t bitrate;		// total bit rate
	unsigned int reserved1;
	unsigned int reserved2;
} ContainerFormat_t;

typedef struct AVRational_t{
	int num; ///< numerator
	int den; ///< denominator
} AVRational_t;

// Video format
typedef struct VideoFormat_t
{
	eVideoCodec_t codec;           // Compression format
	struct {
		unsigned int numerator;     // frame rate numerator   (0 = unspecified or variable frame rate)
		unsigned int denominator;   // frame rate denominator (0 = unspecified or variable frame rate)
	} frame_rate;                   // frame rate = numerator / denominator (for example: 30000/1001)
	AVRational_t time_base;
	unsigned int frame_width;       // coded frame width
	unsigned int frame_height;      // coded frame height 
	struct {                        // area of the frame that should be displayed
		int left;                   // typical example:
		int top;                    //   frame_width = 1920, frame_height = 1088
		int right;                  //   display_area = { 0,0,1920,1080 }
		int bottom;
	} display_area;
	ePixelFormat_t pixel_format;    // Pixel format
	int64_t bitrate;           // video bitrate (bps, 0=unknown)
	struct {                        // Display Aspect Ratio = x:y (4:3, 16:9, etc)
		int x;
		int y;
	} display_aspect_ratio;
} VideoFormat_t;

// Audio Format
typedef struct AudioFormat_t
{
	eAudioCodec_t codec;       // Compression format
	AVRational_t time_base;
	eSampleFormat_t sample_format;  // Sample format
	unsigned int channels;			// number of audio channels
	unsigned int sample_rate;		// sampling frequency
	int64_t bitrate;			// For uncompressed, can also be used to determine bits per sample
	unsigned int reserved1;
	unsigned int reserved2;
} AudioFormat_t;

typedef struct ImageFormat_t
{
	ePixelFormat_t pixel_format;    // Pixel format
	unsigned int frame_width;       // coded frame width
	unsigned int frame_height;      // coded frame height 
	struct {                        // area of the frame that should be displayed
		int left;                   // typical example:
		int top;                    //   frame_width = 1920, frame_height = 1088
		int right;                  //   display_area = { 0,0,1920,1080 }
		int bottom;
	} display_area;
	struct {                        // Display Aspect Ratio = x:y (4:3, 16:9, etc)
		int x;
		int y;
	} display_aspect_ratio;
} ImageFormat_t;

typedef enum {
	AVPACKET_ENDOFSTREAM  = 0x01,			//Set when last packet for this stream
	AVPACKET_TIMESTAMP	 = 0x02,			//
	AVPACKET_DISCONTINUITY = 0x04,
	AVPACKET_KEY_FRAME	 = 0x08,			//
} AvPacketFlags_t;



typedef struct AVPacket_t
{
	AVPacket_t(unsigned char* buf_ptr, size_t buffer_size)
		: buf(buf_ptr)
		, buf_size(buffer_size)
		, payload(NULL)
		, payload_size(0)
		, dts(0)
		, pts(0)
		, flags(0)
	{
		payload = buf;
	}
	AVPacket_t()
		: buf(NULL)
		, buf_size(0)
		, payload(NULL)
		, payload_size(0)
		, dts(0)
		, pts(0)
		, flags(0)
	{
	}

	eMediaType_t  media_type;
	unsigned long flags;            // Combination of CUVID_PKT_XXX flags
	unsigned char * buf;
	size_t	buf_size;
	unsigned char *payload;   // Pointer to packet payload data (may be NULL if EOS flag is set)
	size_t payload_size;     // number of bytes in the payload (may be zero if EOS flag is set)
	int64_t dts;     // coded order time stamp
	int64_t pts;     // presentation time stamp
}AVPacket_t;

typedef struct Period_t
{
	int64_t start_time;		//in ms
	int64_t end_time;		//in ms
}Period_t;	
typedef struct FishEyeStitcherParam_t {
	int target_width;
	int target_height;
	int warp_step_x;
	int warp_step_y;
	int first_region_left;
	int first_region_width;
	int first_table_width;
	int first_table_height;
	int second_region_left1;
	int second_region_width1;
	int second_table_width1;
	int second_table_height1;
	int second_region_left2;
	int second_region_width2;
	int second_table_width2;
	int second_table_height2;
} FishEyeStitcherParam_t;


typedef struct TabledImageROI_t// region of interest, could be seam cut\ frame valid area \color summary zone
{
	int left;
	int top;
	int width;
	int height;
	int table_width;
	int table_height;
}TabledImageROI_t;

typedef struct ImageParameters
{

	int mapImageWidth;
	int mapImageHeight;

	int proStep_X;
	int proStep_Y;

	//int sparseproImgW;
	//int sparseproImgH;

	int panoImageW;
	int panoImageH;

	int scale;

	TabledImageROI_t first_region;
	TabledImageROI_t second_region[2];
} ImageParameters;

typedef struct FloatMapPoint
{//maptable , float precision
	float x;
	float y;
} FloatMapPoint;

typedef struct FixMapPoint
{//maptable , int precision
	int x;
	int y;
} FixMapPoint;

typedef struct AdjustFloatMapPoint : public FloatMapPoint
{
	float l;  //< luminance adjust
} AdjustFloatMapPoint;
#endif
