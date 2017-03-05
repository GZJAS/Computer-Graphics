//
//  Line.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#ifndef Line_hpp
#define Line_hpp
#include "Color.hpp"
#include "Point.hpp"
#include <cmath>
#include <stdio.h>
#include <array>
#include <vector>
#include <string>

class Line {
public:
    /* data members */
    Point p1;
    Point p2;
    Color color;
    
    /* member functions */
    Line(Point p1, Point p2);
    void lineDDA();
    void lineBres();


}; // end class definition
#endif /* Line_hpp */
