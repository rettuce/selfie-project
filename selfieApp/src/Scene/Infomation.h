//
//  Infomation.h
//
#pragma once

#include "ofxFlash.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxAnimationPrimitives.h"

class CountDown : public ofxAnimationPrimitives::Instance
{
public:
    ofxTrueTypeFontUC *font;
    ofImage *countBg;
    ofVec2f pos;
    int count,basecount;
    
    CountDown(int count_, ofxTrueTypeFontUC *font_, ofImage *countBg_)
    : basecount(count_)
    , font(font_)
    , countBg(countBg_)
    {
        count = basecount;
        pos.set(WIDTH*0.5,HEIGHT*0.5);
    }
    void update(){
        
        float per = 1.0/float(basecount);
        int c = floor((1-getLife())/per);
        count = basecount - c;
    }
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofEnableAlphaBlending();
            
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor( 255 );
            countBg->draw( pos );
            font->drawString( ofToString(count) ,
                              pos.x-font->stringWidth( ofToString(count)) * ((count==1)? 1.5 : 0.5) ,
                              pos.y+font->stringHeight( ofToString(count))*0.5 );
            
            ofNoFill();
            ofEnableSmoothing();
            ofEnableAntiAliasing();

            float size = 103.;
            ofSetLineWidth( 10 );
            ofSetColor( 0 );
            float st = 0; float ed = int(360*(1-getLife()) );
            ofBeginShape();
            for (int i=st; i<ed; i++){
                float x = cos((i-90)*TWO_PI/360)*size + pos.x;
                float y = sin((i-90)*TWO_PI/360)*size + pos.y;
                ofCurveVertex(x, y);
            }
            ofEndShape();
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
};

class StartWord : public ofxAnimationPrimitives::Instance
{
public:
    string w = "Start!";
    ofxTrueTypeFontUC *font;
    ofVec2f pos;
    
    StartWord(ofxTrueTypeFontUC *font_)
    : font(font_)
    {
        pos.set(WIDTH*0.5,HEIGHT*0.5);
    }
    void update(){
    }
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofEnableAlphaBlending();
            
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor( 255 , 255*getLife());
            
            font->drawString( w ,
                             pos.x-font->stringWidth( w ) *0.5 ,
                             pos.y+font->stringHeight( w )*0.5 );
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
};

class InfoImg : public ofxAnimationPrimitives::Instance
{
    ofVec2f pos;
    ofImage *img;
    float phase = 0.;
    bool isBg;
public:
    InfoImg(ofImage *img_, ofVec2f pos_=ofVec2f(WIDTH*0.5,HEIGHT*0.5), bool isBg_=true)
    : img(img_), pos(pos_), isBg(isBg_)
    {}
    
    void update(){
        phase += 0.01;
    }
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofEnableAlphaBlending();
            
            float per;
            if(isInfinity()){
                ofSetColor( 255 , 255*0.4 );
                per = ofMap( phase, 1.0, 2.0, 0, 1, true);
            }else{
                ofSetColor( 255 , 255*0.2 );
                per = ofMap(getLife(), 0., 0.2, 0, 1, true);
            }
            ofFill();
            if(isBg) ofRect(0, 0, WIDTH, HEIGHT);
            
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor( 255 , 255*per );
            img->draw( pos );
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
};






class Infomation : public BaseScene
{
    ofxAnimationPrimitives::InstanceManager mng;
    
    ofxFlashStage* stageInfo;
    ofxFlashXFL xfl;
    
    map<string, ofxFlashMovieClip*> mcs;
    map<string, ofImage> infos;
    ofxFlashMovieClip* nowMC = NULL;
    
    ofxTrueTypeFontUC font;
    ofImage countBg, thanks, url;
    
public:
    
    Infomation() : BaseScene( "Infomation", 1, NOUSE_CAMERA )
    {
        gifimg.img = &gif;
        thanks.loadImage("assets/images/comp.png");
        url.loadImage("assets/images/url.png");
        
        // font
        font.loadFont("assets/SAQUlA.ttf", 130, true, true);
        countBg.loadImage("assets/images/count.png");
        
        // flash
        stageInfo = new ofxFlashStage();			// ofxFlash setup.
        xfl.loadFile( "assets/anim/DOMDocument.xml" );		// load XFL flash file.
        xfl.build(stageInfo);
        
        int numChild = stageInfo->root()->numChildren();
        for (int i=0; i<numChild; i++) {
            ofxFlashMovieClip* mc = (ofxFlashMovieClip*)stageInfo->root()->getChildAt(i);
            string name = mc->name();
            string s = "Load stageInfo MC - Name : " + name;
            
            mcs[name] = mc;
            infos[name] = ofImage("assets/images/"+ name + ".png");
            mc->visible(false);
            mc->stop();
        }
        
//        for(auto itr = mcs.begin(); itr != mcs.end(); ++itr) {
//            std::cout << "key = " << itr->first           // キーを表示
//            << ", val = " << itr->second << "\n";    // 値を表示
//            
//            itr->second->gotoAndStop(10);
//        }
    }
    
    void setup(){}
    void close(){}
    void updateEnter(){}
    void updateEver(){}
    
    
    // End Btnが押された処理
    void init()
    {
        $G(Data)->clear();

        isLastInfo = false;
        gifcount = 0;
        gifimg.pos.y = -1000;
        mng.release<InfoImg>();
        
        ofxOscMessage m3;
        m3.setAddress("/setFocus");
        m3.addFloatArg(0.0);
        $G(Osc)->sendOscMessage(m3);
    }
    
    
    int frameCount = 5;
    
    void updateScene()
    {
//        if(ofGetFrameNum()%frameCount==0) stageInfo->update();   // $G(Data)->SCENE:0, frame:1/5, rec:750ms
//        if(ofGetFrameNum()%frameCount==0) stageInfo->update();   // $G(Data)->SCENE:1, frame:1/7, rec:1500ms
        if(ofGetFrameNum()%frameCount==0) stageInfo->update();     // $G(Data)->SCENE:2, frame:1/6, rec:1700ms
        
        Osc *osc = $G(Osc);
        ofxOscMessage m;
        
        if(osc->getLatest("/info/init", m)) init();
        if(osc->getLatest("/info/play", m)) preset( m.getArgAsInt32(0), m.getArgAsString(1) );

        if(nowMC && $G(Data)->isRunning )
        {
            $G(Data)->progress = nowMC->currentFrame() / (float)nowMC->totalFrames();
            
            // Ball position 使いたい時だけ
            if( $G(Data)->getScene() == 0 || $G(Data)->getScene() == 3 ) {
                Tweenzor::add( &$G(Data)->nowPointer, $G(Data)->nowPointer,
                              getBallPosition(),
                              0.f, 0.1f, EASE_OUT_QUAD );
            }
            
            if($G(Data)->progress>=1.){
                nowMC->gotoAndStop(0);
                nowMC->visible(false);
                nowMC = NULL;
                $G(Data)->progress = 0.;
                $G(Data)->isRunning = false;
                
                ofxOscMessage m2;
                m2.setAddress("/rec/comp");
                $G(Osc)->sendOscMessage(m2);
                
                
                nextCompInfo();
            }
        }
        
        mng.update();
        
        if(isLastInfo) {
            gifUpdate();
        }
    }
    
    ofImage gif;
    ImgObj gifimg;
    int gifcount = 0;
    
    void gifUpdate() {
        int c = gifcount;
        if(ofGetFrameNum()%5==0) gifcount++;
        if( c == gifcount ) return;
        
        if( gifcount >= $G(Data)->addsPixs.size() ){
            if( $G(Data)->getScene()==2 ) gifcount = 1;
            else gifcount = 0;
        }
        gif.setFromPixels( $G(Data)->addsPixs[gifcount] );
        gif.resize($G(Data)->qrimg.img->width, $G(Data)->qrimg.img->height);
    }
    
    
    
    
    void preset( int sceneID, string McName )
    {
        $G(Data)->setNewUser();
        $G(Data)->setScene( sceneID );
        
        string name = McName;
        
        nowMC = mcs[name];
        nowMC->visible(true);
        nowMC->gotoAndPlay(0);
        
        if($G(Data)->getScene()==0) {   // finger
            frameCount = 5;
        }else if($G(Data)->getScene()==1){   // unframe
            frameCount = 7;
        }else if($G(Data)->getScene()==2){   // rect
            frameCount = 6;
            $G(Data)->rect = ofRectangle( nowMC->getChildByName("rect")->x(), nowMC->getChildByName("rect")->y(),
                                         450, 450);
        }else if($G(Data)->getScene()==3){   // circle
            frameCount = 7;
            
        }else if($G(Data)->getScene()==4 || $G(Data)->getScene()==5){   // kinect & kinect2
            frameCount = 3;
        }
        
        float InfoPracticeTime = 5.0; // インフォ+練習タイム 5s

        if($G(Data)->getScene()==2){
            mng.createInstance<InfoImg>(&infos[name], ofVec2f(WIDTH*0.5, HEIGHT*0.5-100.))->play(InfoPracticeTime);
        }else{
            mng.createInstance<InfoImg>(&infos[name])->play(InfoPracticeTime);
        }
        
        float perStart;
        Tweenzor::add( &perStart, 0.f, 1.f, 0.f, InfoPracticeTime );
        Tweenzor::addCompleteListener( Tweenzor::getTween(&perStart), this, &Infomation::playCountDown );
    }
    
    void playCountDown(float* arg)
    {
        float CountDownTime = 3.; // CountDown 3s

        mng.createInstance<CountDown>(3, &font, &countBg)->play(CountDownTime);
        nowMC->gotoAndStop(0);
        
        float perStart;
        Tweenzor::add( &perStart, 0.f, 1.f, 0.f, CountDownTime );
        Tweenzor::addCompleteListener( Tweenzor::getTween(&perStart), this, &Infomation::PerAllComp );
    }
    
    void PerAllComp(float* arg)
    {
        $G(Data)->nowPointer.set( getBallPosition() );
        nowMC->gotoAndPlay(0);
        $G(Data)->isRunning = true;
        
        mng.createInstance<StartWord>(&font)->play(1.5);
    
        ofxOscMessage m2;
        m2.setAddress("/rec/start");
        $G(Osc)->sendOscMessage(m2);
    }
    
    
    // SCENE 0,3
    ofVec2f getBallPosition() {
        ofVec2f pos = ofVec2f::zero();
        if( $G(Data)->getScene() == 0 || $G(Data)->getScene() == 3 ) {
            pos.set(nowMC->getChildByName("ball")->x(), nowMC->getChildByName("ball")->y());
        }
        return pos;
    }
    
    
    
    
    bool isLastInfo = false;
    void nextCompInfo()
    {
        float margin = 220.;
        float qrH = $G(Data)->qrimg.img->getHeight();
        float offsetY = -150.;
        float yy0 = HEIGHT*0.5 + qrH*0.5 + offsetY;
        
        Tweenzor::add( &$G(Data)->qrimg.pos,
                       ofVec2f(WIDTH*0.5-margin,-1000.), ofVec2f(WIDTH*0.5-margin, yy0 ),
                       0.f, 2.5f, EASE_OUT_BOUNCE );
        Tweenzor::add( &$G(Data)->qrimg.angle, 360.f, 0.f, 0.f, 2.2f, EASE_IN_OUT_BACK );
        
        Tweenzor::add( &gifimg.pos,
                       ofVec2f(WIDTH*0.5+margin,-1000.), ofVec2f(WIDTH*0.5+margin, yy0 ),
                       0.7f, 2.5f, EASE_OUT_BOUNCE );
        Tweenzor::add( &gifimg.angle, -360.f, 0.f, 0.7f, 2.2f, EASE_IN_OUT_BACK );
        
        float yy1 = HEIGHT*0.5 - 80. - thanks.height*0.5 + offsetY;
        mng.createInstance<InfoImg>(&thanks, ofVec2f(WIDTH*0.5, yy1 ))->playInfinity();
        
        float yy2 = HEIGHT*0.5 + qrH + 100. + url.height*0.5 + offsetY;
        mng.createInstance<InfoImg>(&url, ofVec2f(WIDTH*0.5, yy2 ), false)->playInfinity();
        
        ofxOscMessage m3;
        m3.setAddress("/setFocus");
        m3.addFloatArg(1.0);
        $G(Osc)->sendOscMessage(m3);
        
        isLastInfo = true;
    }
    
    
    
    void drawScene()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofSetColor(255);
            stageInfo->draw();

            mng.draw();
            
            if(isLastInfo)
            {
                $G(Data)->qrimg.draw();
                gifimg.draw();
            }
            
#ifdef MODE_DEBBUG
            // kinect debug
            ofNoFill();
            ofRect(WIDTH-5, 30, 2, 100*$G(Data)->nearPer );
#endif
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
};
