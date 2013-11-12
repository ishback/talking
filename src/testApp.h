

#ifndef _TEST_APP
#define _TEST_APP



#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"



struct anglePoint { // a datatype that contains centroids its angle to the center of centroid
    ofPoint centroid;
    float angle;
};


class testApp : public ofBaseApp {
public:
    
    enum CC_MODES {
        CC_MODE_DISPLAY, //it displays the 
        CC_MODE_READ, //read from camera and display as is
        CC_MODE_CALIBRATE, //shows marker and calibrates (finds mesh)
        CC_MODE_THRESHOLD, //capture and display thresholded
        CC_MODE_CONTOURS, //shows circle same area as captures blob
        CC_MODE_PROGRESS_BAR,
        CC_MODE_MOUSE_POINTER,
        CC_MODE_CURSOR,
        CC_MODE_PONG
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
    void drawData();
    void drawMouseCursor();
    void drawBar();
    
    void checkWalls();
    void checkBar();
    void drawBall();
    void checkIfBall();

    void updateBlink();
    void syncFreqBlinks();

    vector<anglePoint> blobCenters;
    
    ofVideoGrabber movie;
    
    ofImage calibrationImage;
    ofxCvColorImage rgb, resized;
    ofxCvGrayscaleImage grayImage, grayThres, blobFilled;
    ofxCvContourFinder contours;
    float blobArea, lastArea;
    
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
    
    int barLength; //progress bar
    int barHeight;
    int barMineCurrent;
    int barOtherCurrent;
    
    // Cursor
    bool cursorOn;
    int cursorBlinkInterval;
    int cursorLastSwitchTime;

    float blinkFreq;
    int blinkCount;
    int blobStable;
    int blobEnergy;
    bool blobsOnLastFrame;
    bool blinkOn;
    bool freqsSynched;
    int lastBlinkTime;
    int myBlinkPeriod;

    bool pongBall; // true if ball, false if bar.
    ofVec2f pos, vel;
    int ballRadius;
    int yPosBar;
    int xPosBar; 
    int barPongHeight;
    int barPongWidth;
    
};

#endif