//
//  DomeData.cpp
//  LaserShow
//
//  Created by Seb Lee-Delisle on 11/03/2014.
//
//

#include "DomeData.h"



void draw();



void DomeData :: init() {
	

	params.add(pos.set("pos", ofPoint(640,480), ofPoint(-1280,-960, -4000), ofPoint(2560,1920, 0)));
	params.add(scale.set("scale", 1, 0.1, 4));
	
	params.add(rotation.set("rot", ofPoint(30,0,0), ofPoint(0,-10,-10), ofPoint(50,10,10)));
	params.add(start.set("start", ofPoint(-500,300), ofPoint(-640,-480),ofPoint(640,480)));
	params.add(c1.set("c1", ofPoint(-480, -50), ofPoint(-640,-480),ofPoint(640,480)));
	params.add(c2.set("c2", ofPoint(-350,-220), ofPoint(-640,-480),ofPoint(640,480)));
	params.add(end.set("end", ofPoint(0,-220), ofPoint(-640,-480),ofPoint(640,480)));
	
	params.add(level1.set("level1", ofPoint(-500,0), ofPoint(-640,-480),ofPoint(0,480)));
	params.add(level2.set("level2", ofPoint(-500,100), ofPoint(-640,-480),ofPoint(0,480)));
	params.add(level3.set("level3", ofPoint(-500,200), ofPoint(-640,-480),ofPoint(0,480)));
	params.add(level4.set("level4", ofPoint(-500,300), ofPoint(-640,-480),ofPoint(0,480)));
	
	params.add(scaleXY.set("scaleXY", ofPoint(1,1,1), ofPoint(0.1,0.1,1),ofPoint(3,3,1)));
	params.add(divAngle.set("division angle",12, 0, 30));

	gui.setup("Dome data", "domeData.xml");
	gui.setVisible(true);
	
	gui.add(params);
	gui.load();

	level1Handle.setPoint(&level1, ofPoint(-940,-480));
	level2Handle.setPoint(&level2, ofPoint(-940,-480));
	level3Handle.setPoint(&level3, ofPoint(-940,-480));
	level4Handle.setPoint(&level4, ofPoint(-940,-480));
	startHandle.setPoint(&start, ofPoint(-940,-480));
	startHandle.handleSize *=2; 
	endHandle.setPoint(&end, ofPoint(-940,-480));
	endHandle.handleSize *=2; 
	c1Handle.setPoint(&c1, ofPoint(-940,-480));
	c2Handle.setPoint(&c2, ofPoint(-940,-480));
	
	
}


void DomeData :: draw() {
	editable = gui.getVisible();
	
	if(!editable) return;

	//level1.draw();
	
	
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(940,480);
	
	level1Handle.render();
	level2Handle.render();
	level3Handle.render();
	level4Handle.render();
	startHandle.render();
	c1Handle.render();
	c2Handle.render();
	endHandle.render();
	
	
	ofSetLineWidth(3);
	ofSetColor(255,0,255);
	ofBezier(start, c1, c2, end);
	ofLine(start, c1);
	ofLine(end,c2);
	
	ofSetColor(0,255,255);
	
	ofTranslate(-300,0);
	
	ofPopMatrix();
	ofPushMatrix();
	
	ofTranslate(pos);
	
	ofRotateX(rotation->x);
	ofRotateY(rotation->y);
	ofRotateZ(rotation->z);
	ofScale(scale, scale, scale);

	
	ofSetColor(255, 100);
	ofSetLineWidth(0.5);

	ofPushMatrix();
	ofScale(scaleXY->x, scaleXY->y, scaleXY->z);
	for(float x = level4->x; x <= -level4->x; x+= level4->x / -5.0f) {
		ofLine(x, level4->y, x, end->y);
	}
	for(float y = level4->y; y >= end->y; y+= (end->y - level4->y)/10.0f) {
		
		ofLine(level4->x, y, -level4->x, y);
	}
	
	
	ofPopMatrix();
	
	ofSetColor(0,255,0);
	ofPoint scalarXY = scaleXY.get(); 
	
	for(float angle = 0; angle>= -90; angle-=divAngle) {
			ofPushMatrix();
			ofRotateY( angle);
			ofScale(scaleXY->x, scaleXY->y, scaleXY->z);
			ofSetLineWidth(0.5);

			ofBezier(start, c1, c2, end);
			ofSetLineWidth(3);
			
			ofCircle(level1, 1);
			ofCircle(level2, 1);
			ofCircle(level3, 1);
			ofCircle(level4, 1);

			ofPopMatrix();
			
			ofPushMatrix();
			ofRotateY( -180 - angle);
			ofScale(scaleXY->x, scaleXY->y, scaleXY->z);
			ofSetLineWidth(0.5);
			
			//ofBezier(start, c1, c2, end);
			ofSetLineWidth(3);
			
			ofCircle(level1, 1);
			ofCircle(level2, 1);
			ofCircle(level3, 1);
			ofCircle(level4, 1);
			
			ofPopMatrix();
			
	}

	ofPopStyle();
	ofPopMatrix();
	
	
	//gui.draw();
	
}


void DomeData :: setPolylineToDomeSquare(ofPolyline& poly, int row, int col, float scale){
	
	poly.clear();
	float angle1 = getAngleForCol(col);
	float angle2 = getAngleForCol(col+1);
	
	float unitForRow[] = {-0.2,-0.01,0.2,0.47, 0.7, 1};
	if(row>4) row = 4;
	float startHeight = unitForRow[row];
	float endHeight = unitForRow[row+1];
	
	if(scale!=1.0f) {
		float diff = (angle1-angle2) / 2.0f * (1-scale);
		angle1-=diff;
		angle2+=diff;
		
		diff = (endHeight-startHeight) /2.0f * (1-scale);
		startHeight +=diff;
		endHeight -= diff;
		
	}
	
	
	for(float i = startHeight; i<=endHeight; i+=0.05) {
		ofPoint p = getPointInDome(i, angle1);
		poly.addVertex(p);
		
	}
	
	

	for(float a = angle1; a>=angle2; a-=1) {
		ofPoint p = getPointInDome(endHeight, a);
		poly.addVertex(p);
		
		
	}
	
	for(float i = endHeight; i>=startHeight; i-=0.05) {
		ofPoint p = getPointInDome(i, angle2);
		poly.addVertex(p);
		
	}
	for(float a = angle2; a<angle1; a+=1) {
		ofPoint p = getPointInDome(startHeight, a);
		poly.addVertex(p);
		
		
	}
	
}

float DomeData :: getAngleForCol(int col) {
	
	if(col<=6) return 360 + col*-divAngle;
	else return 180 + (13-col)*divAngle;
	
}

vector<ofPoint> DomeData :: getLevelPoints(){
	
	vector<ofPoint> levelpoints;
	
	for(int i =0; i<5; i++) {
		
		for(float angle = 0; angle>= -90; angle-=divAngle) {
			
			ofPoint p;
			
			if(i ==0 ) p= end;
			else if(i ==1 ) p= level1;
			else if(i ==2 ) p= level2;
			else if(i ==3 ) p= level3;
			else if(i ==4 ) p= level4;
			
			ofPoint q=p;
			
			
			q *= scaleXY;
			
			q.rotate(angle, ofPoint(0,1,0));
			
			q *= scale;
			q.rotate(rotation->z, ofPoint(0,0,1));
			q.rotate(rotation->y, ofPoint(0,1,0));
			q.rotate(rotation->x, ofPoint(1,0,0));
			
			q += pos;
			
			levelpoints.push_back(q);
			
			q = p;
			q *= scaleXY;
			q.rotate(-180 - angle, ofPoint(0,1,0));
			
			q *= scale;
			q.rotate(rotation->z, ofPoint(0,0,1));
			q.rotate(rotation->y, ofPoint(0,1,0));
			q.rotate(rotation->x, ofPoint(1,0,0));
		
			q += pos;
			
			levelpoints.push_back(q);
		}
	}
	return levelpoints; 
	
	
}

void DomeData::mouseDragged(int x, int y){
	if(!editable) return;

	
	
}

void DomeData::mousePressed(int x, int y){
	if(!editable) return;
	
	
}

void DomeData::mouseReleased(int x, int y){
	if(!editable) return;
	
	save();
	
}

ofPoint DomeData::getBezierPoint(float t) {
	return ofBezierPoint(start, c1, c2, end, t); 
	
	
}

void DomeData::save(){
	
	gui.save();
}



void DomeData::load(){
	gui.load();
	
}

