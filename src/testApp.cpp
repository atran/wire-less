//	Using Camara Lucida's framework
//	www.camara-lucida.com.ar

#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	
	init_keys();
    
	contourFinder.setMinAreaRadius(100);
	contourFinder.setMaxAreaRadius(150);	
    
    //imgd.loadImage("tran.png");
    
    imgd.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    
	//kinect.init();
	//kinect.setVerbose(true);
	//kinect.open();
    
	// start with the live kinect source
	kinectSource = &kinect;

    
	bRecord = false;
	bPlayback = false;
    
    
	box2d.init();
	//box2d.setGravity(0, 10);
	box2d.createGround();
	box2d.setFPS(15.0);
	box2d.registerGrabbing();
    
    VF.setupField(10,10,ofGetWidth(), ofGetHeight());
	//VF.randomizeField(1.0);
    
}

void testApp::update()
{
    box2d.update();
    
    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
	float minDis = ofGetMousePressed() ? 300 : 200;
    
    //	for(int i=0; i<circles.size(); i++) {
    //        circles[i].color.r = ofRandom(0,255);
    //		float dis = mouse.distance(circles[i].getPosition());
    //		if(dis < minDis) circles[i].addRepulsionForce(mouse, 10);
    //		else circles[i].addAttractionPoint(mouse, 10.0);
    //	}
    //    
    
    // vector field calculations
    for(int i=0; i<circles.size(); i++) {
		ofVec2f	frc = VF.getForceFromPos(circles[i].getPosition().x, circles[i].getPosition().y);
		circles[i].addForce(frc, 10);
		
		// also a small force  towards the center
		ofVec2f centerDiff = ofPoint (contourFinder.getCentroid(0).x, contourFinder.getCentroid(0).y) -  circles[i].getPosition();
		centerDiff.normalize();
		circles[i].addForce(centerDiff, 1.2);
		
		circles[i].setDamping(0.93f);
		
	}

    kinectSource->update();
    
    if(kinectSource->isFrameNew()) {
        // record ?
		if(bRecord && kinectRecorder.isOpened()) {
			kinectRecorder.newFrame(kinect.getRawDepthPixels(), kinect.getPixels());
		}
        
        
        imgd.setFromPixels(kinectSource->getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
        imgd.update();
        unsigned char * pix = imgd.getPixels();
        
        
        // give particles calibrated color from kinect
        for(int i=0; i<circles.size(); i++) {
            CustomParticle c = circles[i];
            
            /*kinectSource->getCalibratedRGBPixels()
            c.getPosition().x;
            c.getPosition().y;
            c.color.r = ;
            c.color.g = ;
            c.color.b = ;*/
        }
        
        
    }
    contourFinder.findContours(imgd);
}

void testApp::draw()
{	 
	ofEnableAlphaBlending();
    glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    
    
    imgd.draw(0,0,100,100);
    
    
    for(int i=0; i<circles.size(); i++) {
		ofFill();
		ofSetHexColor(0x90d4e3);
		circles[i].draw();
	}
    
    
    int z = contourFinder.size();
    //cout << "blobs: " << z << endl;
    polyLine.destroy();	

    ofSetColor(0xfffffff);
    VF.draw();
    
    if (z == 0) {
        circles.clear();
    }
	if (radio_on) {
        for(int i = 0; i < z; i++) {
            vector<cv::Point> contour = contourFinder.getContour(i);
            int numPoints = contour.size();    
        }
	} else {
        
        for(int i = 0; i < z; i++) {
            
            // some different styles of contour centers
            ofVec2f centroid = toOf(contourFinder.getCentroid(i));
            ofVec2f average = toOf(contourFinder.getAverage(i));
            ofVec2f center = toOf(contourFinder.getCenter(i));
            
            // you can also get the area and perimeter using ofPolyline:
            // ofPolyline::getArea() and ofPolyline::getPerimeter()
            double area = contourFinder.getContourArea(i);
            double length = contourFinder.getArcLength(i);	
            
            ofPushMatrix();
            
            ofSetColor(yellowPrint);
            ofPolyline perimeter = contourFinder.getPolyline(i);
            perimeter = perimeter.getResampledBySpacing(1);	
			perimeter.setClosed(true);
            perimeter.draw();
            //perimeter.getSmoothed(10).draw();
            
            ofPopMatrix();
            
            polyLine.addVertexes(perimeter);
            polyLine.simplify();
            //polyLine.setPhysics(0.0, 0.5, 0.5);
            polyLine.create(box2d.getWorld());
            
            perimeter.clear();
            
            if (circles.size() < 1000) {
                ofPushMatrix();
                ofSetColor(yellowPrint);
                CustomParticle circle;
                circle.setPhysics(0.1, 0.53, 0.9);
                circle.setup(box2d.getWorld(), centroid.x, centroid.y, 2);
                circle.color.r = ofRandom(0, 255);
                circle.color.g = ofRandom(0, 255);
                circle.color.b = ofRandom(0, 255);
                circles.push_back(circle);
                ofPopMatrix();
            } else {
                //circles.pop_back();
            }
  
            
            
        }
        
	}
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
}


void testApp::keyPressed(int key)
{
	pressed[key] = true;
	
    if (key =='r') {
		radio_on = !radio_on;
	}
    if(key == 'c') {
		float r = ofRandom(4, 20);		// a random radius 4px - 20px
		CustomParticle circle;
		circle.setPhysics(0.0, 0.53, 0.1);
		circle.setup(box2d.getWorld(), mouseX, mouseY, r);
		circles.push_back(circle);
	}
    if(key == 'e') {
        bRecord = !bRecord;
        if(bRecord) {
            startRecording();
        } else {
            stopRecording();
        }
    }
    
    if(key == 'q') {
        bPlayback = !bPlayback;
        if(bPlayback) {
            startPlayback();
        } else {
            stopPlayback();
        }
        
        
    }
}

void testApp::keyReleased(int key)
{
    pressed[key] = false;
}

void testApp::init_keys()
{
    for (int i = 0; i < 512; i++) 
        pressed[i] = false;
}

void testApp::mouseMoved(int x, int y )
{
    
}

void testApp::mouseDragged(int x, int y, int button)
{
    
}



void testApp::mousePressed(int x, int y, int button)
{
    cout << x << ", " << y << endl;
}

void testApp::mouseReleased(int x, int y, int button)
{
    
}

void testApp::resized(int w, int h)
{
    
}

void testApp::debug()
{
    
}

//--------------------------------------------------------------
void testApp::startRecording() {
    
    // stop playback if running
    stopPlayback();
    
    kinectRecorder.init(ofToDataPath("recording1.dat"));
    bRecord = true;
}

//--------------------------------------------------------------
void testApp::stopRecording() {
    kinectRecorder.close();
    bRecord = false;
}

//--------------------------------------------------------------
void testApp::startPlayback() {
    
    stopRecording();
    kinect.close();
    
    // set record file and source
    kinectPlayer.setup(ofToDataPath("recording.dat"), true);
    kinectPlayer.loop();
    kinectSource = &kinectPlayer;
    bPlayback = true;
}

//--------------------------------------------------------------
void testApp::stopPlayback() {
    kinectPlayer.close();
    kinect.open();
    kinectSource = &kinect;
    bPlayback = false;
}
