//
//  #include "MyFlowTools.h" MyFlowTools.cpp
//  jingzhe_client
//
//  Created by Pampa Nie on 26/03/2018.
//

#include "MyFlowTools.h"

MyFlowTools::MyFlowTools(){
	
}

MyFlowTools::~MyFlowTools(){
	
}
//--------------------------------------------------------------

void MyFlowTools::setup(int _w,int _h,float _ratio,string _id){
	drawWidth = _w;
	drawHeight = _h;
	flowWidth = _w / _ratio;
	flowHeight = _h / _ratio;
	ratio = _ratio;
	
	opticalFlow.setup(flowWidth, flowHeight);
	velocityMask.setup(drawWidth, drawHeight);
	
	// FLUID & PARTICLES
	fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	//	velocityDots.setup(flowWidth / 4, flowHeight / 4);
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	
	// VISUALIZATION
	//	displayScalar.setup(flowWidth, flowHeight);
	//	velocityField.setup(flowWidth / 4, flowHeight / 4);
	//	temperatureField.setup(flowWidth / 4, flowHeight / 4);
	//	pressureField.setup(flowWidth / 4, flowHeight / 4);
	//	velocityTemperatureField.setup(flowWidth / 4, flowHeight / 4);
	
	// MOUSE DRAW
	//	mouseForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	myID = _id;
	setupGui();
	
}
//--------------------------------------------------------------

void MyFlowTools::update(ofFbo *_fboForFluidP,ofFbo *_obstacleFboP){
	
	opticalFlow.setSource(_fboForFluidP->getTexture());
	opticalFlow.update();
	velocityMask.setDensity(_fboForFluidP->getTexture());
	velocityMask.setVelocity(opticalFlow.getOpticalFlow());
	velocityMask.update();
	
	
	
	// fluid
	fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
	fluidSimulation.addDensity(velocityMask.getColorMask());
	fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
	//	fluidSimulation.addObstacle(obstacleFbo.getTexture());
	// try to add temporary obstacle
	
	fluidSimulation.addTempObstacle(_obstacleFboP->getTexture());
	
	
	fluidSimulation.update();
	
	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluidSimulation.getSpeed());
		particleFlow.setCellSize(fluidSimulation.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
		//		particleFlow.addDensity(fluidSimulation.getDensity());
		particleFlow.setObstacle(fluidSimulation.getObstacle());
	}
	particleFlow.update();
	
	
}


//--------------------------------------------------------------
void MyFlowTools::draw(){
	
	switch(drawMode) {
		case DRAW_COMPOSITE: drawComposite(); break;
		case DRAW_PARTICLES: drawParticles(); break;
		case DRAW_FLUID_FIELDS: drawFluidFields(); break;
			//			case DRAW_FLUID_DENSITY: drawFluidDensity(); break;
			//			case DRAW_FLUID_VELOCITY: drawFluidVelocity(); break;
			//			case DRAW_FLUID_PRESSURE: drawFluidPressure(); break;
			//			case DRAW_FLUID_TEMPERATURE: drawFluidTemperature(); break;
			//			case DRAW_FLUID_DIVERGENCE: drawFluidDivergence(); break;
			//			case DRAW_FLUID_VORTICITY: drawFluidVorticity(); break;
			//			case DRAW_FLUID_BUOYANCY: drawFluidBuoyance(); break;
		case DRAW_FLUID_OBSTACLE: drawFluidObstacle(); break;
			//			case DRAW_FLOW_MASK: drawMask(); break;
			//			case DRAW_OPTICAL_FLOW: drawOpticalFlow(); break;
		case DRAW_SOURCE: drawSource(); break;
			//			case DRAW_MOUSE: drawMouseForces(); break;
			//			case DRAW_VELDOTS: drawVelocityDots(); break;
	}
	
	
}
//--------------------------------------------------------------

void MyFlowTools::exit(){
	
}

//--------------------------------------------------------------

void MyFlowTools::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	
	gui.add(drawMode.set("draw mode", DRAW_NOTHING, DRAW_NOTHING, DRAW_MOUSE));
	
	drawMode.addListener(this,&MyFlowTools::drawModeSetName);
	gui.add(drawName.set("MODE", "draw name"));
	
	
	int guiColorSwitch = 0;
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(particleFlow.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(opticalFlow.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(fluidSimulation.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(velocityMask.parameters);
	
	gui.minimizeAll();
	
	
}
//--------------------------------------------------------------

void MyFlowTools::drawGui() {
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui.draw();
	ofPopStyle();
}
//--------------------------------------------------------------

void MyFlowTools::drawModeSetName(int &_value) {
	switch(_value) {
		case DRAW_NOTHING:			drawName.set("Draw Nothing");		break;
		case DRAW_COMPOSITE:		drawName.set("Composite"); 			break;
		case DRAW_PARTICLES:		drawName.set("Particles"); 			break;
		case DRAW_FLUID_FIELDS:		drawName.set("Fluid Fields"); 		break;
			//		case DRAW_FLUID_DENSITY:	drawName.set("Fluid Density  "); break;
			//		case DRAW_FLUID_VELOCITY:	drawName.set("Fluid Velocity (3)"); break;
			//		case DRAW_FLUID_PRESSURE:	drawName.set("Fluid Pressure (4)"); break;
			//		case DRAW_FLUID_TEMPERATURE:drawName.set("Fld Temperature(5)"); break;
			//		case DRAW_FLUID_DIVERGENCE: drawName.set("Fld Divergence "); break;
			//		case DRAW_FLUID_VORTICITY:	drawName.set("Fluid Vorticity"); break;
			//		case DRAW_FLUID_BUOYANCY:	drawName.set("Fluid Buoyancy "); break;
		case DRAW_FLUID_OBSTACLE:	drawName.set("Fluid Obstacle"); 	break;
			//		case DRAW_OPTICAL_FLOW:		drawName.set("Optical Flow   (6)"); break;
			//		case DRAW_FLOW_MASK:		drawName.set("Flow Mask      (7)"); break;
		case DRAW_SOURCE:			drawName.set("Source"); 			break;
			//		case DRAW_MOUSE:			drawName.set("Left Mouse     (8)"); break;
			//		case DRAW_VELDOTS:			drawName.set("VelDots        (0)"); break;
	}
}


//--------------------------------------------------------------

void MyFlowTools::setParticleColor(ofColor _color){
	particleFlow.setParticleColor(_color);
}
//--------------------------------------------------------------
ofColor MyFlowTools::getParticleColor(){
	return particleFlow.getParticleColor();
}

//--------------------------------------------------------------

void MyFlowTools::setFlowToDrawRatio(float _ratio){
	ratio = _ratio;
	flowWidth = drawWidth / ratio;
	flowHeight = drawHeight / ratio;
}
//--------------------------------------------------------------

float MyFlowTools::getFlowToDrawRatio(){
	return ratio;
}
//--------------------------------------------------------------

void MyFlowTools::setDrawWidth(int _w){
	drawWidth = _w;
	flowWidth = drawWidth / ratio;
}
//--------------------------------------------------------------

int MyFlowTools::getDrawWidth(){
	return drawWidth;
}
//--------------------------------------------------------------

void MyFlowTools::setDrawHeight(int _h){
	drawHeight = _h;
	flowHeight = drawHeight / ratio;
}
//--------------------------------------------------------------

int MyFlowTools::getDrawHeight(){
	return drawHeight;
}
//--------------------------------------------------------------

int MyFlowTools::getFlowWidth(){
	return flowWidth;
}
//--------------------------------------------------------------

int MyFlowTools::getFlowHeight(){
	return flowHeight;
}
//--------------------------------------------------------------

void MyFlowTools::drawComposite(int _x, int _y, int _width, int _height){
	ofPushStyle();
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	fluidSimulation.draw(_x, _y, _width, _height);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	ofPopStyle();
}
//--------------------------------------------------------------

void MyFlowTools::drawParticles(int _x, int _y, int _width, int _height){
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	ofPopStyle();
}
//--------------------------------------------------------------

void MyFlowTools::drawFluidFields(int _x, int _y, int _width, int _height){
	ofPushStyle();
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	pressureField.setPressure(fluidSimulation.getPressure());
	pressureField.draw(_x, _y, _width, _height);
	velocityTemperatureField.setVelocity(fluidSimulation.getVelocity());
	velocityTemperatureField.setTemperature(fluidSimulation.getTemperature());
	velocityTemperatureField.draw(_x, _y, _width, _height);
	temperatureField.setTemperature(fluidSimulation.getTemperature());
	
	ofPopStyle();
}

//--------------------------------------------------------------

void MyFlowTools::drawFluidObstacle(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	fluidSimulation.getObstacle().draw(_x, _y, _width, _height);
	ofPopStyle();
}

void MyFlowTools::drawSource(int _x, int _y, int _width, int _height){
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	//	cameraFbo.draw(_x, _y, _width, _height);  ====== TODO
	ofPopStyle();
}

//-==================================================
//==================================================
//	backup from update();
//	mouseForces.update(deltaTime);

//	for (int i=0; i<mouseForces.getNumForces(); i++) {
//		if (mouseForces.didChange(i)) {
//			switch (mouseForces.getType(i)) {
//				case FT_DENSITY:
//					fluidSimulation.addDensity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
//					break;
//				case FT_VELOCITY:
//					fluidSimulation.addVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
//					particleFlow.addFlowVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
//					break;
//				case FT_TEMPERATURE:
//					fluidSimulation.addTemperature(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
//					break;
//				case FT_PRESSURE:
//					fluidSimulation.addPressure(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
//					break;
//				case FT_OBSTACLE:
//					fluidSimulation.addTempObstacle(mouseForces.getTextureReference(i));
//				default:
//					break;
//			}
//		}
//	}

//--------------------------------------------------------------
//void ofApp::drawComposite(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//
//	ofEnableBlendMode(OF_BLENDMODE_ADD);
//	fluidSimulation.draw(_x, _y, _width, _height);
//
//	ofEnableBlendMode(OF_BLENDMODE_ADD);
//	if (particleFlow.isActive())
//		particleFlow.draw(_x, _y, _width, _height);
//
//	if (showLogo) {
//		flowToolsLogoImage.draw(_x, _y, _width, _height);
//	}
//
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawParticles(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//	if (particleFlow.isActive())
//		particleFlow.draw(_x, _y, _width, _height);
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidFields(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//
//	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//	pressureField.setPressure(fluidSimulation.getPressure());
//	pressureField.draw(_x, _y, _width, _height);
//	velocityTemperatureField.setVelocity(fluidSimulation.getVelocity());
//	velocityTemperatureField.setTemperature(fluidSimulation.getTemperature());
//	velocityTemperatureField.draw(_x, _y, _width, _height);
//	temperatureField.setTemperature(fluidSimulation.getTemperature());
//
//	ofPopStyle();
//}
//--------------------------------------------------------------

//void ofApp::drawFluidObstacle(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//	fluidSimulation.getObstacle().draw(_x, _y, _width, _height);
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawSource(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//	cameraFbo.draw(_x, _y, _width, _height);
//	ofPopStyle();
//}

//======================================================
//======================================================

// within setupGui()

//	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
//	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
//	guiColorSwitch = 1 - guiColorSwitch;
//	gui.add(mouseForces.leftButtonParameters);

//	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
//	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
//	guiColorSwitch = 1 - guiColorSwitch;
//	gui.add(mouseForces.rightButtonParameters);

//	visualizeParameters.setName("visualizers");
//	visualizeParameters.add(showScalar.set("show scalar", true));
//	visualizeParameters.add(showField.set("show field", true));
//	visualizeParameters.add(displayScalarScale.set("scalar scale", 0.15, 0.05, 1.0));
//	displayScalarScale.addListener(this, &ofApp::setDisplayScalarScale);
//	visualizeParameters.add(velocityFieldScale.set("velocity scale", 0.1, 0.0, 0.5));
//	velocityFieldScale.addListener(this, &ofApp::setVelocityFieldScale);
//	visualizeParameters.add(temperatureFieldScale.set("temperature scale", 0.1, 0.0, 0.5));
//	temperatureFieldScale.addListener(this, &ofApp::setTemperatureFieldScale);
//	visualizeParameters.add(pressureFieldScale.set("pressure scale", 0.02, 0.0, 0.5));
//	pressureFieldScale.addListener(this, &ofApp::setPressureFieldScale);
//	visualizeParameters.add(velocityLineSmooth.set("line smooth", false));
//	velocityLineSmooth.addListener(this, &ofApp::setVelocityLineSmooth);
//
//	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
//	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
//	guiColorSwitch = 1 - guiColorSwitch;
//	gui.add(visualizeParameters);

//	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
//	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
//	guiColorSwitch = 1 - guiColorSwitch;
//	gui.add(velocityDots.parameters);

// if the settings file is not present the parameters will not be set during this setup



//--------------------------------------------------------------
//void ofApp::drawFluidDensity(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//
//	fluidSimulation.draw(_x, _y, _width, _height);
//
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidVelocity(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	if (showScalar.get()) {
//		ofClear(0,0);
//		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//		//	ofEnableBlendMode(OF_BLENDMODE_DISABLED); // altenate mode
//		displayScalar.setSource(fluidSimulation.getVelocity());
//		displayScalar.draw(_x, _y, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		velocityField.setVelocity(fluidSimulation.getVelocity());
//		velocityField.draw(_x, _y, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidPressure(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofClear(128);
//	if (showScalar.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//		displayScalar.setSource(fluidSimulation.getPressure());
//		displayScalar.draw(_x, _y, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//		pressureField.setPressure(fluidSimulation.getPressure());
//		pressureField.draw(_x, _y, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidTemperature(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	if (showScalar.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//		displayScalar.setSource(fluidSimulation.getTemperature());
//		displayScalar.draw(_x, _y, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		temperatureField.setTemperature(fluidSimulation.getTemperature());
//		temperatureField.draw(_x, _y, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidDivergence(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	if (showScalar.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//		displayScalar.setSource(fluidSimulation.getDivergence());
//		displayScalar.draw(_x, _y, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		temperatureField.setTemperature(fluidSimulation.getDivergence());
//		temperatureField.draw(_x, _y, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidVorticity(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	if (showScalar.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//		displayScalar.setSource(fluidSimulation.getConfinement());
//		displayScalar.draw(_x, _y, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		ofSetColor(255, 255, 255, 255);
//		velocityField.setVelocity(fluidSimulation.getConfinement());
//		velocityField.draw(_x, _y, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawFluidBuoyance(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	if (showScalar.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//		displayScalar.setSource(fluidSimulation.getSmokeBuoyancy());
//		displayScalar.draw(_x, _y, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		velocityField.setVelocity(fluidSimulation.getSmokeBuoyancy());
//		velocityField.draw(_x, _y, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------

//--------------------------------------------------------------
//void ofApp::drawMask(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//	velocityMask.draw(_x, _y, _width, _height);
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawOpticalFlow(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	if (showScalar.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//		displayScalar.setSource(opticalFlow.getOpticalFlowDecay());
//		displayScalar.draw(0, 0, _width, _height);
//	}
//	if (showField.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		velocityField.setVelocity(opticalFlow.getOpticalFlowDecay());
//		velocityField.draw(0, 0, _width, _height);
//	}
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawMouseForces(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofClear(0,0);
//
//	for(int i=0; i<mouseForces.getNumForces(); i++) {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		if (mouseForces.didChange(i)) {
//			ftDrawForceType dfType = mouseForces.getType(i);
//			if (dfType == FT_DENSITY)
//				mouseForces.getTextureReference(i).draw(_x, _y, _width, _height);
//		}
//	}
//
//	for(int i=0; i<mouseForces.getNumForces(); i++) {
//		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//		if (mouseForces.didChange(i)) {
//			switch (mouseForces.getType(i)) {
//				case FT_VELOCITY:
//					velocityField.setVelocity(mouseForces.getTextureReference(i));
//					velocityField.draw(_x, _y, _width, _height);
//					break;
//				case FT_TEMPERATURE:
//					temperatureField.setTemperature(mouseForces.getTextureReference(i));
//					temperatureField.draw(_x, _y, _width, _height);
//					break;
//				case FT_PRESSURE:
//					pressureField.setPressure(mouseForces.getTextureReference(i));
//					pressureField.draw(_x, _y, _width, _height);
//					break;
//				default:
//					break;
//			}
//		}
//	}
//
//	ofPopStyle();
//}

//--------------------------------------------------------------
//void ofApp::drawVelocityDots(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_ADD);
//	velocityDots.setVelocity(fluidSimulation.getVelocity());
//	velocityDots.draw(_x, _y, _width, _height);
//	ofPopStyle();
//}



