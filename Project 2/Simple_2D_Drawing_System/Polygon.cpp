//
//  Polygon.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Polygon.hpp"

extern float xmin, xmax, ymin, ymax;
extern int pid;
void setPixelXY(int x, int y, double c);

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
Polygon::Polygon(std::vector<std::array<float, 2>> points){
    for(auto it = points.begin(); it != points.end(); it++){
        Point *pt = new Point();
        pt->x = (*it)[0];
        pt->y = (*it)[1];
        vertices.push_back(pt);
        xc.push_back((*it)[0]);
        yc.push_back((*it)[1]);
        
    }
    n = (int)vertices.size();
    id = pid++;
    name = "Polygon";
    
}

void Polygon::draw() {
    Polygon::drawPolygon();
}

void Polygon::updateParameters() {
    xc.clear();
    yc.clear();
    for(auto it = vertices.begin(); it != vertices.end(); it++){
        xc.push_back((*it)->x);
        yc.push_back((*it)->y);
    }
    n = (int)vertices.size();
}

// creates edge buckets from the given edges
std::list<Bucket*> Polygon::createEdges () {
    int startIndex = n - 1;
    int yMax;
    int yMin;
    int initialX;
    int sign;
    int dX;
    int dY;
    Point v1;
    Point v2;
    Point tmp;
    
    std::list<Bucket*> edgeTable;   // list that contains all of the edges that make up the figure
    
    // Create the edge buckets and place in tempEdgeTable
    for (int i = 0; i < n; i++) {
        v1 = { static_cast<float>(xc[startIndex]), static_cast<float>(yc[startIndex]) };
        v2 = { static_cast<float>(xc[i]), static_cast<float>(yc[i]) };
        
        // Check and swap vertices if not in left to right order
        if (v2.x < v1.x) {
            tmp = v1;
            v1 = v2;
            v2 = tmp;
        }
        yMax = (v1.y > v2.y) ? v1.y : v2.y;
        yMin = (v1.y < v2.y) ? v1.y : v2.y;
        initialX = (v1.y < v2.y) ? v1.x : v2.x;
        sign = ((v2.y - v1.y) < 0) ? -1 : 1;
        dX = abs(v2.x - v1.x);
        dY = abs(v2.y - v1.y);
        
        if (dY != 0) {
            Bucket *freshBucket = new Bucket;
            
            *freshBucket = {
                yMax,
                yMin,
                initialX,
                sign,
                dX,
                dY,
                0
            };
            
            
            edgeTable.push_back(freshBucket);
        }
        startIndex = i;
    }
    return edgeTable;
}

// Given the edge table of the polygon, fill the polygons
void Polygon::processEdgeTable (std::list<Bucket*> edgeTable) {
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
            
            for (int x = b1.x; x < b2.x; x++) {
                setPixelXY(x, scanLine, color);
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
void Polygon::drawPolygon() {
    // Create Edge Table
    std::list<Bucket*> finalEdgeTable = createEdges();
    
    // Sort edges by minY
    finalEdgeTable.sort(minYCompare);
    
    // Process Edge Table and draw Polygon
    processEdgeTable(finalEdgeTable);
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


void Polygon::clipLeft(Point *p1, Point *p2){
    float dy = p2->y - p1->y;
    float dx = p2->x - p1->x;
    float m;
    if(fabs(dx) > 0)
        m = dy/dx;
    else
        m = 100000;
    Point *intr = new Point();
    intr->x = xmin;
    intr->y = round(m * (xmin - p1->x) + p1->y);
    
    // in to in
    if(p1->x >= xmin && p2->x >= xmin){
        tmp.push_back(p2);
    }
    
    // out to in
    else if(p1->x < xmin && p2->x >= xmin){
        tmp.push_back(intr);
        tmp.push_back(p2);
    }
    
    // in to out
    else if(p1->x >= xmin && p2->x < xmin){
        tmp.push_back(intr);
    }
    
    // out to out
    // nothing was changed
    
}

void Polygon::clipTop(Point *p1, Point *p2){
    float dy = p2->y - p1->y;
    float dx = p2->x - p1->x;
    float m;
    if(fabs(dx) > 0)
        m = dy/dx;
    else
        m = 100000;
    Point *intr = new Point();
    intr->y = ymax;
    intr->x = round((1/m) * (ymax - p1->y) + p1->x);
    
    
    // in to in
    if(p1->y <= ymax && p2->y <= ymax){
        tmp.push_back(p2);
    }
    
    // out to in
    else if(p1->y > ymax && p2->y <= ymax){
        tmp.push_back(intr);
        tmp.push_back(p2);
    }
    
    // in to out
    else if(p1->y <= ymax && p2->y > ymax){
        tmp.push_back(intr);
    }
    
    // out to out
    // nothing was changed
}

void Polygon::clipRight(Point *p1, Point *p2){
    float dy = p2->y - p1->y;
    float dx = p2->x - p1->x;
    float m;
    if(fabs(dx) > 0)
        m = dy/dx;
    else
        m = 100000;
    Point *intr = new Point();
    intr->x = xmax;
    intr->y = round(m * (xmax - p1->x) + p1->y);
    
    // in to in
    if(p1->x <= xmax && p2->x <= xmax){
        tmp.push_back(p2);
    }
    
    // out to in
    else if(p1->x > xmax && p2->x <= xmax){
        tmp.push_back(intr);
        tmp.push_back(p2);
    }
    
    // in to out
    else if(p1->x <= xmax && p2->x > xmax){
        tmp.push_back(intr);
    }
    
    // out to out
    // nothing was changed
    
    
    
}

void Polygon::clipBottom(Point *p1, Point *p2){
    float dy = p2->y - p1->y;
    float dx = p2->x - p1->x;
    float m;
    if(fabs(dx) > 0)
        m = dy/dx;
    else
        m = 100000;
    Point *intr = new Point();
    intr->y = ymin;
    intr->x = round((1/m) * (ymin - p1->y) + p1->x);
    
    // in to in
    if(p1->y >= ymin && p2->y >= ymin){
        tmp.push_back(p2);
    }
    
    // out to in
    else if(p1->y < ymin && p2->y >= ymin){
        tmp.push_back(intr);
        tmp.push_back(p2);
    }
    
    // in to out
    else if(p1->y >= ymin && p2->y < ymin){
        tmp.push_back(intr);
    }
    
    // out to out
    // do nothing
    
}

int Polygon::clip() {
    results.clear();
    tmp.clear();
    unchanged = false;
    
    Point *p1;
    Point *p2;
    
    results = vertices;
    
    // clip with respect to Left edge
    for(int i = 0; i < results.size() - 1; i++){
        p1 = results[i];
        p2 = results[i+1];
        clipLeft(p1, p2);
    }
    p1 = results[results.size() - 1];
    p2 = results[0];
    clipLeft(p1, p2);
    
    
    if (!tmp.empty()){
        results = tmp;
        tmp.clear();
    }
    else {
        return -1;
    }
    
    // clip with respect to Top edge
    for(int i = 0; i < results.size() - 1; i++){
        p1 = results[i];
        p2 = results[i+1];
        clipTop(p1, p2);
    }
    p1 = results[results.size() - 1];
    p2 = results[0];
    clipTop(p1, p2);
    
    
    if (!tmp.empty()){
        results = tmp;
        tmp.clear();
    }
    else {
        return -1;
    }
    
    // clip with respect to Right edge
    for(int i = 0; i < results.size() - 1; i++){
        p1 = results[i];
        p2 = results[i+1];
        clipRight(p1, p2);
    }
    p1 = results[results.size() - 1];
    p2 = results[0];
    clipRight(p1, p2);
    
    
    
    if (!tmp.empty()){
        results = tmp;
        tmp.clear();
    }
    else {
        return -1;
    }
    
    // clip with respect to Bottom edge
    for(int i = 0; i < results.size() - 1; i++){
        p1 = results[i];
        p2 = results[i+1];
        clipBottom(p1, p2);
    }
    p1 = results[results.size() - 1];
    p2 = results[0];
    clipBottom(p1, p2);
    
    if(!tmp.empty()){
        results = tmp;
    }
    else {
        return -1;
    }
    
    
    if(!results.empty()){
        vertices.clear();
        vertices = results;
        updateParameters();
        return 1;
    }
    return -1;
}

