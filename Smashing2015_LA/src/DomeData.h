//
//  DomeData.h
//  LaserShow
//
//  Created by Seb Lee-Delisle on 11/03/2014.
//
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "EditableBezier.h"
#include "DragHandle.h"

class DomeData {

	public :
	
	void init();
	
	void draw();
	
	void save();
	void load();
	
	ofPoint getBezierPoint(float t);
	ofPoint getPointInDome(float curvePos, float angle) {
		
		ofPoint q = getBezierPoint(curvePos);
		
		q.rotate(angle, ofPoint(0,1,0));
		q *= scaleXY;
		q.rotate(rotation->z, ofPoint(0,0,1));
		q.rotate(rotation->y, ofPoint(0,1,0));
		q.rotate(rotation->x, ofPoint(1,0,0));
		q *= scale;
		q += pos;
		
		return q;
		
		
	}
	
	void mouseDragged(int x, int y);
	void mousePressed(int x, int y);
	void mouseReleased(int x, int y);
	
	void setPolylineToDomeSquare(ofPolyline& poly, int row, int col, float scale = 1);
	float  getAngleForCol(int col);

		
	ofParameterGroup params;
	ofXml settings;
	bool editable;

	ofParameter<ofPoint> pos;
	ofParameter<ofPoint> rotation;
	ofParameter<ofPoint> start, end, c1, c2, scaleXY;
	ofParameter<float> scale, divAngle;
	
	ofParameter<ofVec3f> level1, level2, level3, level4;
	
	DragHandle level1Handle, level2Handle, level3Handle, level4Handle,
				startHandle, c1Handle, c2Handle, endHandle;
	
	vector<ofPoint> getLevelPoints();
	
	//EditableBezier level1;
	
	
	ofxPanel gui;
	

};