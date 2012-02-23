#ifndef _PARTICLE
#define _PARTICLE

#include "ofMain.h"

class Particle
{
    public:
        Particle() ;
        Particle( ofPoint _position, char _depth, ofColor _color){
            position = _position;
            depth = _depth;
            color = _color;
            velocity = ofPoint(ofRandom(-5,5), ofRandom(-5,5));
            spawnPoint = _position;
        }
    
    ofPoint position, velocity;
    ofPoint acceleration;
    ofPoint spawnPoint;
    char depth;
    ofColor color;
}

#endif
