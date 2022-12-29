#pragma once

// A lot of this code is a rework of:
// https://github.com/apc-llc/moviemaker-cpp/blob/master/include/movie.h
// However it is to be noted, that there are improvements due to FFMpeg 4.2

#include <stdint.h>
#include <string>
#include <vector>


extern "C" {
	//#include <libavutil/mathematics.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}


class ffConvert {
private:
	const unsigned int width, height;
	unsigned int fFrame;
	int framerate;

public:
	// Framerate is a default of 24 because that is the default of Maya
	ffConvert(const std::string& filename, const unsigned int width, const unsigned int height, const int frameRate = 24);
	~ffConvert();

	void addFrame(const unsigned char* pixels);
};