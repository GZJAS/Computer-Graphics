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
#include <unistd.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// global variables
int window_width, window_height;
int PixelBufferSize;
float *PixelBuffer;
int min, max;
int global_id;
bool SET_CONTROL_POINTS = false, SET_ONE = false, BEZIER = false;
Point GlobalPt;


int buffer = 50;
std::vector<Point>newCurve;
int resolution = 6;

// global variable to hold all polygons
std::vector<Polygon *>all_curves;


void display();

float findMin(){
    std::vector<float> min;
    
    
    // go through each shape and find min x, y, z
    for (auto curve: all_curves){
        std::vector<Point >::iterator xmin = std::min_element(curve->control_pts.begin(), curve->control_pts.end(), [](const Point p1, const Point p2){return p1.x < p2.x;});
        std::vector<Point >::iterator ymin = std::min_element(curve->control_pts.begin(), curve->control_pts.end(), [](const Point p1, const Point p2){return p1.y < p2.y;});
        min.push_back((*xmin).x);
        min.push_back((*ymin).y);
    }
    
    // find the min out of all x, y, z
    std::vector<float>::iterator min_itr = std::min_element(min.begin(), min.end(), [](float &x1, float &x2){return x1 < x2;});

    return *min_itr;
}

float findMax(){
    std::vector<float> max;
    
    
    // go through each shape and find max x, y, z
    for (auto curve: all_curves){
        std::vector<Point >::iterator xmax = std::max_element(curve->control_pts.begin(), curve->control_pts.end(), [](const Point p1, const Point p2){return p1.x < p2.x;});
        std::vector<Point >::iterator ymax = std::max_element(curve->control_pts.begin(), curve->control_pts.end(), [](const Point p1, const Point p2){return p1.y < p2.y;});
        max.push_back((*xmax).x);
        max.push_back((*ymax).y);

    }
    
    // find the max out of all x, y, z
    std::vector<float>::iterator max_itr = std::max_element(max.begin(), max.end(), [](float &x1, float &x2){return x1 < x2;});
    
    return *max_itr;
}

// convert NDC coordinates to world coordinates
void mapToWorld(float &x, float &y){
    
    x = x * (::max - ::min + 1) / (window_width) + ::min;
    y = y * (::max - ::min + 1) / (window_height) + ::min;
    
}

// convert x,y coordinates to NDC
void maptToNDC(float &x, float &y){
    float xbar, ybar;
    
    // find min and max
    ::min = findMin() - buffer;
    ::max = findMax() + buffer;
    
    // compute the new point
    xbar = (x - ::min)/(::max + 1 - ::min);
    ybar = (y - ::min)/(::max + 1 - ::min);

    // find corresponding pixel
    x = xbar * window_width;
    y = ybar * window_height;
    
}

// generate a random color that is not back -- parameter doesn't matter
Color randomColor(int){
    int r = round(((double) rand()/ RAND_MAX));
    int g = round(((double) rand()/ RAND_MAX));
    int b = round(((double) rand()/ RAND_MAX));
    if(r == 0 && g == 0 && b == 0){
        r = 1.0;
        g = 1.0;
        b = 1.0;
    }
    return Color(r, g, b);
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
    for(int i = 0; i < PixelBufferSize; i++){
        PixelBuffer[i] = 0.0;
    }
}


int findCurveID(){
    int whichcurve;
    std::cout << "Enter id of curve: " << std::endl;
    for(auto curve : all_curves){
        std::cout << "id: " << curve->id << std::endl;
    }
    std::cout << ">> ";
    std::cin >> whichcurve;
    
    return whichcurve;
}

int findPoint(Polygon *poly){
    int whichpoint;
    std::cout << "Enter point of curve: " << std::endl;
    for(int i = 0; i < poly->control_pts.size(); i++){
        std::cout << i << ": (" << poly->control_pts.at(i).x << ", " << poly->control_pts.at(i).y << ")" << std::endl;
    }
    std::cout << ">> ";
    std::cin >> whichpoint;
    
    return whichpoint;
}

std::string beforeOrAfter(){
    std::string answer;
    std::cout << "Would you like to insert before or after the point?" << std::endl <<
    "\t(a) Before" << std::endl <<
    "\t(b) After" << std::endl ;
    
    
    std::cout << ">> ";
    std::cin >> answer;
    
    return answer;
}

bool modifyKnot(Polygon *poly, int i, float f){
    if(poly->knots.at(i+1) >= f && poly->knots.at(i-1) <= f){
        poly->knots.at(i) = f;
        return true;
    }
    return false;
}

bool setNewK(Polygon *poly, int newk){
    if (newk <= poly->control_pts.size() - 1 && newk >= 2){
        return true;
    }
    return false;
}



void interface(){
    std::string answer;
    int whichcurve, whichpoint;
    
    std::cout << std::endl << "What would you like to do?" << std::endl <<
    "\t(1) Change resolution" << std::endl <<
    "\t(2) Draw (Bezier/B-spline) curve" << std::endl <<
    "\t(3) Insert control point" << std::endl <<
    "\t(4) Modify control point" << std::endl <<
    "\t(5) Delete control point" << std::endl <<
    "\t(6) Change order of B-Spline" << std::endl <<
    "\t(7) Modify knot value" << std::endl <<
    "\t(8) Write to file" << std::endl <<
    "\t(9) Exit" << std::endl;

    
    std::cout << ">> ";
    std::cin >> answer;
    
    if(answer == "1"){
    /*********************
     * Change resolution *
     *********************/
        std::cout << "Current resolution: " << resolution << std::endl;
        std::cout << "New resolution: ";
        cin >> resolution;
    }
    else if(answer == "2"){
    /*********************
     * Draw Bezier Curve *
     *********************/
        std::string type;
        std::cout << " Which type of curve would you like to draw?" << std::endl <<
        "\t(a) Bezier" << std::endl <<
        "\t(b) B-spline" << std::endl;
        
        std::cout << ">> ";
        std::cin >> type;
        
        if(answer == "a"){
            BEZIER = true;
        }
        else if(answer == "b"){
            BEZIER = false;
        }
        
        std::cout << " Click on the screen to set points for the new curve. When you are done, press 'b'. press 'i' for interface" << std::endl;
        SET_CONTROL_POINTS = true;
        return;
    }
    else if(answer == "3"){
    /***********************
     * Insert control point *
     ************************/
        std::cout << " Click on the screen to insert a new control point. When you are done, press 'n'. press 'i' for interface" << std::endl;

        SET_ONE = true;
    }
    else if(answer == "4"){
    /***********************
     * Modify control point *
     ************************/
        std::cout << "Click on the screen to set a control point. When you are done press 'm'" << std::endl;
        SET_ONE = true;
        
    }
    else if(answer == "5"){
    /***********************
     * Delete control point *
     ************************/

        do {
            whichcurve = findCurveID();
        } while(whichcurve < 0 || whichcurve > all_curves.size() - 1);
 
        Polygon *poly = getObjectByID(whichcurve);
        
        do {
            whichpoint = findPoint(poly);
        } while(whichpoint < 0 || whichpoint > poly->control_pts.size() - 1);
        
    
        poly->deleteControlPt(whichpoint);

    }
    else if(answer == "6"){
    /******************************
     * Change k value for polygon *
     ******************************/
        int newk;
        whichcurve = findCurveID();
        while(whichcurve < 0 || whichcurve > all_curves.size() - 1){
            std::cout << "Invalid  answer please try again" <<std::endl;
            whichcurve = findCurveID();
        }
        
        Polygon *poly = getObjectByID(whichcurve);
        
        if(poly->type != Spline){
            std::cout << "Curve is not a b-spline curve. " << std::endl;
            interface();
        }
        else {

            std::cout << "Current k value: " << poly->k <<  std::endl;
            std::cout << "Change k value to: ";
            std::cin >> newk;
            
            while(!setNewK(poly, newk)){
                std::cout << "k must be an int between 2 and " << poly->control_pts.size() - 1 << std::endl;
                std::cout << "Change k value to: ";
                std::cin >> newk;
            }
            
            poly->k = newk;
            poly->createKnotVector();
//            poly->setKValue(newk);
        }

    }
    else if(answer == "7"){
    /*********************
     * Change knot value *
     *********************/
        int index;
        whichcurve = findCurveID();
        while(whichcurve < 0 || whichcurve > all_curves.size() - 1){
            std::cout << "Invalid  answer please try again" <<std::endl;
            whichcurve = findCurveID();
        }
        
        Polygon *poly = getObjectByID(whichcurve);
        
        if(poly->type != Spline){
            std::cout << "Curve is not a b-spline curve. " << std::endl;
            interface();
        }
        else {
            // display knots values
            std::cout << "Choose index of which knot you would like to modify" << std::endl;
            for(int i = 0; i < poly->knots.size(); i++){
                std::cout << i << ": " << poly->knots.at(i)<< std::endl;
            }
            
            std::cout << ">> ";
            std::cin >> index;
            while(index < 0 || index > poly->knots.size()){
                std::cout << "Invalid index. Please try again. " << std::endl;
                std::cin >> index;
            }
            
            float newValue;
            std::cout << "New value: ";
            std::cin >> newValue;
            
            bool check = modifyKnot(poly, index, newValue);
            while(!check){
                std::cout << "New knot value at index " << index <<  " must be between " << poly->knots.at(index-1) << " and " << poly->knots.at(index+1) << std::endl;
                std::cout << "New value: ";
                std::cin >> newValue;
                check = modifyKnot(poly, index, newValue);
            }
            
            poly->modifyKnot(index, newValue);
        }
        
        
    }
    else if(answer == "8"){
    /*****************
     * Write to File *
     *****************/
        std::ofstream outputFile("outfile.txt", std::ios::out);
        
        // exit program if unable to create file
        if(!outputFile){
            std::cerr << "File could not be opened" << std::endl;
        }
        
        for(auto curve: all_curves){
            if(curve->type == Bezier){
                outputFile << "b" << std::endl;
            }
            
            else if(curve->type == Spline){
                outputFile << "s" << std::endl;
                outputFile << curve->k << std::endl;
                for(auto knot : curve->knots){
                    outputFile << knot << " ";
                }
                outputFile << std::endl;

            }
            
            for (auto pt : curve->control_pts){
                outputFile<< pt.x << " " << pt.y << std::endl;
            }
            
            outputFile << std::endl;
        }
        
    }
    
    else if(answer == "9"){
        exit(0);
    }
    else {
        cout << "Invalid answer. Please try again" << endl;
        interface();
    }
}

// main display loop, this function will be called again and again by OpenGL
void display(){

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearAllPixels();
    
    
    // draw curves
    for(int i = 0; i < all_curves.size(); i++){

        // map control points to NDC
        for(int j = 0; j < all_curves[i]->control_pts.size(); j++){
            maptToNDC(all_curves[i]->control_pts[j].x, all_curves[i]->control_pts[j].y);
        }
        
        if(all_curves[i]->type == Bezier){
            all_curves[i]->deCasteljau();
        }
        else if(all_curves[i]->type == Spline){
            all_curves[i]->deBoor();
        }
        
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
    glutPostRedisplay();
    
}

void keyboard(unsigned char key, int, int){
    switch(key){

        case 'i':{
        /*************
         * Interface *
         *************/
            interface();
            break;
        }

        case 'n':
        {
        /************************
         * Insert control point *
         ************************/
            std::string answer;
            int whichcurve, whichpoint;
            
            
            whichcurve = findCurveID();
            while(whichcurve < 0 || whichcurve > all_curves.size() - 1){
                std::cout << "Invalid  answer please try again" <<std::endl;
                whichcurve = findCurveID();
            }

            Polygon *poly = getObjectByID(whichcurve);
            
            whichpoint = findPoint(poly);
            while(whichpoint < 0 || whichpoint > poly->control_pts.size() - 1){
                std::cout << "Invalid  answer please try again" <<std::endl;
                whichpoint = findPoint(poly);
            }
            
            
            answer = beforeOrAfter();
            while(answer != "a" && answer != "b"){
                std::cout << "Invalid  answer please try again" <<std::endl;
                answer = beforeOrAfter();
            }
            
            if(answer == "a"){
                poly->insertControlPt(whichpoint, GlobalPt);
            }
            
            else if(answer == "b"){
                poly->insertControlPt(whichpoint+1, GlobalPt);
            }

            break;
        }
            

        case 'm':
        {
        /***********************
         * Modify control point *
         ************************/
            
            std::string answer;
            int whichcurve, whichpoint;
            
            
            
            whichcurve = findCurveID();
            while(whichcurve < 0 || whichcurve > all_curves.size() - 1){
                std::cout << "Invalid  answer please try again" <<std::endl;
                whichcurve = findCurveID();
            }
            
            Polygon *poly = getObjectByID(whichcurve);
            
            whichpoint = findPoint(poly);
            while(whichpoint < 0 || whichpoint > poly->control_pts.size() - 1){
                std::cout << "Invalid  answer please try again" <<std::endl;
                whichpoint = findPoint(poly);
            }
            
            poly->modifyControlPt(whichpoint, GlobalPt);
            break;
            
        }

        case 'b':
        {
        /*************
         * New Curve *
         *************/
            if(SET_CONTROL_POINTS){
                SET_CONTROL_POINTS = false;
                if(BEZIER == true){
                    Polygon *bezier = new Polygon(newCurve);
                    bezier->color = randomColor(1);
                    bezier->type = Bezier;
                    all_curves.push_back(bezier);
                    newCurve.clear();
                    
                }
                else {
                    Polygon *bspline = new Polygon(newCurve);
                    bspline->type = Spline;
                    bspline->color = randomColor(1);
                    bspline->k = 2;
                    bspline->createKnotVector();
                    all_curves.push_back(bspline);
                    newCurve.clear();
                    
                }
            }
            break;
        }
            
    }
}

void mouse_click(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if(SET_CONTROL_POINTS){
            std::cout << "\t(" << x << ", " << window_height - y << ")" << std::endl;
            Point pt = Point((float)x, window_height - (float)y);
            newCurve.push_back(pt);
        }
        else if(SET_ONE){
            std::cout << "\t(" << x << ", " << window_height - y << ")" << std::endl;
            Point pt = Point((float)x, window_height - (float)y);
            GlobalPt = pt;
            SET_ONE = false;
        }

    }
        
    glutPostRedisplay();
}

int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;

    
    std::string chars;
    
    std::ifstream inputFile("file.txt", std::ios::in);
    
    // exit program if unable to create file
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    // read in file
    while(!inputFile.eof()){
        getline(inputFile, chars);
        
        // bezier curves
        if(chars == "b"){
        
            std::vector<Point>control_points;
            while(chars != ""){
                getline(inputFile, chars);
                
                if(chars == "")
                    break;
                
                std::stringstream iss;
                iss.str(chars);
                float x, y;
                iss >> x >> y;
                Point pt(x, y);
                control_points.push_back(pt);

            }
            Polygon *poly = new Polygon(control_points);
            poly->color = randomColor(1);
            poly->type = Bezier;
            all_curves.push_back(poly);
            
        }
        
        // b-spline curves
        else if (chars == "s"){
            std::vector<Point>control_points;
            std::vector<float>knots;
            
            getline(inputFile, chars);
            
            // k
            int order = std::stoi(chars);
            
            getline(inputFile, chars);
            
            // knot vector
            string knot;
            std::stringstream iss(chars);
            while(iss >> knot){
                knots.push_back(std::stof(knot));
            }
            

            // control points
            while(chars != ""){
                getline(inputFile, chars);
                
                if(chars == "")
                    break;
                
                float x, y;
                iss.clear();
                iss.str(chars);
                iss >> x >> y;
                Point pt(x, y);
                control_points.push_back(pt);
                
            }
            Polygon *poly = new Polygon(control_points);
            poly->k = order;
            poly->knots = knots;
            poly->color = randomColor(1);
            poly->type = Spline;
            all_curves.push_back(poly);
        }
        
        
    }

    
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


