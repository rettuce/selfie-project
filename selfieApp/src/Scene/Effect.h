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

class Lines : public ofxAnimationPrimitives::Instance
{
    ofVec3f pos;
    ofVec3f vec;
    float size;
    ofColor color;
    vector<ofVec3f> points;
public:
    
    Lines( float dur, const ofVec3f& pos_, const ofColor& color, float lv)
    : color(color)
    {
        float randsize = 20.;
        ofVec3f rand( ofRandomf()*randsize, ofRandomf()*randsize, ofRandomf()*randsize );
        pos = rand + pos_;
        
        vec.x = ofRandom(-lv, lv);
        vec.y = ofRandom(-lv, lv);
        vec.z = ofRandom(lv);
        play(dur);
    }
    
    void update()
    {
        ofVec3f force;
        force.x = ofRandom(-8,8);
        force.y = ofRandom(-4,4);
        force.z = ofRandom(5);
        
        vec *= 1.03;
        vec.rotate(cos(pos.x*0.01)*ofRandom(-30,30), pos.getNormalized());
        
        if(ofRandomf()>0.9)
            vec.x *= -1;
        if(ofRandomf()>0.9)
            vec.y *= -1;
        
        vec += force * 0.2;
        pos += vec * 0.5;
        points.push_back(pos);
    }
    
    void draw()
    {
        ofPushMatrix();
        ofPushStyle();
        {
            glLineWidth(5);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < points.size(); i++)
            {
                ofSetColor(color, ofMap(i, 0, points.size(), 0, 255) * getLife()*((float)color.a/255.));
                glVertex3fv(points[i].getPtr());
            }
            glEnd();
        }
        ofPopStyle();
        ofPopMatrix();
    }
};

class Line2 : public ofxAnimationPrimitives::Instance
{
    ofVec2f p0,p1;
    ofColor color;
public:
    
    Line2( const ofVec2f& p0_, const ofVec2f& p1_, const ofColor& color )
    : p0(p0_), p1(p1_), color(color)
    {}
    void update(){}
    void draw()
    {
        ofPushMatrix();
        ofPushStyle();
        {
            glLineWidth(3);
            glBegin(GL_LINE_STRIP);
            ofSetColor(color, 255. * getLife() );
            glVertex2f( p0.x + ofRandomf()*30, p0.y + ofRandomf()*30 );
            glVertex2f( p1.x + ofRandomf()*30, p1.y + ofRandomf()*30 );
            glEnd();
        }
        ofPopStyle();
        ofPopMatrix();
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
            }else if( $G(Data)->getScene()==4 ) {
                drawRepeat();
            }else if( $G(Data)->getScene()==5 ) {
                cout<< "drawHall" <<endl;
                drawHall();
            }else if( $G(Data)->getScene()==6 ) {
                drawEnd();
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
    
    void drawRepeat()
    {
        if(!$G(Data)->addsPixs.empty())
        {
            ofImage img;
            int index = $G(Data)->addsPixs.size()-1;
            
            for (int i=0; i<index; i++) {
                img.setFromPixels( ofPixels($G(Data)->addsPixs[index]) );
                ofRectangle r = $G(Data)->rect;
                ofSetColor(255, 255/index);
                
                ofPushMatrix();
                {
                    ofSetRectMode(OF_RECTMODE_CENTER);
                    ofTranslate((WIDTH)*0.5,(HEIGHT)*0.5);
                    img.draw(0,0, HEIGHT, HEIGHT);
                }
                ofPopMatrix();
            }
        }
    }
    
    void drawHall()
    {
        if(ofGetFrameNum()%4==0) bang();
        mng.draw();
    }
    void bang()
    {
        ofColor lineColor = $G(Color)->color1;
        ofVec3f start = $G(Data)->nowPointer;
        
        for (int i=0; i<20; i++) {
            float life = ofRandom(2);
            float vectorRandom = ofRandom(3.);
            mng.createInstance<Lines>( life, start, lineColor, vectorRandom );
        }
    }
    
    
    void drawEnd()
    {
        if(!$G(Data)->rposis.empty())
        {
            if(ofGetFrameNum()%4==0){
                bang2(0,1);
                bang2(1,2);
                bang2(2,3);
                bang2(3,4);
                bang2(2,5);
                
                bang2(6,7);
                bang2(7,8);
                bang2(8,9);
                
                bang2(10,11);
                bang2(10,14);
                bang2(11,12);
                bang2(12,13);
                bang2(13,14);
            }
            mng.draw();
        }
    }
    void bang2(int ind0, int ind1)
    {
        ofColor lineColor0 = $G(Color)->color0;
        ofColor lineColor1 = $G(Color)->color1;
        ofVec2f start = $G(Data)->rposis[ind0];
        ofVec2f end = $G(Data)->rposis[ind1];
        
        for (int i=0; i<5; i++) {
            float life = ofRandom(3);
            mng.createInstance<Line2>( start, end, lineColor0 )->play(life);
            mng.createInstance<Line2>( start, end, lineColor1 )->play(life);
        }
    }

    

};
