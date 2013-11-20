#include "testApp.h"
#include <ar.h>


//--------------------------------------------------------------
void testApp::setup() {
    ofSetFrameRate(30);
    
    debug = false;
    
    rotate90 = false;
    
    ofBackground(0, 0, 0);
    ofEnableSmoothing();
    ofSetCircleResolution(100);
    

    if (env == 0) {
        w = ofGetWidth();
        h = ofGetHeight();
        camW = 640;
        camH = 480;
    } else {
        w = 720;
        h = 446;
        camW = 640;
        camH = 480;
    }
    
    movie.setDesiredFrameRate(30);
    movie.initGrabber(camW, camH);
    
    wWin = h;
    
    xOffset = (w - wWin) / 2;
    
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

    cursorBlinkInterval = 500; // the other's period
    myBlinkPeriod = 1000; // my period


    ofSetLineWidth(1);

    mode = CC_MODE_CALIBRATE;
    calibrationEnergy = 0;
    isCalibrated = false;
    frameIsNew = false;

    calibrationImage.loadImage("calibration_border.jpg");
    calibrationImage.resize(wWin, h);
    // calibrationImage.mirror(1, 0); //uncomment when using a mirror.

    //reserve memory for cv images
    fullSize.allocate(camW, camH);
    rgb.allocate(camW, camH);
    colorWarp.allocate(wWin, h, OF_IMAGE_COLOR);
    grayImage.allocate(camW, camH);
    grayThres.allocate(wWin, h);
    grayOfImage.allocate(wWin, h, OF_IMAGE_GRAYSCALE);
    fbo.allocate(wWin, h);
    markerFbo.allocate(camW, camH);
    colorMarker.allocate(camW, camH, OF_IMAGE_COLOR);
    blobFilled.allocate(wWin, h);
    currentEnvir.allocate(camW, camH);
    lastEnvir.allocate(camW, camH);
    diffEnvir.allocate(camW, camH);

    setDestinationPoints();
    
    // ARToolkit setup
    artk.setup(camW, camH);
    artk.setUndistortionMode(ofxARToolkitPlus::UNDIST_STD);
    artk.setThreshold(threshold);

    otherIsBall = false;
    otherIsPaddle = false;
    IAmBall = false;
    yPosBar = h - 80;
    barPongWidth = 200;
    barPongHeight = 40;
    ballInitRadius = 50;
    pos.set(wWin / 2, h / 2);
    velInit.set(6, 10);
    ratioMarkerArea = 0;
    waitTime = 5000;
}

//--------------------------------------------------------------
void testApp::update() {
    movie.update();
    
    if (movie.isFrameNew()) {
        rgb.setFromPixels(movie.getPixels(), camW, camH);
        rgb.updateTexture();
        frameIsNew = true;
    } else {
        frameIsNew = false;
    }
    
    if (needsRecalibration){
        mode = CC_MODE_CALIBRATE;
        needsRecalibration = false;
    }

    switch (mode) {

    case CC_MODE_READ:
        rgbToFbo();
        fboToColorWarp();
        colorWarpToGrayThres();
        checkEnvironment();
        //adjustSensitivity();

        break;

    case CC_MODE_DISPLAY:

        break;

    case CC_MODE_CALIBRATE:
        grayImage = rgb;
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
            } else if (blobArea == 0) {
                blobArea = contours.blobs[0].area;
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

        contours.findContours(grayThres, 100, wWin * h, 1, false);

        blobFilled.set(0);
            //cout << contours.nBlobs << endl;
            
        if (ILost) {
            cout << "I lost." << endl;
            if (loseTime == 0) {
                loseTime = ofGetElapsedTimeMillis(); // we start counting
            } else {
                if ((ofGetElapsedTimeMillis() - loseTime) > waitTime){
                    IAmBall = true;
                    IAmPaddle = false;
                    otherIsBall = false;
                    otherIsPaddle = false;
                    loseTime = 0;
                    pos.x = wWin / 2;
                    pos.y = h / 2;
                    vel = velInit;
                    ballRadius = ballInitRadius;
                    ILost = false;
                    otherLost = false;
                    cout << "I'm setting myself to Ball" << endl;
                }
            }
            
        }
        
        else if (contours.nBlobs) {
            blobFilled.drawBlobIntoMe(contours.blobs[0], 255); //draws the outline of the blob into the blobFilled image
            blobEnergy += 1;
            if (blobEnergy >= 0) {
                blobEnergy = blobStable;

                if ( !(IAmBall || IAmPaddle) ) {
                    // I'm not the ball or paddle, so check if the other is the ball
                    if (checkOtherIsBall()) {
                        // other is ball, so I'm paddle
                        IAmBall = false;
                        IAmPaddle = true;
                        otherIsBall = true;
                        otherIsPaddle = false;
                        otherLost = false;
                    } else {
                        IAmBall = true;
                        IAmPaddle = false;
                        otherIsBall = false;
                        otherIsPaddle = false;
                        vel = velInit;
                        ballRadius = ballInitRadius;
                    }
                } else if (IAmBall) {
                    // I'm the ball
                    
                    if (otherLost) {
                        cout << "I'm ball, other has lost." << endl;
                        // The other has lost, shrink me down then check the other until it shows the ball.
                        if (ballRadius > 0){
                            ballRadius--;
                        } else {
                            if (checkOtherIsBall()) {
                                IAmPaddle = true;
                                IAmBall = false;
                                otherIsBall = true;
                                otherIsPaddle = false;
                                otherLost = false;
                            }
                        }
                    }
                    
                    else if ( !(otherIsBall || otherIsPaddle) ) {
                        
                        if (checkOtherIsPaddle()) {
                            otherIsPaddle = true;
                            otherIsBall = false;
                        } else {
                            // the other isn't anything yet, assume beginning of game.
                            // Can we just set the other to the paddle here? Or do we have to check?
                            pos.x = wWin/2;
                            pos.y = h/2;
                            otherIsPaddle = false;
                        }

                    }
                    
                    else if (otherIsPaddle) {
                        // update pos
                        cout << "I'm ball, other is paddle" << endl;
                        // other is paddle, keep moving and check walls and paddle bouncing
                        vel.x = vel.x * 1.001;
                        vel.y = vel.y * 1.001;
                        pos += vel;
                        checkWalls();
                        checkBar();
                        xPosBar = contours.blobs[0].centroid.x;
                    }
                    
                    else if (otherIsBall) {
                        cout << "I'm ball, other is ball (?)" << endl;
                        // Something is wrong!
                    }
                    
                } else if (IAmPaddle) {
                    
                    if (checkOtherIsBall()) {
                        otherIsBall = true;
                        xPosBar = contours.blobs[0].centroid.x;
                    } else {
                        // add energy
                        IAmPaddle = false;
                        otherIsBall = false;
                        ILost = true;
                    }
                    
                }
            }
            
        } else {                    // there are no blobs
            
            blobEnergy -= 1;
            if (blobEnergy <= 0) {
                blobEnergy = 0;
                cout << "no blobs" << endl;
                if (IAmBall) {
                    if (otherLost) {
                        IAmPaddle = false;
                        IAmBall = false;
                    } else {
                        // other hasn't lost, but there is no blob... obstruction?  Just wait.
                    }
                }
                
                if (IAmPaddle) {
                    ILost = true;
                }
                
                if (ILost) {
                    if (loseTime == 0) {
                        loseTime = ofGetElapsedTimeMillis(); // we start counting
                    } else {
                        if ((ofGetElapsedTimeMillis() - loseTime) > waitTime){
                            IAmBall = true;
                            IAmPaddle = false;
                            otherIsBall = false;
                            otherIsPaddle = false;
                            loseTime = 0;
                            pos.x = wWin / 2;
                            pos.y = h / 2;
                            cout << "I'm setting myself to Ball" << endl;
                        }
                    }
                    
                }

            }
        }
        break;
    }
}

//--------------------------------------------------------------
void testApp::draw() {
    ofSetColor(255, 255, 255);

    
    ofPushMatrix();

    
//    ofPopMatrix();
//    
//    ofPushMatrix();
    
    ofTranslate(xOffset, 0);
    
    if (rotate90) {
        ofTranslate(wWin, 0);
        ofRotate(90);
    }

    
    switch (mode) {

    case CC_MODE_READ: {
        drawRGB();
        drawData();
        colorWarp.draw(0, 0, wWin, h);
        drawEnvironment();
        break;
    }

    case CC_MODE_DISPLAY: {
        drawCircle();
        break;
    }

    case CC_MODE_CALIBRATE: {
        drawCalibration();

        drawRGB();
        drawData();

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
            ofTexture texture = rgb.getTextureReference();
            artk.getDetectedMarkerOrderedBorderCorners(myIndex, corners);
            setSourcePoints(texture, corners);
            updateMesh();
            
            drawARCorners(corners);
            
            getMarkerImage();

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
            ofRect(100, 100, 80, 160);
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
        drawData();
        drawRGB();
        drawBlobFilled();
        
        if (IAmPaddle) {
            drawBar();
        } else if (IAmBall){
            drawBall();
        }
        break;
    }

    }
    
    ofPopMatrix();
}

void testApp::checkEnvironment() {
    if (frameIsNew) {
        currentEnvir = rgb;
        pixMarkerCV = markerCV.getPixels();
        pixCurrentEnvir = currentEnvir.getPixels();
        for (int i=0; i < markerCV.width * markerCV.height; i++){
            if (pixMarkerCV[i] == 255){
                pixCurrentEnvir[i] = 255;
            }
        }
        
        diffEnvir = currentEnvir;
        diffEnvir.absDiff(lastEnvir);

        diffEnvir.threshold(127);
        int nWhitePixels = diffEnvir.countNonZeroInRegion(0, 0, camW, camH);
        if (nWhitePixels > 500){
            calibrationEnergy++;
        } else {
            calibrationEnergy--;
            if (calibrationEnergy < 0){
                calibrationEnergy = 0;
            }
        }
        if (calibrationEnergy >= 5){
            needsRecalibration = true;
            calibrationEnergy = 0;
        }
        
        lastEnvir = currentEnvir;
    }
}

void testApp::drawEnvironment() {
    if (!debug) return;
    ofPushMatrix();
    {
        ofTranslate(wWin, 0);
        ofScale(0.25, 0.25);
        colorMarker.draw(0, 0);
        markerCV.draw(0, camH);
        lastEnvir.draw(0, 2*camH);
        currentEnvir.draw(0, 3*camH);
        diffEnvir.draw(0, 4*camH);
        
    }
    ofPopMatrix();
}

void testApp::getMarkerImage(){
    colorMarker.clear();
    markerCV.clear();
    markerQuad.clear();
    
    markerQuad.moveTo(sourcePoints[0]);
    markerQuad.lineTo(sourcePoints[2]);
    markerQuad.lineTo(sourcePoints[3]);
    markerQuad.lineTo(sourcePoints[1]);
    markerQuad.close();
    
    {
        markerFbo.begin();
        ofClear(0);
        ofFill();
        ofSetColor(255);
        markerQuad.draw();
        markerFbo.end();
    }
    
    markerFbo.readToPixels(colorMarker.getPixelsRef());
    colorMarker.setImageType(OF_IMAGE_GRAYSCALE);
    colorMarker.update();
    //colorMarker.draw(0,0);

    markerCV.setFromPixels(colorMarker.getPixelsRef()); //of to CV image
    markerCV.updateTexture();
    //markerCV.draw(0, 400);
}

void testApp::setSourcePoints(ofTexture &texture, vector<ofPoint> &corners) {
    ofVec2f p0 = texture.getCoordFromPoint(corners[0].x, corners[0].y);
    ofVec2f p3 = texture.getCoordFromPoint(corners[3].x, corners[3].y);
    ofVec2f p1 = texture.getCoordFromPoint(corners[1].x, corners[1].y);
    ofVec2f p2 = texture.getCoordFromPoint(corners[2].x, corners[2].y);
    
    sourcePoints.clear();
    sourcePoints.push_back(p0);
    sourcePoints.push_back(p3);
    sourcePoints.push_back(p1);
    sourcePoints.push_back(p2);
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
                if (blinkCount > 1){
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

float testApp::getRatioMarkerArea(){
    if (contours.nBlobs){
        ratioMarkerArea = (float(grayThres.countNonZeroInRegion(0, 0, grayThres.width, grayThres.height))) / float((grayThres.width * grayThres.height));
    }
    return ratioMarkerArea;

}


bool testApp::checkOtherIsBall() {
    float areaBoundingBox = contours.blobs[0].boundingRect.width * contours.blobs[0].boundingRect.height;
    float ratio = contours.blobs[0].boundingRect.width / contours.blobs[0].boundingRect.height;
    // we can use the area of blob to area of bounding box.
    // or we can use the ratio between height and width of the bounding box. We Do That.
    cout << getRatioMarkerArea() << endl;
    if ((ratio < 1.5) && (getRatioMarkerArea() < 0.1)) {
        // The other is the ball
        return true;
    } else {
        return false;
    }
}

bool testApp::checkOtherIsPaddle() {
    float areaBoundingBox = contours.blobs[0].boundingRect.width * contours.blobs[0].boundingRect.height;
    float ratio = contours.blobs[0].boundingRect.width / contours.blobs[0].boundingRect.height;
    // we can use the area of blob to area of bounding box.
    // or we can use the ratio between height and width of the bounding box. We Do That.
    cout << getRatioMarkerArea() << endl;
    if ((ratio >= 1.5) && (getRatioMarkerArea() < 0.1)) {
        // The other is the paddle
        return true;
    }
    return false;
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

    if (pos.y + ballRadius > yPosBar - barPongHeight / 2) {
        if ((pos.x > xPosBar + barPongWidth / 2) || (pos.x < xPosBar - barPongWidth / 2)) {
            otherLost = true;
            vel = velInit;
            cout << "LOSE!!!" << endl;
        } else {
            pos.y = yPosBar - barPongHeight / 2 - ballRadius - 5;
            vel.y = -vel.y;
            cout << "good" << endl;
        }
    }
}

void testApp::drawBall() {

    ofSetColor(255);
    ofCircle(pos.x, pos.y, ballRadius);
}

void testApp::adjustSensitivity() {
    pixels = grayThres.getPixels();
    float white, black;
    for (int i=0; i < grayThres.width * grayThres.height; i++){
        if (pixels[i] == 0){
            black++;
        } else if (pixels[i] == 255){
            white++;
        }
    }
    calibrationBWRatio = white / black;
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
    if (!debug) return;
    ofPushMatrix();
    {
        ofTranslate(h + 50, 50);
        ofDrawBitmapString("Blobs: " + ofToString(contours.nBlobs), 0, 0);
        ofDrawBitmapString("blinkCount:         " + ofToString(blinkCount), 0, 40);
        ofDrawBitmapString("myPeriod:           " + ofToString(myBlinkPeriod), 0, 50);
        ofDrawBitmapString("The others Period:  " + ofToString(cursorBlinkInterval), 0, 60);
        ofDrawBitmapString("RATIO B/W:          " + ofToString(calibrationBWRatio), 0, 70);
        if (contours.nBlobs) {
            ofDrawBitmapString("Area:           " + ofToString(contours.blobs[0].area), 0, 10);
            ofDrawBitmapString("Corrected Area: " + ofToString(blobArea), 0, 20);
            ofDrawBitmapString("Factor:         " + ofToString(factor), 0, 30);
            float progress = float(barMineCurrent)/float(barLength) * 100;
            ofDrawBitmapString("Bar progress:   " + ofToString(progress) + "%", 0, 80);
        }
        ofDrawBitmapString("I'm ball:           " + ofToString(IAmBall), 0, 90);
        ofDrawBitmapString("I'm paddle:         " + ofToString(IAmPaddle), 0, 100);
        ofDrawBitmapString("Other is ball:      " + ofToString(otherIsBall), 0, 110);
        ofDrawBitmapString("Other is paddle:    " + ofToString(otherIsPaddle), 0, 120);
        ofDrawBitmapString("Other lost:         " + ofToString(otherLost), 0, 130);
        ofDrawBitmapString("I lost:             " + ofToString(ILost), 0, 140);
        ofDrawBitmapString("Lose time:          " + ofToString(loseTime), 0, 150);
        ofDrawBitmapString("Blob Energy:        " + ofToString(blobEnergy), 0, 160);
    }
    ofPopMatrix();
}

void testApp::drawARCorners(vector<ofPoint> &corners) {
    if (!debug) return;
    ofPushMatrix();
    {
        ofTranslate(wWin, h - 240);
        ofScale(0.5, 0.5);
        //                ofSetHexColor(0x00FFff);
        for (int i = 0; i < corners.size(); i++) {
//            cout << corners[i].x << ", " << corners[i].y << endl;
            ofDrawBitmapString(ofToString(i), corners[i].x, corners[i].y);
        }
    }
    ofPopMatrix();
}


void testApp::drawRGB() {
    if (!debug) return;
    ofPushMatrix();
    {
        ofTranslate(wWin, h - 240);
        ofScale(0.25, 0.25);
        rgb.draw(0, 0);

    }
    ofPopMatrix();

}

void testApp::drawBlobFilled() {
    if (!debug) return;
    ofPushMatrix();
    {
        ofTranslate(h, h - 480);
        ofScale(0.25, 0.25);
        ofSetColor(255);
        
        blobFilled.draw(0, 0);
        colorWarp.draw(0, 0);
        contours.draw();
        if (contours.nBlobs) {
            ofSetColor(255, 0, 0);
            ofFill();
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
    mesh.clearNormals();
    mesh.addTexCoords(sourcePoints); // origin coordinates
    mesh.addVertices(destinationPoints);
}

void testApp::setDestinationPoints() {
    // SETUP MESH
    destinationPoints.clear();
    //we can use 5 vertex to close it clockwise, or change the order to match Tri-strip sequence
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    destinationPoints.push_back(ofVec3f(0, 0, 0));
    destinationPoints.push_back(ofVec3f(0, h, 0));
    destinationPoints.push_back(ofVec3f(wWin, 0, 0));
    destinationPoints.push_back(ofVec3f(wWin, h, 0));
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
        ballRadius = ballInitRadius;
        vel = velInit;
        IAmBall = IAmPaddle = otherIsPaddle = otherIsBall = false;
        //checkTheOther(); //checks the area of the blob compared to the bounding box to identify if it's a circle or a rectangle.
        break;

    case '9':
        mode = CC_MODE_CURSOR;
        freqsSynched = false;
        break;
    
    case 'd':
        debug = !debug;
        break;
            
    case 'r':
        rotate90 = !rotate90;
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
