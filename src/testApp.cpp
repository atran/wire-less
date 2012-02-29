//	Using Camara Lucida's framework
//	www.camara-lucida.com.ar

#include "testApp.h"

void testApp::setup()
{
	//ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	
	init_keys();
	debug_depth_texture = true;
	
	grayImage.allocate(kinect.getWidth(), kinect.getHeight());
	depthImage.allocate(kinect.getWidth(), kinect.getHeight());
	colorImg.allocate(kinect.getWidth(), kinect.getHeight());
	pastDepth.allocate(kinect.getWidth(), kinect.getHeight());
	
	if (!init_kinect())
		return;
	
    
	tex_width = 1024;
	tex_height = 768;
    debug_hue_texture = true;
    
	mesh = new cml::Mesh_freenect(raw_depth_pix);
	
	camluc.init(ofToDataPath("camara_lucida/kinect_calibration.yml"),
				ofToDataPath("camara_lucida/projector_calibration.yml"),
				ofToDataPath("camara_lucida/camara_lucida_config.xml"),
				mesh, ofGetWidth(), ofGetHeight(), 1);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_hud, this, &testApp::render_hud);
		
	//thresholds
	far = 0;
	near = 254;
		
	//serial
	//serial.listDevices();
	//vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	//serial.setup("/dev/cu.usbmodemfa131",57600); 
    cursorMode = 0 ; 
    forceRadius = 45 ; 
    friction = 0.85 ; 
    springFactor = 0.12 ;
    
    radio_on = false;
}

void testApp::update()
{
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update();
		
//	if(serial.available() > 1){ 
//		
//		unsigned char bytesReturned[4];      
//		serial.readBytes(bytesReturned, 4); 
//		string serialData = (char*) bytesReturned; // cast to char  
//		int rf_holder = ofToInt(serialData);
//		if (rf_holder != 0) rf = rf_holder;
//	}
	
	
//	if (timer > 0) {
//		timer--;
//	} else {
//		if (rf > 194) { 
//			radio_on = true;
//			//start 3 second timer
//			timer = 90;
//		}
//		else if (rf < 194) {
//			radio_on = false;
//		}		
//	}
        
    // create a depth grayscale image, grab the pixels
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

    
	// binarize depth grayscale image

    grayImage.flagImageChanged();
    
    if (!radio_on){
        createParticles(2);
    } else if (radio_on && ofGetFrameNum() % 200 == 0){
        createParticles(5);
    }
    
    updateParticles();
}

void testApp::createParticles(int _sampling)
{
    unsigned char * pix = grayImage.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight()-1;
	unsigned char * colorPixels = colorImg.getPixels();
    
    
    particles.clear();
    sampling = _sampling;
    int w = grayImage.width;
    int h = grayImage.height;
    numParticles = w*h/sampling;
    
    
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
            int index = ( y * w + x );
            int grey = pix[index];
            
            ofColor color;
            color = kinect.getCalibratedColorAt(x,y);
            if (grey < near && grey > far){
                particles.push_back( Particle ( ofPoint ( x + xOffset , y + yOffset ) , grey, color ) ) ;
            }
            
        }
    }

}

void testApp::draw()
{	    
	ofEnableAlphaBlending();
	camluc.render();
	
	glScalef(1, -1, 1);
	glTranslatef(-0.3, 0.3, 1);
	glutWireTeapot(0.1);
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_texture, this, &testApp::render_texture);
	ofRemoveListener(camluc.render_hud, this, &testApp::render_hud);
	
	camluc.dispose();
	kinect.close();
	
	delete mesh;
	mesh = NULL;
}

void testApp::render_hud(ofEventArgs &args)
{

}

void testApp::render_texture(ofEventArgs &args)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float w = 1024;
	float h = 768;
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    if (debug_hue_texture) {
        mesh->debug_hue_texture(0, 0, tex_width, tex_height);  
    }
    
    
    else {
        std::vector<Particle>::iterator p;
        glColor3f(1, 1, 1);
        
        glBegin(GL_POINTS); 
        
        if (!radio_on) {
            for (p = particles.begin();p!=particles.end();p++){
                int x = (p->position.x) + ofRandom(-1,1);
                int y = (p->position.y) + ofRandom(-1,1);
                glColor3ub(255,255,255);
//                glColor3ub((unsigned char)p->color.r,(unsigned char)p->color.g,(unsigned char)p->color.b);
                glVertex3f(x,y,0);
            }
        } else if (radio_on) {
            for ( p = particles.begin() ; p != particles.end() ; p++ )
            {
                if (ofGetFrameNum() % 200 >= 0 && ofGetFrameNum() % 200 < 10){
                    glColor3ub(255,255,255);
                }
                else {
                    glColor3ub((unsigned char)p->color.r,(unsigned char)p->color.g,(unsigned char)p->color.b);
                }
                glVertex3f(p->position.x, p->position.y , 0 );
            }
        }
        
        glEnd();
        
        
//		kinect.getDepthTextureReference().draw(0, 0, tex_width, tex_height);

    }

    
	//depthImage.draw(0,h/2,w/2,h);
	//grayImage.draw(0,0,w,h);
	//kinect.getDepthTextureReference().draw(w/2, h/2, w/2+400, h/2+300);	
	//contourFinder.draw(0, 0, w, h);

	glColor3f(1, 1, 0);
	ofCircle(800, 200, 100);
}

bool testApp::init_kinect()
{
	//kinect.enableCalibrationUpdate(false);
	//kinect.enableDepthNearValueWhite(false);
	
	kinect.init();
	kinect.open();
	//	ktilt = 0;
	//	kinect.setCameraTiltAngle(ktilt);
	
	return update_kinect();
}

bool testApp::update_kinect()
{
	if (!kinect.isConnected())
		return false;
	
	kinect.update();
	raw_depth_pix = kinect.getRawDepthPixels();
	
	return true;
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


void testApp::keyPressed(int key)
{
	pressed[key] = true;
    
    if (key == 'm'){
        mesh->print();
    }
	if (key == 'd')
	{
		camluc.toggle_debug();
	}
	if (key == 'f')
	{
        if (ofGetWindowPositionX() == 0)
        {
            ofSetWindowPosition(1440,0);
            ofSetFullscreen(true);
        }
        else
        {
            ofSetWindowPosition(0,0);
            ofSetFullscreen(false);
        }
	}
	if (key == 't') 
	{
		far++;
		cout << "far: " << far << endl;

	}
	if (key == 'y')
	{
		far--;
		cout << "far: " << far << endl;
	}
	if (key == 'u')
	{
		near++;
		cout << "near: " << near << endl;
	}
	if (key =='i') {
		near--;
		cout << "near: " << near << endl;
	}
	if (key =='r') {
		radio_on = !radio_on;
	}
    if (key == 'a') {
        debug_hue_texture = !debug_hue_texture;
        cout << "debug? " << debug_hue_texture << endl;
        //kinect.toggleCalibrationUpdate();
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