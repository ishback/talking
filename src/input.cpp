//
//  input.cpp
//  compChat
//
//  Created by ishac on 11/6/13.
//
//

#include "input.h"

input::input(){

}

//------------------------------------------------------------
void input::resetInput(){
    pos.zero();
    area = 0;
    factor = 0;
    ratio = 0;
}

//------------------------------------------------------------
void input::updateInput(ofxCvGrayscaleImage grayThres){
    
    int w = grayThres.width;
    int h = grayThres.height;
    
    contours.findContours(grayThres, 1000, w*h , 1, false); //only one blob, no holes.
    if (contours.nBlobs){
        blob = contours.blobs[0]; //we always work with the biggest blob.

        setPos();
        setArea();
        setFactor();
        setRatio();
        isEmpty = false;
    } else {
        isEmpty = true;
    }

}

//------------------------------------------------------------
void input::setPos(){
    pos = blob.centroid;
}

void input::setArea(){
    area = blob.area;
}

void input::setFactor(){
    factor = blob.area / (blob.boundingRect.width * blob.boundingRect.height);
}

void input::setRatio(){
    ratio = blob.boundingRect.width / blob.boundingRect.height;
}


