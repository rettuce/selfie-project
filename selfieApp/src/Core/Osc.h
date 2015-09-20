#pragma once
#include "ofxGlobalContext.h"
#include "ofxKeyValueOSC.h"
#include "ofxXmlSettings.h"

class Osc : public ofxGlobalContext::Context, public ofxKeyValueOSC
{
public:
    Osc(int receiverport, int senderport, string senderip = "localhost" )
{
        setup( receiverport );
        setEnable(false);
        
        ofxXmlSettings data ;
        if ( data.loadFile("setting/setting.xml") ) {
            senderip = data.getValue("osc:sender_ip", senderip);
            senderport = data.getValue("osc:sender_port", senderport);
        }
        
        send.setup(senderip, senderport);
        
        ofxOscMessage m;
        m.setAddress("/oF/setup");
        sendOscMessage(m);
    }
    
    void sendOscMessage(ofxOscMessage m){
        send.sendMessage(m);
    }
    void update()
    {
        static ofEventArgs e;
        ofxKeyValueOSC::onUpdate(e);
    }
protected:
    ofxOscSender send;
};