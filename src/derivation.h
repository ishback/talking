//
//  derivation.h
//  compChat
//
//  Created by ishac on 11/7/13.
//
//

#include "ofMain.h"
#include "analysis.h"
#ifndef compChat_derivation_h
#define compChat_derivation_h

struct derivationProperties {
    ofVec2f velocity;
    float areaGrowth;
    float frequency;
    int blinkCount;
    bool blinkOn;
};

class derivation
{
public:
    
    analysisProperties a_p;
    derivationProperties d_p;
    

    // transitional variables
    int analysisStable;
    int energy;
    int numStoredAnalyses; //to increase robustness of derivation
    vector <analysisProperties> analyses;
    
    float lastBlinkTime;
    
    derivation();
    
    void resetInput();
    void updateDerivation(analysisProperties &a_p);
    void storeAnalysis();
    void setVelocity();
    void setAreaGrowth();
    void setFrequency();
    
    
protected:
private:
};



#endif
