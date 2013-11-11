//
//  analysis.cpp
//  compChat
//
//  Created by ishac on 11/6/13.
//
//

#include "analysis.h"

analysis::analysis(){

}

//------------------------------------------------------------
void analysis::resetAnalysis(){
    a_p.pos.zero();
    a_p.area = 0;
    a_p.factor = 0;
    a_p.ratio = 0;
}

//------------------------------------------------------------
void analysis::updateAnalysis(senseProperties &_s_p){
    
    int w = _s_p.grayThresh.width;
    int h = _s_p.grayThresh.height;
    
    contours.findContours(_s_p.grayThresh, 1000, w*h , 1, false); //only one blob, no holes.
    if (contours.nBlobs){
        blob = contours.blobs[0]; //we always work with the biggest blob.

        setPos();
        setArea();
        setFactor();
        setRatio();
        a_p.isEmpty = false;
    } else {
        a_p.isEmpty = true;
    }

}


analysisProperties analysis::getAnalysisProperties() {
    return a_p;
}

void analysis::setPos(){
    a_p.pos = blob.centroid;
}

void analysis::setArea(){
    a_p.area = blob.area;
}

void analysis::setFactor(){
    a_p.factor = blob.area / (blob.boundingRect.width * blob.boundingRect.height);
}

void analysis::setRatio(){
    a_p.ratio = blob.boundingRect.width / blob.boundingRect.height;
}



