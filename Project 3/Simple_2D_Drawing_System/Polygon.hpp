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
#include "Point2D.hpp"
#include <stdio.h>
#include <vector>
#include <list>
#include <iterator>
#include <array>

// Polygon class
class Polygon : public Geometry {
public:
    /* data member */
    Point centroid;
    std::vector<Point *> points;
    
    /* member functions */
    // constructor
    Polygon(std::vector<Point *> pts);
    void findCentroid();
    std::list<Bucket*> createEdges(std::string plane);
    void processEdgeTable (std::list<Bucket*>, std::string plane);
    
    
    void drawPolygon(std::string plane);
    void drawOutlines();
    
}; // end class definition


#endif /* Polygon_hpp */
