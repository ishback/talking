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
    factor = 1;
    lastArea = 0;

    barLength = wWin - 200;
    barHeight = barLength / 10;
    barMineCurrent = 0;
    barOtherCurrent = 0;

    // cursor
    cursorOn = false;
    
    cursorLastSwitchTime = ofGetElapsedTimeMillis();
    blinkCount = 0;
    blinkFreq = 0;
    blobEnergy = 0;
    blobStable = 5;
    blinkOn = false;
    cursorBlinkInterval = 500; // my period
    myBlinkPeriod = 300; // the other's period

    ofSetLineWidth(1);

    mode = CC_MODE_CALIBRATE;
    isCalibrated = false;

    movie.initGrabber(w, h, true);



    calibrationImage.loadImage("calibration.jpg");
    calibrationImage.resize(h, h);
    // calibrationImage.mirror(1, 0); //uncomment when using a mirror.

    //reserve memory for cv images
    rgb.allocate(w, h);
    colorWarp.allocate(wWin, h, OF_IMAGE_COLOR);
    grayImage.allocate(w, h);
    grayThres.allocate(wWin, h);
    grayOfImage.allocate(wWin, h, OF_IMAGE_GRAYSCALE);
    fbo.allocate(h, h);
    blobFilled.allocate(wWin, h);

    resized.allocate(w, h);

    initDisplayMode();
    initReadMode();

    artk.setup(w, h);
    artk.setUndistortionMode(ofxARToolkitPlus::UNDIST_STD);
    artk.setThreshold(threshold);

    // pongBall = false;
    yPosBar = h - 80;
    barPongWidth = 300;
    barPongHeight = 40;
    ballRadius = 50;
    pos.set(wWin / 2, h / 2);
    vel.set(6, 10);
}

//--------------------------------------------------------------
void testApp::update() {

    if (movie.isFrameNew()) {
        rgb.setFromPixels(movie.getPixels(), w, h);
    }
    
    movie.update();

    switch (mode) {

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

        contours.findContours(grayThres, 1000, w * h, 1, false);
        cout << contours.nBlobs << "  ";
        //blobFilled.set(0);
        //blobFilled.drawBlobIntoMe(contours.blobs[0], 255);

        if (contours.nBlobs) {
            if (blobArea != 0) {
                lastArea = blobArea;

                blobArea = contours.blobs[0].area;
                factor = ((lastArea - blobArea) / lastArea) / 2 + 1;
                //factor = (blobArea - lastArea) / 2;

                blobArea = blobArea * factor;
                //blobArea -= factor;
                cout << lastArea << "  " << blobArea << endl;
            } else if (blobArea == 0) {
                blobArea = contours.blobs[0].area;
                cout << "here" << blobArea << endl;
            }
        }

        break;

    case CC_MODE_PROGRESS_BAR:

        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();

        contours.findContours(grayThres, 1000, w * h, 1, false);
        blobFilled.set(0);

        updateBlink();
        
//        if (contours.nBlobs) {
//            factor = 1 + (h / 2 - float(mouseY)) / (h / 2) * 1;
//            blobArea = contours.blobs[0].area * factor;
//            blobFilled.drawBlobIntoMe(contours.blobs[0], 255); //draws the filled blob into the blobFilled image
//        }

        break;


    case CC_MODE_CURSOR:

        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();
        
        if ((ofGetElapsedTimeMillis() - cursorLastSwitchTime) > myBlinkPeriod/2) {
            cursorOn = !cursorOn;
            cursorLastSwitchTime = ofGetElapsedTimeMillis();
        }
        updateBlink();
        //syncFreqBlinks();
        
            
            
        break;

    case CC_MODE_MOUSE_POINTER:

        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();

        contours.findContours(grayThres, 50, w * h, 1, false);
        blobFilled.set(0);
        if (contours.nBlobs) {
            blobFilled.drawBlobIntoMe(contours.blobs[0], 255); //draws the outline of the blob into the blobFilled image
        }

        break;

    case CC_MODE_PONG:

        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();

        contours.findContours(grayThres, 1000, w * h, 1, false);
        blobFilled.set(0);
        if (contours.nBlobs) {
            blobFilled.drawBlobIntoMe(contours.blobs[0], 255); //draws the outline of the blob into the blobFilled image
        }
        checkIfBall(); //checks the area of the blob compared to the bounding box to identofy if it's a circle or a rectangle.

        if (pongBall) {
            vel.x = vel.x * 1.001;
            vel.y = vel.y * 1.001;
            pos += vel;
            checkWalls();
            checkBar();
            // xPosBar = mouseX; //just for testing
            xPosBar = contours.blobs[0].centroid.x;
        } else {
            // xPosBar = mouseX;
            xPosBar = contours.blobs[0].centroid.x;
        }
        break;
    }
}

//--------------------------------------------------------------
void testApp::draw() {
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
            artk.getDetectedMarkerOrderedCorners(myIndex, corners);

            sourcePoints.clear();
            sourcePoints.push_back(ofVec2f(corners[0].x, corners[0].y));
            sourcePoints.push_back(ofVec2f(corners[3].x, corners[3].y));
            sourcePoints.push_back(ofVec2f(corners[1].x, corners[1].y));
            sourcePoints.push_back(ofVec2f(corners[2].x, corners[2].y));

            updateMesh();
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

        break;
    }

    case CC_MODE_THRESHOLD: {

        drawRGB();
        grayThres.draw(0, 0);

        break;
    }

    case CC_MODE_CONTOURS: {

        drawData();
        drawRGB();
        drawBlobFilled();
        ofSetColor(255);
        ofFill();
        if (contours.nBlobs) {
            float radius = sqrt(blobArea / PI);
            cout << "radius " << radius << endl;
            ofCircle(contours.blobs[0].centroid.x, contours.blobs[0].centroid.y, radius);
        }
        break;
    }

    case CC_MODE_PROGRESS_BAR: {

        drawData();
        drawRGB();
        drawBlobFilled();

        ofSetColor(255);
        ofNoFill();
        ofRect((wWin - barLength) / 2, h / 2 - barHeight / 2, barLength, barHeight);
        barMineCurrent = blinkCount * 3;
        ofFill();
        ofRect((wWin - barLength) / 2, h / 2 - barHeight / 2, ofClamp(barMineCurrent, 0, barLength), barHeight);
        
//        if (contours.nBlobs) {
//            barOtherCurrent = blobArea / barHeight;
//            barMineCurrent = barOtherCurrent;
//            ofFill();
//            ofRect((wWin - barLength) / 2, h / 2 - barHeight / 2, ofClamp(barMineCurrent, 0, barLength), barHeight);
//        }
        break;
    }

    case CC_MODE_CURSOR: {

        drawData();
        drawRGB();
        drawBlobFilled();
        ofSetColor(255);
        ofFill();
        if (cursorOn) {
            ofRect(100, 100, 20, 40);
        }

        //contours.draw();
        break;
    }

    case CC_MODE_MOUSE_POINTER: {

        drawData();
        drawRGB();
        drawBlobFilled();
        ofSetColor(255);
        ofFill();
        if (contours.nBlobs && contours.blobs[0].area < 100000) {
            drawMouseCursor();
        }
        break;
    }

    case CC_MODE_PONG: {

        if (pongBall) {
            drawData();
            drawRGB();
            drawBlobFilled();
            drawBall();
            // drawBar(); // just for testing

        } else {
            drawData();
            drawRGB();
            drawBlobFilled();
            drawBar();
            break;
        }
    }

    }

}



void testApp::updateBlink() {
    contours.findContours(grayThres, 100, w * h, 1, false);

    if (contours.nBlobs) {
        // blob detected
//        blobEnergy += 1;
//        if (blobEnergy >= blobStable) {
//            blobEnergy = blobStable;
            if (!blinkOn) {
                blinkOn = true;
                if (blinkCount > 0){
                    cursorBlinkInterval = ofGetElapsedTimeMillis() - lastBlinkTime;
                    blinkFreq = 1000 / cursorBlinkInterval;
                    lastBlinkTime = ofGetElapsedTimeMillis();
                    syncFreqBlinks();
                }
                blinkCount += 1;
            }
//        }
    } else {
//        blobEnergy -= 1;
//        if (blobEnergy <= 0) {
//            blobEnergy = 0;
            blinkOn = false;
//        }
    }
}

void testApp::syncFreqBlinks(){
    int diffPeriods = myBlinkPeriod - cursorBlinkInterval;
    if ((abs(diffPeriods) > 10) && !freqsSynched){ // We only adjust if the diff is big enough
        myBlinkPeriod += (cursorBlinkInterval - myBlinkPeriod) / 2;
    } else {
        freqsSynched = true;
    }
}

void testApp::checkIfBall() {
    if (contours.nBlobs) {
        float areaBoundingBox = contours.blobs[0].boundingRect.width * contours.blobs[0].boundingRect.height;
        float ratio = contours.blobs[0].boundingRect.width / contours.blobs[0].boundingRect.height;
        // we compare the area of blob to area of bounding box. we could also compare the ratio between height and width of the bounding box.
        // if (contours.blobs[0].area > areaBoundingBox*0.85){ //it sees the bar, set to Ball.
        //     pongBall = true;

        // } else if (contours.blobs[0].area < areaBoundingBox*0.85){ // it's sees a ball, set to Bar
        //     pongBall = false;
        // }
        if (ratio > 1.5) {
            //it sees the bar, set to Ball.
            pongBall = true;
        } else {
            // it's sees a ball, set to Bar
            pongBall = false;
        }
    }
}

void testApp::checkWalls() {

    if (pos.x + ballRadius > wWin) {
        pos.x = wWin - ballRadius;
        vel.x = -vel.x;
        cout << "here1" << endl;
    } else if (pos.x - ballRadius < 0) {
        pos.x = 0 + ballRadius;
        vel.x = -vel.x;
        cout << "here2" << endl;
    }
    if (pos.y - ballRadius < 0) {
        pos.y = 0 + ballRadius;
        vel.y = -vel.y;
        cout << "here3" << endl;
    } else if (pos.y + ballRadius > h) {
        pos.y = h - ballRadius;
        vel.y = -vel.y;
        cout << "here4" << endl;
    }
}

void testApp::checkBar() {

    if (pos.y + ballRadius > yPosBar - barHeight / 2) {
        if ((pos.x > xPosBar + barPongWidth / 2) || (pos.x < xPosBar - barPongWidth / 2)) {
            pos.x = wWin / 2;
            pos.y = h / 2;
            cout << "OUT!!!" << endl;
        } else {
            pos.y = yPosBar - barPongHeight / 2 - ballRadius;
            vel.y = -vel.y;
            cout << "good" << endl;
        }


    }
}

void testApp::drawBall() {
    ofSetColor(255);
    ofCircle(pos.x, pos.y, ballRadius);
}

void testApp::drawBar() {

    if (contours.nBlobs) {
        ofSetColor(255);
        ofFill();
        ofRect(xPosBar - barPongWidth / 2, yPosBar - barPongHeight / 2, barPongWidth, barPongHeight);
    }
}

void testApp::drawMouseCursor() {
    ofPushMatrix();
    {
        //ofTranslate(contours.blobs[0].centroid.x, contours.blobs[0].centroid.x);
        ofTranslate(50, 50);
        ofRotate(-30);
        ofSetColor(255);
        ofFill();
        //ofTriangle(0, -12, -6, 4, 6, 4);
        ofSetPolyMode(OF_POLY_WINDING_NONZERO);
        ofBeginShape();
        ofVertex(0, -16);
        ofVertex(-8, 6);
        ofVertex(0, 0);
        ofVertex(8, 6);
        ofEndShape();
    }
    ofPopMatrix();

}

void testApp::drawData() {
    ofPushMatrix();
    {
        ofTranslate(h + 50, 50);
        ofDrawBitmapString("Blobs: " + ofToString(contours.nBlobs), 0, 0);
        ofDrawBitmapString("blinkCount:         " + ofToString(blinkCount), 0, 40);
        ofDrawBitmapString("myPeriod:           " + ofToString(myBlinkPeriod), 0, 50);
        ofDrawBitmapString("The others Period:  " + ofToString(cursorBlinkInterval), 0, 60);
        if (contours.nBlobs) {
            ofDrawBitmapString("Area:           " + ofToString(contours.blobs[0].area), 0, 10);
            ofDrawBitmapString("Corrected Area: " + ofToString(blobArea), 0, 20);
            ofDrawBitmapString("Factor:         " + ofToString(factor), 0, 30);
            float progress = float(barMineCurrent)/float(barLength) * 100;
            ofDrawBitmapString("Bar progress:   " + ofToString(progress) + "%", 0, 80);
        }
    }
    ofPopMatrix();
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
        ofSetColor(255);
        blobFilled.draw(0, 0);
        contours.draw();
        ofSetColor(255, 0, 0);
        ofFill();
        if (contours.nBlobs) {
            ofCircle(contours.blobs[0].centroid.x, contours.blobs[0].centroid.y, 10);
        }
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
            blinkCount = 0;
        break;

    case '7':
        mode = CC_MODE_MOUSE_POINTER;
        break;

    case '8':
        mode = CC_MODE_PONG;
        pos.set(wWin / 2, h / 2);
        break;

    case '9':
        mode = CC_MODE_CURSOR;
        freqsSynched = false;
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
