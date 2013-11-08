
#include "calibration.h"

void calibration::setup() {

    calibrationImage.loadImage("calibration.jpg");
    calibrationImage.resize(wWin, h);

    isCalibrated = false;

    artk.setup(w, h);
    artk.setUndistortionMode(ofxARToolkitPlus::UNDIST_STD);
    artk.setThreshold(threshold);

    setDestinationPoints();

}

void calibration::calibrate() {
    int myIndex = artk.getMarkerIndex(1);
    ofDrawBitmapString(ofToString(artk.getNumDetectedMarkers()), w - 40, 40);
    if (myIndex >= 0) {
        // TODO - this should happen only after some time period
        isCalibrated = true;

        // Get the corners
        vector<ofPoint> corners;
        setSourcePoints(myIndex, corners);
        updateMesh();


        // ofPushMatrix();
        // {
        //     ofTranslate(wWin, h - 240);
        //     ofScale(0.25, 0.25);
        //     ofSetHexColor(0x00FFff);
        //     for (int i = 0; i < corners.size(); i++) {
        //         ofDrawBitmapString(ofToString(i), corners[i].x, corners[i].y);

        //     }
        // }
        // ofPopMatrix();
    }
    
}

void calibration::setDestinationPoints() {
    // SETUP MESH
    destinationPoints.clear();
    //we can use 5 vertex to close it clockwise, or change the order to match Tri-strip sequence
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    destinationPoints.push_back(ofVec3f(0, 0, 0));
    destinationPoints.push_back(ofVec3f(0, h, 0));
    destinationPoints.push_back(ofVec3f(h, 0, 0));
    destinationPoints.push_back(ofVec3f(h, h, 0));
}

void calibration::setSourcePoints(int index, vector<ofPoint> &corners) {
    artk.getDetectedMarkerOrderedBorderCorners(index, corners);

    sourcePoints.clear();
    sourcePoints.push_back(ofVec2f(corners[0].x, corners[0].y));
    sourcePoints.push_back(ofVec2f(corners[3].x, corners[3].y));
    sourcePoints.push_back(ofVec2f(corners[1].x, corners[1].y));
    sourcePoints.push_back(ofVec2f(corners[2].x, corners[2].y));
}

void calibration::updateMesh() {
    mesh.clearTexCoords();
    mesh.clearVertices();
    mesh.addTexCoords(sourcePoints); // origin coordinates
    mesh.addVertices(destinationPoints);
}

ofMesh calibration::getMesh() {
	return mesh;
}