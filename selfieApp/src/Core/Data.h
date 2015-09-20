//
//  Data.h
//
#pragma once

#include "ofxQREncode.h"

class ImgObj {
public:
    ofVec2f pos;
    float angle = 0.;
    float alpha = 1.;
    ofImage *img;
    
    ImgObj(){
        pos.set(WIDTH*0.5,HEIGHT*-0.3);
    }
    void update(){
    }
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofFill();
            ofEnableAlphaBlending();
            ofSetColor( 255 , 255*alpha );
            ofTranslate( pos );
            ofRotate( angle );
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofRect(0,0, img->width+20, img->height+20 );
            img->draw( 0, 0 );
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
};

class Data : public ofxGlobalContext::Context
{
    ofxQREncode qrencode;
    int SCENE = -1;
    
public:
    
    ofImage QR;
    ImgObj qrimg;
    
    string KEY = "";
    
    Data(){
        qrimg.img = &QR;
    }
    
    // kinect
    float nearPer = 0.;

    // info
    bool isRunning = false;
    float progress = 0.;
    ofVec2f nowPointer = ofVec2f::zero();   // ball position SCENE:0
    ofRectangle rect;                       // rectangle area SCENE:2
    
    // add frame
    vector<ofPixels> addsPixs;
    
    void setNewUser()
    {
        addsPixs.clear();
        std::vector<ofPixels>().swap(addsPixs);

        progress = 0.;
        qrimg.pos.y = -1000;
        
        KEY = ofGetTimestampString("%Y%m%d-%H%M%S");
        string URL = "http://selfie.rettuce.com/?id=" + KEY;
        qrencode.saveFile( QR, URL );
        
        cout<< "UNIQUE KEY > " << KEY <<endl;
    }
    
    
    
    void setScene(int s) {
        SCENE = s;
    }
    int getScene(){ return SCENE; }
    
};

