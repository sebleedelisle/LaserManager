//
//  WordParticle.h
//  SmashingLaser2015
//
//  Created by Seb Lee-Delisle on 17/03/2015.
//
//

#pragma once 

class WordParticle {
	
	public :
	
	void init(string wordtext, ofPoint position, ofPoint velocity = ofPoint(0,0,0)){
		
		text = wordtext;
		pos = position;
		vel = velocity;
		size = 1;
		life = 0;
		lifeExpectancy = 1;
	};
	
	
	void update(float deltaTime) {
		pos+=(vel*deltaTime);
		life+=deltaTime;
		
	}
	
	
	string text;
	ofPoint pos, vel;
	float size;
	float life, lifeExpectancy;
	
	
	
	
	
};
