#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	
	payload.init(1024, 800);
	data.init(payload);
	this->load();
	
	gui.init();
	gui.addInstructions();
	gui.add(data.getCameraInProjector(), "Camera in projector");
	gui.add(data.getProjectorInCamera(), "Projector in camera");
	ofPtr<ofxCvGui::Panels::Node> nodepanel = gui.add(*this, "3D");
	for (int i=0; i<3; i++) {
		gui.add(slide[i], "Slide " + ofToString(i));
	}

	
	this->cam = &(nodepanel->getCamera());
	nodepanel->setGridScale(2.0f);
	this->cam->setPosition(1.0f, 1.0f, 1.0f);
	this->cam->lookAt(ofVec3f());
	this->cam->setNearClip(0.01f);
	
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
		slide[i].allocate(data.getDataSet().getPayloadWidth(), data.getDataSet().getPayloadHeight(), OF_IMAGE_GRAYSCALE);
	}
	
	this->threshold = 0.1;
	this->triangulate();
	
	this->offset = ofVec3f(0.07, 0.33, 0.76);
	this->rotation = ofVec3f(-43,14,9);
	this->makeTransform();
	
	this->seperate();
}

//--------------------------------------------------------------
void testApp::load(){
	data.loadDataSet("scan");
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
				
				float findex = (z / (-0.19f)) * 3.0f;
				unsigned int index = findex;
				if (iPoint % 20 == 0)
					cout << "xyz: " << xyz  << ", index:" << index << ", findex:" << findex << endl;
				
				if (index < 3)
					slide[index].getPixels()[p.projector] = 255;
			}
		}
		
		for (int i=0; i<3; i++) {
			slide[i].update();
			slide[i].saveImage(ofToString(i) + ".png");
		}
	}
}


//--------------------------------------------------------------
void testApp::update(){
	
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
	ofxCvGui::AssetRegister.drawText("threshold = " + ofToString(threshold), 20, 300);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	switch (key) {
		case 'l':
			load();
			break;
		case 't':
			triangulate();
			break;
		case 's':
			seperate();
			break;
			
		case 'c':
			cam->toggleCursorDraw();
			break;
			
		case OF_KEY_LEFT:
			rotation.y -= 1;
			break;
		case OF_KEY_RIGHT:
			rotation.y += 1;
			break;
		case OF_KEY_DOWN:
			rotation.x -= 1;
			break;
		case OF_KEY_UP:
			rotation.x += 1;
			break;
		case 'a':
			rotation.z -= 1;
			break;
		case 'z':
			rotation.z += 1;
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