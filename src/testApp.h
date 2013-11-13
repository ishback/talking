

#ifndef _TEST_APP
#define _TEST_APP



#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"
#include "VideoSource.h"

struct anglePoint { // a datatype that contains centroids its angle to the center of centroid
    ofPoint centroid;
    float angle;
};


class testApp : public ofBaseApp {
public:
    
    enum CC_MODES {
        CC_MODE_DISPLAY,
        CC_MODE_READ,
        CC_MODE_CALIBRATE,
        CC_MODE_THRESHOLD,
        CC_MODE_CONTOURS,
        CC_MODE_PROGRESS_BAR,
        CC_MODE_TEST
    };
    
    enum ENV {
        OSX,
        RPI
    };
    
    void setup();
    void update();
    void draw();
    
    void mousePressed(int x, int y, int button);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void findCenterMarks(ofxCvContourFinder & cnt);
    static bool byAngle(const anglePoint &a, const anglePoint &b);
    void sortCentroids(ofxCvContourFinder &contours);
    float getAngle(ofPoint &p1, ofPoint &p2);

    void initDisplayMode();
    void initReadMode();
    
    void updateMesh();
    
    void drawCalibration();
    void drawCircle();
    void drawRGB();
    void drawBlobFilled();
    void rgbToFbo();
    void fboToColorWarp();
    void colorWarpToGrayThres();

    vector<anglePoint> blobCenters;
    
    // ofVideoGrabber movie;
    IVideoSource* movie;


    int env;
    
    
    ofImage calibrationImage;
    ofxCvColorImage rgb, resized;
    ofxCvGrayscaleImage grayImage, grayThres, blobFilled;
    ofxCvContourFinder contours;
    float blobArea;
    
    ofImage             grayOfImage;
    ofImage             colorWarp;
    ofMesh              mesh;
    
    ofFbo   fbo;
    
    int w,h;
    int wWin; //width of the square window
    int findHue;
    unsigned char * pixels;
    vector < ofVec2f > sourcePoints;
    vector < ofVec3f > destinationPoints;
    
    int mode;
    bool isCalibrated;

    ofxARToolkitPlus artk;
    int threshold;
    float factor;
    int barLength;
    int barHeight;
    int barMineCurrent;
    int barOtherCurrent;
    
};

#endif