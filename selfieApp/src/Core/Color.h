//
//  Color.h
//
#pragma once

class Color : public ofxGlobalContext::Context
{
public:
    ofFloatColor color0,color1,bgcolor;
    
    Color():
    color0(ofFloatColor(1))
    ,color1(ofFloatColor(1))
    ,bgcolor(ofFloatColor(0))
    {}
    
    void update()
    {
        Osc *osc = $G(Osc);
        osc->getLatest("/color0", color0);
        osc->getLatest("/color1", color1);
        osc->getLatest("/bgcolor", bgcolor);
    }
};
