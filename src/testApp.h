

#ifndef _TEST_APP
#define _TEST_APP



#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"

//#ifndef TARGET_RASPBERRY_PI
//#include "ofxMacamPs3Eye.h"
//#endif

//#include "VideoSource.h"



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
    
    void setSourcePoints(ofTexture &texture, vector<ofPoint> &corners);
    void setDestinationPoints();
    void checkEnvironment();
    void getMarkerImage();
    
    void updateMesh();
    void adjustSensitivity();
    void drawCalibration();
    void drawCircle();
    void drawBlobFilled();
    void rgbToFbo();
    void fboToColorWarp();
    void colorWarpToGrayThres();
    
    // debug info
    void drawRGB();
    void drawEnvironment();
    void drawData();
    void drawARCorners(vector<ofPoint> &corners);
    
    void drawMouseCursor();
    void drawBar();
    
    void checkWalls();
    void checkBar();
    void drawBall();
    void checkTheOther();
    bool checkOtherIsPaddle();
    bool checkOtherIsBall();
    bool checkOtherIsBallAtCenter();
    bool checkOtherIsCursor();
    void checkILost();
    float getRatioMarkerArea();

    void updateBlink();
    void syncFreqBlinks();
    void resetPong();

    vector<anglePoint> blobCenters;
    
    ofVideoGrabber movie;
//    IVideoSource* movie;
//    ofxMacamPs3Eye movie;
    
    // set in main.cpp, 0 or 1 depending on OSX or RPI
    int env;
    
    ofImage calibrationImage;
    ofxCvColorImage rgb, fullSize;;
    ofxCvGrayscaleImage grayImage, grayThres, blobFilled, currentEnvir, lastEnvir, diffEnvir, markerCV;
    ofxCvContourFinder contours;
    float blobArea, lastArea;
    
    ofImage             grayOfImage;
    ofImage             colorWarp, testImage, colorMarker;
    ofMesh              mesh;
    
    ofFbo   fbo, markerFbo;
    
    int w,h,camW,camH,xOffset;
    int wWin; //width of the square window
    int findHue;
    unsigned char * pixels;
    unsigned char * pixCurrentEnvir;
    unsigned char * pixMarkerCV;
    vector < ofVec2f > sourcePoints;
    vector < ofVec3f > destinationPoints;
    vector <ofVec3f> normals;
    ofPath markerQuad;
    
    bool frameIsNew;
    bool needsRecalibration;
    int calibrationEnergy;
    int mode;
    bool isCalibrated;
    float calibrationBWRatio;

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

    bool IAmBall;
    bool IAmPaddle;
    bool otherIsBall;
    bool otherIsPaddle;
    bool otherLost;
    bool ILost;
    ofVec2f pos, velInit, vel;
    int ballInitRadius;
    int ballRadius;
    int yPosBar;
    int xPosBar; 
    int barPongHeight;
    int barPongWidth;
    float ratioMarkerArea;
    int loseTime;
    int waitTime;
    bool wasBallFirst;
    int numGamesPlayed;
    int numGamesBeforeSwitch;
    
    int checkTime;
    int checkDuration;
    
    bool debug;
    
    bool rotate90;
    
};

#endif