#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"

struct senseProperties {
    ofMesh mesh;
    ofxCvGrayscaleImage grayThresh;
};

class sense
{
public:
//	sense();
//	~sense();

	void setup();
	void update(ofMesh &mesh, unsigned char* pixels);
	void draw();
	void rgbToFbo(ofMesh &mesh);
	void fboToColorWarp();
	void colorWarpToGrayThresh();
    senseProperties getSenseProperties();

private:
    senseProperties s_p;
    
    ofxCvColorImage rgb, resized;
    ofxCvGrayscaleImage grayImage, grayThres;
    
    ofImage             grayOfImage;
    ofImage             colorWarp;
    
    ofFbo   fbo;
    
    int w,h;
    int wWin; //width of the square window
    
    unsigned char * pixels;
    
    vector < ofVec2f > sourcePoints;
    vector < ofVec3f > destinationPoints;
    
    ofxARToolkitPlus artk;
    int threshold;
};