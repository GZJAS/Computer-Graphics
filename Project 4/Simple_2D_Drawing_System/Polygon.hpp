//
//  Polygon.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#ifndef Polygon_hpp
#define Polygon_hpp

#include "Line.hpp"
#include "Point.hpp"
#include "Color.hpp"
#include <stdio.h>
#include <vector>
#include <list>
#include <array>
#include <iostream>

using namespace std;

// Polygon class
class Polygon{
public:
    std::vector<Point *>vertices;
    std::vector<Point>bezier_pts;
    std::vector<Point>control_pts;
    int id, numPoints;
    Color color;
    
    /* member functions */
    // constructor
    Polygon(std::vector<Point> points);

    
    Point getBzrPt(float t);
    void deCasteljau();
    
    
    
}; // end class definition


#endif /* Polygon_hpp */
