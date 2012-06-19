#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	
	ofBackground(50);	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	font.loadFont("DIN.otf", 64);
	
	serial.listDevices();
	//vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
		
//	serial.setup(9, 9600); //open the first device
	
	serial.setup("/dev/tty.usbmodem411",9600); // mac osx example
	
	for (int i=0; i<4; i++) {
		lastChange[i] = 0;
		data[i] = 0;
	}
	
	offset = 0;
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
}

//--------------------------------------------------------------
string asString(unsigned char value) {
	string text = ofToString( (int) value);
	while (text.length() < 3)
		text = " " + text;
	return text;
}

void testApp::draw(){
	
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
	
	font.drawString(msg, 50, 100);
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
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

