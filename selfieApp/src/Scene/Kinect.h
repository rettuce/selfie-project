//
//  Kinect.h
//
#pragma once

#include "ofxMultiKinectV2.h"
#include "GpuRegistration.h"
#include "DelaunayMesh.h"

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
    ofVboMesh mesh, premesh;
    ofVec3f offset = ofVec3f::zero();
    ofVec3f center = ofVec3f::zero();
    
    bool process_occlusion;
    float ThresholdMin, ThresholdMax, vleng;
    int validityCount = 0;
    int bure = 2;
    float param = 100.;
    
    DelaunayMesh dlny;
    
public:
    
    Kinect() : BaseScene( "Kinect", 1, USE_CAMERA, OF_BLENDMODE_ADD )
    {
        ofEnableArbTex();
        process_occlusion = true;
        
        depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragmentShader);
        depthShader.linkProgram();
        
        irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
        irShader.linkProgram();
        
        // kinect.open(true, true, 0, 2); // GeForce on MacBookPro Retina
        kinect.open(true, true, 0, 2);
        kinect.start();
        
        gr.setup(kinect.getProtonect(), 2);
        
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        
    }
    
    void setup(){}
    void close(){}
    void updateEnter(){}
    
    void updateEver()
    {
        Osc *osc = $G(Osc);
        ofxOscMessage m;
        osc->getLatest("/kinect/param", param);
        osc->getLatest("/kinect/bure", bure);
        osc->getLatest("/kinect/offset", offset);
        osc->getLatest("/kinect/center", center);
        osc->getLatest("/kinect/vleng", vleng);
        osc->getLatest("/kinect/min", ThresholdMin);
        osc->getLatest("/kinect/max", ThresholdMax);
    }
    
    
    void updateScene()
    {
        if($G(Data)->getScene()<4) return;
        
        kinect.update();
        if (kinect.isFrameNew()) {
            colorTex.loadData(kinect.getColorPixelsRef());
            depthTex.loadData(kinect.getDepthPixelsRef());
            irTex.loadData(kinect.getIrPixelsRef());
            
            depthTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            gr.update(depthTex, colorTex, process_occlusion);
            
            ofPixels *pixels = gr.getRegisteredTexturePixels();
            
            int step = 2;
            int h = pixels->getHeight()*0.5;
            int w = pixels->getWidth()*0.5;
            
            
            
            if($G(Data)->getScene()==4)
            {
                mesh.clear();
                {
                    validityCount = 0;
                    float maxZ = 999999999;
                    
                    for(int y = 0; y < h; y += step) {
                        for(int x = 0; x < w; x += step) {
                            float dist = kinect.getDistanceAt(x, y);
                            
                            if(dist > ThresholdMin && dist < ThresholdMax) {
                                
                                if(ofPoint(center-ofPoint(x,y)).length()>vleng) continue;   // 中心点から遠いものは除外
                                
                                int xx = 0; int yy = 0;
                                if( x>bure && x<w-bure ) xx = int(bure * ofRandomf());
                                if( y>bure && y<h-bure ) yy = int(bure * ofRandomf());
                                
                                ofVec3f pt = kinect.getWorldCoordinateAt( x+xx, y+yy, dist );
                                float h = ofMap(dist, ThresholdMin, ThresholdMax, 1, 0, true);
                                ofColor c = pixels->getColor((x+xx)*2, (y+yy)*2);
                                c.a = 255 * getAlpha() * h;
                                
                                if(h>0){
                                    if( maxZ > dist ) maxZ = dist;
                                    validityCount++;
                                }
                                mesh.addColor(c);
                                mesh.addVertex(pt + offset);
                            }
                        }
                    }
                    if( validityCount <= 100) maxZ = 0;
                    float per = (maxZ - ThresholdMin)/(ThresholdMax - ThresholdMin);
                    if( validityCount <= 100) per = 1;
                    else if( per > 0.8 ) {
                        per = 1;
                        $G(Data)->nearPer = 1;
                    }
                    Tweenzor::add( &$G(Data)->nearPer, $G(Data)->nearPer, ofMap( per, 0, 1, 0, 1, true), 0.f, 0.3f);
                }
                
                
            }else if($G(Data)->getScene()==5){
                
                mesh.clear();
                premesh.clear();
                {
                    validityCount = 0;
                    float maxZ = 999999999;
                    
                    for(int y = 0; y < h; y += step) {
                        for(int x = 0; x < w; x += step) {
                            float dist = kinect.getDistanceAt(x, y);
                            
                            if(dist > ThresholdMin && dist < ThresholdMax) {
                                
                                if(ofPoint(center-ofPoint(x,y)).length()>vleng) continue;   // 中心点から遠いものは除外
                                
                                int xx = 0; int yy = 0;
                                if( x>bure && x<w-bure ) xx = int(bure * ofRandomf());
                                if( y>bure && y<h-bure ) yy = int(bure * ofRandomf());
                                
                                ofVec3f pt = kinect.getWorldCoordinateAt( x+xx, y+yy, dist );
                                float h = ofMap(dist, ThresholdMin, ThresholdMax, 1, 0, true);
                                ofColor c = pixels->getColor((x+xx)*2, (y+yy)*2);
                                c.a = 255 * getAlpha() * h;
                                
                                premesh.addColor(c);
                                premesh.addVertex(pt + offset);
                            }
                        }
                    }
                }
                
                // Delaunay
                dlny.reset();
                for ( int i=0; i<premesh.getNumVertices(); i+=5 ) {
                    ofVec3f v = premesh.getVertex(i);
                    dlny.addPoint(v.x, v.y, v.z, premesh.getColor(i));
                }
                
                int size = dlny.triangulate();
                for (int i=0; i<size; i++)
                {
                    ITRIANGLE &tri = dlny.triangles[i];
                    
                    if(tri.p1+tri.p2+tri.p3!=0)
                    {
                        ofVec3f a = dlny.vertices[tri.p1].getVec3f();
                        ofVec3f b = dlny.vertices[tri.p2].getVec3f();
                        ofVec3f c = dlny.vertices[tri.p3].getVec3f();
                        
                        ofVec2f p1((a.x+b.x)*.5, (a.y+b.y)*.5);
                        ofVec2f p2((b.x+c.x)*.5, (b.y+c.y)*.5);
                        ofVec2f p3((c.x+a.x)*.5, (c.y+a.y)*.5);
                        
                        for (int i=0; i<5; i++)
                        {
                            p1.set((p1.x+p2.x)*.5, (p1.y+p2.y)*.5);
                            p2.set((p2.x+p3.x)*.5, (p2.y+p3.y)*.5);
                            p3.set((p1.x+p3.x)*.5, (p1.y+p3.y)*.5);
                        }
                        
                        float dist1 = ofDist(a.x, a.y, b.x, b.y);
                        float dist2 = ofDist(b.x, b.y, c.x, c.y);
                        float dist3 = ofDist(a.x, a.y, c.x, c.y);
                        
                        if( dist1+dist2+dist3 < param ){
                            ofColor col = dlny.vertices[tri.p1].c;
//                            ofColor col = pixels->getColor(p1.x,p1.y);
                            dlny.addFace( a, b, c, col );
                        }
                    }
                }
                
                // mesh z-add
                // ...
                
            }
            
        }
        
    }
    
    void drawScene()
    {
#ifdef MODE_DEBBUG
        drawRender();
#endif
    }
    
    void drawRender()
    {
        if($G(Data)->getScene()<4) return;
        
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofEnableDepthTest();
            ofSetColor(255, 255*getAlpha());
            glPointSize(7);
            ofScale(-1,1);
            
            // point
            
            if($G(Data)->getScene()==4) {
//                glBegin(GL_POINTS);
//                for (int i=0; i<mesh.getNumColors(); i++) {
//                    ofSetColor( mesh.getColor(i), 255*getAlpha());
//                    glVertex3fv( mesh.getVertex(i).getPtr() );
//                }
//                glEnd();
                mesh.drawVertices();
                
            }else if($G(Data)->getScene()==5){
                
                premesh.drawVertices();
                dlny.draw();
            }
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
    
    
};
