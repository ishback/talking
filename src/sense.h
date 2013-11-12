#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"
#include "calibration.h"

struct senseProperties {
    ofxCvGrayscaleImage grayThresh;
};

class sense
{
public:
//	sense();
//	~sense();

	void setup(int _w, int _h, int _wWin);
	void update(calibrationProperties* c_p, unsigned char* pixels);
	void update(ofMesh &mesh, unsigned char* pixels);
	void draw();
	void rgbToFbo(ofMesh* mesh);
	void fboToColorWarp();
	void colorWarpToGrayThresh();
    senseProperties* getSenseProperties();

//private:
//    calibrationProperties* c_p;
    senseProperties* s_p;
    ofMesh* mesh;
    unsigned char* pixels;
    
//    ofImage rgb;
    ofxCvColorImage rgb;
    ofxCvGrayscaleImage grayImage, grayThres;
    
    ofImage             grayOfImage, testImage;
    ofImage             colorWarp;
    
    ofFbo   fbo;
    
    int w,h;
    int wWin; //width of the square window
    int threshold;
};