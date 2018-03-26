#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	// global settings
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	ofSetVerticalSync(false);
	
	
	
	// init fft all 0 .............................................
	fftSize = 128;
	for (int i = 0; i < fftSize; i ++) {
		fft.push_back(i * 1.1f);
	}
	//	cout << &fft << endl;
	rms = 0.0f;
	sender.setup(HOST, PORT);
	
	
	// init midi .............................................
	// print the available output ports to the console
	midiOut.listPorts(); // via instance
	//ofxMidiOut::listPorts(); // via static too
	
	// connect
	midiOut.openPort(0); // by number
	//midiOut.openPort("IAC Driver Pure Data In"); // by name
	channel = 1;
	note = 0;
	velocity = 0;
	
	// init kinect kinect2 .............................................
	kinect.setRegistration(true);
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 245;
	farThreshold = 213;
	bThreshWithOpenCV = true;
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	
	
	// syphon .............................................
	
	
	
	
	
}

//--------------------------------------------------------------
void ofApp::update(){
	oscSendAudioData();
	updateKinect();
	
	
	
}

void ofApp::updateKinect(){
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels());
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
			
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			ofPixels & pix = grayImage.getPixels();
			int numPixels = pix.size();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		//		Canny(grayImage, grayImage, 100, 200, 3);
		
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20,
								   // if use approximate points
								   //								   true
								   false
								   );
		
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
}
//--------------------------------------------------------------
void ofApp::exit(){
	midiOut.closePort();
	
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
	
	
}
//--------------------------------------------------------------
void ofApp::midiSend(){
	int key = 69;// for test only
	note = ofMap(key, 48, 122, 0, 127);
	velocity = 10;
	midiOut.sendNoteOn(channel, note,  velocity);
	
	// print out both the midi note and the frequency
	ofLogNotice() << "note: " << note
	<< " freq: " << ofxMidi::mtof(note) << " Hz";
}
//--------------------------------------------------------------
void ofApp::oscSendAudioData(){
	ofxOscBundle b;
	ofxOscMessage m;
	m.setAddress(FFT);
	m.addIntArg(fftSize);
	for(int i = 0;i < fftSize;i++){
		m.addFloatArg(fft.at(i));
	}
	
	b.addMessage(m);
	m.clear();
	
	m.setAddress(RMS);
	m.addFloatArg(rms);
	b.addMessage(m);
	
	sender.sendBundle(b);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
	
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	
}
