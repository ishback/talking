#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"

struct calibrationProperties {
    ofMesh* mesh;
};

class calibration {
public:
//    calibration();
//    ~calibration();

    void setup(int _w, int _h, int _wWin);
    void update(unsigned char* pixels);
    void draw();

    void calibrate();
    void setDestinationPoints();
    void setSourcePoints(int index);
    void updateMesh();
    
    void updateMesh(ofMesh &mesh);
    
    calibrationProperties* getCalibrationProperties();
    
    bool isCalibrated;
    
    calibrationProperties* c_p;

private:
    vector<ofPoint> corners;
    
    ofImage calibrationImage;
    
    ofxCvColorImage colorImage;
    
    ofxCvGrayscaleImage grayImage, grayThres;

    ofImage             grayOfImage;
    ofImage             colorWarp;
    ofMesh              mesh;

    ofFbo   fbo;

    int w, h, wWin;

    unsigned char *pixels;

    vector < ofVec2f > sourcePoints;
    vector < ofVec3f > destinationPoints;

    ofxARToolkitPlus artk;
    int threshold;
};