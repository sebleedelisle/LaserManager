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
	//static LaserManagerAnaglyphic * instanceAnaglyphic();
	//static LaserManagerAnaglyphic * laserManagerAnaglyphic;
	
	LaserManagerAnaglyphic() : LaserManager() {
		
		// TODO figure out where the camera actually is
		
		convergeAngle = 1.43;//2.5;
		horizontalOffset = 26.3;// 23;
		cameraPosition.set(640,480,-500);
		
	};
	
	void update(){
		
//		addLaserLineEased(ofPoint(640,100,0),ofPoint(640,700,0),ofColor::white);
//		addLaserLineEased(ofPoint(440,100,200),ofPoint(440,700,200),ofColor::white);
//		addLaserLineEased(ofPoint(840,100,-200),ofPoint(840,700,-200),ofColor::white);
		
		
		LaserManager::update();
//		convergeAngle = (ofGetMouseY()-480) * 0.01;
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
	ofPoint cameraPosition;
	
	ofPolyline tmpPolyLeft;
	ofPolyline tmpPolyRight; 
	

};


//LaserManagerAnaglyphic * LaserManagerAnaglyphic :: laserManagerAnaglyphic = NULL;
//
//LaserManagerAnaglyphic * LaserManagerAnaglyphic::instanceAnaglyphic() {
//	if(laserManagerAnaglyphic == NULL) {
//		laserManagerAnaglyphic = new LaserManagerAnaglyphic();
//	}
//	return laserManagerAnaglyphic;
//}

