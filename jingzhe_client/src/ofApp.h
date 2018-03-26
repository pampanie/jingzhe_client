#pragma once

#include "ofMain.h"

#include "ofxFft.h"

#include "ofxOsc.h"

#include "ofxMidi.h"

#include "ofxOpenCv.h"
#include "ofxCv.h"

#include "ofxKinect.h"
#include "ofxKinectV2.h"

#include "ofxFlowTools.h"
#include "MyFlowTools.h"

#include "ofxSyphon.h"

#include "ofxFft.h"
#include "ofxAudioAnalyzer.h"
#include "ofxAudioDecoder.h"

using namespace cv;
using namespace ofxCv;


#define HOST "localhost"
#define PORT 12345

// protocol for osc address
#define FFT 	"/fft"
#define RMS		"/rms"


class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	// global =======================
	int					syphonTexW = 640;
	int					syphonTexH = 360;
	
	
	// audio ================
	void					setupAudio();
	ofSoundStream 			soundStream;
	ofSoundStream			soundStreamUmc;
	ofSoundStream			soundStreamSfUmc;
	vector<ofSoundDevice>	deviceList;
	
	ofSoundBuffer			chennalBuffer;
	
	int sampleRate = 44100;
	int bufferSize = 512;
	
	ofxFft* 				fft;
	vector<float> 			fftBins;
	float*					curFft;
	
	
	void					audioIn(ofSoundBuffer &inBuffer);
	ofxAudioAnalyzer 		audioAnalyzer;
	ofSoundBuffer 			audioInSoundBuffer;
	void 					myAudioAnalyze(ofSoundBuffer &soundBuffer);
	
	vector<float>		RMSs;
	
	
	// osc ===================
	void				setupOSC();
	void 				oscSendAudioData();
	ofxOscSender 		sender;
	int					fftSize;
	float				rmsData;
	vector<float>		fftData;
	
	// midi out
	void				setupMIDI();
	void				midiSend();
	ofxMidiOut 			midiOut;
	int 				channel;
	int 				note, velocity;
	
	// kinect kinect2=================
	void				setupKinect();
	void				updateKinect();
	
	ofxKinect 			kinect;
	ofxKinectV2			kinect2;
	
	bool 				bThreshWithOpenCV;
	bool 				bDrawPointCloud;
	
	int 				nearThreshold;
	int 				farThreshold;
	
	int 				angle;
	
	ofxCvColorImage 	colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder 	contourFinder;
	
	// fluid particle =============================
	MyFlowTools			myFlowTools;
	ofFbo				fboForFluid;
	ofFbo				fboForObstacle;
	
	
	// syphon out =======================
	void				setupSyphon();
	ofxSyphonServer 	mainOutputSyphonServer;
	ofFbo				mainOutFbo;
	ofFbo				animateFbo;
	ofFbo				kinectFbo;
	ofFbo				videoFbo;
	
	
	
	// gui ===============================
	void				setupGUI();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
