//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//

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

using namespace std;

float *PixelBuffer;
int window_width = 200;
int window_height = 200;
int PixelBufferSize = window_width * window_height * 3;

void clearAllPixels();
void setPixel(int x, int y, double c);
void display();

class Polygon {
    public:
        vector<array<double, 2>> vertices;
    
        Polygon(vector<array<double, 2>> vertices){
            this->vertices = vertices;
        }
    
};

vector<Polygon *> all_polygons;

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
    
    // For each polygon in our global all_polygons vector
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


