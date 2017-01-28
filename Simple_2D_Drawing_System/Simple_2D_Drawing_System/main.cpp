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

#define PI 3.14159265359

int display_count = 0;
static int pid = 0;
int window_width, window_height;
int PixelBufferSize;
float *PixelBuffer;
float xmin, xmax, ymin, ymax;
enum{TOP=0x1,BOTTOM=0x2,RIGHT=0x4,LEFT=0x8};


// function prototypes
void update();
void setPixel(int x, int y, double c);
void clearAllPixels();
void interface();


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

// Assigns code to a vertex based on the clipping window size
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

// Base class that all shapes will derive from
class Geometry {
public:
    std::vector<Point *> vertices;
    std::vector<std::vector<double>> matrix;
    std::string name;
    
    Point centroid;
    int n, id;
    
    // overridden functions in derived classes
    virtual void draw() = 0;
    virtual int clip() = 0;
    virtual void updateParameters() = 0;
    
    
    // convert vertices vector into matrix (vector of vectors)
    void convertToMatrix(){
        std::vector<double>x_row;
        std::vector<double>y_row;
        for(auto vertex : vertices){
            x_row.push_back(static_cast<double>(vertex->x));
            y_row.push_back(static_cast<double>(vertex->y));
        }
        
        std::vector<double>bottom_row;
        for(int i = 0; i < n; i++){
            bottom_row.push_back(1);
        }
        
        matrix.push_back(x_row);
        matrix.push_back(y_row);
        matrix.push_back(bottom_row);
    }
    
    void convertToVector(){
        int numPoints = (int)matrix[0].size();
        vertices.clear();
        for(int i = 0; i < numPoints; i++){
            Point *pt = new Point();
            pt->x = matrix[0][i];
            pt->y = matrix[1][i];
            vertices.push_back(pt);
        }
    }
    
    // find centroid of polygon
    // http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
    void findCentroid()
    {
        centroid = {0, 0};
        float signedArea = 0.0;
        float x0 = 0.0; // Current vertex X
        float y0 = 0.0; // Current vertex Y
        float x1 = 0.0; // Next vertex X
        float y1 = 0.0; // Next vertex Y
        float a = 0.0;  // Partial signed area
        
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
    std::vector<std::vector<double>> matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB){
        
        std::vector<std::vector<double>> matrixC;
        int r1 = (int)matrixA.size();       // 3
        int c1 = (int)matrixA[0].size();
        int c2 = n;
        
        std::vector<double>row;
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
    std::vector<std::vector<double>> create_rot_matrix(double alpha){
        // rot matrix in the form of:
        //        |   cost(alpha)   -sin(alpha)     0   |
        //        |   sin(alpha)    cos(alpha)beta  0   |
        //        |   0                 0           1   |
        std::vector<std::vector<double>> rot_matrix;
        std::vector<double>row = {cos(alpha * PI/180), -sin(alpha * PI/180), 0.0};
        rot_matrix.push_back(row);
        
        row = {sin(alpha * PI/180), cos(alpha * PI/180), 0.0};
        rot_matrix.push_back(row);
        
        row = {0.0, 0.0, 1.0};
        rot_matrix.push_back(row);
        
        return rot_matrix;
        
    }
    
    // return a matrix with given scaling parameters
    std::vector<std::vector<double>> create_scale_matrix(double alpha, double beta){
        // scale matrix in the form of:
        //        |    alpha   0       0   |
        //        |    0       beta    0   |
        //        |    0       0       1   |
        std::vector<std::vector<double>> scale_matrix;
        std::vector<double>row = {alpha, 0.0, 0.0};
        scale_matrix.push_back(row);
        
        row = {0.0, beta, 0.0};
        scale_matrix.push_back(row);
        
        row = {0.0, 0.0, 1.0};
        scale_matrix.push_back(row);
        
        return scale_matrix;
        
    }
    
    // return a matrix with given tranlate parameters
    std::vector<std::vector<double>> create_trans_matrix(double x, double y){
        // translate matrix in the form of:
        //        |     1     0      x    |
        //        |     0     1      y    |
        //        |     0     0      1    |
        std::vector<std::vector<double>> trans_matrix;
        std::vector<double>row = {1.0, 0.0, x};
        trans_matrix.push_back(row);
        
        row = {0.0, 1.0, y};
        trans_matrix.push_back(row);
        
        row = {0.0, 0.0, 1.0};
        trans_matrix.push_back(row);
        
        return trans_matrix;
        
    }
    
    
    // translate a polygon or line by (x, y) !!! NOT TO (x, y)
    void translate(int x, int y){
        
        std::vector<std::vector<double>> trans_matrix;
        std::vector<std::vector<double>> cpymatrix;
        std::vector<std::vector<double>> result_matrix;
        
        // get matrix for translating
        trans_matrix = create_trans_matrix(x, y);
        
        // copy matrix
        cpymatrix = matrix;

        //multiply original matrix by translation matrix
        result_matrix = matrixMultiply(trans_matrix, cpymatrix);
        
        // copy back into matrix
        matrix = result_matrix;
        
        
    }
    
    // rotate polygon or line by alpha
    void rotate(double alpha){
        std::vector<std::vector<double>> rot_matrix;
        std::vector<std::vector<double>> cpymatrix;
        std::vector<std::vector<double>> trans_matrix;
        std::vector<std::vector<double>> result_matrix;
        std::vector<std::vector<double>> rev_trans_matrix;
        
        // find centroid and set coordinates
        findCentroid();
        
        // matrix to translate polygon/line's centroid by (-cx, -cy)
        rev_trans_matrix = create_trans_matrix(-centroid.x, -centroid.y);
        
        // get matrix for rotating polygon
        rot_matrix = create_rot_matrix(alpha);
        
         // matrix to translate polygon/line's centroid back to (cx, cy)
        trans_matrix = create_trans_matrix(centroid.x, centroid.y);

        
        // copy matrix
        cpymatrix = matrix;
        
        // carry out operations for rotating: translate, rotate, and translate back
        result_matrix = matrixMultiply(rev_trans_matrix, cpymatrix);
        result_matrix = matrixMultiply(rot_matrix, result_matrix);
        result_matrix = matrixMultiply(trans_matrix, result_matrix);
        
        // copy back into matrix
        matrix = result_matrix;
        
    }
    
    void scale(double alpha, double beta){
        std::vector<std::vector<double>> scale_matrix;
        std::vector<std::vector<double>> cpymatrix;
        std::vector<std::vector<double>> trans_matrix;
        std::vector<std::vector<double>> result_matrix;
        std::vector<std::vector<double>> rev_trans_matrix;
        
        // find centroid and set coordinates
        findCentroid();
        
        // matrix to translate polygon/line's centroid by (-cx, -cy)
        rev_trans_matrix = create_trans_matrix(-centroid.x, -centroid.y);
        
        // get matrix for rotating polygon
        scale_matrix = create_scale_matrix(alpha, beta);
        
        // matrix to translate polygon/line's centroid back to (cx, cy)
        trans_matrix = create_trans_matrix(centroid.x, centroid.y);
        
        
        // copy matrix
        cpymatrix = matrix;
        
        // carry out operations for rotating: translate, rotate, and translate back
        result_matrix = matrixMultiply(rev_trans_matrix, cpymatrix);
        result_matrix = matrixMultiply(scale_matrix, result_matrix);
        result_matrix = matrixMultiply(trans_matrix, result_matrix);
        
        // copy back into matrix
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
        name = "Line";
    }
    
    Line(float x1, float y1, float x2, float y2){
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        id = ::pid++;
        n = 2;
        name = "Line";
    }
    
    
    void updateParameters() override {
        this->x1 = vertices[0]->x;
        this->y1 = vertices[0]->y;
        this->x2 = vertices[1]->x;
        this->y2 = vertices[1]->y;
        n = 2;
    }
    
    void draw() override {
        lineDDA();
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
    
    
    int clip() override {
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
    
}; // end class definition

// Polygon class
class Polygon : public Geometry {
public:
    /* data members */
    std::vector<int> xc;
    std::vector<int> yc;
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
        name = "Polygon";
        
    }
    
    void draw() override {
        drawPolygon();
    }
    
    void updateParameters() override {
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
    
    int clip() override {
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
        
        // clip with respect to Right edge
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
    
    
}; // end class definition

// global variable to hold all polygons
std::vector<Geometry *>all_shapes;

int getPosByID(Geometry *obj){
    for(int i = 0; i < all_shapes.size(); i++){
        if (all_shapes[i]->id == obj->id){
            return i;
        }
    }
    return -1;
}

Geometry *getObjectByID(int id){
    for(auto i: all_shapes){
        if (i->id == id){
            return i;
        }
    }
    return nullptr;
}


// sets the local in pixelBuffer to c given x and y coordinates
void setPixel(int x, int y, double c){
    int i = (y * window_width + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBuffer[i + j] = c;
    }
    return;
    
}

// returns a list of polygon ids inside all_shapes vector
std::vector<int>findPolygons(){
    std::vector<int>polygon_ids;
    for(auto i : all_shapes){
        if (i->name == "Polygon"){
            polygon_ids.push_back(i->id);
        }
    }
    return polygon_ids;
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
void display(){
    
    std::string answer;
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    if(display_count){
        interface();
    }
    
    clearAllPixels();
    
    for(auto i : all_shapes){
        i->draw();
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
    
    display_count = 1;
    glutPostRedisplay();
}

void interface(){
    std::string answer;
    std::vector<std::array<float, 2>>points;
    
    std::cout << std::endl << "What would you like to do?" << std::endl <<
    "\t(1) Draw new line" << std::endl <<
    "\t(2) Draw new polygon" << std::endl <<
    "\t(3) Clip" << std::endl <<
    "\t(4) Translate" << std::endl <<
    "\t(5) Scale" << std::endl <<
    "\t(6) Rotate" << std::endl <<
    "\t(7) Save to file" << std::endl <<
    "\t(8) Exit" << std::endl;
    
    std::cout << ">> ";
    std::cin >> answer;
    
    if (answer == "1"){
        std::string algorithm;
        float x1, y1, x2, y2;
        std::cout << "Coordinates for first point: " << std::endl;
        std::cin >> x1 >> y1;
        std::cout << "Coordinates for second point: " << std::endl;
        std::cin >> x2 >> y2;
        Line *line = new Line(x1, y1, x2, y2);
        all_shapes.push_back(line);
        
        
        std::cout << std::endl << "How would you like to draw the line?" << std::endl <<
        "\t(a) Using DDA algorithm " << std::endl <<
        "\t(b) Using Bresenham's algorithm " << std::endl;
        std::cout << ">> ";
        std::cin >> algorithm;

        std::cout << "Line created with id " << line->id << std::endl << std::endl;

        if(algorithm == "a"){
            line->lineDDA();
        }
        else if (algorithm == "b"){
            line->lineBres();
        }
    }
    else if (answer == "2"){

        std::string val1, val2;
        int numPoints;
        points.clear();
        std::cout << "How many vertices will this polygon have? " << std::endl << "points = ";
        std::cin >> numPoints;
        
        for(int i = 0; i < numPoints; i++){
            std::cout << "Enter coordinates for vertex " << i + 1 << ": " << std::endl;
            std::cin >> val1 >> val2;
            float x = std::stof(val1);
            float y = std::stof(val2);
            std::array<float, 2>coordinates = {x, y};
            points.push_back(coordinates);
        }
        
        if(!points.empty()){
            Polygon *polygon = new Polygon(points);
            all_shapes.push_back(polygon);
            polygon->drawPolygon();
            std::cout << "Polygon created with id " << polygon->id << std::endl << std::endl;
        }

        
    }
    else if (answer == "3"){
        std::cout << "xmin = ";
        std::cin >> xmin;
        std::cout << "xmax = ";
        std::cin >> xmax;
        std::cout << "ymin = ";
        std::cin >> ymin;
        std::cout << "ymax = ";
        std::cin >> ymax;
        
        std::vector<int>delete_index;
        for(auto i : all_shapes){
            int exists = i->clip();
            if (exists <= 0){
                int pos = getPosByID(i);
                if(pos >= 0){
                    delete_index.push_back(pos);
                    
                }
            }
        }
        
        // delete all shapes whose vertices are outside clipping window
        for (auto i : delete_index){
            all_shapes[i] = all_shapes.back();
            all_shapes.pop_back();
        }
        
    }
    
    else if (answer == "4"){
        int shape_id;
        int x, y;
        std::vector<int>polygons;
        
        std::cout << "Enter the id of the polygon you wish to translate: " << std::endl;
        polygons = findPolygons();
        for(auto poly_id : polygons){
            std::cout << poly_id << std::endl;
        }
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        Geometry *shape = getObjectByID(shape_id);
        
        // get transformation parameters
        std::cout << "How would you like to translate the Polygon?" << std::endl;
        std::cout << "x = ";
        std::cin >> x;
        std::cout << "y = ";
        std::cin >> y;
        
        // carry out transformation operations
        shape->convertToMatrix();
        shape->translate(x, y);
        shape->convertToVector();
        shape->updateParameters();

    }
    else if (answer == "5"){
        int shape_id;
        double alpha, beta;
        std::vector<int>polygons;
        std::cout << "Enter the id of the polygon you wish to scale: " << std::endl;
        polygons = findPolygons();
        for(auto poly_id : polygons){
            std::cout << poly_id << std::endl;
        }
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        Geometry *shape = getObjectByID(shape_id);
        
        // get transformation parameters
        std::cout << "How would you like to scale the Polygon? " << std::endl;
        std::cout << "alpha = ";
        std::cin >> alpha;
        std::cout << "beta = ";
        std::cin >> beta;
        
        // carry out transformation operations
        shape->convertToMatrix();
        shape->scale(alpha, beta);
        shape->convertToVector();
        shape->updateParameters();
        
    }
    else if (answer == "6"){
        int shape_id;
        double angle;
        std::vector<int>polygons;
        std::cout << "Enter the id of the polygon you wish to rotate: " << std::endl;
        polygons = findPolygons();
        for(auto poly_id : polygons){
            std::cout << poly_id << std::endl;
        }
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        Geometry *shape = getObjectByID(shape_id);
        
        std::cout << "How would you like to rotate the Polygon? " << std::endl;
        std::cout << "angle = ";
        std::cin >> angle;
 
        
        // carry out transformation operations
        shape->convertToMatrix();
        shape->rotate(angle);
        shape->convertToVector();
        shape->updateParameters();
    }
    else if (answer == "7"){
        std::ofstream outputFile("file.txt", std::ios::out);
        
        // exit program if unable to create file
        if(!outputFile){
            std::cerr << "File could not be opened" << std::endl;
        }
        
        outputFile << all_shapes.size() << std::endl << std::endl;
        
        for(auto shape : all_shapes){
            outputFile << shape->n << std::endl;
            for(auto vertex : shape->vertices){
                outputFile << vertex->x << " " << vertex->y << std::endl;
            }
            outputFile << std::endl;
        }
    }
    else if (answer == "8"){
        exit(0);
    }
    else {
        std::cout << "Invalid answer. Please try again" << std::endl << std::endl;
        interface();
    }
    
}

int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;
    
    int numPolygons, numPoints = 0;
    std::string chars;
    
    std::ifstream inputFile("file.txt", std::ios::in);
    
    // exit program if unable to create file
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    // read in first line of file = number of polygons we are creating
    getline(inputFile, chars);
    numPolygons = atoi(chars.c_str());
    
    
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
            float x = std::stof(val1);
            float y = std::stof(val2);
            std::cout << std::endl;
            std::array<float, 2>coordinates = {x, y};
            points.push_back(coordinates);
            getline(inputFile, chars);
        }
        
        if(!points.empty()){
            if (numPoints == 2){
                Line *line = new Line(points);
                all_shapes.push_back(line);
            }
            else if (numPoints > 2){
                Polygon *polygon = new Polygon(points);
                all_shapes.push_back(polygon);
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
    
    //clears the buffer of OpenGL
    glClearColor(0, 0, 0, 0);
    
    //sets display function
    glutDisplayFunc(display);
    
    //main display loop, will display until terminate
    glutMainLoop();
    return 0;
}


