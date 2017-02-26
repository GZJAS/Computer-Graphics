//
//  Geometry.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Geometry.hpp"


// dot product on two vectors (represented as points)
float Geometry::dotProduct(Point p1, Point p2){
    float sum = 0;
    sum += p1.x * p2.x;
    sum += p1.y * p2.y;
    sum += p1.z * p2.z;
    return sum;
}

Point Geometry::normalize(const Point &p){
    Point newpoint;
    float magnitude = length(p);
    newpoint.x = p.x / magnitude;
    newpoint.y = p.y / magnitude;
    newpoint.z = p.z / magnitude;
    return newpoint;
}

// cross product of two vectors
Point Geometry::crossProduct(Point p1, Point p2){
    // a x b = <a2b3 - a3b2, a3b1 - a1b3, a1b2 - a2b1>
    Point newpoint;
    newpoint.x = p1.y * p2.z - p1.z * p2.y;
    newpoint.y = p1.z * p2.x - p1.x * p2.z;
    newpoint.z = p1.x * p2.y - p1.y * p2.x;
    return newpoint;
}

float Geometry::length(Point p){
    return sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2));
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


