//
//  Recorder.h
//
#pragma once

#include "ofxGifEncoder.h"

class Recorder : public ofxGlobalContext::Context
{
    int nFrames;
    ofxGifEncoder gifEncoder;
    ofPixels pixels;

public:
    
    Recorder(){
        nFrames = 0;
        gifEncoder.setup( OUT_GIFW, OUT_GIFH, 2., 256);
        ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &Recorder::onGifSaved);
    }
    ~Recorder(){
        gifEncoder.exit();
    }
    
    void init()
    {
        nFrames = 0;
        gifEncoder.reset();
    }
    
    void update()
    {
        Osc* osc = $G(Osc);
        ofxOscMessage m;
        
        if(osc->getLatest("/Recorder/capture", m)){
            capture( &$G(SceneManager)->render );
            $G(Audio)->seAdd();
        };
        if(osc->getLatest("/Recorder/add", m)){
            addFrame( &$G(SceneManager)->render );
            $G(Audio)->seAdd();
        };
        if(osc->getLatest("/Recorder/save", m)){
            gifEncoder.save("output/gif/"+ $G(Data)->KEY +".gif");
            $G(Audio)->seSave();
        };
        
    }
    
    void capture( ofFbo *fbo )
    {
        ofFbo copyFbo;
        copyFbo.allocate(OUT_GIFW,OUT_GIFH,GL_RGB);
        copyFbo.begin();
        fbo->draw(0, 0, OUT_GIFW,OUT_GIFH );
        copyFbo.end();
        copyFbo.readToPixels(pixels);

        ofSaveImage(pixels, "output/png/"+ $G(Data)->KEY +".png");

        string path = ofToDataPath( "output/png/" ).c_str()+$G(Data)->KEY+".png";
        $G(Post)->filePost( path, API_PNG );
    }
    
    void addFrame( ofFbo *fbo )
    {
        ofFbo copyFbo;
        copyFbo.allocate(OUT_GIFW,OUT_GIFH,GL_RGB);
        copyFbo.begin();
        fbo->draw(0, 0, OUT_GIFW,OUT_GIFH );
        copyFbo.end();
        copyFbo.readToPixels(pixels);
        
        if(nFrames!=0 || $G(Data)->getScene()!=2)
        {
            gifEncoder.addFrame( pixels.getPixels(),
                                OUT_GIFW,OUT_GIFH,
                                pixels.getBitsPerPixel(),
                                .1f
                                );
        }
        
        nFrames++;
        $G(Data)->addsPixs.push_back( pixels );
    }
    
    void onGifSaved(string &fileName) {
        cout << "gif saved as " << fileName << endl;
        
        string path = ofToDataPath( "output/gif/" ).c_str()+$G(Data)->KEY+".gif";
        $G(Post)->filePost( path, API_GIF );
        
        init();
    }
        
    
};