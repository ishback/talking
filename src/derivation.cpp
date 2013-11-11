//
//  derivation.cpp
//  compChat
//
//  Created by ishac on 11/7/13.
//
//

#include "derivation.h"

derivation::derivation(){
    analysisStable = 5;
    numStoredAnalyses = 5;
}

//------------------------------------------------------------
void derivation::resetInput() {
    
    d_p.velocity.zero();
    d_p.areaGrowth = 0;
    d_p.frequency = 0;
    d_p.blinkCount = 0;
    d_p.blinkOn = false;
    
    energy = 0;
    lastBlinkTime = 0;
}


//------------------------------------------------------------
void derivation::updateDerivation(analysisProperties &_a_p) {
    
    a_p = _a_p;
    
    storeAnalysis();
    
    if (!a_p.isEmpty) { // blob detected
        energy += 1;
        if (energy >= analysisStable) {
            energy = analysisStable;
            if (!d_p.blinkOn) {
                d_p.blinkOn = true;
                d_p.blinkCount += 1;
                setFrequency();
            }
            if (!analyses[analyses.size()-2].isEmpty)
            setVelocity();
            setAreaGrowth();
        }
    } else {
        energy -= 1;
        if (energy <= 0) {
            energy = 0;
            d_p.blinkOn = false;
        }
    }
}


//------------------------------------------------------------
void derivation::storeAnalysis() {
    analyses.push_back(a_p);
    if (analyses.size() > numStoredAnalyses){
        analyses.erase(analyses.begin());
    }
}

void derivation::setVelocity() {
    
    d_p.velocity = a_p.pos - analyses[analyses.size()-2].pos;
    
    // we can average the last 5 measured velocities if needed.
//    if (inputStable && !i.isEmpty){
//        int sumCounter = 0;
//        for (int j = inputs.size() - 1; (!inputs[j].isEmpty && !inputs[j-1].isEmpty); j--) {
//            velocity += inputs[j].pos - inputs[j-1].pos;
//            sumCounter++;
//        }
//        velocity = velocity / sumCounter;
//    }
    
}

void derivation::setAreaGrowth() {
    // we can average the last 5 measured growths if needed.
    d_p.areaGrowth = (a_p.area - analyses[analyses.size()-2].area) / analyses[analyses.size()-2].area;
}

void derivation::setFrequency() {
    // we can average the last 5 measured frecuencies if needed.
    float blinkPeriod = ofGetElapsedTimeMillis() - lastBlinkTime;
    d_p.frequency = 1000 / blinkPeriod;
    lastBlinkTime = ofGetElapsedTimeMillis();
}
