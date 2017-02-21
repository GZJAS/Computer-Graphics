//
//  threeD.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 2/5/17.
//
//

#include "threeD.hpp"

void NDCmapToXY(float &x, float &y);
void NDCmapToYZ(float &z, float &y);
void NDCmapToXZ(float &x, float &z);

threeD::threeD(std::vector<Point *>points){
    vertices = points;
    id = global_id++;
    convertToMatrix();
}


void threeD::Phong(){
    
    for(int i = 0; i < vertices.size(); i++){
        
        Point H;
        H.x = lightsource->x + viewsource->x;
        H.y = lightsource->y + viewsource->y;
        H.z = lightsource->z + viewsource->z;
        float magnitude = sqrt(pow(H.x, 2) + pow(H.y, 2) + pow(H.z, 2));
        
        H.x /= magnitude;
        H.y /= magnitude;
        H.z /= magnitude;
        
        // Phong formula = ka*Ia + kd*IL*(N . L) + ks*IL*(N . H)^ns
        // red
        float ambient_color = ka * ambientlight.r;
        float diffuse_color = kd * lightsource->color.r * dotProduct(*normals[i], *lightsource);
        float specular_color = ks * lightsource->color.r * pow(dotProduct(*normals[i], H), ns);
        
        vertices[i]->color.r = ambient_color + diffuse_color + specular_color;
        
        // green
        ambient_color = ka * ambientlight.g;
        diffuse_color = kd * lightsource->color.g * dotProduct(*normals[0], *lightsource);
        specular_color = ks * lightsource->color.g * pow(dotProduct(*normals[i], H), ns);
        
        vertices[i]->color.g = ambient_color + diffuse_color + specular_color;

        // blue
        ambient_color = ka * ambientlight.b;
        diffuse_color = kd * lightsource->color.b * dotProduct(*normals[0], *lightsource);
        specular_color = ks * lightsource->color.b * pow(dotProduct(*normals[i], H), ns);
        
        vertices[i]->color.b = ambient_color + diffuse_color + specular_color;
        
    }


}

// remove those polygons whose projection onto a plane results in a line
bool threeD::NotJustALine(Polygon *polygon, std::string plane){
    std::set<Point2D> unique2DPoints;
    
    if(plane == "xy"){
        
        // copy x and y values into vector of Point2D
        for(auto i : polygon->points){
            const Point2D point2D = Point2D(i->x, i->y);
            unique2DPoints.insert(point2D);
        }
           
        // if the size of the two containers are not the same, at least one 2D point is a duplicate, so the plane must be a line
        if(unique2DPoints.size() != polygon->points.size()){
            return false;
        }
    }
    else if(plane == "yz"){
        // copy y and z values into vector of Point2D
        for(auto point : polygon->points){
            const Point2D point2D = Point2D(point->z, point->y);
            unique2DPoints.insert(point2D);
        }

        // if the size of the two containers are not the same, at least one 2D point is a duplicate, so the plane must be a line
        if(unique2DPoints.size() != polygon->points.size()){
            return false;
        }
    }
    else if(plane == "xz"){
        // copy x and z values into vector of Point2D
        for(auto point : polygon->points){
            const Point2D point2D = Point2D(point->x, point->z);
            unique2DPoints.insert(point2D);
        }

        // if the size of the two containers are not the same, at least one 2D point is a duplicate, so the plane must be a line
        if(unique2DPoints.size() != polygon->points.size()){
            return false;
        }
    }
    
    return true;
}

// project onto xy plane
void threeD::drawXY(){
  
    Phong();
    
    for(auto face: faces){
            face->findCentroid();
    }
    
    // sort faces by z centroid coordinate in decreasing order
    std::sort(faces.begin(), faces.end(), [](Polygon *poly1, Polygon *poly2){return poly1->centroid.z > poly2->centroid.z;});
    
    for(auto face: faces){
        if (NotJustALine(face, "xy"))
            face->drawPolygon("xy");
    }
    
}

// project onto yz plane
void threeD::drawYZ(){
   
    Phong();
    
    for(auto face: faces){
        face->findCentroid();
    }
    
    // sort faces by x centroid coordinate in decreasing order
    std::sort(faces.begin(), faces.end(), [](Polygon *poly1, Polygon *poly2){return poly1->centroid.x > poly2->centroid.x;});
    
    for(auto face: faces){
        if (NotJustALine(face, "yz"))
            face->drawPolygon("yz");
    }


}

// project onto xz plane
void threeD::drawXZ(){
    Phong();
    
    for(auto face: faces){
        face->findCentroid();
    }
    
    // sort faces by y centroid coordinate in decreasing order
    std::sort(faces.begin(), faces.end(), [](Polygon *poly1, Polygon *poly2){return poly1->centroid.y > poly2->centroid.y;});
    
    for(auto face: faces){
        if (NotJustALine(face, "xz"))
            face->drawPolygon("xz");
    }

}


