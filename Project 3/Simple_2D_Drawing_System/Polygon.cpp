//
//  Polygon.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Polygon.hpp"

extern float xmin, xmax, ymin, ymax;
void setPixelXY(int x, int y, Color color);
void setPixelYZ(int x, int y, Color color);
void setPixelXZ(int x, int y, Color color);

using namespace std;

// Compares the yMins of the given buckets parameters
bool minYCompare (Bucket* edge1, Bucket* edge2) {
    return edge1->yMin < edge2->yMin;
}

// Compares the Xs of the given buckets parameters
bool xCompare (Bucket* edge1, Bucket* edge2) {
    if (edge1->x < edge2->x) {
        return true;
    } else if (edge1->x > edge2->x) {
        return false;
    } else {
        return ((edge1->dX / edge1->dY) < (edge2->dX / edge2->dY));
    }
}

// constructor
Polygon::Polygon(std::vector<Point *> pts){
    points = pts;
}


// find centroid of Polygon
void Polygon::findCentroid(){
    double sumx = 0, sumy = 0, sumz = 0;
    
    for (auto i : points){
        sumx += i->x;
        sumy += i->y;
        sumz += i->z;
    }
    
    centroid.x = sumx/points.size();
    centroid.y = sumy/points.size();
    centroid.z = sumz/points.size();
}


// creates edge buckets from the given edges
std::list<Bucket*> Polygon::createEdges (std::string plane) {
    int startIndex = (int)points.size() - 1;
    int yMax;
    int yMin;
    int initialX;
    int sign;
    int dX;
    int dY;
    
    Point2D v1;
    Point2D v2;
    Point2D tmp;

    std::list<Bucket*> edgeTable;   // list that contains all of the edges that make up the figure
    
    // Create the edge buckets and place in tempEdgeTable
    for (int i = 0; i < points.size(); i++) {
        if (plane == "xy"){
            v1.a = points.at(startIndex)->x;
            v1.b = points.at(startIndex)->y;
            v1.color = points.at(startIndex)->color;
            v2.a = points.at(i)->x;
            v2.b = points.at(i)->y;
            v2.color = points.at(i)->color;

        }
        else if (plane == "yz"){
            v1.a = points.at(startIndex)->z;
            v1.b = points.at(startIndex)->y;
            v1.color = points.at(startIndex)->color;
            v2.a = points.at(i)->z;
            v2.b = points.at(i)->y;
            v2.color = points.at(i)->color;
            
        }
        else if(plane == "xz"){
            v1.a = points.at(startIndex)->x;
            v1.b = points.at(startIndex)->z;
            v1.color = points.at(startIndex)->color;
            v2.a = points.at(i)->x;
            v2.b = points.at(i)->z;
            v2.color = points.at(i)->color;
        }
        
        // Check and swap vertices if not in left to right order
        if (v2.a < v1.a) {
            tmp = v1;
            v1 = v2;
            v2 = tmp;
        }
        yMax = (v1.b > v2.b) ? v1.b : v2.b;
        yMin = (v1.b < v2.b) ? v1.b : v2.b;
        initialX = (v1.b < v2.b) ? v1.a : v2.a;
        sign = ((v2.b - v1.b) < 0) ? -1 : 1;
        dX = abs(v2.a - v1.a);
        dY = abs(v2.b - v1.b);
        
        
        // swap vertices if v2 has higher y
        if (v2.b < v1.b){
            tmp = v2;
            v2 = v1;
            v1 = tmp;
        }
        
        if (dY != 0) {
            Bucket *freshBucket = new Bucket;
            
            *freshBucket = {
                yMax,
                yMin,
                initialX,
                sign,
                dX,
                dY,
                0,
                v1,
                v2
            };
            
            
            edgeTable.push_back(freshBucket);
        }
        startIndex = i;
    }
    return edgeTable;
}

// Given the edge table of the polygon, fill the polygons
void Polygon::processEdgeTable (std::list<Bucket*> edgeTable, std::string plane) {
    int scanLine = edgeTable.front()->yMin;
    Bucket b1;
    Bucket b2;
    std::list<Bucket*> activeList;
    
    while (!edgeTable.empty()) {
        // Remove edges from active list if y == ymax
        if (!activeList.empty()) {
            for (auto i = activeList.begin(); i != activeList.end();) {
                Bucket* curr = *i;
                
                if (curr->yMax == scanLine) {
                    i = activeList.erase(i);
                    edgeTable.remove (curr);
                    delete (curr);
                } else {
                    i++;
                }
            }
        }
        
        // Add edges from ET to AL if y == ymin
        for (auto i = edgeTable.begin (); i != edgeTable.end(); i++) {
            Bucket* edge = *i;
            
            if (edge->yMin == scanLine) {
                activeList.push_back(edge);
            }
        }
        
        
        // Sort by x & slope
        activeList.sort(xCompare);
        
        // Fill polygon pixels
        for (auto i = activeList.begin(); i != activeList.end(); i++) {
            b1 = **i;
            std::advance(i, 1);
            b2 = **i;
        

            Point2D edgepoint1(b1.x, scanLine);
            Point2D edgepoint2(b2.x, scanLine);
            
            //calculate color for edgepoints
            
            edgepoint1.color = b1.p1.color * (((float) b1.yMax - scanLine) / (float) b1.dY) + b1.p2.color * ((float) scanLine / b1.dY);
            
            edgepoint2.color = b2.p1.color * (((float) b2.yMax - scanLine) / (float) b2.dY) + b2.p2.color * ((float) scanLine / b2.dY);
            
            
            
            for (int x = b1.x; x < b2.x; x++) {
                Point2D drawPoint(x, scanLine);
                drawPoint.color = edgepoint1.color * ((float)(b2.x - x) / (float)(b2.x - b1.x))  +  edgepoint2.color * ((float)x / (float)(b2.x - b1.x));

                
                if (plane == "xy"){
                    setPixelXY(x, scanLine, drawPoint.color);
                }
                else if (plane == "yz"){
                    setPixelYZ(x, scanLine, drawPoint.color);
                }
                else if (plane == "xz"){
                    setPixelXZ(x, scanLine, drawPoint.color);
                }

            }
        }
        
        // Increment scanline
        scanLine++;
        
        // Increment the X variables based on the slope
        for (auto i = activeList.begin(); i != activeList.end(); i++) {
            Bucket* edge = *i;
            
            if (edge->dX != 0) {
                edge->sum += edge->dX;
                
                while (edge->sum >= edge->dY) {
                    edge->x += (edge->sign);
                    edge->sum -= edge->dY;
                }
            }
        }
    }
}

// Draw a filled polygon
void Polygon::drawPolygon(std::string plane) {
    
    // Create Edge Table
    std::list<Bucket*> finalEdgeTable = createEdges(plane);
    
    // Sort edges by minY
    finalEdgeTable.sort(minYCompare);
    
    // Process Edge Table and draw Polygon
    processEdgeTable(finalEdgeTable, plane);
}

// Draw outline of polygon
void Polygon::drawOutlines(){
    for(int i = 0; i < vertices.size()-1; i++){
        float x1 = vertices[i]->x;
        float y1 = vertices[i]->y;
        float x2 = vertices[i+1]->x;
        float y2 = vertices[i+1]->y;
        Line line(x1, y1, x2, y2, 0, 0);
        line.lineDDA(x1, y1, x2, y2, "xy");
        
    }
    
    // connect starting vertice to ending vertice
    float x1 = vertices[0]->x;
    float y1 = vertices[0]->y;
    float x2 = vertices[vertices.size()-1]->x;
    float y2 = vertices[vertices.size()-1]->y;
    Line line(x1, y1, x2, y2, 0, 0);
    line.lineDDA(x1, y1, x2, y2, "xy");
}

