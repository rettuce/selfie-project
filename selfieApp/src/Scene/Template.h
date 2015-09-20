//
//  Template.h
//
#pragma once

class Template : public BaseScene
{
public:
    
    Template() : BaseScene( "Template", 0, USE_CAMERA )
    {
        
    }
    
    void setup(){}
    void close(){}
    void updateEnter(){}
    void updateEver(){}
    
    void updateScene()
    {
    }
    
    void drawScene()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        ofPushMatrix();
        ofPushStyle();
        {
            ofSetColor(255,0,0);
            glPointSize(5);
            ofSpherePrimitive(100., 10).drawVertices();
            ofSpherePrimitive(100., 10).drawWireframe();
        }
        ofPopStyle();
        ofPopMatrix();
        glPopAttrib();
    }

};
