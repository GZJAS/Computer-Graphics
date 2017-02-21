//
//  Vertex.h
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/18/17.
//
//

#ifndef Vertex_h
#define Vertex_h
#include "Point2D.hpp"
/*
 Bucket struct to hold edge information
 */
struct Bucket {
    int yMax;       // Maximum Y position of the edge
    int yMin;       // Minimum Y position of the edge
    int x;          // The current x position along the scan line, initially start at the same point as the yMin of the edge
    int sign;       // The sign of the edge's slope
    int dX;         // The absolute delta x b/w edge's vertex points
    int dY;         // The absolute delta y b/w edge's vertex points
    double sum;     // Initiated to 0. Used as the scan lines are being filled to x to the next position
    Point2D p1;
    Point2D p2;
    
};

#endif /* Vertex_h */
