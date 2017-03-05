//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//
#include "Line.hpp"
#include "Polygon.hpp"
#include "Edge.hpp"
#include <iostream>
#include <iterator>
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
float *PixelBuffer;
float xmin, xmax, ymin, ymax;
int global_id;
bool SET_CONTROL_POINTS = false;

std::vector<Point>newBezierCurve;

//for testing purposes
std::vector<Point>controlpoints = {Point(100, 200), Point(200,300), Point(400,300), Point(500,200)};
std::vector<Point>controlpoints2 = {Point(99, 427), Point(201, 535), Point(352, 516), Point(436, 449), Point(398, 386), Point(278, 386)};
int resolution = 100;

// global variable to hold all polygons
std::vector<Polygon *>all_curves;


float findMin(){
    std::vector<float> min;
    
    
    // go through each shape and find min x, y, z
    for (auto shape: all_curves){
        std::vector<Point *>::iterator xmin = std::min_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->x < p2->x;});
        std::vector<Point *>::iterator ymin = std::min_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->y < p2->y;});
        min.push_back((*xmin)->x);
        min.push_back((*ymin)->y);
    }
    
    // find the min out of all x, y, z
    std::vector<float>::iterator min_itr = std::min_element(min.begin(), min.end(), [](float &x1, float &x2){return x1 < x2;});

    return *min_itr;
}

float findMax(){
    std::vector<float> max;
    
    
    // go through each shape and find max x, y, z
    for (auto shape: all_curves){
        std::vector<Point *>::iterator xmax = std::max_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->x < p2->x;});
        std::vector<Point *>::iterator ymax = std::max_element(shape->vertices.begin(), shape->vertices.end(), [](Point *p1, Point *p2){return p1->y < p2->y;});
        max.push_back((*xmax)->x);
        max.push_back((*ymax)->y);

    }
    
    // find the max out of all x, y, z
    std::vector<float>::iterator max_itr = std::max_element(max.begin(), max.end(), [](float &x1, float &x2){return x1 < x2;});
    
    return *max_itr;
}



// convert x,y coordinates to NDC for xy plane
void maptToNDC(float &x, float &y){
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


Polygon *getObjectByID(int id){
    for(auto i: all_curves){
        if (i->id == id){
            return i;
        }
    }
    return nullptr;
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixel(int x, int y, Color c){
    int i = (y * window_width + x) * 3;
    PixelBuffer[i] = c.r;
    PixelBuffer[i+1] = c.g;
    PixelBuffer[i+2] = c.b;
    return;
    
}


// clears all of the pixels from the screen
void clearAllPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBuffer[i] = 0.0;
    }
}

void interface(){
    std::string answer;
    std::cout << std::endl << "What would you like to do?" << std::endl <<
    "\t(1) Draw Bezier Curve" << std::endl <<
    "\t(2) Insert Bezier point" << std::endl <<
    "\t(3) Modify Bexier point" << std::endl <<
    "\t(4) Delete Bexier point" << std::endl;
    
    std::cout << ">> ";
    std::cin >> answer;
    
    if(answer == "1"){
        std::cout << "Clicks will now be tracked. Click on the screen to set Bezier points for the new curve. When you are done, press 'm' to complete the curve" << std::endl;
        SET_CONTROL_POINTS = true;
        return;
    }
    else if(answer == "2"){
        
    }
    else if(answer == "3"){
        
    }
    else if(answer == "4"){
        
    }
    else {
        cout << "Invalid answer. Please try again" << endl;
        interface();
    }
}

// main display loop, this function will be called again and again by OpenGL
void display(){
    
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearAllPixels();
    
    if(!display_count) {
        Polygon *poly = new Polygon(controlpoints);
        poly->color = Color(0,1,0);
        all_curves.push_back(poly);
        
        Polygon *poly2 = new Polygon(controlpoints2);
        poly2->color = Color(1,0,0);
        all_curves.push_back(poly2);
    }
    
    for(auto curve : all_curves){
        curve->deCasteljau();
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    display_count = 1;
    
    //window refresh
    glFlush();
    glutPostRedisplay();
    
}

void keyboard(unsigned char key, int, int){
    switch(key){
        case 'i':
            interface();
            break;
        case 'm':
            SET_CONTROL_POINTS = false;
            Polygon *poly = new Polygon(newBezierCurve);
            poly->color = Color(1,0,0);
            all_curves.push_back(poly);
            newBezierCurve.clear();
            break;
    }
}

void mouse_click(int button, int state, int x, int y)
{
    if(SET_CONTROL_POINTS){
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            std::cout << "\t(" << x << ", " << y << ")" << std::endl;
            Point pt = Point((float)x, window_height - (float)y);
            newBezierCurve.push_back(pt);
        }

    }
        
    glutPostRedisplay();
}

int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;

    
//    std::string chars;
//    
//    std::ifstream inputFile("file.txt", std::ios::in);
//    
//    // exit program if unable to create file
//    if(!inputFile){
//        std::cerr << "File could not be opened" << std::endl;
//    }
//    
//    
//    // read in file
    
    
    PixelBuffer = new float[PixelBufferSize];
    
 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello Graphics!!");
    glutDisplayFunc(display);

    
    // mouse event handlers
    glutMouseFunc(mouse_click);
    
    
    // handler for keyboard input
    glutKeyboardFunc(keyboard);
    
    
    glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
    glutMainLoop();//main display loop, will display until terminate
    return 0;}


