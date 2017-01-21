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

// function prototypes
void display();
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
class Line {
public:
    /* data members */
    std::vector<std::array<double, 2>> vertices;
    double x1, y1, x2, y2, id, n;
    int color = 1.0;
    
    
    /* member functions */
    Line(std::vector<std::array<double, 2>> points){
        vertices = points;
        x1 = vertices.at(0)[0];
        y1 = vertices.at(0)[1];
        x2 = vertices.at(1)[0];
        y2 = vertices.at(1)[1];
        id = ::pid++;
        n = (int)vertices.size();
    }
    
    Line(double x1, double y1, double x2, double y2){
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
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
    void lineBres()
    {
        // Bresenham's line algorithm
        
        const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
        
        if(steep)
            // dy > dx, therefore slope > 1
        {
            std::swap(x1, y1);
            std::swap(x2, y2);
        }
        
        if(x1 > x2)
            // going from left to right, make sure left is smaller of two
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        
        const float dx = x2 - x1;
        const float dy = fabs(y2 - y1);
        
        float error = dx / 2.0f;
        const int ystep = (y1 < y2) ? 1 : -1;
        int y = (int)y1;
        
        const int maxX = (int)x2;
        
        for(int x=(int)x1; x<maxX; x++)
        {
            if(steep)
            {
                setPixel(y,x, color);
            }
            else
            {
                setPixel(x,y, color);
            }
            
            error -= dy;
            if(error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
};

class Polygon {
    public:
        /* data members */
        std::vector<std::array<double, 2>> vertices;
        std::vector<int> xc;
        std::vector<int> yc;
        int n, id;
    
        /* member functions */
        // constructor
        Polygon(std::vector<std::array<double, 2>> points){
            vertices = points;
            for(auto it = vertices.begin(); it != vertices.end(); it++){
                xc.push_back((*it)[0]);
                yc.push_back((*it)[1]);
            }
            n = (int)vertices.size();
            id = ::pid++;
            
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
            float x1 = vertices[i][0];
            float y1 = vertices[i][1];
            float x2 = vertices[i+1][0];
            float y2 = vertices[i+1][1];
            Line line(x1, y1, x2, y2);
            line.lineBres();
            
        }
        
        // connect starting vertice to ending vertice
        float x1 = vertices[0][0];
        float y1 = vertices[0][1];
        float x2 = vertices[vertices.size()-1][0];
        float y2 = vertices[vertices.size()-1][1];
        Line line(x1, y1, x2, y2);
        line.lineBres();
    }


}; // end class definition

std::vector<Polygon *> all_polygons; // global variable to hold all polygons
std::vector<Line *> all_lines; // global variable to hold all polygons

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
void display(){
    
    clearAllPixels();
    
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    for(Polygon *polygon : all_polygons){
        polygon->drawOutlines();
        std::cout << "poly size: " << polygon->n << std::endl;
    }
    
    for(Line *line : all_lines){
        line->lineDDA();
        std::cout << "line size: " << line->n << std::endl;
    }
    
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
    std::vector<std::array<double, 2>>points;
    
    getline(inputFile, chars);
    
    // read in file
    while(!inputFile.eof()){
        
        while(chars != ""){
            std::istringstream iss;
            iss.str(chars);
            std::string val1, val2;
            iss >> val1 >> val2;
            double x = std::stod(val1);
            double y = std::stod(val2);
            std::cout << "val1 = " << val1 << std::endl;
            std::cout << "val2 = " << val2 << std::endl;
            std::cout << std::endl;
            std::array<double, 2>coordinates = {x, y};
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
    int MainWindow = glutCreateWindow("Hello Graphics!!");
    glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
    //sets display function
    glutDisplayFunc(display);
    
    glutMainLoop(); //main display loop, will display until terminate
    return 0;
}


