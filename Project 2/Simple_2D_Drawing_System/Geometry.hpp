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
    Point centroid;
    int n, id;
    double color = 1.0;
    
    
    // convert vertices vector into matrix (vector of vectors)
    void convertToMatrix();
    void convertToVector();
    void findCentroid();
    std::vector<std::vector<double>> matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB);
    std::vector<std::vector<double>> create_rot_matrix(Edge *rotaxis, double angle);
    std::vector<std::vector<double>> create_scale_matrix(double Sx, double Sy, double Sz);
    std::vector<std::vector<double>> create_trans_matrix(double x, double y, double z);
    void translate(double x, double y, double z);
    void rotate(Edge *rotaxis, double angle);
    void scale(double Sx, double Sy, double Sz);
};

#endif /* Geometry_hpp */

