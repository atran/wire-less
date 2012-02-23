//	Using Camara Lucida's framework
//	www.camara-lucida.com.ar

#include "testApp.h"

void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(30);
	
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
		
	//serial
	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	serial.setup("/dev/cu.usbmodemfa131",57600); 
}

void testApp::update()
{
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update();
		
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

	//depthImage.draw(0,h/2,w/2,h);
	grayImage.draw(0,0,w,h);
	//kinect.getDepthTextureReference().draw(w/2, h/2, w/2+400, h/2+300);	
	//contourFinder.draw(0, 0, w, h);

	//glColor3f(1, 1, 0);
	//ofCircle(800, 200, 100);
}

bool testApp::init_kinect()
{
	//kinect.enableCalibrationUpdate(false);
	kinect.enableDepthNearValueWhite(false);
	
	kinect.init(false, true, true);
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