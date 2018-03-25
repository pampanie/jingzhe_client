#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxOsc.h"

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
	
	void oscSendAudioData();
	ofxOscSender 		sender;
	int					fftSize;
	float				rms;
	vector<float>		fft;
	
	
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
