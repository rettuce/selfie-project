//
//  Camera.h
//
#pragma once

class Camera : public ofxGlobalContext::Context, public ofEasyCam
{
    ofVec3f campos,camtarget, staticpos,rotatepos;
    ofQuaternion curRot;
    bool isStatic;
    float rotateradius, isRotate, timescale;

public:
    
    void setup()
    {
        ofEasyCam::disableMouseInput();
        isRotate = true;
        rotateradius = 1000.;
        isRotate = 1.;
        timescale = 1.;
        campos.set( rotateradius, rotateradius , rotateradius );
        staticpos.set( rotateradius, rotateradius , rotateradius );
        camtarget.set(0, 0, 0);
    }
    
    void update()
    {
        Osc* osc = $G(Osc);
        ofxOscMessage m;
        
        osc->getLatest("/cam/rotate", isRotate);
        osc->getLatest("/cam/timescale", timescale);
        osc->getLatest("/cam/rotateradius", rotateradius);
        
        if(osc->getLatest("/cam/pos", m)){
            changePosition(ofVec3f(m.getArgAsFloat(0),m.getArgAsFloat(1),m.getArgAsFloat(2)));
        };
        if(osc->getLatest("/cam/target", m)){
            changeTarget(ofVec3f(m.getArgAsFloat(0),m.getArgAsFloat(1),m.getArgAsFloat(2)));
        };
        
        float time = ofGetFrameNum() / 60. * 0.1 * timescale;
        rotatepos.x = cos( time ) * rotateradius;
        rotatepos.z = sin( -time ) * rotateradius;
        rotatepos.y = sin( time*0.5 ) * rotateradius*0.7;
        
        campos = rotatepos * isRotate + staticpos*(1-isRotate);
        
        this->setPosition(campos);
        this->setTarget(camtarget);
    }
    void changePosition(ofVec3f t) {
        Tweenzor::add( &isRotate, isRotate, 0., 0.f, 0.3f, EASE_OUT_EXPO );
        Tweenzor::add( &staticpos, staticpos, t, 0.f, ofRandom(0.3,0.5), EASE_OUT_EXPO );
    }
    void changeTarget(ofVec3f t) {
        Tweenzor::add( &camtarget, camtarget, t, 0.f, ofRandom(0.3,0.5), EASE_OUT_EXPO );
    }
    
};