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
	int bufferSize = 256;
	
	ofxFft* 				fft;
	vector<float> 			fftBins;
	float*					curFft;
	
	
	void					audioIn(ofSoundBuffer &inBuffer);
	ofxAudioAnalyzer 		audioAnalyzer;
	ofSoundBuffer 			audioInSoundBuffer;
	void 					myAudioAnalyze(ofSoundBuffer &soundBuffer);
	
//	vector<float>		RMSs;
	
	
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
	void				drawKinect();
	
	ofxKinect 			kinect1;
	ofxKinectV2			kinect2;
	
	ofParameter<bool> 	bThreshWithOpenCV;
//	bool 				bDrawPointCloud;
	
	ofParameter<int>  	k1Angle;
	ofParameter<int>  	k2Angle;

	ofxCvColorImage 	colorImg;
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder 	contourFinder;
	
	// ------- for multi kinect2
//	ofxPanel 			kinect2Panel;
//
//	vector < shared_ptr<ofxKinectV2> > kinects;
//
//	vector <ofTexture> 	texDepth;
//	vector <ofTexture> 	texRGB;
	
	
	ofTexture			k1TexDepth;
	
	ofTexture			k2TexDepth;
//	ofTexture			k2TexRGB;
	
	ofParameter<int>	k1GrayThreshNear;
	ofParameter<int>	k1GrayThreshFar;
	
	ofParameter<int>	k2Near;
	ofParameter<int>	k2Far;
	ofParameter<int>	k2GrayThreshNear;
	ofParameter<int>	k2GrayThreshFar;

	ofParameter<int>	minAreaContour;
	ofParameter<int>	maxAreaContour;
	ofParameter<int>    contourConsidered;
	ofParameter<bool>	bContourFindHole;
	
	
	// contour of kinect2
	vector<float>			contourBlobAreas;
	vector<ofRectangle>		contourBoundingBoxes;
	vector<ofPoint>			contourBlobCentroids;
	// fluid particle =============================
	MyFlowTools			myFlowTools;
	ofFbo				fboForFluid;
	ofFbo				fboForObstacle;
	
	
	// syphon out =======================
	void				setupSyphon();
	void				updateSyphon();
	ofxSyphonServer 	k1OutputSyphonServer;
	ofxSyphonServer 	k2OutputSyphonServer;
	ofxSyphonServer 	animateOutputSyphonServer;
	ofxSyphonServer 	videoOutputSyphonServer;
	
	ofFbo				animateFbo;
	ofFbo				kinect1Fbo;
	ofFbo				kinect2Fbo;
	ofFbo				videoFbo;
	
	
	
	// gui ===============================
	void				setupGUI();
	void 				drawGui();

	ofxPanel			gui;
	
	
	
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
