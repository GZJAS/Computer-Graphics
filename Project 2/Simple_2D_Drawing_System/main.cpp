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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

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


float findMin(){
    std::vector<float> min;
    
    
    // go through each shape and find min x, y, z
    for (auto shape: all_shapes){
        std::vector<Point *>::iterator xmin = std::min_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->x < p2->x;});
        std::vector<Point *>::iterator ymin = std::min_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->y < p2->y;});
        std::vector<Point *>::iterator zmin = std::min_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->z < p2->z;});
        min.push_back((*xmin)->x);
        min.push_back((*ymin)->y);
        min.push_back((*zmin)->z);
    }
    
    // find the min out of all x, y, z
    std::vector<float>::iterator min_itr = std::min_element(min.begin(), min.end(), [](float &x1, float &x2){return x1 < x2;});

    return *min_itr;
}

float findMax(){
    std::vector<float> max;
    
    
    // go through each shape and find max x, y, z
    for (auto shape: all_shapes){
        std::vector<Point *>::iterator xmax = std::max_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->x < p2->x;});
        std::vector<Point *>::iterator ymax = std::max_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->y < p2->y;});
        std::vector<Point *>::iterator zmax = std::max_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->z < p2->z;});
        max.push_back((*xmax)->x);
        max.push_back((*ymax)->y);
        max.push_back((*zmax)->z);
    }
    
    // find the max out of all x, y, z
    std::vector<float>::iterator max_itr = std::max_element(max.begin(), max.end(), [](float &x1, float &x2){return x1 < x2;});
    
    return *max_itr;
}



// convert x,y coordinates to NDC for xy plane
void NDCmapToXY(float &x, float &y){
    float xbar, ybar, min, max;
    
    // find min and max
    min = findMin();
    max = findMax();
    
    // compute the new point
    xbar = (x - min)/(max + 1 - min);
    ybar = (y - min)/(max + 1 - min);

    // find corresponding pixel
    x = xbar * window_width/2;
    y = ybar * window_height/2;
    
}

// convert y,z coordinates to NDC for yz plane
void NDCmapToYZ(float &z, float &y){
    float zbar, ybar, min, max;
    
    // find min and max
    min = findMin();
    max = findMax();
    
    // compute new NDC point
    zbar = (z - min)/(max + 1 - min);
    ybar = (y - min)/(max + 1 - min);
    
    // find corresponding pixel
    z = zbar * window_width/2;
    y = ybar * window_height/2;
    
    
}

// convert x,z coordinates to NDC for xz plane
void NDCmapToXZ(float &x, float &z){
    float xbar, zbar, min, max;
 
    // find min and max
    min = findMin();
    max = findMax();
    
    // compute new NDC point
    xbar = (x - min)/(max + 1 - min);
    zbar = (z - min)/(max + 1 - min);
    
    // find corresponding pixel
    x = xbar * window_width/2;
    z = zbar * window_height/2;
    
}


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
void setPixelYZ(int z, int y, double c){
    int i = (z * window_width/2 + y) * 3;
    for(int j = 0; j < 3; j++){
        PixelBufferYZ[i + j] = c;
    }
    return;
    
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelXZ(int x, int z, double c){
    int i = (z * window_width/2 + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBufferXZ[i + j] = c;
    }
    return;
    
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelXY(int x, int y, double c){
    int i = (y * window_width/2 + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBufferXY[i + j] = c;
    }
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
    std::string answer;
    std::vector<std::array<float, 2>>points;

    std::cout << std::endl << "What would you like to do?" << std::endl <<
    "\t(1) Translate" << std::endl <<
    "\t(2) Scale" << std::endl <<
    "\t(3) Rotate" << std::endl <<
    "\t(4) Save to file" << std::endl <<
    "\t(5) Exit" << std::endl;

    std::cout << ">> ";
    std::cin >> answer;

    if (answer == "1"){
        int shape_id, x, y, z;
        
        std::cout << "Enter the id of the shape you would like to translate: " << std::endl;
        
        for(auto shape: all_shapes){
            std::cout << shape->id << std::endl;
        }
        
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the shape the user specified by id
        threeD *shape = getObjectByID(shape_id);
        
        // get translation parameters
        std::cout << "How would you like to translate this shape? " << std::endl;
        std::cout << "x = ";
        std::cin >> x;
        std::cout << "y = ";
        std::cin >> y;
        std::cout << "z = ";
        std::cin >> z;
        
        // carry out transformation operation
        shape->translate(x, y, z);
        shape->convertToVector();


    }
    else if (answer == "2"){
        int shape_id;
        double Sx, Sy, Sz;

        std::cout << "Enter the id of the shape you would like to scale: " << std::endl;
        
        for(auto shape: all_shapes){
            std::cout << shape->id << std::endl;
        }
        
        std::cout << ">> ";
        std::cin >> shape_id;

        // get the object the user specified
        threeD *shape = getObjectByID(shape_id);

        // get transformation parameters
        std::cout << "How would you like to scale the shape? " << std::endl;
        std::cout << "Sx = ";
        std::cin >> Sx;
        std::cout << "Sy = ";
        std::cin >> Sy;
        std::cout << "Sz = ";
        std::cin >> Sz;

        // carry out transformation operations
        shape->scale(Sx, Sy, Sz);

    }
    else if (answer == "3"){
        int shape_id;
        double angle;
        Point *p1 = new Point();
        Point *p2 = new Point();
        
        std::cout << "Enter the id of the shape you would like to rotate: " << std::endl;
        
        for(auto shape: all_shapes){
            std::cout << shape->id << std::endl;
        }
        
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        threeD *shape = getObjectByID(shape_id);
        
        
        std::cout << "About which edge would you like to rotate the shape?" << std::endl;
        std::cout << "Enter the coordinate of the first point of the rotation axis separated by spaces: " << std::endl;
        std::cin >> p1->x >> p1->y >> p1->z;
        std::cout << "Enter the coordinate of the second point of the rotation axis separated by spaces: " << std::endl;
        std::cin >> p2->x >> p2->y >> p2->z;
        std::cout << "Enter the angle you wish to rotate the shape by: " << std::endl;
        std::cout << "angle = ";
        std::cin >> angle;
        
        // create rotation axis
        Edge *edge = new Edge();
        edge->p1 = p1;
        edge->p2 = p2;
        Line  *line = new Line(edge);
        line->color = 0.5;
        
        // carry out rotate operation
        shape->rotate(edge, angle);
    }
    else if (answer == "4"){
        std::ofstream outputFile("file.txt", std::ios::out);

        // exit program if unable to create file
        if(!outputFile){
            std::cerr << "File could not be opened" << std::endl;
        }
        
        
        for(auto shape : all_shapes){
            outputFile << shape->vertices.size() << std::endl;
            for(auto point : shape->vertices){
                outputFile << point->x << " " << point->y << " " << point->z << std::endl;
            }
            outputFile << shape->edges.size() << std::endl;
            for(auto edge: shape->edges){
                outputFile << findEdges(shape, edge->p1) << " " << findEdges(shape, edge->p2) << std::endl;
                
            }
            outputFile << std::endl;
        }

    }
    else if (answer == "5"){
        exit(0);
    }
    else if (answer == "6"){
        return;
    }
    else {
        std::cout << "Invalid answer. Please try again" << std::endl << std::endl;
        interface();
    }

}

// main display loop, this function will be called again and again by OpenGL
void displayXY(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearXYPixels();
    
    for(auto i : all_shapes){
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
    
    if(display_count){
        interface();
    }
    
    for (auto i: all_shapes){
        i->drawXZ();
    }

    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXZ);
    
    display_count = 1;
    
    glFlush();
    glutPostRedisplay();
}

void display(){
    cout << "MAIN DISPLAY" << endl;
    glutSetWindow(MainWindow);
    glClear(GL_COLOR_BUFFER_BIT);
    
}

int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;
    
    int numPoints, numEdges;
    std::string chars;
    
    std::ifstream inputFile("file.txt", std::ios::in);
    
    // exit program if unable to create file
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    std::vector<Point *>points;
    std::vector<Edge *>edges;
    
    // read in file
    while(!inputFile.eof()){
        
        getline(inputFile, chars);
        numPoints = atoi(chars.c_str());
        
        points.clear();
        edges.clear();
        
        if(chars == ""){
            break;
        }
        
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
        
        // get edges
        getline(inputFile, chars);
        numEdges = atoi(chars.c_str());
        
        for(int i = 0; i < numEdges; i++){
            getline(inputFile, chars);
            std::stringstream iss;
            iss.str(chars);
            std::string point1, point2;
            iss >> point1 >> point2;
            int p1 = atoi(point1.c_str());
            int p2 = atoi(point2.c_str());
            Edge *newEdge = new Edge();
            newEdge->p1 = &(*newShape->vertices.at(p1));
            newEdge->p2 = &(*newShape->vertices.at(p2));
            edges.push_back(newEdge);
            
        }
        newShape->edges = edges;
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


