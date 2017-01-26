//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//
#include "Bucket.h"
#include "Point.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <iostream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <cstdlib>
#include <array>
#include <list>

using namespace std;

static int pid = 0;

int window_width = 200;
int window_height = 200;
int PixelBufferSize = window_width * window_height * 3;
float *PixelBuffer;
float xmin = 65, xmax = 90, ymin = 50, ymax = 100;
enum{TOP=0x1,BOTTOM=0x2,RIGHT=0x4,LEFT=0x8};


// function prototypes
void update();
void setPixel(int x, int y, double c);
void clearAllPixels();


// Compares the yMins of the given buckets parameters
bool minYCompare (Bucket* edge1, Bucket* edge2) {
    return edge1->yMin < edge2->yMin;
}

//Compares the Xs of the given buckets parameters
bool xCompare (Bucket* edge1, Bucket* edge2) {
    if (edge1->x < edge2->x) {
        return true;
    } else if (edge1->x > edge2->x) {
        return false;
    } else {
        return ((edge1->dX / edge1->dY) < (edge2->dX / edge2->dY));
    }
}

unsigned int code(float x,float y){
    unsigned int code=0;
    if(y>ymax)
        code|=TOP;
    if(y<ymin)
        code|=BOTTOM;
    if(x>xmax)
        code|=RIGHT;
    if(x<xmin)
        code|=LEFT;
    return code;
}

class Geometry {
public:
    std::vector<Point *> vertices;
    std::vector<int> xc;
    std::vector<int> yc;
    std::vector<std::vector<int>> matrix;
    
    Point centroid;
    
    int n, id;
    
    // convert vertices vector into matrix (vector of vectors)
    void convertToMatrix(){
        vector<int>x_row;
        vector<int>y_row;
        for(auto vertex : vertices){
            x_row.push_back(vertex->x);
            y_row.push_back(vertex->y);
        }
        
        vector<int>bottom_row;
        for(int i = 0; i < n; i++){
            bottom_row.push_back(1);
        }
        
        matrix.push_back(x_row);
        matrix.push_back(y_row);
        matrix.push_back(bottom_row);
    }
    
    void convertToVector(){
        int numPoints = matrix[0].size();
        vertices.clear();
        for(int i = 0; i < numPoints; i++){
            Point *pt = new Point();
            pt->x = matrix[0][i];
            pt->y = matrix[1][i];
            vertices.push_back(pt);
        }
    }
    
    void printMatrix(std::vector<std::vector<int>> someMatrix){
        cout << "row size = " << someMatrix.size() << endl;
        cout << "col size = " << someMatrix[0].size() << endl;
        for(int i = 0; i < someMatrix.size(); i++){

            for(int j = 0; j < someMatrix[i].size(); j++){
                
                cout << someMatrix[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    
    void printVertices(){
        for(auto i: vertices){
            cout << i->x << " " << i->y << endl;
        }
        cout << endl;
    }
    
    // find centroid of polygon
    // http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
    void findCentroid()
    {
        centroid = {0, 0};
        double signedArea = 0.0;
        double x0 = 0.0; // Current vertex X
        double y0 = 0.0; // Current vertex Y
        double x1 = 0.0; // Next vertex X
        double y1 = 0.0; // Next vertex Y
        double a = 0.0;  // Partial signed area
        
        // For all vertices
        for (int i = 0; i < vertices.size(); ++i)
        {
            x0 = vertices[i]->x;
            y0 = vertices[i]->y;
            x1 = vertices[(i+1) % n]->x;
            y1 = vertices[(i+1) % n]->y;
            a = x0*y1 - x1*y0;
            signedArea += a;
            centroid.x += (x0 + x1)*a;
            centroid.y += (y0 + y1)*a;
        }
        
        signedArea *= 0.5;
        centroid.x /= (6.0*signedArea);
        centroid.y /= (6.0*signedArea);
        
    }
    
    // multiply two matrices together
    std::vector<std::vector<int>> matrixMultiply(std::vector<std::vector<int>> matrixA, std::vector<std::vector<int>> matrixB){
        
        std::vector<std::vector<int>> matrixC;
        int r1 = (int)matrixA.size();       // 3
        int c1 = (int)matrixA[0].size();
        int c2 = n;
        
        std::vector<int>row;
        for(int i = 0; i < n; i++){
            row.push_back(0);
        }
        for(int i = 0; i < matrixA.size(); i++){
            matrixC.push_back(row);
        }
        

        for(int i = 0; i < r1; i++){
            for (int j = 0; j < c2; j++){
                for(int k = 0; k < c1; k++){
                    matrixC.at(i).at(j) += matrixA.at(i).at(k) * matrixB.at(k).at(j);
                }
            }
        }
        
        return matrixC;
    }
    
    
    // return a matrix with given rotate parameters
    std::vector<std::vector<int>> create_rot_matrix(int alpha){
        // rot matrix in the form of:
        //        |   cost(alpha)   -sin(alpha)     0   |
        //        |   sin(alpha)    cos(alpha)beta  0   |
        //        |   0                 0           1   |
        std::vector<std::vector<int>> rot_matrix;
        vector<int>row = {static_cast<int>(cos(alpha)), static_cast<int>(-sin(alpha)), 0};
        rot_matrix.push_back(row);
        
        row = {static_cast<int>(sin(alpha)), static_cast<int>(cos(alpha)), 0};
        rot_matrix.push_back(row);
        
        row = {0, 0, 1};
        rot_matrix.push_back(row);
        
        return rot_matrix;
        
    }
    
    // return a matrix with given scaling parameters
    std::vector<std::vector<int>> create_scale_matrix(int alpha, int beta){
        // scale matrix in the form of:
        //        |    alpha   0       0   |
        //        |    0       beta    0   |
        //        |    0       0       1   |
        std::vector<std::vector<int>> scale_matrix;
        vector<int>row = {alpha, 0, 0};
        scale_matrix.push_back(row);
        
        row = {0, beta, 0};
        scale_matrix.push_back(row);
        
        row = {0, 0, 1};
        scale_matrix.push_back(row);
        
        return scale_matrix;
        
    }
    
    // return a matrix with given tranlate parameters
    std::vector<std::vector<int>> create_trans_matrix(int x, int y){
        // translate matrix in the form of:
        //        |     1     0      x    |
        //        |     0     1      y    |
        //        |     0     0      1    |
        std::vector<std::vector<int>> trans_matrix;
        vector<int>row = {1, 0, x};
        trans_matrix.push_back(row);
        
        row = {0, 1, y};
        trans_matrix.push_back(row);
        
        row = {0, 0, 1};
        trans_matrix.push_back(row);
        
        return trans_matrix;
        
    }
    
    void translate(int x, int y){
        
        std::vector<std::vector<int>> trans_matrix;
        std::vector<std::vector<int>> cpymatrix;
        std::vector<std::vector<int>> result_matrix;
        trans_matrix = create_trans_matrix(x, y);
        
        
        
        // copy matrix
        cpymatrix = matrix;

        //multiply original matrix by translation matrix
        result_matrix = matrixMultiply(trans_matrix, cpymatrix);
        
        // copy back into vector
        matrix = result_matrix;
        
        
    }
    
    
};


// function prototype
int getPosByID(std::vector<Geometry *> vect, Geometry *obj);

class Line : public Geometry {
public:
    /* data members */
    float x1, y1, x2, y2;
    int color = 1.0;
    
    
    /* member functions */
    Line(std::vector<std::array<float, 2>> points){
        for(auto it = points.begin(); it != points.end(); it++){
            Point *pt = new Point();
            pt->x = (*it)[0];
            pt->y = (*it)[1];
            vertices.push_back(pt);
        }
        x1 = points.at(0)[0];
        y1 = points.at(0)[1];
        x2 = points.at(1)[0];
        y2 = points.at(1)[1];
        id = ::pid++;
        n = (int)vertices.size();
    }
    
    Line(float x1, float y1, float x2, float y2){
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }
    
    
    void updateParameters(){
        this->x1 = vertices[0]->x;
        this->y1 = vertices[0]->y;
        this->x2 = vertices[1]->x;
        this->y2 = vertices[1]->y;
        n = (int)vertices.size();
    }
    
    // draws a single line given a vertex pair and color according to DDA algorithm
    void lineDDA()
    {
        float dX,dY,iSteps;
        float xInc,yInc,iCount,x,y;
        
        dX = x1 - x2;
        dY = y1 - y2;
        
        if (fabs(dX) > fabs(dY))
        {
            iSteps = fabs(dX);
        }
        else
        {
            iSteps = fabs(dY);
        }
        
        xInc = dX/iSteps;
        yInc = dY/iSteps;
        
        x = x1;
        y = y1;
        
        for (iCount=1; iCount<=iSteps; iCount++)
        {
            setPixel(floor(x),floor(y),color);
            x -= xInc;
            y -= yInc;
        }
        return;
    }
    
    
    // draws a single line given a vertex pair and color according to Bresenham algorithm
    void lineBres(){
        
        const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
        
        float _x1 = x1;
        float _x2 = x2;
        float _y1 = y1;
        float _y2 = y2;
        
        if(steep)
            // dy > dx, therefore slope > 1
        {
            std::swap(_x1, _y1);
            std::swap(_x2, _y2);
        }
        
        if(_x1 > _x2)
            // going from left to right, make sure left is smaller of two
        {
            std::swap(_x1, _x2);
            std::swap(_y1, _y2);
        }
        
        const float dx = _x2 - _x1;
        const float dy = fabs(_y2 - _y1);
        
        float error = dx / 2.0f;
        const int ystep = (_y1 < _y2) ? 1 : -1;
        int y = (int)_y1;
        
        const int maxX = (int)_x2;
        
        for(int x=(int)_x1; x<maxX; x++)
        {
            if(steep)
            {
                setPixel(y, x, color);
            }
            else
            {
                setPixel(x, y, color);
            }
            
            error -= dy;
            if(error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
    
    
    int clipLine(){
        unsigned int outcode0,outcode1;
        bool accept = false, done = false;
        
        float _x1 = x1;
        float _x2 = x2;
        float _y1 = y1;
        float _y2 = y2;
        
        
        outcode0 = code(_x1,_y1);
        outcode1 = code(_x2,_y2);
        
        do
        {
            if(outcode0 == 0 && outcode1==0)
            {
                accept = true;
                done = true;
            }
            else if(outcode0 & outcode1)
            {
                done = true;
            }
            else
            {
                float x,y;
                int ocd=outcode0 ? outcode0 : outcode1;
                if(ocd & TOP)
                {
                    x=_x1+(_x2-_x1)*(ymax-_y1)/(_y2-_y1);
                    y=ymax;
                }
                else if(ocd & BOTTOM)
                {
                    x=_x1+(_x2-_x1)*(ymin-_y1)/(_y2-_y1);
                    y=ymin;
                }
                else if(ocd & LEFT)
                {
                    y=_y1+(_y2-_y1)*(xmin-_x1)/(_x2-_x1);
                    x=xmin;
                }
                else
                {
                    y=_y1+(_y2-_y1)*(xmax-_x1)/(_x2-_x1);
                    x=xmax;
                }
                if(ocd==outcode0)
                {
                    _x1=x;
                    _y1=y;
                    outcode0=code(_x1,_y1);
                }
                else
                {
                    _x2=x;
                    _y2=y;
                    outcode1=code(_x2,_y2);
                }
            }
            
        }
        while(done == false);
        
        if(accept == true){
            x1 = _x1;
            y1 = _y1;
            x2 = _x2;
            y2 = _y2;
            return 1;
        }
        return -1;
    }
    
};

class Polygon : public Geometry {
public:
    /* data members */

    std::vector<Point *>results;
    std::vector<Point *>tmp;
    bool unchanged = false;
    
    /* member functions */
    // constructor
    Polygon(std::vector<std::array<float, 2>> points){
        for(auto it = points.begin(); it != points.end(); it++){
            Point *pt = new Point();
            pt->x = (*it)[0];
            pt->y = (*it)[1];
            vertices.push_back(pt);
            xc.push_back((*it)[0]);
            yc.push_back((*it)[1]);
            
        }
        n = (int)vertices.size();
        id = ::pid++;
        
    }
    
    void updateParameters(){
        xc.clear();
        yc.clear();
        for(auto it = vertices.begin(); it != vertices.end(); it++){
            xc.push_back((*it)->x);
            yc.push_back((*it)->y);
        }
        n = (int)vertices.size();
    }
    
    // creates edge buckets from the given edges
    std::list<Bucket*> createEdges () {
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
            v1 = { xc[startIndex], yc[startIndex] };
            v2 = { xc[i], yc[i] };
            
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
    void processEdgeTable (std::list<Bucket*> edgeTable) {
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
                    setPixel(x, scanLine, 1.0);
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
    void drawPolygon() {
        // Create Edge Table
        std::list<Bucket*> finalEdgeTable = createEdges();
        
        // Sort edges by minY
        finalEdgeTable.sort(minYCompare);
        
        // Process Edge Table and draw Polygon
        processEdgeTable(finalEdgeTable);
    }
    
    // Draw outline of polygon
    void drawOutlines(){
        for(int i = 0; i < vertices.size()-1; i++){
            float x1 = vertices[i]->x;
            float y1 = vertices[i]->y;
            float x2 = vertices[i+1]->x;
            float y2 = vertices[i+1]->y;
            Line line(x1, y1, x2, y2);
            line.lineDDA();
            
        }
        
        // connect starting vertice to ending vertice
        float x1 = vertices[0]->x;
        float y1 = vertices[0]->y;
        float x2 = vertices[vertices.size()-1]->x;
        float y2 = vertices[vertices.size()-1]->y;
        Line line(x1, y1, x2, y2);
        line.lineDDA();
    }
    
    struct PointCompare {
        bool operator()(Point *a, Point *b)
        {
            if (a->x == b->x){
                return a->y < b->y;
            }
            
            return a->x < b->x;
        }
    };
    
    
    
    void clipLeft(Point *p1, Point *p2){
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
    
    void clipTop(Point *p1, Point *p2){
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
    
    void clipRight(Point *p1, Point *p2){
        float dy = p2->y - p1->y;
        float dx = p2->x - p1->x;
        float m;
        if(fabs(dx) > 0)
            m = dy/dx;
        else
            m = 100000;
        Point *intr = new Point();
        intr->x = xmax;
        intr->y = round(m * (ymax - p1->y) + p1->x);
        
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
    
    void clipBottom(Point *p1, Point *p2){
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
    
    int clipPolygon(){
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
        p1 = results[vertices.size() - 1];
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
        p1 = results[vertices.size() - 1];
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
        p1 = results[vertices.size() - 1];
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
        p1 = results[vertices.size() - 1];
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
    
    
}; // end class definition

std::vector<Polygon *> all_polygons; // global variable to hold all polygons
std::vector<Line *> all_lines; // global variable to hold all polygons

template <typename T>
int getPosByID(std::vector<T> vect, T obj){
    for(int i = 0; i < vect.size(); i++){
        if (vect[i]->id == obj->id){
            return i;
        }
    }
    return -1;
}


// sets the local in pixelBuffer to c given x and y coordinates
void setPixel(int x, int y, double c){
    int i = (y * window_width + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBuffer[i + j] = c;
    }
    return;
    
}



// clears all of the pixels from the screen
void clearAllPixels(){
    for(int i = 0; i < window_width; i++){
        for(int j = 0; j < window_height; j++){
            setPixel(i, j, 0.0);
        }
    }
}


// main display loop, this function will be called again and again by OpenGL
void update(){
    
    cout << "entered update" << endl;
    
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearAllPixels();
    
    
//    /********************
//     * polygon clipping
//     ********************/
//    vector<int>polygons_index;
//    for(auto i : all_polygons){
//        int exists = i->clipPolygon();
//        if (exists <= 0){
//            int pos = getPosByID(all_polygons, i);
//            if (pos >= 0){
//                polygons_index.push_back(pos);
//            }
//        }
//    }
//
//    for (auto i : polygons_index){
//        all_polygons[i] = all_polygons.back();
//        all_polygons.pop_back();
//    }
//
//    for(auto i : all_polygons){
//        i->drawPolygon();
//    }
    

//    
//    /*********************
//        Clipping lines
//     **********************/
//    Line *line = new Line(70, 80, 85, 80);
//    all_lines.push_back(line);
//    
//    
//    vector<int>index;
//    for(auto i : all_lines){
//        int exists = i->clipLine();
//        if (exists <= 0){
//            int pos = getPosByID(all_lines, i);
//            if(pos >= 0){
//                cout << "index = " << pos << endl;
//                index.push_back(pos);
//            }
//        }
//    }
//    
//    for (auto i : index){
//        all_lines[i] = all_lines.back();
//        all_lines.pop_back();
//    }
//    
//    
//    for(auto i : all_lines){
//        i->lineBres();
//    }
    
//    all_polygons[0]->drawPolygon();
//    all_polygons[0]->convertToMatrix();
//    all_polygons[0]->translate(100, 0);
//    all_polygons[0]->convertToVector();
//    all_polygons[0]->updateParameters();
//    all_polygons[0]->drawPolygon();
    
    all_lines[0]->lineDDA();
    all_lines[0]->convertToMatrix();
    all_lines[0]->translate(7, 20);
    all_lines[0]->convertToVector();
    all_lines[0]->updateParameters();
    all_lines[0]->lineDDA();
    
//    all_polygons[0]->matrixmultipleexample();
//
//    
//    all_polygons[0]->printMatrix(all_polygons[0]->matrix1);
//    all_polygons[0]->printMatrix(all_polygons[0]->matrix2);
//    all_polygons[0]->matrixmultipleexample();
//    
//    for(auto i : all_polygons){
//        i->drawPolygon();
//        i->findCentroid();
//        cout << "centroid x = " << i->centroid.x << endl;
//        cout << "centroid y = " << i->centroid.y << endl;
//    }
    
    
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
    
}


int main(int argc, char *argv[]){
    
    int numPolygons, numPoints = 0;
    std::string chars;
    
    std::ifstream inputFile("input.txt");
    
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    // read in first line of file = number of polygons we are creating
    getline(inputFile, chars);
    numPolygons = atoi(chars.c_str());
    std::cout << "numPolygons: " << numPolygons << std::endl;
    
    // container to hold coordinates
    std::vector<std::array<float, 2>>points;
    
    getline(inputFile, chars);
    
    // read in file
    while(!inputFile.eof()){
        
        while(chars != ""){
            std::istringstream iss;
            iss.str(chars);
            std::string val1, val2;
            iss >> val1 >> val2;
            double x = std::stof(val1);
            double y = std::stof(val2);
            std::cout << "val1 = " << val1 << std::endl;
            std::cout << "val2 = " << val2 << std::endl;
            std::cout << std::endl;
            std::array<float, 2>coordinates = {static_cast<float>(x), static_cast<float>(y)};
            points.push_back(coordinates);
            getline(inputFile, chars);
        }
        
        if(!points.empty()){
            if (numPoints == 2){
                std::cout << "line" << std::endl;
                Line *line = new Line(points);
                all_lines.push_back(line);
            }
            else if (numPoints > 2){
                Polygon *polygon = new Polygon(points);
                all_polygons.push_back(polygon);
            }
            points.clear();
        }
        
        getline(inputFile, chars);
        numPoints = atoi(chars.c_str());
        getline(inputFile, chars);
    }
    
    //allocate new pixel buffer, need initialization!!
    PixelBuffer = new float[PixelBufferSize];
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    //set window size to 200*200
    glutInitWindowSize(window_width, window_height);
    //set window position
    glutInitWindowPosition(100, 100);
    
    //create and set main window title
    glutCreateWindow("Hello Graphics!!");
    glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
    //sets display function
    glutDisplayFunc(update);
    
    glutMainLoop(); //main display loop, will display until terminate
    return 0;
}


