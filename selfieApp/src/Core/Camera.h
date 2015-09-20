//
//  Camera.h
//
#pragma once

class Camera : public ofxGlobalContext::Context, public ofEasyCam
{
    ofVec3f campos,camtarget;
    ofQuaternion curRot;
    bool isStatic;

public:
    
    void setup()
    {
        ofEasyCam::disableMouseInput();
        campos.set( 0 );
        camtarget.set( 0 );
    }
    
    void update()
    {
        Osc* osc = $G(Osc);
        ofxOscMessage m;
        
        if(osc->getLatest("/cam/pos", m)){
            changePosition(ofVec3f(m.getArgAsFloat(0),m.getArgAsFloat(1),m.getArgAsFloat(2)));
        };
        if(osc->getLatest("/cam/target", m)){
            changeTarget(ofVec3f(m.getArgAsFloat(0),m.getArgAsFloat(1),m.getArgAsFloat(2)));
        };
        this->setPosition(campos);
        this->setTarget(camtarget);
    }
    void changePosition(ofVec3f t) {
        Tweenzor::add( &campos, campos, t, 0.f, 0.1, EASE_OUT_EXPO );
    }
    void changeTarget(ofVec3f t) {
        Tweenzor::add( &camtarget, camtarget, t, 0.f, 0.1, EASE_OUT_EXPO );
    }
    
};