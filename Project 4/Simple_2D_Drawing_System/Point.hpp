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
    
    Point &operator=(const Point &p){
        this->x = p.x;
        this->y = p.y;
        return *this;
    }
    
    bool operator==(const Point &p){
        if(this->x == p.x && this->y == p.y){
            return true;
        }
        else {
            return false;
        }
    }

};
#endif /* Point_h */
