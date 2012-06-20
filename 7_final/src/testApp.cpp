#include "testApp.h"

void setDeviceProps(ofVideoGrabber & cam) {


#ifdef TARGET_WIN32
	videoInput & device = ((ofDirectShowGrabber*) &* cam.getGrabber())->getDevice();
	int ID = 0;

	device.setVideoSettingFilter(ID, device.propSharpness, 0);
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	return;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	device.setVideoSettingCamera(ID, device.propFocus, 0);
	device.setVideoSettingCamera(ID, device.propExposure, -4);
	device.setVideoSettingCamera(ID, device.propGain, 10);

	
#endif
}

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofEnableSmoothing();

	rx.setup(5556);
	tx.setup("localhost", 5555);

	video.initGrabber(1280, 800, true);
	setDeviceProps(video);
	
	payload.init(1024, 800);
	data.init(payload);
	
	float fov = 83.0f;
	float throwratio = tan(fov / 2.0f) * 2.0f;
	
	camera.color = ofColor(255, 100, 100);
	camera.setWidth(data.getWidth());
	camera.setHeight(data.getHeight());

	ofMatrix4x4 cV = ofMatrix4x4(0.9793,  0.1949, -0.0547, 0,
								 0.1530, -0.8897, -0.4302, 0,
								 0.1325, -0.4129,  0.9011, 0,
								 -0.0939, 0.2546, 0.0401,  1);
	cV.preMultScale(ofVec3f(-1, 1, 1));
	cV.postMultScale(-1, 1, -1);
	
	ofMatrix4x4 cP = ofMatrix4x4(3.2723, 0, 0, 0,
								 0, 5.8250, 0, 0,
								 -0.6492, -0.5240, 1.01, 1.0,
								 0.0, 0.0, -0.0404, 0.0);
	cP.postMultScale(1, -1, 1);
	camera.setTransformMatrix(cV);
	camera.setProjection(cP);
	
	//camera.distortion = ofVec4f(-0.01, 2.5721, -0.0141, 0.0139);
	
	
	
	projector.color = ofColor(100, 100, 255);
	
	
	projector.setWidth(payload.getWidth());
	projector.setHeight(payload.getHeight());
	
	ofMatrix4x4 pV = ofMatrix4x4(0.9906, -0.1263, -0.0533, 0,
								 0.1125, 0.9710, -0.2110, 0,
								 0.0784, 0.2030, 0.9760, 0,
								 -0.2455, -0.1972, 0.0971, 1);
	pV.preMultScale(ofVec3f(-1, 1, 1));
	pV.postMultScale(-1, -1, -1);
	ofMatrix4x4 pP = ofMatrix4x4(3.6687, 0, 0, 0,
								 0, 6.5441, 0, 0,
								 0.1807, 1.3720, 1.0582, 1.0,
								 0, 0, -0.2328, 0.0);
	pP.preMultScale(ofVec3f(1.0f, -1.0f, 1.0f));
	pP.postMultScale(ofVec3f(1.0f, 1.0f, 1.0f));
	projector.setTransformMatrix(pV);
	projector.setProjection(pP);
	
	for (int i=0; i<3; i++) {
		slide[i].allocate(payload.getWidth(), payload.getHeight(), OF_IMAGE_GRAYSCALE);
	}
	
	this->threshold = 1e11;
	this->triangulate();
	
	this->offset = ofVec3f(0.07, 0.33, 0.76);
	this->rotation = ofVec3f(-49,7,9);
	this->makeTransform();
	
	this->seperate();

	
	data.setThreshold(10);
}

//--------------------------------------------------------------
void testApp::triangulate(){
	ofxTriangulate::Triangulate(data.getDataSet(), camera, projector, mesh, threshold);
	cout << mesh.getVertices().size() << " found" << endl;
}

//--------------------------------------------------------------
void testApp::makeTransform() {
	transform.makeIdentityMatrix();
	transform.postMultRotate(rotation.x, 1, 0, 0);
	transform.postMultRotate(rotation.z, 0, 0, 1);
	transform.postMultRotate(rotation.y, 0, 1, 0);
	transform.preMultTranslate(offset);
}

//--------------------------------------------------------------
void testApp::seperate(){
	if (data.getDataSet().getHasData()) {
		for (int i=0; i<3; i++)
			memset(slide[i].getPixels(), 0, slide[i].getWidth() * slide[i].getHeight());
		
		int iPoint=0;
		ofxGraycode::DataSet::const_iterator it;
		for (it = data.getDataSet().begin(); it != data.getDataSet().end(); ++it) {
			ofxGraycode::DataSet::const_iterator::reference p(*it);
			if (p.active) {
				ofVec3f xyz = mesh.getVertex(iPoint++) * transform;	
				float z = xyz.z;
				
				float findex = ofMap(z, -0.03, -0.2, 0, 2) + 0.5f;
				unsigned int index = findex;
				
				if (index < 3)
					slide[index].getPixels()[p.projector] = 255;
			}
		}
		
		ofImage rgb;
		int width = slide[0].getWidth();
		int height = slide[0].getHeight();
		rgb.allocate(width, height, OF_IMAGE_COLOR);
		for (int i=0; i<3; i++) {
			slide[i].update();

			unsigned char * in = slide[i].getPixels();
			unsigned char * out = rgb.getPixels();

			for (int p=0; p<width*height; p++)
			{
				memset(out, *in, 3);
				in++;
				out+=3;
			}

			rgb.saveImage(ofToString(i) + ".png");
		}
	}
}


//--------------------------------------------------------------
void testApp::update(){

	video.update();

	while (rx.hasWaitingMessages()) {
		ofxOscMessage msg;
		rx.getNextMessage(&msg);

		cout << msg.getAddress() << endl;

		if (msg.getAddress() == "/capture")
		{
			if (!data.hasData()) {
				data << video;
				if (data.hasData()) {
					triangulate();
					seperate();
					data.saveDataSet("scan");
					ofxOscMessage cheer;
					cheer.setAddress("/complete");
					tx.sendMessage(cheer);
				} else {
					ofxOscMessage request;
					request.setAddress("/request");
					tx.sendMessage(request);
				}
			}
		}

		if (msg.getAddress() == "/clear")
		{
			data.reset();
		}
	}
}

//--------------------------------------------------------------
void testApp::customDraw(){
	
	ofPushMatrix();
	glMultMatrixf(transform.getPtr());
	
	camera.draw();
	projector.draw();
	mesh.drawVertices();

	camera.drawOnNearPlane(data.getProjectorInCamera());
	projector.drawOnNearPlane(data.getCameraInProjector());
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::draw(){
	video.draw(0, 0);

	ofDrawBitmapString("frame : " + ofToString(data.getFrame()) + "/" + ofToString(data.getFrameCount()), 20, 20);
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, 100);
	data.draw(50, 100, 300, 200);
	data.getCameraInProjector().draw(400, 100, 300, 200);
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	switch (key) {
		case 't':
			triangulate();
			break;
		case 's':
			seperate();
			break;

		case 'v':
			video.videoSettings();
			break;
		
		case OF_KEY_UP:
			data.setThreshold(data.getThreshold()+1);
			cout << "Threshold: " << (int)data.getThreshold() << endl;
			break;
		case OF_KEY_DOWN:
			data.setThreshold(data.getThreshold()-1);
			cout << "Threshold: " << (int)data.getThreshold() << endl;
			break;

		case 'a':
			data.saveDataSet("scan");
			break;

		default:
			break;
	}
	
	this->makeTransform();
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