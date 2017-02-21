//
//  threeD.hpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 2/5/17.
//
//

#ifndef threeD_hpp
#define threeD_hpp


#include "Geometry.hpp"
#include "Line.hpp"
#include "Polygon.hpp"
#include "Point2D.hpp"
#include "Color.hpp"
#include <stdio.h>
#include <set>

class threeD : public Geometry {
public:
    std::vector<Edge *> edges;
    std::vector<Polygon *> faces;
    std::vector<Point *> normals;
    
    static Point *lightsource;
    static Point *viewsource;
    static Color ambientlight;
    
    float ka, kd, ks, ns;
    
    Color color;
    
    threeD(std::vector<Point *>);

    void Phong();
    bool NotJustALine(Polygon *polygon, std::string plane);
    
    void drawXY();
    void drawYZ();
    void drawXZ();

};

#endif /* threeD_hpp */
