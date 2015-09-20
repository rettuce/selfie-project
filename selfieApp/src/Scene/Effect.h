//
//  Effect.h
//
#pragma once

#include "ofxSvg.h"

class Circle1 : public ofxAnimationPrimitives::Instance
{
public:
    ofVec2f pos;
    ofColor color;
    ofVec3f vec;
    float size;
    
    Circle1(ofVec2f pos_, ofColor color_, float size_)
    : pos(pos_), color(color_), size(size_)
    {
        pos += ofVec2f(100.*ofRandomf(), 100.*ofRandomf());
        
        vec.set(ofRandomf(),ofRandomf(),ofRandomf());
        vec.normalize();
    }
    void update(){
        pos += vec*5;
    }
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            if(rand()%2==0) ofFill();
            else ofNoFill();
            ofEnableAlphaBlending();
            ofSetLineWidth(5);
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor( color , 255*getLife());
            ofCircle(pos, 50.+size*(1-getLife()) );
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
};




class Effect : public BaseScene
{
    ofxAnimationPrimitives::InstanceManager mng;
    ofxSVG svg;
    vector<ofPolyline> outlines;
    
public:
    
    Effect() : BaseScene( "Effect", 1, NOUSE_CAMERA )
    {
        svg.load("assets/images/unframe.svg");
        for (int i = 0; i < svg.getNumPath(); i++){
            ofPath p = svg.getPathAt(i);
            p.setPolyWindingMode(OF_POLY_WINDING_ODD);
            vector<ofPolyline>& lines = p.getOutline();
            for(int j=0;j<(int)lines.size();j++){
                outlines.push_back(lines[j].getResampledBySpacing(1));
            }
        }
    }
    
    void setup(){}
    void close(){}
    void updateEnter(){}
    void updateEver(){}
    
    void updateScene()
    {
        mng.update();
    }
    
    void drawScene()
    {
#ifdef MODE_DEBBUG
        drawRender();
#endif
    }
    void drawRender()
    {
        if($G(Data)->progress==0) return;
        
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofEnableAlphaBlending();
            
            if( $G(Data)->getScene()==0 ) {
                drawBall();
            }else if( $G(Data)->getScene()==1 ) {
                drawLogo();
            }else if( $G(Data)->getScene()==2 ) {
                drawRecCap();
            }else if( $G(Data)->getScene()==3 ) {
                drawCircle();
            }

        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
    void drawBall()
    {
        ofTranslate( $G(Data)->nowPointer );
        ofRotateX(ofGetFrameNum()*2);
        ofRotateY(ofGetFrameNum()*1.5);
        ofRotateZ(ofGetFrameNum()*1.8);
        ofSetColor( $G(Color)->color0, 150 );
        glPointSize(10);
        ofSetLineWidth(3);
        
        float scl = ofMap( $G(Data)->progress, 0,0.2, 0,1, true );
        ofSpherePrimitive(123, 8).drawVertices();
        ofSpherePrimitive(120, 8).drawWireframe();
    }
    
    void drawLogo()
    {
        float scl = 3.2;
        ofTranslate((WIDTH-svg.getWidth()*scl)*0.5+20, (HEIGHT-svg.getHeight()*scl)*0.5-10);
        ofScale(scl,scl);
        
        ofNoFill();
        ofSetLineWidth(10);
        ofSetColor( $G(Color)->color1 );
        for (int i = 0; i < (int)outlines.size(); i++)
        {
            ofPolyline & line = outlines[i];
            int num = $G(Data)->progress * line.size();
            ofBeginShape();
            for (int j = 0; j < num; j++){
                ofPoint vert;
                if(j>=line.size()){
                    int jj = j - line.size();
                    vert = ofPoint(line[jj].x,line[jj].y);
                }else{
                    vert = ofPoint(line[j].x,line[j].y);
                }
                vert.x += ofRandomf();
                vert.y += ofRandomf();
                ofVertex(vert);
            }
            ofEndShape();
        }
    }
    
    void drawRecCap()
    {
        if(!$G(Data)->addsPixs.empty())
        {
            ofImage img;
            int index = $G(Data)->addsPixs.size()-1;
            img.setFromPixels( ofPixels($G(Data)->addsPixs[index]) );
            ofRectangle r = $G(Data)->rect;
            ofSetColor(255);
            img.draw(r.x, r.y, r.width, r.height );
            
            ofNoFill();
            ofSetLineWidth(3);
            ofRect( r );
        }
    }
    
    void drawCircle()
    {
        if(ofGetFrameNum()%4==0) {
            mng.createInstance<Circle1>( $G(Data)->nowPointer, $G(Color)->color0, ofRandom(50,200) )->play(0.5);
        }
        mng.draw();
    }
    
    
    

};
