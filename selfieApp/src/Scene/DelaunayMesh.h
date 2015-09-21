//
//  DelaunayMesh.h
//
#pragma once

#include "ofxDelaunay.h"

class DelaunayMesh : public ofxDelaunay
{
public:

    ofVboMesh mesh;
    vector<ofColor> colors;
        
    void reset() {
        mesh.clear();
        colors.clear();
        ofxDelaunay::reset();
    }
    
    int addColor( const ofColor& c ){
        colors.push_back(c);
        return colors.size();
    }
    
    int addColors( vector<ofColor>& cs ){
        for(int i = 0; i < cs.size(); i++){
            addColor( cs[i] );
        }
        return colors.size();
    }
    
    void addFace( ofVec3f a, ofVec3f b, ofVec3f c, ofColor col ) {
        mesh.addVertex(a);
        mesh.addVertex(b);
        mesh.addVertex(c);
        mesh.addColor(col);
        mesh.addColor(col);
        mesh.addColor(col);
    }
    
    void addFaceDouble(const ofVec3f a, const ofVec3f b, const ofVec3f c, const ofVec3f d,
                       const ofColor col1, const ofColor col2)
    {
        addFace(a, b, c, col1);
        addFace(a, c, d, col2);
    }

    
    void getColor(){};
    
    void drawColor()
    {
        ofFill();
        
        mesh.clear();
        float size = triangulate();
        
        for (int i=0; i<size; i++)
        {
            ofColor color = colors[i+1];
            if(0!=triangles[i].p1+triangles[i].p2+triangles[i].p3)
            {
                ofVec3f a = ofVec3f(vertices[triangles[i].p1].x,vertices[triangles[i].p1].y,vertices[triangles[i].p1].z);
                ofVec3f b = ofVec3f(vertices[triangles[i].p2].x,vertices[triangles[i].p2].y,vertices[triangles[i].p2].z);
                ofVec3f c = ofVec3f(vertices[triangles[i].p3].x,vertices[triangles[i].p3].y,vertices[triangles[i].p3].z);
                if( ( a.z<0 && b.z<0 ) && c.z < 0 ) addFace( a, b, c, colors[i] );
            }
        }
    };
    
    void draw()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            // line
            mesh.drawWireframe();
            
//            // face
//            mesh.drawFaces();

        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }
    
};
