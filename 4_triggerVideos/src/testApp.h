#pragma once

#include "ofMain.h"
#include "ofxPolyPlane.h"

#define COUNT 3

//max interval between changes per channel
#define FILTER_TIME 1.0f
#define FLICKER_TIME 0.5f

#define FADE_COEFF 0.8f

class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
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
	
	void exit();
	void load();
	void save();
	
	void loadVideo(int layer);
	void stopVideo(int layer);
	
	int offset;
	ofSerial serial;
	
	float lastFlicker[COUNT];
	float lastChange[COUNT];
	bool flickering[COUNT];
	unsigned char data[COUNT];
	
	ofVideoPlayer video[COUNT];
	ofImage	mask[COUNT];
	ofImage masked[COUNT];
	
	ofxPolyPlane plane[COUNT];
	float alpha[COUNT];
	
	bool needsSave;
	bool calibMode;
	int calibSelection;
};

