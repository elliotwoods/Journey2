#include "testApp.h"

void printCProp(videoInput & device, int ID, long prop, string name) {
	long min, max, step, value, flags, def;
	device.getVideoSettingCamera(ID, prop, min, max, step, value, flags, def);

	cout << name << ": " << min << "..." << value << " (" << def << ")..." << max << ", step = " << step << ", flags = " << flags << endl;
	cout << "--" << endl;
}

void printFProp(videoInput & device, int ID, long prop, string name) {
	long min, max, step, value, flags, def;
	device.getVideoSettingFilter(ID, prop, min, max, step, value, flags, def);

	cout << name << ": " << min << "..." << value << " (" << def << ")..." << max << ", step = " << step << ", flags = " << flags << endl;
	cout << "--"<< endl;
}

void getDeviceProps(ofVideoGrabber & cam) {
	#ifdef TARGET_WIN32
	videoInput & device = ((ofDirectShowGrabber*) &* cam.getGrabber())->getDevice();
	int ID = 0;

	cout << "-----" << endl;
	cout << "PROPS" << endl;
	printCProp(device, ID, device.propFocus, "focus");
	printCProp(device, ID, device.propExposure, "exposure");
	printCProp(device, ID, device.propGain, "gain");
	printCProp(device, ID, device.propPan, "pan");
	printCProp(device, ID, device.propTilt, "tilt");
	printCProp(device, ID, device.propZoom, "zoom");
	
	printFProp(device, ID, device.propSharpness, "sharpness");

	cout << "-----"<< endl;
	cout << endl;
	#endif
}

void setDeviceProps(ofVideoGrabber & cam) {
#ifdef TARGET_WIN32
	videoInput & device = ((ofDirectShowGrabber*) &* cam.getGrabber())->getDevice();
	int ID = 0;

	device.setVideoSettingCamera(ID, device.propFocus, 0);
	device.setVideoSettingCamera(ID, device.propExposure, -4);
	device.setVideoSettingCamera(ID, device.propGain, 10);

	device.setVideoSettingFilter(ID, device.propSharpness, 0);
#endif
}

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFullscreen(true);
	ofHideCursor();
	ofBackground(0);

	ofSetWindowShape(1280, 800);
	payload.init(1024, 800);
	encoder.init(payload);
	decoder.init(payload);

	this->reset();

	video.initGrabber(1280, 800, true);
	setDeviceProps(video);
	message.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
}

//--------------------------------------------------------------
void testApp::update(){
	if (!decoder.hasData())
		video.update();

	if (scanning && decoder.hasData() == false) {
		if (!encodeFrame) {
			int time = ofGetElapsedTimeMillis();
			while (ofGetElapsedTimeMillis() < time + WAIT_TIME) {
				ofSleepMillis(1);
				video.update();
			}
		}
		advance();
	} else {
		scanning = false;
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	if (scanning) {
		message.draw((ofGetWidth() - payload.getWidth()) / 2.0f, 0, payload.getWidth(), ofGetHeight());
	} else if (decoder.hasData()) {
		if (showScanFromCamera)
			decoder.getProjectorInCamera().draw(0, 0, ofGetWidth(), ofGetHeight());
		else
			decoder.getCameraInProjector().draw((ofGetWidth() - payload.getWidth()) / 2.0f,0, payload.getWidth(), ofGetHeight());
	} else {
		video.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	bool thresholdChanged = false;

	switch (key) {
	case 'f':
		ofToggleFullscreen();
		break;
	case 'v':
		video.videoSettings();
		getDeviceProps(video);
		break;
	case 'c':
		showScanFromCamera ^= true;
		break;
	case 'r':
		cout << "reset" << endl;
		reset();
		break;
	case 's':
		decoder.saveDataSet("scan");
		break;
	case ' ':
		scanning = true;
		cout << "Auto advance" << endl;
		break;
	case OF_KEY_UP:
		decoder.setThreshold(decoder.getThreshold() + 2);
		thresholdChanged = true;
		break;
	case OF_KEY_DOWN:
		decoder.setThreshold(decoder.getThreshold() - 2);
		thresholdChanged = true;
		break;
	}

	if (thresholdChanged) {
		cout << "threshold: " << (int) decoder.getThreshold() << endl;
	}
}

//--------------------------------------------------------------
void testApp::reset() {
	decoder.reset();
	encoder.reset();
	scanning = false;
	encodeFrame = true;
	showScanFromCamera = false;
	decoder.setThreshold(52);
	showScanFromCamera = false;
}

//--------------------------------------------------------------
void testApp::advance() {
	if (encodeFrame) {
		cout << "encode" << endl;
		if (encoder >> message)
			encodeFrame = false;
		else
			scanning = false;
	} else {
		cout << "decode" << endl;
		decoder << video;
		encodeFrame = true;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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