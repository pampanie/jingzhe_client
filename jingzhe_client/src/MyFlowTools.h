//
//  #include "MyFlowTools.h" MyFlowTools.hpp
//  jingzhe_client
//
//  Created by Pampa Nie on 26/03/2018.
//

#pragma once

#ifndef MyFlowTools_h
#define MyFlowTools_h

#include <stdio.h>
#include "ofxFlowTools.h"
#include "ofxGui.h"

using namespace flowTools;

enum drawModeEnum{
	DRAW_NOTHING = 0,
	DRAW_COMPOSITE,
	//	DRAW_FLUID_DENSITY,
	DRAW_PARTICLES,
	//	DRAW_VELDOTS,
	DRAW_FLUID_FIELDS,
	//	DRAW_FLUID_VELOCITY,
	//	DRAW_FLUID_PRESSURE,
	//	DRAW_FLUID_TEMPERATURE,
	//	DRAW_FLUID_DIVERGENCE,
	//	DRAW_FLUID_VORTICITY,
	//	DRAW_FLUID_BUOYANCY,
	DRAW_FLUID_OBSTACLE,
	//	DRAW_FLOW_MASK,
	//	DRAW_OPTICAL_FLOW,
	DRAW_SOURCE,
	DRAW_MOUSE // not use yet ================  TODO
};

class MyFlowTools {
public:
	MyFlowTools();
	~MyFlowTools();
	void setup(int _w,int _h,float _ratio,string settingFileName);
	void update(ofFbo *_fboForFluidP,ofFbo *_obstacleFboP);
	void draw();
	void exit();
	
	void setParticleColor(ofColor _color);
	ofColor getParticleColor();
	void drawComposite(){
		drawComposite(0,0,drawWidth,drawHeight);
	};
	void drawComposite(int _x, int _y, int _width, int _height);
	
	void drawParticles(){
		drawParticles(0,0,drawWidth,drawHeight);
	};
	void drawParticles(int _x, int _y, int _width, int _height);
	
	void drawFluidFields(){
		drawFluidFields(0,0,drawWidth,drawHeight);
	};
	void drawFluidFields(int _x, int _y, int _width, int _height);
	
	void drawFluidObstacle(){
		drawFluidObstacle(0,0,drawWidth,drawHeight);
	}
	void drawFluidObstacle(int _x, int _y, int _width, int _height);
	
	void drawSource(){
		drawSource(0,0,drawWidth,drawHeight);
	}
	void drawSource(int _x, int _y, int _width, int _height);
	
	int getDrawWidth();
	int getDrawHeight();
	int getFlowWidth();
	int getFlowHeight();
	void setDrawWidth(int _w);
	void setDrawHeight(int _h);
	void setFlowToDrawRatio(float _ratio);
	float getFlowToDrawRatio();
	
	
	ftOpticalFlow		opticalFlow;
	ftVelocityMask		velocityMask;
	ftFluidSimulation	fluidSimulation;
	ftParticleFlow		particleFlow;
	ftVelocityField		velocityField;
	ftTemperatureField	temperatureField;
	ftPressureField		pressureField;
	ftVTField			velocityTemperatureField;
	
	
	// gui
	ofParameter<int>	drawMode;
	ofxPanel			gui;
	void 				setupGui();
	void				updateGui();
	void 				drawGui();
	void				drawModeSetName(int & _value);
	ofParameter<string> drawName;
	string				myID;
	
	
	
private:
	float ratio;
	int drawWidth;
	int drawHeight;
	int flowWidth;
	int flowHeight;
	
	
	
};


#endif /* MyFlowTools_h */


// backup =======================================


//	ftOpticalFlow		opticalFlow;
//	ftVelocityMask		velocityMask;
//	ftFluidSimulation	fluidSimulation;
//	ftParticleFlow		particleFlow;
////	ftVelocitySpheres	velocityDots;
//
//	ofImage				flowToolsLogoImage;
//	bool				showLogo;
//
//	// MouseDraw
////	ftDrawMouseForces	mouseForces;
//
//	// Visualisations
//	ofParameterGroup	visualizeParameters;
//	ftDisplayScalar		displayScalar;
//	ftVelocityField		velocityField;
//	ftTemperatureField	temperatureField;
//	ftPressureField		pressureField;
//	ftVTField			velocityTemperatureField;

//	ofParameter<bool>	showScalar;
//	ofParameter<bool>	showField;
//	ofParameter<float>	displayScalarScale;
//	void				setDisplayScalarScale(float& _value) { displayScalar.setScale(_value); }
//	ofParameter<float>	velocityFieldScale;
//	void				setVelocityFieldScale(float& _value) { velocityField.setVelocityScale(_value); velocityTemperatureField.setVelocityScale(_value); }
//	ofParameter<float>	temperatureFieldScale;
//	void				setTemperatureFieldScale(float& _value) { temperatureField.setTemperatureScale(_value); velocityTemperatureField.setTemperatureScale(_value); }
//	ofParameter<float>	pressureFieldScale;
//	void				setPressureFieldScale(float& _value) { pressureField.setPressureScale(_value); }
//	ofParameter<bool>	velocityLineSmooth;
//	void				setVelocityLineSmooth(bool& _value) { velocityField.setLineSmooth(_value); velocityTemperatureField.setLineSmooth(_value);  }



//	void				drawComposite()			{ drawComposite(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawComposite(int _x, int _y, int _width, int _height);
//	void				drawParticles()			{ drawParticles(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawParticles(int _x, int _y, int _width, int _height);
//	void				drawFluidFields()		{ drawFluidFields(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidFields(int _x, int _y, int _width, int _height);
//	void				drawFluidDensity()		{ drawFluidDensity(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidDensity(int _x, int _y, int _width, int _height);
//	void				drawFluidVelocity()		{ drawFluidVelocity(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidVelocity(int _x, int _y, int _width, int _height);
//	void				drawFluidPressure()		{ drawFluidPressure(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidPressure(int _x, int _y, int _width, int _height);
//	void				drawFluidTemperature()	{ drawFluidTemperature(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidTemperature(int _x, int _y, int _width, int _height);
//	void				drawFluidDivergence()	{ drawFluidDivergence(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidDivergence(int _x, int _y, int _width, int _height);
//	void				drawFluidVorticity()	{ drawFluidVorticity(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidVorticity(int _x, int _y, int _width, int _height);
//	void				drawFluidBuoyance()		{ drawFluidBuoyance(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidBuoyance(int _x, int _y, int _width, int _height);
//	void				drawFluidObstacle()		{ drawFluidObstacle(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawFluidObstacle(int _x, int _y, int _width, int _height);
//	void				drawMask()				{ drawMask(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawMask(int _x, int _y, int _width, int _height);
//	void				drawOpticalFlow()		{ drawOpticalFlow(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawOpticalFlow(int _x, int _y, int _width, int _height);
//	void				drawSource()			{ drawSource(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawSource(int _x, int _y, int _width, int _height);
//	void				drawMouseForces()		{ drawMouseForces(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawMouseForces(int _x, int _y, int _width, int _height);

//	void				drawVelocityDots()		{ drawVelocityDots(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
//	void				drawVelocityDots(int _x, int _y, int _width, int _height);

