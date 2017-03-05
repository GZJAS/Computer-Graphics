//
//  Point.h
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/18/17.
//
//

#ifndef Point_h
#define Point_h
struct Point {
    float x;
    float y;
    
    Point(float x, float y) : x(x), y(y){}
    
    Point(){}
    
    
    Point operator+(const Point &p){
        return Point(x + p.x, y + p.y);
    }
    
    Point operator-(const Point &p){
        return Point(x - p.x, y - p.y);
    }
    
    Point operator*(const float &c){
        return Point(x * c, y * c);
        
    }
    
    
};
#endif /* Point_h */
