//
//  input.h
//  compChat
//
//  Created by ishac on 11/6/13.
//
//
#include "ofMain.h"
#include "ofxOpenCv.h"
#ifndef compChat_input_h
#define compChat_input_h

class input
{
public:
    // Input has five parameters
    ofVec2f pos;
    float area; //significance?
    float factor;
    float ratio;
    bool empty;
    
    // and two transitional variable
    ofxCvBlob blob;
    ofxCvContourFinder contours;
    
    input();
    virtual ~input(){};
    
    void resetInput();
    void updateInput( ofxCvGrayscaleImage grayThres);
    void setPos();
    void setArea();
    void setFactor();
    void setRatio();
    
    
    
protected:
private:
};

#endif
