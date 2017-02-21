//
//  Geometry.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Geometry.hpp"


// convert vertices vector into matrix (vector of vectors)
void Geometry::convertToMatrix(){
    std::vector<double>x_row;
    std::vector<double>y_row;
    std::vector<double >z_row;
    for(auto vertex : vertices){
        x_row.push_back(vertex->x);
        y_row.push_back(vertex->y);
        z_row.push_back(vertex->z);
    }
    
    std::vector<double >bottom_row;
    for(int i = 0; i < vertices.size(); i++){
        bottom_row.push_back(1);
    }
    
    matrix.push_back(x_row);
    matrix.push_back(y_row);
    matrix.push_back(z_row);
    matrix.push_back(bottom_row);
}

// dot product on two vectors (represented as points)
float Geometry::dotProduct(Point p1, Point p2){
    float sum = 0;
    sum += p1.x * p2.x;
    sum += p1.y * p2.y;
    sum += p1.z * p2.z;
    return sum;
}

// find centroid of Shape
void Geometry::findCentroid(){
    double sumx = 0, sumy = 0, sumz = 0;
    
    for (auto i : vertices){
        sumx += i->x;
        sumy += i->y;
        sumz += i->z;
    }
    
    centroid.x = sumx/vertices.size();
    centroid.y = sumy/vertices.size();
    centroid.z = sumz/vertices.size();
}

// multiply two matrices together
std::vector<std::vector<double>> Geometry::matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB){
    
    std::vector<std::vector<double>> matrixC;
    int r1 = (int)matrixA.size();
    int c1 = (int)matrixA[0].size();
    int c2 = (int)matrixB[0].size();
    
    std::vector<double>row;
    
    for(int i = 0; i < r1; i++){
        row.push_back(0);
    }
    for(int i = 0; i < matrixA.size(); i++){
        matrixC.push_back(row);
    }
    
    for(int i = 0; i < r1; i++){
        for (int j = 0; j < c2; j++){
            for(int k = 0; k < c1; k++){
                matrixC.at(i).at(j) += matrixA.at(i).at(k) * matrixB.at(k).at(j);
            }
        }
    }
    
    return matrixC;
}



