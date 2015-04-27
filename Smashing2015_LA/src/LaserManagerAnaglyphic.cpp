//
//  LaserManagerAnaglyphic.cpp
//  SmashingLaser2015_LA
//
//  Created by Seb Lee-Delisle on 27/04/2015.
//
//

#include "LaserManagerAnaglyphic.h"


LaserManagerAnaglyphic * LaserManagerAnaglyphic :: laserManagerAnaglyphic = NULL;

LaserManagerAnaglyphic * LaserManagerAnaglyphic::instanceAnaglyphic() {
	if(laserManagerAnaglyphic == NULL) {
		laserManagerAnaglyphic = new LaserManagerAnaglyphic();
	}
	return laserManagerAnaglyphic;
}

LaserManagerAnaglyphic::LaserManagerAnaglyphic() : LaserManager() {
	
	// TODO figure out where the camera actually is
	
	convergeAngle = 1.11;//2.5; //1.43;//
	horizontalOffset = 15.5;//23; // 26.3;
	cameraPositionLeft.set(640,480,888);
	cameraPositionRight.set(640,480,888);
	leftColour = ofColor(255,0,0);
	rightColour = ofColor(0,140,255);
	
};

void LaserManagerAnaglyphic:: addLaserLineEased(const ofPoint&startpoint, const ofPoint&endpoint, ofFloatColor colour) {
	
	ofPoint p1 = convertToLeftPoint(startpoint);
	ofPoint p2 = convertToLeftPoint(endpoint);
	LaserManager::addLaserLineEased(p1, p2, leftColour);
	p1 = convertToRightPoint(startpoint);
	p2 = convertToRightPoint(endpoint);
	LaserManager::addLaserLineEased(p1, p2, rightColour);
	
	
	
}

void LaserManagerAnaglyphic::addLaserRectEased(const ofPoint&topLeft, const ofPoint&dimensions, ofFloatColor colour){
	
	addLaserLineEased(topLeft, ofPoint(topLeft.x+dimensions.x,topLeft.y, topLeft.z), colour);
	addLaserLineEased(ofPoint(topLeft.x+dimensions.x,topLeft.y, topLeft.z), topLeft+dimensions, colour);
	addLaserLineEased(topLeft+dimensions, ofPoint(topLeft.x,topLeft.y+dimensions.y, topLeft.z), colour);
	addLaserLineEased(ofPoint(topLeft.x,topLeft.y+dimensions.y, topLeft.z), topLeft, colour);
	
}

ofPoint LaserManagerAnaglyphic::convertToLeftPoint(ofPoint p) {
	ofPoint rp = p;
	
	rp.x += horizontalOffset;
	rp.rotate( convergeAngle, cameraPositionLeft, ofPoint(0,1,0) );

	return rp;
};

ofPoint LaserManagerAnaglyphic::convertToRightPoint(ofPoint p){
	ofPoint rp = p;
	rp.x -= horizontalOffset;
	rp.rotate( -convergeAngle, cameraPositionRight, ofPoint(0,1,0) );
	
	return rp;
};



void LaserManagerAnaglyphic::addLaserPolyline(const ofPolyline& line, ColourSystem* coloursystem, float intens){
	
	if((line.getVertices().size()==0)||(line.getPerimeter()<0.1)) return;
	
	
	// convert to 2D
	
	tmpPolyLeft.clear();
	tmpPolyRight.clear();
	
	for(int i = 0; i<line.getVertices().size(); i++) {
		
		tmpPolyLeft.addVertex(gLProject(convertToLeftPoint(line.getVertices()[i])));
		tmpPolyRight.addVertex(gLProject(convertToRightPoint(line.getVertices()[i])));
	}
	
	shapes.push_back(new LaserPolyline(tmpPolyLeft, new ColourSystem(leftColour), intens));
	shapes.push_back(new LaserPolyline(tmpPolyRight, new ColourSystem(rightColour), intens));
	
	
}

void LaserManagerAnaglyphic::addLaserPolyline(const ofPolyline& line, ofColor col, float intens){
	
	addLaserPolyline(line, new ColourSystem(col), intens);
	
}



void LaserManagerAnaglyphic :: addLaserSVG(ofxSVG & svg, ofPoint pos, ofPoint scale, float rotation, float intens ) {
	addLaserSVG(svg, pos, scale, ofPoint(0,0,rotation), ofPoint(0,0), intens);
}


void LaserManagerAnaglyphic :: addLaserSVG(ofxSVG & svg, ofPoint pos, ofPoint scale, ofPoint rotation, ofPoint centreOffset, float intens ) {
	
	ofVec3f centrePoint = ofVec3f(svg.getWidth()/2, svg.getHeight()/2) + centreOffset;
	
	for(int i=0; i<svg.getNumPath(); i++ ) {
		
		vector<ofPolyline>& lines = svg.getPathAt(i).getOutline();
		ofColor col = svg.getPathAt(i).getStrokeColor();
		col*=intens;
		
		for(int j=0; j<lines.size(); j++) {
			ofPolyline line = lines[j];
			
			vector<ofVec3f>& vertices = line.getVertices();
			for(int i = 0; i<vertices.size(); i++) {
				ofVec3f& v = vertices[i];
				v-=centrePoint;
				v.rotate(rotation.x, ofPoint(1,0,0));
				v.rotate(rotation.y, ofPoint(0,1,0));
				v.rotate(rotation.z, ofPoint(0,0,1));
				v*=scale;
				//v.x *= scaleX;
				//v.x+=width/2;
				//v.y+=height/2;
				
				//v.x+=APP_WIDTH/2;
				//v.y +=APP_HEIGHT*0.3;
				v+=pos;
				//line.
				
			}
			line.simplify(0.1);
			//cout << "brightness : " << brightness << endl;
			addLaserPolyline(line,new ColourSystem(col),1);
		}
	}
	
}



//
//
//void LaserManagerAnaglyphic :: addLaserSVG(ofxSVG & svg, ofPoint pos, ofPoint scale, float rotation, float intens ) {
//	addLaserSVG(svg, pos, scale, ofPoint(0,0,rotation), ofPoint(0,0), intens);
//}
//
//void LaserManagerAnaglyphic :: addLaserSVG(ofxSVG & svg, ofPoint pos, ofPoint scale, ofPoint rotation, ofPoint centreOffset, float intens ) {
//	
//	ofVec3f centrePoint = ofVec3f(svg.getWidth()/2, svg.getHeight()/2) + centreOffset;
//	
//	for(int i=0; i<svg.getNumPath(); i++ ) {
//		
//		vector<ofPolyline>& lines = svg.getPathAt(i).getOutline();
//		ofColor col = svg.getPathAt(i).getStrokeColor();
//		col*=intens;
//		
//		for(int j=0; j<lines.size(); j++) {
//			ofPolyline lineLeft = lines[j];
//			ofPolyline lineRight = lines[j];
//			
//			vector<ofVec3f>& verticesLeft = lineLeft.getVertices();
//			vector<ofVec3f>& verticesRight = lineRight.getVertices();
//			
//			for(int i = 0; i<verticesLeft.size(); i++) {
//				ofVec3f v = verticesLeft[i];
//				v-=centrePoint;
//				v.rotate(rotation.x, ofPoint(1,0,0));
//				v.rotate(rotation.y, ofPoint(0,1,0));
//				v.rotate(rotation.z, ofPoint(0,0,1));
//				v*=scale;
//
//				v+=pos;
//				verticesLeft[i] = convertToLeftPoint(v);
//				verticesRight[i] = convertToRightPoint(v);
//				
//				
//			}
//			lineLeft.simplify(0.1);
//			lineRight.simplify(0.1);
//			
//			//cout << "brightness : " << brightness << endl;
//			addLaserPolyline(lineLeft,new ColourSystem(leftColour),1);
//			addLaserPolyline(lineRight,new ColourSystem(rightColour),1);
//
//		}
//	}
//	
//}
