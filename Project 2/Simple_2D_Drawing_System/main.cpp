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
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// global variables
int display_count = 0;
int pid = 0;
int window_width, window_height;
int PixelBufferSize;
float *PixelBufferXY, *PixelBufferYZ, *PixelBufferXZ;
float xmin, xmax, ymin, ymax;
int winXY, winYZ, winXZ, MainWindow;

// global variable to hold all polygons
std::vector<threeD*>all_shapes;


// function prototypes;
void displayXY();
void displayYZ();
void displayXZ();

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
void setPixelYZ(int y, int z, double c){
    int i = (y * window_width/2 + z) * 3;
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
    for(int i = 0; i < window_width/2; i++){
        for(int j = 0; j < window_height/2; j++){
            setPixelXY(i, j, 0.0);
        }
    }
    
    for(int i = 0; i < window_width/2; i++){
        for(int j = 0; j < window_height/2; j++){
            setPixelYZ(i, j, 0.0);
        }
    }
    
    for(int i = 0; i < window_width/2; i++){
        for(int j = 0; j < window_height/2; j++){
            setPixelXZ(i, j, 0.0);
        }
    }
}

//void interface(){
//    std::string answer;
//    std::vector<std::array<float, 2>>points;
//
//    std::cout << std::endl << "What would you like to do?" << std::endl <<
//    "\t(1) Draw new line" << std::endl <<
//    "\t(2) Draw new polygon" << std::endl <<
//    "\t(3) Clip" << std::endl <<
//    "\t(4) Translate" << std::endl <<
//    "\t(5) Scale" << std::endl <<
//    "\t(6) Rotate" << std::endl <<
//    "\t(7) Save to file" << std::endl <<
//    "\t(8) Exit" << std::endl;
//
//    std::cout << ">> ";
//    std::cin >> answer;
//
//    if (answer == "1"){
//        std::string algorithm;
//        float x1, y1, x2, y2;
//        std::cout << "Coordinates for first point: " << std::endl;
//        std::cin >> x1 >> y1;
//        std::cout << "Coordinates for second point: " << std::endl;
//        std::cin >> x2 >> y2;
//        Line *line = new Line(x1, y1, x2, y2);
//        all_shapes.push_back(line);
//
//
//        std::cout << std::endl << "How would you like to draw the line?" << std::endl <<
//        "\t(a) Using DDA algorithm " << std::endl <<
//        "\t(b) Using Bresenham's algorithm " << std::endl;
//        std::cout << ">> ";
//        std::cin >> algorithm;
//
//        std::cout << "Line created with id " << line->id << std::endl << std::endl;
//
//        if(algorithm == "a"){
//            line->lineDDA();
//        }
//        else if (algorithm == "b"){
//            line->lineBres();
//        }
//    }
//    else if (answer == "2"){
//
//        std::string val1, val2;
//        int numPoints;
//        points.clear();
//        std::cout << "How many vertices will this polygon have? " << std::endl << "points = ";
//        std::cin >> numPoints;
//
//        for(int i = 0; i < numPoints; i++){
//            std::cout << "Enter coordinates for vertex " << i + 1 << ": " << std::endl;
//            std::cin >> val1 >> val2;
//            float x = std::stof(val1);
//            float y = std::stof(val2);
//            std::array<float, 2>coordinates = {x, y};
//            points.push_back(coordinates);
//        }
//
//        if(!points.empty()){
//            Polygon *polygon = new Polygon(points);
//            all_shapes.push_back(polygon);
//            polygon->drawPolygon();
//            std::cout << "Polygon created with id " << polygon->id << std::endl << std::endl;
//        }
//
//
//    }
//    else if (answer == "3"){
//        std::cout << "xmin = ";
//        std::cin >> xmin;
//        std::cout << "xmax = ";
//        std::cin >> xmax;
//        std::cout << "ymin = ";
//        std::cin >> ymin;
//        std::cout << "ymax = ";
//        std::cin >> ymax;
//
//        std::vector<int>delete_index;
//        for(auto i : all_shapes){
//            int exists = i->clip();
//            if (exists <= 0){
//                int pos = getPosByID(i);
//                if(pos >= 0){
//                    delete_index.push_back(pos);
//
//                }
//            }
//        }
//
//        // delete all shapes whose vertices are outside clipping window
//        for (auto i : delete_index){
//            all_shapes[i] = all_shapes.back();
//            all_shapes.pop_back();
//        }
//
//    }
//
//    else if (answer == "4"){
//        int shape_id;
//        int x, y;
//        std::vector<int>polygons;
//
//        std::cout << "Enter the id of the polygon you wish to translate: " << std::endl;
//        polygons = findPolygons();
//        for(auto poly_id : polygons){
//            std::cout << poly_id << std::endl;
//        }
//        std::cout << ">> ";
//        std::cin >> shape_id;
//
//        // get the object the user specified
//        Geometry *shape = getObjectByID(shape_id);
//
//        // get transformation parameters
//        std::cout << "How would you like to translate the Polygon?" << std::endl;
//        std::cout << "x = ";
//        std::cin >> x;
//        std::cout << "y = ";
//        std::cin >> y;
//
//        // carry out transformation operations
//        shape->convertToMatrix();
//        shape->translate(x, y);
//        shape->convertToVector();
//        shape->updateParameters();
//
//    }
//    else if (answer == "5"){
//        int shape_id;
//        double alpha, beta;
//        std::vector<int>polygons;
//        std::cout << "Enter the id of the polygon you wish to scale: " << std::endl;
//        polygons = findPolygons();
//        for(auto poly_id : polygons){
//            std::cout << poly_id << std::endl;
//        }
//        std::cout << ">> ";
//        std::cin >> shape_id;
//
//        // get the object the user specified
//        Geometry *shape = getObjectByID(shape_id);
//
//        // get transformation parameters
//        std::cout << "How would you like to scale the Polygon? " << std::endl;
//        std::cout << "alpha = ";
//        std::cin >> alpha;
//        std::cout << "beta = ";
//        std::cin >> beta;
//
//        // carry out transformation operations
//        shape->convertToMatrix();
//        shape->scale(alpha, beta);
//        shape->convertToVector();
//        shape->updateParameters();
//
//    }
//    else if (answer == "6"){
//        int shape_id;
//        double angle;
//        std::vector<int>polygons;
//        std::cout << "Enter the id of the polygon you wish to rotate: " << std::endl;
//        polygons = findPolygons();
//        for(auto poly_id : polygons){
//            std::cout << poly_id << std::endl;
//        }
//        std::cout << ">> ";
//        std::cin >> shape_id;
//
//        // get the object the user specified
//        Geometry *shape = getObjectByID(shape_id);
//
//        std::cout << "How would you like to rotate the Polygon? " << std::endl;
//        std::cout << "angle = ";
//        std::cin >> angle;
//
//
//        // carry out transformation operations
//        shape->convertToMatrix();
//        shape->rotate(angle);
//        shape->convertToVector();
//        shape->updateParameters();
//    }
//    else if (answer == "7"){
//        std::ofstream outputFile("file.txt", std::ios::out);
//
//        // exit program if unable to create file
//        if(!outputFile){
//            std::cerr << "File could not be opened" << std::endl;
//        }
//
//        outputFile << all_shapes.size() << std::endl << std::endl;
//
//        for(auto shape : all_shapes){
//            outputFile << shape->n << std::endl;
//            for(auto vertex : shape->vertices){
//                outputFile << vertex->x << " " << vertex->y << std::endl;
//            }
//            outputFile << std::endl;
//        }
//    }
//    else if (answer == "8"){
//        exit(0);
//    }
//    else {
//        std::cout << "Invalid answer. Please try again" << std::endl << std::endl;
//        interface();
//    }
//
//}

// main display loop, this function will be called again and again by OpenGL
void displayXY(){
    string answer;
    cout << "displayXY" << endl;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glutSetWindow(winXY);
    
    for(auto i : all_shapes){
        i->drawXY();
    }
    
    
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXY);
    
    //window refresh
    glFlush();
    
}

// main display loop, this function will be called again and again by OpenGL
void displayYZ(){
    string answer;
    cout << "displayYZ" << endl;
    glutSetWindow(winYZ);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    for(auto i : all_shapes){
        i->drawYZ();
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferYZ);
    
    glFlush();
}

// main display loop, this function will be called again and again by OpenGL
void displayXZ(){
    string answer;
    cout << "displayXZ" << endl;
    glutSetWindow(winXZ);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    for (auto i: all_shapes){
        i->drawXZ();
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXZ);
    
    glFlush();
}

void display(){
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
            newEdge->p1 = points.at(p1);
            newEdge->p2 = points.at(p2);
            edges.push_back(newEdge);
            
        }
        
        threeD *newShape = new threeD(edges, points);
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


