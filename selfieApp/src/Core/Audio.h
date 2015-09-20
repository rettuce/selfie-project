//
//  Audio.h
//
#pragma once

class Audio : public ofxGlobalContext::Context
{
public:
    ofSoundPlayer add;
    ofSoundPlayer save;
    void setup()
    {
        add.setVolume(0.4f);
        add.loadSound("assets/audio/alert_002.wav");
        save.setVolume(0.8f);
        save.loadSound("assets/audio/comp_select_004.wav");
    }
    
    
    void seAdd(){
        add.play();
    }
    void seSave(){
        save.play();
    }
};