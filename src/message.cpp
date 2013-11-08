//
//  message.cpp
//  compChat
//
//  Created by ishac on 11/7/13.
//
//

#include "message.h"

message::message(){
    inputStable = 5;
    storeInputs = 5;
}

//------------------------------------------------------------
void message::resetInput() {
    
    m_p.velocity.zero();
    m_p.areaGrowth = 0;
    m_p.frequency = 0;
    m_p.blinkCount = 0;
    m_p.blinkOn = false;
    
    energy = 0;
    lastBlinkTime = 0;
}


//------------------------------------------------------------
void message::updateMessage(senseProperties s_p) {
    
    storeInput();
    if (!i_p.isEmpty) { // blob detected
        energy += 1;
        if (energy >= inputStable) {
            energy = inputStable;
            if (!m_p.blinkOn) {
                m_p.blinkOn = true;
                m_p.blinkCount += 1;
                setFrequency();
            }
            if (!inputs[inputs.size()-2].isEmpty)
            setVelocity();
            setAreaGrowth();
        }
    } else {
        energy -= 1;
        if (energy <= 0) {
            energy = 0;
            m_p.blinkOn = false;
        }
    }
}


//------------------------------------------------------------
void message::storeInput() {
    inputs.push_back(i_p);
    if (inputs.size() > storeInputs){
        inputs.erase(inputs.begin());
    }
}

void message::setVelocity() {
    
    velocity = i_p.pos - inputs[inputs.size()-2].pos;
    
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

void message::setAreaGrowth() {
    // we can average the last 5 measured growths if needed.
    m_p.areaGrowth = (i_p.area - inputs[inputs.size()-2].area) / inputs[inputs.size()-2].area;
}

void message::setFrequency() {
    // we can average the last 5 measured frecuencies if needed.
    float blinkPeriod = ofGetElapsedTimeMillis() - lastBlinkTime;
    m_p.frequency = 1000 / blinkPeriod;
    lastBlinkTime = ofGetElapsedTimeMillis();
}
