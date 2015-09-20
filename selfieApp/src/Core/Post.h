//
//  Post.h
//
#pragma once

#include "ofxHttpUtils.h"

class Post : public ofxGlobalContext::Context
{
    ofxHttpForm form;
    ofxHttpUtils http;
    
public:
    
    Post()
    {
        ofAddListener(http.newResponseEvent,this,&Post::nResponse);
        http.start();
    }
    ~Post()
    {
        ofRemoveListener(http.newResponseEvent,this,&Post::nResponse);
        http.stop();
    }

    
    void filePost( string path, string url )
    {
//        form.clearFormFields();
//        http.clearQueue();
        
        cout<< path <<endl;
        
        form.action = HOST + url;
        form.method = OFX_HTTP_POST;
        form.addFile("file", path );
        http.addForm(form);
        string s = "file sent to server : " + url;
        ofLog( OF_LOG_NOTICE, s );
    }
    
    void nResponse(ofxHttpResponse & response) {
        if(response.status==200){
            string s = ofToString(response.status) + ": " + (string)response.responseBody;
            ofLog( OF_LOG_NOTICE, s );
        }
    }

    
    
    
};

