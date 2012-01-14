//	Using Camara Lucida's framework
//	www.camara-lucida.com.ar

#include "testApp.h"

void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);

    a = 0;
    alphaPulse = 0;
	
	init_keys();
	debug_depth_texture = true;
	
	grayImage.allocate(kinect.getWidth(), kinect.getHeight());
	depthImage.allocate(kinect.getWidth(), kinect.getHeight());
	colorImg.allocate(kinect.getWidth(), kinect.getHeight());
	
	if (!init_kinect())
		return;
	
	mesh = new cml::Mesh_freenect(raw_depth_pix);
	
	camluc.init(ofToDataPath("camara_lucida/kinect_calibration.yml"),
				ofToDataPath("camara_lucida/projector_calibration.yml"),
				ofToDataPath("camara_lucida/camara_lucida_config.xml"),
				mesh, ofGetWidth(), ofGetHeight(), 1);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_hud, this, &testApp::render_hud);
	
	far = 162;
	near = 120;
	
	currentLine.p0.x = 1;
	currentLine.p0.y = 100;
	currentLine.p1.x = 100;
	currentLine.p1.y = 100;
	
	lines.push_back(currentLine);
	ofLine(currentLine.p0.x, currentLine.p0.y, currentLine.p1.x, currentLine.p1.y);
	
}

void testApp::update()
{
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update();
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
	ofDrawBitmapString("press 'o' to debug camera as a texture \n 'd' to toggle camara lucida debug, then use 'v' to change viewpoint between camera and projector \n mousedrag to rotate, 'z'+mousedrag to zoom, 'x' to reset the debug transformations \n and 'c'+keyup/down to change depth xoffset", 10, 10);

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
	
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

	unsigned char * pix = grayImage.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight()-1;
	
	depthImage.setFromPixels(pix, kinect.width, kinect.height);
	depthImage.flagImageChanged();
	
	for (int i = numPixels; i > 0; i--){
		if (pix[i] > near && pix[i] < far) {
			pix[i] = 255;
		} else {
			pix[i] = 0;
		}
	}
	
	ofScale(1.6, 1.6, 1);
	contourFinder.findContours(grayImage, 500, (340*240)/1, 5, false);	// find holes
	
	for (int i = 0; i < contourFinder.nBlobs; i++){
//		ofPushMatrix();
//		
//		ofxCvBlob blob = contourFinder.blobs[i];
//		
//		for (int k=0; k<blob.nPts; k++) {
//			ofSetColor(255,255,255,1.0);
//			//ofCircle(contourFinder.blobs[i].pts[k].x, contourFinder.blobs[i].pts[k].y,20);
//			
//			//perpendicular lines
//			vector<ofPoint> points = blob.pts; //full of contour points
//			float length = 10; //defines how long the perpendicular lines are.
//
//
//			for(int i = 0; i < points.size()-1; i++){
//				ofVec2f perpendicular = ofVec2f( (points[i+1] - points[i]) ).getPerpendicular() * length;
//				ofLine(points[i], points[i]+perpendicular);
//			}
//			
//		}
//		
//		ofSetColor(255, 0, 0);
//		ofFill();
//		ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
//		
//		ofPopMatrix();

		polyline = toOf(contourFinder.blobs[i]);
		polyline = polyline.getResampledBySpacing(100);	
		polyline.setClosed(true);
		polyline.draw();
		
		
		bool success = true;
		
		LineSegment clippedLine = constrainLineToPolygon(&currentLine, &polyline, success);
		
		if(success) {
			ofPushStyle();
			ofSetLineWidth(6);
			ofSetColor(0,255,0);
			clippedLine.draw();
			ofPopStyle();
		} 
		currentLinePoint = 0;
		
	}
	
	//depthImage.draw(0,h/2,w/2,h);
	//grayImage.draw(0,0,w,h);
	//kinect.getDepthTextureReference().draw(w/2, h/2, w/2+400, h/2+300);	
	//contourFinder.draw(0, 0, w, h);

	//glColor3f(1, 1, 0);
	//ofCircle(800, 200, 100);
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
	}
	if (key == 'y'){
		far--;
	}
	if (key == 'u'){
		near++;
	}
	if (key =='i') {
		near--;
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
        cout << "Poly has less than 3 points, so we can't really intersect." << endl;
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
	cout << "number of intersections: " << intersections.size() << endl;
    if(intersections.size() > 3) {
        cout << "Found more than three intersection points ... failing b/c of laziness: " << intersections.size() << endl;

        success = false;
    } else if (intersections.size() == 3) {
		success = false;
		
		if (intersections[0] == intersections[1]) {
			cout << "matched first and second" << endl;
			theLs.p0 = intersections[0];
			theLs.p1 = intersections[2];
			cout << theLs.p0 << ", " << theLs.p1 << endl;
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
            cout << "One intersection was found, but neither are inside ... very curious." << endl;
            success = false;
        }
    } else if (intersections.size() == 0) {
		success = false;
	}
    
    // return the line, whether modified or not.
	cout << theLs.p0 << " + " << theLs.p1 << ", " << success << endl;
    return theLs;
}