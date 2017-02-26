//
//  threeD.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 2/5/17.
//
//

#include "threeD.hpp"

extern Point lightsource;
extern Point viewsource;
extern Color ambientIntensity;
extern Color lightIntensity;
extern float bigK;
void NDCmapToXY(float &x, float &y);
void NDCmapToYZ(float &z, float &y);
void NDCmapToXZ(float &x, float &z);
using namespace std;
threeD::threeD(std::vector<Point *>points){
    vertices = points;
    id = global_id++;
}


void threeD::Phong(){

    
    for(int i = 0; i < vertices.size(); i++){
        Point p = *vertices[i];
        Point n = *normals[i];
        Point l = normalize(lightsource - p);
        Point v = normalize(viewsource - p);
        Point r = normalize(n * dotProduct(n, l) * 2 - l);
        
        
        Color amb_color = ka * ambientIntensity;
        float dotln = (dotProduct(l, n) < 0) ? 0 : dotProduct(l, n);
        float dotrv = (dotProduct(r, v) < 0) ? 0 : dotProduct(r, v);
        Color diff_color = (lightIntensity / (length(viewsource - p) + bigK)) * (kd * dotln);
        Color spec_color = (lightIntensity / (length(viewsource - p) + bigK)) * (ks * pow(dotrv, ns));
        Color color = amb_color + diff_color + spec_color;
        vertices[i]->color = color;
    }

}

// project onto xy plane
void threeD::drawXY(){
    
    // sort faces by z centroid coordinate in decreasing order
    std::sort(faces.begin(), faces.end(), [](Polygon *poly1, Polygon *poly2){return poly1->centroid.z > poly2->centroid.z;});
    
    for(auto face: faces){
        if (NotJustALine(face, "xy"))
            face->drawPolygon("xy");
    }
    
}

// project onto yz plane
void threeD::drawYZ(){
    
    // sort faces by x centroid coordinate in decreasing order
    std::sort(faces.begin(), faces.end(), [](Polygon *poly1, Polygon *poly2){return poly1->centroid.x > poly2->centroid.x;});
    
    for(auto face: faces){
        if (NotJustALine(face, "yz"))
            face->drawPolygon("yz");
    }
    
    
}

// project onto xz plane
void threeD::drawXZ(){
    
    // sort faces by y centroid coordinate in decreasing order
    std::sort(faces.begin(), faces.end(), [](Polygon *poly1, Polygon *poly2){return poly1->centroid.y > poly2->centroid.y;});
    
    for(auto face: faces){
        if (NotJustALine(face, "xz"))
            face->drawPolygon("xz");
    }
    
}


// remove those polygons whose projection onto a plane results in a line
bool threeD::NotJustALine(Polygon *polygon, std::string plane){
    std::set<Point2D> unique2DPoints;
    std::set<Point2D> unique1;
    std::set<Point2D> unique2;
    
    if(plane == "xy"){
        
        // copy x and y values into vector of Point2D
        for(auto point: polygon->points){
            Point2D pt1, pt2;
            pt1.a = point->x;
            pt2.a = point->y;
            unique1.insert(pt1);
            unique2.insert(pt2);
            
            
        }
        
        
        if(unique1.size() == 1 || unique2.size() == 1){
            return false;
        }

    }
    else if(plane == "yz"){
        // copy y and z values into vector of Point2D
        for(auto point : polygon->points){
            Point2D pt1, pt2;
            pt1.a = point->z;
            pt2.a = point->y;
            unique1.insert(pt1);
            unique2.insert(pt2);
            
        
        }

        if(unique1.size() == 1 || unique2.size() == 1){
            return false;
        }
        
    }
    else if(plane == "xz"){
        // copy x and z values into vector of Point2D
        for(auto point : polygon->points){
            Point2D pt1, pt2;
            pt1.a = point->x;
            pt2.a = point->z;
            unique1.insert(pt1);
            unique2.insert(pt2);
            
        }

        
        if(unique1.size() == 1 || unique2.size() == 1){
            return false;
        }

    }
    return true;
}

