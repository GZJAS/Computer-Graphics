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
};

#endif /* Point_h */
