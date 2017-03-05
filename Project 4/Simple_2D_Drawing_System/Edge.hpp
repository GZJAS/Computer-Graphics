//
//  Edge.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 2/5/17.
//
//
#include "Point.hpp"

#ifndef Edge_h
#define Edge_h
struct Edge {
    Point p1;
    Point p2;
    
    Edge(Point p1, Point p2) : p1(p1), p2(p2) {}
};

#endif /* Edge_h */
