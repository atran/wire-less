//	Using Camara Lucida's framework
//	www.camara-lucida.com.ar

#include "testApp.h"

void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
<<<<<<< Updated upstream
	ofSetFrameRate(30);

    a = 0;
    alphaPulse = 0;
	
	init_keys();
	debug_depth_texture = true;
	
	grayImage.allocate(kinect.getWidth(), kinect.getHeight());
	depthImage.allocate(kinect.getWidth(), kinect.getHeight());
	colorImg.allocate(kinect.getWidth(), kinect.getHeight());
	pastDepth.allocate(kinect.getWidth(), kinect.getHeight());
	
	if (!init_kinect())
		return;
	
	mesh = new cml::Mesh_freenect(raw_depth_pix);
	
	camluc.init(ofToDataPath("camara_lucida/kinect_calibration.yml"),
				ofToDataPath("camara_lucida/projector_calibration.yml"),
				ofToDataPath("camara_lucida/camara_lucida_config.xml"),
				mesh, ofGetWidth(), ofGetHeight(), 1);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_hud, this, &testApp::render_hud);
		
	//thresholds
	far = 189;
	near = 85;
	
	//generate lines
	for (int z = 0; z < 400; z+=10) {
		currentLine.p0.x = 0;
		currentLine.p0.y = z;
		currentLine.p1.x = 1000;
		currentLine.p1.y = z;
		lines.push_back(currentLine);
	}
	
	//serial
	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	serial.setup("/dev/cu.usbmodemfa131",57600); 
	radio_on = false;
	timer = 0;
=======
	ofSetFrameRate(60);
	
	init_keys();
    a = 0;
    pulse=0;
	contourFinder.setMinAreaRadius(100);
	contourFinder.setMaxAreaRadius(150);	
    
    //imgd.loadImage("tran.png");
    
    imgd.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    colorImg.allocate(640, 480, OF_IMAGE_COLOR);
	//kinect.init();
	//kinect.setVerbose(true);
	//kinect.open();
    
	// start with the live kinect source
	kinectSource = &kinect;

    
	bRecord = false;
	bPlayback = true;
    
    radio_on = false;
    startPlayback();
    
    //VF.setupField(10,10,ofGetWidth(), ofGetHeight());
	//VF.randomizeField(1.0);
	
	//Set spring and sink values
    cursorMode = 0 ; 
    forceRadius = 45 ; 
    friction = 0.85 ; 
    springFactor = 0.12 ; 
    springEnabled = true ;	
    
	trailing = 0;
>>>>>>> Stashed changes
}

void testApp::update()
{
<<<<<<< Updated upstream
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update();
		a += ofDegToRad(10);
		alphaPulse = abs(sin(a)) * 255;
		
	if(serial.available() > 1){ 
		
		unsigned char bytesReturned[4];      
		serial.readBytes(bytesReturned, 4); 
		string serialData = (char*) bytesReturned; // cast to char  
		int rf_holder = ofToInt(serialData);
		if (rf_holder != 0) rf = rf_holder;
	}
	
	
	if (timer > 0) {
		timer--;
	} else {
		if (rf > 194) { 
			radio_on = true;
			//start 3 second timer
			timer = 90;
		}
		else if (rf < 194) {
			radio_on = false;
		}		
	}
}

void testApp::draw()
{	    
	ofEnableAlphaBlending();
	camluc.render();
	
	//glScalef(1, -1, 1);
	//glTranslatef(-0.3, 0.3, 1);
	//glutWireTeapot(0.1);
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
	//ofDrawBitmapString("press 'o' to debug camera as a texture \n 'd' to toggle camara lucida debug, then use 'v' to change viewpoint between camera and projector \n mousedrag to rotate, 'z'+mousedrag to zoom, 'x' to reset the debug transformations \n and 'c'+keyup/down to change depth xoffset", 10, 10);

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
	
	glColor3f(1, 1, 1);
	
	// create a depth grayscale image, grab the pixels
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	unsigned char * pix = grayImage.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight()-1;
	
	// binarize depth grayscale image
	for (int i = numPixels; i > 0; i--){
		if (pix[i] > near && pix[i] < far) {
			pix[i] = 255;
		} else {
			pix[i] = 0;
		}
	}
	
	if (radio_on) {
		ofTranslate(w,0); 
		ofScale(-1, 1, 1);
	
		
		// accumulate pixels
		// TODO: do alpha blending
		
		unsigned char * past_pix = pastDepth.getPixels();
	
		for (int i = numPixels; i > 0; i--){
			if ( pix[i] == 255 || past_pix[i] == 255) {
				pix[i] = 255;
			} else {
				pix[i] = 0;
			}
		}
	
		depthImage.setFromPixels(pix, kinect.width, kinect.height);
		depthImage.draw(0,0,w,h);
	
		pastDepth.setFromPixels(pix, kinect.width, kinect.height);
		
	} else {
		
		pastDepth.set(0);
		
		depthImage.setFromPixels(pix, kinect.width, kinect.height);
		
		ofTranslate(w,0); 
		ofScale(-1.6, 1.6, 1);
		
		contourFinder.findContours(grayImage, 500, (340*240)/1, 5, false);
		
		for (int i = 0; i < contourFinder.nBlobs; i++){
			ofSetColor(255, 0, 0);
			ofFill();
			ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			
			polyline = toOf(contourFinder.blobs[i]);
			polyline = polyline.getResampledBySpacing(10);	
			polyline.setClosed(true);
			polyline.draw();
			
			bool success = true;
			for(int t = 0; t < lines.size(); t++) {
				
				//ofSetColor(255,200);
				//ofLine(lines[t].p0.x, lines[t].p0.y, lines[t].p1.x, lines[t].p1.y);
				LineSegment clippedLine = constrainLineToPolygon(&lines[t], &polyline, success);
			
				if(success) {
					ofPushStyle();
					ofSetLineWidth(10);					
					ofSetColor(255,0,0,alphaPulse);
					clippedLine.draw();
					ofPopStyle();
				} 
				currentLinePoint = 0;
			}
			
		}
	}
	
	//depthImage.draw(0,h/2,w/2,h);
	//grayImage.draw(0,0,w,h);
	//kinect.getDepthTextureReference().draw(w/2, h/2, w/2+400, h/2+300);	
	//contourFinder.draw(0, 0, w, h);

	//glColor3f(1, 1, 0);
	//ofCircle(800, 200, 100);
=======
	kinectSource->update();
    
    if(kinectSource->isFrameNew()) {
        
        a += ofDegToRad(1);
        cout << a << endl;
        pulse = (abs(sin(a))) * 100 + 100;
        // record ?
		if(bRecord && kinectRecorder.isOpened()) {
			kinectRecorder.newFrame(kinect.getRawDepthPixels(), kinect.getPixels());
		}
		colorImg.setFromPixels(kinectSource->getCalibratedRGBPixels(), 640,480, OF_IMAGE_COLOR);
		colorImg.update();
		unsigned char * colorPixels = colorImg.getPixels();

		
        imgd.setFromPixels(kinectSource->getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
		unsigned char * pixels = imgd.getPixels();
		
		int numPixels = imgd.getWidth() * imgd.getHeight();
		for(int i = 0; i < numPixels; i++) {
			//if(pix[i] < nearThreshold && pix[i] > farThreshold) {
			//invert
			if(pixels[i] < 230 && pixels[i] > 100) {
				pixels[i] = 0;
			} else {
				pixels[i] = 255;
			}
		}
		
		//imgd.flagImageChanged();

		imgd.update();
		pixels = imgd.getPixels();
		
		//if (trailing == 10){
        if (trailing != -1){
			particles.clear();
			trailing = 0;
			//if the app performs slowly raise this number
			sampling = 1 ; 
			
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
					
					if (grey < 200) {
						particles.push_back( Particle ( ofPoint ( x + xOffset , y + yOffset ) , grey, color ) ) ;  
					}
				}
			}
		} else {
			trailing++;
		}
		
        
    }
	updateParticles();
}

void testApp::draw()
{	
    ofPushMatrix(); 
    
    //draws a star
    //ofSetPolyMode(OF_POLY_WINDING_NONZERO);
    glBegin(GL_LINE);
    for (int y=0;y<640;y+=5){
        ofBeginShape();
        for (int x=0;x<480*3;x+=3){
            int pix1 = (y*640*3) + x;
            int pix2 = (y*640*3) + (x+1);
            int pix3 = (y*640*3) + (x+2);
            glColor3f(pix1,pix2,pix3);
            glVertex3f(x*2,y*2, 1);
        }
        ofEndShape();
    }
    glEnd();
    ofPopMatrix();
    
//    cout << pulse << endl;
//    ofTranslate(10,100,pulse);
//    
//    glClearColor(0, 0, 0, 1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    
//	//Begin the openGL Drawing Mode
//    glBegin(GL_POINTS);
//
//    //Triangles look Cool too 
//    //glBegin(GL_TRIANGLES);
//	
//    //glBegin(GL_LINE_LOOP);
//
//    //Create an iterator to cycle through the vector
//    std::vector<Particle>::iterator p ; 
//    for ( p = particles.begin() ; p != particles.end() ; p++ )
//    {
//        //glColor3ub((unsigned char)p->grey,(unsigned char)p->grey,(unsigned char)p->grey);
//        //glColor3ub((unsigned char)p->color.r,(unsigned char)p->color.g,(unsigned char)p->color.b);
//		glColor4ub((unsigned char)p->color.r,(unsigned char)p->color.g,(unsigned char)p->color.b,(unsigned char)p->grey);
//        glVertex3f(p->position.x, p->position.y , 0 );
//    }
//    
//    glEnd();
//    ofPopMatrix();  
//
    
    ofSetColor ( 255 , 255 , 255 ) ;
    
    string output = "S :: Springs on/off : " + ofToString(springEnabled) +
    
    "\n C :: CursorMode repel/attract " + ofToString( cursorMode ) +
    
    "\n # of particles : " + ofToString( numParticles ) +
    
    " \n fps:" +ofToString( ofGetFrameRate() ) ;
    
    ofDrawBitmapString(output ,20,666);
	imgd.draw(0,0,100,150);

>>>>>>> Stashed changes
}

ofPolyline testApp::toOf(const ofxCvBlob& blob) {
	ofPolyline poly;
	poly.addVertexes(blob.pts);
	return poly;
}

bool testApp::init_kinect()
{
	//kinect.enableCalibrationUpdate(false);
	kinect.enableDepthNearValueWhite(false);
	
	kinect.init(false, true, true);
	kinect.setVerbose(false);
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
	//orgb_pix = kinect.getPixels();
	
	return true;
}

void testApp::keyPressed(int key)
{
	pressed[key] = true;
	
	switch (key)
	{		
		case 'm':
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
			break;		
			
		//case 'o':
			//debug_depth_texture = !debug_depth_texture;
			//kinect.toggleCalibrationUpdate();
			//break;
	}
	
	if (key == 'd')
	{
		camluc.toggle_debug();
	}
	if (key == 'f')
	{
		mesh->print();
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
		radio_on = true;
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
LineSegment testApp::constrainLineToPolygon(LineSegment* ls, ofPolyline* poly, bool &success) {
    
    success = true; // assume we can do it
    
    // this is the default line we will return. it's just a copy of the one we are testing
    LineSegment theLs = LineSegment(ls->p0,ls->p1);
	
    // poly isn't big enough, so just return original line
    if(poly->size() < 3) {
        // cout << "Poly has less than 3 points, so we can't really intersect." << endl;
        success = false;
        return theLs;
    }
    
    // check to see if either end of our line is actually INSIDE the polygon
    bool p0Inside = poly->inside(ls->p0);
    bool p1Inside = poly->inside(ls->p1);
    
    // get a list of the poly verts
    vector<ofPoint> verts = poly->getVertices();
    
    // create a vector to collect all of the intersections
    vector<ofPoint> intersections;
    
    // imagine the poly as a series of connected line segments (end to end).
    // cycle through those line segments and and test to see if our line 
    // intersects with any of them.
    // when an intersection is located, save it to the array of intersections
    for(int i = 0; i < verts.size(); i++) {
        int nextI = i != (verts.size() - 1) ? i + 1 : 0;
        LineSegment polySegment = LineSegment(verts[i],verts[nextI]);
        ofVec2f intersection;
        IntersectResult result = polySegment.Intersect(theLs, intersection);
        // we are only interested in doing something with intersection points
        if(result == INTERESECTING) {
            intersections.push_back(intersection);
        }
    }
    
    // now that we have found all of the places where our line intersects the polygon
    // we can figure out what our constrained line should look like.
    // there are a few pathological cases, which are possible to figure out, but
    // a bit more difficult.  basically it can happen when a line passes
    // through a polygon's concavity (generating more than 2 intersections)
    if(intersections.size() > 3) {
        // cout << "Found more than three intersection points ... failing b/c of laziness: " << intersections.size() << endl;

<<<<<<< Updated upstream
        success = false;
    } else if (intersections.size() == 3) {
		success = false;
		
		if (intersections[0] == intersections[1]) {
			// cout << "matched first and second" << endl;
			theLs.p0 = intersections[0];
			theLs.p1 = intersections[2];
			success = true;
		}
	} else if(intersections.size() == 2) {
        theLs.p0 = intersections[0];
        theLs.p1 = intersections[1];
        success = true;
    } else if(intersections.size() == 1) {
        if(p0Inside) {
            theLs.p1 = intersections[0];
            success = true;
        } else if(p1Inside) {
            theLs.p0 = intersections[0];
            success = true;
        } else {
            // cout << "One intersection was found, but neither are inside ... very curious." << endl;
            success = false;
        }
    } else if (intersections.size() == 0) {
		success = false;
	}
    
    // return the line, whether modified or not.
    return theLs;
}
=======
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
>>>>>>> Stashed changes
