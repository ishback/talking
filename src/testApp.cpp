#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
    ofBackground(0, 0, 0);
    ofEnableSmoothing();
    ofSetCircleResolution(100);

    w = ofGetWidth();
    h = ofGetHeight();

    mode = CC_MODE_DISPLAY;

    movie.initGrabber(w, h, true);

    //reserve memory for cv images
    rgb.allocate(w, h);
    hsb.allocate(w, h);
    hue.allocate(w, h);
    sat.allocate(w, h);
    bri.allocate(w, h);
    resized.allocate(w, h);
    filtered.allocate(w, h);

    findHue = 85;

    initDisplayMode();
    initReadMode();
}

//--------------------------------------------------------------
void testApp::update() {

    switch (mode) {

    case CC_MODE_READ:
        cout << "READ" << endl;
        if (movie.isFrameNew()) {
            cout << "FRAME IS NEW" << endl;
            // resized = rgb;
            // rgb.resize(320, 240);


            // create filtered image for contours
            thresholdByHue(findHue);

            //run the contour finder on the filtered image to find blobs with a certain hue
            blobCenters.clear();
            contours.findContours(filtered, 20, w * h / 2, 4, false);

            sortCentroids(contours);

            updateMesh();
        }
        movie.update();
        break;

    case CC_MODE_DISPLAY:
        
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
        }
        ofPopMatrix();
        rgb.getTextureReference().bind();
        mesh.draw();
        rgb.getTextureReference().unbind();
        break;

    case CC_MODE_DISPLAY:
        cout << "DISPLAY" << endl;
        drawCalibration(findHue);
        drawCircle();
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
    // for (int i = 0; i < contours.nBlobs; i++) {
    //     ofCircle(contours.blobs[i].centroid.x, contours.blobs[i].centroid.y, 20);
    // }
    // if (centerMarks.x != -1) {
    //     ofCircle(centerMarks.x, centerMarks.y, 5);
    // }

    // ofSetColor(255);
    // rgb.getTextureReference().bind();
    // mesh.draw();
    // rgb.getTextureReference().unbind();


}

void testApp::drawCalibration(int hueValue) {
    ofColor col = ofColor::fromHsb(hueValue, 255, 255);
    ofSetColor(col);
    int radius = 20;
    ofCircle(ofPoint(50, 50), radius);
    ofCircle(ofPoint(50, h-50), radius);
    ofCircle(ofPoint(h-50, 50), radius);
    ofCircle(ofPoint(h-50, h-50), radius);
}

void testApp::drawCircle() {
    ofSetColor(255);
    ofCircle(ofPoint(h/2, h/2), 300);
}

void testApp::thresholdByHue(int hueValue) {
    //copy webcam pixels to rgb image
    rgb.setFromPixels(movie.getPixels(), w, h);

    //mirror horizontal
    rgb.mirror(false, true);

    //duplicate rgb
    hsb = rgb;

    //convert to hsb
    hsb.convertRgbToHsv();

    //store the three channels as grayscale images
    hsb.convertToGrayscalePlanarImages(hue, sat, bri);

    //filter image based on the hue value were looking for
    for (int i = 0; i < w * h; i++) {
        filtered.getPixels()[i] = ofInRange(hue.getPixels()[i], hueValue - 30, hueValue + 30) ? 255 : 0;
    }
    filtered.flagImageChanged();
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
    int mx = x % w;
    int my = y % h;

    //get hue value on mouse position
    findHue = hue.getPixels()[my * w + mx];
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
