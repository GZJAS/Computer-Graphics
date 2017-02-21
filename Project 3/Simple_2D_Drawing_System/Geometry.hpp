//
//  Geometry.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#ifndef Geometry_hpp
#define Geometry_hpp
#include <cmath>
#include <stdio.h>
#include "Point.hpp"
#include "Edge.hpp"
#include "Bucket.hpp"
#include "Geometry.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

static int global_id = 0.0;

// Base class that all shapes will derive from
class Geometry {
public:
    std::vector<Point *> vertices;
    std::vector<std::vector<double>> matrix;
    std::string name;
    int id;
    double color = 1.0;
    Point centroid;
    
    
    void findCentroid();
    
    // convert vertices vector into matrix (vector of vectors)
    void convertToMatrix();
    float dotProduct(Point p1, Point p2);
    std::vector<std::vector<double>> matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB);
};

#endif /* Geometry_hpp */

