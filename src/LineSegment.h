#pragma once

#include "ofMain.h"

enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERESECTING, INTERESECTING };


class LineSegment
{
public:
    ofVec2f p0;
    ofVec2f p1;
    
    LineSegment() {
        p0 = ofVec2f();
        p1 = ofVec2f();
    }
    
    LineSegment(const ofVec2f& begin, const ofVec2f& end)
    : p0(begin), p1(end) {}
    
    
    void draw() {
        ofLine(p0, p1);
    }
	
	void print_me() {
		ofLog(OF_LOG_VERBOSE, "%i", p0.x);
	}
    
    IntersectResult Intersect(const LineSegment& other_line, ofVec2f& intersection)
    {
        float denom = ((other_line.p1.y - other_line.p0.y)*(p1.x - p0.x)) -
        ((other_line.p1.x - other_line.p0.x)*(p1.y - p0.y));
        
        float nume_a = ((other_line.p1.x - other_line.p0.x)*(p0.y - other_line.p0.y)) -
        ((other_line.p1.y - other_line.p0.y)*(p0.x - other_line.p0.x));
        
        float nume_b = ((p1.x - p0.x)*(p0.y - other_line.p0.y)) -
        ((p1.y - p0.y)*(p0.x - other_line.p0.x));
        
        if(denom == 0.0f)
        {
            if(nume_a == 0.0f && nume_b == 0.0f)
            {
                return COINCIDENT;
            }
            return PARALLEL;
        }
        
        float ua = nume_a / denom;
        float ub = nume_b / denom;
        
        if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
        {
            // Get the intersection point.
            intersection.x = p0.x + ua*(p1.x - p0.x);
            intersection.y = p0.y + ua*(p1.y - p0.y);
            
            return INTERESECTING;
        }
        
        return NOT_INTERESECTING;
    }
};