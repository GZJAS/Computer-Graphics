//
//  threeD.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 2/5/17.
//
//

#ifndef threeD_hpp
#define threeD_hpp

#include <stdio.h>
#include "Geometry.hpp"

class threeD : public Geometry {
public:
    std::vector<Edge *> edges;
    
    
    threeD(std::vector<Edge *>edges, std::vector<Point *>points);

    void drawXY();
    void drawYZ();
    void drawXZ();
    
    void draw();
    int clip();
    void updateParameters();
    
    
};

#endif /* threeD_hpp */
