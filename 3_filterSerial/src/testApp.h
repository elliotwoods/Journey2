#pragma once

#include "ofMain.h"

#define COUNT 4

//max interval between changes per channel
#define FILTER_TIME 1.0f

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
	
	ofTrueTypeFont		font;
	ofSerial	serial;
	
	float lastChange[COUNT];
	unsigned char data[COUNT];
	int offset;
};