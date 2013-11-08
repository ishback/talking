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

struct inputProperties {
    ofVec2f pos;
    float area; //significance?
    float factor;
    float ratio;
    bool isEmpty;
};

class input
{
public:
    
    senseProperties s_p;
    inputProperties i_p;
    
    // transitional variables
    ofxCvBlob blob;
    ofxCvContourFinder contours;
    
    input();
    virtual ~input(){};
    
    void resetInput();
    void updateInput(senseProperties s_p);
    void setPos();
    void setArea();
    void setFactor();
    void setRatio();
    
    
    
protected:
private:
};

#endif
