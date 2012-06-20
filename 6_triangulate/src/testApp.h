#pragma once

#include "ofMain.h"
#include "ofxGraycode.h"
#include "ofxRay.h"
#include "ofxGrabCam.h"
#include "ofxCvGui.h"
#include "ofxTriangulate.h"

class testApp : public ofBaseApp, public ofNode {

public:
	void setup();
	
	void load();
	void makeTransform();
	void triangulate();
	void seperate();
	
	void update();

	void customDraw();
	
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxCvGui::Builder gui;
	ofxGrabCam * cam;
	
	ofxRay::Camera camera;
	ofxRay::Projector projector;
	
	ofxGraycode::PayloadGraycode payload;
	ofxGraycode::Decoder data;
	
	ofMesh mesh;
	float threshold;
	
	ofVec3f offset;
	ofVec3f rotation;
	ofImage slide[3];
	
	ofMatrix4x4 transform;
};
