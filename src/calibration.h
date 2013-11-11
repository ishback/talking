#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"

class calibration {
public:
//    calibration();
//    ~calibration();

    void setup();
    void update();
    void draw();

    void calibrate();
    void setDestinationPoints();
    void setSourcePoints(int index, vector<ofPoint> &corners);
    void updateMesh();
    
    ofMesh getMesh();

    void setMovie();
    void setWidth();
    void setHeight();

    bool isCalibrated;

    ofImage calibrationImage;

    ofxCvGrayscaleImage grayImage, grayThres;

    ofImage             grayOfImage;
    ofImage             colorWarp;
    ofMesh              mesh;

    ofFbo   fbo;

    int w, h;
    int wWin; //width of the square window

    unsigned char *pixels;

    vector < ofVec2f > sourcePoints;
    vector < ofVec3f > destinationPoints;

    ofxARToolkitPlus artk;
    int threshold;
};