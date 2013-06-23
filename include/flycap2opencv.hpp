#pragma once
#include "opencv2/opencv.hpp"
#include "FlyCapture2.h"

struct FlyCap2OpenCV {
	FlyCapture2::Error error;
	FlyCapture2::PGRGuid guid;
	FlyCapture2::BusManager busMgr;
	FlyCapture2::Camera cam;
	FlyCapture2::Image rawImage;
	FlyCapture2::Image convertedImg;
	FlyCapture2::PixelFormat outPixelFmt;
	IplImage* frame;

	static const char *videoModeName[(int)FlyCapture2::NUM_VIDEOMODES];
	static const char *frameRateValue[(int)FlyCapture2::NUM_FRAMERATES];

	FlyCap2OpenCV(): frame(0), outPixelFmt(FlyCapture2::PIXEL_FORMAT_MONO8) {}
	~FlyCap2OpenCV() { deinit(); }

	//need to be called fisrt
	inline bool init(const unsigned int idx=0) {
		error=busMgr.GetCameraFromIndex(idx, &guid);
		if(error!=FlyCapture2::PGRERROR_OK) {
			error.PrintErrorTrace();
			return false;
		}

		error=cam.Connect(&guid);
		if(error!=FlyCapture2::PGRERROR_OK) {
			error.PrintErrorTrace();
			return false;
		}

		return true;
	}

	//need to be called before start to capture and
	//after custom settings to cam object
	inline bool initBuffer(const bool use_mono=true, const bool use_bgr=true) {
		if(use_mono) outPixelFmt=FlyCapture2::PIXEL_FORMAT_MONO8;
		else if(use_bgr) outPixelFmt=FlyCapture2::PIXEL_FORMAT_BGR;
		else outPixelFmt=FlyCapture2::PIXEL_FORMAT_RGB8;

		error=cam.StartCapture();
		if(error!=FlyCapture2::PGRERROR_OK) {
			error.PrintErrorTrace();
			return false;
		}

		cam.RetrieveBuffer(&rawImage);
		if(frame!=0) {
			cvReleaseImage(&frame);
			frame=0;
		}
		int depth=8, nchannels=use_mono?1:3;
		frame=cvCreateImage(cvSize(rawImage.GetCols(),rawImage.GetRows()), depth, nchannels);

		FlyCapture2::VideoMode vm;
		FlyCapture2::FrameRate fr;
		cam.GetVideoModeAndFrameRate(&vm, &fr);
		std::cout<<"[FlyCap2OpenCV::initBuffer] current video mode="<<std::endl;
		if(vm<FlyCapture2::NUM_VIDEOMODES) {
			std::cout<<"\t"<<FlyCap2OpenCV::videoModeName[(int)vm]<<std::endl;
		} else {
			std::cout<<"\tnot recognized!"<<std::endl;
		}

		std::cout<<"[FlyCap2OpenCV::initBuffer] current frame rate="<<std::endl;
		if(fr<FlyCapture2::NUM_FRAMERATES) {
			std::cout<<"\t"<<FlyCap2OpenCV::frameRateValue[(int)fr]<<std::endl;
		} else {
			std::cout<<"\tnot recognized!"<<std::endl;
		}
		return true;
	}

	//read an image into dst from a point grey camera
	inline bool read(cv::Mat& dst) {
		if(!cam.IsConnected()) {
			std::cout<<"[FlyCap2OpenCV::read] FlyCapture2::Camera is not connected,"
				" please call FlyCap2OpenCV::init first!"<<std::endl;
			return false;
		}
		if(frame==0) {
			std::cout<<"[FlyCap2OpenCV::read] frame==0, please call"
				" FlyCap2OpenCV::initBuffer first!"<<std::endl;
			return false;
		}

		cam.RetrieveBuffer(&rawImage);
		error=rawImage.Convert(outPixelFmt, &convertedImg);
		if(error!=FlyCapture2::PGRERROR_OK) {
			error.PrintErrorTrace();
			return false;
		}
		memcpy(frame->imageData, convertedImg.GetData(), convertedImg.GetDataSize());
		dst = cv::Mat(frame);
		return !dst.empty();
	}

	//no need to call explicitly, will be automatically called in deconstructor
	inline bool deinit() {
		if(frame!=0) {
			cvReleaseImage(&frame);
			frame=0;
		}
		error=cam.StopCapture();
		if(error!=FlyCapture2::PGRERROR_OK) {
			error.PrintErrorTrace();
		}
		if(cam.IsConnected()) {
			error=cam.Disconnect();
			if(error!=FlyCapture2::PGRERROR_OK) {
				error.PrintErrorTrace();
				return false;
			}
		}
		std::cout<<"[FlyCap2OpenCV::deinit] done!"<<std::endl;
		return true;
	}
};

const char *FlyCap2OpenCV::videoModeName[]={
	"160x120YUV444",
	"320x240YUV422",
	"640x480YUV411",
	"640x480YUV422",
	"640x480RGB",
	"640x480Y8",
	"640x480Y16",
	"800x600YUV422",
	"800x600RGB",
	"800x600Y8",
	"800x600Y16",
	"1024x768YUV422",
	"1024x768RGB",
	"1024x768Y8",
	"1024x768Y16",
	"1280x960YUV422",
	"1280x960RGB",
	"1280x960Y8",
	"1280x960Y16",
	"1600x1200YUV422",
	"1600x1200RGB",
	"1600x1200Y8",
	"1600x1200Y16",
	"FORMAT7"
};

const char *FlyCap2OpenCV::frameRateValue[]={
	"1.875Hz",
	"3.75Hz",
	"7.5Hz",
	"15Hz",
	"30Hz",
	"60Hz",
	"120Hz",
	"240Hz",
	"FORMAT7"
};
