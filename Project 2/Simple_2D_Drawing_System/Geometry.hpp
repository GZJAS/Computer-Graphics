//
//  Geometry.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#ifndef Geometry_hpp
#define Geometry_hpp

#include <stdio.h>
#include "Point.hpp"
#include "Bucket.hpp"
#include "Geometry.hpp"
#include <iostream>
#include <vector>
#include <cmath>

// Base class that all shapes will derive from
class Geometry {
public:
    std::vector<Point *> vertices;
    std::vector<std::vector<double>> matrix;
    std::string name;
    
    Point centroid;
    int n, id;
    
    // overridden functions in derived classes
    virtual void draw() = 0;
    virtual int clip() = 0;
    virtual void updateParameters() = 0;
    
    
    // convert vertices vector into matrix (vector of vectors)
    void convertToMatrix();
    void convertToVector();
    void findCentroid();
    std::vector<std::vector<double>> matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB);
    std::vector<std::vector<double>> create_rot_matrix(double alpha);
    std::vector<std::vector<double>> create_scale_matrix(double alpha, double beta);
    std::vector<std::vector<double>> create_trans_matrix(double x, double y);
    void translate(int x, int y);
    void rotate(double alpha);
    void scale(double alpha, double beta);
};

#endif /* Geometry_hpp */

