#include "testApp.h"
#include <ar.h>


//--------------------------------------------------------------
void testApp::setup() {
    ofBackground(0, 0, 0);
    ofEnableSmoothing();
    ofSetCircleResolution(100);

    w = ofGetWidth();
    h = ofGetHeight();
    wWin = h;
    blobArea = 0;
    threshold = 127;
    factor = 0;

    barLength = wWin - 200;
    barHeight = 40;
    barMineCurrent = 0;
    barOtherCurrent = 0;

    mode = CC_MODE_TEST;
    isCalibrated = false;

    //    movie.listDevices();
    //    movie.setDeviceID(2);
    //    movie.setVerbose(true);
    // movie.initGrabber(w, h, true);



    calibrationImage.loadImage("calibration.jpg");
    calibrationImage.resize(h, h);

    //reserve memory for cv images
    rgb.allocate(w, h);
    colorWarp.allocate(h, h, OF_IMAGE_COLOR);
    grayImage.allocate(w, h);
    grayThres.allocate(h, h);
    blobFilled.allocate(w, h);
    grayOfImage.allocate(h, h, OF_IMAGE_GRAYSCALE);
    fbo.allocate(h, h);

    resized.allocate(w, h);

    initDisplayMode();
    initReadMode();

    artk.setup(w, h);
    artk.setUndistortionMode(ofxARToolkitPlus::UNDIST_STD);
    artk.setThreshold(threshold);
}

//--------------------------------------------------------------
void testApp::update() {

    movie->update();

    // if (movie->isFrameNew()) {
    //     ofPixels pix = movie->getPixels();
    //     rgb.setFromPixels(pix.getPixels(), w, h);
    // }

    // factor = 1 + (h/2 - float(mouseY))/(h/2) * 1;

    // switch (mode) {

    // case CC_MODE_READ:
    //     // cout << "READ" << endl;

    //     break;

    // case CC_MODE_DISPLAY:

    //     break;

    // case CC_MODE_CALIBRATE:
    //     // convert our camera image to grayscale
    //     grayImage = rgb;

    //     // Pass in the new image pixels to artk
    //     artk.update(grayImage.getPixels());
    //     break;

    // case CC_MODE_THRESHOLD:


    //         rgbToFbo();
    //         fboToColorWarp();
    //         colorWarpToGrayThres();


    //     break;

    //     case CC_MODE_CONTOURS:

    //         rgbToFbo();
    //         fboToColorWarp();
    //         colorWarpToGrayThres();

    //         contours.findContours(grayThres, 100, w*h/2, 1, false);

    //         if (contours.nBlobs) {
    //             blobArea = contours.blobs[0].area * factor;
    //         }


    //         break;

    //     case CC_MODE_PROGRESS_BAR:

    //         rgbToFbo();
    //         fboToColorWarp();
    //         colorWarpToGrayThres();

    //         contours.findContours(grayThres, 100, w*h/2, 1, false);


    //         blobFilled.set(0);


    //         if (contours.nBlobs) {
    //             blobArea = contours.blobs[0].area * factor;
    //             cout << contours.blobs[0].area << endl;
    //             blobFilled.drawBlobIntoMe(contours.blobs[0], 255);
    //         }

    //         break;
    // }
}

//--------------------------------------------------------------
void testApp::draw() {

    if (movie->isFrameNew()) {
        ofPixels pix = movie->getPixels();
        rgb.setFromPixels(pix.getPixels(), w, h);
        cout << "new frame" << endl;
    }

    factor = 1 + (h / 2 - float(mouseY)) / (h / 2) * 1;

    switch (mode) {
    case CC_MODE_TEST:
        movie->draw(0, 0, 100, 100);
        break;

    case CC_MODE_READ:
        // cout << "READ" << endl;

        break;

    case CC_MODE_DISPLAY:

        break;

    case CC_MODE_CALIBRATE:
        // convert our camera image to grayscale
        grayImage = rgb;

        // Pass in the new image pixels to artk
        artk.update(grayImage.getPixels());
        break;

    case CC_MODE_THRESHOLD:


        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();


        break;

    case CC_MODE_CONTOURS:

        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();

        contours.findContours(grayThres, 100, w * h / 2, 1, false);

        if (contours.nBlobs) {
            blobArea = contours.blobs[0].area * factor;
        }


        break;

    case CC_MODE_PROGRESS_BAR:

        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();

        contours.findContours(grayThres, 100, w * h / 2, 1, false);


        blobFilled.set(0);


        if (contours.nBlobs) {
            blobArea = contours.blobs[0].area * factor;
            cout << contours.blobs[0].area << endl;
            blobFilled.drawBlobIntoMe(contours.blobs[0], 255);
        }

        break;
    }




    ofSetColor(255, 255, 255);

    switch (mode) {

    case CC_MODE_READ: {
        drawRGB();
        rgb.getTextureReference().bind();
        mesh.draw();
        rgb.getTextureReference().unbind();
        break;
    }

    case CC_MODE_DISPLAY: {
        drawCircle();
        break;
    }

    case CC_MODE_CALIBRATE: {
        drawCalibration();

        drawRGB();

        // ARTK 2D stuff
        // See if marker ID '0' was detected
        // and draw blue corners on that marker only

        int myIndex = artk.getMarkerIndex(1);
        ofDrawBitmapString(ofToString(artk.getNumDetectedMarkers()), w - 40, 40);
        if (myIndex >= 0) {
            // TODO - this should happen only after some time period
            isCalibrated = true;
            //            mode = CC_MODE_READ;

            // Get the corners
            vector<ofPoint> corners;
            artk.getDetectedMarkerOrderedBorderCorners(myIndex, corners);

            sourcePoints.clear();
            sourcePoints.push_back(ofVec2f(corners[0].x, corners[0].y));
            sourcePoints.push_back(ofVec2f(corners[3].x, corners[3].y));
            sourcePoints.push_back(ofVec2f(corners[1].x, corners[1].y));
            sourcePoints.push_back(ofVec2f(corners[2].x, corners[2].y));

            updateMesh();

            // Can also get the center like this:
            // ofPoint center = artk.getDetectedMarkerCenter(myIndex);
            ofPushMatrix();
            {
                ofTranslate(h, h - 240);
                ofScale(0.25, 0.25);
                ofSetHexColor(0x00FFff);
                for (int i = 0; i < corners.size(); i++) {
                    ofDrawBitmapString(ofToString(i), corners[i].x, corners[i].y);
                }
            }
            ofPopMatrix();
        }

        break;
    }

    case CC_MODE_THRESHOLD: {

        drawRGB();
        grayThres.draw(0, 0);

        break;
    }

    case CC_MODE_CONTOURS: {

        drawRGB();
        ofSetColor(255);
        ofFill();
        if (contours.nBlobs) {
            float radius = sqrt(blobArea / PI);
            cout << radius << endl;
            ofCircle(contours.blobs[0].centroid.x, contours.blobs[0].centroid.y, radius);
        }

        break;
    }
    case CC_MODE_PROGRESS_BAR: {

        drawRGB();
        drawBlobFilled();
        contours.draw();
        ofSetColor(255);
        ofNoFill();
        ofRect((wWin - barLength) / 2, h / 2 - barHeight / 2, barLength, barHeight);
        if (contours.nBlobs) {
            barOtherCurrent = blobArea / barHeight;
            barMineCurrent = barOtherCurrent;
            ofFill();
            ofRect((wWin - barLength) / 2, h / 2 - barHeight / 2, ofClamp(barMineCurrent, 0, barLength), barHeight);
        }
        break;
    }

    }

}

void testApp::drawRGB() {
    ofPushMatrix();
    {
        ofTranslate(h, h - 240);
        ofScale(0.25, 0.25);
        rgb.draw(0, 0);
    }
    ofPopMatrix();
}

void testApp::drawBlobFilled() {
    ofPushMatrix();
    {
        ofTranslate(h, h - 480);
        ofScale(0.25, 0.25);
        blobFilled.draw(0, 0);
    }
    ofPopMatrix();
}

void testApp::rgbToFbo() {
    fbo.begin();

    rgb.getTextureReference().bind();
    mesh.draw();
    rgb.getTextureReference().unbind();

    fbo.end();
}

void testApp::fboToColorWarp() {
    fbo.readToPixels(colorWarp.getPixelsRef());
    colorWarp.update();
}

void testApp::colorWarpToGrayThres() {
    grayOfImage = colorWarp;
    grayOfImage.setImageType(OF_IMAGE_GRAYSCALE);
    grayThres.setFromPixels(grayOfImage.getPixelsRef()); // From OF to CV
    grayThres.threshold(threshold);
}



void testApp::drawCalibration() {
    calibrationImage.draw(0, 0);
}

void testApp::drawCircle() {
    ofFill();
    ofSetColor(255);
    ofCircle(ofPoint(h / 2, h / 2), 300);
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

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {

    switch (key) {

    case '1':
        if (isCalibrated) {
            mode = CC_MODE_DISPLAY;
        }
        break;

    case '2':
        if (isCalibrated) {
            mode = CC_MODE_READ;
        }
        break;

    case '3':
        mode = CC_MODE_CALIBRATE;
        isCalibrated = false;
        break;

    case '4':
        mode = CC_MODE_THRESHOLD;
        break;

    case '5':
        mode = CC_MODE_CONTOURS;
        break;

    case '6':
        mode = CC_MODE_PROGRESS_BAR;
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
