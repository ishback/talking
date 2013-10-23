#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
    ofBackground(0, 0, 0);
    ofEnableSmoothing();
    ofSetCircleResolution(100);

    w = ofGetWidth();
    h = ofGetHeight();
    centerMarks.x = -1;
    threshold = 85;

    mode = CC_CALIBRATE;
    isCalibrated = false;

    movie.initGrabber(w, h, true);

    calibrationImage.loadImage("calibration.jpg");
    calibrationImage.resize(h, h);
    
    //reserve memory for cv images
    rgb.allocate(w, h);
	grayImage.allocate(w, h);
	grayThres.allocate(w, h);
    
    resized.allocate(w, h);

    initDisplayMode();
    initReadMode();
    
    artk.setup(w, h);
	artk.setThreshold(threshold);
}

//--------------------------------------------------------------
void testApp::update() {

    if (movie.isFrameNew()) {
        if (movie.isFrameNew()) {
            rgb.setFromPixels(movie.getPixels(), w, h);
//            rgb.mirror(false, true);
        }
    }
    movie.update();
    
    switch (mode) {

    case CC_MODE_READ:
        // cout << "READ" << endl;
        
        break;

    case CC_MODE_DISPLAY:
        
        break;
        
    case CC_CALIBRATE:
        // convert our camera image to grayscale
        grayImage = rgb;
        
        // Pass in the new image pixels to artk
        artk.update(grayImage.getPixels());
        break;
    }
    


}

//--------------------------------------------------------------
void testApp::draw() {
    ofSetColor(255, 255, 255);

    switch (mode) {

    case CC_MODE_READ:
        ofPushMatrix();
        {
            ofTranslate(h, h-240);
            ofScale(0.25, 0.25);
            rgb.draw(0, 0);
//            filtered.draw(0, -240*4);
        }
        ofPopMatrix();
        rgb.getTextureReference().bind();
        mesh.draw();
        rgb.getTextureReference().unbind();
        break;

    case CC_MODE_DISPLAY:
        drawCircle();
        break;
            
    case CC_CALIBRATE:
        drawCalibration();
            
        ofPushMatrix();
        {
            ofTranslate(h, h-240);
            ofScale(0.25, 0.25);
            rgb.draw(0, 0);
            //            filtered.draw(0, -240*4);
        }
        ofPopMatrix();
            
        // ARTK 2D stuff
        // See if marker ID '0' was detected
        // and draw blue corners on that marker only
        int myIndex = artk.getMarkerIndex(0);
        
        ofDrawBitmapString(ofToString(artk.getNumDetectedMarkers()), w-40, 40);

        if(myIndex >= 0) {
            cout << "DETECTED" << endl;
            // TODO - this should happen only after some time period
            isCalibrated = true;
            mode = CC_MODE_READ;
            
            // Get the corners
            vector<ofPoint> corners;
            artk.getDetectedMarkerBorderCorners(myIndex, corners);
            
            sourcePoints.clear();
            sourcePoints.push_back(ofVec2f(corners[0].x, corners[0].y));
            sourcePoints.push_back(ofVec2f(corners[1].x, corners[1].y));
            sourcePoints.push_back(ofVec2f(corners[3].x, corners[3].y));
            sourcePoints.push_back(ofVec2f(corners[2].x, corners[2].y));


            

            
            
            updateMesh();
            
            // Can also get the center like this:
            // ofPoint center = artk.getDetectedMarkerCenter(myIndex);
//            ofSetHexColor(0x0000ff);
//            for(int i=0;i<corners.size();i++) {
//                ofCircle(corners[i].x, corners[i].y, 10);
//            }
        }
            
        artk.draw(0, 0);

        break;
    }
    


    

    // //draw all cv images
    // rgb.draw(0, 0);
    // //hsb.draw(640,0);
    // //hue.draw(0,240);
    // sat.draw(320, 240);
    // bri.draw(640, 240);
    // filtered.draw(0, 480);
    // contours.draw(0, 480);
    // img.draw(0, 0);

    // ofSetColor(255, 0, 0);
    // ofFill();


    // //draw red circles for found blobs
    for (int i = 0; i < contours.nBlobs; i++) {
        ofCircle(contours.blobs[i].centroid.x, contours.blobs[i].centroid.y, 20);
    }
    if (centerMarks.x != -1) {
        ofCircle(centerMarks.x, centerMarks.y, 5);
    }

    // ofSetColor(255);
    // rgb.getTextureReference().bind();
    // mesh.draw();
    // rgb.getTextureReference().unbind();


}

void testApp::drawCalibration() {
    calibrationImage.draw(0, 0);
}

void testApp::drawCircle() {
    ofSetColor(255);
    ofCircle(ofPoint(h/2, h/2), 300);
}

void testApp::updateMesh() {
    mesh.clearTexCoords();
    mesh.clearVertices();
    mesh.addTexCoords(sourcePoints); // origin coordinates
    mesh.addVertices(destinationPoints);
}

void testApp::initDisplayMode() {

}

void testApp::initReadMode() {
    // SETUP MESH
    destinationPoints.clear();
    //we can use 5 vertex to close it clockwise, or change the order to match Tri-strip sequence
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    destinationPoints.push_back(ofVec3f(0, 0, 0));
    destinationPoints.push_back(ofVec3f(0, h, 0));
    destinationPoints.push_back(ofVec3f(h, 0, 0));
    destinationPoints.push_back(ofVec3f(h, h, 0));
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

    //calculate local mouse x,y in image
//    int mx = x % w;
//    int my = y % h;
//
//    //get hue value on mouse position
//    findHue = hue.getPixels()[my * w + mx];
}

//--------------------------------------------------------------
void testApp::findCenterMarks(ofxCvContourFinder &contours) {
    int xAve = 0;
    int yAve = 0;
    for (int i = 0; i < contours.nBlobs; i++) {
        xAve += contours.blobs[i].centroid.x;
        yAve += contours.blobs[i].centroid.y;
    }
    xAve = xAve / contours.nBlobs;
    yAve = yAve / contours.nBlobs;
    centerMarks.set(xAve, yAve);
}

void testApp::sortCentroids(ofxCvContourFinder &contours) {
    //contours.findContours(flat, 20, w*h/2, 4, false);
    if (contours.nBlobs == 4) { // find center if we have 4 blobs
        findCenterMarks(contours);
    } else {
        centerMarks.x = -1;   // otherwise set center to -1
        return;
    }
    sourcePoints.clear();
    for (int i = 0; i < contours.nBlobs; i++) {
        anglePoint aP;
        aP.centroid = contours.blobs[i].centroid;
        aP.angle = getAngle(contours.blobs[i].centroid, centerMarks);

        blobCenters.push_back(aP);
    }
    ofSort(blobCenters, byAngle);

    sourcePoints.push_back(blobCenters[1].centroid);
    sourcePoints.push_back(blobCenters[2].centroid);
    sourcePoints.push_back(blobCenters[0].centroid);
    sourcePoints.push_back(blobCenters[3].centroid);
}

//--------------------------------------------------------------
float testApp::getAngle(ofPoint &p1, ofPoint &p2) {

    float a = atan2(p2.y - p1.y, p1.x - p2.x);
    //a = (a+TWO_PI)%(TWO_PI);
    if (a < 0) a = a + 2 * PI;
    return a;

}

//--------------------------------------------------------------
bool testApp::byAngle(const anglePoint &a, const anglePoint &b) {
    return a.angle < b.angle;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {

    switch (key) {

    case '1':
        mode = CC_MODE_DISPLAY;
        break;

    case '2':
        mode = CC_MODE_READ;
        break;
            
    case '3':
        mode = CC_CALIBRATE;
        break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo) {

}
