//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//
#include "Bucket.h"
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

float *PixelBuffer;
int window_width = 200;
int window_height = 200;
int PixelBufferSize = window_width * window_height * 3;

void clearAllPixels();
void setPixel(int x, int y, double c);
void display();
void drawLine(float x1, float y1, float x2, float y2, int color );

/*
 Compares the yMins of the given buckets parameters
 
 @param edge1 First edge bucket
 @param edge2 Second edge bucket
 
 @return true if edge1's yMin < edge2's yMin, false otherwise
 */
bool minYCompare (Bucket* edge1, Bucket* edge2) {
    return edge1->yMin < edge2->yMin;
}

/*
 Compares the Xs of the given buckets parameters
 
 @param edge1 First edge bucket
 @param edge2 Second edge bucket
 
 @return true if edge1's X is less than edge2 X, false otherwise
 */
bool xCompare (Bucket* edge1, Bucket* edge2) {
    if (edge1->x < edge2->x) {
        return true;
    } else if (edge1->x > edge2->x) {
        return false;
    } else {
        return ((edge1->dX / edge1->dY) < (edge2->dX / edge2->dY));
    }
}

struct Vertex {
    int x;
    int y;
};

class Polygon {
    public:
        /* data members */
        vector<std::array<double, 2>> vertices;
        vector<int> xc;
        vector<int> yc;
        int n;
    
        /* member functions */
        // constructor
        Polygon(std::vector<std::array<double, 2>> points){
            vertices = points;
            for(auto it = vertices.begin(); it != vertices.end(); it++){
                xc.push_back((*it)[0]);
                yc.push_back((*it)[1]);
            }
            n = (int)vertices.size();
            
        }
    
        // creates edge buckets from the given edges
        list<Bucket*> createEdges () {
            int startIndex = n - 1;
            int yMax;
            int yMin;
            int initialX;
            int sign;
            int dX;
            int dY;
            Vertex v1;
            Vertex v2;
            Vertex tmp;

            list<Bucket*> edgeTable;   // list that contains all of the edges that make up the figure
            
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
    
    ///
    // Draw a filled polygon
    //
    // The polygon has n distinct vertices.  The coordinates of the vertices
    // making up the polygon are stored in the x and y arrays.  The ith
    // vertex will have coordinate (x[i],y[i]).
    //
    // @param n - number of vertices
    // @param x - x coordinates
    // @param y - y coordinates
    ///
    void drawPolygon() {
        // Create Edge Table
        list<Bucket*> finalEdgeTable = createEdges();
        
        // Sort edges by minY
        finalEdgeTable.sort(minYCompare);
        
        // Process Edge Table and draw Polygon
        processEdgeTable(finalEdgeTable);
    }
    

}; // end class definition

vector<Polygon *> all_polygons; // global variable to hold all polygons

void drawOutlines(){
    for(Polygon *polygon : all_polygons){
        for(int i = 0; i < polygon->vertices.size()-1; i++){
            float x1 = polygon->vertices[i][0];
            float y1 = polygon->vertices[i][1];
            float x2 = polygon->vertices[i+1][0];
            float y2 = polygon->vertices[i+1][1];
            drawLine(x1, y1, x2, y2, 1.0);

        }

        // connect starting vertice to ending vertice
        float x1 = polygon->vertices[0][0];
        float y1 = polygon->vertices[0][1];
        float x2 = polygon->vertices[polygon->vertices.size()-1][0];
        float y2 = polygon->vertices[polygon->vertices.size()-1][1];
        drawLine(x1, y1, x2, y2, 1.0);



    }
}

// draws a single line given a vertex pair and color
// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
void drawLine(float x1, float y1, float x2, float y2, int color )
{
    // Bresenham's line algorithm
    
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    
    if(steep)
    // dy > dx, therefore slope > 1
    {
        swap(x1, y1);
        swap(x2, y2);
    }
    
    if(x1 > x2)
    // going from left to right, make sure left is smaller of two
    {
        swap(x1, x2);
        swap(y1, y2);
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
            setPixel(y,x, 1.0);
        }
        else
        {
            setPixel(x,y, 1.0);
        }
        
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

// clears all of the pixels from the screen
void clearAllPixels(){
    for(int i = 0; i < window_width; i++){
        for(int j = 0; j < window_height; j++){
            setPixel(i, j, 1.0);
        }
    }
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixel(int x, int y, double c){
    int i = (y * window_width + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBuffer[i + j] = c;
    }
    return;
}

// main display loop, this function will be called again and again by OpenGL
void display(){
    
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
//    for(Polygon *polygon : all_polygons){
//        polygon->drawPolygon();
//    }
    
    drawOutlines();

    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
}



int main(int argc, char *argv[]){
    
    int numPolygons;
    string line;
    
    ifstream inputFile("input.txt");
    
    if(!inputFile){
        cerr << "File could not be opened" << endl;
    }
    
    // read in first line of file = number of polygons we are creating
    getline(inputFile, line);
    numPolygons = atoi(line.c_str());
    cout << "numPolygons: " << numPolygons << endl;
    
    // containers to hold coordinates and polygons
    vector<array<double, 2>>points;
    vector<vector<array<double, 2>>> polygons;
    
    // read in file
    while(!inputFile.eof()){
        
        getline(inputFile, line);
        cout << line << endl;
        // if empty space, we've reached the start of a new polygon
        if (line == ""){
            getline(inputFile, line);
            if (!points.empty()){
                polygons.push_back(points);
                Polygon *polygon = new Polygon(points);
                all_polygons.push_back(polygon);
                points.clear();
            }
        }
        else {
            istringstream iss;
            iss.str(line);
            string val1, val2;
            iss >> val1 >> val2;
            double x = stod(val1);
            double y = stod(val2);
            array<double, 2>coordinates = {x, y};
            points.push_back(coordinates);
        }
    }

    // ***** FOR DEBUGGING PURPOSES ONLY *****
    /**************
    cout << "polygons"<< endl;
    for (int i = 0; i < polygons.size(); i++)
    {
        for (int j = 0; j < polygons[i].size(); j++)
        {
            for(int k = 0; k < 2; k++){
                cout << polygons[i][j][k] << " ";
            }
            cout << endl;
            
        }
        cout << endl;
    }
    
    if(all_polygons.size() == numPolygons){
        cout << "Same size" << endl;
    }
    else {
        cout << "polygons.size() = " << polygons.size() << endl;
        cout << "numPolygons = " << numPolygons << endl;
    }
    
    cout << "end polygons" << endl;
    ****************/
    
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
    
    glutMainLoop();//main display loop, will display until terminate
    return 0;
}


