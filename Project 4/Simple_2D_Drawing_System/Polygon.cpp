//
//  Polygon.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Polygon.hpp"
extern int global_id;
extern int resolution;


// constructor
Polygon::Polygon(std::vector<Point > points){
    control_pts = points;
    numPoints = static_cast<int>(control_pts.size());
    id = global_id++;
}

// calculate Bezier point given t
Point Polygon::getBzrPt(float t){
    Point pt;
    std::vector<Point>tmp = control_pts;
    int i = numPoints - 1;
    while(i > 0){
        for(int k = 0; k < i; k++){
            tmp.at(k) = tmp.at(k) * (1 - t) + tmp.at(k+1) * t;
            
        }
        i--;
    }
    
    return tmp.at(0);
    
}


void Polygon::deCasteljau(){
    bezier_pts.clear();
    
    double step = ((double)1)/(resolution - 1);
    
    // calculate all Bezier points
    for(float i = 0; i <= 1; i += step ){

        Point pt = getBzrPt(i);
        bezier_pts.push_back(pt);
    }
    
    
    // create lines
    for(int i = 0; i < bezier_pts.size() - 1; i++){
        Line line(bezier_pts.at(i), bezier_pts.at(i+1));
        line.color = color;
        line.lineDDA();
    }
    
    
}


