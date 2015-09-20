//
//  BaseScene.h
//
#pragma once

enum USECAMERA{
    USE_CAMERA,
    NOUSE_CAMERA
};

class BaseScene
{
protected:
    float alpha;
    ofBlendMode blendmode;
    bool preVisible;
    USECAMERA usecam;
    
public:
    string name;
    
    BaseScene(const string& name, float alpha=0., USECAMERA ucam = USE_CAMERA, ofBlendMode blendmode = OF_BLENDMODE_ALPHA)
    : name(name)
    , alpha(alpha)
    , usecam(ucam)
    , blendmode(blendmode)
    { }
    
    virtual void update()
    {
        Osc* osc = $G(Osc);
        ofxOscMessage m;
        while (osc->get("/" + name + "/alpha", alpha));
        while (osc->get("/" + name + "/blendmode", (int&)blendmode));
        
        updateEver();
        
        if (isVisible()){
            if(!preVisible) updateEnter();
            updateScene();
        }
        preVisible = isVisible();
    }
    virtual void draw()
    {
        if(usecam==USE_CAMERA) $G(Camera)->ofEasyCam::begin();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushStyle();
        ofPushMatrix();
        ofEnableBlendMode(blendmode);
        {
            drawScene();
        }
        ofPopMatrix();
        ofPopStyle();
        glPopAttrib();
        if(usecam==USE_CAMERA) $G(Camera)->ofEasyCam::end();
    }
    virtual void render()
    {
        if(usecam==USE_CAMERA) $G(Camera)->ofEasyCam::begin();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushStyle();
        ofPushMatrix();
        ofEnableBlendMode(blendmode);
        {
            drawRender();
        }
        ofPopMatrix();
        ofPopStyle();
        glPopAttrib();
        if(usecam==USE_CAMERA) $G(Camera)->ofEasyCam::end();
    }
    
    virtual void setup(){}
    virtual void close(){}
    virtual void updateEnter(){}
    virtual void updateEver(){}
    virtual void updateScene() = 0;
    virtual void drawScene() = 0;
    virtual void drawRender(){};
    
    bool isVisible() const { return alpha > 0.01; }
    float getAlpha() const { return alpha; }
    void setAlpha(const float a ){ alpha = a; }
    ofBlendMode getBlendMode() const { return blendmode; }
};
