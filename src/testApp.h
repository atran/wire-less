#pragma once

#include "ofMain.h"
#include <GLUT/glut.h>
#include "ofxCv.h"
#include "vectorField.h"
#include "ofxKinect.h"
#include "Particle.h"
#include "CamaraLucida.h"
#include "cmlMesh_freenect.h"
#include <GLUT/glut.h>





class testApp : public ofBaseApp 
{
public:	
    
	cml::CamaraLucida camluc;
    
	void render_texture(ofEventArgs &args);
	void render_hud(ofEventArgs &args);
    
	cml::Mesh_freenect *mesh;
    
	uint16_t *raw_depth_pix;
    
    bool radio_on;
	//	ui
	
	bool pressed[512];
	void init_keys();
   
    ofImage imgd;

	// analysis
    ofxCv::ContourFinder contourFinder;
    float threshold;
    
	ofPolyline						drawing;		  //	we draw with this first

	
    vectorField VF;
    
    ofImage colorImg;
    
    int trailing;
    float a;
    float pulse;
    
    ofxKinect 			kinect;
  
    int 				nearThreshold;
    int					farThreshold;
    ofxBase3DVideo* 	kinectSource;
    bool 				bRecord;
    bool 				bPlayback;
    ofxKinectPlayer 	kinectPlayer;
    

    int sampling ;                  //pixels to skip
	vector<Particle> particles ;    //vector to store pixels
	int numParticles ;          //Number of particles 
    
    
    
	//Spring and Sink Factors
	bool springEnabled ;        //toggle whether particles return to their origin
	float forceRadius ;             //radius of repellent/attraction force
	float friction ;                //damping to slow the particles down
	float springFactor ;          //how much the particle "springs" back to origin
	int cursorMode ;            

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
    
    void startPlayback();
    void stopPlayback();
    void updateParticles();
};
