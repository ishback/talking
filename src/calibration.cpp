
#include "calibration.h"

void calibration::setup(int _w, int _h, int _wWin) {
    
    wWin = _wWin;
    w = _w;
    h = _h;
    threshold = 127;

    calibrationImage.loadImage("calibration.jpg");
    calibrationImage.resize(wWin, h);
    
    colorImage.allocate(w, h);
    grayImage.allocate(w, h);

    isCalibrated = false;

    artk.setup(w, h);
    artk.setUndistortionMode(ofxARToolkitPlus::UNDIST_STD);
    artk.setThreshold(threshold);
    
    c_p = new calibrationProperties;
    c_p->mesh = new ofMesh;

    //we can use 5 vertex to close it clockwise, or change the order to match Tri-strip sequence
    c_p->mesh->setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    setDestinationPoints();

}

void calibration::update(unsigned char* pixels) {
    if (!isCalibrated) {
        colorImage.setFromPixels(pixels, w, h);
        grayImage = colorImage;
        calibrate();
    }
}

void calibration::draw() {
    if (!isCalibrated) {
        calibrationImage.draw(0, 0, wWin, h);
        ofDrawBitmapString(ofToString(artk.getNumDetectedMarkers()), w - 40, 40);
//        grayImage.draw(0, 0);
        ofPushMatrix();
        {
            ofTranslate(wWin, h - 240);
            ofScale(0.25, 0.25);
            ofSetHexColor(0x00FFff);
            for (int i = 0; i < corners.size(); i++) {
                ofDrawBitmapString(ofToString(i), corners[i].x, corners[i].y);
            }
        }
        ofPopMatrix();
    }
}

void calibration::calibrate() {
    artk.update(grayImage.getPixels());
    int myIndex = artk.getMarkerIndex(1);
    if (myIndex >= 0) {
        // TODO - this should happen only after some time period
        isCalibrated = true;

        // Get the corners
        setSourcePoints(myIndex);
        updateMesh();
    }
    
}

void calibration::setDestinationPoints() {
    destinationPoints.clear();
    destinationPoints.push_back(ofVec3f(0, 0, 0));
    destinationPoints.push_back(ofVec3f(0, h, 0));
    destinationPoints.push_back(ofVec3f(wWin, 0, 0));
    destinationPoints.push_back(ofVec3f(wWin, h, 0));
}

void calibration::setSourcePoints(int index) {
    artk.getDetectedMarkerOrderedBorderCorners(index, corners);

    sourcePoints.clear();
    sourcePoints.push_back(ofVec2f(corners[0].x, corners[0].y));
    sourcePoints.push_back(ofVec2f(corners[3].x, corners[3].y));
    sourcePoints.push_back(ofVec2f(corners[1].x, corners[1].y));
    sourcePoints.push_back(ofVec2f(corners[2].x, corners[2].y));
}

void calibration::updateMesh() {
    c_p->mesh->clearTexCoords();
    c_p->mesh->clearVertices();
    c_p->mesh->addTexCoords(sourcePoints); // origin coordinates
    c_p->mesh->addVertices(destinationPoints);
}

calibrationProperties* calibration::getCalibrationProperties() {
    return c_p;
}
