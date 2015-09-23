//
//  WebCamera.h
//
#pragma once

#include "ofxUVC.h"
#include "ofxYAML.h"

class WebCamera : public BaseScene
{
    ofQTKitGrabber	vidGrabber;
    ofTexture tex;
    ofxUVC uvcControl;
    ofxYAML yaml;
    string cameraName;
    int camWidth, camHeight;
    
    vector<ofxUVCControl> controls;
    float dmyalp = 0.;
    
public:
    
    WebCamera() : BaseScene( "WebCamera", 1, NOUSE_CAMERA )
    {
        yaml.load("camera_settings.yml");
        
        int cameraToUse;
        yaml.doc["cameraToUse"] >> cameraToUse;
        
        int vendorId, productId, interfaceNum;
        yaml.doc["cameras"][cameraToUse]["vendorId"] >> vendorId;
        yaml.doc["cameras"][cameraToUse]["productId"] >> productId;
        yaml.doc["cameras"][cameraToUse]["interfaceNum"] >> interfaceNum;
        yaml.doc["cameras"][cameraToUse]["name"] >> cameraName;
        yaml.doc["cameras"][cameraToUse]["width"] >> camWidth;
        yaml.doc["cameras"][cameraToUse]["height"] >> camHeight;
        
        vidGrabber.initGrabber(camWidth, camHeight);
        
        
        int deviceId = 0;   ///////////////////////////////////////////////////////////// <- lab dev
        
        vector<string> availableCams = vidGrabber.listVideoDevices();
        
        for(int i = 0; i < availableCams.size(); i++){
            cout<< availableCams.at(i) <<endl;
            if(availableCams.at(i) == "HD Pro Webcam C920"){
                deviceId = i;
            }
        }
        vidGrabber.setDeviceID(deviceId);
        
        uvcControl.useCamera(vendorId, productId, interfaceNum);
        uvcControl.setAutoExposure(false);
        uvcControl.setAutoFocus(false);
        uvcControl.setAutoWhiteBalance(false);
        
        controls = uvcControl.getCameraControls();
        for (ofxUVCControl &c : controls ) {
            cout<< c.name <<endl;
            cout<< c.status <<endl;
        }
    }
    
    void setup(){}
    void close(){}
    void updateEnter(){}
    
    void updateScene()
    {
        Osc *osc = $G(Osc);
        ofxOscMessage m;

        if(osc->getLatest("/uvc/Exposure", m)){ uvcControl.setExposure(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/WhiteBalance", m)){ uvcControl.setWhiteBalance(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/Gain", m)){ uvcControl.setGain(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/Saturation", m)){ uvcControl.setSaturation(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/Brightness", m)){ uvcControl.setBrightness(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/Contrast", m)){ uvcControl.setContrast(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/Sharpness", m)){ uvcControl.setSharpness(m.getArgAsFloat(0)); };
        if(osc->getLatest("/uvc/AbsoluteFocus", m)){ uvcControl.setAbsoluteFocus(m.getArgAsFloat(0)); };
        
        if($G(Data)->isKinectScene()) {
            dmyalp = ofClamp(($G(Data)->nearPer-0.55)/0.45, 0,1);
            float AFocus = (1-$G(Data)->nearPer)/0.4;
            uvcControl.setAbsoluteFocus(AFocus);
            
        }else{
            dmyalp = 1.;
        }

        vidGrabber.update();
        tex.loadData(vidGrabber.getPixelsRef());
    }
    
    void drawScene()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofSetColor(255, 255*getAlpha());
            ofScale(-1,1);
            tex.draw(-camWidth,0, camWidth, camHeight);
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
    void drawRender()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofSetColor(255, 255*getAlpha() * dmyalp);
            ofScale(-1,1);
            tex.draw(-camWidth,0, camWidth, camHeight);
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
};
