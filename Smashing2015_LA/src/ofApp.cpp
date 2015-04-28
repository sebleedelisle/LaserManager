#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	ofSetFrameRate(60);

	screenWidth = 1280;
	screenHeight = 960;
	
	guideImage.loadImage("img/GuideImageLA.jpg");
	music.loadSound("../../../Music/RobotsEdit2.aif");
	video.loadMovie("../../../Music/Main Timeline Stereo 3D_6.mov");
	video.setVolume(0);
	video.play();
	video.update();
	video.stop();
	
	svgs.push_back(ofxSVG());
    svgs.back().load("SmashingCat.svg");
    
	svgs.push_back(ofxSVG());
	svgs.back().load("NyanCat.svg");
	
	svgs.push_back(ofxSVG());
	svgs.back().load("california.svg");
	
	worldMap.load("world map new file.svg");
	laMap.load("california.svg");
	
	previewProjector = false;
	
	projectorFbo.allocate(1280, 720, GL_RGB, 4);
	uiFbo.allocate(screenWidth, screenHeight, GL_RGB, 2); 

	projectorFbo.begin();
	ofSetColor(0);
	ofRect(0,0,1280,720);
	projectorFbo.end();
	
	ofSetColor(255);
	//projectorPosition.set(screenWidth/5*2.14, screenHeight*4/5 * 0.99, screenWidth/5, screenHeight/5);
	projectorPosition.set(460, 327, 370, 213);
	
	pipeOrganData.init(projectorPosition); 
	
	laserManager.setup(screenWidth, screenHeight);
	
	laserManager.renderLaserPreview = true;
	laserManager.showPostTransformPreview = true;

	int panelwidth = 200;
	
	ofxBaseGui::setDefaultWidth(panelwidth);
    ofxBaseGui::setDefaultHeight(16);
	ofxBaseGui::setDefaultSpacing(3);
    ofxBaseGui::setDefaultElementSpacing(3);
	ofxBaseGui::setDefaultElementIndentation(1);
	ofxBaseGui::setDefaultTextPadding(5);
	ofxBaseGui::setUseTTF(true);
	
	ofxBaseGui::loadFont("VeraMoIt.ttf",8, true);
	
	laserGui.setup("LaserManager", "laserSettings.xml");
	laserGui.setPosition(ofPoint(screenWidth+220 - panelwidth - 10,10));
	laserGui.setVisible(true);
	laserGui.add(&laserManager.connectButton);
	laserGui.add(laserManager.parameters);
	
	laserGui.load();
	panels.push_back(&laserGui);
	
	appGui.setup("App");
	appGui.setPosition(ofPoint(screenWidth+220 - panelwidth - 10,10));
	appGui.setVisible(false);
	appGui.add(pipeOrganData.editable.set("Pipe Organ editable", false));
	appGui.add(laserDomePoints.set("laser dome points", false));
	appGui.add(laserOrganPoints.set("laser organ points", false));
	appGui.add(showGuideImage.set("show guide image", true));
	appGui.add(showBands.set("show fft bands", false));
	appGui.add(clappyBird.sensitivity.set("flappy sensitivity", 1,0,10)); 
	
	redGui.setup("Laser Red", "laserred.xml");
	redGui.add(laserManager.redParams );
    greenGui.setup("Laser Green", "lasergreen.xml");
	greenGui.add(laserManager.greenParams );
    blueGui.setup("Laser Blue", "laserblue.xml");
	blueGui.add(laserManager.blueParams );
    redGui.load();
	greenGui.load();
	blueGui.load();
	redGui.setPosition(ofPoint(10 ,10));
	greenGui.setPosition(ofPoint(10 +panelwidth + 10,10));
	blueGui.setPosition(ofPoint(10 +panelwidth*2 + 20,10));
	
	
	//appGui.load();
	panels.push_back(&appGui);

	
	clappyBirdActive = false;
	
	soundPositionMS = 0;
	
	sync.tempo = 128.05;
	sync.startPosition = -80;//(60000/111) - 5; // start after 1 beat intro
	
	//pipeOrganData.load();
	domeData.init();
	domeData.load();
	
	domeData.gui.setPosition(ofPoint(screenWidth+220 - panelwidth - 10,10));
	domeData.gui.setVisible(false);
	
	panels.push_back(&domeData.gui);
	
	effectDomeLines.setDomeData(&domeData);
	effectPipeOrganLines.setObjects(&pipeOrganData, &particleSystemManager);
	effectParticles.setObjects(&pipeOrganData, &domeData);
	
	smoothVol = 0;
	
	calculateScreenSizes();
	smoothedInputVolume = 0;
	soundStream.setup(this, 0, 2, 48000, 1024, 1);
	left.resize(1024);
	right.resize(1024);
    
    svgCounter = 0;
	

	proximaNovaLB.loadFont("Proxima Nova Light", 48, true, true);
	
	wordParticles = deque<WordParticle>();
	
	
}


//--------------------------------------------------------------
void ofApp::update(){
	
	//cam.truck(1);
	float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
	
	ofSoundUpdate();
	laserManager.update();
	if(music.getIsPlaying()) {
		soundPositionMS = music.getPositionMS();
		if(sync.barTriggered) setVideoPositionMS(soundPositionMS);
	}
	if(soundPositionMS > (video.getDuration()*1000)) video.stop();
	else if(music.getIsPlaying()) video.play();
	
	video.update();
	
	sync.update(soundPositionMS);
	
	screenAnimation.update();
	
	particleSystemManager.update(deltaTime);
	effectLaserBeams.update(deltaTime);
	effectDomeLines.update(deltaTime);
	effectPipeOrganLines.update(deltaTime);
	
	// wordParticles
	
	//cout << wordParticles.size() << endl;
	for(int i = 0; i<wordParticles.size(); i++) {
		
		WordParticle& wp = wordParticles[i];
		wp.update(deltaTime);
		
	}
	
	while((wordParticles.size()>0) && (wordParticles.front().life > wordParticles.front().lifeExpectancy)) {
		wordParticles.erase(wordParticles.begin(), wordParticles.begin()+1);
		
	}

	
	if(clappyBirdActive) clappyBird.update(smoothedInputVolume, deltaTime);
	else clappyBird.reset();


}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);

	
	int numBands = 500;
	vol = 0;
	
	
	float * val = ofSoundGetSpectrum(numBands);
	
	for(int i = 0;i<numBands; i++) {
		vol+=val[i];
		
	}

	vol/=(float)numBands / 20.0f;
	//cout << vol << endl;
	updatePeakFrequency(val, numBands);
	//ofSetupScreenPerspective(1280, 960,ofGetMouseY());
	
	smoothVol += (vol-smoothVol) *0.5;
	volumes.push_front(smoothVol);
	if(volumes.size()>500) volumes.pop_back();
	
	
	uiFbo.begin();
	ofSetupScreenPerspective(screenWidth,screenHeight,50);

	ofSetColor(0);
	ofFill(); 
	ofRect(0,0,screenWidth, screenHeight);
	ofNoFill();
	ofSetColor(255);
	
	if(!previewProjector) {
		ofSetColor(200);
		if(showGuideImage) guideImage.draw(0,0,screenWidth, screenHeight);
		ofSetColor(255);
		ofRect(projectorPosition.x-1, projectorPosition.y-1, projectorPosition.width+2, projectorPosition.height+2);
		projectorFbo.draw(projectorPosition);
		
		pipeOrganData.draw();
		
		if((pipeOrganData.editable) && (laserOrganPoints)){
			vector<Pipe>& pipes = pipeOrganData.pipes;
			for(int i = 0; i< pipes.size(); i++) {
				Pipe& pipe = pipes[i];
				if((pipe.topDragging) || (pipe.bottomDragging)) {
					laserManager.addLaserLineEased(pipe.top, pipe.bottom, ofColor::white);
				} else {
					laserManager.addLaserLineEased(pipe.top, pipe.bottom, ofColor::cyan);
				} 
			}
		}
		ofSetupScreenPerspective(screenWidth,screenHeight,50);

		domeData.draw();
		
		
		if((domeData.editable) && (laserDomePoints)){
			
			vector<ofPoint> points = domeData.getLevelPoints();
			
			for(int i = 0; i<points.size(); i++) {
				laserManager.addLaserDot(points[i], ofColor :: white);
		
				
			}
			
		}

	} else {
		ofSetColor(50);
		ofRect(screenWidth/2 - projectorFbo.getWidth()/2 -1, screenHeight/2 - projectorFbo.getHeight()/2-1, projectorFbo.getWidth()+2, projectorFbo.getHeight()+2);
		ofSetColor(255);
		projectorFbo.draw(screenWidth/2 - projectorFbo.getWidth()/2, screenHeight/2 - projectorFbo.getHeight()/2);
		
	}

	ofDrawBitmapString(ofToString(round(ofGetFrameRate())), 0,10);
	
	if(showBands) {
		numBands = 100;
		int barWidth = (1024/numBands);
		ofFill();
		for(int i = 0; i<numBands; i++) {
			
			float size = val[i] * 100;
			//ofRect(i*barWidth, 768 - size, barWidth-1, size );
			ofRect(i*barWidth, 0, barWidth-1, size );
			
		}
		
		ofSetColor(255,0,0);

		ofRect((numBands+1)*barWidth, 0, barWidth-1, vol*100 );
		ofSetColor(255);
		ofRect((numBands+2)*barWidth, 0, barWidth-1, smoothedInputVolume*1000 );
		ofSetColor(255);
	}
	
	
	ofNoFill();
	
	float time = soundPositionMS/1000.0f;

	ofDrawBitmapString(ofToString(time), 0,25);
	
	uiFbo.end();

	//----------------- FBO BEGIN --------------------------------
	projectorFbo.begin();
	ofPushStyle();

	ofFill();
	ofSetColor(0);
	ofRect(0,0,1280,720);
	ofSetColor(255);
	
	video.draw(0,0,1280,720);
	
	
	

	
	
	ofPushMatrix();
	ofTranslate(640,360);
	
	// draw wordparticles
	for(int i = 0; i<wordParticles.size(); i++) {
		
		WordParticle& wp = wordParticles[i];
		ofPushMatrix();
		ofTranslate(wp.pos);
		ofSetColor(255, 255, 255,ofMap(wp.life, 0, wp.lifeExpectancy, 255, 0));
		
		ofFill();
		
		//ofCircle(wp.pos, 10); //, <#float y#>, <#float radius#>)
		writeMessage(wp.text);
		
		
		
		ofPopMatrix();
		
		
	}
	
	
	screenAnimation.draw(sync, vol);
	
	ofPopMatrix(); 
	
	ofPopStyle();
	projectorFbo.end();

	//----------------- FBO END --------------------------------

		
	ofPopMatrix();
	
	uiFbo.begin();
	ofSetupScreenPerspective(screenWidth,screenHeight,50);


	
	// EFFECTS ---------------------------------------------
	
	drawEffects();
    
    //if(ofGetElapsedTimef() / 0.8f > svgCounter) {
    //    svgCounter++;
        
        if((currentSVG>=0) && (currentSVG<svgs.size())) {
            laserManager.addLaserSVG(svgs[currentSVG], ofPoint(690,580 + sin(ofGetElapsedTimef()*3) * 50),ofPoint(0.5,0.5));
        
            //laserManager.addLaserSVG(svgs[currentSVG], ofPoint(390,580),ofPoint(0.5,0.5));
		
        }
	
    //}
    if(clappyBirdActive) clappyBird.draw(laserManager);
	
	
	laserManager.draw();
	laserManager.renderLaserPreview = !previewProjector; 
	if(!previewProjector) {
		laserManager.renderLaserPath(ofRectangle(0,0,screenWidth, screenHeight), false);
		//laserManager.renderPreview();
	}
	
	ofDrawBitmapString(sync.getString(), 1000,10);
	sync.draw(1100,10);
	
	uiFbo.end();
	uiFbo.draw(0,0);
	
	projectorFbo.draw(secondScreenRect); 
	
	// GUI PANELS
	for(int i = 0; i<panels.size(); i++) {
		panels[i]->draw();
	}
	redGui.draw();
	greenGui.draw();
	blueGui.draw();
	

}

void ofApp :: drawEffects() {
	
	resetEffects();
	//showWooeeyShapes();
	//showWaveform();
	
	//domeData.setPolylineToDomeSquare(poly,  (float) ofGetMouseY()/(float)ofGetHeight() * 5, (float) ofGetMouseX()/(float)ofGetWidth() * 12, sync.eighthPulse);
	//laserManager.addLaserPolyline(poly);
	
	
	//showBlippySquares();
	
	// BIG SQUARES AND THEN WORLD MAP

	if((sync.currentBar>=24) && (sync.currentBar<28)) {
		// squares
		resetEffects();
		
		if(sync.currentBar==24) {
			
			float totalshapes = 10;
			float progress = ofMap(sync.barPulse, 1,0.875, 0, 1, true);
			
			float endshapes = floor(progress * totalshapes);
			float startshapes = floor(ofMap(sync.barPulse, 0.75,0.5, 0,totalshapes, true));
			
			ofPoint centre(675,840);
			//		ofPoint topleft = centre + ofPoint(-140,-140);
			//		ofPoint botright = centre + ofPoint(140,140);
			//
			//
			//		float colourOffset = ofMap(sync.barPulse, 1, 0, 0, 100);
			//		ofColor c;
			//		for(int i = startshapes; i<endshapes; i++) {
			//			c.setHsb((int)(colourOffset + 200-i*12)%255, 255, 255);
			//			//ofSetColor(c);
			//
			//			ofPoint offset(0,i*-40);
			//			lm.addLaserLineEased( left + offset, centre+offset, c);
			//			lm.addLaserLineEased(centre + offset, right + offset, c);
			//
			//		}
			
			
			for(int i = startshapes; i<endshapes; i++) {
//				float scale = (float) i / (float)totalshapes;
//				scale*=scale;
//				scale*=1.5;
//				laserManager.addLaserRectEased(ofPoint(640,480) + (ofPoint(-400,-380)*scale), ofPoint(800,600,0)*scale, ofColor::white);
				
				float z = ofMap(i, 0, 10, -500, 500);
				laserManager.addLaserRectEased(ofPoint(640,480, z) + (ofPoint(-400,-100)*1), ofPoint(800,200)*1, ofColor::white);
				
			}

			
			
			
		}

		
		// world map coming forward with radar lines
		
		if(sync.currentBarFloat>24.25) {
			
			// z zoom forward at start
			float zpos = ofMap(sync.currentBarFloat, 24.25,24.75,-10000,-200,true);// 0;//ofMap(sync.currentBarFloat, 25,27.75,200,600);//1200);
			

			
			// scaling up from zero to start
			float scale = 1;// ofMap(sync.currentBarFloat, 24.25,24.75,0,0.9,true);
			
			// xangle increased over time to tilt the world map
			float xangle = ofMap(sync.currentBarFloat, 25,28,-60,-65, true);
			
			// transition in - xpos and ypos set at end
			float ypos = 400; // ofMap(sync.currentBarFloat, 24.25,24.75,450,450,true);
			// slow increase in x
			//float xpos = ofMap(sync.currentBarFloat, 24.25,24.75,645,645,true);
			float xpos = 645;//
			
			
			// slow increase
			if( sync.currentBarFloat>24.75) {
				xpos = ofMap(sync.currentBarFloat,24.75,28,645,745,true);
				ypos = ofMap(sync.currentBarFloat,24.75,28,400,450,true);
				
				zpos = ofMap(sync.currentBarFloat,24.75,28,-200,400,true);
				
			}

			
			
			float brightness = 1;
			// make the map fly forward at the end
			if(sync.currentBarFloat>27.75) {
				zpos = ofMap(sync.currentBarFloat,27.75,28, 400,1000);
				
				brightness = ofMap(sync.currentBarFloat,27.75,28, 1,0);
				
			}
			
			laserManager.addLaserSVG(worldMap, ofPoint(xpos,ypos, zpos),ofPoint(scale,scale),ofPoint(xangle,0,0), ofPoint(10,-47), brightness );
		
			
			
			// RADAR LINE
			
			ofPoint radarOffset(-145,10,0);
			
			if(sync.currentBar>=26) {
				
				float rotation = sync.barPulse/2;
				if(sync.currentBar%2==1) rotation+=0.5;
				rotation*=PI*2;
				
				ofPoint radarEndPoint = ofPoint(sin(rotation)*120, cos(rotation)*80);
				//ofPoint radarEndPoint2 = ofPoint(sin(rotation)*120, cos(rotation)*80);
				//radarEndPoint2.rotate(3,ofPoint(0,0,1));
				
				radarEndPoint.rotate(-xangle, ofPoint(1,0,0));
				//radarEndPoint2.rotate(-xangle, ofPoint(1,0,0));
				
				laserManager.addLaserLineEased(ofPoint(xpos,ypos,zpos)+ radarOffset, ofPoint(xpos, ypos,zpos) + radarEndPoint+ radarOffset, ofColor(0,100,0));
				
				//laserManager.addLaserLineEased(ofPoint(xpos,ypos,zpos), ofPoint(xpos, ypos,zpos) + radarEndPoint2, ofColor::green);
				
			}
		}

	}
	
	// UK MAP SECTION
	
	if((sync.currentBarFloat>=27.75) && (sync.currentBar<32)) {
		// scaling up from zero to start
		float scale = 1;//ofMap(sync.currentBarFloat, 24.5,24.75,0,0.5,true);
		float zpos = ofMap(sync.currentBarFloat, 27.75, 32,100,700);//1200);
		
		float xpos = 645;
		float ypos = 460;
		
		
		// xangle increased over time to tilt the world map
		float xangle = -60; // ofMap(sync.currentBarFloat, 25,28,-40,-60, true);
		
		float brightness = 1;
		// make the map fly forward at the beginning
		if((sync.currentBarFloat>27.75) && (sync.currentBarFloat<=28)) {
			zpos = ofMap(sync.currentBarFloat,27.75,28, -12000,100);
			brightness = ofMap(sync.currentBarFloat,27.75,28, 0,1);
			
		}
		
		laserManager.addLaserSVG(laMap, ofPoint(xpos,ypos, zpos),ofPoint(scale, scale),ofPoint(xangle,0,0), ofPoint(14,45), brightness );
		
		ofPoint radarOffset(0,10,0);
		
		
		float rotation = sync.barPulse/2;
		if(sync.currentBar%2==1) rotation+=0.5;
		rotation*=PI*2;
		
		ofPoint radarEndPoint = ofPoint(sin(rotation)*100, cos(rotation)*100);
		//ofPoint radarEndPoint2 = ofPoint(sin(rotation)*180, cos(rotation)*180);
		//radarEndPoint2.rotate(3,ofPoint(0,0,1));
		
		radarEndPoint.rotate(-xangle, ofPoint(1,0,0));
		//radarEndPoint2.rotate(-xangle, ofPoint(1,0,0));
		
		laserManager.addLaserLineEased(ofPoint(xpos,ypos,zpos)+radarOffset, ofPoint(xpos, ypos,zpos) + radarEndPoint + radarOffset, ofColor(0,100,0));
//		
//		laserManager.addLaserLineEased(ofPoint(xpos,ypos,zpos), ofPoint(xpos, ypos,zpos) + radarEndPoint2, ofColor::green);
//		
		
	

	
	}



	if((sync.currentBarFloat >= 31.875) && (sync.currentBarFloat < 35.5)) {
		resetEffects();
		
		if((int)(sync.currentBarFloat+0.125)%2==0) effectPipeOrganLines.setMode(2);
		
		
	}
	
	if((sync.currentBar >= 36) && (sync.currentBar < 44)) {
		resetEffects();
		
		if(sync.currentBar%2==0) effectPipeOrganLines.setMode(2);
		else  showWooeeyShapes((sync.currentBar>=40)?-0.15:0);
		
	}
	
	if((sync.currentBar >= 44) && (sync.currentBar < 45)) {
		resetEffects();
		effectDomeLines.setMode(1);
		
	}

	
	if((sync.currentBar >= 46) && (sync.currentBar < 49)) {
		resetEffects();
		
		//if(sync.currentBar%2==0)
			effectPipeOrganLines.setMode(2);
		
	}
	if((sync.currentBar >= 45) && (sync.currentBar < 47)) {
		effectLaserBeams.mode = 3;
	}
	
	if((sync.currentBar>=49) && (sync.currentBar<61)) {
		showBlippySquares();
		if((sync.currentBar>=53) && (sync.currentBar<57) ) showWooeeyShapes(0,true);
		
	}
	
	if((sync.currentBar >=57) && (sync.currentBar< 61) ) {
		
		showWaveform();
		
		
	}
	/*
	if((sync.currentBar>=28) && (sync.currentBar<30)) {
		
		
		
		drawSpirograph(ofPoint(350,600), 20,50,80,ofMap(sync.currentBarFloat, 29, 30,0,1, true),ofMap(sync.currentBarFloat, 28, 29,0,1, true),
					   ofMap(sync.currentBarFloat, 28, 30, 180,0));
		
	}
	if((sync.currentBar>=30) && (sync.currentBar<32)) {
		resetEffects();
		
		drawSpirograph(ofPoint(1000,600), 20,80,60,ofMap(sync.currentBarFloat, 31, 32,0,1, true),ofMap(sync.currentBarFloat, 30, 31,0,1, true),
					   ofMap(sync.currentBarFloat, 28, 30, 180,0));
		
	}
	// SOLO
	if((sync.currentBar >= 32) && (sync.currentBar < 40)) {
		resetEffects();
		effectPipeOrganLines.setMode(2);
	
	}
	if((sync.currentBar >= 35) && (sync.currentBar < 36)) {
		resetEffects();
		if(sync.barTriggered) effectParticles.makeStarBurst();
	}
	if((sync.currentBar >= 40) && (sync.currentBar < 41)) {
		resetEffects();
	}
	if((sync.currentBarFloat > 41.5) && (sync.currentBar < 42)) {
		resetEffects();
		effectDomeLines.setMode(3);
	}// POST SOLO BUILD (DOME RINGS)
	if((sync.currentBar >= 42) && (sync.currentBar < 46)) {
		resetEffects();
		effectDomeLines.setMode(1);
	}
	// POST SOLO BUILD (DOME LINES)
	if((sync.currentBar >= 46) && (sync.currentBar < 50)) {
		resetEffects();
		effectDomeLines.setMode(2);
	}
	// MELLOW DROP OUT - WHITE LASER BEAMS
	if((sync.currentBar >= 50) && (sync.currentBar < 54)) {
		resetEffects();
		effectLaserBeams.mode = 1;
		
	}
	// COLOURED LASER BEAMS
	if((sync.currentBar >= 54) && (sync.currentBar < 56)) {
		resetEffects();
		effectLaserBeams.mode = 2;
		
	}
	// DROP OUT BEFORE OUTRO
	if((sync.currentBar >= 57) && (sync.currentBar < 59)) {
		resetEffects();
		effectDomeLines.setMode(3);
	}
	
	if((sync.currentBar>=59) && (sync.currentBar<74)) {
		if ((sync.barTriggered) && ((sync.currentBar==59)||(sync.currentBar == 63))) effectParticles.makeStarBurst();
			
		effectPipeOrganLines.setMode(3);
		effectLaserBeams.mode = 0;
		if((sync.currentBar>=67) && (sync.currentBar<73))  effectDomeLines.setMode(5);
		else effectDomeLines.setMode(0);
		if(sync.currentBar>=71) effectDomeLines.doSecondCircleRow = true;
	}
	//effectDomeLines.setMode(3;
	if((sync.currentBar >= 74) && (sync.currentBar < 75)) {
		resetEffects();
		effectDomeLines.setMode(3);
		
	}
	if((sync.currentBar == 74) && (sync.currentBeat == 3) && (sync.eighthTriggered)){

		effectParticles.makeStarBurst(0.5);
		effectParticles.makeStarBurst(0.3);
	}
	
	if(sync.currentBar > 75) {
		
		resetEffects();
		
		ofColor col;
		col.setHsb((ofGetElapsedTimeMillis()/4)%255, 255, 255);
		laserManager.addLaserText(" THANKS!", ofPoint(640,480), 10,col, true);
		
	}
	
	 
	 */
	
	
	if((sync.currentBar>=61) && (sync.currentBarFloat<89.1)) {
		
		float rotationDistance = 66;
		effectPipeOrganLines.lightSide(true, ofColor(0,200,200));
		effectPipeOrganLines.lightSide(false, ofColor(0,200,200));
		
		
		// 61 - 62 - get ready
		// 63 - 64 - clap when circle hits pipe
		// 65 - 66 try it now
		
		vector<float> clapsLeft;
		clapsLeft.push_back(65.75);
		//clapsLeft.push_back(67);
		clapsLeft.push_back(67.75);
		
//		clapsLeft.push_back(69);
		clapsLeft.push_back(69.75);
		clapsLeft.push_back(70.75);
		clapsLeft.push_back(71.75);

		clapsLeft.push_back(73.25);
		clapsLeft.push_back(73.75);
		clapsLeft.push_back(74.25);
		clapsLeft.push_back(74.75);
		clapsLeft.push_back(75.25);
		clapsLeft.push_back(75.75);
		
		vector<float>clapsRight = clapsLeft;
		
		
		clapsLeft.push_back(77.25);
		clapsLeft.push_back(77.75);
		clapsLeft.push_back(78.25);
		clapsLeft.push_back(78.75);

		clapsRight.push_back(79.25);
		clapsRight.push_back(79.75);
		clapsRight.push_back(80.25);
		clapsRight.push_back(80.75);
		
		clapsLeft.push_back(81.25);
		clapsRight.push_back(81.75);
		clapsLeft.push_back(82.25);
		clapsRight.push_back(82.75);
		clapsLeft.push_back(83.25);
		clapsRight.push_back(83.75);
		clapsLeft.push_back(84.25);
		clapsRight.push_back(84.75);
		
		clapsLeft.push_back(85.0);
		clapsLeft.push_back(85.25);
		clapsRight.push_back(85.5);
		clapsRight.push_back(85.75);
		
		clapsLeft.push_back(86.0);
		clapsLeft.push_back(86.25);
		

		clapsRight.push_back(86.5);
		clapsRight.push_back(86.75);
		
		clapsLeft.push_back(87.0);
		clapsLeft.push_back(87.125);
		clapsRight.push_back(87.25);
		clapsRight.push_back(87.375);
		clapsLeft.push_back(87.5);
		clapsLeft.push_back(87.625);
		clapsRight.push_back(87.75);
		clapsRight.push_back(87.875);
		
		clapsLeft.push_back(88.0);
		clapsLeft.push_back(88.125);
		clapsLeft.push_back(88.25);
		clapsLeft.push_back(88.375);
		clapsLeft.push_back(88.5);
		clapsLeft.push_back(88.625);
		clapsLeft.push_back(88.75);
		clapsLeft.push_back(88.875);
		clapsLeft.push_back(89);
		
		clapsRight.push_back(88.0);
		clapsRight.push_back(88.125);
		clapsRight.push_back(88.25);
		clapsRight.push_back(88.375);
		clapsRight.push_back(88.5);
		clapsRight.push_back(88.625);
		clapsRight.push_back(88.75);
		clapsRight.push_back(88.875);
		clapsRight.push_back(89);
		
		
		float latency = 0.05;
		
		for (int i = 0; i<clapsLeft.size(); i++) {
			float clap = clapsLeft[i];
			
			if((sync.eighthTriggered) && (sync.currentBar + ((float)sync.current8th / 8.0f) == clap)) {
		
				effectPipeOrganLines.pulseSide(true, ofColor::cyan);
			}
			
			clap-=latency;
			
			if((sync.currentBarFloat+1 > clap) && (sync.currentBarFloat<clap)) {
				
				float t = ofMap(clap, sync.currentBarFloat+1, sync.currentBarFloat, 0, 1);
				ofPoint pos = domeData.getPointInDome(0.06, t*-rotationDistance);
				
				
				laserManager.addLaserCircle(pos, ofColor::cyan, 20);
				
			}
			
		}
		
		for (int i = 0; i<clapsRight.size(); i++) {
			float clap = clapsRight[i];
			
			if((sync.eighthTriggered) && (sync.currentBar + ((float)sync.current8th / 8.0f) == clap)) {
				
				effectPipeOrganLines.pulseSide(false, ofColor::cyan);
			}
			
			clap-=latency;
			
			if((sync.currentBarFloat+1 > clap) && (sync.currentBarFloat<clap)) {
				
				float t = ofMap(clap, sync.currentBarFloat+1, sync.currentBarFloat, 0, 1);
				ofPoint pos = domeData.getPointInDome(0.06, 180 + (t*rotationDistance));
				
				
				laserManager.addLaserCircle(pos, ofColor::cyan, 20);
				
			}
			
		}
		
		//check bar has elapsed
		
		
		
		
	}
	
	if((sync.currentBar>=89)&&(sync.currentBar<90)) {
		showWaveform(0.1,0);
	}
	
	particleSystemManager.draw();
	effectLaserBeams.draw(laserManager,smoothVol);
	effectDomeLines.draw(sync, smoothVol, laserManager);
	effectPipeOrganLines.draw(sync, smoothVol, laserManager, currentPeakFrequency);

	updateWooeeyShapes();
	
	
}


void ofApp::showWaveform(float vpos, float threshold) {
	poly.clear();
	
	float resolution = 0.5;
	float speed = 20; // lower is faster
	float numiterations = 90.0f/speed;
	float size = 0.2; // this is the multiplier for the sin function from -1 to 1
	
	for(int i = 0; i<speed && i< volumes.size() ; i++) {
		
		// this ensures that the wave shrinks as it goes across
		
		float scale = ofMap(i, speed*0, speed, 1, 0, true);
		
		float volume = ofMap(volumes[i], threshold, 1, 0, 1, true) * scale;
		for(float j= 0; j<numiterations; j+=resolution) {
			
			
			//poly.addVertex( ofPoint(centre.x - (i*20 + j), centre.y + 100- (sin(j/20.0f*PI*2)* volume )));
			
			
			poly.addVertex(domeData.getPointInDome((sin(j/numiterations*PI*2)*size*volume) +vpos,  -90-(i*(numiterations) + (j))));
		}
	}
	
	
	
	laserManager.addLaserPolyline(poly);
	
	poly.clear();
	
	for(int i = 0; i<speed && i< volumes.size() ; i++) {
		
		// this ensures that the wave shrinks as it goes across
		
		float scale = ofMap(i, speed*0, speed, 1, 0, true);
		
		float volume = ofMap(volumes[i], threshold, 1, 0, 1, true)*scale;
		for(float j= 0; j<numiterations; j+=resolution) {
			
			
			//poly.addVertex( ofPoint(centre.x - (i*20 + j), centre.y + 100- (sin(j/20.0f*PI*2)* volume )));
			
			poly.addVertex(domeData.getPointInDome((sin(j/numiterations*PI*2)*size*-volume) +vpos,  -90+(i*(numiterations) + (j))));
		}
	}
	laserManager.addLaserPolyline(poly);
	
}

void ofApp::showWooeeyShapes(float voffset, bool oddBarNumber) {
	
	int notes[] = {1,0,0,1,3,1,0,0,3,0,4,8,6,6,0,0};
	//bool oddBarNumber = false;
	
	int beatnum = sync.current8th;
	beatnum += ((sync.currentBar+(oddBarNumber?1:0))%2) *8;
	// beatnum should now be 1 - 16 across a 2 bar pattern.
	
	int note = notes[beatnum];
	
	if(note>0) {
		note-=1;
		
		float targetPosition =((float)note/12.0f)+voffset;
		
		if(currentWooeyPosition == -1) currentWooeyPosition = targetPosition;
		currentWooeyPosition += (targetPosition-currentWooeyPosition)*0.3;
		
		ofPoint p = domeData.getPointInDome(currentWooeyPosition, -120);
		
		laserManager.addLaserCircle(p, ofColor::white, 20 );
		
		p = domeData.getPointInDome(currentWooeyPosition, -60);
		
		laserManager.addLaserCircle(p, ofColor::white, 20 );
		//domeData.setPolylineToDomeSquare(poly,  rownum, 6+note, sync.sixteenthTriggered? 1: sync.sixteenthPulse);
		
		//domeData.setPolylineToDomeSquare(poly,  rownum, 6-note, sync.sixteenthTriggered? 1: sync.sixteenthPulse);
		
		
	} else {
		currentWooeyPosition = -1;
		
	}
	
	
	
}

void ofApp::updateWooeeyShapes() {
	
	
	wooeyPositions.push_front(currentWooeyPosition);
	if(wooeyPositions.size()> 50) wooeyPositions.pop_back();

	
	poly.clear();
	
	for(int i = 0; i<wooeyPositions.size(); i++) {
		float pos = wooeyPositions[i];
		
		if(pos <0) {
			
			if(poly.getVertices().size()>0) {
				laserManager.addLaserPolyline(poly, ofColor::cyan);
				poly.clear();
			}
		} else {
			poly.addVertex(domeData.getPointInDome(pos, -120 - i));
		}
		
	}
	if(poly.getVertices().size()>0) {
		laserManager.addLaserPolyline(poly, ofColor::cyan);
		poly.clear();
	}
	
	
	poly.clear();
	
	for(int i = 0; i<wooeyPositions.size(); i++) {
		float pos = wooeyPositions[i];
		
		if(pos <0) {
			
			if(poly.getVertices().size()>0) {
				laserManager.addLaserPolyline(poly, ofColor::cyan);
				poly.clear();
			}
		} else {
			poly.addVertex(domeData.getPointInDome(pos, i-60));
		}
		
	}
	if(poly.getVertices().size()>0) {
		laserManager.addLaserPolyline(poly, ofColor::cyan);
		poly.clear();
	}

	
}

void ofApp::showBlippySquares(int rownum,  bool oddBarNumber) {
	
	int notes[] = {2, 1, 2, 1, 2,0,4,0,2,1,2,1,2,2,2,0};
	
	int beatnum = sync.current8th;
	beatnum += ((sync.currentBar+(oddBarNumber?1:0))%2) *8;
	// beatnum should now be 1 - 16 across a 2 bar pattern.
	
	int note = notes[beatnum];
	
	if(note>0) {
		
		//if()
		domeData.setPolylineToDomeSquare(poly,  rownum, 6+note, sync.sixteenthTriggered? 1: sync.sixteenthPulse);
		laserManager.addLaserPolyline(poly);
		domeData.setPolylineToDomeSquare(poly,  rownum, 6-note, sync.sixteenthTriggered? 1: sync.sixteenthPulse);
		laserManager.addLaserPolyline(poly);
	}
	
	
}

void ofApp::resetEffects() {
	
	effectPipeOrganLines.setMode(0);
	effectLaserBeams.mode = 0;
	effectDomeLines.setMode(0);
	
}



void ofApp ::  drawSpirograph(ofPoint pos, int numrevolutions, float smallradius, float largeradius, float start, float end, float rotation) {
	
	poly.clear();
	ColourSystemGradient* grad = new ColourSystemGradient();
	
	int totaldivisions =360;
	//int numrevolutions = 20;
	//float smallradius = 50;
	//float largeradius = 200;
	ofPoint p;
	for(int i = totaldivisions*start; i<=totaldivisions*end; i++) {
		float angle1 = ofMap(i, 0, totaldivisions, 0, 360);
		float angle2 = ofMap(i, 0, totaldivisions, 0, 360*numrevolutions);
		
		float sr = smallradius;
		if((i/10)<volumes.size()) sr*= ofMap(volumes[i/10],0,1,0.8,1.1,true);
		p.set(sr,0,0);
		
		p.rotate(angle2, ofPoint(0,0,1));
		p.x+=largeradius;
		p.rotate(angle1+rotation, ofPoint(0,0,1));
		
		p+=pos;
		//p.z += i;
		
		poly.addVertex(p);
		int hue = ((int)ofMap(i,0,totaldivisions, 0, 255))%255;
		ofColor c;
		
		float brightness = 255;
		float fadedistance = 5; 
		
		if(i<totaldivisions*start+20) {
			brightness = ofMap(i, totaldivisions*start,totaldivisions*start+20, 0,255 );
			
		}

		if(i>totaldivisions*end-20) {
			brightness = ofMap(i, totaldivisions*end,totaldivisions*end-20, 0,255 );
			
		}
		c.setHsb(hue, 255,brightness);
		grad->addColourStop(c, ofMap(i,totaldivisions*start,totaldivisions*end, 0, 1));
		
		
		
	}
	//poly.draw();
	laserManager.addLaserPolyline(poly, grad);
	
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	if(key == 'f') ofToggleFullscreen();
	if(key == '\t') {
		
		int activePanelIndex = -1; 
		for(int i =0; i<panels.size(); i++) {
			if(panels[i]->getVisible()) activePanelIndex = i;
			panels[i]->setVisible(false);
			
		}
		if(activePanelIndex ==-1) {
			panels[0]->setVisible(true);
		} else if(activePanelIndex<panels.size()-1) {
			panels[activePanelIndex+1]->setVisible(true);
		}
		
		redGui.setVisible(laserGui.getVisible());
		greenGui.setVisible(laserGui.getVisible());
		blueGui.setVisible(laserGui.getVisible());
			
	}
	if(key == 'w') laserManager.showWarpPoints = !laserManager.showWarpPoints;
	if(key == ' ') previewProjector = !previewProjector;
	if(key == OF_KEY_DOWN) {
		setPosition(0);
		effectPipeOrganLines.setMode(0);
		effectLaserBeams.mode = 0;
		effectDomeLines.setMode(0);

		music.play();
		video.play();
	}
	if(key == OF_KEY_LEFT) {
		soundPositionMS = sync.getMSForBarNumber(sync.currentBar-1);
		setPosition(soundPositionMS);
		effectPipeOrganLines.setMode(0);
		effectLaserBeams.mode = 0;
		effectDomeLines.setMode(0);
	} else if(key == OF_KEY_RIGHT) {
		soundPositionMS = sync.getMSForBarNumber(sync.currentBar+1);
		setPosition(soundPositionMS);
		effectPipeOrganLines.setMode(0);
		effectLaserBeams.mode = 0;
		effectDomeLines.setMode(0);
	}
	if(key =='l') laserManager.showLaserPath = !laserManager.showLaserPath;
		
		
	
	if(key == 'p') {
		if(music.getIsPlaying()) {
			
			music.stop();
			video.stop();
		} else {

			music.play();
			video.play();
			setPosition(soundPositionMS);
			effectPipeOrganLines.setMode(0);
			effectLaserBeams.mode = 0;
			effectDomeLines.setMode(0);
		}
	
	}
	if(key=='c') {
		currentSVG++;
		if(currentSVG>=svgs.size()) currentSVG = -1;
	}
	if(key=='v') clappyBirdActive = !clappyBirdActive;
    

	if(key == '1') {
		
		addWordParticle("BAD!", ofPoint(-200,-200));
	
	
	}
	if(key == '2') {
		addWordParticle("OK!", ofPoint(-200,-200));
	}
	if(key == '3') {
		addWordParticle("PERFECT!", ofPoint(-200,-200));
	}
	
	
	if(key == '7') {
		
		addWordParticle("BAD!", ofPoint(200,-200));
		
		
	}
	if(key == '8') {
		addWordParticle("OK!", ofPoint(200,-200));
	}
	if(key == '9') {
		addWordParticle("PERFECT!", ofPoint(200,-200));
	}
	
	
//	if(key == '4') {
//		effectLaserBeams.mode = 2;
//		effectPipeOrganLines.setMode(0);
//		effectDomeLines.setMode(0);
//	}
//	if(key == '5') {
//		effectLaserBeams.mode = 0;
//		effectPipeOrganLines.setMode(1);
//		effectDomeLines.setMode(0);
//	}
//	if(key == '6') {
//		effectLaserBeams.mode = 0;
//		effectPipeOrganLines.setMode(2);
//		effectDomeLines.setMode(0);
//	}
//	if(key == '7') {
//	
//		effectLaserBeams.mode = 0;
//		effectPipeOrganLines.setMode(3);
//		effectDomeLines.setMode(0);
//	}
//	
//	if(key == '0') {
//		effectLaserBeams.mode = 0;
//		effectPipeOrganLines.setMode(0);
//		effectDomeLines.setMode(0);
//	}
	
	if(key == 's') {
		effectParticles.makeStarBurst(0.5);
	}

}

void ofApp :: setPosition(int posMS){
	music.setPositionMS(posMS);

	setVideoPositionMS(posMS);
	
	currentWooeyPosition = -1; 
	
}


void ofApp :: addWordParticle(string message, ofPoint pos) {
	
	wordParticles.push_back(WordParticle());
	wordParticles.back().init(message, pos, ofPoint(0,0,300));
	
	
	
}



void ofApp :: setVideoPositionMS(int posMS){
	float vidPos = (float)posMS / (video.getDuration()*1000);
	video.setPosition(ofClamp(vidPos, 0, 1));
	if(!music.getIsPlaying()) {
		video.play();
		video.setPosition(ofClamp(vidPos, 0, 1));
		video.stop();
		video.update();
	}
	
}


void ofApp :: updatePeakFrequency(float * val, int numBands){

	
	//pipeOrganData.draw();

	//int numPipes = 20;
	//ofRectangle rect(506,631,270,190);

	int bottom = 30;
	int top = 70;
	float threshold = 0.1;
	
	float loudestLevel = threshold;
	int loudestIndex = 0;
	
	
	for(int i = bottom; i<numBands && i<top; i++) {
	
		if(val[i]>loudestLevel) {
			loudestIndex = i;
			loudestLevel = val[i];
		} else {
			// feeble attempt at countering the low end weighting of FFT.
			loudestLevel *=0.95;
		}
	}
	
	if(loudestIndex>0) {
		
		currentPeakFrequency = ofMap(loudestIndex, bottom, top, 0, 1);
		
		//loudestIndex = floor(ofMap(loudestIndex, bottom, top, 0, pipeOrganData.pipes.size()));
		//if(loudestIndex>=pipeOrganData.pipes.size()) loudestIndex = pipeOrganData.pipes.size()-1;
		
		//currentPipeIndex += (loudestIndex-currentPipeIndex) * 0.5;
		
		//if(currentPipeIndex<loudestIndex) currentPipeIndex++;
		//else if(currentPipeIndex>loudestIndex) currentPipeIndex--;
		
		//ofLine(pipeOrganData.pipes[(int)currentPipeIndex].top, pipeOrganData.pipes[(int)currentPipeIndex].bottom);
	} else {
		currentPeakFrequency = -1;
	}
	
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int width, int height){
	
	
	calculateScreenSizes();
}


void ofApp::calculateScreenSizes(){
    int monitorCount;
	
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	
	//cout << "RESIZE" << " " << ofGetWindowMode()<< endl;
   // screens.clear();
    
    int leftMost = 0,
	topMost = 0;
    
	vector<ofRectangle> screens;
	
    for(int i = 0; i < monitorCount; i++){
		
		ofRectangle screen;
		
		int x=0,
		y=0,
		w=0,
		h=0;
		
        glfwGetMonitorPos(monitors[i],&x,&y);
        const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[i]);
		screen.x = x;
		screen.y = y;
        screen.width = desktopMode->width;
        screen.height = desktopMode->height;
        
        screens.push_back(screen);
        cout << i << " " << screen << endl;
		if( leftMost > screen.x ) leftMost = screen.x;
		if( topMost > screen.y ) topMost = screen.y;
		
    }
    
    for(int i = 0; i < monitorCount; i++){
	//	screens[i].x -= leftMost;
	//	screens[i].y -= topMost;
		
    }
    
	//std::sort( screens.begin(), screens.end(), screenSort );
	
	//uiScreenRect = screens.back();
	
	
	if(screens.size()>1) {
		secondScreenRect = screens[1];
	} else {
		secondScreenRect.set(screens[0].getRight(), screens[0].getTop(), 1280,720);
	}
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	
	
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int numChannels){
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;
	
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
		
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
	//smoothedInputVolume *= 0.8;
	//smoothedInputVolume += 0.5 * curVol;
	//smoothedInputVolume = curVol;
	
	smoothedInputVolume += (( curVol-smoothedInputVolume) * 0.1);
	//bufferCounter++;

		
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
	pipeOrganData.mouseDragged(x,y); 
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	cout << "MOUSE : " << x << " " << y << endl;
	pipeOrganData.mousePressed(x,y); 
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	pipeOrganData.mouseReleased(x,y);

}


//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::exit() {
	redGui.save();
	greenGui.save();
	blueGui.save();
	
	laserGui.save();
	//pipeOrganData.save();
	domeData.save();
	laserManager.warp.saveSettings();
}



void ofApp::writeMessage(string message) {
	float halfWidth = proximaNovaLB.stringWidth(message)/2;
	proximaNovaLB.drawString(message, -halfWidth, 0);
}

