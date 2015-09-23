//
//  SceneManager.h
//
#pragma once

#include "BuildSetting.h"
#include "BaseScene.h"

#include "WebCamera.h"
#include "Kinect.h"
#include "Infomation.h"
#include "Effect.h"

class SceneManager : public ofxGlobalContext::Context
{
    vector<BaseScene*> layers;
    
    template <typename T>
    T* add(){
        T* o = new T;
        layers.push_back(o);
        o->setup();
        return o;
    }
    
public:
    ofFbo display, render;
    
    SceneManager()
    {
        ofEnableArbTex();
        {
            ofFbo::Settings s;
            s.width = WIDTH;
            s.height = HEIGHT;
            s.numSamples = 4;
            s.internalformat = GL_RGBA;
            s.depthStencilAsTexture = true;
            s.depthStencilInternalFormat = GL_DEPTH_COMPONENT24;
            s.useDepth = true;
            s.useStencil = true;
            display.allocate(s);
            
            ofFbo::Settings s2 = s;
            s2.internalformat = GL_RGB;
            s2.width = HEIGHT;
            s2.height = HEIGHT;
            render.allocate(s2);
        }
//        ofDisableArbTex();        
    }
    
    void setup()
    {
        add<WebCamera>();
        add<Kinect>();
        add<Infomation>();
        add<Effect>();
    }
    
    void update() {
        for (auto &it : layers) it->update();
        
        display.begin();
        {
            ofClear(0);
            ofSetColor($G(Color)->bgcolor);
            ofRect(0,0, WIDTH, HEIGHT);
            ofSetColor(255);
            drawLayer();
        }
        display.end();
        
        render.begin();
        {
            ofClear(0);
            ofSetColor($G(Color)->bgcolor, 255);
            ofRect(0,0, HEIGHT, HEIGHT);
            ofSetColor(255);
            drawRender();
        }
        render.end();
    }
    
    void draw() {
        display.draw(0, 0, WIDTH, HEIGHT);
        
#ifdef MODE_DEBBUG
        ofSetColor(0);
        float size = 200.;
//        float xx = ((float)WIDTH-size)*0.5;
        float xx = ((float)WIDTH-(30+size));
        ofRect( xx, 5, size, size);
        ofSetColor(255);
        render.draw( xx, 5, size, size);
#endif
    }
    
    void drawLayer() {
        for (auto &it : layers) {
            if( it->isVisible() ) it->draw();
        }
    }
    
    void drawRender() {
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(-((float)WIDTH-(float)HEIGHT)*0.5, 0);
        for (auto &it : layers) {
            if( it->isVisible() ) it->render();
        }
        ofPopMatrix();
        ofPopStyle();
    }
    
    void close() {
        for (auto &it : layers) it->close();
    }
};

