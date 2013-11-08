//
//  message.h
//  compChat
//
//  Created by ishac on 11/7/13.
//
//

#include "ofMain.h"
#include "input.h"
#ifndef compChat_message_h
#define compChat_message_h

struct messageProperties {
    ofVec2f velocity;
    float areaGrowth;
    float frequency;
    int blinkCount;
    bool blinkOn;
};

class message
{
public:
    
    inputProperties i_p;
    messageProperties m_p;
    

    // transitional variables
    int inputStable;
    int energy;
    int storeInputs; //to increase robustness of message
    vector <inputProperties> inputs;
    
    float lastBlinkTime;
    
    message();
    
    void resetInput();
    void updateMessage(inputProperties i_p);
    void storeInput();
    void setVelocity();
    void setAreaGrowth();
    void setFrequency();
    
    
protected:
private:
};



#endif
