#pragma once

#include "ofMain.h"
#include <GLUT/glut.h>
#include "ofxCv.h"
#include "ofxBox2d.h"
#include "vectorField.h"
#include "ofxKinect.h"



// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle {
	
public:
	CustomParticle() {
	}
	ofColor color;
	void draw() {
		float radius = getRadius();
		
		glPushMatrix();
		glTranslatef(getPosition().x, getPosition().y, 0);
		
		ofSetColor(color.r, color.g, color.b);
		ofFill();
		ofCircle(0, 0, radius);	
		
		glPopMatrix();
		
	}
};


class testApp : public ofBaseApp 
{
public:	

    bool radio_on;
	//	ui
	
	bool pressed[512];
	void init_keys();
   
    ofImage imgd;

	// analysis
    ofxCv::ContourFinder contourFinder;
    float threshold;
    
    
    //ofxBox2d
    ofxBox2d						box2d;			  //	the box2d world
	ofxBox2dPolygon					polyLine;		  //	the box2d polygon/line shape
	ofPolyline						drawing;		  //	we draw with this first

    vector		<CustomParticle>	circles;		  //	custom box2d circles
	
    vectorField VF;
    
    ofxKinect 			kinect;
    ofxKinectRecorder 	kinectRecorder;
    ofxKinectPlayer 	kinectPlayer;
    int 				nearThreshold;
    int					farThreshold;
    ofxBase3DVideo* 	kinectSource;
    bool 				bRecord;
    bool 				bPlayback;
    
    /// start/stop recording and playback,
    /// make sure you don't record and playback simultaneously 
    /// using the same file!!!
    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();

	// app
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
	
	void debug();
};
