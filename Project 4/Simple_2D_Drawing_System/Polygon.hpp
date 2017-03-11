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

enum Curve {
    Bezier,
    Spline
};

// Polygon class
class Polygon{
public:
    std::vector<Point>bezier_pts;
    std::vector<Point>control_pts;
    std::vector<Point>boor_pts;
    std::vector<float>knots;
    int id, k = 0;
    Color color;
    Curve type;

    
    /* member functions */
    // constructor
    Polygon(std::vector<Point> points);
    void Setup();
    void Draw();
    
    Point getBzrPt(float t);
    Point getBoorPt(float u, int I);
    
    void insertControlPt(int i, Point pt);
    void modifyControlPt(int i, Point newpt);
    void deleteControlPt(int i);

    void deCasteljau();
    void deBoor();
    
    int WhichInterval(double x, int start);
    void createKnotVector();
    bool modifyKnot(int i, float f);
    void setKValue(int newk);
    void updateCurve();
    
}; // end class definition


#endif /* Polygon_hpp */
