//
//  Polygon.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#ifndef Polygon_hpp
#define Polygon_hpp

#include "Geometry.hpp"
#include "Line.hpp"
#include <stdio.h>
#include <vector>
#include <list>
#include <iterator>
#include <array>

// Polygon class
class Polygon : public Geometry {
public:
    /* data members */
    std::vector<int> xc;
    std::vector<int> yc;
    std::vector<Point *>results;
    std::vector<Point *>tmp;
    bool unchanged = false;
    
    /* member functions */
    // constructor
    Polygon(std::vector<std::array<float, 2>> points);
    void draw();
    void updateParameters();
    std::list<Bucket*> createEdges();
    void processEdgeTable (std::list<Bucket*> edgeTable);
    void drawPolygon();
    void drawOutlines();
    void clipLeft(Point *p1, Point *p2);
    void clipTop(Point *p1, Point *p2);
    void clipRight(Point *p1, Point *p2);
    void clipBottom(Point *p1, Point *p2);
    int clip();
    
    
}; // end class definition


#endif /* Polygon_hpp */
