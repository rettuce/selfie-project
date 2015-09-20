//
//  Kinect.h
//
#pragma once

#include "ofxMultiKinectV2.h"
#include "GpuRegistration.h"

#define STRINGIFY(x) #x

static string depthFragmentShader =
STRINGIFY(
          uniform sampler2DRect tex;
          void main()
          {
              vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);
              float value = col.r;
              float low1 = 500.0;
              float high1 = 1000.0;
              float low2 = 1.0;
              float high2 = 0.0;
              float d = clamp(low2 + (value - low1) * (high2 - low2) / (high1 - low1), 0.0, 1.0);
              if (d == 1.0) {
                  d = 0.0;
              }
              gl_FragColor = vec4(vec3(d), 1.0);
          }
          );

static string irFragmentShader =
STRINGIFY(
          uniform sampler2DRect tex;
          void main()
          {
              vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);
              float value = col.r / 65535.0;
              gl_FragColor = vec4(vec3(value), 1.0);
          }
          );

class Kinect : public BaseScene
{
    ofShader depthShader;
    ofShader irShader;
    
    ofxMultiKinectV2 kinect;
    ofTexture depthTex;
    ofTexture colorTex;
    ofTexture irTex;
    GpuRegistration gr;
    
    ofEasyCam cam;
    ofVboMesh mesh;
    ofVec3f offset = ofVec3f::zero();
    ofVec3f center = ofVec3f::zero();
    
    bool process_occlusion;
    float ThresholdMin, ThresholdMax, vleng;
    int validityCount = 0;

public:
    
    Kinect() : BaseScene( "Kinect", 1, USE_CAMERA )
    {
        ofEnableArbTex();
        process_occlusion = true;
        
        depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragmentShader);
        depthShader.linkProgram();
        
        irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
        irShader.linkProgram();
        
        // kinect.open(true, true, 0, 2); // GeForce on MacBookPro Retina
//        kinect.open(true, true, 0, 2);
//        kinect.start();
        
        gr.setup(kinect.getProtonect(), 2);
        
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        
//        $G(Data)->colorTex = &colorTex;
    }
    
    void setup(){}
    void close(){}
    void updateEnter(){}
    
    void updateEver()
    {
        Osc *osc = $G(Osc);
        ofxOscMessage m;
        osc->getLatest("/kinect/offset", offset);
        osc->getLatest("/kinect/center", center);
        osc->getLatest("/kinect/vleng", vleng);
        osc->getLatest("/kinect/min", ThresholdMin);
        osc->getLatest("/kinect/max", ThresholdMax);
    }
    
    
    void updateScene()
    {
        kinect.update();
        if (kinect.isFrameNew()) {
            colorTex.loadData(kinect.getColorPixelsRef());
            depthTex.loadData(kinect.getDepthPixelsRef());
            irTex.loadData(kinect.getIrPixelsRef());
            
            depthTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            gr.update(depthTex, colorTex, process_occlusion);
            
            ofPixels *pixels = gr.getRegisteredTexturePixels();
            
            mesh.clear();
            {
                int step = 2;
                int h = pixels->getHeight()*0.5;
                int w = pixels->getWidth()*0.5;
                
                int zurehaba = int(ofRandom(0));
                validityCount = 0;
                float maxZ = 999999999;
                
                ofColor precolor=ofColor(1);
                for(int y = zurehaba; y < h-zurehaba; y += step) {
                    for(int x = zurehaba; x < w-zurehaba; x += step) {
                        float dist = kinect.getDistanceAt(x, y);
                        if(dist > ThresholdMin && dist < ThresholdMax) {
                            
                            if(ofPoint(center-ofPoint(x,y)).length()>vleng) continue;
                            
                            int zureX = int(ofRandomf()*zurehaba);
                            int zureY = int(ofRandomf()*zurehaba);
                            
                            ofVec3f pt = kinect.getWorldCoordinateAt(x+zureX, y+zureY, dist );

                            float h = ofMap(dist, ThresholdMin, ThresholdMax, 1, 0, true);
                            ofColor c = pixels->getColor(x*2+zureX, y*2+zureY);
//                            if( c == ofColor::black ){
//                                c = precolor;
//                            }else{
//                                precolor = c;
//                            }
                            
                            c.a = 255 * getAlpha() * h;
                            
                            if(h>0){
                                if( maxZ > dist ) maxZ = dist;
                                validityCount++;
                            }
                            
//                            ofColor c = img.getColor(x+zureX, y+zureY);
                            mesh.addColor(c);
                            mesh.addVertex(pt + offset);
                        }
                    }
                }
                if( validityCount == 0) maxZ = 0;
                float per = (maxZ - ThresholdMin)/(ThresholdMax - ThresholdMin);
                if( validityCount == 0) per = 1;
                else if( per > 0.8 ) {
                    per = 1;
                    $G(Data)->nearPer = 1;
                }
//                $G(Data)->nearPer = ofMap( per, 0, 1, 0, 1, true);
                
                Tweenzor::add( &$G(Data)->nearPer, $G(Data)->nearPer, ofMap( per, 0, 1, 0, 1, true), 0.f, 0.1f);
                
                
//                
//                cout<< "validityCount " << validityCount <<endl;
//                cout<< "maxZ " << maxZ <<endl;
//                cout<< "Now : " << $G(Data)->nearPer <<endl;
            }
            
        }
        
    }
    
    void drawScene()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofEnableDepthTest();
            ofSetColor(255, 255*getAlpha());
            glPointSize(7);
            
            ofScale(-1,1);
            mesh.draw();
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
            ofEnableDepthTest();
            ofSetColor(255, 255*getAlpha());
            glPointSize(7);
            
            ofScale(-1,1);
            mesh.draw();
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
    
    
};
