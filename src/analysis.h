//
//  analysis.h
//  compChat
//
//  Created by ishac on 11/6/13.
//
//
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "sense.h"

#ifndef compChat_analysis_h
#define compChat_analysis_h

struct analysisProperties {
    ofVec2f pos;
    float area; //significance?
    float factor;
    float ratio;
    bool isEmpty;
};

class analysis
{
public:
    
//    senseProperties s_p;
    
    analysisProperties a_p;
    
    // transitional variables
    ofxCvBlob blob;
    ofxCvContourFinder contours;
    
    analysis();
    virtual ~analysis(){};
    
    void resetAnalysis();
    void updateAnalysis(senseProperties &_s_p);
    void setPos();
    void setArea();
    void setFactor();
    void setRatio();
    
    analysisProperties getAnalysisProperties();
    
protected:
private:
};

#endif
