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
    startPlayback();

    
	bRecord = false;
	bPlayback = false;
    
    VF.setupField(10,10,ofGetWidth(), ofGetHeight());
	//VF.randomizeField(1.0);
    
    cursorMode = 0 ; 
    forceRadius = 45 ; 
    friction = 0.85 ; 
    springFactor = 0.12 ; 
    springEnabled = true ;	
    
}

void testApp::update()
{
    kinectSource->update();
        
    if(kinectSource->isFrameNew()) {
        
        a += ofDegToRad(1);
        // record ?
		colorImg.setFromPixels(kinectSource->getCalibratedRGBPixels(), 640,480, OF_IMAGE_COLOR);
		colorImg.update();
		unsigned char * colorPixels = colorImg.getPixels();
        
        
        imgd.setFromPixels(kinectSource->getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
		unsigned char * pixels = imgd.getPixels();
        
		int numPixels = imgd.getWidth() * imgd.getHeight();
//		for(int i = 0; i < numPixels; i++) {
//			//if(pix[i] < nearThreshold && pix[i] > farThreshold) {
//			//invert
//			if(pixels[i] < 230 && pixels[i] > 44) {
//			} else {
//				pixels[i] = 0;
//			}
//		}
        
		//imgd.flagImageChanged();
        
		imgd.update();
		pixels = imgd.getPixels();
        
		if (trailing == 10){
        //if (trailing == 100){
			particles.clear();
			trailing = 0;
			//if the app performs slowly raise this number
			sampling = 1; 
            
			//store width and height for optimization and clarity
			int w = imgd.width ; 
			int h = imgd.height ; 
			numParticles = ( imgd.width * imgd.height ) / sampling ; 
            
			//offsets to center the particle son screen
			int xOffset = (ofGetWidth() - w ) /2 ; 
			int yOffset = (ofGetHeight() - h ) /2 ;
            
			//Loop through all the rows
			for ( int x = 0 ; x < w ; x+=sampling ) 
			{
				//Loop through all the columns
				for ( int y = 0 ; y < h ; y+=sampling ) 
				{
					//Pixels are stored as unsigned char ( 0 <-> 255 ) as RGB
					//If our image had transparency it would be 4 for RGBA
					int index = ( y * w + x ) ; 
					int colorIndex = index * 3 ;
					int grey = pixels[index];
                    
					ofColor color;
					color.r = colorPixels[colorIndex] ;       //red pixel
					color.g = colorPixels[colorIndex+1] ;     //blue pixel
					color.b = colorPixels[colorIndex+2] ;     //green pixel
//                    if(grey < 230 && grey > 0) {
                    particles.push_back( Particle ( ofPoint ( x + xOffset , y + yOffset ) , grey, color ) ) ;  
//                    }

				}
			}
		} else {
			trailing++;
            //particles.clear();
		}
        
        
    }
	updateParticles();
}

void testApp::draw()
{	 
    float w = 1024;
	float h = 768;
    glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1, 1, 1);
    ofPushMatrix(); 
    
    //draws a star
    //ofSetPolyMode(OF_POLY_WINDING_NONZERO);
    //    glBegin(GL_LINE);
    //    for (int y=0;y<640;y+=5){
    //        ofBeginShape();
    //        for (int x=0;x<480*3;x+=3){
    //            int pix1 = (y*640*3) + x;
    //            int pix2 = (y*640*3) + (x+1);
    //            int pix3 = (y*640*3) + (x+2);
    //            glColor3f(pix1,pix2,pix3);
    //            glVertex3f(x*2,y*2, 1);
    //        }
    //        ofEndShape();
    //    }
    //    glEnd();
    //    ofPopMatrix();
    
    //cout << pulse << endl;
    //ofTranslate(10,100,pulse);
    
    glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	//Begin the openGL Drawing Mode
    glBegin(GL_POINTS);
    
    //Triangles look Cool too 
    //glBegin(GL_TRIANGLES);
	
    //glBegin(GL_LINE_LOOP);
    
    //Create an iterator to cycle through the vector
    std::vector<Particle>::iterator p ; 
    for ( p = particles.begin() ; p != particles.end() ; p++ )
    {
        //glColor3ub((unsigned char)p->grey,(unsigned char)p->grey,(unsigned char)p->grey);
        glColor3ub((unsigned char)p->color.r,(unsigned char)p->color.g,(unsigned char)p->color.b);
		//glColor4ub((unsigned char)p->color.r,(unsigned char)p->color.g,(unsigned char)p->color.b,(unsigned char)p->grey);
        glVertex3f(p->position.x, p->position.y , 0 );
    }
    
    glEnd();
    ofPopMatrix();  
    
    
    ofSetColor ( 255 , 255 , 255 ) ;
    
    string output = "S :: Springs on/off : " + ofToString(springEnabled) +
    
    "\n C :: CursorMode repel/attract " + ofToString( cursorMode ) +
    
    "\n # of particles : " + ofToString( numParticles ) +
    
    " \n fps:" +ofToString( ofGetFrameRate() ) ;
    
    ofDrawBitmapString(output ,20,666);
	imgd.draw(0,0,100,150);


}


void testApp::exit()
{
	kinect.close();
}


void testApp::keyPressed(int key)
{
	pressed[key] = true;
	
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

void testApp::startPlayback() {
    
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

void testApp::updateParticles() {
	ofPoint diff ;          //Difference between particle and mouse
    float dist ;            //distance from particle to mouse ( as the crow flies ) 
    float ratio ;           //Ratio of how strong the effect is = 1 + (-dist/maxDistance) ;
    const ofPoint mousePosition = ofPoint( mouseX , mouseY ) ; //Allocate and retrieve mouse values once.
    
    
    //Create an iterator to cycle through the vector
    std::vector<Particle>::iterator p ; 
    for ( p = particles.begin() ; p != particles.end() ; p++ ) 
    {
        ratio = 1.0f ; 
        p->velocity *= friction ; 
        //reset acceleration every frame
        p->acceleration = ofPoint() ; 
        diff = mousePosition - p->position ;  
        dist = ofDist( 0 , 0 , diff.x , diff.y ) ;
        //If within the zone of interaction
        if ( dist < forceRadius )  
        {
            ratio = -1 + dist / forceRadius ; 
            //Repulsion
            if ( cursorMode == 0 ) 
                p->acceleration -= ( diff * ratio) ;
            //Attraction
            else
                p->acceleration += ( diff * ratio ) ; 
        }
        if ( springEnabled ) 
        {
            //Move back to the original position
            p->acceleration.x += springFactor * (p->spawnPoint.x - p->position.x);
            p->acceleration.y += springFactor * (p->spawnPoint.y - p->position.y) ;
        }
        
        p->velocity += p->acceleration * ratio ; 
        p->position += p->velocity ; 
    }
}