//
//  LaserManagerAnaglyphic.h
//  SmashingLaser2015_LA
//
//  Created by Seb Lee-Delisle on 26/04/2015.
//
//

#pragma once

#include "LaserManager.h"




class LaserManagerAnaglyphic : public LaserManager {


	public :
	static LaserManagerAnaglyphic * instanceAnaglyphic();
	static LaserManagerAnaglyphic * laserManagerAnaglyphic;
	
	LaserManagerAnaglyphic();
	void update(){
		
//		addLaserLineEased(ofPoint(640,200,0),ofPoint(640,800,0),ofColor::white);
//		addLaserLineEased(ofPoint(440,200,200),ofPoint(440,800,200),ofColor::white);
//		addLaserLineEased(ofPoint(840,200,-200),ofPoint(840,800,-200),ofColor::white);
//		//cout << ofGetMouseX() << endl;

		
		LaserManager::update();
//		convergeAngle = (ofGetMouseY()) * 0.01;
//		horizontalOffset = (float) ofGetMouseX()*0.1f;
//
//		cout << convergeAngle << " " << horizontalOffset << endl;
		
	};

	void addLaserLineEased(const ofPoint& startpoint,const ofPoint& endpoint, ofFloatColor colour);
	
	
	void addLaserPolyline(const ofPolyline& line, ColourSystem* coloursystem = NULL, float intens = 1);
	void addLaserPolyline(const ofPolyline& line, ofColor col, float intens = 1);

	
	void addLaserSVG(ofxSVG & svg, ofPoint pos, ofPoint scale = ofPoint(1,1,1), float rotation = 0, float intens =1 );
	
	void addLaserSVG(ofxSVG & svg, ofPoint pos, ofPoint scale, ofPoint rotation, ofPoint centerOffset = ofPoint(0,0,0), float intens =1 );

	
	ofPoint convertToLeftPoint(ofPoint p);
	ofPoint convertToRightPoint(ofPoint p);
	
	float horizontalOffset;
	float convergeAngle;
	ofPoint cameraPositionLeft;
	ofPoint cameraPositionRight;
	
	ofPolyline tmpPolyLeft;
	ofPolyline tmpPolyRight;
	
	ofColor leftColour;
	ofColor rightColour;
	

};



