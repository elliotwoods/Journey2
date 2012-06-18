#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	
	ofBackground(0);	
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofEnableAlphaBlending();
	ofSetFrameRate(30.0f);
	
	serial.setup("/dev/tty.usbmodem411",9600); // mac osx example
	
	for (int i=0; i<4; i++) {
		lastChange[i] = 0.0f;
		data[i] = 0;
		alpha[i] = 0.0f;
		
		masked[i].allocate(512, 512, OF_IMAGE_COLOR_ALPHA);
		plane[i].setSource(masked[i]);
		plane[i].resetCalibration();
	}
	
	this->load();
	
	offset = 0;
	needsSave = false;
	calibMode = false;
	calibSelection = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	
	const int bufferLength = 100;
	unsigned char buffer[bufferLength];

	int read = serial.readByte();
	while (read >= 0) {
		if (read == 255) {
			offset = 0;
			break;
		} else if (offset < COUNT) {
			if (data[offset++] != read && ofGetElapsedTimef() - lastChange[offset++] > FILTER_TIME) {
				data[offset++] = read;
				lastChange[offset++] = ofGetElapsedTimef();
				
				
			}
		} else {
			offset++;
		}
		cout << offset << " : " << read << endl;
		
		read = serial.readByte();
	}
	
	for (int i=0; i<COUNT; i++) {
		if (data[i] != 0) {
			alpha[i] = FADE_COEFF + (1 - FADE_COEFF) * alpha[i];
		} else {
			alpha[i] = FADE_COEFF * alpha[i];
		}
		video[i].setVolume(alpha[i] * 100.0f);
	}
	
	//mask videos
	unsigned char *rgb, *a, *rgba;
	int count;
	for (int i=0; i<COUNT; i++) {
		
		if (video[i].isFrameNew()) {
			rgb = video[i].getPixels();
			a = mask[i].getPixels();
			rgba = masked[i].getPixels();
			count = video[i].getWidth() * video[i].getHeight();
			
			for (int j=0; j<count; j++) {
				*rgba++ = *rgb++;
				*rgba++ = *rgb++;
				*rgba++ = *rgb++;
				*rgba++ = *a++;
			}
			
			masked[i].update();
		}
	}
}

//--------------------------------------------------------------
string asString(unsigned char value) {
	string text = ofToString( (int) value);
	while (text.length() < 3)
		text = " " + text;
	return text;
}

void testApp::draw(){
	
	ofPushStyle();
	for (int i=0; i<COUNT; i++) {
		if (data[i] !=0 || calibSelection == i) {
			ofSetColor(alpha[i] * 255.0f);
			plane[i].draw();
		}
	}
	ofPopStyle();
	
	if (calibMode) {
		string msg;
		
		for (int i=0; i<4; i++) {
			bool isOffset = i == offset;
			
			if (isOffset)
				msg += "[";
			else
				msg += " ";
			
			msg += asString(data[i]);
			
			if (isOffset)
				msg += "]";
			else
				msg += " ";
		}
		
		ofDrawBitmapString(msg, 20, 20);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	bool reassignCalibrated = false;
	
	switch (key) {
		case ' ':
			calibMode ^= true;
			ofBackground(calibMode ? 50 : 0);
			reassignCalibrated = true;
			break;
			
		case OF_KEY_PAGE_DOWN:
			calibSelection++;
			calibSelection %= COUNT;
			reassignCalibrated = true;
			break;
			
		case OF_KEY_PAGE_UP:
			calibSelection--;
			if (calibSelection < 0)
				calibSelection = COUNT - 1;
			reassignCalibrated = true;
			break;
			
		case 'l':
			this->load();
			break;
		case 's':
			this->save();
			break;
	}
	
	if (reassignCalibrated) {
		for (int i=0; i<COUNT; i++) {
			if (calibMode) {
				plane[i].setCalibrateMode(calibSelection == i);
			} else {
				plane[i].setCalibrateMode(false);
			}
		}
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}

//--------------------------------------------------------------
void testApp::loadVideo(int layer) {
	if (data[layer] != 0) {
		video[layer].loadMovie(ofToString(data[layer] + ".mov"));
		mask[layer].loadImage(ofToString(data[layer] + ".png"));
		
		if (!masked[layer].isAllocated()) {
			masked[layer].allocate(video[layer].getWidth(), video[layer].getHeight(), OF_IMAGE_COLOR_ALPHA);
		}
	}
}

//--------------------------------------------------------------
void testApp::exit() {
	this->save();
}


//--------------------------------------------------------------
void testApp::load() {
	for (int i=0; i<COUNT; i++) {
		plane[i].load(ofToString(i));
	}
}

//--------------------------------------------------------------
void testApp::save() {
	for (int i=0; i<COUNT; i++) {
		plane[i].save(ofToString(i));
	}
}