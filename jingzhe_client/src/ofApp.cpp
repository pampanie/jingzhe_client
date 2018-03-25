#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// init fft all 0
	fftSize = 128;
	for (int i = 0; i < fftSize; i ++) {
		fft.push_back(i * 1.1f);
	}
//	cout << &fft << endl;
	rms = 0.0f;
	sender.setup(HOST, PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
	oscSendAudioData();
}

//--------------------------------------------------------------
void ofApp::draw(){

}
//--------------------------------------------------------------
void ofApp::exit(){
	
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
