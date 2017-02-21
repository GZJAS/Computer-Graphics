//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//
#include "Line.hpp"
#include "Polygon.hpp"
#include "threeD.hpp"
#include "Edge.hpp"
#include "Color.hpp" 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <map>


#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// global variables
int display_count = 0;
int window_width, window_height;
int PixelBufferSize;
float *PixelBufferXY, *PixelBufferYZ, *PixelBufferXZ;
float xmin, xmax, ymin, ymax;
int winXY, winYZ, winXZ, MainWindow;
std::map<std::string, Color> ColorMap = {{"red", {1, 0, 0}},
                                        {"green", {0, 1, 0}},
                                        {"blue", {0, 0, 1}},
                                        {"white", {1, 1, 1}}};


Point *threeD::lightsource = new Point(0, 0, 0, ColorMap["white"]);
Point *threeD::viewsource = new Point(0, 50, 100);
Color threeD::ambientlight = {.25, .25, .25};


// global variable to hold all polygons
std::vector<threeD*>all_shapes;
std::vector<threeD*>rotate_axis;


// function prototypes;
void displayXY();
void displayYZ();
void displayXZ();
void setPixelXZ(int x, int z, double c);
void setPixelYZ(int z, int y, double c);
void setPixelXY(int x, int y, double c);


threeD *getObjectByID(int id){
    for(auto i: all_shapes){
        if (i->id == id){
            return i;
        }
    }
    return nullptr;
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


// sets the local in pixelBuffer to c given x and y coordinates
void setPixelYZ(int z, int y, Color color){
    int i = (y * window_width/2 + z) * 3;
    PixelBufferYZ[i] = color.r;
    PixelBufferYZ[i + 1] = color.g;
    PixelBufferYZ[i + 2] = color.b;
    return;
    
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelXZ(int x, int z, Color color){
    int i = (z * window_width/2 + x) * 3;
    PixelBufferXZ[i] = color.r;
    PixelBufferXZ[i + 1] = color.g;
    PixelBufferXZ[i + 2] = color.b;
    return;
    
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelXY(int x, int y, Color color){
    int i = (y * window_width/2 + x) * 3;
    PixelBufferXY[i] = color.r;
    PixelBufferXY[i + 1] = color.g;
    PixelBufferXY[i + 2] = color.b;
    return;
    
}

// clears all of the pixels from the screen
void clearXYPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBufferXY[i] = 0.0;
    }
}
void clearYZPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBufferYZ[i] = 0.0;
    }

}

void clearXZPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBufferXZ[i] = 0.0;
    }
}

int findEdges(threeD *shape, Point *point){
    for(int i = 0; i < shape->vertices.size(); i++){
        if(shape->vertices[i] == point){
            return i;
        }
    }
    return -1;
}

void interface(){
    return;
}

// main display loop, this function will be called again and again by OpenGL
void displayXY(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    
    for(auto i : all_shapes){
        i->findCentroid();
        std::sort(all_shapes.begin(), all_shapes.end(), [](threeD *shape1, threeD *shape2){return shape1->centroid.z > shape2->centroid.z;});
        i->drawXY();
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXY);
    
    //window refresh
    glFlush();
    glutPostRedisplay();
    
}

// main display loop, this function will be called again and again by OpenGL
void displayYZ(){
    glutSetWindow(winYZ);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearYZPixels();
    
    for(auto i : all_shapes){
        i->findCentroid();
        std::sort(all_shapes.begin(), all_shapes.end(), [](threeD *shape1, threeD *shape2){return shape1->centroid.x > shape2->centroid.x;});
        i->drawYZ();

    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferYZ);
    
    glFlush();
    glutPostRedisplay();
}

// main display loop, this function will be called again and again by OpenGL
void displayXZ(){
    string answer;
    glutSetWindow(winXZ);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearXZPixels();
    
//    if(display_count){
//        interface();
//    }
    
    for (auto i: all_shapes){
        i->findCentroid();
        std::sort(all_shapes.begin(), all_shapes.end(), [](threeD *shape1, threeD *shape2){return shape1->centroid.y > shape2->centroid.y;});

        i->drawXZ();
    }

    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXZ);
    
    display_count = 1;
    
    glFlush();
    glutPostRedisplay();
}

void display(){
    glutSetWindow(MainWindow);
    glClear(GL_COLOR_BUFFER_BIT);
    
}

int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;
    
    int numPoints, numFaces;
    std::string chars;
    
    std::ifstream inputFile("file.txt", std::ios::in);
    
    // exit program if unable to create file
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    std::vector<Point *>points;
    std::vector<Point *>normals;
    std::vector<Polygon *>faces;
    
    // read in file
    while(!inputFile.eof()){
        
        getline(inputFile, chars);
        numPoints = atoi(chars.c_str());
        
        points.clear();
        normals.clear();
        faces.clear();
        
        
        if(chars == ""){
            break;
        }
        
        // get vertices
        for(int i = 0; i < numPoints; i++){
            getline(inputFile, chars);
            std::stringstream iss;
            iss.str(chars);
            std::string val1, val2, val3;
            iss >> val1 >> val2 >> val3;
            Point *newPoint = new Point();
            newPoint->x = std::stof(val1);
            newPoint->y = std::stof(val2);
            newPoint->z = std::stof(val3);
            points.push_back(newPoint);
        }
        
        threeD *newShape = new threeD(points);
        
        // get normal vectors for each vertex
        for(int i = 0; i < numPoints; i++){
            getline(inputFile, chars);
            std::stringstream iss;
            iss.str(chars);
            std::string val1, val2, val3;
            iss >> val1 >> val2 >> val3;
            Point *normalvec = new Point();
            normalvec->x = std::stof(val1);
            normalvec->y = std::stof(val2);
            normalvec->z = std::stof(val3);
            normals.push_back(normalvec);
        }
        
        newShape->normals = normals;
        
        
        // get faces
        getline(inputFile, chars);
        numFaces = atoi(chars.c_str());
        
        for(int i = 0; i < numFaces; i++){
            getline(inputFile, chars);
            std::stringstream iss(chars);
            int index;
            std::vector<Point *>vertices;
            while(iss >> index){
                vertices.push_back(&(*newShape->vertices.at(index)));
            }
            Polygon *poly = new Polygon(vertices);
            newShape->faces.push_back(poly);
        }
        
        // get shape color
        getline(inputFile, chars);
        newShape->color = ColorMap[chars];
        
        // ka
        getline(inputFile, chars);
        newShape->ka = std::stof(chars);
        
        // kd
        getline(inputFile, chars);
        newShape->kd = std::stof(chars);
        
        // ks
        getline(inputFile, chars);
        newShape->ks = std::stof(chars);
        
        // ns
        getline(inputFile, chars);
        newShape->ns = std::stof(chars);
        
        
        all_shapes.push_back(newShape);
        
        getline(inputFile, chars);
        
        
    }
    
    
    //intialize each buffer to be 1/4 of main window size
    PixelBufferXY = new float[PixelBufferSize/4];
    PixelBufferYZ = new float[PixelBufferSize/4];
    PixelBufferXZ = new float[PixelBufferSize/4];
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    
    //set window size
    glutInitWindowSize(window_width, window_height);
    
    //set window position
    glutInitWindowPosition(100, 100);
    
    //create and set main window title
    MainWindow = glutCreateWindow("Hello Graphics!!");
    glutDisplayFunc(display);
    
    // subwindow winXY will be located in upper left of main window
    winXY = glutCreateSubWindow(MainWindow, 0, 0, window_width/2, window_height/2);
    glutDisplayFunc(displayXY);
    
    // subwindow winYZ will be located in upper right of main window
    winYZ = glutCreateSubWindow(MainWindow, window_width/2, 0, window_width/2, window_height/2);
    glutDisplayFunc(displayYZ);
    
    // subwindow winXZ will be located in bottom right of main window
    winXZ = glutCreateSubWindow(MainWindow, window_width/2, window_height/2, window_width/2, window_height/2);
    glutDisplayFunc(displayXZ);
    
    //clears the buffer of OpenGL
    glClearColor(0, 0, 0, 0);
    
    //main display loop, will display until terminate
    glutMainLoop();
    return 0;
}


