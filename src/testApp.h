//	Cámara Lúcida
//	www.camara-lucida.com.ar
//
//	Copyright (C) 2011  Christian Parsons
//	www.chparsons.com.ar
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "ofMain.h"
#include "LineSegment.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
<<<<<<< Updated upstream
#include "CamaraLucida.h"
#include "cmlMesh_freenect.h"
#include <GLUT/glut.h>
=======
#include "Particle.h"




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

>>>>>>> Stashed changes

class testApp : public ofBaseApp 
{
public:
	
	cml::CamaraLucida camluc;
	cml::Mesh_freenect *mesh;
	
	void render_texture(ofEventArgs &args);
	void render_hud(ofEventArgs &args);
	
	bool debug_depth_texture;
	
	
	//	kinect
	
	ofxKinect kinect;
	uint16_t *raw_depth_pix;
	uint8_t *rgb_pix;
	
	bool init_kinect();
	bool update_kinect();
	
	// threshold
	int far;
	int near;
	
	//serial
	ofSerial	serial;
	bool		radio_on;
	int			rf;
	int			timer;

<<<<<<< Updated upstream
=======
	int trailing;
	
    bool radio_on;
>>>>>>> Stashed changes
	//	ui
	
	bool pressed[512];
	void init_keys();
<<<<<<< Updated upstream
	
	// analysis
	ofxCvContourFinder 	contourFinder;

	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage depthImage;
	ofxCvGrayscaleImage pastDepth;
=======
   
    ofImage imgd;
    ofImage colorImg;
    
	// analysis
    ofxCv::ContourFinder contourFinder;
    float threshold;
    float a;
    float pulse;
    
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
	
	int sampling ;                  //pixels to skip
	vector<Particle> particles ;    //vector to store pixels
	int numParticles ;          //Number of particles 
	
	//Spring and Sink Factors
	bool springEnabled ;        //toggle whether particles return to their origin
	float forceRadius ;             //radius of repellent/attraction force
	float friction ;                //damping to slow the particles down
	float springFactor ;          //how much the particle "springs" back to origin
	int cursorMode ;            
    
>>>>>>> Stashed changes

	ofxCvColorImage colorImg;

	ofPolyline toOf(const ofxCvBlob& blob);
	
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
	
<<<<<<< Updated upstream
	LineSegment constrainLineToPolygon(LineSegment* ls, ofPolyline* poly, bool& success);
	ofPolyline polyline;
    
    vector<LineSegment> lines;
    vector<LineSegment> clippedLines;
    vector<LineSegment> failedClippedLines;
    
    LineSegment currentLine;
    int currentLinePoint;
    
    float a;
    float alphaPulse;
	
	
=======
	void updateParticles();

>>>>>>> Stashed changes
};
