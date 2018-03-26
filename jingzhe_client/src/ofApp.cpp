#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	// global settings
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	ofSetVerticalSync(false);
	
	
	setupAudio();
	setupOSC();
	setupMIDI();
	setupKinect();
	setupSyphon();

}
//--------------------------------------------------------------

void ofApp::setupKinect(){
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
	
	
	
}
//--------------------------------------------------------------

void ofApp::setupMIDI(){
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
}
//--------------------------------------------------------------

void ofApp::setupOSC(){
	sender.setup(HOST, PORT);
}
//--------------------------------------------------------------

void ofApp::setupAudio(){
	// init audio
	soundStream.printDeviceList();
	
	deviceList = soundStream.getDeviceList();
	soundStream.setDeviceID(1);
	
	int inChennals = 2;
	int outChennals = 0;
	soundStream.setup(this, outChennals, inChennals, sampleRate, bufferSize, 8);
	
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
	
	
	audioAnalyzer.setup(sampleRate, bufferSize, inChennals);
	
	chennalBuffer.allocate(bufferSize, 1);
	
	
	
	
	// init fft all 0 .............................................
	fftSize = 128;
	for (int i = 0; i < fftSize; i ++) {
		fftData.push_back(i * 1.1f);
	}
	//	cout << &fft << endl;
	rmsData = 0.0f;
	
}
//--------------------------------------------------------------
void ofApp::setupSyphon(){
	
}
//--------------------------------------------------------------
void ofApp::setupGUI(){
	
}
//--------------------------------------------------------------
void ofApp::update(){
	oscSendAudioData();
	updateKinect();
}
//--------------------------------------------------------------
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
	
	// audio analyzer ================
	audioAnalyzer.exit();
	ofSoundStreamStop();
	
	
	
	
}


//--------------------------------------------------------------
// could be heavy works on many sound input analyse
void ofApp::myAudioAnalyze(ofSoundBuffer &soundBuffer){
	
	
	soundBuffer.getChannel(chennalBuffer, 0);
	
	fft->setSignal(chennalBuffer.getBuffer().data());
	
	curFft = fft->getAmplitude();
	
	//
	//	for(int i = 0; i < fftBins.size(); i++) {
	//		cout << fftBins[i] << endl;
	//	}
	
	//        cout << "----------- my audio analyzing -----------" << endl;
	
	//	fft->setSignal(chennalBuffer1.getBuffer().data());
	//	float* curFft = fft->getAmplitude();
	
	
	//	ofSoundBuffer tempSoundBuffer;
	
	//	for (int i = 0; i < UmcSfChannels; i++) {
	//		tempSoundBuffer = soundBuffer.getChannel(tempSoundBuffer, i);
	//		RMSs.at(i) = tempSoundBuffer.getRMSAmplitude();
	
	//		RMSs.at(i) = soundBuffer.getRMSAmplitudeChannel(i);
	//	}
	//
	
	
	
	//	float smoothing = 0.2;
	//-:get Values:
	//    rms     = audioAnalyzer.getValue(RMS, 0, smoothing);
	//	power   = audioAnalyzer.getValue(POWER, 0, smoothing);
	//    pitchFreq = audioAnalyzer.getValue(PITCH_FREQ, 0, smoothing);
	//    pitchConf = audioAnalyzer.getValue(PITCH_CONFIDENCE, 0, smoothing);
	//    pitchSalience  = audioAnalyzer.getValue(PITCH_SALIENCE, 0, smoothing);
	//    inharmonicity   = audioAnalyzer.getValue(INHARMONICITY, 0, smoothing);
	//    hfc = audioAnalyzer.getValue(HFC, 0, smoothing);
	//    specComp = audioAnalyzer.getValue(SPECTRAL_COMPLEXITY, 0, smoothing);
	//    centroid = audioAnalyzer.getValue(CENTROID, 0, smoothing);
	//    rollOff = audioAnalyzer.getValue(ROLL_OFF, 0, smoothing);
	//    oddToEven = audioAnalyzer.getValue(ODD_TO_EVEN, 0, smoothing);
	//    strongPeak = audioAnalyzer.getValue(STRONG_PEAK, 0, smoothing);
	//    strongDecay = audioAnalyzer.getValue(STRONG_DECAY, 0, smoothing);
	//    //Normalized values for graphic meters:
	//    pitchFreqNorm   = audioAnalyzer.getValue(PITCH_FREQ, 0, smoothing, TRUE);
	//    hfcNorm     = audioAnalyzer.getValue(HFC, 0, smoothing, TRUE);
	//    specCompNorm = audioAnalyzer.getValue(SPECTRAL_COMPLEXITY, 0, smoothing, TRUE);
	//    centroidNorm = audioAnalyzer.getValue(CENTROID, 0, smoothing, TRUE);
	//    rollOffNorm  = audioAnalyzer.getValue(ROLL_OFF, 0, smoothing, TRUE);
	//    oddToEvenNorm   = audioAnalyzer.getValue(ODD_TO_EVEN, 0, smoothing, TRUE);
	//    strongPeakNorm  = audioAnalyzer.getValue(STRONG_PEAK, 0, smoothing, TRUE);
	//    strongDecayNorm = audioAnalyzer.getValue(STRONG_DECAY, 0, smoothing, TRUE);
	//
	//    dissonance = audioAnalyzer.getValue(DISSONANCE, 0, smoothing);
	//
	//	spectrum = audioAnalyzer.getValues(SPECTRUM, 0, smoothing);
	//    melBands = audioAnalyzer.getValues(MEL_BANDS, 0, smoothing);
	//    mfcc = audioAnalyzer.getValues(MFCC, 0, smoothing);
	//    hpcp = audioAnalyzer.getValues(HPCP, 0, smoothing);
	//
	//    tristimulus = audioAnalyzer.getValues(TRISTIMULUS, 0, smoothing);
	//
	//    isOnset = audioAnalyzer.getOnsetValue(0);
	//
	
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
		m.addFloatArg(fftData.at(i));
	}
	
	b.addMessage(m);
	m.clear();
	
	m.setAddress(RMS);
	m.addFloatArg(rmsData);
	b.addMessage(m);
	
	sender.sendBundle(b);
}

//--------------------------------------------------------------

void ofApp::audioIn(ofSoundBuffer &inBuffer){
	//	cout << inBuffer.getDeviceID() << endl;
	
	
	
	audioInSoundBuffer = inBuffer;
	
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
