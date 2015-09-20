
#include "ofMain.h"
#include "BuildSetting.h"
#include "ofxTweenzor.h"
#include "ofxGlobalContext.h"
#define $G $Context

#include "Core/Data.h"
#include "Core/Osc.h"
#include "Core/Audio.h"
#include "Core/Color.h"
#include "Core/Post.h"
#include "Core/Camera.h"
#include "Core/TimeLine.h"
#include "Core/SceneManager.h"
#include "Core/Recorder.h"

class ofApp : public ofBaseApp
{
    ofxGlobalContext::Manager& context;
    
public:
    
    ofApp()
    : context(ofxGlobalContext::Manager::defaultManager())
    {}
    void exit(){
        $G(SceneManager)->close();
        Tweenzor::destroy();
    }

    void setup()
    {
        ofSetFrameRate(60);
        ofSetCircleResolution(100);
        ofBackground(0);
        
        Tweenzor::init();
        
        // contexts
        context.createContext<Data>();
        context.createContext<Post>();
        context.createContext<Osc>(10005,10006,"localhost");
        context.createContext<Color>();
        context.createContext<Audio>()->setup();
        context.createContext<TimeLine>();
        context.createContext<Camera>()->setup();
        context.createContext<SceneManager>()->setup();
        context.createContext<Recorder>();
        
        $G(Data)->setNewUser();
    }
    
    void update()
    {
        context.update();
        Tweenzor::update( ofGetElapsedTimeMillis() );
//        Tweenzor::update( ofGetFrameNum()/60.*1000. );  // 60fps

        Osc *osc = $G(Osc);
        ofxOscMessage m;
        while (osc->get("/test/bang", m)){ ofLogNotice(m.getAddress()); };
    }
    
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            $G(SceneManager)->draw();
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
        
        ofDrawBitmapString(ofToString(ofGetFrameRate()), WIDTH-80, 30);
    }
    
    void keyPressed(int key)
    {
        ofxOscMessage m;
        switch (key)
        {
            case ',':
                m.setAddress("/oF/start");
                $G(Osc)->sendOscMessage(m);
                break;
            case '.':
                m.setAddress("/oF/end");
                $G(Osc)->sendOscMessage(m);
                break;
            case 'f':
                ofToggleFullscreen();
                break;
            case '\t':
                ofToggleFullscreen();
                break;
        }
    };
    void keyReleased(int key){};
    void mouseMoved(int x, int y ){};
    void mouseDragged(int x, int y, int button){};
    void mousePressed(int x, int y, int button){};
    void mouseReleased(int x, int y, int button){};
    void windowResized(int w, int h){};
};


#include "ofAppGLFWWindow.h"

int main(int argc, const char** argv)
{
    ofAppGLFWWindow window;
    window.setMultiDisplayFullscreen(true);
    window.setNumSamples(4);
    ofSetupOpenGL(&window, WIDTH, HEIGHT, OF_WINDOW);
    ofRunApp(new ofApp);
    return 0;
}