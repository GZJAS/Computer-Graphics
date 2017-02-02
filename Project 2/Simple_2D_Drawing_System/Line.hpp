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
    float x1, y1, x2, y2;
    int color = 1.0;
    
    
    /* member functions */
    Line(std::vector<std::array<float, 2>> points);
    Line(float x1, float y1, float x2, float y2);
    void updateParameters() override;
    unsigned int code(float x,float y);
    void draw() override;
    void lineDDA();
    void lineBres();
    int clip() override;
}; // end class definition
#endif /* Line_hpp */
