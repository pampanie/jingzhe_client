#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	// global settings
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	ofSetVerticalSync(false);
	// setup kinect before syphon
	setupKinect();

	
	setupAudio();
	setupOSC();
	setupMIDI();
	
	setupSyphon();
	setupGUI();

}
//--------------------------------------------------------------

void ofApp::setupKinect(){
	// init kinect kinect2 .............................................
	kinect1.setRegistration(true);
//	kinect1.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	kinect1.init(false, false); // disable video image (faster fps)
	
	kinect1.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	// print the intrinsic IR sensor values
	if(kinect1.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect1.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect1.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect1.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect1.getZeroPlaneDistance() << "mm";
	}
	
	
//	colorImg.allocate(kinect1.width, kinect1.height);
	grayImage.allocate(kinect1.width, kinect1.height);
	grayThreshNear.allocate(kinect1.width, kinect1.height);
	grayThreshFar.allocate(kinect1.width, kinect1.height);
	

	
	//see how many devices we have.
	ofxKinectV2 tmp;
	vector <ofxKinectV2::KinectDeviceInfo> deviceList = tmp.getDeviceList();
	cout << deviceList.size() << endl;

//	kinect2.open(deviceList[0].serial);
//	k2TexDepth.allocate(kinect2.getDepthPixels());

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
		fftData.push_back(0.0f);
	}
	rmsData = 0.0f;
	
}
//--------------------------------------------------------------
void ofApp::setupSyphon(){
	
	kinect1Fbo.allocate(syphonTexW, syphonTexH,GL_RGBA);
	k1OutputSyphonServer.setName("kinect1 output syphon");

	kinect2Fbo.allocate(syphonTexW, syphonTexH,GL_RGBA);
	k2OutputSyphonServer.setName("kinect2 output syphon");
	
	
	
}
//--------------------------------------------------------------
void ofApp::setupGUI(){
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	
	
	
	// ...............  kinect
	gui.add(bThreshWithOpenCV.set("thresh with opencv",false));
	gui.add(k1GrayThreshNear.set("kinect1 near",0,1,255));
	gui.add(k1GrayThreshFar.set("kinect1 far",0,1,255));
	gui.add(k1Angle.set("kinect1 angle",0,2,90));
	
	gui.add(k2Near.set("kinect2 near",0,100,10000));
	gui.add(k2Far.set("kinect2 far",0,100,6000));
//	gui.add(k2Angle.set("kinect2 angle",0,2,90));
	gui.add(k2GrayThreshNear.set("kinect2 gray near",0,1,255));
	gui.add(k2GrayThreshFar.set("kinect2 gray far",0,1,255));
	
	gui.add(minAreaContour.set("min contour area", 10, 0, 100));
	gui.add(maxAreaContour.set("max contour area", 10, 0, 1000));
	gui.add(contourConsidered.set("contour considered",0,1,10));
	gui.add(bContourFindHole.set("contour with hole",false));

	
	// seva setting with give name
	if (!ofFile("settings.xml"))
		gui.saveToFile("settings.xml");
	
	gui.loadFromFile("settings.xml");
}
//--------------------------------------------------------------
void ofApp::update(){
	oscSendAudioData();
	updateKinect();
	updateSyphon();
}
//--------------------------------------------------------------
void ofApp::updateSyphon(){
	
	// for test
	kinect1Fbo.begin();
	ofSetColor(255,0,0,255);
	ofDrawRectangle(0,0,syphonTexW,syphonTexH);
	kinect1Fbo.end();
	k1OutputSyphonServer.publishTexture(&kinect1Fbo.getTexture());

	// for use
//	k1OutputSyphonServer.publishTexture(&grayImage.getTexture());
	
	// for test
	kinect2Fbo.begin();
	ofSetColor(0,255,0,255);
	ofDrawRectangle(0,0,syphonTexW,syphonTexH);
	kinect2Fbo.end();
	k2OutputSyphonServer.publishTexture(&kinect2Fbo.getTexture());

	// for use
//	k2OutputSyphonServer.publishTexture(&k2TexDepth);
}

//--------------------------------------------------------------
void ofApp::updateKinect(){
	kinect1.setCameraTiltAngle(k1Angle.get());
	kinect1.update();
	
	// there is a new frame and we are connected
	if(kinect1.isFrameNew()) {
		
		// load grayscale depth image from the kinect1 source
		grayImage.setFromPixels(kinect1.getDepthPixels());
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(k1GrayThreshNear.get(), true);
			grayThreshFar.threshold(k1GrayThreshFar.get());
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
			
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			ofPixels & pix = grayImage.getPixels();
			int numPixels = pix.size();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < k1GrayThreshNear.get() && pix[i] > k1GrayThreshFar.get()) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		//		Canny(grayImage, grayImage, 100, 200, 3);
		k1TexDepth.loadData(grayImage.getPixels());
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
//		contourFinder.findContours(grayImage, 10, (kinect1.width*kinect1.height)/2, 20,
//								   // if use approximate points
//								   //								   true
//								   false
//								   );
		
	}
	
	// clear for use for kinect2
	grayImage.clear();
	
	
	// kinect2
	kinect2.update();
	if(kinect2.isFrameNew()){
		kinect2.minDistance = k2Near.get();
		kinect2.maxDistance = k2Far.get();
		
		
		// load grayscale depth image from the kinect1 source
		grayImage.setFromPixels(kinect2.getDepthPixels());
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(k2GrayThreshNear.get(), true);
			grayThreshFar.threshold(k2GrayThreshFar.get());
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
			
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			ofPixels & pix = grayImage.getPixels();
			int numPixels = pix.size();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < k2GrayThreshNear.get() && pix[i] > k2GrayThreshFar.get()) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		//		Canny(grayImage, grayImage, 100, 200, 3);
		
		k2TexDepth.loadData(grayImage.getPixels());
		
		
		// get kinect2 catch  bounding box
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinect1.width*kinect1.height)/2, 20,
										   // if use approximate points
										   //								   true
										   false
										   );
		
		contourFinder.findContours(grayImage,
								   minAreaContour.get(),
								   maxAreaContour.get(),
								   contourConsidered.get(),
								   bContourFindHole.get());
		
		
		// got contours
		int blobs = contourFinder.blobs.size();
		if(blobs > 0){
			
			//		cout << contourFinder.blobs.at(0).nPts << endl;
			for (int i = 0; i<blobs; i++) {
				contourBlobAreas.push_back(contourFinder.blobs.at(i).area);
				contourBoundingBoxes.push_back(contourFinder.blobs.at(i).boundingRect);
				contourBlobCentroids.push_back(contourFinder.blobs.at(i).centroid);
				
			}

		}
		
	}

	
}

//--------------------------------------------------------------
void ofApp::draw(){
	drawKinect();
	drawGui();

}

//--------------------------------------------------------------
void ofApp::drawKinect(){
//	ofDrawBitmapString("ofxKinectV2: Work in progress addon.\nBased on the excellent work by the OpenKinect libfreenect2 team\n\n-Requires USB 3.0 port ( superspeed )\n-Requires patched libusb. If you have the libusb from ofxKinect ( v1 ) linked to your project it will prevent superspeed on Kinect V2", 10, 14);

	// for debug show
//	k1TexDepth.draw(0,0);
//	k2TexDepth.draw(650,0);
	
}


void ofApp::drawGui(){
	gui.draw();
}


//--------------------------------------------------------------
void ofApp::exit(){
	midiOut.closePort();
	
	kinect1.setCameraTiltAngle(0); // zero the tilt on exit
	kinect1.close();
	
	kinect2.close();
	
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
