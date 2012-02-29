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

#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include "CamaraLucida.h"
#include "cmlMesh_freenect.h"
#include <GLUT/glut.h>

#include "Particle.h"

class testApp : public ofBaseApp 
{
public:
	
	cml::CamaraLucida camluc;
	cml::Mesh_freenect *mesh;
	
	void render_texture(ofEventArgs &args);
	void render_hud(ofEventArgs &args);
	
	bool debug_depth_texture;
    bool debug_hue_texture;

	
	
	//	kinect
	
	ofxKinect kinect;
	uint16_t *raw_depth_pix;
	uint8_t *rgb_pix;
	
	bool init_kinect();
	bool update_kinect();
    
    
	float tex_width;
	float tex_height;
	
	// threshold
	int far;
	int near;
	
	//serial
	ofSerial	serial;
	bool		radio_on;
	int			rf;
	int			timer;

	//	ui
	
	bool pressed[512];
	void init_keys();
	
	// analysis
	ofxCvContourFinder 	contourFinder;
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage depthImage;
	ofxCvGrayscaleImage pastDepth;
	ofxCvColorImage colorImg;
    
    //particles
    int sampling;
    vector<Particle> particles;
    int numParticles;
    
    //particle engine
    int cursorMode; 
    float forceRadius; 
    float friction; 
    float springFactor; 
    bool springEnabled;

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
    void createParticles(int _sampling);
    void updateParticles();
	
};
