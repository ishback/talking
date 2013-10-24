

#ifndef _TEST_APP
#define _TEST_APP

#define CC_MODE_DISPLAY 1
#define CC_MODE_READ 2

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxQuadWarp.h"


struct anglePoint { // a datatype that contains centroids its angle to the center of centroid
    ofPoint centroid;
    float angle;
};


class testApp : public ofBaseApp{
public:
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
    
    vector<anglePoint> blobCenters;
    
    ofVideoGrabber movie;
    
    ofxCvColorImage rgb,hsb;
    ofxCvGrayscaleImage hue,sat,bri,filtered, flat;
    ofxCvContourFinder contours;
    ofPoint centerMarks;

    ofImage             img;
    ofMesh              mesh;
    
    int w,h;
    int findHue;
    unsigned char * pixels;
    vector < ofVec2f > sourcePoints;
    vector < ofVec3f > destinationPoints;
    
    int mode;
    
};

#endif