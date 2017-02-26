//
//  Point.h
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/18/17.
//
//

#ifndef Point_h
#define Point_h
#include "Color.hpp"
struct Point {
    float x = 0;
    float y = 0;
    float z = 0;
    Color color;
    
    Point(float x, float y, float z, Color color=Color()) : x(x), y(y), z(z), color(color){}
    
    Point(){}
    
    Point operator+(const Point &p){
        Point newpoint;
        newpoint.x = this->x + p.x;
        newpoint.y = this->y + p.y;
        newpoint.z = this->z + p.z;
        return newpoint;
    }
    
    Point operator-(const Point &p){
        Point newpoint;
        newpoint.x = this->x - p.x;
        newpoint.y = this->y - p.y;
        newpoint.z = this->z - p.z;
        return newpoint;
    }
    
    Point operator*(const float &c){
        Point newpoint;
        newpoint.x = this->x * c;
        newpoint.y = this->y * c;
        newpoint.z = this->z * c;
        
        return newpoint;
    }
    


};

#endif /* Point_h */
