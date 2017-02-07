//
//  Line.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#ifndef Line_hpp
#define Line_hpp

#include "Geometry.hpp"
#include <stdio.h>
#include <array>

class Line : public Geometry {
public:
    /* data members */
    float x1, y1, x2, y2, z1, z2;
    
    
    /* member functions */
    Line(Edge *edge);
    Line(float x1, float y1, float x2, float y2, float z1, float z2);
    void updateParameters() ;
    unsigned int code(float x,float y);
    void draw() ;
    void lineDDA(float _x1, float _y1, float _x2, float _y2, std::string plane);
    void lineBres(float _x1, float _y1, float _x2, float _y2, std::string plane);
    int clip() ;
}; // end class definition
#endif /* Line_hpp */
